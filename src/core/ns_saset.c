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
 * $Id: ns_set.c 850 2005-08-07 14:52:04Z geniusdex $
 *
 */
/*************************************************************************/

#include "module.h"

class CommandNSSASet : public Command
{
private:
	CommandReturn DoSetDisplay(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;
		int i;
		NickAlias *na;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		/* First check whether param is a valid nick of the group */
		for (i = 0; i < nc->aliases.count; ++i)
		{
			na = static_cast<NickAlias *>(nc->aliases.list[i]);
			if (!stricmp(na->nick, param))
			{
				param = na->nick;   /* Because case may differ */
				break;
			}
		}

		if (i == nc->aliases.count)
		{
			notice_lang(s_NickServ, u, NICK_SASET_DISPLAY_INVALID, nc->display);
			return MOD_CONT;
		}

		change_core_display(nc, param);
		notice_lang(s_NickServ, u, NICK_SASET_DISPLAY_CHANGED, nc->display);
		return MOD_CONT;
	}

	CommandReturn DoSetPassword(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		int len = strlen(param);
		char tmp_pass[PASSMAX];

		if (NSSecureAdmins && u->nc != nc && nc->IsServicesOper())
		{
			notice_lang(s_NickServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}
		else if (!stricmp(nc->display, param) || (StrictPasswords && len < 5))
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
			params[2].clear();
			alog("%s: Failed to encrypt password for %s (set)", s_NickServ, nc->display);
			notice_lang(s_NickServ, u, NICK_SASET_PASSWORD_FAILED, nc->display);
			return MOD_CONT;
		}
		params[2].clear();

		if (enc_decrypt(nc->pass, tmp_pass, PASSMAX - 1) == 1)
			notice_lang(s_NickServ, u, NICK_SASET_PASSWORD_CHANGED_TO, nc->display, tmp_pass);
		else
			notice_lang(s_NickServ, u, NICK_SASET_PASSWORD_CHANGED, nc->display);

		alog("%s: %s!%s@%s used SASET PASSWORD on %s (e-mail: %s)", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, nc->display, nc->email ? nc->email : "none");
		if (WallSetpass)
			ircdproto->SendGlobops(s_NickServ, "\2%s\2 used SASET PASSWORD on \2%s\2", u->nick, nc->display);
		return MOD_CONT;
	}

	CommandReturn DoSetUrl(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (nc->url)
			delete [] nc->url;

		if (param)
		{
			nc->url = sstrdup(param);
			notice_lang(s_NickServ, u, NICK_SASET_URL_CHANGED, nc->display, param);
		}
		else
		{
			nc->url = NULL;
			notice_lang(s_NickServ, u, NICK_SASET_URL_UNSET, nc->display);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetEmail(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (!param && NSForceEmail)
		{
			notice_lang(s_NickServ, u, NICK_SASET_EMAIL_UNSET_IMPOSSIBLE);
			return MOD_CONT;
		}
		else if (NSSecureAdmins && u->nc != nc && nc->IsServicesOper())
		{
			notice_lang(s_NickServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}
		else if (param && !MailValidate(param))
		{
			notice_lang(s_NickServ, u, MAIL_X_INVALID, param);
			return MOD_CONT;
		}

		alog("%s: %s!%s@%s used SASET EMAIL on %s (e-mail: %s)", s_NickServ, u->nick, u->GetIdent().c_str(), u->host, nc->display, nc->email ? nc->email : "none");

		if (nc->email)
			delete [] nc->email;

		if (param)
		{
			nc->email = sstrdup(param);
			notice_lang(s_NickServ, u, NICK_SASET_EMAIL_CHANGED, nc->display, param);
		}
		else
		{
			nc->email = NULL;
			notice_lang(s_NickServ, u, NICK_SASET_EMAIL_UNSET, nc->display);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetICQ(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (param)
		{
			int32 tmp = atol(param);
			if (!tmp)
				notice_lang(s_NickServ, u, NICK_SASET_ICQ_INVALID, param);
			else
			{
				nc->icq = tmp;
				notice_lang(s_NickServ, u, NICK_SASET_ICQ_CHANGED, nc->display,
				            param);
			}
		}
		else
		{
			nc->icq = 0;
			notice_lang(s_NickServ, u, NICK_SASET_ICQ_UNSET, nc->display);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetGreet(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (nc->greet)
			delete [] nc->greet;

		if (param)
		{
			char buf[BUFSIZE];
			const char *rest = params.size() > 3 ? params[3].c_str() : NULL;

			snprintf(buf, sizeof(buf), "%s%s%s", param, rest ? " " : "", rest ? rest : "");

			nc->greet = sstrdup(buf);
			notice_lang(s_NickServ, u, NICK_SASET_GREET_CHANGED, nc->display, buf);
		}
		else
		{
			nc->greet = NULL;
			notice_lang(s_NickServ, u, NICK_SASET_GREET_UNSET, nc->display);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetKill(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!stricmp(param, "ON"))
		{
			nc->flags |= NI_KILLPROTECT;
			nc->flags &= ~(NI_KILL_QUICK | NI_KILL_IMMED);
			notice_lang(s_NickServ, u, NICK_SASET_KILL_ON, nc->display);
		}
		else if (!stricmp(param, "QUICK"))
		{
			nc->flags |= NI_KILLPROTECT | NI_KILL_QUICK;
			nc->flags &= ~NI_KILL_IMMED;
			notice_lang(s_NickServ, u, NICK_SASET_KILL_QUICK, nc->display);
		}
		else if (!stricmp(param, "IMMED"))
		{
			if (NSAllowKillImmed)
			{
				nc->flags |= NI_KILLPROTECT | NI_KILL_IMMED;
				nc->flags &= ~NI_KILL_QUICK;
				notice_lang(s_NickServ, u, NICK_SASET_KILL_IMMED, nc->display);
			}
			else
				notice_lang(s_NickServ, u, NICK_SASET_KILL_IMMED_DISABLED);
		}
		else if (!stricmp(param, "OFF"))
		{
			nc->flags &= ~(NI_KILLPROTECT | NI_KILL_QUICK | NI_KILL_IMMED);
			notice_lang(s_NickServ, u, NICK_SASET_KILL_OFF, nc->display);
		}
		else
			syntax_error(s_NickServ, u, "SASET KILL", NSAllowKillImmed ? NICK_SASET_KILL_IMMED_SYNTAX : NICK_SASET_KILL_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetSecure(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!stricmp(param, "ON"))
		{
			nc->flags |= NI_SECURE;
			notice_lang(s_NickServ, u, NICK_SASET_SECURE_ON, nc->display);
		}
		else if (!stricmp(param, "OFF"))
		{
			nc->flags &= ~NI_SECURE;
			notice_lang(s_NickServ, u, NICK_SASET_SECURE_OFF, nc->display);
		}
		else
			syntax_error(s_NickServ, u, "SASET SECURE", NICK_SASET_SECURE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetPrivate(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!stricmp(param, "ON"))
		{
			nc->flags |= NI_PRIVATE;
			notice_lang(s_NickServ, u, NICK_SASET_PRIVATE_ON, nc->display);
		}
		else if (!stricmp(param, "OFF"))
		{
			nc->flags &= ~NI_PRIVATE;
			notice_lang(s_NickServ, u, NICK_SASET_PRIVATE_OFF, nc->display);
		}
		else
			syntax_error(s_NickServ, u, "SASET PRIVATE", NICK_SASET_PRIVATE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetMsg(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!UsePrivmsg)
		{
			notice_lang(s_NickServ, u, NICK_SASET_OPTION_DISABLED, "MSG");
			return MOD_CONT;
		}

		if (!stricmp(param, "ON"))
		{
			nc->flags |= NI_MSG;
			notice_lang(s_NickServ, u, NICK_SASET_MSG_ON, nc->display);
		}
		else if (!stricmp(param, "OFF"))
		{
			nc->flags &= ~NI_MSG;
			notice_lang(s_NickServ, u, NICK_SASET_MSG_OFF, nc->display);
		}
		else
			syntax_error(s_NickServ, u, "SASET MSG", NICK_SASET_MSG_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetHide(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		int flag, onmsg, offmsg;

		if (!stricmp(param, "EMAIL"))
		{
			flag = NI_HIDE_EMAIL;
			onmsg = NICK_SASET_HIDE_EMAIL_ON;
			offmsg = NICK_SASET_HIDE_EMAIL_OFF;
		}
		else if (!stricmp(param, "USERMASK"))
		{
			flag = NI_HIDE_MASK;
			onmsg = NICK_SASET_HIDE_MASK_ON;
			offmsg = NICK_SASET_HIDE_MASK_OFF;
		}
		else if (!stricmp(param, "STATUS"))
		{
			flag = NI_HIDE_STATUS;
			onmsg = NICK_SASET_HIDE_STATUS_ON;
			offmsg = NICK_SASET_HIDE_STATUS_OFF;
		}
		else if (!stricmp(param, "QUIT"))
		{
			flag = NI_HIDE_QUIT;
			onmsg = NICK_SASET_HIDE_QUIT_ON;
			offmsg = NICK_SASET_HIDE_QUIT_OFF;
		}
		else
		{
			syntax_error(s_NickServ, u, "SASET HIDE", NICK_SASET_HIDE_SYNTAX);
			return MOD_CONT;
		}

		param = params.size() > 3 ? params[3].c_str() : NULL;
		if (!param)
			syntax_error(s_NickServ, u, "SASET HIDE", NICK_SASET_HIDE_SYNTAX);
		else if (!stricmp(param, "ON"))
		{
			nc->flags |= flag;
			notice_lang(s_NickServ, u, onmsg, nc->display, s_NickServ);
		}
		else if (!stricmp(param, "OFF"))
		{
			nc->flags &= ~flag;
			notice_lang(s_NickServ, u, offmsg, nc->display, s_NickServ);
		}
		else
			syntax_error(s_NickServ, u, "SASET HIDE", NICK_SASET_HIDE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetNoExpire(User *u, std::vector<std::string> &params, NickAlias *na)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (!param)
		{
			syntax_error(s_NickServ, u, "SASET NOEXPIRE", NICK_SASET_NOEXPIRE_SYNTAX);
			return MOD_CONT;
		}

		if (!stricmp(param, "ON"))
		{
			na->status |= NS_NO_EXPIRE;
			notice_lang(s_NickServ, u, NICK_SASET_NOEXPIRE_ON, na->nick);
		}
		else if (!stricmp(param, "OFF"))
		{
			na->status &= ~NS_NO_EXPIRE;
			notice_lang(s_NickServ, u, NICK_SASET_NOEXPIRE_OFF, na->nick);
		}
		else
			syntax_error(s_NickServ, u, "SASET NOEXPIRE", NICK_SASET_NOEXPIRE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetAutoOP(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!stricmp(param, "ON"))
		{
			nc->flags &= ~NI_AUTOOP;
			notice_lang(s_NickServ, u, NICK_SASET_AUTOOP_ON, nc->display);
		}
		else if (!stricmp(param, "OFF"))
		{
			nc->flags |= NI_AUTOOP;
			notice_lang(s_NickServ, u, NICK_SASET_AUTOOP_OFF, nc->display);
		}
		else
			syntax_error(s_NickServ, u, "SET AUTOOP", NICK_SASET_AUTOOP_SYNTAX);

		return MOD_CONT;
	}

	CommandReturn DoSetLanguage(User *u, std::vector<std::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		int langnum;

		if (param[strspn(param, "0123456789")]) /* i.e. not a number */
		{
			syntax_error(s_NickServ, u, "SASET LANGUAGE", NICK_SASET_LANGUAGE_SYNTAX);
			return MOD_CONT;
		}
		langnum = atoi(param) - 1;
		if (langnum < 0 || langnum >= NUM_LANGS || langlist[langnum] < 0)
		{
			notice_lang(s_NickServ, u, NICK_SASET_LANGUAGE_UNKNOWN, langnum + 1, s_NickServ);
			return MOD_CONT;
		}
		nc->language = langlist[langnum];
		notice_lang(s_NickServ, u, NICK_SASET_LANGUAGE_CHANGED);

		return MOD_CONT;
	}
public:
	CommandNSSASet() : Command("SASET", 2, 4, "nickserv/saset")
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *nick = params[0].c_str();
		const char *cmd = params[1].c_str();

		NickAlias *na;

		if (readonly)
		{
			notice_lang(s_NickServ, u, NICK_SASET_DISABLED);
			return MOD_CONT;
		}
		if (!(na = findnick(nick)))
		{
			notice_lang(s_NickServ, u, NICK_SASET_BAD_NICK, nick);
			return MOD_CONT;
		}

		if (na->status & NS_FORBIDDEN)
			notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else if (na->nc->flags & NI_SUSPENDED)
			notice_lang(s_NickServ, u, NICK_X_SUSPENDED, na->nick);
		else if (!stricmp(cmd, "DISPLAY"))
			return this->DoSetDisplay(u, params, na->nc);
		else if (!stricmp(cmd, "PASSWORD"))
			return this->DoSetPassword(u, params, na->nc);
		else if (!stricmp(cmd, "URL"))
			return this->DoSetUrl(u, params, na->nc);
		else if (!stricmp(cmd, "EMAIL"))
			return this->DoSetEmail(u, params, na->nc);
		else if (!stricmp(cmd, "ICQ"))
			return this->DoSetICQ(u, params, na->nc);
		else if (!stricmp(cmd, "GREET"))
			return this->DoSetGreet(u, params, na->nc);
		else if (!stricmp(cmd, "KILL"))
			return this->DoSetKill(u, params, na->nc);
		else if (!stricmp(cmd, "SECURE"))
			return this->DoSetSecure(u, params, na->nc);
		else if (!stricmp(cmd, "PRIVATE"))
			return this->DoSetPrivate(u, params, na->nc);
		else if (!stricmp(cmd, "MSG"))
			return this->DoSetMsg(u, params, na->nc);
		else if (!stricmp(cmd, "HIDE"))
			return this->DoSetHide(u, params, na->nc);
		else if (!stricmp(cmd, "NOEXPIRE"))
			return this->DoSetNoExpire(u, params, na);
		else if (!stricmp(cmd, "AUTOOP"))
			return this->DoSetAutoOP(u, params, na->nc);
		else if (!stricmp(cmd, "LANGUAGE"))
			return this->DoSetLanguage(u, params, na->nc);
		else
			notice_lang(s_NickServ, u, NICK_SASET_UNKNOWN_OPTION, cmd);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (subcommand.empty())
			notice_help(s_NickServ, u, NICK_HELP_SASET);
		else if (subcommand == "DISPLAY")
			notice_help(s_NickServ, u, NICK_HELP_SASET_DISPLAY);
		else if (subcommand == "PASSWORD")
			notice_help(s_NickServ, u, NICK_HELP_SASET_PASSWORD);
		else if (subcommand == "URL")
			notice_help(s_NickServ, u, NICK_HELP_SASET_URL);
		else if (subcommand == "EMAIL")
			notice_help(s_NickServ, u, NICK_HELP_SASET_EMAIL);
		else if (subcommand == "ICQ")
			notice_help(s_NickServ, u, NICK_HELP_SASET_ICQ);
		else if (subcommand == "GREET")
			notice_help(s_NickServ, u, NICK_HELP_SASET_GREET);
		else if (subcommand == "KILL")
			notice_help(s_NickServ, u, NICK_HELP_SASET_KILL);
		else if (subcommand == "SECURE")
			notice_help(s_NickServ, u, NICK_HELP_SASET_SECURE);
		else if (subcommand == "PRIVATE")
			notice_help(s_NickServ, u, NICK_HELP_SASET_PRIVATE);
		else if (subcommand == "MSG")
			notice_help(s_NickServ, u, NICK_HELP_SASET_MSG);
		else if (subcommand == "HIDE")
			notice_help(s_NickServ, u, NICK_HELP_SASET_HIDE);
		else if (subcommand == "NOEXPIRE")
			notice_help(s_NickServ, u, NICK_HELP_SASET_NOEXPIRE);
		else if (subcommand == "AUTOOP")
			notice_help(s_NickServ, u, NICK_HELP_SASET_AUTOOP);
		else if (subcommand == "LANGUAGE")
			notice_help(s_NickServ, u, NICK_HELP_SASET_LANGUAGE);
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_NickServ, u, "SASET", NICK_SASET_SYNTAX);
	}
};

class NSSASet : public Module
{
public:
	NSSASet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id: ns_set.c 850 2005-08-07 14:52:04Z geniusdex $");
		this->SetType(CORE);

		this->AddCommand(NICKSERV, new CommandNSSASet(), MOD_UNIQUE);
	}
	void NickServHelp(User *u)
	{
		notice_lang(s_NickServ, u, NICK_HELP_CMD_SASET);
	}
};

MODULE_INIT("ns_saset", NSSASet)
