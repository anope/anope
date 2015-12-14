#include "module.h"
#include "accounttype.h"
//#include "account.h"

AccountType::AccountType(Module *me) : Serialize::Type<AccountImpl>(me, "NickCore")
	, display(this, "display")
	, pass(this, "pass")
	, email(this, "email")
	, language(this, "language")
{

}

void AccountType::Display::SetField(AccountImpl *acc, const Anope::string &disp)
{
	NickServ::nickcore_map& map = NickServ::service->GetAccountMap();

	if (!acc->GetDisplay().empty())
		map.erase(acc->GetDisplay());

	Serialize::Field<AccountImpl, Anope::string>::SetField(acc, disp);

	if (!disp.empty())
		map[disp] = acc;

	acc->o = Oper::Find(disp);
}

NickServ::Account *AccountType::FindAccount(const Anope::string &acc)
{
	Serialize::ID id;
	EventReturn result = Event::OnSerialize(&Event::SerializeEvents::OnSerializeFind, this, &this->display, acc, id);
	if (result == EVENT_ALLOW)
		return RequireID(id);

	NickServ::nickcore_map &map = NickServ::service->GetAccountMap();
	auto it = map.find(acc);
	if (it != map.end())
		return it->second;
	return nullptr;
}

