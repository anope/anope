/* OperServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandOSUpdate : public Command
{
 public:
	CommandOSUpdate(Module *creator) : Command(creator, "operserv/update", 0, 0)
	{
		this->SetDesc(_("Force the Services databases to be updated immediately"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Log(LOG_ADMIN, source, this);
		source.Reply(_("Updating databases."));
		Anope::SaveDatabases();
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Causes Services to update all database files."));
		return true;
	}
};

class OSUpdate : public Module
{
	CommandOSUpdate commandosupdate;

 public:
	OSUpdate(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosupdate(this)
	{

	}
};

MODULE_INIT(OSUpdate)
