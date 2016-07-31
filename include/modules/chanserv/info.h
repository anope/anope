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
	struct CoreExport ChanInfo : Events
	{
		static constexpr const char *NAME = "chaninfo";

		using Events::Events;

		/** Called when a user requests info for a channel
		 * @param source The user requesting info
		 * @param ci The channel the user is requesting info for
		 * @param info Data to show the user requesting information
		 * @param show_hidden true if we should show the user everything
		 */
		virtual void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool show_hidden) anope_abstract;
	};
}

