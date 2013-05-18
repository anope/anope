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

/*************************************************************************/

#include "module.h"

static bool SendRegmail(User *u, const NickAlias *na, const BotInfo *bi);

class CommandNSConfirm : public Command
{
 public:
	CommandNSConfirm(Module *creator) : Command(creator, "nickserv/confirm", 1, 2)
	{
		this->SetDesc(_("Confirm a passcode"));
		this->SetSyntax(_("\037passcode\037"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &passcode = params[0];

		if (source.nc && !source.nc->HasExt("UNCONFIRMED") && source.HasPriv("nickserv/confirm"))
		{
			NickAlias *na = NickAlias::Find(passcode);
			if (na == NULL)
				source.Reply(NICK_X_NOT_REGISTERED, passcode.c_str());
			else if (na->nc->HasExt("UNCONFIRMED") == false)
				source.Reply(_("Nick \002%s\002 is already confirmed."), na->nick.c_str());
			else
			{
				na->nc->Shrink("UNCONFIRMED");
				Log(LOG_ADMIN, source, this) << "to confirm nick " << na->nick << " (" << na->nc->display << ")";
				source.Reply(_("Nick \002%s\002 has been confirmed."), na->nick.c_str());
			}
		}
		else if (source.nc)
		{
			Anope::string *code = source.nc->GetExt<ExtensibleItemClass<Anope::string> *>("ns_register_passcode");
			if (code != NULL && *code == passcode)
			{
				NickCore *nc = source.nc;
				nc->Shrink("ns_register_passcode");
				Log(LOG_COMMAND, source, this) << "to confirm their email";
				source.Reply(_("Your email address of \002%s\002 has been confirmed."), source.nc->email.c_str());
				nc->Shrink("UNCONFIRMED");

				if (source.GetUser())
				{
					IRCD->SendLogin(source.GetUser());
					const NickAlias *na = NickAlias::Find(source.GetNick());
					if (!Config->GetBlock("options")->Get<bool>("nonicknameownership") && na != NULL && na->nc == source.GetAccount() && na->nc->HasExt("UNCONFIRMED") == false)
						source.GetUser()->SetMode(NickServ, "REGISTERED");
				}
			}
			else
				source.Reply(_("Invalid passcode."));
		}
		else
			source.Reply(_("Invalid passcode."));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command is used by several commands as a way to confirm\n"
				"changes made to your account.\n"
				" \n"
				"This is most commonly used to confirm your email address once\n"
				"you register or change it.\n"
				" \n"
				"This is also used after the RESETPASS command has been used to\n"
				"force identify you to your nick so you may change your password."));
		if (source.HasPriv("nickserv/confirm"))
			source.Reply(_("Additionally, Services Operators with the \037nickserv/confirm\037 permission can\n"
				"replace \037passcode\037 with a users nick to force validate them."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		source.Reply(NICK_CONFIRM_INVALID);
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		NickAlias *na;
		User *u = source.GetUser();
		Anope::string u_nick = source.GetNick();
		size_t nicklen = u_nick.length();
		Anope::string pass = params[0];
		Anope::string email = params.size() > 1 ? params[1] : "";
		const Anope::string &nsregister = Config->GetModule(this->owner)->Get<const Anope::string>("registration");

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

		time_t nickregdelay = Config->GetModule(this->owner)->Get<time_t>("nickregdelay");
		time_t reg_delay = Config->GetModule("nickserv")->Get<time_t>("regdelay");
		if (u && !u->HasMode("OPER") && nickregdelay && Anope::CurTime - u->timestamp < nickregdelay)
		{
			source.Reply(_("You must have been using this nick for at least %d seconds to register."), nickregdelay);
			return;
		}

		/* Prevent "Guest" nicks from being registered. -TheShadow */

		/* Guest nick can now have a series of between 1 and 7 digits.
		 *   --lara
		 */
		const Anope::string &guestnick = Config->GetBlock("options")->Get<const Anope::string>("guestnickprefix");
		if (nicklen <= guestnick.length() + 7 && nicklen >= guestnick.length() + 1 && !u_nick.find_ci(guestnick) && u_nick.substr(guestnick.length()).find_first_not_of("1234567890") == Anope::string::npos)
		{
			source.Reply(NICK_CANNOT_BE_REGISTERED, u_nick.c_str());
			return;
		}

		if (!IRCD->IsNickValid(u_nick))
		{
			source.Reply(NICK_CANNOT_BE_REGISTERED, u_nick.c_str());
			return;
		}

		if (Config->GetBlock("nickserv")->Get<bool>("restrictopernicks"))
			for (unsigned i = 0; i < Config->Opers.size(); ++i)
			{
				Oper *o = Config->Opers[i];

				if (!source.IsOper() && u_nick.find_ci(o->name) != Anope::string::npos)
				{
					source.Reply(NICK_CANNOT_BE_REGISTERED, u_nick.c_str());
					return;
				}
			}

		if (Config->GetModule("nickserv")->Get<bool>("forceemail", "yes") && email.empty())
			this->OnSyntaxError(source, "");
		else if (u && Anope::CurTime < u->lastnickreg + reg_delay)
			source.Reply(_("Please wait %d seconds before using the REGISTER command again."), (u->lastnickreg + reg_delay) - Anope::CurTime);
		else if ((na = NickAlias::Find(u_nick)))
			source.Reply(NICK_ALREADY_REGISTERED, u_nick.c_str());
		else if (pass.equals_ci(u_nick) || (Config->GetBlock("options")->Get<bool>("strictpasswords") && pass.length() < 5))
			source.Reply(MORE_OBSCURE_PASSWORD);
		else if (pass.length() > Config->GetBlock("options")->Get<unsigned>("passlen"))
			source.Reply(PASSWORD_TOO_LONG);
		else if (!email.empty() && !Mail::Validate(email))
			source.Reply(MAIL_X_INVALID, email.c_str());
		else
		{
			NickCore *nc = new NickCore(u_nick);
			na = new NickAlias(u_nick, nc);
			Anope::Encrypt(pass, nc->pass);
			if (!email.empty())
				nc->email = email;

			if (u)
			{
				na->last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
				na->last_realname = u->realname;

				u->Login(nc);
			}

			Log(LOG_COMMAND, source, this) << "to register " << na->nick << " (email: " << (!na->nc->email.empty() ? na->nc->email : "none") << ")";

			FOREACH_MOD(I_OnNickRegister, OnNickRegister(source.GetUser(), na));

			if (na->nc->GetAccessCount())
				source.Reply(_("Nickname \002%s\002 registered under your user@host-mask: %s"), u_nick.c_str(), na->nc->GetAccess(0).c_str());
			else
				source.Reply(_("Nickname \002%s\002 registered."), u_nick.c_str());

			Anope::string tmp_pass;
			if (Anope::Decrypt(na->nc->pass, tmp_pass) == 1)
				source.Reply(_("Your password is \002%s\002 - remember this for later use."), tmp_pass.c_str());

			if (nsregister.equals_ci("admin"))
			{
				nc->ExtendMetadata("UNCONFIRMED");
				source.Reply(_("All new accounts must be validated by an administrator. Please wait for your registration to be confirmed."));
			}
			else if (nsregister.equals_ci("mail"))
			{
				nc->ExtendMetadata("UNCONFIRMED");
				if (SendRegmail(u, na, source.service))
				{
					time_t unconfirmed_expire = Config->GetModule("nickserv")->Get<time_t>("unconfirmedexpire", "1d");
					BotInfo *bi;
					Anope::string cmd;
					if (Command::FindCommandFromService("nickserv/confirm", bi, cmd))
						source.Reply(_("A passcode has been sent to %s, please type \002%s%s %s <passcode>\002 to confirm your email address."), email.c_str(), Config->StrictPrivmsg.c_str(), bi->nick.c_str(), cmd.c_str());
					source.Reply(_("If you do not confirm your email address within %s your account will expire."), Anope::Duration(unconfirmed_expire).c_str());
				}
			}
			else if (nsregister.equals_ci("none"))
			{
				if (u)
				{
					IRCD->SendLogin(u);
					if (!Config->GetBlock("options")->Get<bool>("nonicknameownership") && na->nc == u->Account() && na->nc->HasExt("UNCONFIRMED") == false)
						u->SetMode(NickServ, "REGISTERED");
				}
			}

			if (u)
				u->lastnickreg = Anope::CurTime;
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Registers your nickname in the %s database. Once\n"
				"your nick is registered, you can use the \002SET\002 and \002ACCESS\002\n"
				"commands to configure your nick's settings as you like\n"
				"them. Make sure you remember the password you use when\n"
				"registering - you'll need it to make changes to your nick\n"
				"later. (Note that \002case matters!\002 \037ANOPE\037, \037Anope\037, and\n"
				"\037anope\037 are all different passwords!)\n"
				" \n"
				"Guidelines on choosing passwords:\n"
				" \n"
				"Passwords should not be easily guessable. For example,\n"
				"using your real name as a password is a bad idea. Using\n"
				"your nickname as a password is a much worse idea ;) and,\n"
				"in fact, %s will not allow it. Also, short\n"
				"passwords are vulnerable to trial-and-error searches, so\n"
				"you should choose a password at least 5 characters long.\n"
				"Finally, the space character cannot be used in passwords."),
				source.service->nick.c_str(), source.service->nick.c_str());

		if (!Config->GetModule("nickserv")->Get<bool>("forceemail", "yes"))
		{
			source.Reply(" ");
			source.Reply(_("The \037email\037 parameter is optional and will set the email\n"
					"for your nick immediately.\n"
					"Your privacy is respected; this e-mail won't be given to\n"
					"any third-party person. You may also wish to \002SET HIDE\002 it\n"
					"after registering if it isn't the default setting already."));
		}

		source.Reply(" ");
		source.Reply(_("This command also creates a new group for your nickname,\n"
				"that will allow you to register other nicks later sharing\n"
				"the same configuration, the same set of memos and the\n"
				"same channel privileges."));
		return true;
	}
};

class CommandNSResend : public Command
{
 public:
	CommandNSResend(Module *creator) : Command(creator, "nickserv/resend", 0, 0)
	{
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!Config->GetModule(this->owner)->Get<const Anope::string>("registration").equals_ci("mail"))
			return;

		const NickAlias *na = NickAlias::Find(source.GetNick());

		if (na == NULL)
			source.Reply(NICK_NOT_REGISTERED);
		else if (na->nc != source.GetAccount() || source.nc->HasExt("UNCONFIRMED") == false)
			source.Reply(_("Your account is already confirmed."));
		else
		{
			if (Anope::CurTime < source.nc->lastmail + Config->GetModule(this->owner)->Get<time_t>("resenddelay"))
				source.Reply(_("Cannot send mail now; please retry a little later."));
			else if (SendRegmail(source.GetUser(), na, source.service))
			{
				na->nc->lastmail = Anope::CurTime;
				source.Reply(_("Your passcode has been re-sent to %s."), na->nc->email.c_str());
				Log(LOG_COMMAND, source, this) << "to resend registration verification code";
			}
			else
				Log(this->owner) << "Unable to resend registration verification code for " << source.GetNick();
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		if (!Config->GetModule(this->owner)->Get<const Anope::string>("registration").equals_ci("mail"))
			return false;

		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command will re-send the auth code (also called passcode)\n"
				"to the e-mail address of the nickname in the database."));
		return true;
	}

	void OnServHelp(CommandSource &source) anope_override
	{
		if (Config->GetModule(this->owner)->Get<const Anope::string>("registration").equals_ci("mail"))
			Command::OnServHelp(source);
	}
};

class NSRegister : public Module
{
	CommandNSRegister commandnsregister;
	CommandNSConfirm commandnsconfirm;
	CommandNSResend commandnsrsend;

 public:
	NSRegister(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsregister(this), commandnsconfirm(this), commandnsrsend(this)
	{
		if (Config->GetModule(this)->Get<const Anope::string>("registration").equals_ci("disable"))
			throw ModuleException("Module " + this->name + " will not load with registration disabled.");
	}
};

static bool SendRegmail(User *u, const NickAlias *na, const BotInfo *bi)
{
	NickCore *nc = na->nc;

	Anope::string *code = na->nc->GetExt<ExtensibleItemClass<Anope::string> *>("ns_register_passcode");
	Anope::string codebuf;
	if (code == NULL)
	{
		int chars[] = {
			' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
			'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
			'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
			'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
			'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
		};
		int idx, min = 1, max = 62;
		for (idx = 0; idx < 9; ++idx)
			codebuf += chars[1 + static_cast<int>((static_cast<float>(max - min)) * static_cast<uint16_t>(rand()) / 65536.0) + min];
		nc->Extend("ns_register_passcode", new ExtensibleItemClass<Anope::string>(codebuf));
	}
	else
		codebuf = *code;

	Anope::string subject = Language::Translate(na->nc, Config->GetBlock("mail")->Get<const Anope::string>("registration_subject").c_str()),
		message = Language::Translate(na->nc, Config->GetBlock("mail")->Get<const Anope::string>("registration_message").c_str());

	subject = subject.replace_all_cs("%n", na->nick);
	subject = subject.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<const Anope::string>("networkname"));
	subject = subject.replace_all_cs("%c", codebuf);

	message = message.replace_all_cs("%n", na->nick);
	message = message.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<const Anope::string>("networkname"));
	message = message.replace_all_cs("%c", codebuf);

	return Mail::Send(u, nc, bi, subject, message);
}

MODULE_INIT(NSRegister)
