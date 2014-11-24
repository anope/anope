

class ChannelImpl : public ChanServ::Channel
{
 public:
	//using ChanServ::Channel::Channel;
	ChannelImpl(Serialize::TypeBase *type) : ChanServ::Channel(type) { }
	ChannelImpl(Serialize::TypeBase *type, Serialize::ID id) : ChanServ::Channel(type, id) { }
	ChannelImpl(Serialize::TypeBase *type, const Anope::string &chname);
	/*ChannelImpl(const ChanServ::Channel &ci);*/
	~ChannelImpl();
	void Delete() override;

	Anope::string GetName() override;
	void SetName(const Anope::string &) override;

	Anope::string GetDesc() override;
	void SetDesc(const Anope::string &) override;

	time_t GetTimeRegistered() override;
	void SetTimeRegistered(const time_t &) override;

	time_t GetLastUsed() override;
	void SetLastUsed(const time_t &) override;

	Anope::string GetLastTopic() override;
	void SetLastTopic(const Anope::string &) override;

	Anope::string GetLastTopicSetter() override;
	void SetLastTopicSetter(const Anope::string &) override;

	time_t GetLastTopicTime() override;
	void SetLastTopicTime(const time_t &) override;

	int16_t GetBanType() override;
	void SetBanType(const int16_t &) override;

	time_t GetBanExpire() override;
	void SetBanExpire(const time_t &) override;

	BotInfo *GetBI() override;
	void SetBI(BotInfo *) override;

	ServiceBot *GetBot() override;
	void SetBot(ServiceBot *) override;

	MemoServ::MemoInfo *GetMemos() override;

	bool IsFounder(const User *user) override;
	void SetFounder(NickServ::Account *nc) override;
	NickServ::Account *GetFounder() override;
	void SetSuccessor(NickServ::Account *nc) override;
	NickServ::Account *GetSuccessor() override;
	ChanServ::ChanAccess *GetAccess(unsigned index) /*const*/ override;
	ChanServ::AccessGroup AccessFor(const User *u) override;
	ChanServ::AccessGroup AccessFor(NickServ::Account *nc) override;
	unsigned GetAccessCount()/* const*/ override;
	unsigned GetDeepAccessCount() const override;
	void ClearAccess() override;
	AutoKick* AddAkick(const Anope::string &user, NickServ::Account *akicknc, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0) override;
	AutoKick* AddAkick(const Anope::string &user, const Anope::string &mask, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0) override;
	AutoKick* GetAkick(unsigned index) override;
	unsigned GetAkickCount() override;
	void ClearAkick() override;
	int16_t GetLevel(const Anope::string &priv) override;
	void SetLevel(const Anope::string &priv, int16_t level) override;
	void RemoveLevel(const Anope::string &priv) override;
	void ClearLevels() override;
	Anope::string GetIdealBan(User *u) override;
};
