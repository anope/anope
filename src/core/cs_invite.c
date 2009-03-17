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


/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_INVITE);
}

class CommandCSInvite : public Command
{
 public:
	CommandCSInvite() : Command("INVITE", 1, 1)
	{

	}
	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		Channel *c;
		ChannelInfo *ci;

		if (!(c = findchan(chan)))
		{
			notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
			return MOD_CONT;
		}
		else if (!(ci = c->ci))
		{
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
			return MOD_CONT;
		}
		else if (ci->flags & CI_FORBIDDEN)
		{
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
			return MOD_CONT;
		}
		else if (ci->flags & CI_SUSPENDED)
		{
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
			return MOD_CONT;
		}
		else if (!u || !check_access(u, ci, CA_INVITE))
		{
			notice_lang(s_ChanServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}

		if (is_on_chan(c, u))
			notice_lang(s_ChanServ, u, CHAN_INVITE_ALREADY_IN, c->name);
		else
		{
			ircdproto->SendInvite(whosends(ci), chan, u->nick);
			notice_lang(s_ChanServ, u, CHAN_INVITE_SUCCESS, c->name);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_INVITE);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "INVITE", CHAN_INVITE_SYNTAX);
	}
};




class CSInvite : public Module
{
 public:
	CSInvite(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSInvite(), MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};



MODULE_INIT("cs_invite", CSInvite)
