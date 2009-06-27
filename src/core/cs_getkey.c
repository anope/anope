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

class CommandCSGetKey : public Command
{
 public:
	CommandCSGetKey() : Command("GETKEY", 1, 1)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		ChannelInfo *ci;

		ci = cs_findchan(chan);

		if (!check_access(u, ci, CA_GETKEY) && !u->nc->HasCommand("chanserv/getkey"))
		{
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (!ci->c || !ci->c->key)
		{
			notice_lang(s_ChanServ, u, CHAN_GETKEY_NOKEY, chan);
			return MOD_CONT;
		}

		notice_lang(s_ChanServ, u, CHAN_GETKEY_KEY, chan, ci->c->key);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_GETKEY);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "GETKEY", CHAN_GETKEY_SYNTAX);
	}
};


class CSGetKey : public Module
{
 public:
	CSGetKey(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSGetKey(), MOD_UNIQUE);
	}
	void ChanServHelp(User *u)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_GETKEY);
	}
};

MODULE_INIT("cs_getkey", CSGetKey)
