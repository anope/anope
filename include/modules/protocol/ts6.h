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

#pragma once

namespace ts6
{

class Proto : public IRCDProto
{
 public:
	Proto(Module *creator, const Anope::string &proto_name) : IRCDProto(creator, proto_name)
	{
	}

	/* Retrieves the next free UID or SID */
	Anope::string UID_Retrieve() override;
	Anope::string SID_Retrieve() override;
};

} // namespace ts6
