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
	struct CoreExport BotCreate : Events
	{
		static constexpr const char *NAME = "botcreate";

		using Events::Events;
		
		/** Called when a new bot is made
		 * @param bi The bot
		 */
		virtual void OnBotCreate(ServiceBot *bi) anope_abstract;
	};

	struct CoreExport BotChange : Events
	{
		static constexpr const char *NAME = "botchange";

		using Events::Events;
		
		/** Called when a bot is changed
		 * @param bi The bot
		 */
		virtual void OnBotChange(ServiceBot *bi) anope_abstract;
	};

	struct CoreExport BotDelete : Events
	{
		static constexpr const char *NAME = "botdelete";

		using Events::Events;
		
		/** Called when a bot is deleted
		 * @param bi The bot
		 */
		virtual void OnBotDelete(ServiceBot *bi) anope_abstract;
	};
}

