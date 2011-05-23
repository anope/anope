/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "operserv.h"

class CommandOSRestart : public Command
{
 public:
	CommandOSRestart() : Command("RESTART", 0, 0, "operserv/restart")
	{
		this->SetDesc(_("Save databases and restart Services"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		quitmsg = "RESTART command received from " + u->nick;
		save_databases();
		quitting = restarting = true;
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002RESTART\002\n"
				" \n"
				"Causes Services to save all databases and then restart\n"
				"(i.e. exit and immediately re-run the executable)."));
		return true;
	}
};

class OSRestart : public Module
{
	CommandOSRestart commandosrestart;

 public:
	OSRestart(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->AddCommand(operserv->Bot(), &commandosrestart);
	}
};

MODULE_INIT(OSRestart)
