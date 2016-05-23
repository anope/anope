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
#include "modules/nickserv.h"

static bool SendRegmail(User *u, NickServ::Nick *na, ServiceBot *bi);

class CommandNSConfirm : public Command
{
 public:
	CommandNSConfirm(Module *creator) : Command(creator, "nickserv/confirm", 1, 2)
	{
		this->SetDesc(_("Confirm a passcode"));
		this->SetSyntax(_("\037passcode\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &passcode = params[0];

		if (source.nc && !source.nc->HasFieldS("UNCONFIRMED") && source.HasPriv("nickserv/confirm"))
		{
			NickServ::Nick *na = NickServ::FindNick(passcode);
			if (na == NULL)
			{
				source.Reply(_("\002{0}\002 isn't registered."), passcode);
				return;
			}

			if (na->GetAccount()->HasFieldS("UNCONFIRMED") == false)
			{
				source.Reply(_("\002{0}\002 is already confirmed."), na->GetNick());
				return;
			}

			na->GetAccount()->UnsetS<bool>("UNCONFIRMED");
			EventManager::Get()->Dispatch(&NickServ::Event::NickConfirm::OnNickConfirm, source.GetUser(), na->GetAccount());
			Log(LOG_ADMIN, source, this) << "to confirm nick " << na->GetNick() << " (" << na->GetAccount()->GetDisplay() << ")";
			source.Reply(_("\002{0}\002 has been confirmed."), na->GetNick());
		}
		else if (source.nc)
		{
			Anope::string *code = source.nc->GetExt<Anope::string>("passcode");
			if (code == nullptr || *code != passcode)
			{
				source.Reply(_("Invalid passcode."));
				return;
			}

			NickServ::Account *nc = source.nc;
			nc->Shrink<Anope::string>("passcode");
			Log(LOG_COMMAND, source, this) << "to confirm their email";
			source.Reply(_("Your email address of \002{0}\002 has been confirmed."), source.nc->GetEmail());
			nc->UnsetS<bool>("UNCONFIRMED");

			EventManager::Get()->Dispatch(&NickServ::Event::NickConfirm::OnNickConfirm, source.GetUser(), nc);

			if (source.GetUser())
			{
				NickServ::Nick *na = NickServ::FindNick(source.GetNick());
				if (na)
				{
					IRCD->SendLogin(source.GetUser(), na);
					if (!Config->GetModule("nickserv")->Get<bool>("nonicknameownership") && na->GetAccount() == source.GetAccount() && !na->GetAccount()->HasFieldS("UNCONFIRMED"))
						source.GetUser()->SetMode(source.service, "REGISTERED");
				}
			}
		}
		else
			source.Reply(_("Invalid passcode."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("This command is used by several commands as a way to confirm changes made to your account.\n"
		               "\n"
		               "This is most commonly used to confirm your email address once you register or change it.\n"
		               "\n"
		               "This is also used after when resetting your password to force identify you to your account so you may change your password."));
		if (source.HasPriv("nickserv/confirm"))
			source.Reply(_("Additionally, Services Operators with the \037nickserv/confirm\037 permission can\n"
				"replace \037passcode\037 with a users nick to force validate them."));
		return true;
	}
};

class CommandNSRegister : public Command
{
 public:
	CommandNSRegister(Module *creator) : Command(creator, "nickserv/register", 1, 2)
	{
		this->SetDesc(_("Register a nickname"));
		if (Config->GetModule("nickserv")->Get<bool>("forceemail", "yes"))
			this->SetSyntax(_("\037password\037 \037email\037"));
		else
			this->SetSyntax(_("\037password\037 \037[email]\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();
		Anope::string u_nick = source.GetNick();
		size_t nicklen = u_nick.length();
		Anope::string pass = params[0];
		Anope::string email = params.size() > 1 ? params[1] : "";
		const Anope::string &nsregister = Config->GetModule(this->GetOwner())->Get<Anope::string>("registration");

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, nickname registration is temporarily disabled."));
			return;
		}

		if (nsregister.equals_ci("disable"))
		{
			source.Reply(_("Registration is currently disabled."));
			return;
		}

		time_t nickregdelay = Config->GetModule(this->GetOwner())->Get<time_t>("nickregdelay");
		time_t reg_delay = Config->GetModule("nickserv")->Get<time_t>("regdelay");
		if (u && !u->HasMode("OPER") && nickregdelay && Anope::CurTime - u->timestamp < nickregdelay)
		{
			source.Reply(_("You must have been using this nickname for at least {0} seconds to register."), nickregdelay);
			return;
		}

		/* Prevent "Guest" nicks from being registered. -TheShadow */

		/* Guest nick can now have a series of between 1 and 7 digits.
		 *   --lara
		 */
		const Anope::string &guestnick = Config->GetModule("nickserv")->Get<Anope::string>("guestnickprefix", "Guest");
		if (nicklen <= guestnick.length() + 7 && nicklen >= guestnick.length() + 1 && !u_nick.find_ci(guestnick) && u_nick.substr(guestnick.length()).find_first_not_of("1234567890") == Anope::string::npos)
		{
			source.Reply(_("\002{0}\002 may not be registered."), u_nick);
			return;
		}

		if (!IRCD->IsNickValid(u_nick))
		{
			source.Reply(_("\002{0}\002 may not be registered."), u_nick);
			return;
		}

		if (ServiceBot::Find(u_nick, true))
		{
			source.Reply(_("\002{0}\002 may not be registered."), u_nick);
			return;
		}

		if (Config->GetModule("nickserv")->Get<bool>("restrictopernicks"))
			for (Oper *o : Serialize::GetObjects<Oper *>())
			{
				if (!source.IsOper() && u_nick.find_ci(o->GetName()) != Anope::string::npos)
				{
					source.Reply(_("\002{0}\002 may not be registered because it is too similar to an operator nick."), u_nick);
					return;
				}
			}

		unsigned int passlen = Config->GetModule("nickserv")->Get<unsigned>("passlen", "32");

		if (Config->GetModule("nickserv")->Get<bool>("forceemail", "yes") && email.empty())
		{
			this->OnSyntaxError(source, "");
			return;
		}

		if (u && Anope::CurTime < u->lastnickreg + reg_delay)
		{
			source.Reply(_("Please wait \002{0}\002 seconds before using the {1} command again."), (u->lastnickreg + reg_delay) - Anope::CurTime, source.command);
			return;
		}

		if (NickServ::FindNick(u_nick) != NULL)
		{
			source.Reply(_("\002{0}\002 is already registered."), u_nick);
			return;
		}

		if (pass.equals_ci(u_nick) || (Config->GetBlock("options")->Get<bool>("strictpasswords") && pass.length() < 5))
		{
			source.Reply(_("Please try again with a more obscure password. Passwords should be at least five characters long, should not be something easily guessed"
			               " (e.g. your real name or your nickname), and cannot contain the space or tab characters."));
			return;
		}

		if (pass.length() > Config->GetModule("nickserv")->Get<unsigned>("passlen", "32"))
		{
			source.Reply(_("Your password is too long, it can not contain more than \002{0}\002 characters."), Config->GetModule("nickserv")->Get<unsigned>("passlen", "32"));
			return;
		}

		if (!email.empty() && !Mail::Validate(email))
		{
			source.Reply(_("\002{0}\002 is not a valid e-mail address."), email);
			return;
		}

		NickServ::Account *nc = Serialize::New<NickServ::Account *>();
		nc->SetDisplay(u_nick);

		NickServ::Nick *na = Serialize::New<NickServ::Nick *>();
		na->SetNick(u_nick);
		na->SetAccount(nc);
		na->SetTimeRegistered(Anope::CurTime);
		na->SetLastSeen(Anope::CurTime);

		Anope::string epass;
		Anope::Encrypt(pass, epass);
		nc->SetPassword(epass);
		if (!email.empty())
			nc->SetEmail(email);

		if (u)
		{
			na->SetLastUsermask(u->GetIdent() + "@" + u->GetDisplayedHost());
			na->SetLastRealname(u->realname);
		}
		else
			na->SetLastRealname(source.GetNick());

		Log(LOG_COMMAND, source, this) << "to register " << na->GetNick() << " (email: " << (!na->GetAccount()->GetEmail().empty() ? na->GetAccount()->GetEmail() : "none") << ")";

		source.Reply(_("\002{0}\002 has been registered."), u_nick);

		if (nsregister.equals_ci("admin"))
		{
			nc->SetS<bool>("UNCONFIRMED", true);
			// User::Identify() called below will notify the user that their registration is pending
		}
		else if (nsregister.equals_ci("mail"))
		{
			if (!email.empty())
			{
				nc->SetS<bool>("UNCONFIRMED", true);
				SendRegmail(NULL, na, source.service);
			}
		}

		EventManager::Get()->Dispatch(&NickServ::Event::NickRegister::OnNickRegister, source.GetUser(), na, pass);

		if (u)
		{
			u->Identify(na);
			u->lastnickreg = Anope::CurTime;
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Registers your nickname. Once your nickname is registered, you will be able to use most features of services, including owning and managing channels."
		               "Make sure you remember the password - you'll need it to identify yourself later. Your email address will only be used if you forget your password."));

		if (!Config->GetModule("nickserv")->Get<bool>("forceemail", "yes"))
		{
			source.Reply(" ");
			source.Reply(_("The \037email\037 parameter is optional and will set the email\n"
					"for your nick immediately.\n"
					"Your privacy is respected; this e-mail won't be given to\n"
					"any third-party person. You may also wish to \002SET HIDE\002 it\n"
					"after registering if it isn't the default setting already."));
		}

		if (!Config->GetModule("nickserv")->Get<bool>("nonicknameownership"))
		{
			source.Reply(" ");
			source.Reply(_("This command also creates a new group for your nickname, which will allow you to group other nicknames later, which share the same configuration, the same set of memos and the same channel privileges."));
		}
		return true;
	}
};

class CommandNSResend : public Command
{
 public:
	CommandNSResend(Module *creator) : Command(creator, "nickserv/resend", 0, 0)
	{
		this->SetDesc(_("Resend registration confirmation email"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!Config->GetModule(this->GetOwner())->Get<Anope::string>("registration").equals_ci("mail"))
		{
			source.Reply(_("Access denied."));
			return;
		}

		NickServ::Nick *na = NickServ::FindNick(source.GetNick());

		if (na == NULL)
		{
			source.Reply(_("Your nickname isn't registered."));
			return;
		}

		if (na->GetAccount() != source.GetAccount() || !source.nc->HasFieldS("UNCONFIRMED"))
		{
			source.Reply(_("Your account is already confirmed."));
			return;
		}

		if (Anope::CurTime < source.nc->lastmail + Config->GetModule(this->GetOwner())->Get<time_t>("resenddelay"))
		{
			source.Reply(_("Cannot send mail now; please retry a little later."));
			return;
		}

		if (!SendRegmail(source.GetUser(), na, source.service))
		{
			Log(this->GetOwner()) << "Unable to resend registration verification code for " << source.GetNick();
			return;
		}

		na->GetAccount()->lastmail = Anope::CurTime;
		source.Reply(_("Your passcode has been re-sent to \002{0}\002."), na->GetAccount()->GetEmail());
		Log(LOG_COMMAND, source, this) << "to resend registration verification code";
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (!Config->GetModule(this->GetOwner())->Get<Anope::string>("registration").equals_ci("mail"))
			return false;

		source.Reply(_("This command will resend you the registration confirmation email."));
		return true;
	}

	void OnServHelp(CommandSource &source) override
	{
		if (Config->GetModule(this->GetOwner())->Get<Anope::string>("registration").equals_ci("mail"))
			Command::OnServHelp(source);
	}
};

class NSRegister : public Module
	, public EventHook<Event::NickIdentify>
	, public EventHook<NickServ::Event::PreNickExpire>
{
	CommandNSRegister commandnsregister;
	CommandNSConfirm commandnsconfirm;
	CommandNSResend commandnsrsend;

	Serialize::Field<NickServ::Account, bool> unconfirmed;
	Serialize::Field<NickServ::Account, Anope::string> passcode;

 public:
	NSRegister(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::NickIdentify>(this)
		, EventHook<NickServ::Event::PreNickExpire>(this)
		, commandnsregister(this)
		, commandnsconfirm(this)
		, commandnsrsend(this)
		, unconfirmed(this, "UNCONFIRMED")
		, passcode(this, "passcode")
	{
		if (Config->GetModule(this)->Get<Anope::string>("registration").equals_ci("disable"))
			throw ModuleException("Module " + Module::name + " will not load with registration disabled.");
	}

	void OnNickIdentify(User *u) override
	{
		ServiceBot *NickServ;
		if (unconfirmed.HasExt(u->Account()) && (NickServ = Config->GetClient("NickServ")))
		{
			const Anope::string &nsregister = Config->GetModule(this)->Get<Anope::string>("registration");
			if (nsregister.equals_ci("admin"))
				u->SendMessage(NickServ, _("All new accounts must be validated by an administrator. Please wait for your registration to be confirmed."));
			else
				u->SendMessage(NickServ, _("Your email address is not confirmed. To confirm it, follow the instructions that were emailed to you."));
			NickServ::Nick *this_na = NickServ::FindNick(u->Account()->GetDisplay());
			time_t time_registered = Anope::CurTime - this_na->GetTimeRegistered();
			time_t unconfirmed_expire = Config->GetModule(this)->Get<time_t>("unconfirmedexpire", "1d");
			if (unconfirmed_expire > time_registered)
				u->SendMessage(NickServ, _("Your account will expire, if not confirmed, in %s."), Anope::Duration(unconfirmed_expire - time_registered, u->Account()).c_str());
		}
	}

	void OnPreNickExpire(NickServ::Nick *na, bool &expire) override
	{
		if (unconfirmed.HasExt(na->GetAccount()))
		{
			time_t unconfirmed_expire = Config->GetModule(this)->Get<time_t>("unconfirmedexpire", "1d");
			if (unconfirmed_expire && Anope::CurTime - na->GetTimeRegistered() >= unconfirmed_expire)
				expire = true;
		}
	}
};

static bool SendRegmail(User *u, NickServ::Nick *na, ServiceBot *bi)
{
	NickServ::Account *nc = na->GetAccount();

	Anope::string *code = na->GetAccount()->GetExt<Anope::string>("passcode");
	if (code == NULL)
		code = na->GetAccount()->Extend<Anope::string>("passcode", Anope::Random(9));

	Anope::string subject = Language::Translate(na->GetAccount(), Config->GetBlock("mail")->Get<Anope::string>("registration_subject").c_str()),
		message = Language::Translate(na->GetAccount(), Config->GetBlock("mail")->Get<Anope::string>("registration_message").c_str());

	subject = subject.replace_all_cs("%n", na->GetNick());
	subject = subject.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<Anope::string>("networkname"));
	subject = subject.replace_all_cs("%c", *code);

	message = message.replace_all_cs("%n", na->GetNick());
	message = message.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<Anope::string>("networkname"));
	message = message.replace_all_cs("%c", *code);

	return Mail::Send(u, nc, bi, subject, message);
}

MODULE_INIT(NSRegister)
