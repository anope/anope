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
	struct CoreExport SetChannelOption : Events
	{
		/** Called when a chanserv/set command is used
		 * @param source The source of the command
		 * @param cmd The command
		 * @param ci The channel the command was used on
		 * @param setting The setting passed to the command. Probably ON/OFF.
		 * @return EVENT_ALLOW to bypass access checks, EVENT_STOP to halt immediately.
		 */
		virtual EventReturn OnSetChannelOption(CommandSource &source, Command *cmd, ChannelInfo *ci, const Anope::string &setting) anope_abstract;
	};
	static EventHandlersReference<SetChannelOption> OnSetChannelOption("OnSetChannelOption");
}

