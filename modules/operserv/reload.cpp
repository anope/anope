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

class CommandOSReload : public Command
{
 public:
	CommandOSReload(Module *creator) : Command(creator, "operserv/reload", 0, 0)
	{
		this->SetDesc(_("Reload services' configuration file"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		try
		{
			Log(LOG_ADMIN, source, this);

			Configuration::Conf *new_config = new Configuration::Conf();
			Configuration::Conf *old = Config;
			Config = new_config;
			Config->Post(old);
			delete old;

			source.Reply(_("Services' configuration has been reloaded."));
		}
		catch (const ConfigException &ex)
		{
			Log(this->owner) << "Error reloading configuration file: " << ex.GetReason();
			source.Reply(_("Error reloading configuration file: {0}"), ex.GetReason());
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Causes Services to reload the configuration file."));
		return true;
	}
};

class OSReload : public Module
{
	CommandOSReload commandosreload;

 public:
	OSReload(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosreload(this)
	{

	}
};

MODULE_INIT(OSReload)
