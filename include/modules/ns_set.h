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
	struct CoreExport SetNickOption : Events
	{
		/** Called when a nickserv/set command is used.
		 * @param source The source of the command
		 * @param cmd The command
		 * @param nc The nickcore being modifed
		 * @param setting The setting passed to the command. Probably ON/OFF.
		 * @return EVENT_STOP to halt immediately
		 */
		virtual EventReturn OnSetNickOption(CommandSource &source, Command *cmd, NickServ::Account *nc, const Anope::string &setting) anope_abstract;
	};
	static EventHandlersReference<SetNickOption> OnSetNickOption("OnSetNickOption");
}
