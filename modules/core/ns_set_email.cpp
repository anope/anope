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

class CommandNSSetEmail : public Command
{
 public:
	CommandNSSetEmail() : Command("EMAIL", 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		if (params.empty() && Config.NSForceEmail)
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_EMAIL_UNSET_IMPOSSIBLE);
			return MOD_CONT;
		}
		else if (!params.empty() && !MailValidate(params[0]))
		{
			notice_lang(Config.s_NickServ, u, MAIL_X_INVALID, params[0].c_str());
			return MOD_CONT;
		}

		Alog() << Config.s_NickServ << ": " << u->GetMask() << " (e-mail: " << (!u->Account()->email.empty() ? u->Account()->email : "none")  << ") changed its e-mail to " << (!params.empty() ? params[0] : "none");

		if (!params.empty())
		{
			u->Account()->email = params[0];
			notice_lang(Config.s_NickServ, u, NICK_SET_EMAIL_CHANGED, params[0].c_str());
		}
		else
		{
			u->Account()->email.clear();
			notice_lang(Config.s_NickServ, u, NICK_SET_EMAIL_UNSET);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_EMAIL);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET_EMAIL);
	}
};

class CommandNSSASetEmail : public Command
{
 public:
	CommandNSSASetEmail() : Command("EMAIL", 1, 2, "nickserv/saset/email")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		if (!nc)
			throw CoreException("NULL nc in CommandNSSASetEmail");

		Anope::string param = params.size() > 1 ? params[1] : "";

		if (param.empty() && Config.NSForceEmail)
		{
			notice_lang(Config.s_NickServ, u, NICK_SASET_EMAIL_UNSET_IMPOSSIBLE);
			return MOD_CONT;
		}
		else if (Config.NSSecureAdmins && u->Account() != nc && nc->IsServicesOper())
		{
			notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}
		else if (!param.empty() && !MailValidate(param))
		{
			notice_lang(Config.s_NickServ, u, MAIL_X_INVALID, param.c_str());
			return MOD_CONT;
		}

		Alog() << Config.s_NickServ << ": " << u->GetMask() << " used SASET EMAIL on " << nc->display << " (e-mail: " << (!nc->email.empty() ? nc->email : "none") << ")";

		if (!param.empty())
		{
			nc->email = param;
			notice_lang(Config.s_NickServ, u, NICK_SASET_EMAIL_CHANGED, nc->display.c_str(), param.c_str());
		}
		else
		{
			nc->email.clear();
			notice_lang(Config.s_NickServ, u, NICK_SASET_EMAIL_UNSET, nc->display.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_EMAIL);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET_EMAIL);
	}
};

class NSSetEmail : public Module
{
	CommandNSSetEmail commandnssetemail;
	CommandNSSASetEmail commandnssasetemail;

 public:
	NSSetEmail(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(&commandnssetemail);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(&commandnssasetemail);
	}

	~NSSetEmail()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand(&commandnssetemail);

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand(&commandnssasetemail);
	}
};

MODULE_INIT(NSSetEmail)
