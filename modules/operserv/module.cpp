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

class CommandOSModLoad : public Command
{
 public:
	CommandOSModLoad(Module *creator) : Command(creator, "operserv/modload", 1, 1)
	{
		this->SetDesc(_("Load a module"));
		this->SetSyntax(_("\037modname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &mname = params[0];

		ModuleReturn status = ModuleManager::LoadModule(mname, source.GetUser());
		if (status == MOD_ERR_OK)
		{
			Log(LOG_ADMIN, source, this) << "to load module " << mname;
			source.Reply(_("Module \002{0}\002 loaded."), mname);
		}
		else if (status == MOD_ERR_EXISTS)
			source.Reply(_("Module \002{0}\002 is already loaded."), mname);
		else
			source.Reply(_("Unable to load module \002{0}\002."), mname);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("This command loads the module named \037modname\037."));
		return true;
	}
};

class CommandOSModReLoad : public Command
{
 public:
	CommandOSModReLoad(Module *creator) : Command(creator, "operserv/modreload", 1, 1)
	{
		this->SetDesc(_("Reload a module"));
		this->SetSyntax(_("\037modname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &mname = params[0];

		Module *m = ModuleManager::FindModule(mname);
		if (!m)
		{
			source.Reply(_("Module \002{0}\002 isn't loaded."), mname);
			return;
		}

		if (!m->handle || m->GetPermanent())
		{
			source.Reply(_("Unable to remove module \002{0}\002."), m->name);
			return;
		}

		Module *protocol = ModuleManager::FindFirstOf(PROTOCOL);
		if (m->type == PROTOCOL && m != protocol)
		{
			source.Reply(_("You can not reload this module directly, instead reload {0}."), protocol ? protocol->name : "(unknown)");
			return;
		}

		/* Unrecoverable */
		bool fatal = m->type == PROTOCOL;
		ModuleReturn status = ModuleManager::UnloadModule(m, source.GetUser());

		if (status != MOD_ERR_OK)
		{
			source.Reply(_("Unable to remove module \002{0}\002."), mname);
			return;
		}

		status = ModuleManager::LoadModule(mname, source.GetUser());
		if (status == MOD_ERR_OK)
		{
			Log(LOG_ADMIN, source, this) << "to reload module " << mname;
			source.Reply(_("Module \002{0}\002 reloaded."), mname);
		}
		else
		{
			if (fatal)
			{
				Anope::QuitReason = "Unable to reload module " + mname;
				Anope::Quitting = true;
			}
			else
				source.Reply(_("Unable to load module \002{0}\002."), mname);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("This command reloads the module named \037modname\037."));
		return true;
	}
};

class CommandOSModUnLoad : public Command
{
 public:
	CommandOSModUnLoad(Module *creator) : Command(creator, "operserv/modunload", 1, 1)
	{
		this->SetDesc(_("Un-Load a module"));
		this->SetSyntax(_("\037modname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &mname = params[0];

		Module *m = ModuleManager::FindModule(mname);
		if (!m)
		{
			source.Reply(_("Module \002{0}\002 isn't loaded."), mname);
			return;
		}

		if (!m->handle || m->GetPermanent() || m->type == PROTOCOL)
		{
			source.Reply(_("Unable to remove module \002{0}\002."), m->name);
			return;
		}

		Log(this->GetOwner()) << "Trying to unload module [" << mname << "]";

		ModuleReturn status = ModuleManager::UnloadModule(m, source.GetUser());

		if (status == MOD_ERR_OK)
		{
			Log(LOG_ADMIN, source, this) << "to unload module " << mname;
			source.Reply(_("Module \002{0}\002 unloaded."), mname);
		}
		else
			source.Reply(_("Unable to remove module \002{0}\002."), mname);

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("This command unloads the module named \037modname\037."));
		return true;
	}
};

class OSModule : public Module
{
	CommandOSModLoad commandosmodload;
	CommandOSModReLoad commandosmodreload;
	CommandOSModUnLoad commandosmodunload;

 public:
	OSModule(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosmodload(this)
		, commandosmodreload(this)
		, commandosmodunload(this)
	{
		this->SetPermanent(true);

	}
};

MODULE_INIT(OSModule)
