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
		/** Called right before a channel is dropped
		 * @param source The user dropping the channel
		 * @param ci The channel
		 */
		virtual EventReturn OnChanDrop(CommandSource &source, ChanServ::Channel *ci) anope_abstract;
	};
}

template<> struct EventName<Event::ChanDrop> { static constexpr const char *const name = "OnChanDrop"; };
