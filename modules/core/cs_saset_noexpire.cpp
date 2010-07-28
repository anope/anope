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

class CommandCSSASetNoexpire : public Command
{
 public:
	CommandCSSASetNoexpire(const Anope::string &cname) : Command(cname, 2, 2, "chanserv/saset/noexpire")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		if (!ci)
			throw CoreException("NULL ci in CommandCSSASetNoexpire");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_NO_EXPIRE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_NOEXPIRE_ON, ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_NO_EXPIRE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_NOEXPIRE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "NOEXPIRE");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &)
	{
		notice_help(Config.s_ChanServ, u, CHAN_SERVADMIN_HELP_SET_NOEXPIRE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &)
	{
		syntax_error(Config.s_ChanServ, u, "SET NOEXPIRE", CHAN_SET_NOEXPIRE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET_NOEXPIRE);
	}
};

class CSSetNoexpire : public Module
{
 public:
	CSSetNoexpire(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(new CommandCSSASetNoexpire("NOEXPIRE"));
	}

	~CSSetNoexpire()
	{
		Command *c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand("NOEXPIRE");
	}
};

MODULE_INIT(CSSetNoexpire)
