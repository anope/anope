

class ChannelImpl : public ChanServ::Channel
{
 public:
	 ChannelImpl(const Anope::string &chname);
	 ChannelImpl(const ChanServ::Channel &ci);
	 ~ChannelImpl();

	 void Serialize(Serialize::Data &data) const override;
	 static Serializable* Unserialize(Serializable *obj, Serialize::Data &);

	 bool IsFounder(const User *user) override;
	 void SetFounder(NickServ::Account *nc) override;
	 NickServ::Account *GetFounder() const override;
	 void SetSuccessor(NickServ::Account *nc) override;
	 NickServ::Account *GetSuccessor() const override;
	 BotInfo *WhoSends() const override;
	 void AddAccess(ChanServ::ChanAccess *access) override;
	 ChanServ::ChanAccess *GetAccess(unsigned index) const override;
	 ChanServ::AccessGroup AccessFor(const User *u) override;
	 ChanServ::AccessGroup AccessFor(const NickServ::Account *nc) override;
	 unsigned GetAccessCount() const override;
	 unsigned GetDeepAccessCount() const override;
	 ChanServ::ChanAccess *EraseAccess(unsigned index) override;
	 void ClearAccess() override;
	 AutoKick* AddAkick(const Anope::string &user, NickServ::Account *akicknc, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0) override;
	 AutoKick* AddAkick(const Anope::string &user, const Anope::string &mask, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0) override;
	 AutoKick* GetAkick(unsigned index) const override;
	 unsigned GetAkickCount() const override;
	 void EraseAkick(unsigned index) override;
	 void ClearAkick() override;
	 int16_t GetLevel(const Anope::string &priv) const override;
	 void SetLevel(const Anope::string &priv, int16_t level) override;
	 void RemoveLevel(const Anope::string &priv) override;
	 void ClearLevels() override;
	 Anope::string GetIdealBan(User *u) const override;
	 void AddChannelReference(const Anope::string &what) override;
	 void RemoveChannelReference(const Anope::string &what) override;
	 void GetChannelReferences(std::deque<Anope::string> &chans) override;
};
