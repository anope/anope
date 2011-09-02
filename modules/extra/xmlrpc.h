
class XMLRPCClientSocket;
class XMLRPCListenSocket;
class XMLRPCServiceInterface;

class XMLRPCClientSocket : public ClientSocket, public BufferedSocket
{
 protected:
	Anope::string query;

 public:
	XMLRPCClientSocket(ListenSocket *ls, const sockaddrs &addr) : ClientSocket(ls, addr), BufferedSocket() { }

	virtual ~XMLRPCClientSocket() { }

	virtual bool Read(const Anope::string &message) = 0;

	virtual bool GetData(Anope::string &tag, Anope::string &data) = 0;

	virtual void HandleMessage() = 0;
};

class XMLRPCListenSocket : public ListenSocket
{
 protected:
	std::vector<Anope::string> allowed;

 public:
	Anope::string username;
	Anope::string password;

	XMLRPCListenSocket(const Anope::string &bindip, int port, bool ipv6, const Anope::string &u, const Anope::string &p, const std::vector<Anope::string> &a) : ListenSocket(bindip, port, ipv6), allowed(a), username(u), password(p) { }

	virtual ~XMLRPCListenSocket() { }
	
	virtual ClientSocket *OnAccept(int fd, const sockaddrs &addr) = 0;
};

class XMLRPCRequest
{
	std::map<Anope::string, Anope::string> replies;

 public:
	Anope::string name;
	Anope::string id;
	std::deque<Anope::string> data;
	
	inline void reply(const Anope::string &dname, const Anope::string &ddata) { this->replies.insert(std::make_pair(dname, ddata)); }
	inline const std::map<Anope::string, Anope::string> &get_replies() { return this->replies; }
};

class XMLRPCEvent
{
 public:
	virtual void Run(XMLRPCServiceInterface *iface, XMLRPCClientSocket *source, XMLRPCRequest *request) = 0;
};

class XMLRPCServiceInterface : public Service<Base>
{
 public:
	XMLRPCServiceInterface(Module *creator, const Anope::string &sname) : Service<Base>(creator, sname) { }

	virtual void Register(XMLRPCEvent *event) = 0;

	virtual void Unregister(XMLRPCEvent *event) = 0;

	virtual void Reply(XMLRPCClientSocket *source, XMLRPCRequest *request) = 0;
	
	virtual Anope::string Sanitize(const Anope::string &string) = 0;
	
	virtual void RunXMLRPC(XMLRPCClientSocket *source, XMLRPCRequest *request) = 0;
};

