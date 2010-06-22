/* ChanServ core functions
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

class CommandCSSetURL : public Command
{
 public:
	CommandCSSetURL(const ci::string &cname, const ci::string &cpermission = "") : Command(cname, 1, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		if (ci->url)
			delete [] ci->url;
		if (params.size() > 1)
		{
			ci->url = sstrdup(params[1].c_str());
			notice_lang(Config.s_ChanServ, u, CHAN_URL_CHANGED, ci->name.c_str(), ci->url);
		}
		else
		{
			ci->url = NULL;
			notice_lang(Config.s_ChanServ, u, CHAN_URL_UNSET, ci->name.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_URL, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_URL);
	}
};

class CommandCSSASetURL : public CommandCSSetURL
{
 public:
	CommandCSSASetURL(const ci::string &cname) : CommandCSSetURL(cname, "chanserv/saset/url")
	{
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_URL, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SASET URL", CHAN_SASET_SYNTAX);
	}
};

class CSSetURL : public Module
{
 public:
	CSSetURL(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetURL("URL"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetURL("URL"));
	}

	~CSSetURL()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("URL");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("URL");
	}
};

MODULE_INIT(CSSetURL)
