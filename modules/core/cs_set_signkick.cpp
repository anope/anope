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

class CommandCSSetSignKick : public Command
{
 public:
	CommandCSSetSignKick(const Anope::string &cname, const Anope::string &cpermission = "") : Command(cname, 2, 2, cpermission)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		assert(ci);

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SIGNKICK);
			ci->UnsetFlag(CI_SIGNKICK_LEVEL);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SIGNKICK_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("LEVEL"))
		{
			ci->SetFlag(CI_SIGNKICK_LEVEL);
			ci->UnsetFlag(CI_SIGNKICK);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SIGNKICK_LEVEL, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SIGNKICK);
			ci->UnsetFlag(CI_SIGNKICK_LEVEL);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SIGNKICK_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "SIGNKICK");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SIGNKICK, "SET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SET SIGNKICK", CHAN_SET_SIGNKICK_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_SIGNKICK);
	}
};

class CommandCSSASetSignKick : public CommandCSSetSignKick
{
 public:
	CommandCSSASetSignKick(const Anope::string &cname) : CommandCSSetSignKick(cname, "chanserv/saset/signkick")
	{
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SIGNKICK, "SASET");
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SASET SIGNKICK", CHAN_SASET_SIGNKICK_SYNTAX);
	}
};

class CSSetSignKick : public Module
{
 public:
	CSSetSignKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(new CommandCSSetSignKick("SIGNKICK"));

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetSignKick("SIGNKICK"));
	}

	~CSSetSignKick()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand("SIGNKICK");

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("SIGNKICK");
	}
};

MODULE_INIT(CSSetSignKick)
