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
enum QueryFlags
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

class DNSRequestTimeout; // Forward declarations
struct DNSRecord;
class Module;

/** The request
 */
class CoreExport DNSRequest
{
	/* Timeout timer for this request */
	DNSRequestTimeout *timeout;
	/* Use result cache if available */
	bool use_cache;

 public:
	/* Request id */
 	unsigned short id;
 	/* Creator of this request */
	Module *creator;

	/* Address we're looking up */
	Anope::string address;
	/* QueryType, A, AAAA, PTR etc */
	QueryType QT;

	DNSRequest(const Anope::string &addr, QueryType qt, bool cache = false, Module *c = NULL);

	virtual ~DNSRequest();

	void Process();

	virtual void OnLookupComplete(const DNSRecord *r) = 0;

	virtual void OnError(const DNSRecord *r);
};

/** A full packet sent to the nameserver, may contain multiple queries
 */
struct DNSPacket
{
	/* Our 16-bit id for this header */
	unsigned short id;
	/* Flags on the query */
	unsigned short flags;
	/* Number of queries */
	unsigned short qdcount;
	/* Number of resource records in answer */
	unsigned short ancount;
	/* Number of NS resource records in authority records section */
	unsigned short nscount;
	/* Number of resource records in the additional records section */
	unsigned short arcount;
	/* How many of the bytes of the payload are in use */
	unsigned short payload_count;
	/* The queries, at most can be 512 bytes */
	unsigned char payload[512];

	inline DNSPacket();

	bool AddQuestion(const Anope::string &address, QueryType qt);

	inline void FillPacket(const unsigned char *input, const size_t length);

	inline void FillBuffer(unsigned char *buffer);
};

struct CoreExport DNSRecord
{
	/* Name of the initial lookup */
	Anope::string name;
	/* Result of the lookup */
	Anope::string result;
	/* Type of query this was */
	QueryType type;
	/* Error, if there was one */
	DNSError error;
	/* Record class, should always be 1 */
	unsigned short record_class;
	/* Time to live */
	time_t ttl;
	/* Record length */
	unsigned short rdlength;

	/* When this record was created in our cache */
	time_t created;

	inline DNSRecord(const Anope::string &n);
	operator bool() const;
};

/** DNS manager, manages the connection and all requests
 */
class CoreExport DNSManager : public Timer, public Socket
{
	std::multimap<Anope::string, DNSRecord *> cache;
	sockaddrs addrs;
 public:
	std::deque<DNSPacket *> packets;
	std::map<short, DNSRequest *> requests;

	static const int DNSPort = 53;

	DNSManager(const Anope::string &nameserver, int port);

	~DNSManager();

	bool ProcessRead();

	bool ProcessWrite();

	void AddCache(DNSRecord *rr);
	bool CheckCache(DNSRequest *request);
	void Tick(time_t now);

	void Cleanup(Module *mod);

	/** Does a BLOCKING DNS query and returns the first IP.
	 * Only use this if you know what you are doing. Unless you specifically
	 * need a blocking query use the DNSRequest system
	 */
	static DNSRecord BlockingQuery(const Anope::string &mask, QueryType qt);
};

/** A DNS timeout, one is made for every DNS request to detect timeouts
 */
class DNSRequestTimeout : public Timer
{
	DNSRequest *request;
 public:
 	bool done;

	DNSRequestTimeout(DNSRequest *r, time_t timeout);

	~DNSRequestTimeout();

	void Tick(time_t);
};

extern DNSManager *DNSEngine;

#endif // DNS_H

