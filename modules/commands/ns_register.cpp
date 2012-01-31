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

static bool SendRegmail(User *u, NickAlias *na, BotInfo *bi);

class CommandNSConfirm : public Command
{
 public:
	CommandNSConfirm(Module *creator) : Command(creator, "nickserv/confirm", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Confirm an auth code"));
		this->SetSyntax(_("\037passcode\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &passcode = params[0];

		if (u->Account() && !u->Account()->HasFlag(NI_UNCONFIRMED) && u->HasPriv("nickserv/confirm"))
		{
			NickAlias *na = findnick(passcode);
			if (na == NULL)
				source.Reply(NICK_X_NOT_REGISTERED, passcode.c_str());
			else if (na->nc->HasFlag(NI_UNCONFIRMED) == false)
				source.Reply(_("Nick \002%s\002 is already confirmed."), na->nick.c_str());
			else
			{
				na->nc->UnsetFlag(NI_UNCONFIRMED);
				Log(LOG_ADMIN, u, this) << "to confirm nick " << na->nick << " (" << na->nc->display << ")";
				source.Reply(_("Nick \002%s\002 has been confirmed."), na->nick.c_str());
			}
		}
		else if (u->Account())
		{
			Anope::string *code = u->Account()->GetExt<ExtensibleString *>("ns_register_passcode");
			if (code != NULL && *code == passcode)
			{
				u->Account()->Shrink("ns_register_passcode");
				Log(LOG_COMMAND, u, this) << "to confirm their email";
				source.Reply(_("Your email address of \002%s\002 has been confirmed."), u->Account()->email.c_str());
				u->Account()->UnsetFlag(NI_UNCONFIRMED);

				ircdproto->SendLogin(u);
				NickAlias *na = findnick(u->nick);
				if (!Config->NoNicknameOwnership && na != NULL && na->nc == u->Account() && na->nc->HasFlag(NI_UNCONFIRMED) == false)
					u->SetMode(findbot(Config->NickServ), UMODE_REGISTERED);
			}
			else
				source.Reply(_("Invalid passcode."));
		}
		else
			source.Reply(_("Invalid passcode."));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
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
		if (u->Account() && u->HasPriv("nickserv/confirm"))
			source.Reply(_("Additionally, Services Operators with the \037nickserv/confirm\037 permission can\n"
				"replace \037passcode\037 with a users nick to force validate them."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(NICK_CONFIRM_INVALID);
	}
};

class CommandNSRegister : public Command
{
 public:
	CommandNSRegister(Module *creator) : Command(creator, "nickserv/register", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Register a nickname"));
		if (Config->NSForceEmail)
			this->SetSyntax(_("\037password\037 \037email\037"));
		else
			this->SetSyntax(_("\037password\037 \037[email]\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickAlias *na;
		size_t prefixlen = Config->NSGuestNickPrefix.length();
		size_t nicklen = u->nick.length();
		Anope::string pass = params[0];
		Anope::string email = params.size() > 1 ? params[1] : "";

		if (readonly)
		{
			source.Reply(_("Sorry, nickname registration is temporarily disabled."));
			return;
		}

		if (!u->HasMode(UMODE_OPER) && Config->NickRegDelay && Anope::CurTime - u->my_signon < Config->NickRegDelay)
		{
			source.Reply(_("You must have been using this nick for at least %d seconds to register."), Config->NickRegDelay);
			return;
		}

		/* Prevent "Guest" nicks from being registered. -TheShadow */

		/* Guest nick can now have a series of between 1 and 7 digits.
		 *   --lara
		 */
		if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1 && !u->nick.find_ci(Config->NSGuestNickPrefix) && u->nick.substr(prefixlen).find_first_not_of("1234567890") == Anope::string::npos)
		{
			source.Reply(NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
			return;
		}

		if (Config->RestrictOperNicks)
			for (unsigned i = 0; i < Config->Opers.size(); ++i)
			{
				Oper *o = Config->Opers[i];

				if (!u->HasMode(UMODE_OPER) && u->nick.find_ci(o->name) != Anope::string::npos)
				{
					source.Reply(NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
					return;
				}
			}

		if (Config->NSForceEmail && email.empty())
			this->OnSyntaxError(source, "");
		else if (Anope::CurTime < u->lastnickreg + Config->NSRegDelay)
			source.Reply(_("Please wait %d seconds before using the REGISTER command again."), (u->lastnickreg + Config->NSRegDelay) - Anope::CurTime);
		else if ((na = findnick(u->nick)))
			source.Reply(NICK_ALREADY_REGISTERED, u->nick.c_str());
		else if (pass.equals_ci(u->nick) || (Config->StrictPasswords && pass.length() < 5))
			source.Reply(MORE_OBSCURE_PASSWORD);
		else if (pass.length() > Config->PassLen)
			source.Reply(PASSWORD_TOO_LONG);
		else if (!email.empty() && !MailValidate(email))
			source.Reply(MAIL_X_INVALID, email.c_str());
		else
		{
			na = new NickAlias(u->nick, new NickCore(u->nick));
			enc_encrypt(pass, na->nc->pass);
			if (!email.empty())
				na->nc->email = email;

			Anope::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
			na->last_usermask = last_usermask;
			na->last_realname = u->realname;
			if (Config->NSAddAccessOnReg)
				na->nc->AddAccess(create_mask(u));

			u->Login(na->nc);

			Log(LOG_COMMAND, u, this) << "to register " << na->nick << " (email: " << (!na->nc->email.empty() ? na->nc->email : "none") << ")";

			FOREACH_MOD(I_OnNickRegister, OnNickRegister(na));

			if (Config->NSAddAccessOnReg)
				source.Reply(_("Nickname \002%s\002 registered under your account: %s"), u->nick.c_str(), na->nc->GetAccess(0).c_str());
			else
				source.Reply(_("Nickname \002%s\002 registered."), u->nick.c_str());

			Anope::string tmp_pass;
			if (enc_decrypt(na->nc->pass, tmp_pass) == 1)
				source.Reply(_("Your password is \002%s\002 - remember this for later use."), tmp_pass.c_str());

			if (Config->NSEmailReg)
			{
				na->nc->SetFlag(NI_UNCONFIRMED);
				if (SendRegmail(u, na, source.owner))
				{
					source.Reply(_("A passcode has been sent to %s, please type %s%s confirm <passcode> to confirm your email address."), email.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str());
					source.Reply(_("If you do not confirm your email address within %s your account will expire."), duration(Config->NSUnconfirmedExpire).c_str());
				}
			}
			else
			{
				ircdproto->SendLogin(u);
				if (!Config->NoNicknameOwnership && na->nc == u->Account() && na->nc->HasFlag(NI_UNCONFIRMED) == false)
					u->SetMode(findbot(Config->NickServ), UMODE_REGISTERED);
			}

			u->lastnickreg = Anope::CurTime;
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply("\n");
		source.Reply(_("Registers your nickname in the %s database. Once\n"
				"your nick is registered, you can use the \002SET\002 and \002ACCESS\002\n"
				"commands to configure your nick's settings as you like\n"
				"them. Make sure you remember the password you use when\n"
				"registering - you'll need it to make changes to your nick\n"
				"later. (Note that \002case matters!\002 \037ANOPE\037, \037Anope\037, and \n"
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
				"Finally, the space character cannot be used in passwords.\n"),
				Config->NickServ.c_str(), Config->NickServ.c_str());

		if (!Config->NSForceEmail)
			source.Reply(_(" \n"
					"The parameter \037email\037 is optional and will set the email\n"
					"for your nick immediately.\n"
					"Your privacy is respected; this e-mail won't be given to\n"
					"any third-party person.\n"
					" \n"));

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
		this->SetSyntax("");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!Config->NSEmailReg)
			return;

		User *u = source.u;
		NickAlias *na = findnick(u->nick);

		if (na == NULL)
			source.Reply(NICK_NOT_REGISTERED);
		else if (na->nc != u->Account() || u->Account()->HasFlag(NI_UNCONFIRMED) == false)
			source.Reply(_("Your account is already confirmed."));
		else
		{
			if (Anope::CurTime < u->Account()->lastmail + Config->NSResendDelay)
				source.Reply(_("Cannot send mail now; please retry a little later."));
			else if (SendRegmail(u, na, source.owner))
			{
				na->nc->lastmail = Anope::CurTime;
				source.Reply(_("Your passcode has been re-sent to %s."), na->nc->email.c_str());
				Log(LOG_COMMAND, u, this) << "to resend registration verification code";
			}
			else
				Log() << "Unable to resend registration verification code for " << u->nick;
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (!Config->NSEmailReg)
			return false;

		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command will re-send the auth code (also called passcode)\n"
				"to the e-mail address of the user whom is performing it."));
		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		if (Config->NSEmailReg)
			Command::OnServHelp(source);
	}
};

class NSRegister : public Module
{
	CommandNSRegister commandnsregister;
	CommandNSConfirm commandnsconfirm;
	CommandNSResend commandnsrsend;

 public:
	NSRegister(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsregister(this), commandnsconfirm(this), commandnsrsend(this)
	{
		this->SetAuthor("Anope");

	}
};

static bool SendRegmail(User *u, NickAlias *na, BotInfo *bi)
{
	Anope::string *code = na->nc->GetExt<ExtensibleString *>("ns_register_passcode");
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
			codebuf += chars[1 + static_cast<int>((static_cast<float>(max - min)) * getrandom16() / 65536.0) + min];
		na->nc->Extend("ns_register_passcode", new ExtensibleString(codebuf));
	}
	else
		codebuf = *code;

	Anope::string subject = translate(na->nc, Config->MailRegistrationSubject.c_str());
	Anope::string message = translate(na->nc, Config->MailRegistrationMessage.c_str());

	subject = subject.replace_all_cs("%n", na->nick);
	subject = subject.replace_all_cs("%N", Config->NetworkName);
	subject = subject.replace_all_cs("%c", codebuf);

	message = message.replace_all_cs("%n", na->nick);
	message = message.replace_all_cs("%N", Config->NetworkName);
	message = message.replace_all_cs("%c", codebuf);

	return Mail(u, na->nc, bi, subject, message);
}

MODULE_INIT(NSRegister)
