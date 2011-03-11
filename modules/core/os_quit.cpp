/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class CommandOSQuit : public Command
{
 public:
	CommandOSQuit() : Command("QUIT", 0, 0, "operserv/quit")
	{
		this->SetDesc(_("Terminate the Services program with no save"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		quitmsg = "QUIT command received from " + u->nick;

		if (Config->GlobalOnCycle)
			oper_global("", "%s", Config->GlobalOnCycleMessage.c_str());
		quitting = true;
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002QUIT\002\n"
				" \n"
				"Causes Services to do an immediate shutdown; databases are\n"
				"\002not\002 saved.  This command should not be used unless\n"
				"damage to the in-memory copies of the databases is feared\n"
				"and they should not be saved.  For normal shutdowns, use the\n"
				"\002SHUTDOWN\002 command."));
		return true;
	}
};

class OSQuit : public Module
{
	CommandOSQuit commandosquit;

 public:
	OSQuit(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosquit);
	}
};

MODULE_INIT(OSQuit)
