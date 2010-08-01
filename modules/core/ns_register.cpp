/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
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
	CommandReturn ActuallyConfirmNick(User *u, NickRequest *nr, bool force)
	{
		NickAlias *na = new NickAlias(nr->nick, new NickCore(nr->nick));

		if (!na)
		{
			Alog() << Config.s_NickServ << ": makenick(" << u->nick << ") failed";
			notice_lang(Config.s_NickServ, u, NICK_REGISTRATION_FAILED);
			return MOD_CONT;
		}

		Anope::string tmp_pass;

		na->nc->pass = nr->password;

		na->nc->memos.memomax = Config.MSMaxMemos;

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
			if (Config.NSAddAccessOnReg)
				na->nc->AddAccess(create_mask(u));
		}

		na->time_registered = na->last_seen = time(NULL);
		na->nc->language = Config.NSDefLanguage;
		if (!nr->email.empty())
			na->nc->email = nr->email;

		if (!force)
		{
			u->Login(na->nc);
			Alog() << Config.s_NickServ << ": '" << u->nick << "' registered by " << u->GetIdent() << "@" << u->host << " (e-mail: " << (!nr->email.empty() ? nr->email : "none") << ")";
			if (Config.NSAddAccessOnReg)
				notice_lang(Config.s_NickServ, u, NICK_REGISTERED, u->nick.c_str(), na->nc->GetAccess(0).c_str());
			else
				notice_lang(Config.s_NickServ, u, NICK_REGISTERED_NO_MASK, u->nick.c_str());
			delete nr;

			ircdproto->SendAccountLogin(u, u->Account());
			ircdproto->SetAutoIdentificationToken(u);

			if (enc_decrypt(na->nc->pass, tmp_pass) == 1)
				notice_lang(Config.s_NickServ, u, NICK_PASSWORD_IS, tmp_pass.c_str());

			u->lastnickreg = time(NULL);
		}
		else
		{
			Alog() << Config.s_NickServ << ": '" << nr->nick << "' confirmed by " << u->GetMask() << " (email: " << (!nr->email.empty() ? nr->email : "none") << " )";
			notice_lang(Config.s_NickServ, u, NICK_FORCE_REG, nr->nick.c_str());
			User *user = finduser(nr->nick);
			/* Delrequest must be called before validate_user */
			delete nr;
			if (user)
				validate_user(user);
		}

		FOREACH_MOD(I_OnNickRegister, OnNickRegister(na));

		return MOD_CONT;
	}

	CommandReturn DoConfirm(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string passcode = !params.empty() ? params[0] : "";

		NickRequest *nr = findrequestnick(u->nick);

		if (Config.NSEmailReg)
		{
			if (passcode.empty())
			{
				this->OnSyntaxError(u, "");
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
						ActuallyConfirmNick(u, nr, true);
						return MOD_CONT;
					}
				}
				notice_lang(Config.s_NickServ, u, NICK_CONFIRM_NOT_FOUND, Config.s_NickServ.c_str());

				return MOD_CONT;
			}

			if (!nr->passcode.equals_cs(passcode))
			{
				notice_lang(Config.s_NickServ, u, NICK_CONFIRM_INVALID);
				return MOD_CONT;
			}
		}

		if (!nr)
		{
			notice_lang(Config.s_NickServ, u, NICK_REGISTRATION_FAILED);
			return MOD_CONT;
		}

		ActuallyConfirmNick(u, nr, false);
		return MOD_CONT;
	}

 public:
	CommandNSConfirm(const Anope::string &cmdn, int min, int max) : Command(cmdn, min, max)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		return this->DoConfirm(u, params);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_CONFIRM);
		if (u->Account() && u->Account()->HasPriv("nickserv/confirm"))
			notice_help(Config.s_NickServ, u, NICK_HELP_CONFIRM_OPER);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		notice_lang(Config.s_NickServ, u, NICK_CONFIRM_INVALID);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_CONFIRM);
	}
};

class CommandNSRegister : public CommandNSConfirm
{
 public:
	CommandNSRegister() : CommandNSConfirm("REGISTER", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickRequest *nr = NULL, *anr = NULL;
		NickAlias *na;
		size_t prefixlen = Config.NSGuestNickPrefix.length();
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
			notice_lang(Config.s_NickServ, u, NICK_REGISTRATION_DISABLED);
			return MOD_CONT;
		}

		if (!is_oper(u) && Config.NickRegDelay && time(NULL) - u->my_signon < Config.NickRegDelay)
		{
			notice_lang(Config.s_NickServ, u, NICK_REG_DELAY, Config.NickRegDelay);
			return MOD_CONT;
		}

		if ((anr = findrequestnick(u->nick)))
		{
			notice_lang(Config.s_NickServ, u, NICK_REQUESTED);
			return MOD_CONT;
		}

		/* Prevent "Guest" nicks from being registered. -TheShadow */

		/* Guest nick can now have a series of between 1 and 7 digits.
		 *   --lara
		 */
		if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1 && !u->nick.find_ci(Config.NSGuestNickPrefix) && !u->nick.substr(prefixlen).find_first_not_of("1234567890"))
		{
			notice_lang(Config.s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
			return MOD_CONT;
		}

		if (!ircdproto->IsNickValid(u->nick))
		{
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, u->nick.c_str());
			return MOD_CONT;
		}

		if (Config.RestrictOperNicks)
			for (it = Config.Opers.begin(), it_end = Config.Opers.end(); it != it_end; ++it)
			{
				Anope::string nick = it->first;

				if (u->nick.find_ci(nick) != Anope::string::npos && !is_oper(u))
				{
					notice_lang(Config.s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
					return MOD_CONT;
				}
		}

		if (Config.NSForceEmail && email.empty())
			this->OnSyntaxError(u, "");
		else if (time(NULL) < u->lastnickreg + Config.NSRegDelay)
			notice_lang(Config.s_NickServ, u, NICK_REG_PLEASE_WAIT, (u->lastnickreg + Config.NSRegDelay) - time(NULL));
		else if ((na = findnick(u->nick)))
		{
			/* i.e. there's already such a nick regged */
			if (na->HasFlag(NS_FORBIDDEN))
			{
				Alog() << Config.s_NickServ << ": " << u->GetIdent() << "@" << u->host << " tried to register FORBIDden nick " << u->nick;
				notice_lang(Config.s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
			}
			else
				notice_lang(Config.s_NickServ, u, NICK_ALREADY_REGISTERED, u->nick.c_str());
		}
		else if (pass.equals_ci(u->nick) || (Config.StrictPasswords && pass.length() < 5))
			notice_lang(Config.s_NickServ, u, MORE_OBSCURE_PASSWORD);
		else if (pass.length() > Config.PassLen)
			notice_lang(Config.s_NickServ, u, PASSWORD_TOO_LONG);
		else if (!email.empty() && !MailValidate(email))
			notice_lang(Config.s_NickServ, u, MAIL_X_INVALID, email.c_str());
		else
		{
			for (idx = 0; idx < 9; ++idx)
				passcode += chars[1 + static_cast<int>((static_cast<float>(max - min)) * getrandom16() / 65536.0) + min];
			nr = new NickRequest(u->nick);
			nr->passcode = passcode;
			enc_encrypt(pass, nr->password);
			if (!email.empty())
				nr->email = email;
			nr->requested = time(NULL);
			FOREACH_MOD(I_OnMakeNickRequest, OnMakeNickRequest(nr));
			if (Config.NSEmailReg)
			{
				if (SendRegmail(u, nr))
				{
					notice_lang(Config.s_NickServ, u, NICK_ENTER_REG_CODE, email.c_str(), Config.s_NickServ.c_str());
					Alog() << Config.s_NickServ << ": sent registration verification code to " << nr->email;
				}
				else
				{
					Alog() << Config.s_NickServ << ": Unable to send registration verification mail";
					notice_lang(Config.s_NickServ, u, NICK_REG_UNABLE);
					delete nr;
					return MOD_CONT;
				}
			}
			else
			{
				std::vector<Anope::string> empty_params;
				return this->DoConfirm(u, empty_params);
			}
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_REGISTER);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		if (Config.NSForceEmail)
			syntax_error(Config.s_NickServ, u, "REGISTER", NICK_REGISTER_SYNTAX_EMAIL);
		else
			syntax_error(Config.s_NickServ, u, "REGISTER", NICK_REGISTER_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_REGISTER);
	}
};

class CommandNSResend : public Command
{
 public:
	CommandNSResend() : Command("RESEND", 0, 0)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickRequest *nr = NULL;
		if (Config.NSEmailReg)
		{
			if ((nr = findrequestnick(u->nick)))
			{
				if (time(NULL) < nr->lastmail + Config.NSResendDelay)
				{
					notice_lang(Config.s_NickServ, u, MAIL_LATER);
					return MOD_CONT;
				}
				if (!SendRegmail(u, nr))
				{
					nr->lastmail = time(NULL);
					notice_lang(Config.s_NickServ, u, NICK_REG_RESENT, nr->email.c_str());
					Alog() << Config.s_NickServ << ": re-sent registration verification code for " << nr->nick << " to " << nr->email;
				}
				else
				{
					Alog() << Config.s_NickServ << ": Unable to re-send registration verification mail for " << nr->nick;
					return MOD_CONT;
				}
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_RESEND);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_RESEND);
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

	snprintf(subject, sizeof(subject), getstring(NICK_REG_MAIL_SUBJECT), nr->nick.c_str());
	snprintf(message, sizeof(message), getstring(NICK_REG_MAIL), nr->nick.c_str(), Config.NetworkName.c_str(), Config.s_NickServ.c_str(), nr->passcode.c_str(), Config.NetworkName.c_str());

	return Mail(u, nr, Config.s_NickServ, subject, message);
}

MODULE_INIT(NSRegister)
