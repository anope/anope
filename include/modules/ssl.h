
class SSLService : public Service
{
 public:
	static constexpr const char *NAME = "ssl";
	
	SSLService(Module *o, const Anope::string &n) : Service(o, NAME, n) { }

	virtual void Init(Socket *s) anope_abstract;
};

