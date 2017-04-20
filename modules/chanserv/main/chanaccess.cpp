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
#include "modules/chanserv/main/chanaccess.h"
#include "chanaccesstype.h"

ChanServ::Channel *ChanAccessImpl::GetChannel()
{
	return Get(&ChanAccessType<ChanServ::ChanAccess>::channel);
}

void ChanAccessImpl::SetChannel(ChanServ::Channel *channel)
{
	Object::Set(&ChanAccessType<ChanServ::ChanAccess>::channel, channel);
}

Anope::string ChanAccessImpl::GetCreator()
{
	return Get(&ChanAccessType<ChanServ::ChanAccess>::creator);
}

void ChanAccessImpl::SetCreator(const Anope::string &c)
{
	Object::Set(&ChanAccessType<ChanServ::ChanAccess>::creator, c);
}

time_t ChanAccessImpl::GetLastSeen()
{
	return Get(&ChanAccessType<ChanServ::ChanAccess>::last_seen);
}

void ChanAccessImpl::SetLastSeen(const time_t &t)
{
	Object::Set(&ChanAccessType<ChanServ::ChanAccess>::last_seen, t);
}

time_t ChanAccessImpl::GetCreated()
{
	return Get(&ChanAccessType<ChanServ::ChanAccess>::created);
}

void ChanAccessImpl::SetCreated(const time_t &t)
{
	Object::Set(&ChanAccessType<ChanServ::ChanAccess>::created, t);
}

Anope::string ChanAccessImpl::GetMask()
{
	return Get(&ChanAccessType<ChanServ::ChanAccess>::mask);
}

void ChanAccessImpl::SetMask(const Anope::string &n)
{
	Object::Set(&ChanAccessType<ChanServ::ChanAccess>::mask, n);
}

NickServ::Account *ChanAccessImpl::GetAccount()
{
	return Get(&ChanAccessType<ChanServ::ChanAccess>::account);
}

void ChanAccessImpl::SetAccount(NickServ::Account *acc)
{
	Object::Set(&ChanAccessType<ChanServ::ChanAccess>::account, acc);
}

Anope::string ChanAccessImpl::Mask()
{
	if (NickServ::Account *acc = GetAccount())
		return acc->GetDisplay();

	return GetMask();
}

bool ChanAccessImpl::Matches(const User *u, NickServ::Account *acc)
{
	if (this->GetAccount())
		return this->GetAccount() == acc;

	if (u)
	{
		bool is_mask = this->Mask().find_first_of("!@?*") != Anope::string::npos;
		if (is_mask && Anope::Match(u->nick, this->Mask()))
			return true;
		else if (Anope::Match(u->GetDisplayedMask(), this->Mask()))
			return true;
	}

	if (acc)
		for (NickServ::Nick *na : acc->GetRefs<NickServ::Nick *>())
			if (Anope::Match(na->GetNick(), this->Mask()))
				return true;

	return false;
}

int ChanAccessImpl::Compare(ChanAccess *other)
{
	const std::vector<ChanServ::Privilege> &privs = ChanServ::service->GetPrivileges();
	for (unsigned int i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other->HasPriv(privs[i - 1].name);

		if (!this_p && !other_p)
			continue;

		if (this_p && !other_p)
			return 1;
		else if (!this_p && other_p)
			return -1;
	}

	return 0;
}
