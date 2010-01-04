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
	CommandReturn DoSetDisplay(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 2 ? params[2] : "";
		int i;
		NickAlias *na;

		if (param.empty())
		{
			this->OnSyntaxError(u, "DISPLAY");
			return MOD_CONT;
		}

		/* First check whether param is a valid nick of the group */
		for (i = 0; i < nc->aliases.count; ++i)
		{
			na = static_cast<NickAlias *>(nc->aliases.list[i]);
			if (na->nick == param)
			{
				param = na->nick;   /* Because case may differ */
				break;
			}
		}

		if (i == nc->aliases.count)
		{
			notice_lang(Config.s_NickServ, u, NICK_SASET_DISPLAY_INVALID, nc->display);
			return MOD_CONT;
		}

		change_core_display(nc, param.c_str());
		notice_lang(Config.s_NickServ, u, NICK_SASET_DISPLAY_CHANGED, nc->display);
		return MOD_CONT;
	}

	CommandReturn DoSetPassword(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 2 ? params[2] : "";

		if (param.empty())
		{
			this->OnSyntaxError(u, "PASSWORD");
			return MOD_CONT;
		}

		int len = param.size();
		char tmp_pass[PASSMAX];

		if (Config.NSSecureAdmins && u->nc != nc && nc->IsServicesOper())
		{
			notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}
		else if (nc->display == param || (Config.StrictPasswords && len < 5))
		{
			notice_lang(Config.s_NickServ, u, MORE_OBSCURE_PASSWORD);
			return MOD_CONT;
		}
		else if (enc_encrypt_check_len(len, PASSMAX - 1))
		{
			notice_lang(Config.s_NickServ, u, PASSWORD_TOO_LONG);
			return MOD_CONT;
		}

		if (enc_encrypt(param.c_str(), len, nc->pass, PASSMAX - 1) < 0)
		{
			alog("%s: Failed to encrypt password for %s (set)", Config.s_NickServ, nc->display);
			notice_lang(Config.s_NickServ, u, NICK_SASET_PASSWORD_FAILED, nc->display);
			return MOD_CONT;
		}

		if (enc_decrypt(nc->pass, tmp_pass, PASSMAX - 1) == 1)
			notice_lang(Config.s_NickServ, u, NICK_SASET_PASSWORD_CHANGED_TO, nc->display, tmp_pass);
		else
			notice_lang(Config.s_NickServ, u, NICK_SASET_PASSWORD_CHANGED, nc->display);

		alog("%s: %s!%s@%s used SASET PASSWORD on %s (e-mail: %s)", Config.s_NickServ, u->nick.c_str(), u->GetIdent().c_str(), u->host, nc->display, nc->email ? nc->email : "none");
		if (Config.WallSetpass)
			ircdproto->SendGlobops(findbot(Config.s_NickServ), "\2%s\2 used SASET PASSWORD on \2%s\2", u->nick.c_str(), nc->display);
		return MOD_CONT;
	}

	CommandReturn DoSetUrl(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (nc->url)
			delete [] nc->url;

		if (param)
		{
			nc->url = sstrdup(param);
			notice_lang(Config.s_NickServ, u, NICK_SASET_URL_CHANGED, nc->display, param);
		}
		else
		{
			nc->url = NULL;
			notice_lang(Config.s_NickServ, u, NICK_SASET_URL_UNSET, nc->display);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetEmail(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (!param && Config.NSForceEmail)
		{
			notice_lang(Config.s_NickServ, u, NICK_SASET_EMAIL_UNSET_IMPOSSIBLE);
			return MOD_CONT;
		}
		else if (Config.NSSecureAdmins && u->nc != nc && nc->IsServicesOper())
		{
			notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}
		else if (param && !MailValidate(param))
		{
			notice_lang(Config.s_NickServ, u, MAIL_X_INVALID, param);
			return MOD_CONT;
		}

		alog("%s: %s!%s@%s used SASET EMAIL on %s (e-mail: %s)", Config.s_NickServ, u->nick.c_str(), u->GetIdent().c_str(), u->host, nc->display, nc->email ? nc->email : "none");

		if (nc->email)
			delete [] nc->email;

		if (param)
		{
			nc->email = sstrdup(param);
			notice_lang(Config.s_NickServ, u, NICK_SASET_EMAIL_CHANGED, nc->display, param);
		}
		else
		{
			nc->email = NULL;
			notice_lang(Config.s_NickServ, u, NICK_SASET_EMAIL_UNSET, nc->display);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetICQ(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (param)
		{
			int32 tmp = atol(param);
			if (!tmp)
				notice_lang(Config.s_NickServ, u, NICK_SASET_ICQ_INVALID, param);
			else
			{
				nc->icq = tmp;
				notice_lang(Config.s_NickServ, u, NICK_SASET_ICQ_CHANGED, nc->display, param);
			}
		}
		else
		{
			nc->icq = 0;
			notice_lang(Config.s_NickServ, u, NICK_SASET_ICQ_UNSET, nc->display);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetGreet(User *u, const std::vector<ci::string> &params, NickCore *nc)
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
			notice_lang(Config.s_NickServ, u, NICK_SASET_GREET_CHANGED, nc->display, buf);
		}
		else
		{
			nc->greet = NULL;
			notice_lang(Config.s_NickServ, u, NICK_SASET_GREET_UNSET, nc->display);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetKill(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 2 ? params[2] : "";

		if (param.empty())
		{
			this->OnSyntaxError(u, "KILL");
			return MOD_CONT;
		}

		if (param == "ON")
		{
			nc->SetFlag(NI_KILLPROTECT);
			nc->UnsetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			notice_lang(Config.s_NickServ, u, NICK_SASET_KILL_ON, nc->display);
		}
		else if (param == "QUICK")
		{
			nc->SetFlag(NI_KILLPROTECT);
			nc->SetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			notice_lang(Config.s_NickServ, u, NICK_SASET_KILL_QUICK, nc->display);
		}
		else if (param == "IMMED")
		{
			if (Config.NSAllowKillImmed)
			{
				nc->SetFlag(NI_KILLPROTECT);
				nc->SetFlag(NI_KILL_IMMED);
				nc->UnsetFlag(NI_KILL_QUICK);
				notice_lang(Config.s_NickServ, u, NICK_SASET_KILL_IMMED, nc->display);
			}
			else
				notice_lang(Config.s_NickServ, u, NICK_SASET_KILL_IMMED_DISABLED);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(NI_KILLPROTECT);
			nc->UnsetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			notice_lang(Config.s_NickServ, u, NICK_SASET_KILL_OFF, nc->display);
		}
		else
			syntax_error(Config.s_NickServ, u, "SASET KILL", Config.NSAllowKillImmed ? NICK_SASET_KILL_IMMED_SYNTAX : NICK_SASET_KILL_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetSecure(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 2 ? params[2] : "";

		if (param.empty())
		{
			this->OnSyntaxError(u, "SECURE");
			return MOD_CONT;
		}

		if (param == "ON")
		{
			nc->SetFlag(NI_SECURE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_SECURE_ON, nc->display);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(NI_SECURE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_SECURE_OFF, nc->display);
		}
		else
			syntax_error(Config.s_NickServ, u, "SASET SECURE", NICK_SASET_SECURE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetPrivate(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 2 ? params[2] : "";

		if (param.empty())
		{
			this->OnSyntaxError(u, "PRIVATE");
			return MOD_CONT;
		}

		if (param == "ON")
		{
			nc->SetFlag(NI_PRIVATE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_PRIVATE_ON, nc->display);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(NI_PRIVATE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_PRIVATE_OFF, nc->display);
		}
		else
			syntax_error(Config.s_NickServ, u, "SASET PRIVATE", NICK_SASET_PRIVATE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetMsg(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 2 ? params[2] : "";

		if (param.empty())
		{
			this->OnSyntaxError(u, "MSG");
			return MOD_CONT;
		}

		if (!Config.UsePrivmsg)
		{
			notice_lang(Config.s_NickServ, u, NICK_SASET_OPTION_DISABLED, "MSG");
			return MOD_CONT;
		}

		if (param == "ON")
		{
			nc->SetFlag(NI_MSG);
			notice_lang(Config.s_NickServ, u, NICK_SASET_MSG_ON, nc->display);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(NI_MSG);
			notice_lang(Config.s_NickServ, u, NICK_SASET_MSG_OFF, nc->display);
		}
		else
			syntax_error(Config.s_NickServ, u, "SASET MSG", NICK_SASET_MSG_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetHide(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 2 ? params[2] : "";

		if (param.empty())
		{
			this->OnSyntaxError(u, "HIDE");
			return MOD_CONT;
		}

		int onmsg, offmsg;
		NickCoreFlag flag;

		if (param == "EMAIL")
		{
			flag = NI_HIDE_EMAIL;
			onmsg = NICK_SASET_HIDE_EMAIL_ON;
			offmsg = NICK_SASET_HIDE_EMAIL_OFF;
		}
		else if (param == "USERMASK")
		{
			flag = NI_HIDE_MASK;
			onmsg = NICK_SASET_HIDE_MASK_ON;
			offmsg = NICK_SASET_HIDE_MASK_OFF;
		}
		else if (param == "STATUS")
		{
			flag = NI_HIDE_STATUS;
			onmsg = NICK_SASET_HIDE_STATUS_ON;
			offmsg = NICK_SASET_HIDE_STATUS_OFF;
		}
		else if (param == "QUIT")
		{
			flag = NI_HIDE_QUIT;
			onmsg = NICK_SASET_HIDE_QUIT_ON;
			offmsg = NICK_SASET_HIDE_QUIT_OFF;
		}
		else
		{
			syntax_error(Config.s_NickServ, u, "SASET HIDE", NICK_SASET_HIDE_SYNTAX);
			return MOD_CONT;
		}

		param = params.size() > 3 ? params[3] : "";
		if (param.empty())
			syntax_error(Config.s_NickServ, u, "SASET HIDE", NICK_SASET_HIDE_SYNTAX);
		else if (param == "ON")
		{
			nc->SetFlag(flag);
			notice_lang(Config.s_NickServ, u, onmsg, nc->display, Config.s_NickServ);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(flag);
			notice_lang(Config.s_NickServ, u, offmsg, nc->display, Config.s_NickServ);
		}
		else
			syntax_error(Config.s_NickServ, u, "SASET HIDE", NICK_SASET_HIDE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetNoExpire(User *u, const std::vector<ci::string> &params, NickAlias *na)
	{
		ci::string param = params.size() > 2 ? params[2] : "";

		if (param.empty())
		{
			syntax_error(Config.s_NickServ, u, "SASET NOEXPIRE", NICK_SASET_NOEXPIRE_SYNTAX);
			return MOD_CONT;
		}

		if (param == "ON")
		{
			na->SetFlag(NS_NO_EXPIRE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_NOEXPIRE_ON, na->nick);
		}
		else if (param == "OFF")
		{
			na->UnsetFlag(NS_NO_EXPIRE);
			notice_lang(Config.s_NickServ, u, NICK_SASET_NOEXPIRE_OFF, na->nick);
		}
		else
			syntax_error(Config.s_NickServ, u, "SASET NOEXPIRE", NICK_SASET_NOEXPIRE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetAutoOP(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 2 ? params[2] : "";

		if (param.empty())
		{
			this->OnSyntaxError(u, "AUTOOP");
			return MOD_CONT;
		}

		if (param == "ON")
		{
			nc->UnsetFlag(NI_AUTOOP);
			notice_lang(Config.s_NickServ, u, NICK_SASET_AUTOOP_ON, nc->display);
		}
		else if (param == "OFF")
		{
			nc->SetFlag(NI_AUTOOP);
			notice_lang(Config.s_NickServ, u, NICK_SASET_AUTOOP_OFF, nc->display);
		}
		else
			syntax_error(Config.s_NickServ, u, "SET AUTOOP", NICK_SASET_AUTOOP_SYNTAX);

		return MOD_CONT;
	}

	CommandReturn DoSetLanguage(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u, "LANGUAGE");
			return MOD_CONT;
		}

		int langnum;

		if (param[strspn(param, "0123456789")]) /* i.e. not a number */
		{
			syntax_error(Config.s_NickServ, u, "SASET LANGUAGE", NICK_SASET_LANGUAGE_SYNTAX);
			return MOD_CONT;
		}
		langnum = atoi(param) - 1;
		if (langnum < 0 || langnum >= NUM_LANGS || langlist[langnum] < 0)
		{
			notice_lang(Config.s_NickServ, u, NICK_SASET_LANGUAGE_UNKNOWN, langnum + 1, Config.s_NickServ);
			return MOD_CONT;
		}
		nc->language = langlist[langnum];
		notice_lang(Config.s_NickServ, u, NICK_SASET_LANGUAGE_CHANGED);

		return MOD_CONT;
	}
public:
	CommandNSSASet() : Command("SASET", 2, 4, "nickserv/saset")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params[0].c_str();
		ci::string cmd = params[1];

		NickAlias *na;

		if (readonly)
		{
			notice_lang(Config.s_NickServ, u, NICK_SASET_DISABLED);
			return MOD_CONT;
		}
		if (!(na = findnick(nick)))
		{
			notice_lang(Config.s_NickServ, u, NICK_SASET_BAD_NICK, nick);
			return MOD_CONT;
		}

		if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
		else if (na->nc->HasFlag(NI_SUSPENDED))
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, na->nick);
		else if (cmd == "DISPLAY")
			return this->DoSetDisplay(u, params, na->nc);
		else if (cmd == "PASSWORD")
			return this->DoSetPassword(u, params, na->nc);
		else if (cmd == "URL")
			return this->DoSetUrl(u, params, na->nc);
		else if (cmd == "EMAIL")
			return this->DoSetEmail(u, params, na->nc);
		else if (cmd == "ICQ")
			return this->DoSetICQ(u, params, na->nc);
		else if (cmd == "GREET")
			return this->DoSetGreet(u, params, na->nc);
		else if (cmd == "KILL")
			return this->DoSetKill(u, params, na->nc);
		else if (cmd == "SECURE")
			return this->DoSetSecure(u, params, na->nc);
		else if (cmd == "PRIVATE")
			return this->DoSetPrivate(u, params, na->nc);
		else if (cmd == "MSG")
			return this->DoSetMsg(u, params, na->nc);
		else if (cmd == "HIDE")
			return this->DoSetHide(u, params, na->nc);
		else if (cmd == "NOEXPIRE")
			return this->DoSetNoExpire(u, params, na);
		else if (cmd == "AUTOOP")
			return this->DoSetAutoOP(u, params, na->nc);
		else if (cmd == "LANGUAGE")
			return this->DoSetLanguage(u, params, na->nc);
		else
			notice_lang(Config.s_NickServ, u, NICK_SASET_UNKNOWN_OPTION, cmd.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (subcommand.empty())
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET);
		else if (subcommand == "DISPLAY")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_DISPLAY);
		else if (subcommand == "PASSWORD")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_PASSWORD);
		else if (subcommand == "URL")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_URL);
		else if (subcommand == "EMAIL")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_EMAIL);
		else if (subcommand == "ICQ")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_ICQ);
		else if (subcommand == "GREET")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_GREET);
		else if (subcommand == "KILL")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_KILL);
		else if (subcommand == "SECURE")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_SECURE);
		else if (subcommand == "PRIVATE")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_PRIVATE);
		else if (subcommand == "MSG")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_MSG);
		else if (subcommand == "HIDE")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_HIDE);
		else if (subcommand == "NOEXPIRE")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_NOEXPIRE);
		else if (subcommand == "AUTOOP")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_AUTOOP);
		else if (subcommand == "LANGUAGE")
			notice_help(Config.s_NickServ, u, NICK_HELP_SASET_LANGUAGE);
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "SASET", NICK_SASET_SYNTAX);
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

		this->AddCommand(NICKSERV, new CommandNSSASet());

		ModuleManager::Attach(I_OnNickServHelp, this);
	}
	void OnNickServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET);
	}
};

MODULE_INIT(NSSASet)
