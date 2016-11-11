/*
 * Anope IRC Services
 *
 * Copyright (C) 2015-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "accounttype.h"
#include "modules/nickserv/access.h"

AccountImpl::~AccountImpl()
{
	NickServ::nickcore_map& map = NickServ::service->GetAccountMap();
	map.erase(this->GetDisplay());
}

void AccountImpl::Delete()
{
	EventManager::Get()->Dispatch(&Event::DelCore::OnDelCore, this);

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

Oper *AccountImpl::GetOper()
{
	return Get(&AccountType::oper);
}

void AccountImpl::SetOper(Oper *oper)
{
	Set(&AccountType::oper, oper);
}

MemoServ::MemoInfo *AccountImpl::GetMemos()
{
	return GetRef<MemoServ::MemoInfo *>();
}

void AccountImpl::SetDisplay(NickServ::Nick *na)
{
	if (na->GetAccount() != this || na->GetNick() == this->GetDisplay())
		return;

	EventManager::Get()->Dispatch(&Event::ChangeCoreDisplay::OnChangeCoreDisplay, this, na->GetNick());

	NickServ::nickcore_map& map = NickServ::service->GetAccountMap();

	/* Remove the core from the list */
	map.erase(this->GetDisplay());

	this->SetDisplay(na->GetNick());

	NickServ::Account* &nc = map[this->GetDisplay()];
	if (nc)
		Log(LOG_DEBUG) << "Duplicate account " << this->GetDisplay() << " in nickcore table?";

	nc = this;
}

bool AccountImpl::IsOnAccess(User *u)
{
	Anope::string buf = u->GetIdent() + "@" + u->host, buf2, buf3;
	if (!u->vhost.empty())
		buf2 = u->GetIdent() + "@" + u->vhost;
	if (!u->GetCloakedHost().empty())
		buf3 = u->GetIdent() + "@" + u->GetCloakedHost();

	for (NickAccess *access : GetRefs<NickAccess *>())
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
	for (ChanServ::Channel *c : GetRefs<ChanServ::Channel *>())
		if (c->GetFounder() == this)
			++i;
	return i;
}

