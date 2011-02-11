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

class CommandOSUpdate : public Command
{
 public:
	CommandOSUpdate() : Command("UPDATE", 0, 0, "operserv/update")
	{
		this->SetDesc("Force the Services databases to be updated immediately");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		source.Reply(_("Updating databases."));
		save_data = true;
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002UPDATE\002\n"
				" \n"
				"Causes Services to update all database files as soon as you\n"
				"send the command."));
		return true;
	}
};

class OSUpdate : public Module
{
	CommandOSUpdate commandosupdate;

 public:
	OSUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosupdate);
	}
};

MODULE_INIT(OSUpdate)
