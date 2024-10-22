/* NickServ core functions
 *
 * (C) 2003-2024 Anope Team
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
		this->SetDesc(_("Set options, including kill protection"));
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
		bool hide_privileged_commands = Config->GetBlock("options")->Get<bool>("hideprivilegedcommands"),
		     hide_registered_commands = Config->GetBlock("options")->Get<bool>("hideregisteredcommands");
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
				c->OnServHelp(source);
			}
		}

		source.Reply(_("Type \002%s HELP %s \037option\037\002 for more information\n"
			"on a specific option."), source.service->GetQueryCommand().c_str(), this_name.c_str());

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

		Anope::string this_name = source.command;
		for (const auto &[c_name, info] : source.service->commands)
		{
			if (c_name.find_ci(this_name + " ") == 0)
			{
				ServiceReference<Command> command("Command", info.name);
				if (command)
				{
					source.command = c_name;
					command->OnServHelp(source);
				}
			}
		}

		source.Reply(_("Type \002%s HELP %s \037option\037\002 for more information\n"
				"on a specific option. The options will be set on the given\n"
				"\037nickname\037."), source.service->GetQueryCommand().c_str(), this_name.c_str());
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

		unsigned int minpasslen = Config->GetModule("nickserv")->Get<unsigned>("minpasslen", "10");
		if (len < minpasslen)
		{
			source.Reply(PASSWORD_TOO_SHORT, minpasslen);
			return;
		}

		unsigned int maxpasslen = Config->GetModule("nickserv")->Get<unsigned>("maxpasslen", "50");
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
		source.Reply(_("Changes the password used to identify you as the nick's\n"
			"owner."));
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

		if (Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && source.nc != nc && nc->IsServicesOper())
		{
			source.Reply(_("You may not change the password of other Services Operators."));
			return;
		}

		if (nc->display.equals_ci(params[1]))
		{
			source.Reply(MORE_OBSCURE_PASSWORD);
			return;
		}

		unsigned int minpasslen = Config->GetModule("nickserv")->Get<unsigned>("minpasslen", "10");
		if (len < minpasslen)
		{
			source.Reply(PASSWORD_TOO_SHORT, minpasslen);
			return;
		}

		unsigned int maxpasslen = Config->GetModule("nickserv")->Get<unsigned>("maxpasslen", "50");
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
		source.Reply(_("Sets whether you will be given your channel status modes automatically.\n"
				"Set to \002ON\002 to allow %s to set status modes on you automatically\n"
				"when entering channels. Note that depending on channel settings some modes\n"
				"may not get set automatically."), bi ? bi->nick.c_str() : "ChanServ");
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
		source.Reply(_("Sets whether the given nickname will be given its status modes\n"
				"in channels automatically. Set to \002ON\002 to allow %s\n"
				"to set status modes on the given nickname automatically when it\n"
				"is entering channels. Note that depending on channel settings\n"
				"some modes may not get set automatically."), bi ? bi->nick.c_str() : "ChanServ");
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

		if (Config->GetModule("nickserv")->Get<bool>("nonicknameownership"))
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
		source.Reply(_("Changes the display used to refer to your nickname group in\n"
				"services. The new display MUST be a nick of your group."));
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
		source.Reply(_("Changes the display used to refer to the nickname group in\n"
				"services. The new display MUST be a nick of the group."));
		return true;
	}
};

class CommandNSSetEmail
	: public Command
{
	static bool SendConfirmMail(User *u, NickCore *nc, BotInfo *bi, const Anope::string &new_email)
	{
		Anope::string code = Anope::Random(15);

		std::pair<Anope::string, Anope::string> *n = nc->Extend<std::pair<Anope::string, Anope::string> >("ns_set_email");
		n->first = new_email;
		n->second = code;

		Anope::string subject = Config->GetBlock("mail")->Get<const Anope::string>("emailchange_subject"),
			message = Config->GetBlock("mail")->Get<const Anope::string>("emailchange_message");

		subject = subject.replace_all_cs("%e", nc->email);
		subject = subject.replace_all_cs("%E", new_email);
		subject = subject.replace_all_cs("%n", nc->display);
		subject = subject.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<const Anope::string>("networkname"));
		subject = subject.replace_all_cs("%c", code);

		message = message.replace_all_cs("%e", nc->email);
		message = message.replace_all_cs("%E", new_email);
		message = message.replace_all_cs("%n", nc->display);
		message = message.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<const Anope::string>("networkname"));
		message = message.replace_all_cs("%c", code);

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

		if (param.empty() && Config->GetModule("nickserv")->Get<bool>("forceemail", "yes"))
		{
			source.Reply(_("You cannot unset the email on this network."));
			return;
		}
		else if (Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && source.nc != nc && nc->IsServicesOper())
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

		if (!param.empty() && Config->GetModule("nickserv")->Get<bool>("confirmemailchanges") && !source.IsServicesOper())
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
		source.Reply(_("Associates the given email address with your nickname.\n"
				"This address will be displayed whenever someone requests\n"
				"information on the nickname with the \002INFO\002 command."));
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

class CommandNSSetKeepModes
	: public Command
{
public:
	CommandNSSetKeepModes(Module *creator, const Anope::string &sname = "nickserv/set/keepmodes", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Enable or disable keep modes"));
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
		if (!na)
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
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable keepmodes for " << nc->display;
			nc->Extend<bool>("NS_KEEP_MODES");
			source.Reply(_("Keep modes for %s is now \002on\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable keepmodes for " << nc->display;
			nc->Shrink<bool>("NS_KEEP_MODES");
			source.Reply(_("Keep modes for %s is now \002off\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables keepmodes for your nick. If keep\n"
				"modes is enabled, services will remember your usermodes\n"
				"and attempt to re-set them the next time you authenticate."));
		return true;
	}
};

class CommandNSSASetKeepModes final
	: public CommandNSSetKeepModes
{
public:
	CommandNSSASetKeepModes(Module *creator) : CommandNSSetKeepModes(creator, "nickserv/saset/keepmodes", 2)
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
		source.Reply(_("Enables or disables keepmodes for the given nick. If keep\n"
				"modes is enabled, services will remember users' usermodes\n"
				"and attempt to re-set them the next time they authenticate."));
		return true;
	}
};

class CommandNSSetKill
	: public Command
{
public:
	CommandNSSetKill(Module *creator, const Anope::string &sname = "nickserv/set/kill", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Turn protection on or off"));
		this->SetSyntax("{ON | QUICK | IMMED | OFF}");
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		if (Config->GetModule("nickserv")->Get<bool>("nonicknameownership"))
		{
			source.Reply(_("This command may not be used on this network because nickname ownership is disabled."));
			return;
		}

		const NickAlias *na = NickAlias::Find(user);
		if (!na)
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
			nc->Extend<bool>("KILLPROTECT");
			nc->Shrink<bool>("KILL_QUICK");
			nc->Shrink<bool>("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill on for " << nc->display;
			source.Reply(_("Protection is now \002on\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("QUICK"))
		{
			nc->Extend<bool>("KILLPROTECT");
			nc->Extend<bool>("KILL_QUICK");
			nc->Shrink<bool>("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill quick for " << nc->display;
			source.Reply(_("Protection is now \002on\002 for \002%s\002, with a reduced delay."), nc->display.c_str());
		}
		else if (param.equals_ci("IMMED"))
		{
			if (Config->GetModule(this->owner)->Get<bool>("allowkillimmed"))
			{
				nc->Extend<bool>("KILLPROTECT");
				nc->Shrink<bool>("KILL_QUICK");
				nc->Extend<bool>("KILL_IMMED");
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill immed for " << nc->display;
				source.Reply(_("Protection is now \002on\002 for \002%s\002, with no delay."), nc->display.c_str());
			}
			else
				source.Reply(_("The \002IMMED\002 option is not available on this network."));
		}
		else if (param.equals_ci("OFF"))
		{
			nc->Shrink<bool>("KILLPROTECT");
			nc->Shrink<bool>("KILL_QUICK");
			nc->Shrink<bool>("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable kill for " << nc->display;
			source.Reply(_("Protection is now \002off\002 for \002%s\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "KILL");

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns the automatic protection option for your nick\n"
				"on or off. With protection on, if another user\n"
				"tries to take your nick, they will be given one minute to\n"
				"change to another nick, after which %s will forcibly change\n"
				"their nick.\n"
				" \n"
				"If you select \002QUICK\002, the user will be given only 20 seconds\n"
				"to change nicks instead of the usual 60. If you select\n"
				"\002IMMED\002, the user's nick will be changed immediately \037without\037 being\n"
				"warned first or given a chance to change their nick; please\n"
				"do not use this option unless necessary. Also, your\n"
				"network's administrators may have disabled this option."), source.service->nick.c_str());
		return true;
	}
};

class CommandNSSASetKill final
	: public CommandNSSetKill
{
public:
	CommandNSSASetKill(Module *creator) : CommandNSSetKill(creator, "nickserv/saset/kill", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | QUICK | IMMED | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns the automatic protection option for the nick\n"
				"on or off. With protection on, if another user\n"
				"tries to take the nick, they will be given one minute to\n"
				"change to another nick, after which %s will forcibly change\n"
				"their nick.\n"
				" \n"
				"If you select \002QUICK\002, the user will be given only 20 seconds\n"
				"to change nicks instead of the usual 60. If you select\n"
				"\002IMMED\002, the user's nick will be changed immediately \037without\037 being\n"
				"warned first or given a chance to change their nick; please\n"
				"do not use this option unless necessary. Also, your\n"
				"network's administrators may have disabled this option."), source.service->nick.c_str());
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
		source.Reply(_("Sets whether the given nickname will expire.  Setting this\n"
				"to \002ON\002 prevents the nickname from expiring."));
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

	CommandNSSetKeepModes commandnssetkeepmodes;
	CommandNSSASetKeepModes commandnssasetkeepmodes;

	CommandNSSetKill commandnssetkill;
	CommandNSSASetKill commandnssasetkill;

	CommandNSSetPassword commandnssetpassword;
	CommandNSSASetPassword commandnssasetpassword;

	CommandNSSASetNoexpire commandnssasetnoexpire;

	SerializableExtensibleItem<bool> autoop, neverop, killprotect, kill_quick, kill_immed,
		noexpire;

	struct KeepModes final
		: SerializableExtensibleItem<bool>
	{
		KeepModes(Module *m, const Anope::string &n) : SerializableExtensibleItem<bool>(m, n) { }

		void ExtensibleSerialize(const Extensible *e, const Serializable *s, Serialize::Data &data) const override
		{
			SerializableExtensibleItem<bool>::ExtensibleSerialize(e, s, data);

			if (s->GetSerializableType()->GetName() != "NickCore")
				return;

			const NickCore *nc = anope_dynamic_static_cast<const NickCore *>(s);
			Anope::string modes;
			for (const auto &[last_mode, last_value] : nc->last_modes)
			{
				if (!modes.empty())
					modes += " ";
				modes += last_mode;
				if (!last_value.empty())
					modes += "," + last_value;
			}
			data.Store("last_modes", modes);
		}

		void ExtensibleUnserialize(Extensible *e, Serializable *s, Serialize::Data &data) override
		{
			SerializableExtensibleItem<bool>::ExtensibleUnserialize(e, s, data);

			if (s->GetSerializableType()->GetName() != "NickCore")
				return;

			NickCore *nc = anope_dynamic_static_cast<NickCore *>(s);
			Anope::string modes;
			data["last_modes"] >> modes;
			nc->last_modes.clear();
			for (spacesepstream sep(modes); sep.GetToken(modes);)
			{
				size_t c = modes.find(',');
				if (c == Anope::string::npos)
					nc->last_modes.emplace(modes, "");
				else
					nc->last_modes.emplace(modes.substr(0, c), modes.substr(c + 1));
			}
		}
	} keep_modes;

	/* email, passcode */
	PrimitiveExtensibleItem<std::pair<Anope::string, Anope::string > > ns_set_email;

public:
	NSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsset(this), commandnssaset(this),
		commandnssetautoop(this), commandnssasetautoop(this),
		commandnssetneverop(this), commandnssasetneverop(this),
		commandnssetdisplay(this), commandnssasetdisplay(this),
		commandnssetemail(this), commandnssasetemail(this),
		commandnssetkeepmodes(this), commandnssasetkeepmodes(this),
		commandnssetkill(this), commandnssasetkill(this),
		commandnssetpassword(this), commandnssasetpassword(this),
		commandnssasetnoexpire(this),

		autoop(this, "AUTOOP"), neverop(this, "NEVEROP"),
		killprotect(this, "KILLPROTECT"), kill_quick(this, "KILL_QUICK"),
		kill_immed(this, "KILL_IMMED"),
		noexpire(this, "NS_NO_EXPIRE"),

		keep_modes(this, "NS_KEEP_MODES"), ns_set_email(this, "ns_set_email")
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
			give_modes &= !user->Account() || autoop.HasExt(user->Account());
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

		if (kill_immed.HasExt(na->nc))
			info.AddOption(_("Immediate protection"));
		else if (kill_quick.HasExt(na->nc))
			info.AddOption(_("Quick protection"));
		else if (killprotect.HasExt(na->nc))
			info.AddOption(_("Protection"));
		if (autoop.HasExt(na->nc))
			info.AddOption(_("Auto-op"));
		if (neverop.HasExt(na->nc))
			info.AddOption(_("Never-op"));
		if (noexpire.HasExt(na))
			info.AddOption(_("No expire"));
		if (keep_modes.HasExt(na->nc))
			info.AddOption(_("Keep modes"));
	}

	void OnUserModeSet(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (u->Account() && setter.GetUser() == u)
			u->Account()->last_modes = u->GetModeList();
	}

	void OnUserModeUnset(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (u->Account() && setter.GetUser() == u)
			u->Account()->last_modes = u->GetModeList();
	}

	void OnUserLogin(User *u) override
	{
		if (keep_modes.HasExt(u->Account()))
		{
			User::ModeList modes = u->Account()->last_modes;
			for (const auto &[last_mode, last_value] : modes)
			{
				UserMode *um = ModeManager::FindUserModeByName(last_mode);
				/* if the null user can set the mode, then it's probably safe */
				if (um && um->CanSet(NULL))
					u->SetMode(NULL, last_mode, last_value);
			}
		}
	}
};

MODULE_INIT(NSSet)
