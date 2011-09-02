
class SSLService : public Service<Base>
{
 public:
	SSLService(Module *o, const Anope::string &n) : Service<Base>(o, n) { }
	
	virtual void Init(Socket *s) = 0;
};

