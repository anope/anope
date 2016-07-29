
class NickImpl : public NickServ::Nick
{
	friend class NickType;

	NickServ::Account *account = nullptr;
	Anope::string nick, last_quit, last_realname, last_usermask, last_realhost;
	time_t time_registered = 0, last_seen = 0;
	Anope::string vhost_ident, vhost_host, vhost_creator;
	time_t vhost_created = 0;

 public:
	NickImpl(Serialize::TypeBase *type) : NickServ::Nick(type) { }
	NickImpl(Serialize::TypeBase *type, Serialize::ID id) : NickServ::Nick(type, id) { }
	~NickImpl();
	void Delete() override;

	Anope::string GetNick() override;
	void SetNick(const Anope::string &) override;

	Anope::string GetLastQuit() override;
	void SetLastQuit(const Anope::string &) override;

	Anope::string GetLastRealname() override;
	void SetLastRealname(const Anope::string &) override;

	Anope::string GetLastUsermask() override;
	void SetLastUsermask(const Anope::string &) override;

	Anope::string GetLastRealhost() override;
	void SetLastRealhost(const Anope::string &) override;

	time_t GetTimeRegistered() override;
	void SetTimeRegistered(const time_t &) override;

	time_t GetLastSeen() override;
	void SetLastSeen(const time_t &) override;

	NickServ::Account *GetAccount() override;
	void SetAccount(NickServ::Account *acc) override;

	void SetVhost(const Anope::string &ident, const Anope::string &host, const Anope::string &creator, time_t created = Anope::CurTime) override;
	void RemoveVhost() override;
	bool HasVhost() override;

	Anope::string GetVhostIdent() override;
	void SetVhostIdent(const Anope::string &) override;
	Anope::string GetVhostHost() override;
	void SetVhostHost(const Anope::string &) override;
	Anope::string GetVhostCreator() override;
	void SetVhostCreator(const Anope::string &) override;
	time_t GetVhostCreated() override;
	void SetVhostCreated(const time_t &) override;
};
