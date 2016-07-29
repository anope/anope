/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

namespace Event
{
	struct CoreExport ServiceBotEvent : Events
	{
		static constexpr const char *NAME = "servicebotevent";

		using Events::Events;

		/** Called when a user uses botserv/info on a bot or channel.
		 */
		virtual void OnServiceBot(CommandSource &source, ServiceBot *bi, ChanServ::Channel *ci, InfoFormatter &info) anope_abstract;
	};
}

