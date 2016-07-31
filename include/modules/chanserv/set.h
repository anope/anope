/*
 * Anope IRC Services
 *
 * Copyright (C) 2014-2016 Anope Team <team@anope.org>
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
	struct CoreExport SetChannelOption : Events
	{
		static constexpr const char *NAME = "setchanneloption";

		using Events::Events;

		/** Called when a chanserv/set command is used
		 * @param source The source of the command
		 * @param cmd The command
		 * @param ci The channel the command was used on
		 * @param setting The setting passed to the command. Probably ON/OFF.
		 * @return EVENT_ALLOW to bypass access checks, EVENT_STOP to halt immediately.
		 */
		virtual EventReturn OnSetChannelOption(CommandSource &source, Command *cmd, ChanServ::Channel *ci, const Anope::string &setting) anope_abstract;
	};
}

