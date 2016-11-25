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

#include "channel.h"

class ChannelType : public Serialize::Type<ChannelImpl>
{
 public:
	/* Channel name */
	struct Name : Serialize::Field<ChannelImpl, Anope::string>
	{
		using Serialize::Field<ChannelImpl, Anope::string>::Field;

		void OnSet(ChannelImpl *c, const Anope::string &value) override;
	} name;
	Serialize::Field<ChannelImpl, Anope::string> desc;
	Serialize::Field<ChannelImpl, time_t> time_registered;
	Serialize::Field<ChannelImpl, time_t> last_used;

	Serialize::Field<ChannelImpl, Anope::string> last_topic;
	Serialize::Field<ChannelImpl, Anope::string> last_topic_setter;
	Serialize::Field<ChannelImpl, time_t> last_topic_time;

	Serialize::Field<ChannelImpl, int16_t> bantype;
	Serialize::Field<ChannelImpl, time_t> banexpire;

	/* Channel founder */
	Serialize::ObjectField<ChannelImpl, NickServ::Account *> founder;
	/* Who gets the channel if the founder nick is dropped or expires */
	Serialize::ObjectField<ChannelImpl, NickServ::Account *> successor;

	Serialize::ObjectField<ChannelImpl, BotInfo *> servicebot;

	Serialize::Field<ChannelImpl, bool> greet, fantasy, noautoop, peace, securefounder,
		restricted, secure, secureops, signkick, signkicklevel, noexpire, keepmodes,
		persist, topiclock, keeptopic, _private;

 	ChannelType(Module *);

	ChanServ::Channel *FindChannel(const Anope::string &);
};
