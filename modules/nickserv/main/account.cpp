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

bool AccountImpl::CanGC()
{
	return users.empty();
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

Anope::string AccountImpl::GetGreet()
{
	return Get(&AccountType::greet);
}

void AccountImpl::SetGreet(const Anope::string &greet)
{
	Set(&AccountType::greet, greet);
}

bool AccountImpl::IsUnconfirmed()
{
	return Get(&AccountType::unconfirmed);
}

void AccountImpl::SetUnconfirmed(bool unconfirmed)
{
	Set(&AccountType::greet, unconfirmed);
}

bool AccountImpl::IsPrivate()
{
	return Get(&AccountType::_private);
}

void AccountImpl::SetPrivate(bool _private)
{
	Set(&AccountType::_private, _private);
}

bool AccountImpl::IsAutoOp()
{
	return Get(&AccountType::autoop);
}

void AccountImpl::SetAutoOp(bool autoop)
{
	Set(&AccountType::autoop, autoop);
}

bool AccountImpl::IsKeepModes()
{
	return Get(&AccountType::keepmodes);
}

void AccountImpl::SetKeepModes(bool keepmodes)
{
	Set(&AccountType::keepmodes, keepmodes);
}

bool AccountImpl::IsKillProtect()
{
	return Get(&AccountType::killprotect);
}

void AccountImpl::SetKillProtect(bool killprotect)
{
	Set(&AccountType::killprotect, killprotect);
}

bool AccountImpl::IsKillQuick()
{
	return Get(&AccountType::killquick);
}

void AccountImpl::SetKillQuick(bool killquick)
{
	Set(&AccountType::killquick, killquick);
}

bool AccountImpl::IsKillImmed()
{
	return Get(&AccountType::killimmed);
}

void AccountImpl::SetKillImmed(bool killimmed)
{
	Set(&AccountType::killimmed, killimmed);
}

bool AccountImpl::IsMsg()
{
	return Get(&AccountType::msg);
}

void AccountImpl::SetMsg(bool msg)
{
	Set(&AccountType::msg, msg);
}

bool AccountImpl::IsSecure()
{
	return Get(&AccountType::secure);
}

void AccountImpl::SetSecure(bool secure)
{
	Set(&AccountType::secure, secure);
}

bool AccountImpl::IsMemoSignon()
{
	return Get(&AccountType::memosignon);
}

void AccountImpl::SetMemoSignon(bool memosignon)
{
	Set(&AccountType::memosignon, memosignon);
}

bool AccountImpl::IsMemoReceive()
{
	return Get(&AccountType::memoreceive);
}

void AccountImpl::SetMemoReceive(bool memoreceive)
{
	Set(&AccountType::memoreceive, memoreceive);
}

bool AccountImpl::IsMemoMail()
{
	return Get(&AccountType::memomail);
}

void AccountImpl::SetMemoMail(bool memomail)
{
	Set(&AccountType::memomail, memomail);
}

bool AccountImpl::IsHideEmail()
{
	return Get(&AccountType::hideemail);
}

void AccountImpl::SetHideEmail(bool hideemail)
{
	Set(&AccountType::hideemail, hideemail);
}

bool AccountImpl::IsHideMask()
{
	return Get(&AccountType::hidemask);
}

void AccountImpl::SetHideMask(bool hidemask)
{
	Set(&AccountType::hidemask, hidemask);
}

bool AccountImpl::IsHideStatus()
{
	return Get(&AccountType::hidestatus);
}

void AccountImpl::SetHideStatus(bool hidestatus)
{
	Set(&AccountType::hidestatus, hidestatus);
}

bool AccountImpl::IsHideQuit()
{
	return Get(&AccountType::hidequit);
}

void AccountImpl::SetHideQuit(bool hidequit)
{
	Set(&AccountType::hidequit, hidequit);
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
		Anope::Logger.Debug("Duplicate account {0} in nickcore table?", this->GetDisplay());

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

