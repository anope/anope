
class ModeImpl : public ChanServ::Mode
{
	friend class CSModeType;

	ChanServ::Channel *channel = nullptr;
	Anope::string mode, param;

 public:
	ModeImpl(Serialize::TypeBase *type) : ChanServ::Mode(type) { }
	ModeImpl(Serialize::TypeBase *type, Serialize::ID id) : ChanServ::Mode(type, id) { }

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *) override;

	Anope::string GetMode() override;
	void SetMode(const Anope::string &) override;

	Anope::string GetParam() override;
	void SetParam(const Anope::string &) override;
};

