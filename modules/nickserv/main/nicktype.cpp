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
#include "nicktype.h"

NickType::NickType(Module *me) : Serialize::Type<NickImpl>(me)
	, nick(this, "nick", &NickImpl::nick)
	, last_quit(this, "last_quit", &NickImpl::last_quit)
	, last_realname(this, "last_realname", &NickImpl::last_realname)
	, last_usermask(this, "last_usermask", &NickImpl::last_usermask)
	, last_realhost(this, "last_realhost", &NickImpl::last_realhost)
	, time_registered(this, "time_registered", &NickImpl::time_registered)
	, last_seen(this, "last_seen", &NickImpl::last_seen)
	, nc(this, "nc", &NickImpl::account, true)
{

}

void NickType::Nick::OnSet(NickImpl *na, const Anope::string &value)
{
	/* Remove us from the aliases list */
	NickServ::nickalias_map &map = NickServ::service->GetNickMap();
	Anope::string *old = this->Get_(na);
	if (old != nullptr)
		map.erase(*old);

	map[value] = na;
}

NickServ::Nick *NickType::FindNick(const Anope::string &n)
{
	Serialize::ID id;
	EventReturn result = EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeFind, this, &this->nick, n, id);
	if (result == EVENT_ALLOW)
		return RequireID(id);

	NickServ::nickalias_map &map = NickServ::service->GetNickMap();
	auto it = map.find(n);
	if (it != map.end())
		return it->second;
	return nullptr;
}

