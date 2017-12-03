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
#include "channeltype.h"

ChannelType::ChannelType(Module *me) : Serialize::Type<ChannelImpl>(me)
	, name(this, "name", &ChannelImpl::name)
	, desc(this, "desc", &ChannelImpl::desc)
	, time_registered(this, "time_registered", &ChannelImpl::time_registered)
	, channel_ts(this, "channel_ts", &ChannelImpl::channel_ts)
	, last_used(this, "last_used", &ChannelImpl::last_used)
	, last_topic(this, "last_topic", &ChannelImpl::last_topic)
	, last_topic_setter(this, "last_topic_setter", &ChannelImpl::last_topic_setter)
	, last_topic_time(this, "last_topic_time", &ChannelImpl::last_topic_time)
	, bantype(this, "bantype", &ChannelImpl::bantype)
	, banexpire(this, "banexpire", &ChannelImpl::banexpire)
	, founder(this, "founder", &ChannelImpl::founder)
	, successor(this, "successor", &ChannelImpl::successor)
	, servicebot(this, "servicebot", &ChannelImpl::bi)
	, greet(this, "greet", &ChannelImpl::greet)
	, fantasy(this, "fantasy", &ChannelImpl::fantasy)
	, noautoop(this, "noautoop", &ChannelImpl::noautoop)
	, peace(this, "peace", &ChannelImpl::peace)
	, securefounder(this, "securefounder", &ChannelImpl::securefounder)
	, restricted(this, "restricted", &ChannelImpl::restricted)
	, secureops(this, "secureops", &ChannelImpl::secureops)
	, signkick(this, "signkick", &ChannelImpl::signkick)
	, signkicklevel(this, "signkicklevel", &ChannelImpl::signkicklevel)
	, noexpire(this, "noexpire", &ChannelImpl::noexpire)
	, keepmodes(this, "keepmodes", &ChannelImpl::keepmodes)
	, persist(this, "persist", &ChannelImpl::persist)
	, topiclock(this, "topiclock", &ChannelImpl::topiclock)
	, keeptopic(this, "keeptopic", &ChannelImpl::keeptopic)
	, _private(this, "private", &ChannelImpl::_private)
{

}

void ChannelType::Name::OnSet(ChannelImpl *c, Anope::string *old, const Anope::string &value)
{
	ChanServ::registered_channel_map& map = ChanServ::service->GetChannels();
	if (old != nullptr)
		map.erase(*old);

	map[value] = c;
}

ChanServ::Channel *ChannelType::FindChannel(const Anope::string &chan)
{
	Serialize::ID id;
	EventReturn result = EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeFind, this, &this->name, chan, id);
	if (result == EVENT_ALLOW)
		return RequireID(id);

	// fall back
	ChanServ::registered_channel_map& map = ChanServ::service->GetChannels();
	auto it = map.find(chan);
	if (it != map.end())
		return it->second;
	return nullptr;
}

