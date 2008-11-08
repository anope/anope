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

int do_reload(User * u);
void myOperServHelp(User * u);

class OSReload : public Module
{
 public:
	OSReload(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		moduleAddAuthor("Anope");
		moduleAddVersion("$Id$");
		moduleSetType(this, CORE);

		c = createCommand("RELOAD", do_reload, is_services_root, OPER_HELP_RELOAD, -1, -1, -1, -1);
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
        notice_lang(s_OperServ, u, OPER_HELP_CMD_RELOAD);
    }
}

/**
 * The /os relaod command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_reload(User * u)
{
    if (!read_config(1)) {
        quitmsg = (char *)calloc(28 + strlen(u->nick), 1);
        if (!quitmsg)
            quitmsg =
                "Error during the reload of the configuration file, but out of memory!";
        else
            sprintf((char *)quitmsg, /* XXX */
                    "Error during the reload of the configuration file!");
        quitting = 1;
    }
    send_event(EVENT_RELOAD, 1, EVENT_START);
    notice_lang(s_OperServ, u, OPER_RELOAD);
    return MOD_CONT;
}

MODULE_INIT("os_reload", OSReload)
