
class SSLService : public Service<SSLService>
{
 public:
	SSLService(Module *o, const Anope::string &n) : Service<SSLService>(o, n) { }
	
	virtual void Init(Socket *s) = 0;
};

