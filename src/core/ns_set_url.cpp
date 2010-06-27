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

class CommandNSSetURL : public Command
{
 public:
	CommandNSSetURL(const ci::string &cname) : Command(cname, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		if (u->Account()->url)
			delete [] u->Account()->url;

		if (!params.empty())
		{
			u->Account()->url = sstrdup(params[0].c_str());
			notice_lang(Config.s_NickServ, u, NICK_SET_URL_CHANGED, params[0].c_str());
		}
		else
		{
			u->Account()->url = NULL;
			notice_lang(Config.s_NickServ, u, NICK_SET_URL_UNSET);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SET_URL);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SET_URL);
	}
};

class CommandNSSASetURL : public Command
{
 public:
	CommandNSSASetURL(const ci::string &cname) : Command(cname, 1, 2, "nickserv/saset/url")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		NickCore *nc = findcore(params[0]);
		assert(nc);

		const char *param = params.size() > 1 ? params[1].c_str() : NULL;

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

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_NickServ, u, NICK_HELP_SASET_URL);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_NickServ, u, NICK_HELP_CMD_SASET_URL);
	}
};

class NSSetURL : public Module
{
 public:
	NSSetURL(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->AddSubcommand(new CommandNSSetURL("URL"));

		c = FindCommand(NickServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandNSSASetURL("URL"));
	}

	~NSSetURL()
	{
		Command *c = FindCommand(NickServ, "SET");
		if (c)
			c->DelSubcommand("URL");
		c = FindCommand(NickServ, "SASET");
		if (c)
			c->DelSubcommand("URL");
	}
};

MODULE_INIT(NSSetURL)
