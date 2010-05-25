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

class CommandNSSet : public Command
{
 private:
	CommandReturn DoSetDisplay(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 1 ? params[1] : "";

		if (param.empty())
		{
			this->OnSyntaxError(u, "DISPLAY");
			return MOD_CONT;
		}

		NickAlias *na = findnick(param);
		
		if (!na || na->nc != nc)
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_DISPLAY_INVALID);
			return MOD_CONT;
		}

		change_core_display(nc, param.c_str());
		notice_lang(Config.s_NickServ, u, NICK_SET_DISPLAY_CHANGED, nc->display);
		return MOD_CONT;
	}

	CommandReturn DoSetPassword(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 1 ? params[1] : "";
		std::string buf, tmp_pass;

		if (param.empty())
		{
			this->OnSyntaxError(u, "PASSWORD");
			return MOD_CONT;
		}

		int len = param.size();

		if (nc->display == param || (Config.StrictPasswords && len < 5))
		{
			notice_lang(Config.s_NickServ, u, MORE_OBSCURE_PASSWORD);
			return MOD_CONT;
		}
		else if (len > Config.PassLen)
		{
			notice_lang(Config.s_NickServ, u, PASSWORD_TOO_LONG);
			return MOD_CONT;
		}

		buf = param.c_str(); /* conversion from ci::string to std::string */
		if (enc_encrypt(buf, nc->pass) < 0)
		{
			Alog() << Config.s_NickServ << ": Failed to encrypt password for " << nc->display << " (set)";
			notice_lang(Config.s_NickServ, u, NICK_SET_PASSWORD_FAILED);
			return MOD_CONT;
		}

		if (enc_decrypt(nc->pass, tmp_pass) == 1)
			notice_lang(Config.s_NickServ, u, NICK_SET_PASSWORD_CHANGED_TO, tmp_pass.c_str());
		else
			notice_lang(Config.s_NickServ, u, NICK_SET_PASSWORD_CHANGED);

		Alog() << Config.s_NickServ << ": " << u->GetMask() << " (e-mail: " << (nc->email ? nc->email : "none") << ") changed its password.";

		return MOD_CONT;
	}

	CommandReturn DoSetLanguage(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param)
		{
			this->OnSyntaxError(u, "LANGUAGE");
			return MOD_CONT;
		}

		int langnum;

		if (param[strspn(param, "0123456789")]) /* i.e. not a number */
		{
			syntax_error(Config.s_NickServ, u, "SET LANGUAGE", NICK_SET_LANGUAGE_SYNTAX);
			return MOD_CONT;
		}
		langnum = atoi(param) - 1;
		if (langnum < 0 || langnum >= NUM_LANGS || langlist[langnum] < 0)
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_LANGUAGE_UNKNOWN, langnum + 1, Config.s_NickServ);
			return MOD_CONT;
		}
		nc->language = langlist[langnum];
		notice_lang(Config.s_NickServ, u, NICK_SET_LANGUAGE_CHANGED);
		return MOD_CONT;
	}

	CommandReturn DoSetUrl(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (nc->url)
			delete [] nc->url;

		if (param)
		{
			nc->url = sstrdup(param);
			notice_lang(Config.s_NickServ, u, NICK_SET_URL_CHANGED, param);
		}
		else
		{
			nc->url = NULL;
			notice_lang(Config.s_NickServ, u, NICK_SET_URL_UNSET);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetEmail(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param && Config.NSForceEmail)
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_EMAIL_UNSET_IMPOSSIBLE);
			return MOD_CONT;
		}
		else if (param && !MailValidate(param))
		{
			notice_lang(Config.s_NickServ, u, MAIL_X_INVALID, param);
			return MOD_CONT;
		}

		Alog() << Config.s_NickServ << ": " << u->GetMask() << " (e-mail: " << (nc->email ? nc->email : "none")  << ") changed its e-mail to " << (param ? param : "none");

		if (nc->email)
			delete [] nc->email;

		if (param)
		{
			nc->email = sstrdup(param);
			notice_lang(Config.s_NickServ, u, NICK_SET_EMAIL_CHANGED, param);
		}
		else
		{
			nc->email = NULL;
			notice_lang(Config.s_NickServ, u, NICK_SET_EMAIL_UNSET);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetICQ(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (param)
		{
			int32 tmp = atol(param);
			if (!tmp)
				notice_lang(Config.s_NickServ, u, NICK_SET_ICQ_INVALID, param);
			else
			{
				nc->icq = tmp;
				notice_lang(Config.s_NickServ, u, NICK_SET_ICQ_CHANGED, param);
			}
		}
		else
		{
			nc->icq = 0;
			notice_lang(Config.s_NickServ, u, NICK_SET_ICQ_UNSET);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetGreet(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (nc->greet)
			delete [] nc->greet;

		if (param)
		{
			char buf[BUFSIZE];
			const char *rest = params.size() > 2 ? params[2].c_str() : NULL;

			snprintf(buf, sizeof(buf), "%s%s%s", param, rest ? " " : "", rest ? rest : "");

			nc->greet = sstrdup(buf);
			notice_lang(Config.s_NickServ, u, NICK_SET_GREET_CHANGED, buf);
		}
		else
		{
			nc->greet = NULL;
			notice_lang(Config.s_NickServ, u, NICK_SET_GREET_UNSET);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetKill(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 1 ? params[1] : "";

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
			notice_lang(Config.s_NickServ, u, NICK_SET_KILL_ON);
		}
		else if (param == "QUICK")
		{
			nc->SetFlag(NI_KILLPROTECT);
			nc->SetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			notice_lang(Config.s_NickServ, u, NICK_SET_KILL_QUICK);
		}
		else if (param == "IMMED")
		{
			if (Config.NSAllowKillImmed)
			{
				nc->SetFlag(NI_KILLPROTECT);
				nc->SetFlag(NI_KILL_IMMED);
				nc->UnsetFlag(NI_KILL_QUICK);
				notice_lang(Config.s_NickServ, u, NICK_SET_KILL_IMMED);
			}
			else
				notice_lang(Config.s_NickServ, u, NICK_SET_KILL_IMMED_DISABLED);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(NI_KILLPROTECT);
			nc->UnsetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			notice_lang(Config.s_NickServ, u, NICK_SET_KILL_OFF);
		}
		else
			syntax_error(Config.s_NickServ, u, "SET KILL", Config.NSAllowKillImmed ? NICK_SET_KILL_IMMED_SYNTAX : NICK_SET_KILL_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetSecure(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 1 ? params[1] : "";

		if (param.empty())
		{
			this->OnSyntaxError(u, "SECURE");
			return MOD_CONT;
		}

		if (param == "ON")
		{
			nc->SetFlag(NI_SECURE);
			notice_lang(Config.s_NickServ, u, NICK_SET_SECURE_ON);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(NI_SECURE);
			notice_lang(Config.s_NickServ, u, NICK_SET_SECURE_OFF);
		}
		else
			syntax_error(Config.s_NickServ, u, "SET SECURE", NICK_SET_SECURE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetPrivate(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 1 ? params[1] : "";

		if (param.empty())
		{
			this->OnSyntaxError(u, "PRIVATE");
			return MOD_CONT;
		}

		if (param == "ON")
		{
			nc->SetFlag(NI_PRIVATE);
			notice_lang(Config.s_NickServ, u, NICK_SET_PRIVATE_ON);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(NI_PRIVATE);
			notice_lang(Config.s_NickServ, u, NICK_SET_PRIVATE_OFF);
		}
		else
			syntax_error(Config.s_NickServ, u, "SET PRIVATE", NICK_SET_PRIVATE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetMsg(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 1 ? params[1] : "";

		if (param.empty())
		{
			this->OnSyntaxError(u, "MSG");
			return MOD_CONT;
		}

		if (!Config.UsePrivmsg)
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_OPTION_DISABLED, "MSG");
			return MOD_CONT;
		}

		if (param == "ON")
		{
			nc->SetFlag(NI_MSG);
			notice_lang(Config.s_NickServ, u, NICK_SET_MSG_ON);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(NI_MSG);
			notice_lang(Config.s_NickServ, u, NICK_SET_MSG_OFF);
		}
		else
			syntax_error(Config.s_NickServ, u, "SET MSG", NICK_SET_MSG_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetHide(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 1 ? params[1] : "";

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
			onmsg = NICK_SET_HIDE_EMAIL_ON;
			offmsg = NICK_SET_HIDE_EMAIL_OFF;
		}
		else if (param == "USERMASK")
		{
			flag = NI_HIDE_MASK;
			onmsg = NICK_SET_HIDE_MASK_ON;
			offmsg = NICK_SET_HIDE_MASK_OFF;
		}
		else if (param == "STATUS")
		{
			flag = NI_HIDE_STATUS;
			onmsg = NICK_SET_HIDE_STATUS_ON;
			offmsg = NICK_SET_HIDE_STATUS_OFF;
		}
		else if (param == "QUIT")
		{
			flag = NI_HIDE_QUIT;
			onmsg = NICK_SET_HIDE_QUIT_ON;
			offmsg = NICK_SET_HIDE_QUIT_OFF;
		}
		else
		{
			syntax_error(Config.s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
			return MOD_CONT;
		}

		param = params.size() > 2 ? params[2] : "";
		if (param.empty())
			syntax_error(Config.s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
		else if (param == "ON")
		{
			nc->SetFlag(flag);
			notice_lang(Config.s_NickServ, u, onmsg, Config.s_NickServ);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(flag);
			notice_lang(Config.s_NickServ, u, offmsg, Config.s_NickServ);
		}
		else
			syntax_error(Config.s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
		return MOD_CONT;
	}

	CommandReturn DoSetAutoOP(User *u, const std::vector<ci::string> &params, NickCore *nc)
	{
		ci::string param = params.size() > 1 ? params[1] : "";

		if (param.empty())
		{
			this->OnSyntaxError(u, "AUTOOP");
			return MOD_CONT;
		}

		/**
		 * This works the other way around, the absence of this flag denotes ON
		 * This is so when people upgrade, and dont have the flag
		 * the default is on
		 **/
		if (param == "ON")
		{
			nc->SetFlag(NI_AUTOOP);
			notice_lang(Config.s_NickServ, u, NICK_SET_AUTOOP_ON);
		}
		else if (param == "OFF")
		{
			nc->UnsetFlag(NI_AUTOOP);
			notice_lang(Config.s_NickServ, u, NICK_SET_AUTOOP_OFF);
		}
		else
			syntax_error(Config.s_NickServ, u, "SET AUTOOP", NICK_SET_AUTOOP_SYNTAX);

		return MOD_CONT;
	}
 public:
	CommandNSSet() : Command("SET", 1, 3)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string cmd = params[0];

		if (readonly)
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_DISABLED);
			return MOD_CONT;
		}

/*
		if (na->HasFlag(NS_FORBIDDEN))
			notice_lang(Config.s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
*/
		if (u->Account()->HasFlag(NI_SUSPENDED))
			notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, u->Account()->display);
		else if (cmd == "DISPLAY")
			return this->DoSetDisplay(u, params, u->Account());
		else if (cmd == "PASSWORD")
			return this->DoSetPassword(u, params, u->Account());
		else if (cmd == "LANGUAGE")
			return this->DoSetLanguage(u, params, u->Account());
		else if (cmd == "URL")
			return this->DoSetUrl(u, params, u->Account());
		else if (cmd == "EMAIL")
			return this->DoSetEmail(u, params, u->Account());
		else if (cmd == "ICQ")
			return this->DoSetICQ(u, params, u->Account());
		else if (cmd == "GREET")
			return this->DoSetGreet(u, params, u->Account());
		else if (cmd == "KILL")
			return this->DoSetKill(u, params, u->Account());
		else if (cmd == "SECURE")
			return this->DoSetSecure(u, params, u->Account());
		else if (cmd == "PRIVATE")
			return this->DoSetPrivate(u, params, u->Account());
		else if (cmd == "MSG")
			return this->DoSetMsg(u, params, u->Account());
		else if (cmd == "HIDE")
			return this->DoSetHide(u, params, u->Account());
		else if (cmd == "AUTOOP")
			return this->DoSetAutoOP(u, params, u->Account());
		else
			notice_lang(Config.s_NickServ, u, NICK_SET_UNKNOWN_OPTION, cmd.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (subcommand.empty())
			notice_help(Config.s_NickServ, u, NICK_HELP_SET);
		else if (subcommand == "DISPLAY")
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_DISPLAY);
		else if (subcommand == "PASSWORD")
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_PASSWORD);
		else if (subcommand == "URL")
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_URL);
		else if (subcommand == "EMAIL")
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_EMAIL);
		else if (subcommand == "ICQ")
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_ICQ);
		else if (subcommand == "GREET")
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_GREET);
		else if (subcommand == "KILL")
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_KILL);
		else if (subcommand == "SECURE")
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_SECURE);
		else if (subcommand == "PRIVATE")
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_PRIVATE);
		else if (subcommand == "MSG")
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_MSG);
		else if (subcommand == "HIDE")
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_HIDE);
		else if (subcommand == "AUTOOP")
			notice_help(Config.s_NickServ, u, NICK_HELP_SET_AUTOOP);
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_NickServ, u, "SET", NICK_SET_SYNTAX);
	}
};

class NSSet : public Module
{
 public:
	NSSet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(NickServ, new CommandNSSet());

		ModuleManager::Attach(I_OnNickServHelp, this);
	}
	void OnNickServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET);
	}
};

MODULE_INIT(NSSet)
