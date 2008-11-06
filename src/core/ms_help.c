/* MemoServ core functions
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

class MSHelp : public Module
{
 public:
	MSHelp(const std::string &creator) : Module(creator)
	{
		Command *c;

		moduleAddAuthor("Anope");
		moduleAddVersion("$Id$");
		moduleSetType(CORE);
		c = createCommand("HELP", do_help, NULL, -1, -1, -1, -1, -1);
		moduleAddCommand(MEMOSERV, c, MOD_UNIQUE);
	}
};




/**
 * The /ms help command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_help(User * u)
{
    char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_MemoServ, u, MEMO_HELP_HEADER);
        moduleDisplayHelp(3, u);
        notice_help(s_MemoServ, u, MEMO_HELP_FOOTER, s_ChanServ);
    } else {
        mod_help_cmd(s_MemoServ, u, MEMOSERV, cmd);
    }
    return MOD_CONT;
}

MODULE_INIT("ms_help", MSHelp)
