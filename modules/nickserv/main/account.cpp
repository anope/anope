/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#include "module.h"
#include "accounttype.h"
#include "modules/ns_access.h"

AccountImpl::~AccountImpl()
{
	NickServ::nickcore_map& map = NickServ::service->GetAccountMap();
	map.erase(this->GetDisplay());
}

void AccountImpl::Delete()
{
	Event::OnDelCore(&Event::DelCore::OnDelCore, this);

	for (unsigned i = users.size(); i > 0; --i)
		users[i - 1]->Logout();

	return Serialize::Object::Delete();
}

Anope::string AccountImpl::GetDisplay()
{
	return Get<Anope::string>(&AccountType::display);
}

void AccountImpl::SetDisplay(const Anope::string &disp)
{
	Set(&AccountType::display, disp);
}

Anope::string AccountImpl::GetPassword()
{
	return Get(&AccountType::pass);
}

void AccountImpl::SetPassword(const Anope::string &pass)
{
	Set(&AccountType::pass, pass);
}

Anope::string AccountImpl::GetEmail()
{
	return Get(&AccountType::email);
}

void AccountImpl::SetEmail(const Anope::string &email)
{
	Set(&AccountType::email, email);
}

Anope::string AccountImpl::GetLanguage()
{
	return Get(&AccountType::language);
}

void AccountImpl::SetLanguage(const Anope::string &lang)
{
	Set(&AccountType::language, lang);
}

MemoServ::MemoInfo *AccountImpl::GetMemos()
{
	return GetRef<MemoServ::MemoInfo *>(MemoServ::memoinfo);
}

void AccountImpl::SetDisplay(NickServ::Nick *na)
{
	if (na->GetAccount() != this || na->GetNick() == this->GetDisplay())
		return;

	Event::OnChangeCoreDisplay(&Event::ChangeCoreDisplay::OnChangeCoreDisplay, this, na->GetNick());

	NickServ::nickcore_map& map = NickServ::service->GetAccountMap();

	/* Remove the core from the list */
	map.erase(this->GetDisplay());

	this->SetDisplay(na->GetNick());

	NickServ::Account* &nc = map[this->GetDisplay()];
	if (nc)
		Log(LOG_DEBUG) << "Duplicate account " << this->GetDisplay() << " in nickcore table?";

	nc = this;
}

bool AccountImpl::IsServicesOper() const
{
	return this->o != NULL;
}

bool AccountImpl::IsOnAccess(User *u)
{
	Anope::string buf = u->GetIdent() + "@" + u->host, buf2, buf3;
	if (!u->vhost.empty())
		buf2 = u->GetIdent() + "@" + u->vhost;
	if (!u->GetCloakedHost().empty())
		buf3 = u->GetIdent() + "@" + u->GetCloakedHost();

	for (NickAccess *access : GetRefs<NickAccess *>(nsaccess))
	{
		Anope::string a = access->GetMask();
		if (Anope::Match(buf, a) || (!buf2.empty() && Anope::Match(buf2, a)) || (!buf3.empty() && Anope::Match(buf3, a)))
			return true;
	}
	return false;
}

unsigned int AccountImpl::GetChannelCount()
{
	unsigned int i = 0;
	for (ChanServ::Channel *c : GetRefs<ChanServ::Channel *>(ChanServ::channel))
		if (c->GetFounder() == this)
			++i;
	return i;
}

