/* OperServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
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

void myOperServHelp(User * u);

class OSSession : public Module
{
 public:
	OSSession(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		/**
		* do_session/do_exception are exported from sessions.c - we just want to provide an interface.
		**/
		c = createCommand("SESSION", do_session, is_services_oper, OPER_HELP_SESSION, -1, -1, -1, -1);
		this->AddCommand(OPERSERV, c, MOD_UNIQUE);
		c = createCommand("EXCEPTION", do_exception, is_services_oper, OPER_HELP_EXCEPTION, -1, -1, -1, -1);
		this->AddCommand(OPERSERV, c, MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
	if (is_services_oper(u)) {
		notice_lang(s_OperServ, u, OPER_HELP_CMD_SESSION);
		notice_lang(s_OperServ, u, OPER_HELP_CMD_EXCEPTION);
	}
}

MODULE_INIT("os_session", OSSession)
