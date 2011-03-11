/* NickServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

static bool SendRegmail(User *u, NickAlias *na);

class CommandNSConfirm : public Command
{
 public:
	CommandNSConfirm() : Command("CONFIRM", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Confirm an auth code"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &passcode = params[0];

		if (u->Account() && u->Account()->HasPriv("nickserv/confirm"))
		{
			NickAlias *na = findnick(passcode);
			if (na == NULL)
				source.Reply(_(NICK_X_NOT_REGISTERED), passcode.c_str());
			else if (na->nc->HasFlag(NI_UNCONFIRMED) == false)
				source.Reply(_("Nick \002%s\002 is already confirmed."), na->nick.c_str());
			{
				na->nc->UnsetFlag(NI_UNCONFIRMED);
				Log(LOG_ADMIN, u, this) << "to confirm nick " << na->nick << " (" << na->nc->display << ")";
				source.Reply(_("Nick \002%s\002 has been confirmed."), na->nick.c_str());
			}
		}
		else if (u->Account())
		{
			Anope::string code;
			if (u->Account()->GetExtRegular<Anope::string>("ns_register_passcode", code) && code == passcode)
			{
				u->Account()->Shrink("ns_register_passcode");
				Log(LOG_COMMAND, u, this) << "to confirm their email";
				source.Reply(_("Your email address of \002%s\002 has been confirmed."), u->Account()->email.c_str());
				u->Account()->UnsetFlag(NI_UNCONFIRMED);
			}
			else
				source.Reply(_("Invalid passcode."));
		}
		else
			source.Reply(_("Invalid passcode."));

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		source.Reply(_("Syntax: \002CONFIRM \037passcode\037\002\n"
				" \n"
				"This command is used by several commands as a way to confirm\n"
				"changes made to your account.\n"
				" \n"
				"This is most commonly used to confirm your email address once\n"
				"you register or change it.\n"
				" \n"
				"This is also used after the RESETPASS command has been used to\n"
				"force identify you to your nick so you may change your password."));
		if (u->Account() && u->Account()->HasPriv("nickserv/confirm"))
			source.Reply(_("Additionally, Services Operators with the \037nickserv/confirm\037 permission can\n"
				"replace \037passcode\037 with a users nick to force validate them."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_(NICK_CONFIRM_INVALID));
	}
};

class CommandNSRegister : public Command
{
 public:
	CommandNSRegister() : Command("REGISTER", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Register a nickname"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
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
			return MOD_CONT;
		}

		if (!u->HasMode(UMODE_OPER) && Config->NickRegDelay && Anope::CurTime - u->my_signon < Config->NickRegDelay)
		{
			source.Reply(_("You must have been using this nick for at least %d seconds to register."), Config->NickRegDelay);
			return MOD_CONT;
		}

		/* Prevent "Guest" nicks from being registered. -TheShadow */

		/* Guest nick can now have a series of between 1 and 7 digits.
		 *   --lara
		 */
		if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1 && !u->nick.find_ci(Config->NSGuestNickPrefix) && u->nick.substr(prefixlen).find_first_not_of("1234567890") == Anope::string::npos)
		{
			source.Reply(_(NICK_CANNOT_BE_REGISTERED), u->nick.c_str());
			return MOD_CONT;
		}

		if (!ircdproto->IsNickValid(u->nick))
		{
			source.Reply(_(NICK_X_FORBIDDEN), u->nick.c_str());
			return MOD_CONT;
		}

		if (Config->RestrictOperNicks)
			for (std::list<std::pair<Anope::string, Anope::string> >::iterator it = Config->Opers.begin(), it_end = Config->Opers.end(); it != it_end; ++it)
			{
				Anope::string nick = it->first;

				if (u->nick.find_ci(nick) != Anope::string::npos && !u->HasMode(UMODE_OPER))
				{
					source.Reply(_(NICK_CANNOT_BE_REGISTERED), u->nick.c_str());
					return MOD_CONT;
				}
		}

		if (Config->NSForceEmail && email.empty())
			this->OnSyntaxError(source, "");
		else if (Anope::CurTime < u->lastnickreg + Config->NSRegDelay)
			source.Reply(_("Please wait %d seconds before using the REGISTER command again."), (u->lastnickreg + Config->NSRegDelay) - Anope::CurTime);
		else if ((na = findnick(u->nick)))
		{
			/* i.e. there's already such a nick regged */
			if (na->HasFlag(NS_FORBIDDEN))
			{
				Log(NickServ) << u->GetMask() << " tried to register FORBIDden nick " << u->nick;
				source.Reply(_(NICK_CANNOT_BE_REGISTERED), u->nick.c_str());
			}
			else
				source.Reply(_(NICK_ALREADY_REGISTERED), u->nick.c_str());
		}
		else if (pass.equals_ci(u->nick) || (Config->StrictPasswords && pass.length() < 5))
			source.Reply(_(MORE_OBSCURE_PASSWORD));
		else if (pass.length() > Config->PassLen)
			source.Reply(_(PASSWORD_TOO_LONG));
		else if (!email.empty() && !MailValidate(email))
			source.Reply(_(MAIL_X_INVALID), email.c_str());
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
				if (SendRegmail(u, na))
				{
					source.Reply(_("A passcode has been sent to %s, please type %R%s confirm <passcode> to confirm your email address."), email.c_str(), NickServ->nick.c_str());
					source.Reply(_("If you do not confirm your email address within %s your account will expire."), duration(Config->NSUnconfirmedExpire).c_str());
				}
			}

			ircdproto->SendAccountLogin(u, u->Account());
			ircdproto->SetAutoIdentificationToken(u);

			u->lastnickreg = Anope::CurTime;
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002REGISTER \037password\037 \037[email]\037\002\n"
				" \n"
				"Registers your nickname in the %s database. Once\n"
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
				"Finally, the space character cannot be used in passwords.\n"
				" \n"
				"The parameter \037email\037 is optional and will set the email\n"
				"for your nick immediately. However, it may be required\n"
				"on certain networks.\n"
				"Your privacy is respected; this e-mail won't be given to\n"
				"any third-party person.\n"
				" \n"
				"This command also creates a new group for your nickname,\n"
				"that will allow you to register other nicks later sharing\n"
				"the same configuration, the same set of memos and the\n"
				"same channel privileges. For more information on this\n"
				"feature, type \002%R%s HELP GROUP\002."),
				NickServ->nick.c_str(), NickServ->nick.c_str(), NickServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		if (Config->NSForceEmail)
			SyntaxError(source, "REGISTER", _("\037password\037 \037email\037"));
		else
			SyntaxError(source, "REGISTER", _("\037password\037 [\037email\037]"));
	}
};

class CommandNSResend : public Command
{
 public:
	CommandNSResend() : Command("RESEND", 0, 0)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!Config->NSEmailReg)
			return MOD_CONT;

		User *u = source.u;
		NickAlias *na = findnick(u->nick);

		if (na == NULL)
			source.Reply(_(NICK_NOT_REGISTERED));
		else if (na->nc != u->Account() || u->Account()->HasFlag(NI_UNCONFIRMED) == false)
			source.Reply(_("Your account is already confirmed."));
		else
		{
			if (Anope::CurTime < u->Account()->lastmail + Config->NSResendDelay)
				source.Reply(_("Cannot send mail now; please retry a little later."));
			else if (!SendRegmail(u, na))
			{
				na->nc->lastmail = Anope::CurTime;
				source.Reply(_("Your passcode has been re-sent to %s."), na->nc->email.c_str());
				Log(LOG_COMMAND, u, this) << "to resend registration verification code";
			}
			else
				Log() << "Unable to resend registration verification code for " << u->nick;
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		if (!Config->NSEmailReg)
			return false;

		source.Reply(_("Syntax: \002RESEND\002\n"
				" \n"
				"This command will re-send the auth code (also called passcode)\n"
				"to the e-mail address of the user whom is performing it."));
		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		if (Config->NSEmailReg)
			source.Reply(_("Resend the registration passcode"));
	}
};

class NSRegister : public Module
{
	CommandNSRegister commandnsregister;
	CommandNSConfirm commandnsconfirm;
	CommandNSResend commandnsrsend;

 public:
	NSRegister(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsregister);
		this->AddCommand(NickServ, &commandnsconfirm);
		this->AddCommand(NickServ, &commandnsrsend);
	}
};

static bool SendRegmail(User *u, NickAlias *na)
{
	Anope::string code;
	if (na->nc->GetExtRegular<Anope::string>("ns_register_passcode", code) == false)
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
			code += chars[1 + static_cast<int>((static_cast<float>(max - min)) * getrandom16() / 65536.0) + min];
		na->nc->Extend("ns_register_passcode", new ExtensibleItemRegular<Anope::string>(code));
	}

	Anope::string subject = Anope::printf(_("Nickname Registration (%s)"), na->nick.c_str());
	Anope::string message = Anope::printf(_("Hi,\n"
	" \n"
	"You have requested to register the nickname %s on %s.\n"
	"Please type \" %R%s confirm %s \" to complete registration.\n"
	" \n"
	"If you don't know why this mail was sent to you, please ignore it silently.\n"
	" \n"
	"%s administrators."), na->nick.c_str(), Config->NetworkName.c_str(), Config->s_NickServ.c_str(), code.c_str(), Config->NetworkName.c_str());

	if (Config->UseStrictPrivMsg)
		message = message.replace_all_cs("%R", "/");
	else
		message = message.replace_all_cs("%R", "/msg ");

	return Mail(u, na->nc, NickServ, subject, message);
}

MODULE_INIT(NSRegister)
