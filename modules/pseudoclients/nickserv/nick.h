
class NickImpl : public NickServ::Nick
{
 public:
	NickImpl(const Anope::string &nickname, NickServ::Account *nickcore);
	~NickImpl();

	void Serialize(Serialize::Data &data) const override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &);

	void SetVhost(const Anope::string &ident, const Anope::string &host, const Anope::string &creator, time_t created = Anope::CurTime);
	void RemoveVhost();
	bool HasVhost() const;
	const Anope::string &GetVhostIdent() const override;
	const Anope::string &GetVhostHost() const override;
	const Anope::string &GetVhostCreator() const override;
	time_t GetVhostCreated() const override;
};
