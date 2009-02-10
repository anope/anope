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

void myChanServHelp(User * u);


/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DROP);
}

class CommandCSDrop : public Command
{
 public:
	CommandCSDrop() : Command("DROP", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		char *chan = strtok(NULL, " ");
		ChannelInfo *ci;
		int is_servadmin = is_services_admin(u);

		if (readonly)
		{
			notice_lang(s_ChanServ, u, CHAN_DROP_DISABLED); // XXX: READ_ONLY_MODE?
			return MOD_CONT;
		}

		if (!(ci = cs_findchan(chan)))
		{
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
			return MOD_CONT;
		}

		if (!is_servadmin && (ci->flags & CI_FORBIDDEN))
		{
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
			return MOD_CONT;
		}

		if (!is_servadmin && (ci->flags & CI_SUSPENDED))
		{
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
			return MOD_CONT;
		}

		if (!is_servadmin && (ci->flags & CI_SECUREFOUNDER ? !is_real_founder(u, ci) : !is_founder(u, ci)))
		{
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		int level = get_access(u, ci);

		if (ci->c)
		{
			if (ircd->regmode)
			{
				ci->c->mode &= ~ircd->regmode;
				ircdproto->SendMode(whosends(ci), ci->name, "-r");
			}
		}

		if (ircd->chansqline && (ci->flags & CI_FORBIDDEN))
		{
			ircdproto->SendSQLineDel(ci->name);
		}

		alog("%s: Channel %s dropped by %s!%s@%s (founder: %s)",
			 s_ChanServ, ci->name, u->nick, u->GetIdent().c_str(),
			 u->host, (ci->founder ? ci->founder->display : "(none)"));

		delchan(ci);

		/* We must make sure that the Services admin has not normally the right to
		 * drop the channel before issuing the wallops.
		 */
		if (WallDrop && is_servadmin && level < ACCESS_FOUNDER)
			ircdproto->SendGlobops(s_ChanServ, "\2%s\2 used DROP on channel \2%s\2", u->nick, chan);

		notice_lang(s_ChanServ, u, CHAN_DROPPED, chan);
		send_event(EVENT_CHAN_DROP, 1, chan);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (is_services_admin(u) || is_services_root(u))
            notice_lang(s_ChanServ, u, CHAN_SERVADMIN_HELP_DROP);
		else
			notice_lang(s_ChanServ, u, CHAN_HELP_DROP);

		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "DROP", CHAN_DROP_SYNTAX);
	}
};



class CSDrop : public Module
{
 public:
	CSDrop(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSDrop(), MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};




MODULE_INIT("cs_drop", CSDrop)
