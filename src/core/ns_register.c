/* NickServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

int do_sendregmail(User *u, NickRequest *nr);

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

		std::string tmp_pass;

		na->nc->pass = nr->password;

		na->nc->memos.memomax = Config.MSMaxMemos;

		if (force)
		{
			na->last_usermask = sstrdup("*@*");
			na->last_realname = sstrdup("unknown");
		}
		else
		{
			std::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
			na->last_usermask = sstrdup(last_usermask.c_str());
			na->last_realname = sstrdup(u->realname);
			if (Config.NSAddAccessOnReg)
				na->nc->AddAccess(create_mask(u));
		}

		na->time_registered = na->last_seen = time(NULL);
		na->nc->language = Config.NSDefLanguage;
		if (nr->email)
			na->nc->email = sstrdup(nr->email);

		if (!force)
		{
			u->Login(na->nc);
			Alog() << Config.s_NickServ << ": '" << u->nick << "' registered by " << u->GetIdent() << "@" << u->host << " (e-mail: " << (nr->email ? nr->email : "none") << ")";
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
			Alog() << Config.s_NickServ << ": '" << nr->nick << "' confirmed by " << u->GetMask() << " (email: " << (nr->email ? nr->email : "none") << " )";
			notice_lang(Config.s_NickServ, u, NICK_FORCE_REG, nr->nick);
			User *user = finduser(nr->nick);
			/* Delrequest must be called before validate_user */
			delete nr;
			if (user)
			{
				validate_user(user);
			}
		}
		
		FOREACH_MOD(I_OnNickRegister, OnNickRegister(na));

		return MOD_CONT;

	}

	CommandReturn DoConfirm(User *u, const std::vector<ci::string> &params)
	{
		NickRequest *nr = NULL;
		std::string passcode = !params.empty() ? params[0].c_str() : "";

		nr = findrequestnick(u->nick.c_str());

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
					nr = findrequestnick(passcode.c_str());
					if (nr)
					{
						ActuallyConfirmNick(u, nr, true);
						return MOD_CONT;
					}
				}
				notice_lang(Config.s_NickServ, u, NICK_CONFIRM_NOT_FOUND, Config.s_NickServ);

				return MOD_CONT;
			}

			if (nr->passcode.compare(passcode))
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
	CommandNSConfirm(const ci::string &cmdn, int min, int max) : Command(cmdn, min, max)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		return this->DoConfirm(u, params);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_CONFIRM);
		if (u->Account() && u->Account()->HasPriv("nickserv/confirm"))
			notice_help(Config.s_NickServ, u, NICK_HELP_CONFIRM_OPER);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		notice_lang(Config.s_NickServ, u, NICK_CONFIRM_INVALID);
	}
};

class CommandNSRegister : public CommandNSConfirm
{
 public:
	CommandNSRegister() : CommandNSConfirm("REGISTER", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickRequest *nr = NULL, *anr = NULL;
		NickAlias *na;
		int prefixlen = strlen(Config.NSGuestNickPrefix);
		int nicklen = u->nick.length();
		const char *pass = params[0].c_str();
		const char *email = params.size() > 1 ? params[1].c_str() : NULL;
		char passcode[11];
		int idx, min = 1, max = 62;
		int chars[] =
			{ ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
				'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
				'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
				'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
				'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
			};
		std::list<std::pair<std::string, std::string> >::iterator it;

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

		if ((anr = findrequestnick(u->nick.c_str())))
		{
			notice_lang(Config.s_NickServ, u, NICK_REQUESTED);
			return MOD_CONT;
		}

		/* Prevent "Guest" nicks from being registered. -TheShadow */

		/* Guest nick can now have a series of between 1 and 7 digits.
		 *   --lara
		 */
		if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1 && stristr(u->nick.c_str(), Config.NSGuestNickPrefix) == u->nick.c_str() && strspn(u->nick.c_str() + prefixlen, "1234567890") == nicklen - prefixlen)
		{
			notice_lang(Config.s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
			return MOD_CONT;
		}

		if (!ircdproto->IsNickValid(u->nick.c_str()))
		{
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, u->nick.c_str());
			return MOD_CONT;
		}

		if (Config.RestrictOperNicks)
		{
			for (it = Config.Opers.begin(); it != Config.Opers.end(); ++it)
			{
				std::string nick = it->first;

				if (stristr(u->nick.c_str(), nick.c_str()) && !is_oper(u))
				{
					notice_lang(Config.s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick.c_str());
					return MOD_CONT;
				}
			}
		}

		if (Config.NSForceEmail && !email)
			this->OnSyntaxError(u, "");
		else if (time(NULL) < u->lastnickreg + Config.NSRegDelay)
			notice_lang(Config.s_NickServ, u, NICK_REG_PLEASE_WAIT, (u->lastnickreg + Config.NSRegDelay) - time(NULL));
		else if ((na = findnick(u->nick.c_str())))
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
		else if (!stricmp(u->nick.c_str(), pass) || (Config.StrictPasswords && strlen(pass) < 5))
			notice_lang(Config.s_NickServ, u, MORE_OBSCURE_PASSWORD);
		else if (strlen(pass) > Config.PassLen)
			notice_lang(Config.s_NickServ, u, PASSWORD_TOO_LONG);
		else if (email && !MailValidate(email))
			notice_lang(Config.s_NickServ, u, MAIL_X_INVALID, email);
		else
		{
			for (idx = 0; idx < 9; ++idx)
				passcode[idx] = chars[1 + static_cast<int>((static_cast<float>(max - min)) * getrandom16() / 65536.0) + min];
			passcode[idx] = '\0';
			nr = new NickRequest(u->nick);
			nr->passcode = passcode;
			nr->password = pass;
			enc_encrypt_in_place(nr->password);
			if (email)
				nr->email = sstrdup(email);
			nr->requested = time(NULL);
			FOREACH_MOD(I_OnMakeNickRequest, OnMakeNickRequest(nr));
			if (Config.NSEmailReg)
			{
				if (!do_sendregmail(u, nr))
				{
					notice_lang(Config.s_NickServ, u, NICK_ENTER_REG_CODE, email, Config.s_NickServ);
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
				std::vector<ci::string> empty_params;
				return this->DoConfirm(u, empty_params);
			}
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_REGISTER);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		if (Config.NSForceEmail)
			syntax_error(Config.s_NickServ, u, "REGISTER", NICK_REGISTER_SYNTAX_EMAIL);
		else
			syntax_error(Config.s_NickServ, u, "REGISTER", NICK_REGISTER_SYNTAX);
	}
};

class CommandNSResend : public Command
{
 public:
	CommandNSResend() : Command("RESEND", 0, 0)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickRequest *nr = NULL;
		if (Config.NSEmailReg)
		{
			if ((nr = findrequestnick(u->nick.c_str())))
			{
				if (time(NULL) < nr->lastmail + Config.NSResendDelay)
				{
					notice_lang(Config.s_NickServ, u, MAIL_LATER);
					return MOD_CONT;
				}
				if (!do_sendregmail(u, nr))
				{
					nr->lastmail = time(NULL);
					notice_lang(Config.s_NickServ, u, NICK_REG_RESENT, nr->email);
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

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_RESEND);
		return true;
	}
};

class NSRegister : public Module
{
 public:
	NSRegister(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSRegister());
		this->AddCommand(NICKSERV, new CommandNSConfirm("CONFIRM", 1, 1));
		this->AddCommand(NICKSERV, new CommandNSResend());

		ModuleManager::Attach(I_OnNickServHelp, this);
	}
	void OnNickServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_REGISTER);
		if (Config.NSEmailReg)
		{
			notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_CONFIRM);
			notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_RESEND);
		}
	}
};

/*************************************************************************/

int do_sendregmail(User *u, NickRequest *nr)
{
	MailInfo *mail = NULL;
	char buf[BUFSIZE];

	if (!nr && !u)
		return -1;
	snprintf(buf, sizeof(buf), getstring(NICK_REG_MAIL_SUBJECT), nr->nick);
	mail = MailRegBegin(u, nr, buf, Config.s_NickServ);
	if (!mail)
		return -1;
	fprintf(mail->pipe, "%s", getstring(NICK_REG_MAIL_HEAD));
	fprintf(mail->pipe, "\n\n");
	fprintf(mail->pipe, getstring(NICK_REG_MAIL_LINE_1), nr->nick);
	fprintf(mail->pipe, "\n\n");
	fprintf(mail->pipe, getstring(NICK_REG_MAIL_LINE_2), Config.s_NickServ, nr->passcode.c_str());
	fprintf(mail->pipe, "\n\n");
	fprintf(mail->pipe, "%s", getstring(NICK_REG_MAIL_LINE_3));
	fprintf(mail->pipe, "\n\n");
	fprintf(mail->pipe, "%s", getstring(NICK_REG_MAIL_LINE_4));
	fprintf(mail->pipe, "\n\n");
	fprintf(mail->pipe, getstring(NICK_REG_MAIL_LINE_5), Config.NetworkName);
	fprintf(mail->pipe, "\n.\n");
	MailEnd(mail);
	return 0;
}

MODULE_INIT(NSRegister)
