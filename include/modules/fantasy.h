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
	struct BotFantasy : Events
	{
		static constexpr const char *NAME = "botfantasy";

		using Events::Events;

		/** Called on fantasy command
		 * @param source The source of the command
		 * @param c The command
		 * @param ci The channel it's being used in
		 * @param params The params
		 * @return EVENT_STOP to halt processing and not run the command, EVENT_ALLOW to allow the command to be executed
		 */
		virtual EventReturn OnBotFantasy(CommandSource &source, Command *c, ChanServ::Channel *ci, const std::vector<Anope::string> &params) anope_abstract;
	};

	struct CoreExport BotNoFantasyAccess : Events
	{
		static constexpr const char *NAME = "botnofantasyaccess";

		using Events::Events;

		/** Called on fantasy command without access
		 * @param source The source of the command
		 * @param c The command
		 * @param ci The channel it's being used in
		 * @param params The params
		 * @return EVENT_STOP to halt processing and not run the command, EVENT_ALLOW to allow the command to be executed
		 */
		virtual EventReturn OnBotNoFantasyAccess(CommandSource &source, Command *c, ChanServ::Channel *ci, const std::vector<Anope::string> &params) anope_abstract;
	};
}

