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
	CommandNSSetEmail(const ci::string &cname) : Command(cname, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		if (params.empty() && Config.NSForceEmail)
		{
			notice_lang(Config.s_NickServ, u, NICK_SET_EMAIL_UNSET_IMPOSSIBLE);
			return MOD_CONT;
		}
		else if (!params.empty() && !MailValidate(params[0].c_str()))
		{
			notice_lang(Config.s_NickServ, u, MAIL_X_INVALID, params[0].c_str());
			return MOD_CONT;
		}

		Alog() << Config.s_NickServ << ": " << u->GetMask() << " (e-mail: " << (u->Account()->email ? u->Account()->email : "none")  << ") changed its e-mail to " << (!params.empty() ? params[0] : "none");

		if (u->Account()->email)
			delete [] u->Account()->email;

		if (!params.empty())
		{
			u->Account()->email = sstrdup(params[0].c_str());
			notice_lang(Config.s_NickServ, u, NICK_SET_EMAIL_CHANGED, params[0].c_str());
		}
		else
		{
			u->Account()->email = NULL;
			notice_lang(Config.s_NickServ, u, NICK_SET_EMAIL_UNSET);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
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
	CommandNSSASetEmail(const ci::string &cname) : Command(cname, 1, 2, "nickserv/saset/email")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		assert(nc);

		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

		if (!param && Config.NSForceEmail)
		{
			notice_lang(Config.s_NickServ, u, NICK_SASET_EMAIL_UNSET_IMPOSSIBLE);
			return MOD_CONT;
		}
		else if (Config.NSSecureAdmins && u->Account() != nc && nc->IsServicesOper())
		{
			notice_lang(Config.s_NickServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}
		else if (param && !MailValidate(param))
		{
			notice_lang(Config.s_NickServ, u, MAIL_X_INVALID, param);
			return MOD_CONT;
		}

		Alog() << Config.s_NickServ << ": " << u->GetMask() << " used SASET EMAIL on " << nc->display << " (e-mail: " << (nc->email ? nc->email : "none") << ")";

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

	bool OnHelp(User *u)
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
 public:
	NSSetEmail(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(new CommandNSSetEmail("EMAIL"));

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandNSSASetEmail("EMAIL"));
	}

	~NSSetEmail()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand("EMAIL");

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand("EMAIL");
	}
};

MODULE_INIT(NSSetEmail)
