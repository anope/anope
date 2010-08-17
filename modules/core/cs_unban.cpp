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

class CommandCSUnban : public Command
{
 public:
	CommandCSUnban() : Command("UNBAN", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Channel *c;
		User *u2;

		if (!(c = findchan(chan)))
		{
			notice_lang(Config->s_ChanServ, u, CHAN_X_NOT_IN_USE, chan.c_str());
			return MOD_CONT;
		}

		if (!check_access(u, c->ci, CA_UNBAN))
		{
			notice_lang(Config->s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		u2 = u;
		if (params.size() > 1)
			u2 = finduser(params[1]);

		if (!u2)
		{
			notice_lang(Config->s_ChanServ, u, NICK_X_NOT_IN_USE, params[1].c_str());
			return MOD_CONT;
		}

		common_unban(c->ci, u2->nick);
		if (u2 == u)
			notice_lang(Config->s_ChanServ, u, CHAN_UNBANNED, c->name.c_str());
		else
			notice_lang(Config->s_ChanServ, u, CHAN_UNBANNED_OTHER, u2->nick.c_str(), c->name.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_ChanServ, u, CHAN_HELP_UNBAN);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_ChanServ, u, "UNBAN", CHAN_UNBAN_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_ChanServ, u, CHAN_HELP_CMD_UNBAN);
	}
};

class CSUnban : public Module
{
	CommandCSUnban commandcsunban;

 public:
	CSUnban(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsunban);
	}
};

MODULE_INIT(CSUnban)
