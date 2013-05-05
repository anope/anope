
class SSLService : public Service
{
 public:
	SSLService(Module *o, const Anope::string &n) : Service(o, "SSLService", n) { }
	
	virtual void Init(Socket *s) = 0;
};

