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

class CommandCSSetSecure : public Command
{
 public:
	CommandCSSetSecure(const ci::string &cname, const ci::string &cpermission = "") : Command(cname, 2, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		if (params[1] == "ON")
		{
			ci->SetFlag(CI_SECURE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SECURE_ON, ci->name.c_str());
		}
		else if (params[1] == "OFF")
		{
			ci->UnsetFlag(CI_SECURE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SECURE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "SECURE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SECURE, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SET SECURE", CHAN_SET_SECURE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_SECURE);
	}
};

class CommandCSSASetSecure : public CommandCSSetSecure
{
 public:
	CommandCSSASetSecure(const ci::string &cname) : CommandCSSetSecure(cname, "chanserv/saset/secure")
	{
	}

	bool OnHelp(User *u, const ci::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SECURE, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SASET SECURE", CHAN_SASET_SECURE_SYNTAX);
	}
};

class CSSetSecure : public Module
{
 public:
	CSSetSecure(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetSecure("SECURE"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetSecure("SECURE"));
	}

	~CSSetSecure()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("SECURE");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("SECURE");
	}
};

MODULE_INIT(CSSetSecure)
