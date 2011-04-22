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

class CommandOSReload : public Command
{
 public:
	CommandOSReload() : Command("RELOAD", 0, 0, "operserv/reload")
	{
		this->SetDesc(_("Reload services' configuration file"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ServerConfig *old_config = Config;

		try
		{
			Config = new ServerConfig();
			FOREACH_MOD(I_OnReload, OnReload());
			delete old_config;
			source.Reply(_("Services' configuration file has been reloaded."));
		}
		catch (const ConfigException &ex)
		{
			Config = old_config;
			Log() << "Error reloading configuration file: " << ex.GetReason();
			source.Reply(_("Error reloading confguration file: ") + ex.GetReason());
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002RELOAD\002\n"
				"Causes Services to reload the configuration file. Note that\n"
				"some directives still need the restart of the Services to\n"
				"take effect (such as Services' nicknames, activation of the \n"
				"session limitation, etc.)"));
		return true;
	}
};

class OSReload : public Module
{
	CommandOSReload commandosreload;

 public:
	OSReload(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->AddCommand(operserv->Bot(), &commandosreload);
	}
};

MODULE_INIT(OSReload)
