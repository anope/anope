/* ChanServ core functions
 *
 * (C) 2003-2009 Anope Team
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

class CommandCSUnban : public Command
{
 public:
	CommandCSUnban() : Command("UNBAN", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		Channel *c;
		User *u2;

		if (!(c = findchan(chan)))
		{
			notice_lang(Config.s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
			return MOD_CONT;
		}

		if (!check_access(u, c->ci, CA_UNBAN))
		{
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		u2 = u;
		if (params.size() > 1)
			u2 = finduser(params[1].c_str());

		if (!u2)
		{
			notice_lang(Config.s_ChanServ, u, NICK_X_NOT_IN_USE, params[1].c_str());
			return MOD_CONT;
		}

		common_unban(c->ci, u2->nick);
		if (u2 == u)
			notice_lang(Config.s_ChanServ, u, CHAN_UNBANNED, c->name.c_str());
		else
			notice_lang(Config.s_ChanServ, u, CHAN_UNBANNED_OTHER, u2->nick.c_str(), c->name.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_UNBAN);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "UNBAN", CHAN_UNBAN_SYNTAX);
	}
};

class CSUnban : public Module
{
 public:
	CSUnban(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(CHANSERV, new CommandCSUnban());

		ModuleManager::Attach(I_OnChanServHelp, this);
	}

	void OnChanServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_UNBAN);
	}
};

MODULE_INIT(CSUnban)

