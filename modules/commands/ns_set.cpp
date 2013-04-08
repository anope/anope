/* NickServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandNSSet : public Command
{
 public:
	CommandNSSet(Module *creator) : Command(creator, "nickserv/set", 1, 3)
	{
		this->SetDesc(_("Set options, including kill protection"));
		this->SetSyntax(_("\037option\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->OnSyntaxError(source, "");
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets various nickname options.  \037option\037 can be one of:"));

		Anope::string this_name = source.command;
		for (CommandInfo::map::const_iterator it = source.service->commands.begin(), it_end = source.service->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;

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

		source.Reply(_("Type \002%s%s HELP %s \037option\037\002 for more information\n"
			"on a specific option."), Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str(), this_name.c_str());

		return true;
	}
};

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
		source.Reply(" ");
		source.Reply(_("Sets various nickname options. \037option\037 can be one of:"));

		Anope::string this_name = source.command;
		for (CommandInfo::map::const_iterator it = source.service->commands.begin(), it_end = source.service->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;

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

		source.Reply(_("Type \002%s%s HELP %s \037option\037\002 for more information\n"
				"on a specific option. The options will be set on the given\n"
				"\037nickname\037."), Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str(), this_name.c_str());
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &param = params[1];
		unsigned len = param.length();

		if (source.GetNick().equals_ci(param) || (Config->StrictPasswords && len < 5))
		{
			source.Reply(MORE_OBSCURE_PASSWORD);
			return;
		}
		else if (len > Config->PassLen)
		{
			source.Reply(PASSWORD_TOO_LONG);
			return;
		}

		Log(LOG_COMMAND, source, this) << "to change their password";

		Anope::Encrypt(param, source.nc->pass);
		Anope::string tmp_pass;
		if (Anope::Decrypt(source.nc->pass, tmp_pass) == 1)
			source.Reply(_("Password for \002%s\002 changed to \002%s\002."), source.nc->display.c_str(), tmp_pass.c_str());
		else
			source.Reply(_("Password for \002%s\002 changed."), source.nc->display.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the password used to identify you as the nick's\n"
			"owner."));
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
		const NickAlias *setter_na = NickAlias::Find(params[0]);
		if (setter_na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, params[0].c_str());
			return;
		}
		NickCore *nc = setter_na->nc;

		size_t len = params[1].length();

		if (Config->NSSecureAdmins && source.nc != nc && nc->IsServicesOper())
		{
			source.Reply(_("You may not change the password of other Services Operators."));
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

		Log(LOG_ADMIN, source, this) << "to change the password of " << nc->display;

		Anope::Encrypt(params[1], nc->pass);
		Anope::string tmp_pass;
		if (Anope::Decrypt(nc->pass, tmp_pass) == 1)
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

class CommandNSSetAutoOp : public Command
{
 public:
	CommandNSSetAutoOp(Module *creator, const Anope::string &sname = "nickserv/set/autoop", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Sets whether services should set channel status modes on you automatically."));
		this->SetSyntax(_("{ON | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = NickAlias::Find(user);
		if (na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable autoop for " << na->nc->display;
			nc->ExtendMetadata("AUTOOP");
			source.Reply(_("Services will from now on set status modes on %s in channels."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable autoop for " << na->nc->display;
			nc->Shrink("AUTOOP");
			source.Reply(_("Services will no longer set status modes on %s in channels."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "AUTOOP");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets whether you will be given your channel status modes automatically.\n"
					"Set to \002ON\002 to allow ChanServ to set status modes on you automatically\n"
					"when entering channels. Note that depending on channel settings some modes\n"
					"may not get set automatically."));
		return true;
	}
};

class CommandNSSASetAutoOp : public CommandNSSetAutoOp
{
 public:
	CommandNSSASetAutoOp(Module *creator) : CommandNSSetAutoOp(creator, "nickserv/saset/autoop", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets whether the given nickname will be given its status modes\n"
				"in channels automatically. Set to \002ON\002 to allow ChanServ\n"
				"to set status modes on the given nickname automatically when it\n"
				"is entering channels. Note that depending on channel settings\n"
				"some modes may not get set automatically."));
		return true;
	}
};

class CommandNSSetChanstats : public Command
{
 public:
	CommandNSSetChanstats(Module *creator, const Anope::string &sname = "nickserv/set/chanstats", size_t min = 1 ) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Turn chanstat statistic on or off"));
		this->SetSyntax(_("{ON | OFF}"));
	}
	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, na->nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(na->nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable chanstats for " << na->nc->display;
			na->nc->ExtendMetadata("STATS");
			source.Reply(_("Chanstat statistics are now enabled for your nick."));
		}
		else if (param.equals_ci("OFF"))
		{
			Log(na->nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable chanstats for " << na->nc->display;
			na->nc->Shrink("STATS");
			source.Reply(_("Chanstat statistics are now disabled for your nick."));
		}
		else
			this->OnSyntaxError(source, "CHANSTATS");

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params[0]);
	}
	
	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns Chanstats statistics ON or OFF."));
		return true;
	}
};

class CommandNSSASetChanstats : public CommandNSSetChanstats
{
 public:
	CommandNSSASetChanstats(Module *creator) : CommandNSSetChanstats(creator, "nickserv/saset/chanstats", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns chanstats channel statistics ON or OFF for this user."));
		return true;
	}
};

class CommandNSSetDisplay : public Command
{
 public:
	CommandNSSetDisplay(Module *creator, const Anope::string &sname = "nickserv/set/display", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Set the display of your group in Services"));
		this->SetSyntax(_("\037new-display\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *user_na = NickAlias::Find(user), *na = NickAlias::Find(param);

		if (Config->NoNicknameOwnership)
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
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, user_na->nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		Log(user_na->nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the display of " << user_na->nc->display << " to " << na->nick;

		user_na->nc->SetDisplay(na);
		source.Reply(NICK_SET_DISPLAY_CHANGED, user_na->nc->display.c_str());
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the display used to refer to your nickname group in\n"
				"Services. The new display MUST be a nick of your group."));
		return true;
	}
};

class CommandNSSASetDisplay : public CommandNSSetDisplay
{
 public:
	CommandNSSASetDisplay(Module *creator) : CommandNSSetDisplay(creator, "nickserv/saset/display", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 \037new-display\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the display used to refer to the nickname group in\n"
				"Services. The new display MUST be a nick of the group."));
		return true;
	}
};

class CommandNSSetEmail : public Command
{
	static bool SendConfirmMail(User *u, const BotInfo *bi)
	{
		int chars[] = {
			' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
			'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
			'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
			'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
			'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
		};
		int idx, min = 1, max = 62;
		Anope::string code;
		for (idx = 0; idx < 9; ++idx)
			code += chars[1 + static_cast<int>((static_cast<float>(max - min)) * static_cast<uint16_t>(rand()) / 65536.0) + min];
	
		u->Account()->Extend("ns_set_email_passcode", new ExtensibleItemClass<Anope::string>(code));

		Anope::string subject = Config->MailEmailchangeSubject;
		Anope::string message = Config->MailEmailchangeMessage;

		subject = subject.replace_all_cs("%e", u->Account()->email);
		subject = subject.replace_all_cs("%N", Config->NetworkName);
		subject = subject.replace_all_cs("%c", code);

		message = message.replace_all_cs("%e", u->Account()->email);
		message = message.replace_all_cs("%N", Config->NetworkName);
		message = message.replace_all_cs("%c", code);

		return Mail::Send(u, u->Account(), bi, subject, message);
	}

 public:
	CommandNSSetEmail(Module *creator, const Anope::string &cname = "nickserv/set/email", size_t min = 0) : Command(creator, cname, min, min + 1)
	{
		this->SetDesc(_("Associate an E-mail address with your nickname"));
		this->SetSyntax(_("\037address\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		if (param.empty() && Config->NSForceEmail)
		{
			source.Reply(_("You cannot unset the e-mail on this network."));
			return;
		}
		else if (Config->NSSecureAdmins && source.nc != nc && nc->IsServicesOper())
		{
			source.Reply(_("You may not change the e-mail of other Services Operators."));
			return;
		}
		else if (!param.empty() && !Mail::Validate(param))
		{
			source.Reply(MAIL_X_INVALID, param.c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (!param.empty() && Config->NSConfirmEmailChanges && !source.IsServicesOper())
		{
			source.nc->Extend("ns_set_email", new ExtensibleItemClass<Anope::string>(param));
			Anope::string old = source.nc->email;
			source.nc->email = param;
			if (SendConfirmMail(source.GetUser(), source.service))
				source.Reply(_("A confirmation e-mail has been sent to \002%s\002. Follow the instructions in it to change your e-mail address."), param.c_str());
			source.nc->email = old;
		}
		else
		{
			if (!param.empty())
			{
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the email of " << nc->display << " to " << param;
				nc->email = param;
				source.Reply(_("E-mail address for \002%s\002 changed to \002%s\002."), nc->display.c_str(), param.c_str());
			}
			else
			{
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to unset the email of " << nc->display;
				nc->email.clear();
				source.Reply(_("E-mail address for \002%s\002 unset."), nc->display.c_str());
			}
		}
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params.size() ? params[0] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Associates the given E-mail address with your nickname.\n"
				"This address will be displayed whenever someone requests\n"
				"information on the nickname with the \002INFO\002 command."));
		return true;
	}
};

class CommandNSSASetEmail : public CommandNSSetEmail
{
 public:
	CommandNSSASetEmail(Module *creator) : CommandNSSetEmail(creator, "nickserv/saset/email", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 \037address\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Associates the given E-mail address with the nickname."));
		return true;
	}
};

class CommandNSSetGreet : public Command
{
 public:
	CommandNSSetGreet(Module *creator, const Anope::string &sname = "nickserv/set/greet", size_t min = 0) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Associate a greet message with your nickname"));
		this->SetSyntax(_("\037message\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (!param.empty())
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the greet of " << nc->display;
			nc->greet = param;
			source.Reply(_("Greet message for \002%s\002 changed to \002%s\002."), nc->display.c_str(), nc->greet.c_str());
		}
		else
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to unset the greet of " << nc->display;
			nc->greet.clear();
			source.Reply(_("Greet message for \002%s\002 unset."), nc->display.c_str());
		}

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params.size() > 0 ? params[0] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Makes the given message the greet of your nickname, that\n"
				"will be displayed when joining a channel that has GREET\n"
				"option enabled, provided that you have the necessary\n"
				"access on it."));
		return true;
	}
};

class CommandNSSASetGreet : public CommandNSSetGreet
{
 public:
	CommandNSSASetGreet(Module *creator) : CommandNSSetGreet(creator, "nickserv/saset/greet", 1)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 \037message\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Makes the given message the greet of the nickname, that\n"
				"will be displayed when joining a channel that has GREET\n"
				"option enabled, provided that the user has the necessary\n"
				"access on it."));
		return true;
	}
};

class CommandNSSetHide : public Command
{
 public:
	CommandNSSetHide(Module *creator, const Anope::string &sname = "nickserv/set/hide", size_t min = 2) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Hide certain pieces of nickname information"));
		this->SetSyntax(_("{EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param, const Anope::string &arg)
	{
		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		Anope::string onmsg, offmsg, flag;

		if (param.equals_ci("EMAIL"))
		{
			flag = "HIDE_EMAIL";
			onmsg = _("The E-mail address of \002%s\002 will now be hidden from %s INFO displays.");
			offmsg = _("The E-mail address of \002%s\002 will now be shown in %s INFO displays.");
		}
		else if (param.equals_ci("USERMASK"))
		{
			flag = "HIDE_MASK";
			onmsg = _("The last seen user@host mask of \002%s\002 will now be hidden from %s INFO displays.");
			offmsg = _("The last seen user@host mask of \002%s\002 will now be shown in %s INFO displays.");
		}
		else if (param.equals_ci("STATUS"))
		{
			flag = "HIDE_STATUS";
			onmsg = _("The services access status of \002%s\002 will now be hidden from %s INFO displays.");
			offmsg = _("The services access status of \002%s\002 will now be shown in %s INFO displays.");
		}
		else if (param.equals_ci("QUIT"))
		{
			flag = "HIDE_QUIT";
			onmsg = _("The last quit message of \002%s\002 will now be hidden from %s INFO displays.");
			offmsg = _("The last quit message of \002%s\002 will now be shown in %s INFO displays.");
		}
		else
		{
			this->OnSyntaxError(source, "HIDE");
			return;
		}

		if (arg.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change hide " << param << " to " << arg << " for " << nc->display;
			nc->ExtendMetadata(flag);
			source.Reply(onmsg.c_str(), nc->display.c_str(), Config->NickServ.c_str());
		}
		else if (arg.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change hide " << param << " to " << arg << " for " << nc->display;
			nc->Shrink(flag);
			source.Reply(offmsg.c_str(), nc->display.c_str(), Config->NickServ.c_str());
		}
		else
			this->OnSyntaxError(source, "HIDE");

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to prevent certain pieces of information from\n"
				"being displayed when someone does a %s \002INFO\002 on your\n"
				"nick.  You can hide your E-mail address (\002EMAIL\002), last seen\n"
				"user@host mask (\002USERMASK\002), your services access status\n"
				"(\002STATUS\002) and  last quit message (\002QUIT\002).\n"
				"The second parameter specifies whether the information should\n"
				"be displayed (\002OFF\002) or hidden (\002ON\002)."), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSASetHide : public CommandNSSetHide
{
 public:
	CommandNSSASetHide(Module *creator) : CommandNSSetHide(creator, "nickserv/saset/hide", 3)
	{
		this->SetSyntax("\037nickname\037 {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->ClearSyntax();
		this->Run(source, params[0], params[1], params[2]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to prevent certain pieces of information from\n"
				"being displayed when someone does a %s \002INFO\002 on the\n"
				"nick.  You can hide the E-mail address (\002EMAIL\002), last seen\n"
				"user@host mask (\002USERMASK\002), the services access status\n"
				"(\002STATUS\002) and  last quit message (\002QUIT\002).\n"
				"The second parameter specifies whether the information should\n"
				"be displayed (\002OFF\002) or hidden (\002ON\002)."), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSetKill : public Command
{
 public:
	CommandNSSetKill(Module *creator, const Anope::string &sname = "nickserv/set/kill", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Turn protection on or off"));
		this->SetSyntax(_("{ON | QUICK | IMMED | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Config->NoNicknameOwnership)
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
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			nc->ExtendMetadata("KILLPROTECT");
			nc->Shrink("KILL_QUICK");
			nc->Shrink("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill on for " << nc->display;
			source.Reply(_("Protection is now \002on\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("QUICK"))
		{
			nc->ExtendMetadata("KILLPROTECT");
			nc->ExtendMetadata("KILL_QUICK");
			nc->Shrink("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill quick for " << nc->display;
			source.Reply(_("Protection is now \002on\002 for \002%s\002, with a reduced delay."), nc->display.c_str());
		}
		else if (param.equals_ci("IMMED"))
		{
			if (Config->NSAllowKillImmed)
			{
				nc->ExtendMetadata("KILLPROTECT");
				nc->ExtendMetadata("KILL_IMMED");
				nc->Shrink("KILL_QUICK");
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill immed for " << nc->display;
				source.Reply(_("Protection is now \002on\002 for \002%s\002, with no delay."), nc->display.c_str());
			}
			else
				source.Reply(_("The \002IMMED\002 option is not available on this network."));
		}
		else if (param.equals_ci("OFF"))
		{
			nc->Shrink("KILLPROTECT");
			nc->Shrink("KILL_QUICK");
			nc->Shrink("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable kill for " << nc->display;
			source.Reply(_("Protection is now \002off\002 for \002%s\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "KILL");

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns the automatic protection option for your nick\n"
				"on or off.  With protection on, if another user\n"
				"tries to take your nick, they will be given one minute to\n"
				"change to another nick, after which %s will forcibly change\n"
				"their nick.\n"
				" \n"
				"If you select \002QUICK\002, the user will be given only 20 seconds\n"
				"to change nicks instead of the usual 60.  If you select\n"
				"\002IMMED\002, user's nick will be changed immediately \037without\037 being\n"
				"warned first or given a chance to change their nick; please\n"
				"do not use this option unless necessary. Also, your\n"
				"network's administrators may have disabled this option."), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSASetKill : public CommandNSSetKill
{
 public:
	CommandNSSASetKill(Module *creator) : CommandNSSetKill(creator, "nickserv/saset/kill", 2)
	{
		this->SetSyntax(_("\037nickname\037 {ON | QUICK | IMMED | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->ClearSyntax();
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
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
				"network's administrators may have disabled this option."), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSetLanguage : public Command
{
 public:
	CommandNSSetLanguage(Module *creator, const Anope::string &sname = "nickserv/set/language", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Set the language Services will use when messaging you"));
		this->SetSyntax(_("\037language\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		for (unsigned j = 0; j < Language::Languages.size(); ++j)
		{
			if (param == "en" || Language::Languages[j] == param)
				break;
			else if (j + 1 == Language::Languages.size())
			{
				this->OnSyntaxError(source, "");
				return;
			}
		}

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the langauge of " << nc->display << " to " << param;

		nc->language = param != "en" ? param : "";
		source.Reply(_("Language changed to \002English\002."));

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &param) anope_override
	{
		this->Run(source, source.nc->display, param[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the language Services uses when sending messages to\n"
				"you (for example, when responding to a command you send).\n"
				"\037language\037 should be chosen from the following list of\n"
				"supported languages:"));

		source.Reply("         en (English)");
		for (unsigned j = 0; j < Language::Languages.size(); ++j)
		{
			const Anope::string &langname = Language::Translate(Language::Languages[j].c_str(), _("English"));
			if (langname == "English")
				continue;
			source.Reply("         %s (%s)", Language::Languages[j].c_str(), langname.c_str());
		}

		return true;
	}
};

class CommandNSSASetLanguage : public CommandNSSetLanguage
{
 public:
	CommandNSSASetLanguage(Module *creator) : CommandNSSetLanguage(creator, "nickserv/saset/language", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 \037language\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the language Services uses when sending messages to\n"
				"the given user (for example, when responding to a command they send).\n"
				"\037language\037 should be chosen from the following list of\n"
				"supported languages:"));
		source.Reply("         en (English)");
		for (unsigned j = 0; j < Language::Languages.size(); ++j)
		{
			const Anope::string &langname = Language::Translate(Language::Languages[j].c_str(), _("English"));
			if (langname == "English")
				continue;
			source.Reply("         %s (%s)", Language::Languages[j].c_str(), langname.c_str());
		}
		return true;
	}
};

class CommandNSSetMessage : public Command
{
 public:
	CommandNSSetMessage(Module *creator, const Anope::string &sname = "nickserv/set/message", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Change the communication method of Services"));
		this->SetSyntax(_("{ON | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		if (!Config->UsePrivmsg)
		{
			source.Reply(_("You cannot %s on this network."), source.command.c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable " << source.command << " for " << nc->display;
			nc->ExtendMetadata("MSG");
			source.Reply(_("Services will now reply to \002%s\002 with \002messages\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable " << source.command << " for " << nc->display;
			nc->Shrink("MSG");
			source.Reply(_("Services will now reply to \002%s\002 with \002notices\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "MSG");

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to choose the way Services are communicating with\n"
				"you. With \002MSG\002 set, Services will use messages, else they'll\n"
				"use notices."));
		return true;
	}

	void OnServHelp(CommandSource &source) anope_override
	{
		if (Config->UsePrivmsg)
			Command::OnServHelp(source);
	}
};

class CommandNSSASetMessage : public CommandNSSetMessage
{
 public:
	CommandNSSASetMessage(Module *creator) : CommandNSSetMessage(creator, "nickserv/saset/message", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to choose the way Services are communicating with\n"
				"the given user. With \002MSG\002 set, Services will use messages,\n"
				"else they'll use notices."));
		return true;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params[1]);
	}
};

class CommandNSSetPrivate : public Command
{
 public:
	CommandNSSetPrivate(Module *creator, const Anope::string &sname = "nickserv/set/private", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(Anope::printf(_("Prevent the nickname from appearing in a \002%s%s LIST\002"), Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str()));
		this->SetSyntax(_("{ON | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable private for " << nc->display;
			nc->ExtendMetadata("PRIVATE");
			source.Reply(_("Private option is now \002on\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable private for " << nc->display;
			nc->Shrink("PRIVATE");
			source.Reply(_("Private option is now \002off\002 for \002%s\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "PRIVATE");

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns %s's privacy option on or off for your nick.\n"
				"With \002PRIVATE\002 set, your nickname will not appear in\n"
				"nickname lists generated with %s's \002LIST\002 command.\n"
				"(However, anyone who knows your nickname can still get\n"
				"information on it using the \002INFO\002 command.)"),
				Config->NickServ.c_str(), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSASetPrivate : public CommandNSSetPrivate
{
 public:
	CommandNSSASetPrivate(Module *creator) : CommandNSSetPrivate(creator, "nickserv/saset/private", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns %s's privacy option on or off for the nick.\n"
				"With \002PRIVATE\002 set, the nickname will not appear in\n"
				"nickname lists generated with %s's \002LIST\002 command.\n"
				"(However, anyone who knows the nickname can still get\n"
				"information on it using the \002INFO\002 command.)"),
				Config->NickServ.c_str(), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSetSecure : public Command
{
 public:
	CommandNSSetSecure(Module *creator, const Anope::string &sname = "nickserv/set/secure", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Turn nickname security on or off"));
		this->SetSyntax(_("{ON | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable secure for " << nc->display;
			nc->ExtendMetadata("SECURE");
			source.Reply(_("Secure option is now \002on\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable secure for " << nc->display;
			nc->Shrink("SECURE");
			source.Reply(_("Secure option is now \002off\002 for \002%s\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "SECURE");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns %s's security features on or off for your\n"
				"nick. With \002SECURE\002 set, you must enter your password\n"
				"before you will be recognized as the owner of the nick,\n"
				"regardless of whether your address is on the access\n"
				"list. However, if you are on the access list, %s\n"
				"will not auto-kill you regardless of the setting of the\n"
				"\002KILL\002 option."), Config->NickServ.c_str(), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSASetSecure : public CommandNSSetSecure
{
 public:
	CommandNSSASetSecure(Module *creator) : CommandNSSetSecure(creator, "nickserv/saset/secure", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns %s's security features on or off for your\n"
				"nick. With \002SECURE\002 set, you must enter your password\n"
				"before you will be recognized as the owner of the nick,\n"
				"regardless of whether your address is on the access\n"
				"list. However, if you are on the access list, %s\n"
				"will not auto-kill you regardless of the setting of the\n"
				"\002KILL\002 option."), Config->NickServ.c_str(), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSASetNoexpire : public Command
{
 public:
	CommandNSSASetNoexpire(Module *creator) : Command(creator, "nickserv/saset/noexpire", 1, 2)
	{
		this->SetDesc(_("Prevent the nickname from expiring"));
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		NickAlias *na = NickAlias::Find(params[0]);
		if (na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		Anope::string param = params.size() > 1 ? params[1] : "";

		if (param.equals_ci("ON"))
		{
			Log(LOG_ADMIN, source, this) << "to enable noexpire " << na->nc->display;
			na->ExtendMetadata("NO_EXPIRE");
			source.Reply(_("Nick %s \002will not\002 expire."), na->nick.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(LOG_ADMIN, source, this) << "to disable noexpire " << na->nc->display;
			na->Shrink("NO_EXPIRE");
			source.Reply(_("Nick %s \002will\002 expire."), na->nick.c_str());
		}
		else
			this->OnSyntaxError(source, "NOEXPIRE");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets whether the given nickname will expire.  Setting this\n"
				"to \002ON\002 prevents the nickname from expiring."));
		return true;
	}
};

class NSSet : public Module
{
	CommandNSSet commandnsset;
	CommandNSSASet commandnssaset;

	CommandNSSetAutoOp commandnssetautoop;
	CommandNSSASetAutoOp commandnssasetautoop;

	CommandNSSetChanstats commandnssetchanstats;
	CommandNSSASetChanstats commandnssasetchanstats;
	bool NSDefChanstats;

	CommandNSSetDisplay commandnssetdisplay;
	CommandNSSASetDisplay commandnssasetdisplay;

	CommandNSSetEmail commandnssetemail;
	CommandNSSASetEmail commandnssasetemail;
	
	CommandNSSetGreet commandnssetgreet;
	CommandNSSASetGreet commandnssasetgreet;

	CommandNSSetHide commandnssethide;
	CommandNSSASetHide commandnssasethide;

	CommandNSSetKill commandnssetkill;
	CommandNSSASetKill commandnssasetkill;

	CommandNSSetLanguage commandnssetlanguage;
	CommandNSSASetLanguage commandnssasetlanguage;

	CommandNSSetMessage commandnssetmessage;
	CommandNSSASetMessage commandnssasetmessage;

	CommandNSSetPassword commandnssetpassword;
	CommandNSSASetPassword commandnssasetpassword;

	CommandNSSetPrivate commandnssetprivate;
	CommandNSSASetPrivate commandnssasetprivate;

	CommandNSSetSecure commandnssetsecure;
	CommandNSSASetSecure commandnssasetsecure;

	CommandNSSASetNoexpire commandnssasetnoexpire;

 public:
	NSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsset(this), commandnssaset(this),
		commandnssetautoop(this), commandnssasetautoop(this),
		commandnssetchanstats(this), commandnssasetchanstats(this), NSDefChanstats(false),
		commandnssetdisplay(this), commandnssasetdisplay(this),
		commandnssetemail(this), commandnssasetemail(this),
		commandnssetgreet(this), commandnssasetgreet(this),
		commandnssethide(this), commandnssasethide(this),
		commandnssetkill(this), commandnssasetkill(this),
		commandnssetlanguage(this), commandnssasetlanguage(this),
		commandnssetmessage(this), commandnssasetmessage(this),
		commandnssetpassword(this), commandnssasetpassword(this),
		commandnssetprivate(this), commandnssasetprivate(this),
		commandnssetsecure(this), commandnssasetsecure(this),
		commandnssasetnoexpire(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnNickRegister, I_OnPreCommand };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		NSDefChanstats = config.ReadFlag("chanstats", "NSDefChanstats", "0", 0);
	}

	void OnNickRegister(NickAlias *na) anope_override
	{
		if (NSDefChanstats)
			na->nc->ExtendMetadata("STATS");
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) anope_override
	{
		NickCore *uac = source.nc;

		if (command->name == "nickserv/confirm" && !params.empty() && uac)
		{
			Anope::string *new_email = uac->GetExt<ExtensibleItemClass<Anope::string> *>("ns_set_email"), *passcode = uac->GetExt<ExtensibleItemClass<Anope::string> *>("ns_set_email_passcode");
			if (new_email && passcode)
			{
				if (params[0] == *passcode)
				{
					uac->email = *new_email;
					Log(LOG_COMMAND, source, command) << "to confirm their email address change to " << uac->email;
					source.Reply(_("Your email address has been changed to \002%s\002."), uac->email.c_str());
					uac->Shrink("ns_set_email");
					uac->Shrink("ns_set_email_passcode");
					return EVENT_STOP;
				}
			}
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(NSSet)
