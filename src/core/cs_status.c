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

void myChanServHelp(User *u);

class CommandCSStatus : public Command
{
 public:
	CommandCSStatus() : Command("STATUS", 2, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		ChannelInfo *ci;
		User *u2;
		const char *chan = params[0].c_str();
		const char *nick = params[1].c_str();
		const char *temp = NULL;

		if (!u->nc->HasCommand("chanserv/status"))
			return MOD_CONT; // XXX: error?

		if (!(ci = cs_findchan(chan)))
		{
			temp = chan;
			chan = nick;
			nick = temp;
			ci = cs_findchan(chan);
		}
		if (!ci)
			notice_lang(s_ChanServ, u, CHAN_STATUS_NOT_REGGED, temp);
		else if ((u2 = finduser(nick)))
			notice_lang(s_ChanServ, u, CHAN_STATUS_INFO, chan, nick, get_access(u2, ci));
		else /* !u2 */
			notice_lang(s_ChanServ, u, CHAN_STATUS_NOTONLINE, nick);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_SERVADMIN_HELP_STATUS);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "STATUS", CHAN_STATUS_SYNTAX);
	}
};

class CSStatus : public Module
{
 public:
	CSStatus(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(CHANSERV, new CommandCSStatus(), MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};

/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User *u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_STATUS);
}

MODULE_INIT("cs_status", CSStatus)
