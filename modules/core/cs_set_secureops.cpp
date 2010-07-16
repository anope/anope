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

class CommandCSSetSecureOps : public Command
{
 public:
	CommandCSSetSecureOps(const ci::string &cname, const ci::string &cpermission = "") : Command(cname, 2, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		if (params[1] == "ON")
		{
			ci->SetFlag(CI_SECUREOPS);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SECUREOPS_ON, ci->name.c_str());
		}
		else if (params[1] == "OFF")
		{
			ci->UnsetFlag(CI_SECUREOPS);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SECUREOPS_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "SECUREOPS");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SECUREOPS, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SET SECUREOPS", CHAN_SET_SECUREOPS_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_SECUREOPS);
	}
};

class CommandCSSASetSecureOps : public CommandCSSetSecureOps
{
 public:
	CommandCSSASetSecureOps(const ci::string &cname) : CommandCSSetSecureOps(cname, "chanserv/saset/secureops")
	{
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SECUREOPS, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SASET SECUREOPS", CHAN_SASET_SECUREOPS_SYNTAX);
	}
};

class CSSetSecureOps : public Module
{
 public:
	CSSetSecureOps(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetSecureOps("SECUREOPS"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetSecureOps("SECUREOPS"));
	}

	~CSSetSecureOps()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("SECUREOPS");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("SECUREOPS");
	}
};

MODULE_INIT(CSSetSecureOps)
