#ifndef DNS_H
#define DNS_H

/** Valid query types
 */
enum QueryType
{
	/* Nothing */
	DNS_QUERY_NONE,
	/* A simple A lookup */
	DNS_QUERY_A = 1,
	/* A CNAME lookup */
	DNS_QUERY_CNAME = 5,
	/* Reverse DNS lookup */
	DNS_QUERY_PTR = 12,
	/* IPv6 AAAA lookup */
	DNS_QUERY_AAAA = 28
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

class Module;
struct DNSQuery;
class DNSPacket;

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
	DNSQuery(const DNSPacket &p);
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
	
	void Tick(time_t);
};

/** A full packet sent or recieved to/from the nameserver, may contain multiple queries
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

	/* Our 16-bit id for this header */
	unsigned short id;
	/* Flags on the query */
	unsigned short flags;

	DNSPacket();
	void Fill(const unsigned char *input, const unsigned short len);
	unsigned short Pack(unsigned char *output, unsigned short output_size);
};

/** DNS manager, manages all requests
 */
class CoreExport DNSManager : public Timer, public Socket
{
	typedef std::multimap<Anope::string, ResourceRecord, ci::less> cache_map;
	cache_map cache;
	sockaddrs addrs;
 public:
	std::deque<DNSPacket *> packets;
	std::map<unsigned short, DNSRequest *> requests;

	static const int DNSPort = 53;

	DNSManager(const Anope::string &nameserver, int port);

	~DNSManager();

	bool ProcessRead();

	bool ProcessWrite();

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
	void Tick(time_t now);

	/** Cleanup all pending DNS queries for a module
	 * @param mod The module
	 */
	void Cleanup(Module *mod);

	/** Does a BLOCKING DNS query and returns the first IP.
	 * Only use this if you know what you are doing. Unless you specifically
	 * need a blocking query use the DNSRequest system
	 */
	static DNSQuery BlockingQuery(const Anope::string &mask, QueryType qt);
};

extern DNSManager *DNSEngine;

#endif // DNS_H


