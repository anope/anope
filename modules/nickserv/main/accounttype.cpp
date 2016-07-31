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

AccountType::AccountType(Module *me) : Serialize::Type<AccountImpl>(me)
	, display(this, "display", &AccountImpl::display)
	, pass(this, "pass", &AccountImpl::password)
	, email(this, "email", &AccountImpl::email)
	, language(this, "language", &AccountImpl::language)
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
	EventReturn result = EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeFind, this, &this->display, acc, id);
	if (result == EVENT_ALLOW)
		return RequireID(id);

	NickServ::nickcore_map &map = NickServ::service->GetAccountMap();
	auto it = map.find(acc);
	if (it != map.end())
		return it->second;
	return nullptr;
}

