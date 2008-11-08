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

int do_update(User * u);
void myOperServHelp(User * u);

class OSUpdate : public Module
{
 public:
	OSUpdate(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		moduleAddAuthor("Anope");
		moduleAddVersion("$Id$");
		moduleSetType(CORE);

		c = createCommand("UPDATE", do_update, is_services_root, OPER_HELP_UPDATE, -1, -1, -1, -1);
		moduleAddCommand(OPERSERV, c, MOD_UNIQUE);

		moduleSetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
    if (is_services_root(u)) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_UPDATE);
    }
}

/**
 * The /os update command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_update(User * u)
{
    notice_lang(s_OperServ, u, OPER_UPDATING);
    save_data = 1;
    return MOD_CONT;
}

MODULE_INIT("os_update", OSUpdate)
