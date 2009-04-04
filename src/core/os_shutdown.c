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

class CommandOSShutdown : public Command
{
 public:
	CommandOSShutdown() : Command("SHUTDOWN", 0, 0)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		if (!u->nc->HasCommand("operserv/shutdown")) {
			notice_lang(s_OperServ, u, PERMISSION_DENIED);
			return MOD_STOP;
		}

		quitmsg = new char[32 + strlen(u->nick)];
		if (!quitmsg)
			quitmsg = "SHUTDOWN command received, but out of memory!";
		else
			sprintf(const_cast<char *>(quitmsg), /* XXX */ "SHUTDOWN command received from %s", u->nick);

		if (GlobalOnCycle)
			oper_global(NULL, "%s", GlobalOnCycleMessage);
		save_data = 1;
		delayed_quit = 1;
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!u->nc->HasCommand("operserv/shutdown"))
			return false;

		notice_help(s_OperServ, u, OPER_HELP_SHUTDOWN);
		return true;
	}
};

class OSShutdown : public Module
{
 public:
	OSShutdown(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSShutdown(), MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	notice_lang(s_OperServ, u, OPER_HELP_CMD_SHUTDOWN);
}

MODULE_INIT("os_shutdown", OSShutdown)
