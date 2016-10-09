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

#include "nick.h"

class NickType : public Serialize::Type<NickImpl>
{
 public:
	struct Nick : Serialize::Field<NickImpl, Anope::string>
	{
		using Serialize::Field<NickImpl, Anope::string>::Field;

		void SetField(NickImpl *s, const Anope::string &value) override;
	} nick;
	Serialize::Field<NickImpl, Anope::string> last_quit;
	Serialize::Field<NickImpl, Anope::string> last_realname;
	/* Last usermask this nick was seen on, eg user@host */
	Serialize::Field<NickImpl, Anope::string> last_usermask;
	/* Last uncloaked usermask, requires nickserv/auspex to see */
	Serialize::Field<NickImpl, Anope::string> last_realhost;
	Serialize::Field<NickImpl, time_t> time_registered;
	Serialize::Field<NickImpl, time_t> last_seen;

	/* Account this nick is tied to. Multiple nicks can be tied to a single account. */
	Serialize::ObjectField<NickImpl, NickServ::Account *> nc;

	Serialize::ObjectField<NickImpl, HostServ::VHost *> vhost;

 	NickType(Module *);

	NickServ::Nick *FindNick(const Anope::string &nick);
};
