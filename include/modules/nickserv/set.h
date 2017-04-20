/*
 * Anope IRC Services
 *
 * Copyright (C) 2014-2017 Anope Team <team@anope.org>
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
	struct CoreExport SetNickOption : Events
	{
		static constexpr const char *NAME = "setnickoption";

		using Events::Events;

		/** Called when a nickserv/set command is used.
		 * @param source The source of the command
		 * @param cmd The command
		 * @param nc The nickcore being modifed
		 * @param setting The setting passed to the command. Probably ON/OFF.
		 * @return EVENT_STOP to halt immediately
		 */
		virtual EventReturn OnSetNickOption(CommandSource &source, Command *cmd, NickServ::Account *nc, const Anope::string &setting) anope_abstract;
	};
}

