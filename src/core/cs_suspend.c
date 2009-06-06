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

class CommandCSSuspend : public Command
{
 public:
	CommandCSSuspend() : Command("SUSPEND", 1, 2, "chanserv/suspend")
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		ChannelInfo *ci;
		const char *chan = params[0].c_str();
		const char *reason = params.size() > 1 ? params[1].c_str() : NULL;

		Channel *c;

		/* Assumes that permission checking has already been done. */
		if (ForceForbidReason && !reason)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (chan[0] != '#')
		{
			notice_lang(s_ChanServ, u, CHAN_UNSUSPEND_ERROR);
			return MOD_CONT;
		}

		/* Only SUSPEND existing channels, otherwise use FORBID (bug #54) */
		if (!(ci = cs_findchan(chan)))
		{
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
			return MOD_CONT;
		}

		/* You should not SUSPEND a FORBIDEN channel */
		if (ci->flags & CI_FORBIDDEN)
		{
			notice_lang(s_ChanServ, u, CHAN_MAY_NOT_BE_REGISTERED, chan);
			return MOD_CONT;
		}

		if (readonly)
			notice_lang(s_ChanServ, u, READ_ONLY_MODE);

		if (ci)
		{
			ci->flags |= CI_SUSPENDED;
			ci->forbidby = sstrdup(u->nick);
			if (reason)
				ci->forbidreason = sstrdup(reason);

			if ((c = findchan(ci->name)))
			{
				struct c_userlist *cu, *nextu;
				const char *av[3];

				for (cu = c->users; cu; cu = nextu)
				{
					nextu = cu->next;

					if (is_oper(cu->user))
						continue;

					av[0] = c->name;
					av[1] = cu->user->nick;
					av[2] = reason ? reason : getstring(cu->user->nc, CHAN_SUSPEND_REASON);
					ircdproto->SendKick(findbot(s_ChanServ), av[0], av[1], av[2]);
					do_kick(s_ChanServ, 3, av);
				}
			}

			if (WallForbid)
				ircdproto->SendGlobops(s_ChanServ, "\2%s\2 used SUSPEND on channel \2%s\2", u->nick, ci->name);

			alog("%s: %s set SUSPEND for channel %s", s_ChanServ, u->nick, ci->name);
			notice_lang(s_ChanServ, u, CHAN_SUSPEND_SUCCEEDED, chan);

			FOREACH_MOD(I_OnChanSuspend, OnChanSuspend(ci));
		}
		else
		{
			alog("%s: Valid SUSPEND for %s by %s failed", s_ChanServ, ci->name, u->nick);
			notice_lang(s_ChanServ, u, CHAN_SUSPEND_FAILED, chan);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_SERVADMIN_HELP_SUSPEND);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "SUSPEND", ForceForbidReason ? CHAN_SUSPEND_SYNTAX_REASON : CHAN_SUSPEND_SYNTAX);
	}
};

class CommandCSUnSuspend : public Command
{
 public:
	CommandCSUnSuspend() : Command("UNSUSPEND", 1, 1, "chanserv/suspend")
	{
		this->SetFlag(CFLAG_ALLOW_SUSPENDED);
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		ChannelInfo *ci;
		const char *chan = params[0].c_str();

		if (chan[0] != '#')
		{
			notice_lang(s_ChanServ, u, CHAN_UNSUSPEND_ERROR);
			return MOD_CONT;
		}
		if (readonly)
			notice_lang(s_ChanServ, u, READ_ONLY_MODE);

		/* Only UNSUSPEND already suspended channels */
		if (!(ci = cs_findchan(chan)))
		{
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
			return MOD_CONT;
		}

		if (!(ci->flags & CI_SUSPENDED))
		{
			notice_lang(s_ChanServ, u, CHAN_UNSUSPEND_FAILED, chan);
			return MOD_CONT;
		}

		if (ci)
		{
			ci->flags &= ~CI_SUSPENDED;
			if (ci->forbidreason)
			{
				delete [] ci->forbidreason;
				ci->forbidreason = NULL;
			}
			if (ci->forbidby)
			{
				delete [] ci->forbidby;
				ci->forbidby = NULL;
			}

			if (WallForbid)
				ircdproto->SendGlobops(s_ChanServ, "\2%s\2 used UNSUSPEND on channel \2%s\2", u->nick, ci->name);

			alog("%s: %s set UNSUSPEND for channel %s", s_ChanServ, u->nick, ci->name);
			notice_lang(s_ChanServ, u, CHAN_UNSUSPEND_SUCCEEDED, chan);

			FOREACH_MOD(I_OnChanUnsuspend, OnChanUnsuspend(ci));
		}
		else
		{
			alog("%s: Valid UNSUSPEND for %s by %s failed", s_ChanServ, chan, u->nick);
			notice_lang(s_ChanServ, u, CHAN_UNSUSPEND_FAILED, chan);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_SERVADMIN_HELP_UNSUSPEND);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "UNSUSPEND", CHAN_UNSUSPEND_SYNTAX);
	}
};

class CSSuspend : public Module
{
 public:
	CSSuspend(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(CHANSERV, new CommandCSSuspend(), MOD_UNIQUE);
		this->AddCommand(CHANSERV, new CommandCSUnSuspend(), MOD_UNIQUE);
	}
	void ChanServHelp(User *u)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_SUSPEND);
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_UNSUSPEND);
	}
};

MODULE_INIT("cs_suspend", CSSuspend)
