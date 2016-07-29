#include "httpd.h"

class XMLRPCRequest
{
	std::map<Anope::string, Anope::string> replies;

 public:
	Anope::string name;
	Anope::string id;
	std::deque<Anope::string> data;
	HTTPReply& r;

	XMLRPCRequest(HTTPReply &_r) : r(_r) { }
	inline void reply(const Anope::string &dname, const Anope::string &ddata) { this->replies.insert(std::make_pair(dname, ddata)); }
	inline const std::map<Anope::string, Anope::string> &get_replies() { return this->replies; }
};

class XMLRPCServiceInterface;

class XMLRPCEvent
{
 public:
 	virtual ~XMLRPCEvent() { }
	virtual bool Run(XMLRPCServiceInterface *iface, HTTPClient *client, XMLRPCRequest &request) anope_abstract;
};

class XMLRPCServiceInterface : public Service
{
 public:
	static constexpr const char *NAME = "XMLRPCServiceInterface";
	
	XMLRPCServiceInterface(Module *creator, const Anope::string &sname) : Service(creator, NAME) { }

	virtual void Register(XMLRPCEvent *event) anope_abstract;

	virtual void Unregister(XMLRPCEvent *event) anope_abstract;

	virtual Anope::string Sanitize(const Anope::string &string) anope_abstract;

	virtual void Reply(XMLRPCRequest &request) anope_abstract;
};

