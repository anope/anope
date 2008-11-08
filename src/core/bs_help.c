/* BotServ core functions
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

class BSHelp : public Module
{
 public:
	BSHelp(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		c = createCommand("HELP", do_help, NULL, -1, -1, -1, -1, -1);
		this->AddCommand(BOTSERV, c, MOD_UNIQUE);
	}
};


/**
 * The /bs help command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_help(User * u)
{
    char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_BotServ, u, BOT_HELP);
        moduleDisplayHelp(4, u);
        notice_help(s_BotServ, u, BOT_HELP_FOOTER, BSMinUsers);
    } else {
        mod_help_cmd(s_BotServ, u, BOTSERV, cmd);
    }
    return MOD_CONT;
}

MODULE_INIT("bs_help", BSHelp)
