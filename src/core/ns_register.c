/* NickServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

NickRequest *makerequest(const char *nick);
NickAlias *makenick(const char *nick);
int do_sendregmail(User *u, NickRequest *nr);

class CommandNSConfirm : public Command
{
 protected:
	CommandReturn ActuallyConfirmNick(User *u, NickRequest *nr, bool force)
	{
		NickAlias *na;
		na = makenick(nr->nick);

		if (!na)
		{
			alog("%s: makenick(%s) failed", s_NickServ, u->nick);
			notice_lang(s_NickServ, u, NICK_REGISTRATION_FAILED);
			return MOD_CONT;
		}

		char tmp_pass[PASSMAX];

		memcpy(na->nc->pass, nr->password, PASSMAX);

		na->nc->memos.memomax = MSMaxMemos;

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
			if (NSAddAccessOnReg)
				na->nc->AddAccess(create_mask(u));
		}

		na->time_registered = na->last_seen = time(NULL);
		na->nc->language = NSDefLanguage;
		if (nr->email)
			na->nc->email = sstrdup(nr->email);

		SetOperType(na->nc);

		if (!force)
		{
			u->nc = na->nc;
			alog("%s: '%s' registered by %s@%s (e-mail: %s)", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, nr->email ? nr->email : "none");
			if (NSAddAccessOnReg)
				notice_lang(s_NickServ, u, NICK_REGISTERED, u->nick, na->nc->GetAccess(0).c_str());
			else
				notice_lang(s_NickServ, u, NICK_REGISTERED_NO_MASK, u->nick);
			delnickrequest(nr);

			ircdproto->SendAccountLogin(u, u->nc);
			ircdproto->SetAutoIdentificationToken(u);

			if (enc_decrypt(na->nc->pass, tmp_pass, PASSMAX - 1) == 1)
				notice_lang(s_NickServ, u, NICK_PASSWORD_IS, tmp_pass);

			u->lastnickreg = time(NULL);
		}
		else
		{
			alog("%s: '%s' confirmed by %s!%s@%s (email: %s)", s_NickServ, nr->nick, u->nick, u->GetIdent().c_str(), u->host, nr->email ? nr->email : "none");

			notice_lang(s_NickServ, u, NICK_FORCE_REG, nr->nick);

			User *user = finduser(nr->nick);
			/* Delrequest must be called before validate_user */
			delnickrequest(nr);
			if (user)
			{
				validate_user(user);
			}
		}
		return MOD_CONT;

	}

	CommandReturn DoConfirm(User *u, std::vector<ci::string> &params)
	{
		NickRequest *nr = NULL;
		const char *passcode = params.size() ? params[0].c_str() : NULL;

		nr = findrequestnick(u->nick);

		if (NSEmailReg)
		{
			if (!passcode)
			{
				notice_lang(s_NickServ, u, NICK_CONFIRM_INVALID);
				return MOD_CONT;
			}

			if (!nr)
			{
				if (u->nc && u->nc->HasPriv("nickserv/confirm"))
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
				notice_lang(s_NickServ, u, NICK_CONFIRM_NOT_FOUND, s_NickServ);

				return MOD_CONT;
			}

			if (stricmp(nr->passcode, passcode))
			{
				notice_lang(s_NickServ, u, NICK_CONFIRM_INVALID);
				return MOD_CONT;
			}
		}

		if (!nr)
		{
			notice_lang(s_NickServ, u, NICK_REGISTRATION_FAILED);
			return MOD_CONT;
		}

		ActuallyConfirmNick(u, nr, false);
		return MOD_CONT;
	}
 public:
	CommandNSConfirm(const std::string &cmdn, int min, int max) : Command(cmdn, min, max)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		return this->DoConfirm(u, params);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_NickServ, u, NICK_HELP_CONFIRM);
		if (u->nc && u->nc->HasPriv("nickserv/confirm"))
			notice_help(s_NickServ, u, NICK_HELP_CONFIRM_OPER);
		return true;
	}
};

class CommandNSRegister : public CommandNSConfirm
{
 public:
	CommandNSRegister() : CommandNSConfirm("REGISTER", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		NickRequest *nr = NULL, *anr = NULL;
		NickAlias *na;
		int prefixlen = strlen(NSGuestNickPrefix);
		int nicklen = strlen(u->nick);
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
			notice_lang(s_NickServ, u, NICK_REGISTRATION_DISABLED);
			return MOD_CONT;
		}

		if (!is_oper(u) && NickRegDelay && time(NULL) - u->my_signon < NickRegDelay)
		{
			notice_lang(s_NickServ, u, NICK_REG_DELAY, NickRegDelay);
			return MOD_CONT;
		}

		if ((anr = findrequestnick(u->nick)))
		{
			notice_lang(s_NickServ, u, NICK_REQUESTED);
			return MOD_CONT;
		}

		/* Prevent "Guest" nicks from being registered. -TheShadow */

		/* Guest nick can now have a series of between 1 and 7 digits.
		 *   --lara
		 */
		if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1 && stristr(u->nick, NSGuestNickPrefix) == u->nick && strspn(u->nick + prefixlen, "1234567890") == nicklen - prefixlen)
		{
			notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick);
			return MOD_CONT;
		}

		if (!ircdproto->IsNickValid(u->nick))
		{
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, u->nick);
			return MOD_CONT;
		}

		if (RestrictOperNicks)
		{
			for (it = svsopers_in_config.begin(); it != svsopers_in_config.end(); ++it)
			{
				std::string nick = it->first;

				if (stristr(u->nick, nick.c_str()) && !is_oper(u))
				{
					notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick);
					return MOD_CONT;
				}
			}
		}

		if (NSForceEmail && !email)
			this->OnSyntaxError(u);
		else if (time(NULL) < u->lastnickreg + NSRegDelay)
			notice_lang(s_NickServ, u, NICK_REG_PLEASE_WAIT, (u->lastnickreg + NSRegDelay) - time(NULL));
		else if ((na = findnick(u->nick)))
		{
			/* i.e. there's already such a nick regged */
			if (na->HasFlag(NS_FORBIDDEN))
			{
				alog("%s: %s@%s tried to register FORBIDden nick %s", s_NickServ, u->GetIdent().c_str(), u->host, u->nick);
				notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick);
			}
			else
				notice_lang(s_NickServ, u, NICK_ALREADY_REGISTERED, u->nick);
		}
		else if (!stricmp(u->nick, pass) || (StrictPasswords && strlen(pass) < 5))
			notice_lang(s_NickServ, u, MORE_OBSCURE_PASSWORD);
		else if (enc_encrypt_check_len(strlen(pass), PASSMAX - 1))
			notice_lang(s_NickServ, u, PASSWORD_TOO_LONG);
		else if (email && !MailValidate(email))
			notice_lang(s_NickServ, u, MAIL_X_INVALID, email);
		else
		{
			for (idx = 0; idx < 9; ++idx)
				passcode[idx] = chars[1 + static_cast<int>((static_cast<float>(max - min)) * getrandom16() / 65536.0) + min];
			passcode[idx] = '\0';
			nr = makerequest(u->nick);
			nr->passcode = sstrdup(passcode);
			strscpy(nr->password, pass, PASSMAX);
			enc_encrypt_in_place(nr->password, PASSMAX);
			if (email)
				nr->email = sstrdup(email);
			nr->requested = time(NULL);
			FOREACH_MOD(I_OnMakeNickRequest, OnMakeNickRequest(nr));
			if (NSEmailReg)
			{
				if (!do_sendregmail(u, nr))
				{
					notice_lang(s_NickServ, u, NICK_ENTER_REG_CODE, email, s_NickServ);
					alog("%s: sent registration verification code to %s", s_NickServ, nr->email);
				}
				else
				{
					alog("%s: Unable to send registration verification mail", s_NickServ);
					notice_lang(s_NickServ, u, NICK_REG_UNABLE);
					delnickrequest(nr); /* Delete the NickRequest if we couldnt send the mail */
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
		notice_help(s_NickServ, u, NICK_HELP_REGISTER);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		if (NSForceEmail)
			syntax_error(s_NickServ, u, "REGISTER", NICK_REGISTER_SYNTAX_EMAIL);
		else
			syntax_error(s_NickServ, u, "REGISTER", NICK_REGISTER_SYNTAX);
	}
};

class CommandNSResend : public Command
{
 public:
	CommandNSResend() : Command("RESEND", 0, 0)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		NickRequest *nr = NULL;
		if (NSEmailReg)
		{
			if ((nr = findrequestnick(u->nick)))
			{
				if (time(NULL) < nr->lastmail + NSResendDelay)
				{
					notice_lang(s_NickServ, u, MAIL_LATER);
					return MOD_CONT;
				}
				if (!do_sendregmail(u, nr))
				{
					nr->lastmail = time(NULL);
					notice_lang(s_NickServ, u, NICK_REG_RESENT, nr->email);
					alog("%s: re-sent registration verification code for %s to %s", s_NickServ, nr->nick, nr->email);
				}
				else
				{
					alog("%s: Unable to re-send registration verification mail for %s", s_NickServ, nr->nick);
					return MOD_CONT;
				}
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_NickServ, u, NICK_HELP_RESEND);
		return true;
	}
};

class NSRegister : public Module
{
 public:
	NSRegister(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSRegister());
		this->AddCommand(NICKSERV, new CommandNSConfirm("CONFIRM", 0, 1));
		this->AddCommand(NICKSERV, new CommandNSResend());

		ModuleManager::Attach(I_OnNickServHelp, this);
	}
	void OnNickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_REGISTER);
		if (NSEmailReg)
		{
			notice_lang(s_NickServ, u, NICK_HELP_CMD_CONFIRM);
			notice_lang(s_NickServ, u, NICK_HELP_CMD_RESEND);
		}
	}
};

/*************************************************************************/

NickRequest *makerequest(const char *nick)
{
	NickRequest *nr;

	nr = new NickRequest;
	nr->nick = sstrdup(nick);
	insert_requestnick(nr);
	alog("%s: Nick %s has been requested", s_NickServ, nr->nick);
	return nr;
}

/* Creates a full new nick (alias + core) in NickServ database. */
NickAlias *makenick(const char *nick)
{
	NickAlias *na;
	NickCore *nc;

	/* First make the core */
	nc = new NickCore();
	nc->display = sstrdup(nick);
	slist_init(&nc->aliases);
	insert_core(nc);
	alog("%s: group %s has been created", s_NickServ, nc->display);

	/* Then make the alias */
	na = new NickAlias;
	na->nick = sstrdup(nick);
	na->nc = nc;
	slist_add(&nc->aliases, na);
	alpha_insert_alias(na);
	return na;
}

int do_sendregmail(User *u, NickRequest *nr)
{
	MailInfo *mail = NULL;
	char buf[BUFSIZE];

	if (!nr && !u)
		return -1;
	snprintf(buf, sizeof(buf), getstring(NICK_REG_MAIL_SUBJECT), nr->nick);
	mail = MailRegBegin(u, nr, buf, s_NickServ);
	if (!mail)
		return -1;
	fprintf(mail->pipe, "%s", getstring(NICK_REG_MAIL_HEAD));
	fprintf(mail->pipe, "\n\n");
	fprintf(mail->pipe, getstring(NICK_REG_MAIL_LINE_1), nr->nick);
	fprintf(mail->pipe, "\n\n");
	fprintf(mail->pipe, getstring(NICK_REG_MAIL_LINE_2), s_NickServ, nr->passcode);
	fprintf(mail->pipe, "\n\n");
	fprintf(mail->pipe, "%s", getstring(NICK_REG_MAIL_LINE_3));
	fprintf(mail->pipe, "\n\n");
	fprintf(mail->pipe, "%s", getstring(NICK_REG_MAIL_LINE_4));
	fprintf(mail->pipe, "\n\n");
	fprintf(mail->pipe, getstring(NICK_REG_MAIL_LINE_5), NetworkName);
	fprintf(mail->pipe, "\n.\n");
	MailEnd(mail);
	return 0;
}

MODULE_INIT(NSRegister)
