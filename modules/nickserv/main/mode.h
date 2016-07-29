
class ModeImpl : public NickServ::Mode
{
	friend class NSModeType;

	NickServ::Account *account = nullptr;
	Anope::string mode;

 public:
	ModeImpl(Serialize::TypeBase *type) : NickServ::Mode(type) { }
	ModeImpl(Serialize::TypeBase *type, Serialize::ID id) : NickServ::Mode(type, id) { }

	NickServ::Account *GetAccount() override;
	void SetAccount(NickServ::Account *) override;

	Anope::string GetMode() override;
	void SetMode(const Anope::string &) override;
};

