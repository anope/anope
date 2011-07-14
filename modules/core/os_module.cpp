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

class CommandOSModLoad : public Command
{
 public:
	CommandOSModLoad(Module *creator) : Command(creator, "operserv/modload", 1, 1, "operserv/modload")
	{
		this->SetDesc(_("Load a module"));
		this->SetSyntax(_("\037modname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &mname = params[0];

		ModuleReturn status = ModuleManager::LoadModule(mname, u);
		if (status == MOD_ERR_OK)
		{
			ircdproto->SendGlobops(source.owner, "%s loaded module %s", u->nick.c_str(), mname.c_str());
			source.Reply(_("Module \002%s\002 loaded"), mname.c_str());

			/* If a user is loading this module, then the core databases have already been loaded
			 * so trigger the event manually
			 */
			Module *m = ModuleManager::FindModule(mname);
			if (m)
				m->OnPostLoadDatabases();
		}
		else if (status == MOD_ERR_EXISTS)
			source.Reply(_("Module \002%s\002 is already loaded."), mname.c_str());
		else
			source.Reply(_("Unable to load module \002%s\002"), mname.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command loads the module named FileName from the modules\n"
				"directory."));
		return true;
	}
};

class CommandOSModReLoad : public Command
{
 public:
	CommandOSModReLoad(Module *creator) : Command(creator, "operserv/modreload", 1, 1, "operserv/modload")
	{
		this->SetDesc(_("Reload a module"));
		this->SetSyntax(_("\037modname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &mname = params[0];

		Module *m = ModuleManager::FindModule(mname);
		if (!m)
		{
			source.Reply(_("Module \002%s\002 isn't loaded."), mname.c_str());
			return;
		}

		if (!m->handle || m->GetPermanent())
		{
			source.Reply(_("Unable to remove module \002%s\002"), m->name.c_str());
			return;
		}

		/* Unrecoverable */
		bool fatal = m->type == PROTOCOL;
		ModuleReturn status = ModuleManager::UnloadModule(m, u);

		if (status != MOD_ERR_OK)
		{
			source.Reply(_("Unable to remove module \002%s\002"), mname.c_str());
			return;
		}

		status = ModuleManager::LoadModule(mname, u);
		if (status == MOD_ERR_OK)
		{
			ircdproto->SendGlobops(source.owner, "%s reloaded module %s", u->nick.c_str(), mname.c_str());
			source.Reply(_("Module \002%s\002 reloaded"), mname.c_str());

			/* If a user is loading this module, then the core databases have already been loaded
			 * so trigger the event manually
			 */
			m = ModuleManager::FindModule(mname);
			if (m)
				m->OnPostLoadDatabases();
		}
		else
		{
			if (fatal)
				throw FatalException("Unable to reload module " + mname);
			else
				source.Reply(_("Unable to load module \002%s\002"), mname.c_str());
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command reloads the module named FileName."));
		return true;
	}
};

class CommandOSModUnLoad : public Command
{
 public:
	CommandOSModUnLoad(Module *creator) : Command(creator, "operserv/modunload", 1, 1, "operserv/modload")
	{
		this->SetDesc(_("Un-Load a module"));
		this->SetSyntax(_("\037modname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &mname = params[0];

		Module *m = ModuleManager::FindModule(mname);
		if (!m)
		{
			source.Reply(_("Module \002%s\002 isn't loaded."), mname.c_str());
			return;
		}
		
		if (!m->handle || m->GetPermanent() || m->type == PROTOCOL)
		{
			source.Reply(_("Unable to remove module \002%s\002"), m->name.c_str());
			return;
		}

		Log() << "Trying to unload module [" << mname << "]";

		ModuleReturn status = ModuleManager::UnloadModule(m, u);

		if (status == MOD_ERR_OK)
		{
			source.Reply(_("Module \002%s\002 unloaded"), mname.c_str());
			ircdproto->SendGlobops(source.owner, "%s unloaded module %s", u->nick.c_str(), mname.c_str());
		}
		else
			source.Reply(_("Unable to remove module \002%s\002"), mname.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command unloads the module named FileName from the modules\n"
				"directory."));
		return true;
	}
};

class OSModule : public Module
{
	CommandOSModLoad commandosmodload;
	CommandOSModReLoad commandosmodreload;
	CommandOSModUnLoad commandosmodunload;

 public:
	OSModule(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandosmodload(this), commandosmodreload(this), commandosmodunload(this)
	{
		this->SetAuthor("Anope");
		this->SetPermanent(true);

		ModuleManager::RegisterService(&commandosmodload);
		ModuleManager::RegisterService(&commandosmodreload);
		ModuleManager::RegisterService(&commandosmodunload);
	}
};

MODULE_INIT(OSModule)
