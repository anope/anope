/*
 * Anope IRC Services
 *
 * Copyright (C) 2016 Anope Team <team@anope.org>
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
#include "modules/protocol/ts6.h"

static inline char nextID(int pos, Anope::string &buf)
{
	char &c = buf[pos];
	if (c == 'Z')
		c = '0';
	else if (c != '9')
		++c;
	else if (pos)
		c = 'A';
	else
		c = '0';
	return c;
}

Anope::string ts6::Proto::UID_Retrieve()
{
	if (!IRCD || !IRCD->RequiresID)
		return "";

	static Anope::string current_uid = "AAAAAA";
	int current_len = current_uid.length() - 1;

	do
	{
		while (current_len >= 0 && nextID(current_len--, current_uid) == 'A');
	}
	while (User::Find(Me->GetSID() + current_uid) != nullptr);

	return Me->GetSID() + current_uid;
}

Anope::string ts6::Proto::SID_Retrieve()
{
	if (!IRCD || !IRCD->RequiresID)
		return "";

	static Anope::string current_sid = Config->GetBlock("serverinfo")->Get<Anope::string>("id");
	if (current_sid.empty())
		current_sid = "00A";

	do
	{
		int current_len = current_sid.length() - 1;
		while (current_len >= 0 && nextID(current_len--, current_sid) == 'A');
	}
	while (Server::Find(current_sid) != nullptr);

	return current_sid;
}
