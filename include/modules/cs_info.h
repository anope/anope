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
	struct CoreExport ChanInfo : Events
	{
		/** Called when a user requests info for a channel
		 * @param source The user requesting info
		 * @param ci The channel the user is requesting info for
		 * @param info Data to show the user requesting information
		 * @param show_hidden true if we should show the user everything
		 */
		virtual void OnChanInfo(CommandSource &source, ChannelInfo *ci, InfoFormatter &info, bool show_hidden) anope_abstract;
	};
}

