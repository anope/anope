
class LevelImpl : public ChanServ::Level
{
 public:
	LevelImpl(Serialize::TypeBase *type) : ChanServ::Level(type) { }
	LevelImpl(Serialize::TypeBase *type, Serialize::ID id) : ChanServ::Level(type, id) { }

	ChanServ::Channel *GetChannel();
	void SetChannel(ChanServ::Channel *);

	Anope::string GetName();
	void SetName(const Anope::string &);

	int GetLevel();
	void SetLevel(const int &);
};
