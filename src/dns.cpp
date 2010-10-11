#include "services.h"

DNSManager *DNSEngine = NULL;

static inline unsigned short GetRandomID()
{
	return random();
}

DNSRequest::DNSRequest(const Anope::string &addr, QueryType qt, bool cache, Module *c) : creator(c), address(addr), QT(qt)
{
	if (!DNSEngine)
		DNSEngine = new DNSManager();
	if (!DNSEngine->sock)
	{
		DNSEngine->sock = new DNSSocket();
		DNSEngine->sock->Connect(Config->NameServer, DNSManager::DNSPort);
	}
	if (DNSEngine->packets.size() == 65535)
		throw SocketException("DNS queue full");
	
	Log(LOG_DEBUG_2) << "Resolver: Processing request to lookup " << addr << ", of type " << qt;

	if (cache && DNSEngine->CheckCache(this))
	{
		delete this;
		return;
	}
	
	DNSPacket *p = new DNSPacket();	
	p->flags = DNS_QUERYFLAGS_RD;

	if (!p->AddQuestion(addr, qt))
	{
		Log() << "Resolver: Unable to lookup host " << addr << " of type " << qt << " - internal error";
		delete p;
		delete this;
		return;
	}

	unsigned short packet_id;
	while (DNSEngine->requests.count((packet_id = GetRandomID())));

	p->id = this->id = packet_id;
	DNSEngine->requests[this->id] = this;
	DNSEngine->packets.push_back(p);

	SocketEngine->MarkWritable(DNSEngine->sock);

	this->timeout = new DNSRequestTimeout(this, Config->DNSTimeout);
}

DNSRequest::~DNSRequest()
{
	/* DNSRequest came back, delete the timeout */
	if (!this->timeout->done)
	{
		delete this->timeout;
	}
	/* Timeout timed us out, delete us from the requests map */
	else
	{
		/* We can leave the packet, if it comes back later we will drop it */
		DNSEngine->requests.erase(this->id);
	}
}

void DNSRequest::OnError(DNSError error, const Anope::string &message)
{
}

inline DNSPacket::DNSPacket()
{
	this->id = this->flags = this->qdcount = this->ancount = this->nscount = this->arcount = this->payload_count = 0;
	memset(&this->payload, 0, sizeof(this->payload));
}

bool DNSPacket::AddQuestion(const Anope::string &name, const QueryType qt)
{
	unsigned char temp_buffer[512] = "";
	unsigned short buffer_pos = 0;

	Anope::string working_name = name;

	switch (qt)
	{
		case DNS_QUERY_PTR:
		{
			if (working_name.find(':') != Anope::string::npos)
			{
				in6_addr ip;
				if (!inet_pton(AF_INET6, working_name.c_str(), &ip))
				{
					Log(LOG_DEBUG_2) << "Resolver: Received an invalid IP for PTR lookup (" << working_name << "): " << Anope::LastError();
					return false;
				}

				static const char *const hex = "0123456789abcdef";
				char reverse_ip[128];
				unsigned reverse_ip_count = 0;
				for (int i = 15; i >= 0; --i)
				{
					reverse_ip[reverse_ip_count++] = hex[ip.s6_addr[i] & 0xF];
					reverse_ip[reverse_ip_count++] = '.';
					reverse_ip[reverse_ip_count++] = hex[ip.s6_addr[i] >> 4];
					reverse_ip[reverse_ip_count++] = '.';
				}
				reverse_ip[reverse_ip_count++] = 0;

				working_name = reverse_ip;
				working_name += "ip6.arpa";
				Log(LOG_DEBUG_2) << "IP changed to in6.arpa format: " << working_name;
			}
			else
			{
				in_addr ip;
				if (!inet_pton(AF_INET, working_name.c_str(), &ip))
				{
					Log(LOG_DEBUG_2) << "Resolver: Received an invalid IP for PTR lookup (" << working_name << "): " << Anope::LastError();
					return false;
				}
				unsigned long reverse_ip = ((ip.s_addr & 0xFF) << 24) | ((ip.s_addr & 0xFF00) << 8) | ((ip.s_addr & 0xFF0000) >> 8) | ((ip.s_addr & 0xFF000000) >> 24);
				char ipbuf[INET_ADDRSTRLEN];
				if (!inet_ntop(AF_INET, &reverse_ip, ipbuf, sizeof(ipbuf)))
				{
					Log(LOG_DEBUG_2) << "Resolver: Reformatted IP " << working_name << " back into an invalid IP: " << Anope::LastError();
					return false;
				}

				working_name = ipbuf;
				working_name += ".in-addr.arpa";
				Log(LOG_DEBUG_2) << "IP changed to in-addr.arpa format: " << working_name;
			}
		}
		case DNS_QUERY_CNAME:
		case DNS_QUERY_A:
		case DNS_QUERY_AAAA:
		{
			sepstream sep(working_name, '.');
			Anope::string token;
			while (sep.GetToken(token))
			{
				temp_buffer[buffer_pos++] = token.length();
				memcpy(&temp_buffer[buffer_pos], token.c_str(), token.length());
				buffer_pos += token.length();
			}
			break;
		}
		default:
			Log(LOG_DEBUG_2) << "Resolver: Received an unknown query type format " << qt;
			return false;
	}

	temp_buffer[buffer_pos++] = 0;

	short i = htons(qt);
	memcpy(&temp_buffer[buffer_pos], &i, 2);
	buffer_pos += 2;

	i = htons(1);
	memcpy(&temp_buffer[buffer_pos], &i, 2);
	buffer_pos += 2;

	Log(LOG_DEBUG_3) << "Resolver: Packet payload to: " << temp_buffer;
	Log(LOG_DEBUG_3) << "Resolver: Bytes used: " << buffer_pos << ", payload count " << this->payload_count;

	if (buffer_pos > (sizeof(this->payload) - this->payload_count))
		return false;
	
	memmove(this->payload + this->payload_count, temp_buffer, buffer_pos);
	this->payload_count += buffer_pos;

	this->qdcount++;

	return true;
}

inline void DNSPacket::FillPacket(const unsigned char *input, const size_t length)
{
	this->id = (input[0] << 8) | input[1];
	this->flags = (input[2] << 8) | input[3];
	this->qdcount = (input[4] << 8) | input[5];
	this->ancount = (input[6] << 8) | input[7];
	this->nscount = (input[8] << 8) | input[9];
	this->arcount = (input[10] << 8) | input[11];
	memcpy(this->payload, &input[12], length);
	this->payload_count = length;
}

inline void DNSPacket::FillBuffer(unsigned char *buffer)
{
	buffer[0] = this->id >> 8;
	buffer[1] = this->id & 0xFF;
	buffer[2] = this->flags >> 8;
	buffer[3] = this->flags & 0xFF;
	buffer[4] = this->qdcount >> 8;
	buffer[5] = this->qdcount & 0xFF;
	buffer[6] = this->ancount >> 8;
	buffer[7] = this->ancount & 0xFF;
	buffer[8] = this->nscount >> 8;
	buffer[9] = this->nscount & 0xFF;
	buffer[10] = this->arcount >> 8;
	buffer[11] = this->arcount & 0xFF;
	memcpy(&buffer[12], this->payload, this->payload_count);
}

inline DNSRecord::DNSRecord()
{
	this->type = DNS_QUERY_NONE;
	this->record_class = this->ttl = this->rdlength = 0;
	this->created = Anope::CurTime;
}

DNSSocket::DNSSocket() : ConnectionSocket(false, SOCK_DGRAM)
{
}

DNSSocket::~DNSSocket()
{
	for (unsigned i = DNSEngine->packets.size(); i > 0; --i)
		delete DNSEngine->packets[i - 1];
	DNSEngine->packets.clear();
	Log() << "Resolver: Lost connection to nameserver";
	DNSEngine->sock = NULL;
}

int DNSSocket::SendTo(const unsigned char *buf, size_t len) const
{
	return sendto(this->GetFD(), buf, len, 0, &this->conaddr.sa, this->conaddr.size());
}

int DNSSocket::RecvFrom(char *buf, size_t len, sockaddrs &addrs) const
{
	socklen_t x = sizeof(addrs);
	return recvfrom(this->GetFD(), buf, len, 0, &addrs.sa, &x);
}

bool DNSSocket::ProcessRead()
{
	Log(LOG_DEBUG_2) << "Resolver: Reading from UDP socket";

	sockaddrs from_server;
	unsigned char packet_buffer[524];
	int length = this->RecvFrom(reinterpret_cast<char *>(&packet_buffer), sizeof(packet_buffer), from_server);

	if (length < 0)
		return false;

	if (this->conaddr != from_server)
	{
		Log(LOG_DEBUG_2) << "Resolver: Received an answer from the wrong nameserver, Bad NAT or DNS forging attempt? '" << this->conaddr.addr() << "' != '" << from_server.addr() << "'";
		return true;
	}

	if (length < 12)
	{
		Log(LOG_DEBUG_2) << "Resolver: Received a corrupted packet";
		return true;
	}

	/* Remove header length */
	length -= 12;

	DNSPacket recv_packet;
	recv_packet.FillPacket(packet_buffer, length);

	std::map<short, DNSRequest *>::iterator it = DNSEngine->requests.find(recv_packet.id);
	if (it == DNSEngine->requests.end())
	{
		Log(LOG_DEBUG_2) << "Resolver: Received an answer for something we didn't request";
		return true;
	}
	DNSRequest *request = it->second;
	DNSEngine->requests.erase(it);

	if (!(recv_packet.flags & DNS_QUERYFLAGS_QR))
	{
		Log(LOG_DEBUG_2) << "Resolver: Received a non-answer";
		request->OnError(DNS_ERROR_NOT_AN_ANSWER, "Received a non-answer");
	}
	else if (recv_packet.flags & DNS_QUERYFLAGS_OPCODE)
	{
		Log(LOG_DEBUG_2) << "Resolver: Received a nonstandard query";
		request->OnError(DNS_ERROR_NONSTANDARD_QUERY, "Received a nonstandard query");
	}
	else if (recv_packet.flags & DNS_QUERYFLAGS_RCODE)
	{
		switch (recv_packet.flags & DNS_QUERYFLAGS_RCODE)
		{
			case 1:
				Log(LOG_DEBUG_2) << "Resolver: Format error";
				request->OnError(DNS_ERROR_FORMAT_ERROR, "Format error");
				break;
			case 2:
				Log(LOG_DEBUG_2) << "Resolver: Server failure";
				request->OnError(DNS_ERROR_SERVER_FAILURE, "Server failure");
				break;
			case 3:
				Log(LOG_DEBUG_2) << "Resolver: Domain name not found";
				request->OnError(DNS_ERROR_DOMAIN_NOT_FOUND, "Domain not found");
				break;
			case 4:
				Log(LOG_DEBUG_2) << "Resolver: Not Implemented - The name server does not support the requested kind of query.";
				request->OnError(DNS_ERROR_NOT_IMPLEMENTED, "Not Implemented - The name server does not support the requested kind of query.");
				break;
			case 5:
				Log(LOG_DEBUG_2) << "Resolver: Server refused to perform the specificed operation";
				request->OnError(DNS_ERROR_REFUSED, "Server refused to perform the specified operation");
				break;
			default:
				Log(LOG_DEBUG_2) << "Resolver: Unknown error";
				request->OnError(DNS_ERROR_UNKNOWN, "Unknown error");
		}
	}
	else if (recv_packet.ancount < 1)
	{
		Log(LOG_DEBUG_2) << "Resolver: No resource records returned";
		request->OnError(DNS_ERROR_NO_RECORDS, "No resource records returned");
	}
	else
	{
		unsigned packet_pos = 0;

		/* Skip over the questions in this reponse */
		for (unsigned qdcount = 0; qdcount < recv_packet.qdcount; ++qdcount)
		{
			for (unsigned offset = recv_packet.payload[packet_pos]; offset; packet_pos += offset + 1, offset = recv_packet.payload[packet_pos])
			{
				if (offset & 0xC0)
				{
					packet_pos += 2;
					offset = 0;
				}
			}

			packet_pos += 5;
		}

		for (unsigned ancount = 0; ancount < recv_packet.ancount; ++ancount)
		{
			Anope::string name;
			unsigned packet_pos_ptr = packet_pos;
			for (unsigned offset = recv_packet.payload[packet_pos_ptr]; offset; packet_pos_ptr += offset + 1, offset = recv_packet.payload[packet_pos_ptr])
			{
				if (offset & 0xC0)
				{
					offset = (offset & 0x3F) << 8 | recv_packet.payload[++packet_pos_ptr];
					packet_pos_ptr = offset - 12;
					offset = recv_packet.payload[packet_pos_ptr];
				}
				for (unsigned i = 1; i <= offset; ++i)
					name += recv_packet.payload[packet_pos_ptr + i];
				name += ".";
			}
			name.erase(name.begin() + name.length() - 1);

			if (packet_pos_ptr < packet_pos)
				packet_pos += 2;
			else
				packet_pos = packet_pos_ptr + 1;

			DNSRecord *rr = new DNSRecord;
			rr->name = name;

			Log(LOG_DEBUG_2) << "Resolver: Received answer for " << name;

			rr->type = static_cast<QueryType>(recv_packet.payload[packet_pos] << 8 | (recv_packet.payload[packet_pos + 1] & 0xFF));
			packet_pos += 2;

			rr->record_class = recv_packet.payload[packet_pos] << 8 | (recv_packet.payload[packet_pos + 1] & 0xFF);
			packet_pos += 2;

			rr->ttl = (recv_packet.payload[packet_pos] << 24) | (recv_packet.payload[packet_pos + 1] << 16) | (recv_packet.payload[packet_pos + 2] << 8) | recv_packet.payload[packet_pos + 3];
			packet_pos += 4;

			rr->rdlength = (recv_packet.payload[packet_pos] << 8 | recv_packet.payload[packet_pos + 1]);
			packet_pos += 2;

			switch (rr->type)
			{
				case DNS_QUERY_A:
				{
					unsigned long ip = (recv_packet.payload[packet_pos]) | (recv_packet.payload[packet_pos + 1]) << 8 | (recv_packet.payload[packet_pos + 2] << 16)  | (recv_packet.payload[packet_pos + 3] << 24);
					packet_pos += 4;

					char ipbuf[INET_ADDRSTRLEN];
					if (!inet_ntop(AF_INET, &ip, ipbuf, sizeof(ipbuf)))
					{
						Log(LOG_DEBUG_2) << "Resolver: Received an invalid IP for DNS_QUERY_A: " << Anope::LastError();
						request->OnError(DNS_ERROR_FORMAT_ERROR, "Received an invalid IP from the nameserver: " + Anope::LastError());
						delete rr;
						rr = NULL;
					}
					else
						rr->result = ipbuf;
					break;
				}
				case DNS_QUERY_AAAA:
				{
					unsigned char ip[16];
					for (int i = 0; i < 16; ++i)
						ip[i] = recv_packet.payload[packet_pos + i];
					packet_pos += 16;

					char ipbuf[INET6_ADDRSTRLEN];
					if (!inet_ntop(AF_INET6, &ip, ipbuf, sizeof(ipbuf)))
					{
						Log(LOG_DEBUG_2) << "Resolver: Received an invalid IP for DNS_QUERY_A: " << Anope::LastError();
						request->OnError(DNS_ERROR_FORMAT_ERROR, "Received an invalid IP from the nameserver: " + Anope::LastError());
						delete rr;
						rr = NULL;
					}
					else
						rr->result = ipbuf;
					break;
				}
				case DNS_QUERY_CNAME:
				case DNS_QUERY_PTR:
				{
					name.clear();
					packet_pos_ptr = packet_pos;
					for (unsigned offset = recv_packet.payload[packet_pos_ptr]; offset; packet_pos_ptr += offset + 1, offset = recv_packet.payload[packet_pos_ptr])
					{
						if (offset & 0xC0)
						{
							offset = (offset & 0x3F) << 8 | recv_packet.payload[++packet_pos_ptr];
							packet_pos_ptr = offset - 12;
							offset = recv_packet.payload[packet_pos_ptr];
						}
						for (unsigned i = 1; i <= offset; ++i)
							name += recv_packet.payload[packet_pos_ptr + i];
						name += ".";
					}
					name.erase(name.begin() + name.length() - 1);

					if (packet_pos_ptr < packet_pos)
						packet_pos += 2;
					else
						packet_pos = packet_pos_ptr + 1;

					rr->result = name;
					break;
				}
				default:
					delete rr;
					request->OnError(DNS_ERROR_INVALID_TYPE, "Invalid query type");
					rr = NULL;
			}

			if (rr && request->QT != rr->type)
			{
				delete rr;
				rr = NULL;
			}

			if (rr)
			{
				request->OnLookupComplete(rr);
				DNSEngine->AddCache(rr);
			}
		}
	}
	
	delete request;
	return true;
}

bool DNSSocket::ProcessWrite()
{
	Log(LOG_DEBUG_2) << "Resolver: Writing to UDP socket";

	bool cont = true;
	for (unsigned i = DNSEngine->packets.size(); cont && i > 0; --i)
	{
		DNSPacket *r = DNSEngine->packets[i - 1];

		unsigned char buffer[524];
		r->FillBuffer(buffer);

		cont = this->SendTo(buffer, r->payload_count + 12) == r->payload_count + 12;

		delete r;
		DNSEngine->packets.erase(DNSEngine->packets.begin() + i - 1);
	}
	SocketEngine->ClearWritable(this);
	return cont;
}

DNSManager::DNSManager() : Timer(3600, Anope::CurTime, true)
{
	this->sock = NULL;
}

DNSManager::~DNSManager()
{
	delete this->sock;
}

void DNSManager::AddCache(DNSRecord *rr)
{
	this->cache.insert(std::make_pair(rr->name, rr));
}

bool DNSManager::CheckCache(DNSRequest *request)
{
	std::multimap<Anope::string, DNSRecord *>::iterator it = this->cache.find(request->address);
	if (it != this->cache.end())
	{
		std::multimap<Anope::string, DNSRecord *>::iterator it_end = this->cache.upper_bound(request->address);
		
		for (; it != it_end; ++it)
		{
			DNSRecord *rec = it->second;
			if (rec->created + rec->ttl >= Anope::CurTime)
			{
				request->OnLookupComplete(rec);
			}
		}
		
		return true;
	}

	return false;
}

void DNSManager::Tick(time_t now)
{
	for (std::multimap<Anope::string, DNSRecord *>::iterator it = this->cache.begin(), it_end = this->cache.end(); it != it_end; ++it)
	{
		DNSRecord *req = it->second;

		if (req->created + req->ttl < now)
		{
			delete req;
			this->cache.erase(it);
		}
	}
}

void DNSManager::Cleanup(Module *mod)
{
	for (std::map<short, DNSRequest *>::iterator it = this->requests.begin(), it_end = this->requests.end(); it != it_end;)
	{
		short id = it->first;
		DNSRequest *req = it->second;
		++it;

		if (req->creator && req->creator == mod)
		{
			req->OnError(DNS_ERROR_UNLOADED, "Module is being unloaded");
			delete req;
			this->requests.erase(id);
		}
	}
}

DNSRequestTimeout::DNSRequestTimeout(DNSRequest *r, time_t timeout) : Timer(timeout), request(r)
{
	this->done = false;
}

DNSRequestTimeout::~DNSRequestTimeout()
{
}

void DNSRequestTimeout::Tick(time_t)
{
	this->done = true;
	this->request->OnError(DNS_ERROR_TIMEOUT, "Request timed out");
	delete this->request;
}

