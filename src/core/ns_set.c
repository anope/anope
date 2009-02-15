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
#include "encrypt.h"

void myNickServHelp(User *u);

class CommandNSSet : public Command
{
 private:
	CommandReturn DoSetDisplay(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		int i;
		NickAlias *na;

		/* First check whether param is a valid nick of the group */
		for (i = 0; i < nc->aliases.count; ++i)
		{
			na = static_cast<NickAlias *>(nc->aliases.list[i]);
			if (!stricmp(na->nick, param))
			{
				param = na->nick; /* Because case may differ */
				break;
			}
		}

		if (i == nc->aliases.count)
		{
			notice_lang(s_NickServ, u, NICK_SET_DISPLAY_INVALID);
			return MOD_CONT;
		}

		change_core_display(nc, param);
		notice_lang(s_NickServ, u, NICK_SET_DISPLAY_CHANGED, nc->display);
		return MOD_CONT;
	}

	CommandReturn DoSetPassword(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		int len = strlen(param);
		char tmp_pass[PASSMAX];

		if (!stricmp(nc->display, param) || (StrictPasswords && len < 5))
		{
			notice_lang(s_NickServ, u, MORE_OBSCURE_PASSWORD);
			return MOD_CONT;
		}
		else if (enc_encrypt_check_len(len, PASSMAX - 1))
		{
			notice_lang(s_NickServ, u, PASSWORD_TOO_LONG);
			return MOD_CONT;
		}

		if (enc_encrypt(param, len, nc->pass, PASSMAX - 1) < 0)
		{
			params[1].clear();
			alog("%s: Failed to encrypt password for %s (set)", s_NickServ, nc->display);
			notice_lang(s_NickServ, u, NICK_SET_PASSWORD_FAILED);
			return MOD_CONT;
		}
		params[1].clear();

		if (enc_decrypt(nc->pass, tmp_pass, PASSMAX - 1) == 1)
			notice_lang(s_NickServ, u, NICK_SET_PASSWORD_CHANGED_TO, tmp_pass);
		else
			notice_lang(s_NickServ, u, NICK_SET_PASSWORD_CHANGED);

		alog("%s: %s!%s@%s (e-mail: %s) changed its password.", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, nc->email ? nc->email : "none");

		return MOD_CONT;
	}

	CommandReturn DoSetLanguage(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		int langnum;

		if (param[strspn(param, "0123456789")]) /* i.e. not a number */
		{
			syntax_error(s_NickServ, u, "SET LANGUAGE", NICK_SET_LANGUAGE_SYNTAX);
			return MOD_CONT;
		}
		langnum = atoi(param) - 1;
		if (langnum < 0 || langnum >= NUM_LANGS || langlist[langnum] < 0)
		{
			notice_lang(s_NickServ, u, NICK_SET_LANGUAGE_UNKNOWN, langnum + 1, s_NickServ);
			return MOD_CONT;
		}
		nc->language = langlist[langnum];
		notice_lang(s_NickServ, u, NICK_SET_LANGUAGE_CHANGED);
		return MOD_CONT;
	}

	CommandReturn DoSetUrl(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (nc->url)
			delete [] nc->url;

		if (param)
		{
			nc->url = sstrdup(param);
			notice_lang(s_NickServ, u, NICK_SET_URL_CHANGED, param);
		}
		else
		{
			nc->url = NULL;
			notice_lang(s_NickServ, u, NICK_SET_URL_UNSET);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetEmail(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param && NSForceEmail)
		{
			notice_lang(s_NickServ, u, NICK_SET_EMAIL_UNSET_IMPOSSIBLE);
			return MOD_CONT;
		}
		else if (param && !MailValidate(param))
		{
			notice_lang(s_NickServ, u, MAIL_X_INVALID, param);
			return MOD_CONT;
		}

		alog("%s: %s!%s@%s (e-mail: %s) changed its e-mail to %s.", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, nc->email ? nc->email : "none", param ? param : "none");

		if (nc->email)
			delete [] nc->email;

		if (param)
		{
			nc->email = sstrdup(param);
			notice_lang(s_NickServ, u, NICK_SET_EMAIL_CHANGED, param);
		}
		else
		{
			nc->email = NULL;
			notice_lang(s_NickServ, u, NICK_SET_EMAIL_UNSET);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetICQ(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (param)
		{
			int32 tmp = atol(param);
			if (!tmp)
				notice_lang(s_NickServ, u, NICK_SET_ICQ_INVALID, param);
			else
			{
				nc->icq = tmp;
				notice_lang(s_NickServ, u, NICK_SET_ICQ_CHANGED, param);
			}
		}
		else
		{
			nc->icq = 0;
			notice_lang(s_NickServ, u, NICK_SET_ICQ_UNSET);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetGreet(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (nc->greet)
			delete [] nc->greet;

		if (param)
		{
			char buf[BUFSIZE];
			const char *rest = params.size() > 1 ? params[2].c_str() : NULL;

			snprintf(buf, sizeof(buf), "%s%s%s", param, rest ? " " : "", rest ? rest : "");

			nc->greet = sstrdup(buf);
			notice_lang(s_NickServ, u, NICK_SET_GREET_CHANGED, buf);
		}
		else
		{
			nc->greet = NULL;
			notice_lang(s_NickServ, u, NICK_SET_GREET_UNSET);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetKill(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!stricmp(param, "ON"))
		{
			nc->flags |= NI_KILLPROTECT;
			nc->flags &= ~(NI_KILL_QUICK | NI_KILL_IMMED);
			notice_lang(s_NickServ, u, NICK_SET_KILL_ON);
		}
		else if (!stricmp(param, "QUICK"))
		{
			nc->flags |= NI_KILLPROTECT | NI_KILL_QUICK;
			nc->flags &= ~NI_KILL_IMMED;
			notice_lang(s_NickServ, u, NICK_SET_KILL_QUICK);
		}
		else if (!stricmp(param, "IMMED"))
		{
			if (NSAllowKillImmed)
			{
				nc->flags |= NI_KILLPROTECT | NI_KILL_IMMED;
				nc->flags &= ~NI_KILL_QUICK;
				notice_lang(s_NickServ, u, NICK_SET_KILL_IMMED);
			}
			else
				notice_lang(s_NickServ, u, NICK_SET_KILL_IMMED_DISABLED);
		}
		else if (!stricmp(param, "OFF"))
		{
			nc->flags &= ~(NI_KILLPROTECT | NI_KILL_QUICK | NI_KILL_IMMED);
			notice_lang(s_NickServ, u, NICK_SET_KILL_OFF);
		}
		else
			syntax_error(s_NickServ, u, "SET KILL", NSAllowKillImmed ? NICK_SET_KILL_IMMED_SYNTAX : NICK_SET_KILL_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetSecure(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!stricmp(param, "ON"))
		{
			nc->flags |= NI_SECURE;
			notice_lang(s_NickServ, u, NICK_SET_SECURE_ON);
		}
		else if (!stricmp(param, "OFF"))
		{
			nc->flags &= ~NI_SECURE;
			notice_lang(s_NickServ, u, NICK_SET_SECURE_OFF);
		}
		else
			syntax_error(s_NickServ, u, "SET SECURE", NICK_SET_SECURE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetPrivate(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!stricmp(param, "ON"))
		{
			nc->flags |= NI_PRIVATE;
			notice_lang(s_NickServ, u, NICK_SET_PRIVATE_ON);
		}
		else if (!stricmp(param, "OFF"))
		{
			nc->flags &= ~NI_PRIVATE;
			notice_lang(s_NickServ, u, NICK_SET_PRIVATE_OFF);
		}
		else
			syntax_error(s_NickServ, u, "SET PRIVATE", NICK_SET_PRIVATE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetMsg(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!UsePrivmsg)
		{
			notice_lang(s_NickServ, u, NICK_SET_OPTION_DISABLED, "MSG");
			return MOD_CONT;
		}

		if (!stricmp(param, "ON"))
		{
			nc->flags |= NI_MSG;
			notice_lang(s_NickServ, u, NICK_SET_MSG_ON);
		}
		else if (!stricmp(param, "OFF"))
		{
			nc->flags &= ~NI_MSG;
			notice_lang(s_NickServ, u, NICK_SET_MSG_OFF);
		}
		else
			syntax_error(s_NickServ, u, "SET MSG", NICK_SET_MSG_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetHide(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		int flag, onmsg, offmsg;

		if (!stricmp(param, "EMAIL"))
		{
			flag = NI_HIDE_EMAIL;
			onmsg = NICK_SET_HIDE_EMAIL_ON;
			offmsg = NICK_SET_HIDE_EMAIL_OFF;
		}
		else if (!stricmp(param, "USERMASK"))
		{
			flag = NI_HIDE_MASK;
			onmsg = NICK_SET_HIDE_MASK_ON;
			offmsg = NICK_SET_HIDE_MASK_OFF;
		}
		else if (!stricmp(param, "STATUS"))
		{
			flag = NI_HIDE_STATUS;
			onmsg = NICK_SET_HIDE_STATUS_ON;
			offmsg = NICK_SET_HIDE_STATUS_OFF;
		}
		else if (!stricmp(param, "QUIT"))
		{
			flag = NI_HIDE_QUIT;
			onmsg = NICK_SET_HIDE_QUIT_ON;
			offmsg = NICK_SET_HIDE_QUIT_OFF;
		}
		else
		{
			syntax_error(s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
			return MOD_CONT;
		}

		param = params.size() > 2 ? params[2].c_str() : NULL;
		if (!param)
			syntax_error(s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
		else if (!stricmp(param, "ON"))
		{
			nc->flags |= flag;
			notice_lang(s_NickServ, u, onmsg, s_NickServ);
		}
		else if (!stricmp(param, "OFF"))
		{
			nc->flags &= ~flag;
			notice_lang(s_NickServ, u, offmsg, s_NickServ);
		}
		else
			syntax_error(s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetAutoOP(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		/**
		 * This works the other way around, the absence of this flag denotes ON
		 * This is so when people upgrade, and dont have the flag
		 * the default is on
		 **/
		if (!stricmp(param, "ON"))
		{
			nc->flags &= ~NI_AUTOOP;
			notice_lang(s_NickServ, u, NICK_SET_AUTOOP_ON);
		}
		else if (!stricmp(param, "OFF"))
		{
			nc->flags |= NI_AUTOOP;
			notice_lang(s_NickServ, u, NICK_SET_AUTOOP_OFF);
		}
		else
			syntax_error(s_NickServ, u, "SET AUTOOP", NICK_SET_AUTOOP_SYNTAX);

		return MOD_CONT;
	}
 public:
	CommandNSSet() : Command("SET", 1, 3)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *cmd = params[0].c_str();

		if (readonly)
		{
			notice_lang(s_NickServ, u, NICK_SET_DISABLED);
			return MOD_CONT;
		}

/*
		if (na->status & NS_FORBIDDEN)
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
*/
		if (u->nc->flags & NI_SUSPENDED)
			notice_lang(s_NickServ, u, NICK_X_SUSPENDED, u->nc->display);
		else if (!nick_identified(u))
			notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
		else if (!stricmp(cmd, "DISPLAY"))
			return this->DoSetDisplay(u, params, u->nc);
		else if (!stricmp(cmd, "PASSWORD"))
			return this->DoSetPassword(u, params, u->nc);
		else if (!stricmp(cmd, "LANGUAGE"))
			return this->DoSetLanguage(u, params, u->nc);
		else if (!stricmp(cmd, "URL"))
			return this->DoSetUrl(u, params, u->nc);
		else if (!stricmp(cmd, "EMAIL"))
			return this->DoSetEmail(u, params, u->nc);
		else if (!stricmp(cmd, "ICQ"))
			return this->DoSetICQ(u, params, u->nc);
		else if (!stricmp(cmd, "GREET"))
			return this->DoSetGreet(u, params, u->nc);
		else if (!stricmp(cmd, "KILL"))
			return this->DoSetKill(u, params, u->nc);
		else if (!stricmp(cmd, "SECURE"))
			return this->DoSetSecure(u, params, u->nc);
		else if (!stricmp(cmd, "PRIVATE"))
			return this->DoSetPrivate(u, params, u->nc);
		else if (!stricmp(cmd, "MSG"))
			return this->DoSetMsg(u, params, u->nc);
		else if (!stricmp(cmd, "HIDE"))
			return this->DoSetHide(u, params, u->nc);
		else if (!stricmp(cmd, "AUTOOP"))
			return this->DoSetAutoOP(u, params, u->nc);
		else
			notice_lang(s_NickServ, u, NICK_SET_UNKNOWN_OPTION, cmd);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (subcommand == "DISPLAY")
			notice_help(s_NickServ, u, NICK_HELP_SET_DISPLAY);
		else if (subcommand == "PASSWORD")
			notice_help(s_NickServ, u, NICK_HELP_SET_PASSWORD);
		else if (subcommand == "URL")
			notice_help(s_NickServ, u, NICK_HELP_SET_URL);
		else if (subcommand == "EMAIL")
			notice_help(s_NickServ, u, NICK_HELP_SET_EMAIL);
		else if (subcommand == "ICQ")
			notice_help(s_NickServ, u, NICK_HELP_SET_ICQ);
		else if (subcommand == "GREET")
			notice_help(s_NickServ, u, NICK_HELP_SET_GREET);
		else if (subcommand == "KILL")
			notice_help(s_NickServ, u, NICK_HELP_SET_KILL);
		else if (subcommand == "SECURE")
			notice_help(s_NickServ, u, NICK_HELP_SET_SECURE);
		else if (subcommand == "PRIVATE")
			notice_help(s_NickServ, u, NICK_HELP_SET_PRIVATE);
		else if (subcommand == "MSG")
			notice_help(s_NickServ, u, NICK_HELP_SET_MSG);
		else if (subcommand == "HIDE")
			notice_help(s_NickServ, u, NICK_HELP_SET_HIDE);
		else if (subcommand == "AUTOOP")
			notice_help(s_NickServ, u, NICK_HELP_SET_AUTOOP);
		else
			notice_help(s_NickServ, u, NICK_HELP_SET);

		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_NickServ, u, "SET", NICK_SET_SYNTAX);
	}
};

class NSSet : public Module
{
 public:
	NSSet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSSet(), MOD_UNIQUE);

		this->SetNickHelp(myNickServHelp);
	}
};

/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User *u)
{
	notice_lang(s_NickServ, u, NICK_HELP_CMD_SET);
}

MODULE_INIT("ns_set", NSSet)
