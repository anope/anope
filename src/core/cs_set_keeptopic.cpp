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

class CommandCSSetKeepTopic : public Command
{
 public:
	CommandCSSetKeepTopic(const ci::string &cname, const ci::string &cpermission = "") : Command(cname, 2, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		if (params[1] == "ON")
		{
			ci->SetFlag(CI_KEEPTOPIC);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_KEEPTOPIC_ON, ci->name.c_str());
		}
		else if (params[1] == "OFF")
		{
			ci->UnsetFlag(CI_KEEPTOPIC);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_KEEPTOPIC_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "KEEPTOPIC");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_KEEPTOPIC, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SET KEEPTOPIC", CHAN_SET_KEEPTOPIC_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_KEEPTOPIC);
	}
};

class CommandCSSASetKeepTopic : public CommandCSSetKeepTopic
{
 public:
	CommandCSSASetKeepTopic(const ci::string &cname) : CommandCSSetKeepTopic(cname, "chanserv/saset/keeptopic")
	{
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_KEEPTOPIC, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SET KEEPTOPIC", CHAN_SASET_KEEPTOPIC_SYNTAX);
	}
};

class CSSetKeepTopic : public Module
{
 public:
	CSSetKeepTopic(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetKeepTopic("KEEPTOPIC"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetKeepTopic("KEEPTOPIC"));
	}

	~CSSetKeepTopic()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("KEEPTOPIC");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("KEEPTOPIC");
	}
};

MODULE_INIT(CSSetKeepTopic)
