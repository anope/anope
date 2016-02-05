
class LevelImpl : public ChanServ::Level
{
 public:
	LevelImpl(Serialize::TypeBase *type) : ChanServ::Level(type) { }
	LevelImpl(Serialize::TypeBase *type, Serialize::ID id) : ChanServ::Level(type, id) { }

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *) override;

	Anope::string GetName() override;
	void SetName(const Anope::string &) override;

	int GetLevel() override;
	void SetLevel(const int &) override;
};
