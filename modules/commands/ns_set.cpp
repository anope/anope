/* NickServ core functions
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
#include "modules/ns_info.h"
#include "modules/ns_set.h"
#include "modules/nickserv.h"

class CommandNSSet : public Command
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
		source.Reply(_("Sets various options on your account\n"
		               "\n"
		               "Available options:"));

		Anope::string this_name = source.command;
		bool hide_privileged_commands = Config->GetBlock("options")->Get<bool>("hideprivilegedcommands");
		for (CommandInfo::map::const_iterator it = source.service->commands.begin(), it_end = source.service->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;

			if (c_name.find_ci(this_name + " ") == 0)
			{
				ServiceReference<Command> c("Command", info.name);
				if (!c)
					continue;
				else if (!hide_privileged_commands)
					; // Always show with hide_privileged_commands disabled
				else if (!c->AllowUnregistered() && !source.GetAccount())
					continue;
				else if (!info.permission.empty() && !source.HasCommand(info.permission))
					continue;

				source.command = c_name;
				c->OnServHelp(source);
			}
		}

		CommandInfo *help = source.service->FindCommand("generic/help");
		if (help)
			source.Reply(_("Type \002{0}{1} {2} {3} \037option\037\002 for more information on a particular option."),
			               Config->StrictPrivmsg, source.service->nick, help->cname, this_name);

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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->OnSyntaxError(source, "");
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets various options on other users accounts\n"
		               "\n"
		               "Available options:"));

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

		CommandInfo *help = source.service->FindCommand("generic/help");
		if (help)
			source.Reply(_("Type \002{0}{1} {2} {3} \037option\037\002 for more information on a particular option."),
			               Config->StrictPrivmsg, source.service->nick, help->cname, this_name);

		return true;
	}
};

class CommandNSSetPassword : public Command
{
 public:
	CommandNSSetPassword(Module *creator) : Command(creator, "nickserv/set/password", 1)
	{
		this->SetDesc(_("Changes your password"));
		this->SetSyntax(_("\037new-password\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &param = params[0];
		unsigned len = param.length();

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		if (source.GetNick().equals_ci(param) || (Config->GetBlock("options")->Get<bool>("strictpasswords") && len < 5))
		{
			source.Reply(_("Please try again with a more obscure password. Passwords should be at least five characters long, should not be something easily guessed (e.g. your real name or your nick), and cannot contain the space or tab characters."));
			return;
		}

		if (len > Config->GetModule("nickserv")->Get<unsigned>("passlen", "32"))
		{
			source.Reply(_("Your password is too long, it can not contain more than \002{0}\002 characters."), Config->GetModule("nickserv")->Get<unsigned>("passlen", "32"));
			return;
		}

		Log(LOG_COMMAND, source, this) << "to change their password";

		Anope::Encrypt(param, source.nc->pass);
		Anope::string tmp_pass;
		if (Anope::Decrypt(source.nc->pass, tmp_pass) == 1)
			source.Reply(_("Password for \002{0}\002 changed to \002{1]\002."), source.nc->display, tmp_pass);
		else
			source.Reply(_("Password for \002{0}\002 changed."), source.nc->display);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Changes your password to \037new-password\037."));
		return true;
	}
};

class CommandNSSASetPassword : public Command
{
 public:
	CommandNSSASetPassword(Module *creator) : Command(creator, "nickserv/saset/password", 2, 2)
	{
		this->SetDesc(_("Changes the password of another user"));
		this->SetSyntax(_("\037account\037 \037new-password\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const NickServ::Nick *setter_na = NickServ::FindNick(params[0]);
		if (setter_na == NULL)
		{
			source.Reply(_("\002{0}\002 isn't registered."), params[0]);
			return;
		}
		NickServ::Account *nc = setter_na->nc;

		size_t len = params[1].length();

		if (Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && source.nc != nc && nc->IsServicesOper())
		{
			source.Reply(_("You may not change the password of other Services Operators."));
			return;
		}

		if (nc->display.equals_ci(params[1]) || (Config->GetBlock("options")->Get<bool>("strictpasswords") && len < 5))
		{
			source.Reply(_("Please try again with a more obscure password. Passwords should be at least five characters long, should not be something easily guessed (e.g. your real name or your nick), and cannot contain the space or tab characters."));
			return;
		}

		if (len > Config->GetModule("nickserv")->Get<unsigned>("passlen", "32"))
		{
			source.Reply(_("Your password is too long, it can not contain more than \002{0}\002 characters."), Config->GetModule("nickserv")->Get<unsigned>("passlen", "32"));
			return;
		}

		Log(LOG_ADMIN, source, this) << "to change the password of " << nc->display;

		Anope::Encrypt(params[1], nc->pass);
		Anope::string tmp_pass;
		if (Anope::Decrypt(nc->pass, tmp_pass) == 1)
			source.Reply(_("Password for \002{0}\002 changed to \002{1}\002."), nc->display, tmp_pass);
		else
			source.Reply(_("Password for \002{0}\002 changed."), nc->display);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Changes the password of \037account\037 to \037new-password\037."));
		return true;
	}
};

class CommandNSSetAutoOp : public Command
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
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const NickServ::Nick *na = NickServ::FindNick(user);
		if (na == NULL)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->nc;

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetNickOption(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable autoop for " << na->nc->display;
			nc->Extend<bool>("AUTOOP");
			source.Reply(_("Services will from now on set status modes on \002{0}\002 in channels."), nc->display);
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable autoop for " << na->nc->display;
			nc->Shrink<bool>("AUTOOP");
			source.Reply(_("Services will no longer set status modes on \002{0}\002 in channels."), nc->display);
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
		source.Reply(_("Sets whether you will be given your channel status modes automatically when you join a channel."
		                " Note that depending on channel settings some modes may not get set automatically."));
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
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const NickServ::Nick *user_na = NickServ::FindNick(user), *na = NickServ::FindNick(param);

		if (Config->GetModule("nickserv")->Get<bool>("nonicknameownership"))
		{
			source.Reply(_("This command may not be used on this network because nickname ownership is disabled."));
			return;
		}

		if (user_na == NULL)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}

		if (!na || *na->nc != *user_na->nc)
		{
			source.Reply(_("The new display must be a nickname of the nickname group \002{0}\02."), user_na->nc->display);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetNickOption(&Event::SetNickOption::OnSetNickOption, source, this, user_na->nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		Log(user_na->nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the display of " << user_na->nc->display << " to " << na->nick;

		user_na->nc->SetDisplay(na);
		source.Reply(_("The new display is now \002{0}\002."), user_na->nc->display);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Changes the display used to refer to your nickname group in services. The new display nickname must be a nickname of your group."));
		return true;
	}
};

class CommandNSSASetDisplay : public CommandNSSetDisplay
{
 public:
	CommandNSSASetDisplay(Module *creator) : CommandNSSetDisplay(creator, "nickserv/saset/display", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037account\037 \037new-display\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Changes the display used to refer to the nickname group \037account\037 in services. The new display nickname must be a nickname in the group of \037account\037."));
		return true;
	}
};

class CommandNSSetEmail : public Command
{
	static bool SendConfirmMail(User *u, BotInfo *bi, const Anope::string &new_email)
	{
		Anope::string code = Anope::Random(9);

		std::pair<Anope::string, Anope::string> *n = u->Account()->Extend<std::pair<Anope::string, Anope::string> >("ns_set_email");
		n->first = new_email;
		n->second = code;

		Anope::string subject = Config->GetBlock("mail")->Get<const Anope::string>("emailchange_subject"),
			message = Config->GetBlock("mail")->Get<const Anope::string>("emailchange_message");

		subject = subject.replace_all_cs("%e", u->Account()->email);
		subject = subject.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<const Anope::string>("networkname"));
		subject = subject.replace_all_cs("%c", code);

		message = message.replace_all_cs("%e", u->Account()->email);
		message = message.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<const Anope::string>("networkname"));
		message = message.replace_all_cs("%c", code);

		Anope::string old = u->Account()->email;
		u->Account()->email = new_email;
		bool b = Mail::Send(u, u->Account(), bi, subject, message);
		u->Account()->email = old;
		return b;
	}

 public:
	CommandNSSetEmail(Module *creator, const Anope::string &cname = "nickserv/set/email", size_t min = 0) : Command(creator, cname, min, min + 1)
	{
		this->SetDesc(_("Associate an E-mail address with your nickname"));
		this->SetSyntax(_("\037address\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->nc;

		if (nc->HasExt("UNCONFIRMED"))
		{
			source.Reply(_("You may not change the email of an unconfirmed account."));
			return;
		}

		if (param.empty() && Config->GetModule("nickserv")->Get<bool>("forceemail", "yes"))
		{
			source.Reply(_("You cannot unset the e-mail on this network."));
			return;
		}

		if (Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && source.nc != nc && nc->IsServicesOper())
		{
			source.Reply(_("You may not change the e-mail of other Services Operators."));
			return;
		}

		if (!param.empty() && !Mail::Validate(param))
		{
			source.Reply(_("\002{0}\002 is not a valid e-mail address."), param);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetNickOption(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (!param.empty() && Config->GetModule("nickserv")->Get<bool>("confirmemailchanges") && !source.IsServicesOper())
		{
			if (SendConfirmMail(source.GetUser(), source.service, param))
				source.Reply(_("A confirmation e-mail has been sent to \002{0}\002. Follow the instructions in it to change your e-mail address."), param);
		}
		else
		{
			if (!param.empty())
			{
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the email of " << nc->display << " to " << param;
				nc->email = param;
				source.Reply(_("E-mail address for \002{0}\002 changed to \002{1}\002."), nc->display, param);
			}
			else
			{
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to unset the email of " << nc->display;
				nc->email.clear();
				source.Reply(_("E-mail address for \002{0}\002 unset."), nc->display);
			}
		}
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params.size() ? params[0] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Changes your email address to \037address\037."));
		return true;
	}
};

class CommandNSSASetEmail : public CommandNSSetEmail
{
 public:
	CommandNSSASetEmail(Module *creator) : CommandNSSetEmail(creator, "nickserv/saset/email", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037account\037 \037address\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Changes the email address of \037account\037 to \037address\037."));
		return true;
	}
};

class CommandNSSetKeepModes : public Command
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
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->nc;

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetNickOption(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable keepmodes for " << nc->display;
			nc->Extend<bool>("NS_KEEP_MODES");
			source.Reply(_("Keep modes for \002{0}\002 is now \002on\002."), nc->display);
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable keepmodes for " << nc->display;
			nc->Shrink<bool>("NS_KEEP_MODES");
			source.Reply(_("Keep modes for \002{0}\002 is now \002off\002."), nc->display);
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
		source.Reply(_("Enables or disables keepmodes for your account. If keepmodes is enabled, services will remember your usermodes and attempt to re-set them the next time you log on."));
		return true;
	}
};

class CommandNSSASetKeepModes : public CommandNSSetKeepModes
{
 public:
	CommandNSSASetKeepModes(Module *creator) : CommandNSSetKeepModes(creator, "nickserv/saset/keepmodes", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037account\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables keepmodes for \037account\037. If keep modes is enabled, services will remember users' usermodes and attempt to re-set them the next time they log pn."));
		return true;
	}
};

class CommandNSSetKill : public Command
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
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		if (Config->GetModule("nickserv")->Get<bool>("nonicknameownership"))
		{
			source.Reply(_("This command may not be used on this network because nickname ownership is disabled."));
			return;
		}

		const NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->nc;

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetNickOption(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			nc->Extend<bool>("KILLPROTECT");
			nc->Shrink<bool>("KILL_QUICK");
			nc->Shrink<bool>("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill on for " << nc->display;
			source.Reply(_("Protection is now \002on\002 for \002{0}\002."), nc->display);
		}
		else if (param.equals_ci("QUICK"))
		{
			nc->Extend<bool>("KILLPROTECT");
			nc->Extend<bool>("KILL_QUICK");
			nc->Shrink<bool>("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill quick for " << nc->display;
			source.Reply(_("Protection is now \002on\002 for \002{0}\002, with a reduced delay."), nc->display);
		}
		else if (param.equals_ci("IMMED"))
		{
			if (Config->GetModule(this->owner)->Get<bool>("allowkillimmed"))
			{
				nc->Extend<bool>("KILLPROTECT");
				nc->Shrink<bool>("KILL_QUICK");
				nc->Extend<bool>("KILL_IMMED");
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill immed for " << nc->display;
				source.Reply(_("Protection is now \002on\002 for \002{0}\002, with no delay."), nc->display);
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
			source.Reply(_("Protection is now \002off\002 for \002{0}\002."), nc->display);
		}
		else
			this->OnSyntaxError(source, "KILL");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Turns the automatic protection option for your account on or off."
		               " With protection on, if another user tries to use one of your nicknames, they will be given {0} to change to another their nickname, after which {1} will forcibly change their nickname.\n"
				"\n"
				"If you select \002QUICK\002, the user will be given only {1} to change their nick instead {0}."
				" If you select \002IMMED\002, the user's nickname will be changed immediately \037without\037 being warned first or given a chance to change their nick."
				" With this set, the only way to use the nickname is to match an entry on the account's access list."));
		return true;
	}
};

class CommandNSSASetKill : public CommandNSSetKill
{
 public:
	CommandNSSASetKill(Module *creator) : CommandNSSetKill(creator, "nickserv/saset/kill", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037account\037 {ON | QUICK | IMMED | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Turns the automatic protection option for \037account\037 on or off."
		               " With protection on, if another user tries to use one of the nicknames in the group, they will be given {0} to change to another their nickname, after which {1} will forcibly change their nickname.\n"
				"\n"
				"If you select \002QUICK\002, the user will be given only {1} to change their nick instead {0}."
				" If you select \002IMMED\002, the user's nickname will be changed immediately \037without\037 being warned first or given a chance to change their nick."
				" With this set, the only way to use the nickname is to match an entry on the account's access list."));
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
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->nc;

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetNickOption(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param != "en")
			for (unsigned j = 0; j < Language::Languages.size(); ++j)
			{
				if (Language::Languages[j] == param)
					break;
				else if (j + 1 == Language::Languages.size())
				{
					this->OnSyntaxError(source, "");
					return;
				}
			}

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the language of " << nc->display << " to " << param;

		nc->language = param;
		source.Reply(_("Language changed to \002English\002."));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &param) override
	{
		this->Run(source, source.nc->display, param[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Changes the language services will use when sending messages to you (for example, when responding to a command you send). \037language\037 should be chosen from the following list of supported languages:"));

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
		this->SetSyntax(_("\037account\037 \037language\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Changes the language services will use when sending messages to the given user (for example, when responding to a command they send). \037language\037 should be chosen from the following list of supported languages:"));
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
		this->SetSyntax("{ON | OFF}");
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->nc;

		if (!Config->GetBlock("options")->Get<bool>("useprivmsg"))
		{
			source.Reply(_("You cannot %s on this network."), source.command);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetNickOption(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable " << source.command << " for " << nc->display;
			nc->Extend<bool>("MSG");
			source.Reply(_("Services will now reply to \002{0}\002 with \002messages\002."), nc->display);
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable " << source.command << " for " << nc->display;
			nc->Shrink<bool>("MSG");
			source.Reply(_("Services will now reply to \002{0}\002 with \002notices\002."), nc->display);
		}
		else
			this->OnSyntaxError(source, "MSG");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		Anope::string cmd = source.command;
		size_t i = cmd.find_last_of(' ');
		if (i != Anope::string::npos)
			cmd = cmd.substr(i + 1);

		source.Reply(_("Allows you to choose the way services communicate with you. With \002{0}\002 set, services will use messages instead of notices."), cmd);
		return true;
	}

	void OnServHelp(CommandSource &source) override
	{
		if (!Config->GetBlock("options")->Get<bool>("useprivmsg"))
			Command::OnServHelp(source);
	}
};

class CommandNSSASetMessage : public CommandNSSetMessage
{
 public:
	CommandNSSASetMessage(Module *creator) : CommandNSSetMessage(creator, "nickserv/saset/message", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037account\037 {ON | OFF}"));
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		Anope::string cmd = source.command;
		size_t i = cmd.find_last_of(' ');
		if (i != Anope::string::npos)
			cmd = cmd.substr(i + 1);

		source.Reply(_("Allows you to choose the way services communicate with the given user. With \002{0}\002 set, services will use messages instead of notices."), cmd);
		return true;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}
};

class CommandNSSetSecure : public Command
{
 public:
	CommandNSSetSecure(Module *creator, const Anope::string &sname = "nickserv/set/secure", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Turn nickname security on or off"));
		this->SetSyntax("{ON | OFF}");
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->nc;

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetNickOption(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable secure for " << nc->display;
			nc->Extend<bool>("NS_SECURE");
			source.Reply(_("Secure option is now \002on\002 for \002{0}\002."), nc->display);
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable secure for " << nc->display;
			nc->Shrink<bool>("NS_SECURE");
			source.Reply(_("Secure option is now \002off\002 for \002{0}\002."), nc->display);
		}
		else
			this->OnSyntaxError(source, "SECURE");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Turns the security feature on or off for your account."
		               " With \002SECURE\002 set, you must enter your password before you will be recognized as the owner of the account,"
		               " regardless of whether your address is on the access list or not."
		               " However, if you are on the access list, services will not force you to change your nickname, regardless of the setting of the \002kill\002 option."));
		return true;
	}
};

class CommandNSSASetSecure : public CommandNSSetSecure
{
 public:
	CommandNSSASetSecure(Module *creator) : CommandNSSetSecure(creator, "nickserv/saset/secure", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037account\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Turns the security feature on or off for \037account\037."
		               " With \002SECURE\002 set, you the user must enter their password before they will be recognized as the owner of the account,"
		               " regardless of whether their address address is on the access list or not."
		               " However, if they are on the access list, services will not force them to change your nickname, regardless of the setting of the \002kill\002 option."));
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const Anope::string &user = params[0];
		NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->nc;

		Anope::string param = params.size() > 1 ? params[1] : "";

		if (param.equals_ci("ON"))
		{
			Log(LOG_ADMIN, source, this) << "to enable noexpire for " << na->nc->display;
			na->Extend<bool>("NS_NO_EXPIRE");
			source.Reply(_("\002{0}\002 \002will not\002 expire."), na->nick);
		}
		else if (param.equals_ci("OFF"))
		{
			Log(LOG_ADMIN, source, this) << "to disable noexpire for " << na->nc->display;
			na->Shrink<bool>("NS_NO_EXPIRE");
			source.Reply(_("\002{0}\002 \002will\002 expire."), na->nick);
		}
		else
			this->OnSyntaxError(source, "NOEXPIRE");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Sets whether the given nickname will expire. Setting this to \002ON\002 prevents the nickname from expiring."));
		return true;
	}
};

class NSSet : public Module
	, public EventHook<Event::PreCommand>
	, public EventHook<Event::SetCorrectModes>
	, public EventHook<NickServ::Event::PreNickExpire>
	, public EventHook<Event::NickInfo>
	, public EventHook<Event::UserModeSet>
	, public EventHook<Event::UserModeUnset>
	, public EventHook<Event::UserLogin>
{
	CommandNSSet commandnsset;
	CommandNSSASet commandnssaset;

	CommandNSSetAutoOp commandnssetautoop;
	CommandNSSASetAutoOp commandnssasetautoop;

	CommandNSSetDisplay commandnssetdisplay;
	CommandNSSASetDisplay commandnssasetdisplay;

	CommandNSSetEmail commandnssetemail;
	CommandNSSASetEmail commandnssasetemail;

	CommandNSSetKeepModes commandnssetkeepmodes;
	CommandNSSASetKeepModes commandnssasetkeepmodes;

	CommandNSSetKill commandnssetkill;
	CommandNSSASetKill commandnssasetkill;

	CommandNSSetLanguage commandnssetlanguage;
	CommandNSSASetLanguage commandnssasetlanguage;

	CommandNSSetMessage commandnssetmessage;
	CommandNSSASetMessage commandnssasetmessage;

	CommandNSSetPassword commandnssetpassword;
	CommandNSSASetPassword commandnssasetpassword;

	CommandNSSetSecure commandnssetsecure;
	CommandNSSASetSecure commandnssasetsecure;

	CommandNSSASetNoexpire commandnssasetnoexpire;

	SerializableExtensibleItem<bool> autoop, killprotect, kill_quick, kill_immed,
		message, secure, noexpire;

	struct KeepModes : SerializableExtensibleItem<bool>
	{
		KeepModes(Module *m, const Anope::string &n) : SerializableExtensibleItem<bool>(m, n) { }

		void ExtensibleSerialize(const Extensible *e, const Serializable *s, Serialize::Data &data) const override
		{
			SerializableExtensibleItem<bool>::ExtensibleSerialize(e, s, data);

			if (s->GetSerializableType()->GetName() != "NickServ::Account")
				return;

			const NickServ::Account *nc = anope_dynamic_static_cast<const NickServ::Account *>(s);
			Anope::string modes;
			for (User::ModeList::const_iterator it = nc->last_modes.begin(); it != nc->last_modes.end(); ++it)
			{
				if (!modes.empty())
					modes += " ";
				modes += it->first;
				if (!it->second.empty())
					modes += "," + it->second;
			}
			data["last_modes"] << modes;
		}

		void ExtensibleUnserialize(Extensible *e, Serializable *s, Serialize::Data &data) override
		{
			SerializableExtensibleItem<bool>::ExtensibleUnserialize(e, s, data);

			if (s->GetSerializableType()->GetName() != "NickServ::Account")
				return;

			NickServ::Account *nc = anope_dynamic_static_cast<NickServ::Account *>(s);
			Anope::string modes;
			data["last_modes"] >> modes;
			for (spacesepstream sep(modes); sep.GetToken(modes);)
			{
				size_t c = modes.find(',');
				if (c == Anope::string::npos)
					nc->last_modes.insert(std::make_pair(modes, ""));
				else
					nc->last_modes.insert(std::make_pair(modes.substr(0, c), modes.substr(c + 1)));
			}
		}
	} keep_modes;

	/* email, passcode */
	PrimitiveExtensibleItem<std::pair<Anope::string, Anope::string > > ns_set_email;

 public:
	NSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::PreCommand>("OnPreCommand")
		, EventHook<Event::SetCorrectModes>("OnSetCorrectModes")
		, EventHook<NickServ::Event::PreNickExpire>("OnPreNickExpire")
		, EventHook<Event::NickInfo>("OnNickInfo")
		, EventHook<Event::UserModeSet>("OnUserModeSet")
		, EventHook<Event::UserModeUnset>("OnUserModeUnset")
		, EventHook<Event::UserLogin>("OnUserLogin")
		, commandnsset(this)
		, commandnssaset(this)
		, commandnssetautoop(this)
		, commandnssasetautoop(this)
		, commandnssetdisplay(this)
		, commandnssasetdisplay(this)
		, commandnssetemail(this)
		, commandnssasetemail(this)
		, commandnssetkeepmodes(this)
		, commandnssasetkeepmodes(this)
		, commandnssetkill(this)
		, commandnssasetkill(this)
		, commandnssetlanguage(this)
		, commandnssasetlanguage(this)
		, commandnssetmessage(this)
		, commandnssasetmessage(this)
		, commandnssetpassword(this)
		, commandnssasetpassword(this)
		, commandnssetsecure(this)
		, commandnssasetsecure(this)
		, commandnssasetnoexpire(this)

		, autoop(this, "AUTOOP")
		, killprotect(this, "KILLPROTECT")
		, kill_quick(this, "KILL_QUICK")
		, kill_immed(this, "KILL_IMMED")
		, message(this, "MSG")
		, secure(this, "NS_SECURE")
		, noexpire(this, "NS_NO_EXPIRE")

		, keep_modes(this, "NS_KEEP_MODES")
		, ns_set_email(this, "ns_set_email")
	{

	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) override
	{
		NickServ::Account *uac = source.nc;

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

	void OnSetCorrectModes(User *user, Channel *chan, ChanServ::AccessGroup &access, bool &give_modes, bool &take_modes) override
	{
		if (chan->ci)
		{
			/* Only give modes if autoop is set */
			give_modes &= !user->Account() || autoop.HasExt(user->Account());
		}
	}

	void OnPreNickExpire(NickServ::Nick *na, bool &expire) override
	{
		if (noexpire.HasExt(na))
			expire = false;
	}

	void OnNickInfo(CommandSource &source, NickServ::Nick *na, InfoFormatter &info, bool show_hidden) override
	{
		if (!show_hidden)
			return;

		if (kill_immed.HasExt(na->nc))
			info.AddOption(_("Immediate protection"));
		else if (kill_quick.HasExt(na->nc))
			info.AddOption(_("Quick protection"));
		else if (killprotect.HasExt(na->nc))
			info.AddOption(_("Protection"));
		if (secure.HasExt(na->nc))
			info.AddOption(_("Security"));
		if (message.HasExt(na->nc))
			info.AddOption(_("Message mode"));
		if (autoop.HasExt(na->nc))
			info.AddOption(_("Auto-op"));
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
			for (User::ModeList::iterator it = modes.begin(); it != modes.end(); ++it)
			{
				UserMode *um = ModeManager::FindUserModeByName(it->first);
				/* if the null user can set the mode, then it's probably safe */
				if (um && um->CanSet(NULL))
					u->SetMode(NULL, it->first, it->second);
			}
		}
	}
};

MODULE_INIT(NSSet)
