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
	struct CoreExport ChanDrop : Events
	{
		static constexpr const char *NAME = "chandrop";

		using Events::Events;

		/** Called right before a channel is dropped
		 * @param source The user dropping the channel
		 * @param ci The channel
		 */
		virtual EventReturn OnChanDrop(CommandSource &source, ChanServ::Channel *ci) anope_abstract;
	};
}

