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

void myOperServHelp(User *u);

class CommandOSQuit : public Command
{
 public:
	CommandOSQuit() : Command("QUIT", 0, 0)
	{
	}

	CommandResult Execute(User *u, std::vector<std::string> &params)
	{
		quitmsg = new char[28 + strlen(u->nick)];
		if (!quitmsg)
			quitmsg = "QUIT command received, but out of memory!";
		else
			sprintf(const_cast<char *>(quitmsg), "QUIT command received from %s", u->nick); // XXX we know this is safe, but..

		if (GlobalOnCycle)
			oper_global(NULL, "%s", GlobalOnCycleMessage);
		quitting = 1;
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_root(u))
			return false;

		notice_lang(s_OperServ, u, OPER_HELP_QUIT);
		return true;
	}
};

class OSQuit : public Module
{
 public:
	OSQuit(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSQuit(), MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	if (is_services_root(u))
		notice_lang(s_OperServ, u, OPER_HELP_CMD_QUIT);
}

MODULE_INIT("os_quit", OSQuit)
