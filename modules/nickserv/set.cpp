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
#include "modules/nickserv/info.h"
#include "modules/nickserv/set.h"
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
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sets various options on your account\n"
		               "\n"
		               "Available options:"));

		Anope::string this_name = source.command;
		bool hide_privileged_commands = Config->GetBlock("options")->Get<bool>("hideprivilegedcommands"),
		     hide_registered_commands = Config->GetBlock("options")->Get<bool>("hideregisteredcommands");
		for (CommandInfo::map::const_iterator it = source.service->commands.begin(), it_end = source.service->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;

			if (c_name.find_ci(this_name + " ") == 0)
			{
				ServiceReference<Command> c(info.name);
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
				ServiceReference<Command> command(info.name);
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

		Anope::string tmp_pass;
		Anope::Encrypt(param, tmp_pass);
		source.nc->SetPassword(tmp_pass);

		source.Reply(_("Password for \002{0}\002 changed."), source.nc->GetDisplay());
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

		NickServ::Nick *setter_na = NickServ::FindNick(params[0]);
		if (setter_na == NULL)
		{
			source.Reply(_("\002{0}\002 isn't registered."), params[0]);
			return;
		}
		NickServ::Account *nc = setter_na->GetAccount();

		size_t len = params[1].length();

		if (Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && source.nc != nc && nc->IsServicesOper())
		{
			source.Reply(_("You may not change the password of other Services Operators."));
			return;
		}

		if (nc->GetDisplay().equals_ci(params[1]) || (Config->GetBlock("options")->Get<bool>("strictpasswords") && len < 5))
		{
			source.Reply(_("Please try again with a more obscure password. Passwords should be at least five characters long, should not be something easily guessed (e.g. your real name or your nick), and cannot contain the space or tab characters."));
			return;
		}

		if (len > Config->GetModule("nickserv")->Get<unsigned>("passlen", "32"))
		{
			source.Reply(_("Your password is too long, it can not contain more than \002{0}\002 characters."), Config->GetModule("nickserv")->Get<unsigned>("passlen", "32"));
			return;
		}

		Log(LOG_ADMIN, source, this) << "to change the password of " << nc->GetDisplay();

		Anope::string tmp_pass;
		Anope::Encrypt(params[1], tmp_pass);
		nc->SetPassword(tmp_pass);
		source.Reply(_("Password for \002{0}\002 changed."), nc->GetDisplay());
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

		NickServ::Nick *na = NickServ::FindNick(user);
		if (na == NULL)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->GetAccount();

		EventReturn MOD_RESULT;
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable autoop for " << na->GetAccount()->GetDisplay();
			nc->SetS<bool>("AUTOOP", true);
			source.Reply(_("Services will from now on set status modes on \002{0}\002 in channels."), nc->GetDisplay());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable autoop for " << na->GetAccount()->GetDisplay();
			nc->UnsetS<bool>("AUTOOP");
			source.Reply(_("Services will no longer set status modes on \002{0}\002 in channels."), nc->GetDisplay());
		}
		else
			this->OnSyntaxError(source, "AUTOOP");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->GetDisplay(), params[0]);
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
		source.Reply(_("Sets whether the given nickname will be given their status modes automatically when they join a channel."
		                " Note that depending on channel settings some modes may not get set automatically."));
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

		NickServ::Nick *user_na = NickServ::FindNick(user), *na = NickServ::FindNick(param);

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

		if (!na || na->GetAccount() != user_na->GetAccount())
		{
			source.Reply(_("The new display must be a nickname of the nickname group \002{0}\02."), user_na->GetAccount()->GetDisplay());
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetNickOption::OnSetNickOption, source, this, user_na->GetAccount(), param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		Log(user_na->GetAccount() == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the display of " << user_na->GetAccount()->GetDisplay() << " to " << na->GetNick();

		user_na->GetAccount()->SetDisplay(na);
		if (source.GetUser())
			IRCD->SendLogin(source.GetUser(), na);
		source.Reply(_("The new display is now \002{0}\002."), user_na->GetAccount()->GetDisplay());
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->GetDisplay(), params[0]);
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
	static bool SendConfirmMail(User *u, ServiceBot *bi, const Anope::string &new_email)
	{
		Anope::string code = Anope::Random(9);

		u->Account()->Extend<std::pair<Anope::string, Anope::string> >("ns_set_email", std::make_pair(new_email, code));

		Anope::string subject = Config->GetBlock("mail")->Get<Anope::string>("emailchange_subject"),
			message = Config->GetBlock("mail")->Get<Anope::string>("emailchange_message");

		subject = subject.replace_all_cs("%e", u->Account()->GetEmail());
		subject = subject.replace_all_cs("%E", new_email);
		subject = subject.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<Anope::string>("networkname"));
		subject = subject.replace_all_cs("%c", code);

		message = message.replace_all_cs("%e", u->Account()->GetEmail());
		message = message.replace_all_cs("%E", new_email);
		message = message.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<Anope::string>("networkname"));
		message = message.replace_all_cs("%c", code);

		Anope::string old = u->Account()->GetEmail();
		u->Account()->SetEmail(new_email);
		bool b = Mail::Send(u, u->Account(), bi, subject, message);
		u->Account()->SetEmail(old);
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

		NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->GetAccount();

		if (nc->HasFieldS("UNCONFIRMED"))
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.empty())
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to unset the email of " << nc->GetDisplay();
			nc->SetEmail("");
			source.Reply(_("E-mail address for \002{0}\002 unset."), nc->GetDisplay());
		}
		else if (Config->GetModule("nickserv")->Get<bool>("confirmemailchanges") && !source.IsServicesOper())
		{
			if (SendConfirmMail(source.GetUser(), source.service, param))
				source.Reply(_("A confirmation e-mail has been sent to \002{0}\002. Follow the instructions in it to change your e-mail address."), param);
		}
		else
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the email of " << nc->GetDisplay() << " to " << param;
			nc->SetEmail(param);
			source.Reply(_("E-mail address for \002{0}\002 changed to \002{1}\002."), nc->GetDisplay(), param);
		}
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->GetDisplay(), params.size() ? params[0] : "");
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

		NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->GetAccount();

		EventReturn MOD_RESULT;
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable keepmodes for " << nc->GetDisplay();
			nc->SetS<bool>("NS_KEEP_MODES", true);
			source.Reply(_("Keep modes for \002{0}\002 is now \002on\002."), nc->GetDisplay());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable keepmodes for " << nc->GetDisplay();
			nc->UnsetS<bool>("NS_KEEP_MODES");
			source.Reply(_("Keep modes for \002{0}\002 is now \002off\002."), nc->GetDisplay());
		}
		else
			this->OnSyntaxError(source, "");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->GetDisplay(), params[0]);
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

		NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->GetAccount();

		EventReturn MOD_RESULT;
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			nc->SetS<bool>("KILLPROTECT", true);
			nc->UnsetS<bool>("KILL_QUICK");
			nc->UnsetS<bool>("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill on for " << nc->GetDisplay();
			source.Reply(_("Protection is now \002on\002 for \002{0}\002."), nc->GetDisplay());
		}
		else if (param.equals_ci("QUICK"))
		{
			nc->SetS<bool>("KILLPROTECT", true);
			nc->SetS<bool>("KILL_QUICK", true);
			nc->UnsetS<bool>("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill quick for " << nc->GetDisplay();
			source.Reply(_("Protection is now \002on\002 for \002{0}\002, with a reduced delay."), nc->GetDisplay());
		}
		else if (param.equals_ci("IMMED"))
		{
			if (Config->GetModule(this->GetOwner())->Get<bool>("allowkillimmed"))
			{
				nc->SetS<bool>("KILLPROTECT",true);
				nc->UnsetS<bool>("KILL_QUICK");
				nc->SetS<bool>("KILL_IMMED", true);
				Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to set kill immed for " << nc->GetDisplay();
				source.Reply(_("Protection is now \002on\002 for \002{0}\002, with no delay."), nc->GetDisplay());
			}
			else
				source.Reply(_("The \002IMMED\002 option is not available on this network."));
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetS<bool>("KILLPROTECT");
			nc->UnsetS<bool>("KILL_QUICK");
			nc->UnsetS<bool>("KILL_IMMED");
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable kill for " << nc->GetDisplay();
			source.Reply(_("Protection is now \002off\002 for \002{0}\002."), nc->GetDisplay());
		}
		else
			this->OnSyntaxError(source, "KILL");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->GetDisplay(), params[0]);
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

		NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->GetAccount();

		EventReturn MOD_RESULT;
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param != "en_US")
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

		Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the language of " << nc->GetDisplay() << " to " << param;

		nc->SetLanguage(param);

		if (source.GetAccount() == nc)
			source.Reply(_("Language changed to \002{0}\002."), Language::Translate(param.c_str(), _("English")));
		else
			source.Reply(_("Language for \002{0}\002 changed to \002{1}\002."), nc->GetDisplay(), Language::Translate(param.c_str(), _("English")));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &param) override
	{
		this->Run(source, source.nc->GetDisplay(), param[0]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Changes the language services will use when sending messages to you (for example, when responding to a command you send). \037language\037 should be chosen from the following list of supported languages:"));

		source.Reply("         en_US (English)");
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

		NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->GetAccount();

		if (!Config->GetBlock("options")->Get<bool>("useprivmsg"))
		{
			source.Reply(_("You cannot %s on this network."), source.command);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable " << source.command << " for " << nc->GetDisplay();
			nc->SetS<bool>("MSG", true);
			source.Reply(_("Services will now reply to \002{0}\002 with \002messages\002."), nc->GetDisplay());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable " << source.command << " for " << nc->GetDisplay();
			nc->UnsetS<bool>("MSG");
			source.Reply(_("Services will now reply to \002{0}\002 with \002notices\002."), nc->GetDisplay());
		}
		else
			this->OnSyntaxError(source, "MSG");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->GetDisplay(), params[0]);
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

		NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->GetAccount();

		EventReturn MOD_RESULT;
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to enable secure for " << nc->GetDisplay();
			nc->SetS<bool>("NS_SECURE", true);
			source.Reply(_("Secure option is now \002on\002 for \002{0}\002."), nc->GetDisplay());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to disable secure for " << nc->GetDisplay();
			nc->UnsetS<bool>("NS_SECURE");
			source.Reply(_("Secure option is now \002off\002 for \002{0}\002."), nc->GetDisplay());
		}
		else
			this->OnSyntaxError(source, "SECURE");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->GetDisplay(), params[0]);
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

		Anope::string param = params.size() > 1 ? params[1] : "";

		if (param.equals_ci("ON"))
		{
			Log(LOG_ADMIN, source, this) << "to enable noexpire for " << na->GetAccount()->GetDisplay();
			na->SetS<bool>("NS_NO_EXPIRE", true);
			source.Reply(_("\002{0}\002 \002will not\002 expire."), na->GetNick());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(LOG_ADMIN, source, this) << "to disable noexpire for " << na->GetAccount()->GetDisplay();
			na->UnsetS<bool>("NS_NO_EXPIRE");
			source.Reply(_("\002{0}\002 \002will\002 expire."), na->GetNick());
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

	Serialize::Field<NickServ::Account, bool> autoop, keep_modes, killprotect, kill_quick, kill_immed, message, secure;
	Serialize::Field<NickServ::Nick, bool> noexpire;

	/* email, passcode */
	ExtensibleItem<std::pair<Anope::string, Anope::string > > ns_set_email;

 public:
	NSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::PreCommand>(this)
		, EventHook<Event::SetCorrectModes>(this)
		, EventHook<NickServ::Event::PreNickExpire>(this)
		, EventHook<Event::NickInfo>(this)
		, EventHook<Event::UserModeSet>(this)
		, EventHook<Event::UserModeUnset>(this)
		, EventHook<Event::UserLogin>(this)

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
		, keep_modes(this, "NS_KEEP_MODES")
		, killprotect(this, "KILLPROTECT")
		, kill_quick(this, "KILL_QUICK")
		, kill_immed(this, "KILL_IMMED")
		, message(this, "MSG")
		, secure(this, "NS_SECURE")
		, noexpire(this, "NS_NO_EXPIRE")

		, ns_set_email(this, "ns_set_email")
	{

	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) override
	{
		NickServ::Account *uac = source.nc;

		if (command->GetName() == "nickserv/confirm" && !params.empty() && uac)
		{
			std::pair<Anope::string, Anope::string> *n = ns_set_email.Get(uac);
			if (n)
			{
				if (params[0] == n->second)
				{
					uac->SetEmail(n->first);
					Log(LOG_COMMAND, source, command) << "to confirm their email address change to " << uac->GetEmail();
					source.Reply(_("Your email address has been changed to \002%s\002."), uac->GetEmail().c_str());
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

		if (kill_immed.HasExt(na->GetAccount()))
			info.AddOption(_("Immediate protection"));
		else if (kill_quick.HasExt(na->GetAccount()))
			info.AddOption(_("Quick protection"));
		else if (killprotect.HasExt(na->GetAccount()))
			info.AddOption(_("Protection"));
		if (secure.HasExt(na->GetAccount()))
			info.AddOption(_("Security"));
		if (message.HasExt(na->GetAccount()))
			info.AddOption(_("Message mode"));
		if (autoop.HasExt(na->GetAccount()))
			info.AddOption(_("Auto-op"));
		if (noexpire.HasExt(na))
			info.AddOption(_("No expire"));
		if (keep_modes.HasExt(na->GetAccount()))
			info.AddOption(_("Keep modes"));
	}

	void OnUserModeSet(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (u->Account() && setter.GetUser() == u)
		{
			NickServ::Mode *m = Serialize::New<NickServ::Mode *>();
			if (m != nullptr)
			{
				m->SetAccount(u->Account());
				m->SetMode(mname);
			}
		}
	}

	void OnUserModeUnset(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (u->Account() && setter.GetUser() == u)
		{
			for (NickServ::Mode *m : u->Account()->GetRefs<NickServ::Mode *>())
				if (m->GetMode() == mname)
					m->Delete();
		}
	}

	void OnUserLogin(User *u) override
	{
		if (keep_modes.HasExt(u->Account()))
			for (NickServ::Mode *mode : u->Account()->GetRefs<NickServ::Mode *>())
			{
				UserMode *um = ModeManager::FindUserModeByName(mode->GetMode());
				/* if the null user can set the mode, then it's probably safe */
				if (um && um->CanSet(NULL))
					u->SetMode(NULL, mode->GetMode());
			}
	}
};

MODULE_INIT(NSSet)
