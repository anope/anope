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

class CommandCSSetDescription : public Command
{
 public:
	CommandCSSetDescription(const ci::string &cname, const ci::string &cpermission = "") : Command(cname, 2, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		if (ci->desc)
			delete [] ci->desc;
		ci->desc = sstrdup(params[1].c_str());

		notice_lang(Config.s_ChanServ, u, CHAN_DESC_CHANGED, ci->name.c_str(), ci->desc);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_DESC, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_DESC);
	}
};

class CommandCSSASetDescription : public CommandCSSetDescription
{
 public:
	CommandCSSASetDescription(const ci::string &cname) : CommandCSSetDescription(cname, "chanserv/saset/description")
	{
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_DESC, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SASET", CHAN_SASET_SYNTAX);
	}
};

class CSSetDescription : public Module
{
 public:
	CSSetDescription(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetDescription("DESC"));

		c = FindCommand(ChanServ, "SASEt");
		if (c)
			c->AddSubcommand(new CommandCSSASetDescription("DESC"));
	}

	~CSSetDescription()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("DESC");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("DESC");
	}
};

MODULE_INIT(CSSetDescription)
