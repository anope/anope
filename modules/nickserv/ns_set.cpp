/* NickServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandNSSet final
	: public Command
{
public:
	CommandNSSet(Module *creator) : Command(creator, "nickserv/set", 1, 3)
	{
		this->SetDesc(_("Set nickname options and information"));
		this->SetSyntax(_("\037option\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets various nickname options. \037option\037 can be one of:"));

		Anope::string this_name = source.command;
		bool hide_privileged_commands = Config->GetBlock("options").Get<bool>("hideprivilegedcommands"),
		     hide_registered_commands = Config->GetBlock("options").Get<bool>("hideregisteredcommands");

		HelpWrapper help;
		for (const auto &[c_name, info] : source.service->commands)
		{
			if (c_name.find_ci(this_name + " ") == 0)
			{
				if (info.hide)
					continue;

				ServiceReference<Command> c("Command", info.name);
				// XXX dup
				if (!c)
					continue;
				else if (hide_registered_commands && !c->AllowUnregistered() && !source.GetAccount())
					continue;
				else if (hide_privileged_commands && !info.permission.empty() && !source.HasCommand(info.permission))
					continue;

				source.command = c_name;
				c->OnServHelp(source, help);
			}
		}
		help.SendTo(source);

		source.Reply(_("Type \002%s\032\037option\037\002 for more information on a specific option."),
			source.service->GetQueryCommand("generic/help", this_name).c_str());

		return true;
	}
};

class CommandNSSASet final
	: public Command
{
public:
	CommandNSSASet(Module *creator) : Command(creator, "nickserv/saset", 2, 4)
	{
		this->SetDesc(_("Set SET-options on another nickname"));
		this->SetSyntax(_("\037option\037 \037nickname\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets various nickname options. \037option\037 can be one of:"));

		HelpWrapper help;
		Anope::string this_name = source.command;
		for (const auto &[c_name, info] : source.service->commands)
		{
			if (c_name.find_ci(this_name + " ") == 0)
			{
				ServiceReference<Command> command("Command", info.name);
				if (command)
				{
					source.command = c_name;
					command->OnServHelp(source, help);
				}
			}
		}
		help.SendTo(source);

		source.Reply(_(
				"Type \002%s\032\037option\037\002 for more information "
				"on a specific option. The options will be set on the given "
				"\037nickname\037."
			),
			source.service->GetQueryCommand("generic/help", this_name).c_str());
		return true;
	}
};

class CommandNSSetPassword final
	: public Command
{
public:
	CommandNSSetPassword(Module *creator) : Command(creator, "nickserv/set/password", 1)
	{
		this->SetDesc(_("Set your nickname password"));
		this->SetSyntax(_("\037new-password\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const Anope::string &param = params[0];
		NickCore *nc = source.nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnPasswordValidate, MOD_RESULT, (source, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (!Anope::Encrypt(param, nc->pass))
		{
			source.Reply(_("Passwords can not be changed right now. Please try again later."));
			return;
		}

		Log(LOG_COMMAND, source, this) << "to change their password";
		source.Reply(_("Password for \002%s\002 changed."), source.nc->display.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the password used to identify you as the nick's owner."));
		return true;
	}
};

class CommandNSSASetPassword final
	: public Command
{
public:
	CommandNSSASetPassword(Module *creator) : Command(creator, "nickserv/saset/password", 2, 2)
	{
		this->SetDesc(_("Set the nickname password"));
		this->SetSyntax(_("\037nickname\037 \037new-password\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const NickAlias *setter_na = NickAlias::Find(params[0]);
		if (setter_na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		const Anope::string &param = params[0];
		NickCore *nc = setter_na->nc;

		if (Config->GetModule("nickserv").Get<bool>("secureadmins", "yes") && source.nc != nc && nc->IsServicesOper())
		{
			source.Reply(_("You may not change the password of other Services Operators."));
			return;
		}


		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnPasswordValidate, MOD_RESULT, (source, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (!Anope::Encrypt(param, nc->pass))
		{
			source.Reply(_("Passwords can not be changed right now. Please try again later."));
			return;
		}

		Log(LOG_ADMIN, source, this) << "to change the password of " << nc->display;
		source.Reply(_("Password for \002%s\002 changed."), nc->display.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the password used to identify as the nick's owner."));
		return true;
	}
};

class CommandNSSASetNoexpire final
	: public Command
{
public:
	CommandNSSASetNoexpire(Module *creator) : Command(creator, "nickserv/saset/noexpire", 1, 2)
	{
		this->SetDesc(_("Prevent the nickname from expiring"));
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		NickAlias *na = NickAlias::Find(params[0]);
		if (na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		Anope::string param = params.size() > 1 ? params[1] : "";

		if (param.equals_ci("ON"))
		{
			Log(LOG_ADMIN, source, this) << "to enable noexpire for " << na->nick << " (" << na->nc->display << ")";
			na->Extend<bool>("NS_NO_EXPIRE");
			source.Reply(_("Nick %s \002will not\002 expire."), na->nick.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(LOG_ADMIN, source, this) << "to disable noexpire for " << na->nick << " (" << na->nc->display << ")";
			na->Shrink<bool>("NS_NO_EXPIRE");
			source.Reply(_("Nick %s \002will\002 expire."), na->nick.c_str());
		}
		else
			this->OnSyntaxError(source, "NOEXPIRE");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Sets whether the given nickname will expire. Setting this "
			"to \002ON\002 prevents the nickname from expiring."
		));
		return true;
	}
};

class NSSet final
	: public Module
{
	CommandNSSet commandnsset;
	CommandNSSASet commandnssaset;

	CommandNSSetPassword commandnssetpassword;
	CommandNSSASetPassword commandnssasetpassword;

	CommandNSSASetNoexpire commandnssasetnoexpire;

	SerializableExtensibleItem<bool> noexpire;

public:
	NSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsset(this), commandnssaset(this),
		commandnssetpassword(this), commandnssasetpassword(this),
		commandnssasetnoexpire(this),
		noexpire(this, "NS_NO_EXPIRE")
	{
	}

	void OnPreNickExpire(NickAlias *na, bool &expire) override
	{
		if (noexpire.HasExt(na))
			expire = false;
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool show_hidden) override
	{
		if (!show_hidden)
			return;

		if (noexpire.HasExt(na))
			info.AddOption(_("No expiry"));
	}
};

MODULE_INIT(NSSet)
