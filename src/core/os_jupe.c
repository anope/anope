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

int do_jupe(User * u);
void myOperServHelp(User * u);

class OSJupe : public Module
{
 public:
	OSJupe(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("JUPE", do_jupe, is_services_admin, OPER_HELP_JUPE, -1, -1, -1, -1);
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
    if (is_services_admin(u)) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_JUPE);
    }
}

/**
 * The /os jupe command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_jupe(User * u)
{
    char *jserver = strtok(NULL, " ");
    char *reason = strtok(NULL, "");

    if (!jserver) {
        syntax_error(s_OperServ, u, "JUPE", OPER_JUPE_SYNTAX);
    } else {
        if (!isValidHost(jserver, 3)) {
            notice_lang(s_OperServ, u, OPER_JUPE_HOST_ERROR);
        } else {
			char rbuf[256];
			snprintf(rbuf, sizeof(rbuf), "Juped by %s%s%s", u->nick, reason ? ": " : "", reason ? reason : "");
			if (findserver(servlist, jserver)) ircdproto->SendSquit(jserver, rbuf);
			ircdproto->SendServer(jserver, 2, rbuf);
			new_server(me_server, jserver, rbuf, SERVER_JUPED, NULL);

            if (WallOSJupe)
                ircdproto->SendGlobops(s_OperServ, "\2%s\2 used JUPE on \2%s\2",
                                 u->nick, jserver);
        }
    }
    return MOD_CONT;
}

MODULE_INIT("os_jupe", OSJupe)
