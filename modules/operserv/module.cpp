/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
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
			logger.Command(LogType::ADMIN, source, _("{source} used {command} to load module {0}"), mname);
			source.Reply(_("Module \002{0}\002 loaded."), mname);
		}
		else if (status == MOD_ERR_EXISTS)
		{
			source.Reply(_("Module \002{0}\002 is already loaded."), mname);
		}
		else
		{
			source.Reply(_("Unable to load module \002{0}\002."), mname);
		}
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
			logger.Command(LogType::ADMIN, source, _("{source} used {command} to reload module {0}"), mname);
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
			{
				source.Reply(_("Unable to load module \002{0}\002."), mname);
			}
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

		ModuleReturn status = ModuleManager::UnloadModule(m, source.GetUser());

		if (status == MOD_ERR_OK)
		{
			logger.Command(LogType::ADMIN, source, _("{source} used {command} to unload module {0}"), mname);
			source.Reply(_("Module \002{0}\002 unloaded."), mname);
		}
		else
		{
			source.Reply(_("Unable to remove module \002{0}\002."), mname);
		}
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
