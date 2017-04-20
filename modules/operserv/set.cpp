/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
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

class CommandOSSet : public Command
{
 private:
	void DoList(CommandSource &source)
	{
		logger.Admin(source, _("{source} used {command} LIST"));

		const char *str;

		str = Anope::ReadOnly ? _("{0} is enabled") : _("{0} is disabled");
		source.Reply(str, "READONLY");

		str = Anope::Debug ? _("{0} is enabled") : _("{0} is disabled");
		source.Reply(str, "DEBUG");

		str = Anope::NoExpire ? _("{0} is enabled") : _("{0} is disabled");
		source.Reply(str, "NOEXPIRE");
	}

	void DoSetReadOnly(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(source, "READONLY");
			return;
		}

		if (setting.equals_ci("ON"))
		{
			Anope::ReadOnly = true;
			logger.Admin(source, _("{source} used {command} READONLY ON"));
			source.Reply(_("Services are now in \002read-only\002 mode."));
		}
		else if (setting.equals_ci("OFF"))
		{
			Anope::ReadOnly = false;
			logger.Admin(source, _("{source} used {command} READONLY OFF"));
			source.Reply(_("Services are now in \002read-write\002 mode."));
		}
		else
		{
			source.Reply(_("Setting for READONLY must be \002ON\002 or \002OFF\002."));
		}
	}

	void DoSetSuperAdmin(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &setting = params.size() > 1 ? params[1] : "";

		if (!source.GetUser())
			return;

		if (setting.empty())
		{
			this->OnSyntaxError(source, "SUPERADMIN");
			return;
		}

		/**
		 * Allow the user to turn super admin on/off
		 *
		 * Rob
		 **/
		bool super_admin = Config->GetModule(this->GetOwner())->Get<bool>("superadmin");
		if (!super_admin)
		{
			source.Reply(_("Super admin can not be set because it is not enabled in the configuration."));
			return;
		}

		if (setting.equals_ci("ON"))
		{
			source.GetUser()->super_admin = true;
			source.Reply(_("You are now a super admin."));
			logger.Admin(source, _("{source} used {command} SUPERADMIN ON"),
				source.GetSource(), source.GetCommand());
		}
		else if (setting.equals_ci("OFF"))
		{
			source.GetUser()->super_admin = false;
			source.Reply(_("You are no longer a super admin."));
			logger.Admin(source, _("{source} used {command} SUPERADMIN OFF"),
				source.GetSource(), source.GetCommand());
		}
		else
		{
			source.Reply(_("Setting for super admin must be \002ON\002 or \002OFF\002."));
		}
	}

	void DoSetDebug(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(source, "DEBUG");
			return;
		}

		if (setting.equals_ci("ON"))
		{
			Anope::Debug = 1;
			logger.Admin(source, _("{source} used {command} DEBUG ON"),
				source.GetSource(), source.GetCommand());
			source.Reply(_("Services are now in \002debug\002 mode."));
		}
		else if (setting.equals_ci("OFF") || setting == "0")
		{
			logger.Admin(source, _("{source} used {command} DEBUG OFF"));
			Anope::Debug = 0;
			source.Reply(_("Services are now in \002non-debug\002 mode."));
		}
		else
		{
			try
			{
				Anope::Debug = convertTo<int>(setting);
				logger.Admin(source, _("{source} used {command} DEBUG {0}"), Anope::Debug);
				source.Reply(_("Services are now in \002debug\002 mode (level %d)."), Anope::Debug);
				return;
			}
			catch (const ConvertException &) { }

			source.Reply(_("Setting for DEBUG must be \002ON\002, \002OFF\002, or a positive number."));
		}
	}

	void DoSetNoExpire(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &setting = params.size() > 1 ? params[1] : "";

		if (setting.empty())
		{
			this->OnSyntaxError(source, "NOEXPIRE");
			return;
		}

		if (setting.equals_ci("ON"))
		{
			Anope::NoExpire = true;
			logger.Admin(source, _("{source} used {command} NOEXPIRE ON"), Anope::Debug);
			source.Reply(_("Services are now in \002no expire\002 mode."));
		}
		else if (setting.equals_ci("OFF"))
		{
			Anope::NoExpire = false;
			logger.Admin(source, _("{source} used {command} NOEXPIRE OFF"), Anope::Debug);
			source.Reply(_("Services are now in \002expire\002 mode."));
		}
		else
		{
			source.Reply(_("Setting for NOEXPIRE must be \002ON\002 or \002OFF\002."));
		}
	}
 public:
	CommandOSSet(Module *creator) : Command(creator, "operserv/set", 1, 2)
	{
		this->SetDesc(_("Set various global Services options"));
		this->SetSyntax(_("\037option\037 \037setting\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &option = params[0];

		if (option.equals_ci("LIST"))
			return this->DoList(source);
		else if (option.equals_ci("READONLY"))
			return this->DoSetReadOnly(source, params);
		else if (option.equals_ci("DEBUG"))
			return this->DoSetDebug(source, params);
		else if (option.equals_ci("NOEXPIRE"))
			return this->DoSetNoExpire(source, params);
		else if (option.equals_ci("SUPERADMIN"))
			return this->DoSetSuperAdmin(source, params);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.empty())
		{
			source.Reply(_("Sets various global Services options.  Option names currently defined are:\n"
					"    READONLY   Set read-only or read-write mode\n"
					"    DEBUG      Activate or deactivate debug mode\n"
					"    NOEXPIRE   Activate or deactivate no expire mode\n"
					"    SUPERADMIN Activate or deactivate super admin mode\n"
					"    LIST       List the options"));
		}
		else if (subcommand.equals_ci("LIST"))
#warning "?"
			//source.Reply(_("Syntax: \002LIST\002\n"
			//		" \n"
			source.Reply(("Display the various {0} settings."), source.service->nick);
		else if (subcommand.equals_ci("READONLY"))
			//source.Reply(_("Syntax: \002READONLY {ON | OFF}\002\n"
			//		" \n"
			source.Reply(_("Sets read-only mode on or off.  In read-only mode, norma users will not be allowed to modify any Services data, including channel and nickname access lists, etc."
				       "Services Operators will still be able to do most tasks, but should understand any changes they do may not be permanent.\n"
				       "\n"
			               "This option is equivalent to the command-line option \002--readonly\002."));
		else if (subcommand.equals_ci("DEBUG"))
			//source.Reply(_("Syntax: \002DEBUG {ON | OFF}\002\n"
			//		" \n"
			source.Reply(_("Sets debug mode on or off.\n"
			               "\n"
			               "This option is equivalent to the command-line option \002--debug\002."));
		else if (subcommand.equals_ci("NOEXPIRE"))
			//source.Reply(_("Syntax: \002NOEXPIRE {ON | OFF}\002\n"
			//		" \n"
			source.Reply(_("Sets no expire mode on or off. In no expire mode, nicks, channels, akills and exceptions won't expire until the option is unset.\n"
			               "\n"
			               "This option is equivalent to the command-line option \002--noexpire\002."));
		else if (subcommand.equals_ci("SUPERADMIN"))
			//source.Reply(_("Syntax: \002SUPERADMIN {ON | OFF}\002\n"
			//		" \n"
			source.Reply(_("Setting this will grant you extra privileges, such as the ability to be \"founder\" on all channels."
			               "\n"
			               "This option is \002not\002 persistent, and should only be used when needed, and set back to OFF when no longer needed."));
		else
			return false;
		
		return true;
	}
};

class OSSet : public Module
{
	CommandOSSet commandosset;

 public:
	OSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosset(this)
	{
	}
};

MODULE_INIT(OSSet)
