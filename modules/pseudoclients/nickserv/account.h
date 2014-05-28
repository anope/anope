#include "modules/nickserv.h"

class AccountImpl : public NickServ::Account
{
 public:
	AccountImpl(const Anope::string &nickdisplay);
	~AccountImpl();
	void Serialize(Serialize::Data &data) const override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &);
	void SetDisplay(const NickServ::Nick *na) override;
	bool IsServicesOper() const override;
	void AddAccess(const Anope::string &entry) override;
	Anope::string GetAccess(unsigned entry) const override;
	unsigned GetAccessCount() const override;
	bool FindAccess(const Anope::string &entry) override;
	void EraseAccess(const Anope::string &entry) override;
	void ClearAccess() override;
	bool IsOnAccess(const User *u) const override;
	void AddChannelReference(ChanServ::Channel *ci);
	void RemoveChannelReference(ChanServ::Channel *ci) override;
	void GetChannelReferences(std::deque<ChanServ::Channel *> &queue) override;
};
