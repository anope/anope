/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
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

class CommandCSSetBanType : public Command
{
 public:
	CommandCSSetBanType(const ci::string &cname, const std::string &cpermission = "") : Command(cname, 2, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		char *endptr;

		int16 bantype = strtol(params[1].c_str(), &endptr, 10);

		if (*endptr != 0 || bantype < 0 || bantype > 3)
			notice_lang(Config.s_ChanServ, u, CHAN_SET_BANTYPE_INVALID, params[1].c_str());
		else
		{
			ci->bantype = bantype;
			notice_lang(Config.s_ChanServ, u, CHAN_SET_BANTYPE_CHANGED, ci->name.c_str(), ci->bantype);
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_BANTYPE, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_BANTYPE);
	}
};

class CommandCSSASetBanType : public CommandCSSetBanType
{
 public:
	CommandCSSASetBanType(const ci::string &cname) : CommandCSSetBanType(cname, "chanserv/saset/bantype")
	{
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_BANTYPE, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		// XXX
		syntax_error(Config.s_ChanServ, u, "SASET", CHAN_SASET_SYNTAX);
	}
};

class CSSetBanType : public Module
{
 public:
	CSSetBanType(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetBanType("BANTYPE"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetBanType("BANTYPE"));
	}

	~CSSetBanType()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("BANTYPE");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("BANTYPE");
	}
};

MODULE_INIT(CSSetBanType)
