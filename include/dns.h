/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#ifndef DNS_H
#define DNS_H

#include "sockets.h"
#include "timers.h"
#include "extern.h"
#include "config.h"

/** Valid query types
 */
enum QueryType
{
	/* Nothing */
	DNS_QUERY_NONE,
	/* A simple A lookup */
	DNS_QUERY_A = 1,
	/* An authoritative name server */
	DNS_QUERY_NS = 2,
	/* A CNAME lookup */
	DNS_QUERY_CNAME = 5,
	/* Start of a zone of authority */
	DNS_QUERY_SOA = 6,
	/* Reverse DNS lookup */
	DNS_QUERY_PTR = 12,
	/* IPv6 AAAA lookup */
	DNS_QUERY_AAAA = 28,
	/* Zone transfer */
	DNS_QUERY_AXFR = 252
};

/** Flags that can be AND'd into DNSPacket::flags to receive certain values
 */
enum
{
	DNS_QUERYFLAGS_QR = 0x8000,
	DNS_QUERYFLAGS_OPCODE = 0x7800,
	DNS_QUERYFLAGS_AA = 0x400,
	DBS_QUERYFLAGS_TC = 0x200,
	DNS_QUERYFLAGS_RD = 0x100,
	DNS_QUERYFLAGS_RA = 0x80,
	DNS_QUERYFLAGS_Z = 0x70,
	DNS_QUERYFLAGS_RCODE = 0xF
};

enum DNSError
{
	DNS_ERROR_NONE,
	DNS_ERROR_UNKNOWN,
	DNS_ERROR_UNLOADED,
	DNS_ERROR_TIMEOUT,
	DNS_ERROR_NOT_AN_ANSWER,
	DNS_ERROR_NONSTANDARD_QUERY,
	DNS_ERROR_FORMAT_ERROR,
	DNS_ERROR_SERVER_FAILURE,
	DNS_ERROR_DOMAIN_NOT_FOUND,
	DNS_ERROR_NOT_IMPLEMENTED,
	DNS_ERROR_REFUSED,
	DNS_ERROR_NO_RECORDS,
	DNS_ERROR_INVALIDTYPE
};


struct CoreExport Question
{
	Anope::string name;
	QueryType type;
	unsigned short qclass;

	Question();
	Question(const Anope::string &, QueryType, unsigned short = 1);
};

struct CoreExport ResourceRecord : public Question
{
	unsigned int ttl;
	Anope::string rdata;
	time_t created;

	ResourceRecord(const Anope::string &, QueryType, unsigned short = 1);
	ResourceRecord(const Question &);
};

struct CoreExport DNSQuery
{
	std::vector<Question> questions;
	std::vector<ResourceRecord> answers, authorities, additional;
	DNSError error;

	DNSQuery();
	DNSQuery(const Question &q);
};

/** The request
 */
class CoreExport DNSRequest : public Timer, public Question
{
	/* Use result cache if available */
	bool use_cache;

 public:
	/* Request id */
 	unsigned short id;
 	/* Creator of this request */
	Module *creator;

	DNSRequest(const Anope::string &addr, QueryType qt, bool cache = false, Module *c = NULL);

	virtual ~DNSRequest();

	void Process();

	virtual void OnLookupComplete(const DNSQuery *r) = 0;

	virtual void OnError(const DNSQuery *r);
	
	void Tick(time_t) anope_override;
};

/** A full packet sent or recieved to/from the nameserver
 */
class DNSPacket : public DNSQuery
{
	static const int DNS_POINTER = 0xC0;
	static const int DNS_LABEL = 0x3F;

	void PackName(unsigned char *output, unsigned short output_size, unsigned short &pos, const Anope::string &name);
	Anope::string UnpackName(const unsigned char *input, unsigned short input_size, unsigned short &pos);
	
	Question UnpackQuestion(const unsigned char *input, unsigned short input_size, unsigned short &pos);
	ResourceRecord UnpackResourceRecord(const unsigned char *input, unsigned short input_size, unsigned short &poss);
 public:
	static const int HEADER_LENGTH = 12;

	/* Source or destination of the packet */
	sockaddrs addr;
	/* ID for this packet */
	unsigned short id;
	/* Flags on the packet */
	unsigned short flags;

	DNSPacket(sockaddrs *a);
	void Fill(const unsigned char *input, const unsigned short len);
	unsigned short Pack(unsigned char *output, unsigned short output_size);
};

/** DNS manager
 */
class CoreExport DNSManager : public Timer
{
	class ReplySocket : public virtual Socket
	{
	 public:
		virtual ~ReplySocket() { }
		virtual void Reply(DNSPacket *p) = 0;
	};

	/* Listens for TCP requests */
	class TCPSocket : public ListenSocket
	{
		/* A TCP client */
		class Client : public ClientSocket, public Timer, public ReplySocket
		{
			TCPSocket *tcpsock;
	 		DNSPacket *packet;
			unsigned char packet_buffer[524];
			int length;

		 public:
		 	Client(TCPSocket *ls, int fd, const sockaddrs &addr);
			~Client();

			/* Times out after a few seconds */
			void Tick(time_t) anope_override { }
			void Reply(DNSPacket *p) anope_override;
		 	bool ProcessRead() anope_override;
			bool ProcessWrite() anope_override;
		};

	 public:
		TCPSocket(const Anope::string &ip, int port);

		ClientSocket *OnAccept(int fd, const sockaddrs &addr) anope_override;
	};

	/* Listens for UDP requests */
	class UDPSocket : public ReplySocket
	{
		std::deque<DNSPacket *> packets;
	 public:

		UDPSocket(const Anope::string &ip, int port);
		~UDPSocket();

		void Reply(DNSPacket *p) anope_override;

		std::deque<DNSPacket *>& GetPackets() { return packets; }

	 	bool ProcessRead() anope_override;

		bool ProcessWrite() anope_override;
	};

	typedef std::multimap<Anope::string, ResourceRecord, ci::less> cache_map;
	cache_map cache;

	bool listen;
	uint32_t serial;

 public:
	TCPSocket *tcpsock;
	UDPSocket *udpsock;

	sockaddrs addrs;
	std::map<unsigned short, DNSRequest *> requests;

	DNSManager(const Anope::string &nameserver, const Anope::string &ip, int port);
	~DNSManager();

	bool HandlePacket(ReplySocket *s, const unsigned char *const data, int len, sockaddrs *from);

	/** Add a record to the dns cache
	 * @param r The record
	 */
	void AddCache(DNSQuery &r);

	/** Check the DNS cache to see if request can be handled by a cached result
	 * @return true if a cached result was found.
	 */
	bool CheckCache(DNSRequest *request);

	/** Tick this timer, used to clear the DNS cache.
	 */
	void Tick(time_t now) anope_override;

	/** Cleanup all pending DNS queries for a module
	 * @param mod The module
	 */
	void Cleanup(Module *mod);

	void UpdateSerial();
	uint32_t GetSerial() const;

	/** Does a BLOCKING DNS query and returns the first IP.
	 * Only use this if you know what you are doing. Unless you specifically
	 * need a blocking query use the DNSRequest system
	 */
	static DNSQuery BlockingQuery(const Anope::string &mask, QueryType qt);
};

extern CoreExport DNSManager *DNSEngine;

#endif // DNS_H


