#include "modules/nickserv.h"

class AccountImpl : public NickServ::Account
{
 public:
	AccountImpl(Serialize::TypeBase *type) : NickServ::Account(type) { }
	AccountImpl(Serialize::TypeBase *type, Serialize::ID id) : NickServ::Account(type, id) { }
	~AccountImpl();
	void Delete() override;

	Anope::string GetDisplay() override;
	void SetDisplay(const Anope::string &) override;

	Anope::string GetPassword() override;
	void SetPassword(const Anope::string &) override;

	Anope::string GetEmail() override;
	void SetEmail(const Anope::string &) override;

	Anope::string GetLanguage() override;
	void SetLanguage(const Anope::string &) override;

	MemoServ::MemoInfo *GetMemos() override;

	void SetDisplay(NickServ::Nick *na) override;
	bool IsServicesOper() const override;
	/*void AddAccess(const Anope::string &entry) override;
	Anope::string GetAccess(unsigned entry) const override;
	unsigned GetAccessCount() const override;
	bool FindAccess(const Anope::string &entry) override;
	void EraseAccess(const Anope::string &entry) override;
	void ClearAccess() override;*/
	bool IsOnAccess(User *u) override;
	unsigned int GetChannelCount() override;
};
