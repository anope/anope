/* HelpServ core functions
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

int do_help(User * u);

class HEHelp : public Module
{
 public:
	HEHelp(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		moduleAddAuthor("Anope");
		moduleAddVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("HELP", do_help, NULL, -1, -1, -1, -1, -1);
		moduleAddCommand(HELPSERV, c, MOD_UNIQUE);
	}
};



/**
 * Display the HelpServ help.
 * This core function has been embed in the source for a long time, but
 * it moved into it's own file so we now all can enjoy the joy of
 * modules for HelpServ.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 */
int do_help(User * u)
{
    char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_HelpServ, u, HELP_HELP, s_NickServ, s_ChanServ,
                    s_MemoServ);
        if (s_BotServ) {
            notice_help(s_HelpServ, u, HELP_HELP_BOT, s_BotServ);
        }
        if (s_HostServ) {
            notice_help(s_HelpServ, u, HELP_HELP_HOST, s_HostServ);
        }
        moduleDisplayHelp(7, u);
    } else {
        mod_help_cmd(s_HelpServ, u, HELPSERV, cmd);
    }
    return MOD_CONT;
}

MODULE_INIT("he_help", HEHelp)
