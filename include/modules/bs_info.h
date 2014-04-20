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
	struct CoreExport BotInfoEvent : Events
	{
		/** Called when a user uses botserv/info on a bot or channel.
		 */
		virtual void OnBotInfo(CommandSource &source, BotInfo *bi, ChannelInfo *ci, InfoFormatter &info) anope_abstract;
	};
	extern CoreExport EventHandlers<BotInfoEvent> OnBotInfo;
}

