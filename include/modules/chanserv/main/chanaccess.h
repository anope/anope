#pragma once

class ChanAccessImpl : public ChanServ::ChanAccess
{
 public:
	ChanAccessImpl(Serialize::TypeBase *type) : ChanServ::ChanAccess(type) { }
	ChanAccessImpl(Serialize::TypeBase *type, Serialize::ID id) : ChanServ::ChanAccess(type, id) { }

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *ci) override;

	Anope::string GetCreator() override;
	void SetCreator(const Anope::string &c) override;

	time_t GetLastSeen() override;
	void SetLastSeen(const time_t &t) override;

	time_t GetCreated() override;
	void SetCreated(const time_t &t) override;

	Anope::string GetMask() override;
	void SetMask(const Anope::string &) override;

	Serialize::Object *GetObj() override;
	void SetObj(Serialize::Object *) override;

	Anope::string Mask() override;
	NickServ::Account *GetAccount() override;

	bool Matches(const User *u, NickServ::Account *acc) override;
};
