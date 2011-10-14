/* NickServ core functions
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

class CommandNSSet : public Command
{
 public:
	CommandNSSet(Module *creator) : Command(creator, "nickserv/set", 1, 3)
	{
		this->SetDesc(_("Set options, including kill protection"));
		this->SetSyntax(_("\037option\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		this->OnSyntaxError(source, "");
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets various nickname options.  \037option\037 can be one of:"));
		Anope::string this_name = source.command;
		for (BotInfo::command_map::iterator it = source.owner->commands.begin(), it_end = source.owner->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			CommandInfo &info = it->second;

			if (c_name.find_ci(this_name + " ") == 0)
			{
				service_reference<Command> command(info.name);
				if (command)
				{
					source.command = c_name;
					command->OnServHelp(source);
				}
			}
		}
		source.Reply(_("Type \002%s%s HELP %s \037option\037\002 for more information\n"
				"on a specific option."), Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str(), source.command.c_str());
		return true;
	}
};

class CommandNSSetPassword : public Command
{
 public:
	CommandNSSetPassword(Module *creator) : Command(creator, "nickserv/set/password", 1)
	{
		this->SetDesc(_("Set your nickname password"));
		this->SetSyntax(_("\037new-password\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &param = params[1];
		unsigned len = param.length();

		if (u->Account()->display.equals_ci(param) || (Config->StrictPasswords && len < 5))
		{
			source.Reply(MORE_OBSCURE_PASSWORD);
			return;
		}
		else if (len > Config->PassLen)
		{
			source.Reply(PASSWORD_TOO_LONG);
			return;
		}

		enc_encrypt(param, u->Account()->pass);
		Anope::string tmp_pass;
		if (enc_decrypt(u->Account()->pass, tmp_pass) == 1)
			source.Reply(_("Password for \002%s\002 changed to \002%s\002."), u->Account()->display.c_str(), tmp_pass.c_str());
		else
			source.Reply(_("Password for \002%s\002 changed."), u->Account()->display.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the password used to identify you as the nick's\n"
				"owner."));
		return true;
	}
};

class NSSet : public Module
{
	CommandNSSet commandnsset;
	CommandNSSetPassword commandnssetpassword;

 public:
	NSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsset(this), commandnssetpassword(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSSet)
