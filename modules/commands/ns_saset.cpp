/* NickServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandNSSASet : public Command
{
 public:
	CommandNSSASet(Module *creator) : Command(creator, "nickserv/saset", 2, 4)
	{
		this->SetDesc(_("Set SET-options on another nickname"));
		this->SetSyntax(_("\037option\037 \037nickname\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->OnSyntaxError(source, "");
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_("Sets various nickname options. \037option\037 can be one of:"));
		Anope::string this_name = source.command;
		for (BotInfo::command_map::const_iterator it = source.owner->commands.begin(), it_end = source.owner->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;

			if (c_name.find_ci(this_name + " ") == 0)
			{
				service_reference<Command> command("Command", info.name);
				if (command)
				{
					source.command = c_name;
					command->OnServHelp(source);
				}
			}
		}
		source.Reply(_("Type \002%s%s HELP SASET \037option\037\002 for more information\n"
				"on a specific option. The options will be set on the given\n"
				"\037nickname\037."), Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSASetPassword : public Command
{
 public:
	CommandNSSASetPassword(Module *creator) : Command(creator, "nickserv/saset/password", 2, 2)
	{
		this->SetDesc(_("Set the nickname password"));
		this->SetSyntax(_("\037nickname\037 \037new-password\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.u;
		const NickAlias *setter_na = findnick(params[0]);
		if (setter_na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, params[0].c_str());
			return;
		}
		NickCore *nc = setter_na->nc;

		size_t len = params[1].length();

		if (Config->NSSecureAdmins && u->Account() != nc && nc->IsServicesOper())
		{
			source.Reply(_("You may not change the password of other services operators."));
			return;
		}
		else if (nc->display.equals_ci(params[1]) || (Config->StrictPasswords && len < 5))
		{
			source.Reply(MORE_OBSCURE_PASSWORD);
			return;
		}
		else if (len > Config->PassLen)
		{
			source.Reply(PASSWORD_TOO_LONG);
			return;
		}

		enc_encrypt(params[1], nc->pass);
		Anope::string tmp_pass;
		if (enc_decrypt(nc->pass, tmp_pass) == 1)
			source.Reply(_("Password for \002%s\002 changed to \002%s\002."), nc->display.c_str(), tmp_pass.c_str());
		else
			source.Reply(_("Password for \002%s\002 changed."), nc->display.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the password used to identify as the nick's owner."));
		return true;
	}
};

class NSSASet : public Module
{
	CommandNSSASet commandnssaset;
	CommandNSSASetPassword commandnssasetpassword;

 public:
	NSSASet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssaset(this), commandnssasetpassword(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSSASet)
