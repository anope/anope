/*
 * Anope IRC Services
 *
 * Copyright (C) 2017 Anope Team <team@anope.org>
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

namespace Event
{
	struct CoreExport VhostRequest : Events
	{
		static constexpr const char *NAME = "vhostrequest";

		using Events::Events;

		virtual EventReturn OnVhostRequest(CommandSource *, NickServ::Account *, const Anope::string &, const Anope::string &) anope_abstract;
	};
}

