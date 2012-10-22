/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "anope.h"
#include "dns.h"
#include "sockets.h"
#include "socketengine.h"

#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

DNSManager *DNSEngine = NULL;

Question::Question()
{
	this->type = DNS_QUERY_NONE;
	this->qclass = 0;
}

Question::Question(const Anope::string &n, QueryType t, unsigned short q) : name(n), type(t), qclass(q)
{
}

ResourceRecord::ResourceRecord(const Anope::string &n, QueryType t, unsigned short q) : Question(n, t, q)
{
	this->ttl = 0;
	this->created = Anope::CurTime;
}

ResourceRecord::ResourceRecord(const Question &q) : Question(q)
{
	this->ttl = 0;
	this->created = Anope::CurTime;
}

DNSQuery::DNSQuery()
{
	this->error = DNS_ERROR_NONE;
}

DNSQuery::DNSQuery(const Question &q)
{
	this->questions.push_back(q);
	this->error = DNS_ERROR_NONE;
}

DNSRequest::DNSRequest(const Anope::string &addr, QueryType qt, bool cache, Module *c) : Timer(Config->DNSTimeout), Question(addr, qt), use_cache(cache), id(0), creator(c)
{
	if (!DNSEngine)
		throw SocketException("No DNSEngine");
	if (DNSEngine->GetPackets().size() == 65535)
		throw SocketException("DNS queue full");

	do
	{
		static unsigned short cur_id = rand();
		this->id = cur_id++;
	}
	while (DNSEngine->requests.count(this->id));

	DNSEngine->requests[this->id] = this;
}

DNSRequest::~DNSRequest()
{
	DNSEngine->requests.erase(this->id);
}

void DNSRequest::Process()
{
	Log(LOG_DEBUG_2) << "Resolver: Processing request to lookup " << this->name << ", of type " << this->type;

	if (!DNSEngine)
		throw SocketException("DNSEngine has not been initialized");

	if (this->use_cache && DNSEngine->CheckCache(this))
	{
		Log(LOG_DEBUG_2) << "Resolver: Using cached result";
		delete this;
		return;
	}
	
	DNSPacket *p = new DNSPacket(DNSEngine->addrs);
	p->flags = DNS_QUERYFLAGS_RD;

	p->id = this->id;
	p->questions.push_back(*this);
	DNSEngine->SendPacket(p);
}

void DNSRequest::OnError(const DNSQuery *r)
{
}

void DNSRequest::Tick(time_t)
{
	Log(LOG_DEBUG_2) << "Resolver: timeout for query " << this->name;
	DNSQuery rr(*this);
	rr.error = DNS_ERROR_TIMEOUT;
	this->OnError(&rr);
} 

void DNSPacket::PackName(unsigned char *output, unsigned short output_size, unsigned short &pos, const Anope::string &name)
{
	if (name.length() + 2 > output_size)
		throw SocketException("Unable to pack name");

	Log(LOG_DEBUG_2) << "Resolver: PackName packing " << name;

	sepstream sep(name, '.');
	Anope::string token;

	while (sep.GetToken(token))
	{
		output[pos++] = token.length();
		memcpy(&output[pos], token.c_str(), token.length());
		pos += token.length();
	}

	output[pos++] = 0;
}

Anope::string DNSPacket::UnpackName(const unsigned char *input, unsigned short input_size, unsigned short &pos)
{
	Anope::string name;
	unsigned short pos_ptr = pos, lowest_ptr = input_size;
	bool compressed = false;

	if (pos_ptr >= input_size)
		throw SocketException("Unable to unpack name - no input");

	while (input[pos_ptr] > 0)
	{
		unsigned short offset = input[pos_ptr];

		if (offset & DNS_POINTER)
		{
			if ((offset & DNS_POINTER) != DNS_POINTER)
				throw SocketException("Unable to unpack name - bogus compression header");
			if (pos_ptr + 1 >= input_size)
				throw SocketException("Unable to unpack name - bogus compression header");

			/* Place pos at the second byte of the first (farthest) compression pointer */
			if (compressed == false)
			{
				++pos;
				compressed = true;
			}

			pos_ptr = (offset & DNS_LABEL) << 8 | input[pos_ptr + 1];

			/* Pointers can only go back */
			if (pos_ptr >= lowest_ptr)
				throw SocketException("Unable to unpack name - bogus compression pointer");
			lowest_ptr = pos_ptr;
		}
		else
		{
			if (pos_ptr + offset + 1 >= input_size)
				throw SocketException("Unable to unpack name - offset too large");
			if (!name.empty())
				name += ".";
			for (unsigned i = 1; i <= offset; ++i)
				name += input[pos_ptr + i];

			pos_ptr += offset + 1;
			if (compressed == false)
				/* Move up pos */
				pos = pos_ptr;
		}
	}

	/* +1 pos either to one byte after the compression pointer or one byte after the ending \0 */
	++pos;

	Log(LOG_DEBUG_2) << "Resolver: UnpackName successfully unpacked " << name;

	return name;
}

Question DNSPacket::UnpackQuestion(const unsigned char *input, unsigned short input_size, unsigned short &pos)
{
	Question question;

	question.name = this->UnpackName(input, input_size, pos);

	if (pos + 4 > input_size)
		throw SocketException("Unable to unpack question");

	question.type = static_cast<QueryType>(input[pos] << 8 | input[pos + 1]);
	pos += 2;

	question.qclass = input[pos] << 8 | input[pos + 1];
	pos += 2;

	return question;
}

ResourceRecord DNSPacket::UnpackResourceRecord(const unsigned char *input, unsigned short input_size, unsigned short &pos)
{
	ResourceRecord record = static_cast<ResourceRecord>(this->UnpackQuestion(input, input_size, pos));

	if (pos + 6 > input_size)
		throw SocketException("Unable to unpack resource record");

	record.ttl = (input[pos] << 24) | (input[pos + 1] << 16) | (input[pos + 2] << 8) | input[pos + 3];
	pos += 4;

	//record.rdlength = input[pos] << 8 | input[pos + 1];
	pos += 2;

	switch (record.type)
	{
		case DNS_QUERY_A:
		{
			if (pos + 4 > input_size)
				throw SocketException("Unable to unpack resource record");

			in_addr a;
			a.s_addr = input[pos] | (input[pos + 1] << 8) | (input[pos + 2] << 16)  | (input[pos + 3] << 24);
			pos += 4;

			sockaddrs addrs;
			addrs.ntop(AF_INET, &a);

			record.rdata = addrs.addr();
			break;
		}
		case DNS_QUERY_AAAA:
		{
			if (pos + 16 > input_size)
				throw SocketException("Unable to unpack resource record");

			in6_addr a;
			for (int j = 0; j < 16; ++j)
				a.s6_addr[j] = input[pos + j];
			pos += 16;

			sockaddrs addrs;
			addrs.ntop(AF_INET6, &a);

			record.rdata = addrs.addr();
			break;
		}
		case DNS_QUERY_CNAME:
		case DNS_QUERY_PTR:
		{
			record.rdata = this->UnpackName(input, input_size, pos);
			break;
		}
		default:
			break;
	}

	Log(LOG_DEBUG_2) << "Resolver: " << record.name << " -> " << record.rdata;

	return record;
}

DNSPacket::DNSPacket(const sockaddrs &a) : DNSQuery(), addr(a), id(0), flags(0)
{
}

void DNSPacket::Fill(const unsigned char *input, const unsigned short len)
{
	if (len < DNSPacket::HEADER_LENGTH)
		throw SocketException("Unable to fill packet");

	unsigned short packet_pos = 0;

	this->id = (input[packet_pos] << 8) | input[packet_pos + 1];
	packet_pos += 2;

	this->flags = (input[packet_pos] << 8) | input[packet_pos + 1];
	packet_pos += 2;

	unsigned short qdcount = (input[packet_pos] << 8) | input[packet_pos + 1];
	packet_pos += 2;

	unsigned short ancount = (input[packet_pos] << 8) | input[packet_pos + 1];
	packet_pos += 2;

	unsigned short nscount = (input[packet_pos] << 8) | input[packet_pos + 1];
	packet_pos += 2;

	unsigned short arcount = (input[packet_pos] << 8) | input[packet_pos + 1];
	packet_pos += 2;

	Log(LOG_DEBUG_2) << "Resolver: qdcount: " << qdcount << " ancount: " << ancount << " nscount: " << nscount << " arcount: " << arcount;

	for (unsigned i = 0; i < qdcount; ++i)
		this->questions.push_back(this->UnpackQuestion(input, len, packet_pos));
	
	for (unsigned i = 0; i < ancount; ++i)
		this->answers.push_back(this->UnpackResourceRecord(input, len, packet_pos));

	for (unsigned i = 0; i < nscount; ++i)
		this->authorities.push_back(this->UnpackResourceRecord(input, len, packet_pos));

	for (unsigned i = 0; i < arcount; ++i)
		this->additional.push_back(this->UnpackResourceRecord(input, len, packet_pos));
}

unsigned short DNSPacket::Pack(unsigned char *output, unsigned short output_size)
{
	if (output_size < DNSPacket::HEADER_LENGTH)
		throw SocketException("Unable to pack packet");
	
	unsigned short pos = 0;

	output[pos++] = this->id >> 8;
	output[pos++] = this->id & 0xFF;
	output[pos++] = this->flags >> 8;
	output[pos++] = this->flags & 0xFF;
	output[pos++] = this->questions.size() >> 8;
	output[pos++] = this->questions.size() & 0xFF;
	output[pos++] = this->answers.size() >> 8;
	output[pos++] = this->answers.size() & 0xFF;
	output[pos++] = this->authorities.size() >> 8;
	output[pos++] = this->authorities.size() & 0xFF;
	output[pos++] = this->additional.size() >> 8;
	output[pos++] = this->additional.size() & 0xFF;

	for (unsigned i = 0; i < this->questions.size(); ++i)
	{
		Question &q = this->questions[i];

		if (q.type == DNS_QUERY_PTR)
		{
			sockaddrs ip(q.name);

			if (q.name.find(':') != Anope::string::npos)
			{
				static const char *const hex = "0123456789abcdef";
				char reverse_ip[128];
				unsigned reverse_ip_count = 0;
				for (int j = 15; j >= 0; --j)
				{
					reverse_ip[reverse_ip_count++] = hex[ip.sa6.sin6_addr.s6_addr[j] & 0xF];
					reverse_ip[reverse_ip_count++] = '.';
					reverse_ip[reverse_ip_count++] = hex[ip.sa6.sin6_addr.s6_addr[j] >> 4];
					reverse_ip[reverse_ip_count++] = '.';
				}
				reverse_ip[reverse_ip_count++] = 0;

				q.name = reverse_ip;
				q.name += "ip6.arpa";
			}
			else
			{
				unsigned long forward = ip.sa4.sin_addr.s_addr;
				in_addr reverse;
				reverse.s_addr = forward << 24 | (forward & 0xFF00) << 8 | (forward & 0xFF0000) >> 8 | forward >> 24;

				ip.ntop(AF_INET, &reverse);

				q.name = ip.addr() + ".in-addr.arpa";
			}
		}

		this->PackName(output, output_size, pos, q.name);

		if (pos + 4 >= output_size)
			throw SocketException("Unable to pack packet");

		short s = htons(q.type);
		memcpy(&output[pos], &s, 2);
		pos += 2;

		s = htons(q.qclass);
		memcpy(&output[pos], &s, 2);
		pos += 2;
	}

	std::vector<ResourceRecord> types[] = { this->answers, this->authorities, this->additional };
	for (int i = 0; i < 3; ++i)
		for (unsigned j = 0; j < types[i].size(); ++j)
		{
			ResourceRecord &rr = types[i][j];

			this->PackName(output, output_size, pos, rr.name);

			if (pos + 8 >= output_size)
				throw SocketException("Unable to pack packet");

			short s = htons(rr.type);
			memcpy(&output[pos], &s, 2);
			pos += 2;

			s = htons(rr.qclass);
			memcpy(&output[pos], &s, 2);
			pos += 2;

			long l = htonl(rr.ttl);
			memcpy(&output[pos], &l, 4);
			pos += 4;

			switch (rr.type)
			{
				case DNS_QUERY_A:
				{
					if (pos + 6 > output_size)
						throw SocketException("Unable to pack packet");

					sockaddrs a(rr.rdata);

					s = htons(4);
					memcpy(&output[pos], &s, 2);
					pos += 2;

					memcpy(&output[pos], &a.sa4.sin_addr, 4);
					pos += 4;
					break;
				}
				case DNS_QUERY_AAAA:
				{
					if (pos + 18 > output_size)
						throw SocketException("Unable to pack packet");

					sockaddrs a(rr.rdata);

					s = htons(16);
					memcpy(&output[pos], &s, 2);
					pos += 2;

					memcpy(&output[pos], &a.sa6.sin6_addr, 16);
					pos += 16;
					break;
				}
				case DNS_QUERY_CNAME:
				case DNS_QUERY_PTR:
				{
					if (pos + 2 >= output_size)
						throw SocketException("Unable to pack packet");

					unsigned short packet_pos_save = pos;
					pos += 2;

					this->PackName(output, output_size, pos, rr.rdata);

					i = htons(pos - packet_pos_save - 2);
					memcpy(&output[packet_pos_save], &i, 2);
					break;
				}
				default:
					break;
			}
		}
	
	return pos;
}

DNSManager::DNSManager(const Anope::string &nameserver, const Anope::string &ip, int port) : Timer(300, Anope::CurTime, true), Socket(-1, nameserver.find(':') != Anope::string::npos, SOCK_DGRAM)
{
	this->addrs.pton(this->IPv6 ? AF_INET6 : AF_INET, nameserver, port);
	try
	{
		this->Bind(ip, port);
	}
	catch (const SocketException &ex)
	{
		/* This error can be from normal operation as most people don't use services to handle DNS queries, so put it in debug log */
		Log(LOG_DEBUG) << "Unable to bind DNSManager to port " << port << ": " << ex.GetReason();
	}
}

DNSManager::~DNSManager()
{
	for (unsigned i = this->packets.size(); i > 0; --i)
		delete this->packets[i - 1];
	this->packets.clear();

	for (std::map<unsigned short, DNSRequest *>::iterator it = this->requests.begin(), it_end = this->requests.end(); it != it_end; ++it)
	{	
		DNSRequest *request = it->second;

		DNSQuery rr(*request);
		rr.error = DNS_ERROR_UNKNOWN;
		request->OnError(&rr);

		delete request;
	}
	this->requests.clear();

	this->cache.clear();

	DNSEngine = NULL;
}

bool DNSManager::ProcessRead()
{
	Log(LOG_DEBUG_2) << "Resolver: Reading from DNS socket";

	unsigned char packet_buffer[524];
	sockaddrs from_server;
	socklen_t x = sizeof(from_server);
	int length = recvfrom(this->GetFD(), reinterpret_cast<char *>(&packet_buffer), sizeof(packet_buffer), 0, &from_server.sa, &x);

	if (length < DNSPacket::HEADER_LENGTH)
		return true;

	DNSPacket recv_packet(from_server);

	try
	{
		recv_packet.Fill(packet_buffer, length);
	}
	catch (const SocketException &ex)
	{
		Log(LOG_DEBUG_2) << ex.GetReason();
		return true;
	}

	if (!(recv_packet.flags & DNS_QUERYFLAGS_QR))
	{
		DNSPacket *packet = new DNSPacket(recv_packet);
		packet->flags |= DNS_QUERYFLAGS_QR; /* This is a reponse */

		FOREACH_MOD(I_OnDnsRequest, OnDnsRequest(recv_packet, packet));

		DNSEngine->SendPacket(packet);
		return true;
	}

	if (this->addrs != from_server)
	{
		Log(LOG_DEBUG_2) << "Resolver: Received an answer from the wrong nameserver, Bad NAT or DNS forging attempt? '" << this->addrs.addr() << "' != '" << from_server.addr() << "'";
		return true;
	}

	std::map<unsigned short, DNSRequest *>::iterator it = DNSEngine->requests.find(recv_packet.id);
	if (it == DNSEngine->requests.end())
	{
		Log(LOG_DEBUG_2) << "Resolver: Received an answer for something we didn't request";
		return true;
	}
	DNSRequest *request = it->second;

	if (recv_packet.flags & DNS_QUERYFLAGS_OPCODE)
	{
		Log(LOG_DEBUG_2) << "Resolver: Received a nonstandard query";
		recv_packet.error = DNS_ERROR_NONSTANDARD_QUERY;
		request->OnError(&recv_packet);
	}
	else if (recv_packet.flags & DNS_QUERYFLAGS_RCODE)
	{
		DNSError error = DNS_ERROR_UNKNOWN;

		switch (recv_packet.flags & DNS_QUERYFLAGS_RCODE)
		{
			case 1:
				Log(LOG_DEBUG_2) << "Resolver: format error";
				error = DNS_ERROR_FORMAT_ERROR;
				break;
			case 2:
				Log(LOG_DEBUG_2) << "Resolver: server error";
				error = DNS_ERROR_SERVER_FAILURE;
				break;
			case 3:
				Log(LOG_DEBUG_2) << "Resolver: domain not found";
				error = DNS_ERROR_DOMAIN_NOT_FOUND;
				break;
			case 4:
				Log(LOG_DEBUG_2) << "Resolver: not implemented";
				error = DNS_ERROR_NOT_IMPLEMENTED;
				break;
			case 5:
				Log(LOG_DEBUG_2) << "Resolver: refused";
				error = DNS_ERROR_REFUSED;
				break;
			default:
				break;
		}

		recv_packet.error = error;
		request->OnError(&recv_packet);
	}
	else if (recv_packet.answers.empty())
	{
		Log(LOG_DEBUG_2) << "Resolver: No resource records returned";
		recv_packet.error = DNS_ERROR_NO_RECORDS;
		request->OnError(&recv_packet);
	}
	else
	{
		Log(LOG_DEBUG_2) << "Resolver: Lookup complete for " << request->name;
		request->OnLookupComplete(&recv_packet);
		DNSEngine->AddCache(recv_packet);
	}
	
	delete request;
	return true;
}

bool DNSManager::ProcessWrite()
{
	Log(LOG_DEBUG_2) << "Resolver: Writing to DNS socket";

	DNSPacket *r = !DNSEngine->packets.empty() ? DNSEngine->packets.front() : NULL;
	if (r != NULL)
	{
		try
		{
			unsigned char buffer[524];
			unsigned short len = r->Pack(buffer, sizeof(buffer));

			sendto(this->GetFD(), reinterpret_cast<char *>(buffer), len, 0, &r->addr.sa, r->addr.size());
		}
		catch (const SocketException &) { }

		delete r;
		DNSEngine->packets.pop_front();
	}

	if (DNSEngine->packets.empty())
		SocketEngine::ClearWritable(this);

	return true;
}

void DNSManager::AddCache(DNSQuery &r)
{
	for (unsigned i = 0; i < r.answers.size(); ++i)
	{
		ResourceRecord &rr = r.answers[i];
		Log(LOG_DEBUG_3) << "Resolver cache: added cache for " << rr.name << " -> " << rr.rdata << ", ttl: " << rr.ttl;
		this->cache.insert(std::make_pair(rr.name, rr));
	}
}

bool DNSManager::CheckCache(DNSRequest *request)
{
	cache_map::iterator it = this->cache.find(request->name);
	if (it != this->cache.end())
	{
		DNSQuery record(*request);
		
		for (cache_map::iterator it_end = this->cache.upper_bound(request->name); it != it_end; ++it)
		{
			ResourceRecord &rec = it->second;
			if (rec.created + rec.ttl >= Anope::CurTime)
				record.answers.push_back(rec);
		}

		if (!record.answers.empty())
		{
			Log(LOG_DEBUG_3) << "Resolver: Using cached result for " << request->name;
			request->OnLookupComplete(&record);
			return true;
		}
	}

	return false;
}

void DNSManager::Tick(time_t now)
{
	Log(LOG_DEBUG_2) << "Resolver: Purging DNS cache";

	for (cache_map::iterator it = this->cache.begin(), it_next; it != this->cache.end(); it = it_next)
	{
		ResourceRecord &req = it->second;
		it_next = it;
		++it_next;

		if (req.created + req.ttl < now)
			this->cache.erase(it);
	}
}

void DNSManager::Cleanup(Module *mod)
{
	for (std::map<unsigned short, DNSRequest *>::iterator it = this->requests.begin(), it_end = this->requests.end(); it != it_end;)
	{
		unsigned short id = it->first;
		DNSRequest *req = it->second;
		++it;

		if (req->creator && req->creator == mod)
		{
			DNSQuery rr(*req);
			rr.error = DNS_ERROR_UNLOADED;
			req->OnError(&rr);

			delete req;
			this->requests.erase(id);
		}
	}
}

std::deque<DNSPacket *>& DNSManager::GetPackets()
{
	return this->packets;
}

void DNSManager::SendPacket(DNSPacket *p)
{
	Log(LOG_DEBUG_2) << "Resolver: Queueing packet " << p->id;
	this->packets.push_back(p);

	SocketEngine::MarkWritable(this);
}

DNSQuery DNSManager::BlockingQuery(const Anope::string &mask, QueryType qt)
{
	Question question(mask, qt);
	DNSQuery result(question);

	int type = AF_UNSPEC;
	if (qt == DNS_QUERY_A)
		type = AF_INET;
	else if (qt == DNS_QUERY_AAAA)
		type = AF_INET6;

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = type;

	Log(LOG_DEBUG_2) << "Resolver: BlockingQuery: Looking up " << mask;

	addrinfo *addrresult;
	if (getaddrinfo(mask.c_str(), NULL, &hints, &addrresult) == 0)
	{
		for (addrinfo *cur = addrresult; cur; cur = cur->ai_next)
		{
			sockaddrs addr;
			memcpy(&addr, addrresult->ai_addr, addrresult->ai_addrlen);
			try
			{
				ResourceRecord rr(mask, qt);
				rr.rdata = addr.addr();
				result.answers.push_back(rr);

				Log(LOG_DEBUG_2) << "Resolver: BlockingQuery: " << mask << " -> " << rr.rdata;
			}
			catch (const SocketException &) { }
		}

		freeaddrinfo(addrresult);
	}

	return result;
}



