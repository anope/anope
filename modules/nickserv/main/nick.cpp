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
#include "nicktype.h"

NickImpl::~NickImpl()
{
	/* Remove us from the aliases list */
	NickServ::nickalias_map &map = NickServ::service->GetNickMap();
	map.erase(GetNick());
}

void NickImpl::Delete()
{
	Event::OnDelNick(&Event::DelNick::OnDelNick, this);

	if (this->GetAccount())
	{
		/* Next: see if our core is still useful. */
		std::vector<NickServ::Nick *> aliases = this->GetAccount()->GetRefs<NickServ::Nick *>(NickServ::nick);

		auto it = std::find(aliases.begin(), aliases.end(), this);
		if (it != aliases.end())
			aliases.erase(it);

		if (aliases.empty())
		{
			/* just me */
			this->GetAccount()->Delete();
		}
		else
		{
			/* Display updating stuff */
			if (GetNick().equals_ci(this->GetAccount()->GetDisplay()))
				this->GetAccount()->SetDisplay(aliases[0]);
		}
	}

	return Serialize::Object::Delete();
}

Anope::string NickImpl::GetNick()
{
	return Get<Anope::string>(&NickType::nick);
}

void NickImpl::SetNick(const Anope::string &nick)
{
	Set(&NickType::nick, nick);
}

Anope::string NickImpl::GetLastQuit()
{
	return Get(&NickType::last_quit);
}

void NickImpl::SetLastQuit(const Anope::string &lq)
{
	Set(&NickType::last_quit, lq);
}

Anope::string NickImpl::GetLastRealname()
{
	return Get(&NickType::last_realname);
}

void NickImpl::SetLastRealname(const Anope::string &lr)
{
	Set(&NickType::last_realname, lr);
}

Anope::string NickImpl::GetLastUsermask()
{
	return Get(&NickType::last_usermask);
}

void NickImpl::SetLastUsermask(const Anope::string &lu)
{
	Set(&NickType::last_usermask, lu);
}

Anope::string NickImpl::GetLastRealhost()
{
	return Get(&NickType::last_realhost);
}

void NickImpl::SetLastRealhost(const Anope::string &lr)
{
	Set(&NickType::last_realhost, lr);
}

time_t NickImpl::GetTimeRegistered()
{
	return Get(&NickType::time_registered);
}

void NickImpl::SetTimeRegistered(const time_t &tr)
{
	Set(&NickType::time_registered, tr);
}

time_t NickImpl::GetLastSeen()
{
	return Get(&NickType::last_seen);
}

void NickImpl::SetLastSeen(const time_t &ls)
{
	Set(&NickType::last_seen, ls);
}

NickServ::Account *NickImpl::GetAccount()
{
	return Get(&NickType::nc);
}

void NickImpl::SetAccount(NickServ::Account *acc)
{
	Set(&NickType::nc, acc);
}

void NickImpl::SetVhost(const Anope::string &ident, const Anope::string &host, const Anope::string &creator, time_t created)
{
	Set(&NickType::vhost_ident, ident);
	Set(&NickType::vhost_host, host);
	Set(&NickType::vhost_creator, creator);
	Set(&NickType::vhost_created, created);
}

void NickImpl::RemoveVhost()
{
	Anope::string e;
	Set(&NickType::vhost_ident, e);
	Set(&NickType::vhost_host, e);
	Set(&NickType::vhost_creator, e);
	Set(&NickType::vhost_created, 0);
}

bool NickImpl::HasVhost()
{
	return !GetVhostHost().empty();
}

Anope::string NickImpl::GetVhostIdent()
{
	return Get(&NickType::vhost_ident);
}

void NickImpl::SetVhostIdent(const Anope::string &i)
{
	Set(&NickType::vhost_ident, i);
}

Anope::string NickImpl::GetVhostHost()
{
	return Get(&NickType::vhost_host);
}

void NickImpl::SetVhostHost(const Anope::string &h)
{
	Set(&NickType::vhost_host, h);
}

Anope::string NickImpl::GetVhostCreator()
{
	return Get(&NickType::vhost_creator);
}

void NickImpl::SetVhostCreator(const Anope::string &c)
{
	Set(&NickType::vhost_creator, c);
}

time_t NickImpl::GetVhostCreated()
{
	return Get(&NickType::vhost_created);
}

void NickImpl::SetVhostCreated(const time_t &cr)
{
	Set(&NickType::vhost_created, cr);
}
