/* OperServ core functions
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

class OSHelp : public Module
{
 public:
	OSHelp(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		c = createCommand("HELP", do_help, NULL, -1, -1, -1, -1, -1);
		this->AddCommand(OPERSERV, c, MOD_UNIQUE);
	}
};



/**
 * The /os help command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_help(User * u)
{
    const char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_OperServ, u, OPER_HELP);
        moduleDisplayHelp(5, u);
        notice_help(s_OperServ, u, OPER_HELP_LOGGED);
    } else {
        mod_help_cmd(s_OperServ, u, OPERSERV, cmd);
    }
    return MOD_CONT;
}

MODULE_INIT("os_help", OSHelp)
