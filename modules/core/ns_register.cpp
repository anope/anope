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

static bool SendRegmail(User *u, NickRequest *nr);

class CommandNSConfirm : public Command
{
 protected:
	CommandReturn ActuallyConfirmNick(CommandSource &source, NickRequest *nr, bool force)
	{
		User *u = source.u;
		NickAlias *na = new NickAlias(nr->nick, new NickCore(nr->nick));
		Anope::string tmp_pass;

		na->nc->pass = nr->password;

		na->nc->memos.memomax = Config->MSMaxMemos;

		if (force)
		{
			na->last_usermask = "*@*";
			na->last_realname = "unknown";
		}
		else
		{
			Anope::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
			na->last_usermask = last_usermask;
			na->last_realname = u->realname;
			if (Config->NSAddAccessOnReg)
				na->nc->AddAccess(create_mask(u));
		}

		na->time_registered = na->last_seen = Anope::CurTime;
		na->nc->language = Config->NSDefLanguage;
		if (!nr->email.empty())
			na->nc->email = nr->email;

		if (!force)
		{
			u->Login(na->nc);
			Log(LOG_COMMAND, u, this) << "to register " << nr->nick << " (email: " << (!nr->email.empty() ? nr->email : "none") << ")";
			if (Config->NSAddAccessOnReg)
				source.Reply(_("Nickname \002%s\002 registered under your account: %s"), u->nick.c_str(), na->nc->GetAccess(0).c_str());
			else
				source.Reply(_("Nickname \002%s\002 registered."), u->nick.c_str());
			delete nr;

			ircdproto->SendAccountLogin(u, u->Account());
			ircdproto->SetAutoIdentificationToken(u);

			if (enc_decrypt(na->nc->pass, tmp_pass) == 1)
				source.Reply(_("Your password is \002%s\002 - remember this for later use."), tmp_pass.c_str());

			u->lastnickreg = Anope::CurTime;
		}
		else
		{
			Log(LOG_COMMAND, u, this) << "to confirm " << nr->nick << " (email: " << (!nr->email.empty() ? nr->email : "none") << ")";
			source.Reply(_("Nickname \002%s\002 confirmed"), nr->nick.c_str());
			User *user = finduser(nr->nick);
			/* Delrequest must be called before validate_user */
			delete nr;
			if (user)
				validate_user(user);
		}

		FOREACH_MOD(I_OnNickRegister, OnNickRegister(na));

		return MOD_CONT;
	}

	CommandReturn DoConfirm(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		Anope::string passcode = !params.empty() ? params[0] : "";

		NickRequest *nr = findrequestnick(u->nick);

		if (Config->NSEmailReg)
		{
			if (passcode.empty())
			{
				this->OnSyntaxError(source, "");
				return MOD_CONT;
			}

			if (!nr)
			{
				if (u->Account() && u->Account()->HasPriv("nickserv/confirm"))
				{
					/* If an admin, their nick is obviously already regged, so look at the passcode to get the nick
					   of the user they are trying to validate, and push that user through regardless of passcode */
					nr = findrequestnick(passcode);
					if (nr)
					{
						ActuallyConfirmNick(source, nr, true);
						return MOD_CONT;
					}
				}
				source.Reply(_("Registration step 1 may have expired, please use \"%R%s register <password> <email>\" first."), Config->s_NickServ.c_str());

				return MOD_CONT;
			}

			if (!nr->passcode.equals_cs(passcode))
			{
				source.Reply(LanguageString::NICK_CONFIRM_INVALID);
				return MOD_CONT;
			}
		}

		if (!nr)
		{
			source.Reply(_("Sorry, registration failed."));
			return MOD_CONT;
		}

		ActuallyConfirmNick(source, nr, false);
		return MOD_CONT;
	}

 public:
	CommandNSConfirm(const Anope::string &cmdn, int min, int max) : Command(cmdn, min, max)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc("Confirm a nickserv auth code");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		return this->DoConfirm(source, params);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		source.Reply(_("Syntax: \002CONFIRM \037passcode\037\002\n"
				" \n"
				"This is the second step of nickname registration process.\n"
				"You must perform this command in order to get your nickname\n"
				"registered with %s. The passcode (or called auth code also)\n"
				"is sent to your e-mail address in the first step of the\n"
				"registration process. For more information about the first\n"
				"stage of the registration process, type: \002%R%s HELP REGISTER\002\n"
				" \n"
				"This is also used after the RESETPASS command has been used to\n"
				"force identify you to your nick so you may change your password."),
				NickServ->nick.c_str());
		if (u->Account() && u->Account()->HasPriv("nickserv/confirm"))
			source.Reply(_("Additionally, Services Operators with the \037nickserv/confirm\037 permission can\n"
				"replace \037passcode\037 with a users nick to force validate them."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(LanguageString::NICK_CONFIRM_INVALID);
	}
};

class CommandNSRegister : public CommandNSConfirm
{
 public:
	CommandNSRegister() : CommandNSConfirm("REGISTER", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc("Register a nickname");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickRequest *nr = NULL, *anr = NULL;
		NickAlias *na;
		size_t prefixlen = Config->NSGuestNickPrefix.length();
		size_t nicklen = u->nick.length();
		Anope::string pass = params[0];
		Anope::string email = params.size() > 1 ? params[1] : "";
		Anope::string passcode;
		int idx, min = 1, max = 62;
		int chars[] = {
			' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
			'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
			'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
			'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
			'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
		};
		std::list<std::pair<Anope::string, Anope::string> >::iterator it, it_end;

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

		if ((anr = findrequestnick(u->nick)))
		{
			source.Reply(LanguageString::NICK_REQUESTED);
			return MOD_CONT;
		}

		/* Prevent "Guest" nicks from being registered. -TheShadow */

		/* Guest nick can now have a series of between 1 and 7 digits.
		 *   --lara
		 */
		if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1 && !u->nick.find_ci(Config->NSGuestNickPrefix) && u->nick.substr(prefixlen).find_first_not_of("1234567890") == Anope::string::npos)
		{
			source.Reply(LanguageString::NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
			return MOD_CONT;
		}

		if (!ircdproto->IsNickValid(u->nick))
		{
			source.Reply(LanguageString::NICK_X_FORBIDDEN, u->nick.c_str());
			return MOD_CONT;
		}

		if (Config->RestrictOperNicks)
			for (it = Config->Opers.begin(), it_end = Config->Opers.end(); it != it_end; ++it)
			{
				Anope::string nick = it->first;

				if (u->nick.find_ci(nick) != Anope::string::npos && !u->HasMode(UMODE_OPER))
				{
					source.Reply(LanguageString::NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
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
				source.Reply(LanguageString::NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
			}
			else
				source.Reply(LanguageString::NICK_ALREADY_REGISTERED, u->nick.c_str());
		}
		else if (pass.equals_ci(u->nick) || (Config->StrictPasswords && pass.length() < 5))
			source.Reply(LanguageString::MORE_OBSCURE_PASSWORD);
		else if (pass.length() > Config->PassLen)
			source.Reply(LanguageString::PASSWORD_TOO_LONG);
		else if (!email.empty() && !MailValidate(email))
			source.Reply(LanguageString::MAIL_X_INVALID, email.c_str());
		else
		{
			for (idx = 0; idx < 9; ++idx)
				passcode += chars[1 + static_cast<int>((static_cast<float>(max - min)) * getrandom16() / 65536.0) + min];
			nr = new NickRequest(u->nick);
			nr->passcode = passcode;
			enc_encrypt(pass, nr->password);
			if (!email.empty())
				nr->email = email;
			nr->requested = Anope::CurTime;
			FOREACH_MOD(I_OnMakeNickRequest, OnMakeNickRequest(nr));
			if (Config->NSEmailReg)
			{
				if (SendRegmail(u, nr))
				{
					source.Reply(_("A passcode has been sent to %s, please type %R%s confirm <passcode> to complete registration.\n"
					  "If you need to cancel your registration, use \"%R%s drop <password>\"."),
					  email.c_str(), Config->s_NickServ.c_str(), Config->s_NickServ.c_str());

					Log(LOG_COMMAND, u, this) << "send registration verification code to " << nr->email;
				}
				else
				{
					Log(LOG_COMMAND, u, this) << "unable to send registration verification mail";
					source.Reply(_("Nick NOT registered, please try again later."));
					delete nr;
					return MOD_CONT;
				}
			}
			else
			{
				std::vector<Anope::string> empty_params;
				return this->DoConfirm(source, empty_params);
			}
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
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc("Resend a nickserv auth code");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		NickRequest *nr = NULL;
		if (Config->NSEmailReg)
		{
			if ((nr = findrequestnick(u->nick)))
			{
				if (Anope::CurTime < nr->lastmail + Config->NSResendDelay)
				{
					source.Reply(_("Cannot send mail now; please retry a little later."));
					return MOD_CONT;
				}
				if (!SendRegmail(u, nr))
				{
					nr->lastmail = Anope::CurTime;
					source.Reply(_("Your passcode has been re-sent to %s."), nr->email.c_str());
					Log(LOG_COMMAND, u, this) << "resend registration verification code for " << nr->nick;
				}
				else
				{
					Log(LOG_COMMAND, u, this) << "unable to resend registration verification code for " << nr->nick;
					return MOD_CONT;
				}
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002RESEND\002\n"
				" \n"
				"This command will re-send the auth code (also called passcode)\n"
				"to the e-mail address of the user whom is performing it."));
		return true;
	}
};

class NSRegister : public Module
{
	CommandNSRegister commandnsregister;
	CommandNSConfirm commandnsconfirm;
	CommandNSResend commandnsrsend;

 public:
	NSRegister(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), commandnsconfirm("CONFIRM", 1, 1)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsregister);
		this->AddCommand(NickServ, &commandnsconfirm);
		this->AddCommand(NickServ, &commandnsrsend);
	}
};

static bool SendRegmail(User *u, NickRequest *nr)
{
	char subject[BUFSIZE], message[BUFSIZE];

	snprintf(subject, sizeof(subject), GetString(NULL, "Nickname Registration (%s)").c_str(), nr->nick.c_str());
	snprintf(message, sizeof(message), GetString(NULL, "Hi,\n"
	" \n"
	"You have requested to register the nickname %s on %s.\n"
	"Please type \" %R%s confirm %s \" to complete registration.\n"
	" \n"
	"If you don't know why this mail was sent to you, please ignore it silently.\n"
	" \n"
	"%s administrators.").c_str(), nr->nick.c_str(), Config->NetworkName.c_str(), Config->s_NickServ.c_str(), nr->passcode.c_str(), Config->NetworkName.c_str());

	return Mail(u, nr, NickServ, subject, message);
}

MODULE_INIT(NSRegister)
