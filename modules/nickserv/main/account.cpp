/*
 * Anope IRC Services
 *
 * Copyright (C) 2015-2017 Anope Team <team@anope.org>
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

void AccountImpl::SetEmail(const Anope::string &e)
{
	Set(&AccountType::email, e);
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

void AccountImpl::SetOper(Oper *o)
{
	Set(&AccountType::oper, o);
}

MemoServ::MemoInfo *AccountImpl::GetMemos()
{
	return GetRef<MemoServ::MemoInfo *>();
}

Anope::string AccountImpl::GetGreet()
{
	return Get(&AccountType::greet);
}

void AccountImpl::SetGreet(const Anope::string &g)
{
	Set(&AccountType::greet, g);
}

bool AccountImpl::IsUnconfirmed()
{
	return Get(&AccountType::unconfirmed);
}

void AccountImpl::SetUnconfirmed(bool u)
{
	Set(&AccountType::greet, u);
}

bool AccountImpl::IsPrivate()
{
	return Get(&AccountType::_private);
}

void AccountImpl::SetPrivate(bool p)
{
	Set(&AccountType::_private, p);
}

bool AccountImpl::IsAutoOp()
{
	return Get(&AccountType::autoop);
}

void AccountImpl::SetAutoOp(bool a)
{
	Set(&AccountType::autoop, a);
}

bool AccountImpl::IsKeepModes()
{
	return Get(&AccountType::keepmodes);
}

void AccountImpl::SetKeepModes(bool k)
{
	Set(&AccountType::keepmodes, k);
}

bool AccountImpl::IsKillProtect()
{
	return Get(&AccountType::killprotect);
}

void AccountImpl::SetKillProtect(bool k)
{
	Set(&AccountType::killprotect, k);
}

bool AccountImpl::IsKillQuick()
{
	return Get(&AccountType::killquick);
}

void AccountImpl::SetKillQuick(bool k)
{
	Set(&AccountType::killquick, k);
}

bool AccountImpl::IsKillImmed()
{
	return Get(&AccountType::killimmed);
}

void AccountImpl::SetKillImmed(bool k)
{
	Set(&AccountType::killimmed, k);
}

bool AccountImpl::IsMsg()
{
	return Get(&AccountType::msg);
}

void AccountImpl::SetMsg(bool m)
{
	Set(&AccountType::msg, m);
}

bool AccountImpl::IsMemoSignon()
{
	return Get(&AccountType::memosignon);
}

void AccountImpl::SetMemoSignon(bool m)
{
	Set(&AccountType::memosignon, m);
}

bool AccountImpl::IsMemoReceive()
{
	return Get(&AccountType::memoreceive);
}

void AccountImpl::SetMemoReceive(bool m)
{
	Set(&AccountType::memoreceive, m);
}

bool AccountImpl::IsMemoMail()
{
	return Get(&AccountType::memomail);
}

void AccountImpl::SetMemoMail(bool m)
{
	Set(&AccountType::memomail, m);
}

bool AccountImpl::IsHideEmail()
{
	return Get(&AccountType::hideemail);
}

void AccountImpl::SetHideEmail(bool h)
{
	Set(&AccountType::hideemail, h);
}

bool AccountImpl::IsHideMask()
{
	return Get(&AccountType::hidemask);
}

void AccountImpl::SetHideMask(bool h)
{
	Set(&AccountType::hidemask, h);
}

bool AccountImpl::IsHideStatus()
{
	return Get(&AccountType::hidestatus);
}

void AccountImpl::SetHideStatus(bool h)
{
	Set(&AccountType::hidestatus, h);
}

bool AccountImpl::IsHideQuit()
{
	return Get(&AccountType::hidequit);
}

void AccountImpl::SetHideQuit(bool h)
{
	Set(&AccountType::hidequit, h);
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

unsigned int AccountImpl::GetChannelCount()
{
	unsigned int i = 0;
	for (ChanServ::Channel *c : GetRefs<ChanServ::Channel *>())
		if (c->GetFounder() == this)
			++i;
	return i;
}

time_t AccountImpl::GetLastMail()
{
	return Get(&AccountType::last_mail);
}

void AccountImpl::SetLastMail(time_t l)
{
	Set(&AccountType::last_mail, l);
}
