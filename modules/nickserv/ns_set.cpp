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
		return;
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
		return;
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
		const Anope::string &param = params[0];
		unsigned len = param.length();

		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		if (source.GetNick().equals_ci(param))
		{
			source.Reply(MORE_OBSCURE_PASSWORD);
			return;
		}

		unsigned int minpasslen = Config->GetModule("nickserv").Get<unsigned>("minpasslen", "10");
		if (len < minpasslen)
		{
			source.Reply(PASSWORD_TOO_SHORT, minpasslen);
			return;
		}

		unsigned int maxpasslen = Config->GetModule("nickserv").Get<unsigned>("maxpasslen", "50");
		if (len > maxpasslen)
		{
			source.Reply(PASSWORD_TOO_LONG, maxpasslen);
			return;
		}

		if (!Anope::Encrypt(param, source.nc->pass))
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
		NickCore *nc = setter_na->nc;

		size_t len = params[1].length();

		if (Config->GetModule("nickserv").Get<bool>("secureadmins", "yes") && source.nc != nc && nc->IsServicesOper())
		{
			source.Reply(_("You may not change the password of other Services Operators."));
			return;
		}

		if (nc->display.equals_ci(params[1]))
		{
			source.Reply(MORE_OBSCURE_PASSWORD);
			return;
		}

		unsigned int minpasslen = Config->GetModule("nickserv").Get<unsigned>("minpasslen", "10");
		if (len < minpasslen)
		{
			source.Reply(PASSWORD_TOO_SHORT, minpasslen);
			return;
		}

		unsigned int maxpasslen = Config->GetModule("nickserv").Get<unsigned>("maxpasslen", "50");
		if (len > maxpasslen)
		{
			source.Reply(PASSWORD_TOO_LONG, maxpasslen);
			return;
		}

		if (!Anope::Encrypt(params[1], nc->pass))
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

class CommandNSSetAutoOp
	: public Command
{
public:
	CommandNSSetAutoOp(Module *creator, const Anope::string &sname = "nickserv/set/autoop", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Sets whether services should set channel status modes on you automatically."));
		this->SetSyntax("{ON | OFF}");
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const NickAlias *na = NickAlias::Find(user);
		if (na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetNickOption, MOD_RESULT, (source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable autoop for " << na->nc->display;
			nc->Extend<bool>("AUTOOP");
			source.Reply(_("Services will from now on set status modes on %s in channels."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable autoop for " << na->nc->display;
			nc->Shrink<bool>("AUTOOP");
			source.Reply(_("Services will no longer set status modes on %s in channels."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "AUTOOP");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		BotInfo *bi = Config->GetClient("ChanServ");
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Sets whether you will be given your channel status modes automatically. "
				"Set to \002ON\002 to allow %s to set status modes on you automatically "
				"when entering channels. Note that depending on channel settings some modes "
				"may not get set automatically."
			),
			bi ? bi->nick.c_str() : "ChanServ");
		return true;
	}
};

class CommandNSSASetAutoOp final
	: public CommandNSSetAutoOp
{
public:
	CommandNSSASetAutoOp(Module *creator) : CommandNSSetAutoOp(creator, "nickserv/saset/autoop", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		BotInfo *bi = Config->GetClient("ChanServ");
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Sets whether the given nickname will be given its status modes "
				"in channels automatically. Set to \002ON\002 to allow %s "
				"to set status modes on the given nickname automatically when it "
				"is entering channels. Note that depending on channel settings "
				"some modes may not get set automatically."
			),
			bi ? bi->nick.c_str() : "ChanServ");
		return true;
	}
};

class CommandNSSetNeverOp
	: public Command
{
public:
	CommandNSSetNeverOp(Module *creator, const Anope::string &sname = "nickserv/set/neverop", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Sets whether you can be added to a channel access list."));
		this->SetSyntax("{ON | OFF}");
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const NickAlias *na = NickAlias::Find(user);
		if (na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetNickOption, MOD_RESULT, (source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable neverop for " << na->nc->display;
			nc->Extend<bool>("NEVEROP");
			source.Reply(_("%s can no longer be added to channel access lists."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable neverop for " << na->nc->display;
			nc->Shrink<bool>("NEVEROP");
			source.Reply(_("%s can now be added to channel access lists."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "NEVEROP");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets whether you can be added to a channel access list."));
		return true;
	}
};

class CommandNSSASetNeverOp final
	: public CommandNSSetNeverOp
{
public:
	CommandNSSASetNeverOp(Module *creator) : CommandNSSetNeverOp(creator, "nickserv/saset/neverop", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets whether the given nickname can be added to a channel access list."));
		return true;
	}
};

class CommandNSSetDisplay
	: public Command
{
public:
	CommandNSSetDisplay(Module *creator, const Anope::string &sname = "nickserv/set/display", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Set the display of your group in services"));
		this->SetSyntax(_("\037new-display\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		NickAlias *user_na = NickAlias::Find(user), *na = NickAlias::Find(param);

		if (Config->GetModule("nickserv").Get<bool>("nonicknameownership"))
		{
			source.Reply(_("This command may not be used on this network because nickname ownership is disabled."));
			return;
		}
		if (user_na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		else if (!na || *na->nc != *user_na->nc)
		{
			source.Reply(_("The new display MUST be a nickname of the nickname group %s."), user_na->nc->display.c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetNickOption, MOD_RESULT, (source, this, user_na->nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		Log(user_na->nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the display of " << user_na->nc->display << " to " << na->nick;

		user_na->nc->SetDisplay(na);

		/* Send updated account name */
		for (std::list<User *>::iterator it = user_na->nc->users.begin(); it != user_na->nc->users.end(); ++it)
		{
			User *u = *it;
			IRCD->SendLogin(u, user_na);
		}

		source.Reply(NICK_SET_DISPLAY_CHANGED, user_na->nc->display.c_str());
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Changes the display used to refer to your nickname group in "
			"services. The new display MUST be a nick of your group."
		));
		return true;
	}
};

class CommandNSSASetDisplay final
	: public CommandNSSetDisplay
{
public:
	CommandNSSASetDisplay(Module *creator) : CommandNSSetDisplay(creator, "nickserv/saset/display", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 \037new-display\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Changes the display used to refer to the nickname group in "
			"services. The new display MUST be a nick of the group."
		));
		return true;
	}
};

class CommandNSSetEmail
	: public Command
{
	static bool SendConfirmMail(User *u, NickCore *nc, BotInfo *bi, const Anope::string &new_email)
	{
		Anope::string code = Anope::Random(Config->GetBlock("options").Get<size_t>("codelength", 15));

		std::pair<Anope::string, Anope::string> *n = nc->Extend<std::pair<Anope::string, Anope::string> >("ns_set_email");
		n->first = new_email;
		n->second = code;

		Anope::map<Anope::string> vars = {
			{ "old_email", nc->email                                                               },
			{ "new_email", new_email                                                               },
			{ "account",   nc->display                                                             },
			{ "network",   Config->GetBlock("networkinfo").Get<const Anope::string>("networkname") },
			{ "code",      code                                                                    },
		};

		auto subject = Anope::Template(Config->GetBlock("mail").Get<const Anope::string>("emailchange_subject"), vars);
		auto message = Anope::Template(Config->GetBlock("mail").Get<const Anope::string>("emailchange_message"), vars);

		Anope::string old = nc->email;
		nc->email = new_email;
		bool b = Mail::Send(u, nc, bi, subject, message);
		nc->email = old;
		return b;
	}

public:
	CommandNSSetEmail(Module *creator, const Anope::string &cname = "nickserv/set/email", size_t min = 0) : Command(creator, cname, min, min + 1)
	{
		this->SetDesc(_("Associate an email address with your nickname"));
		this->SetSyntax(_("\037address\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		if (nc->HasExt("UNCONFIRMED"))
		{
			source.Reply(_("You may not change the email of an unconfirmed account."));
			return;
		}

		if (param.empty() && Config->GetModule("nickserv").Get<bool>("forceemail", "yes"))
		{
			source.Reply(_("You cannot unset the email on this network."));
			return;
		}
		else if (Config->GetModule("nickserv").Get<bool>("secureadmins", "yes") && source.nc != nc && nc->IsServicesOper())
		{
			source.Reply(_("You may not change the email of other Services Operators."));
			return;
		}
		else if (!param.empty() && !Mail::Validate(param))
		{
			source.Reply(MAIL_X_INVALID, param.c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetNickOption, MOD_RESULT, (source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		const auto nsmailreg = Config->GetModule("ns_register").Get<const Anope::string>("registration").equals_ci("mail");
		if (!param.empty() && Config->GetModule("nickserv").Get<bool>("confirmemailchanges", nsmailreg ? "yes" : "no") && source.GetAccount() == nc)
		{
			if (SendConfirmMail(source.GetUser(), source.GetAccount(), source.service, param))
			{
				Log(LOG_COMMAND, source, this) << "to request changing the email of " << nc->display << " to " << param;
				source.Reply(_("A confirmation email has been sent to \002%s\002. Follow the instructions in it to change your email address."), param.c_str());
			}
		}
		else
		{
			if (!param.empty())
			{
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the email of " << nc->display << " to " << param;
				nc->email = param;
				source.Reply(_("Email address for \002%s\002 changed to \002%s\002."), nc->display.c_str(), param.c_str());
			}
			else
			{
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to unset the email of " << nc->display;
				nc->email.clear();
				source.Reply(_("Email address for \002%s\002 unset."), nc->display.c_str());
			}
		}
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params.size() ? params[0] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Associates the given email address with your nickname. "
			"This address will be displayed whenever someone requests "
			"information on the nickname with the \002INFO\002 command."
		));
		return true;
	}
};

class CommandNSSASetEmail final
	: public CommandNSSetEmail
{
public:
	CommandNSSASetEmail(Module *creator) : CommandNSSetEmail(creator, "nickserv/saset/email", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 \037address\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Associates the given email address with the nickname."));
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

	CommandNSSetAutoOp commandnssetautoop;
	CommandNSSASetAutoOp commandnssasetautoop;

	CommandNSSetNeverOp commandnssetneverop;
	CommandNSSASetNeverOp commandnssasetneverop;

	CommandNSSetDisplay commandnssetdisplay;
	CommandNSSASetDisplay commandnssasetdisplay;

	CommandNSSetEmail commandnssetemail;
	CommandNSSASetEmail commandnssasetemail;

	CommandNSSetPassword commandnssetpassword;
	CommandNSSASetPassword commandnssasetpassword;

	CommandNSSASetNoexpire commandnssasetnoexpire;

	SerializableExtensibleItem<bool> autoop, neverop, noexpire;

	/* email, passcode */
	PrimitiveExtensibleItem<std::pair<Anope::string, Anope::string > > ns_set_email;

public:
	NSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsset(this), commandnssaset(this),
		commandnssetautoop(this), commandnssasetautoop(this),
		commandnssetneverop(this), commandnssasetneverop(this),
		commandnssetdisplay(this), commandnssasetdisplay(this),
		commandnssetemail(this), commandnssasetemail(this),
		commandnssetpassword(this), commandnssasetpassword(this),
		commandnssasetnoexpire(this),

		autoop(this, "AUTOOP"), neverop(this, "NEVEROP"),
		noexpire(this, "NS_NO_EXPIRE"),
		ns_set_email(this, "ns_set_email")
	{
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) override
	{
		NickCore *uac = source.nc;

		if (command->name == "nickserv/confirm" && !params.empty() && uac)
		{
			std::pair<Anope::string, Anope::string> *n = ns_set_email.Get(uac);
			if (n)
			{
				if (params[0] == n->second)
				{
					uac->email = n->first;
					Log(LOG_COMMAND, source, command) << "to confirm their email address change to " << uac->email;
					source.Reply(_("Your email address has been changed to \002%s\002."), uac->email.c_str());
					ns_set_email.Unset(uac);
					return EVENT_STOP;
				}
			}
		}

		return EVENT_CONTINUE;
	}

	void OnSetCorrectModes(User *user, Channel *chan, AccessGroup &access, bool &give_modes, bool &take_modes) override
	{
		if (chan->ci)
		{
			/* Only give modes if autoop is set */
			give_modes &= !user->IsIdentified() || autoop.HasExt(user->Account());
		}
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

		if (autoop.HasExt(na->nc))
			info.AddOption(_("Auto-op"));
		if (neverop.HasExt(na->nc))
			info.AddOption(_("Never-op"));
		if (noexpire.HasExt(na))
			info.AddOption(_("No expire"));
	}

	void OnUserModeSet(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (u->IsIdentified() && setter.GetUser() == u)
			u->Account()->last_modes = u->GetModeList();
	}

	void OnUserModeUnset(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (u->IsIdentified() && setter.GetUser() == u)
			u->Account()->last_modes = u->GetModeList();
	}
};

MODULE_INIT(NSSet)
