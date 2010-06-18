/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
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

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *reason = params.size() > 1 ? params[1].c_str() : NULL;
		ChannelInfo *ci = cs_findchan(chan);

		Channel *c;

		/* Assumes that permission checking has already been done. */
		if (Config.ForceForbidReason && !reason)
		{
			this->OnSyntaxError(u, "");
			return MOD_CONT;
		}

		if (chan[0] != '#')
		{
			notice_lang(Config.s_ChanServ, u, CHAN_UNSUSPEND_ERROR);
			return MOD_CONT;
		}

		/* You should not SUSPEND a FORBIDEN channel */
		if (ci->HasFlag(CI_FORBIDDEN))
		{
			notice_lang(Config.s_ChanServ, u, CHAN_MAY_NOT_BE_REGISTERED, chan);
			return MOD_CONT;
		}

		if (readonly)
			notice_lang(Config.s_ChanServ, u, READ_ONLY_MODE);

		if (ci)
		{
			ci->SetFlag(CI_SUSPENDED);
			ci->forbidby = sstrdup(u->nick.c_str());
			if (reason)
				ci->forbidreason = sstrdup(reason);

			if ((c = findchan(ci->name.c_str())))
			{
				for (CUserList::iterator it = c->users.begin(); it != c->users.end();)
				{
					UserContainer *uc = *it++;

					if (is_oper(uc->user))
						continue;

					c->Kick(NULL, uc->user, "%s", reason ? reason : getstring(uc->user->Account(), CHAN_SUSPEND_REASON));
				}
			}

			if (Config.WallForbid)
				ircdproto->SendGlobops(findbot(Config.s_ChanServ), "\2%s\2 used SUSPEND on channel \2%s\2", u->nick.c_str(), ci->name.c_str());

			Alog() << Config.s_ChanServ << ": " << u->GetMask() << " set SUSPEND for channel " << ci->name;
			notice_lang(Config.s_ChanServ, u, CHAN_SUSPEND_SUCCEEDED, chan);

			FOREACH_MOD(I_OnChanSuspend, OnChanSuspend(ci));
		}
		else
		{
			Alog() << Config.s_ChanServ << ": Valid SUSPEND for " << ci->name << " by " << u->GetMask() << " failed";
			notice_lang(Config.s_ChanServ, u, CHAN_SUSPEND_FAILED, chan);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_SERVADMIN_HELP_SUSPEND);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "SUSPEND", Config.ForceForbidReason ? CHAN_SUSPEND_SYNTAX_REASON : CHAN_SUSPEND_SYNTAX);
	}
};

class CommandCSUnSuspend : public Command
{
 public:
	CommandCSUnSuspend() : Command("UNSUSPEND", 1, 1, "chanserv/suspend")
	{
		this->SetFlag(CFLAG_ALLOW_SUSPENDED);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		ChannelInfo *ci = cs_findchan(chan);

		if (chan[0] != '#')
		{
			notice_lang(Config.s_ChanServ, u, CHAN_UNSUSPEND_ERROR);
			return MOD_CONT;
		}
		if (readonly)
			notice_lang(Config.s_ChanServ, u, READ_ONLY_MODE);

		/* Only UNSUSPEND already suspended channels */
		if (!(ci->HasFlag(CI_SUSPENDED)))
		{
			notice_lang(Config.s_ChanServ, u, CHAN_UNSUSPEND_FAILED, chan);
			return MOD_CONT;
		}

		if (ci)
		{
			ci->UnsetFlag(CI_SUSPENDED);
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

			if (Config.WallForbid)
				ircdproto->SendGlobops(findbot(Config.s_ChanServ), "\2%s\2 used UNSUSPEND on channel \2%s\2", u->nick.c_str(), ci->name.c_str());

			Alog() << Config.s_ChanServ << ": " << u->GetMask() << " set UNSUSPEND for channel " << ci->name;
			notice_lang(Config.s_ChanServ, u, CHAN_UNSUSPEND_SUCCEEDED, chan);

			FOREACH_MOD(I_OnChanUnsuspend, OnChanUnsuspend(ci));
		}
		else
		{
			Alog() << Config.s_ChanServ << ": Valid UNSUSPEND for " << chan << " by " << u->nick << " failed";
			notice_lang(Config.s_ChanServ, u, CHAN_UNSUSPEND_FAILED, chan);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_SERVADMIN_HELP_UNSUSPEND);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "UNSUSPEND", CHAN_UNSUSPEND_SYNTAX);
	}
};

class CSSuspend : public Module
{
 public:
	CSSuspend(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(CHANSERV, new CommandCSSuspend());
		this->AddCommand(CHANSERV, new CommandCSUnSuspend());

		ModuleManager::Attach(I_OnChanServHelp, this);
	}
	void OnChanServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SUSPEND);
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_UNSUSPEND);
	}
};

MODULE_INIT(CSSuspend)
