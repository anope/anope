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

class CommandCSSuspend : public Command
{
 public:
	CommandCSSuspend() : Command("SUSPEND", 1, 2, "chanserv/suspend")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string reason = params.size() > 1 ? params[1] : "";
		ChannelInfo *ci = cs_findchan(chan);

		Channel *c;

		/* Assumes that permission checking has already been done. */
		if (Config.ForceForbidReason && reason.empty())
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
			notice_lang(Config.s_ChanServ, u, CHAN_MAY_NOT_BE_REGISTERED, chan.c_str());
			return MOD_CONT;
		}

		if (readonly)
			notice_lang(Config.s_ChanServ, u, READ_ONLY_MODE);

		if (ci)
		{
			ci->SetFlag(CI_SUSPENDED);
			ci->forbidby = u->nick;
			if (!reason.empty())
				ci->forbidreason = reason;

			if ((c = findchan(ci->name)))
			{
				for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
				{
					UserContainer *uc = *it++;

					if (is_oper(uc->user))
						continue;

					c->Kick(NULL, uc->user, "%s", !reason.empty() ? reason.c_str() : getstring(uc->user->Account(), CHAN_SUSPEND_REASON));
				}
			}

			if (Config.WallForbid)
				ircdproto->SendGlobops(ChanServ, "\2%s\2 used SUSPEND on channel \2%s\2", u->nick.c_str(), ci->name.c_str());

			Alog() << Config.s_ChanServ << ": " << u->GetMask() << " set SUSPEND for channel " << ci->name;
			notice_lang(Config.s_ChanServ, u, CHAN_SUSPEND_SUCCEEDED, chan.c_str());

			FOREACH_MOD(I_OnChanSuspend, OnChanSuspend(ci));
		}
		else
		{
			Alog() << Config.s_ChanServ << ": Valid SUSPEND for " << ci->name << " by " << u->GetMask() << " failed";
			notice_lang(Config.s_ChanServ, u, CHAN_SUSPEND_FAILED, chan.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_SERVADMIN_HELP_SUSPEND);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "SUSPEND", Config.ForceForbidReason ? CHAN_SUSPEND_SYNTAX_REASON : CHAN_SUSPEND_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SUSPEND);
	}
};

class CommandCSUnSuspend : public Command
{
 public:
	CommandCSUnSuspend() : Command("UNSUSPEND", 1, 1, "chanserv/suspend")
	{
		this->SetFlag(CFLAG_ALLOW_SUSPENDED);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		ChannelInfo *ci = cs_findchan(chan);

		if (chan[0] != '#')
		{
			notice_lang(Config.s_ChanServ, u, CHAN_UNSUSPEND_ERROR);
			return MOD_CONT;
		}
		if (readonly)
			notice_lang(Config.s_ChanServ, u, READ_ONLY_MODE);

		/* Only UNSUSPEND already suspended channels */
		if (!ci->HasFlag(CI_SUSPENDED))
		{
			notice_lang(Config.s_ChanServ, u, CHAN_UNSUSPEND_FAILED, chan.c_str());
			return MOD_CONT;
		}

		if (ci)
		{
			ci->UnsetFlag(CI_SUSPENDED);
			ci->forbidreason.clear();
			ci->forbidby.clear();

			if (Config.WallForbid)
				ircdproto->SendGlobops(ChanServ, "\2%s\2 used UNSUSPEND on channel \2%s\2", u->nick.c_str(), ci->name.c_str());

			Alog() << Config.s_ChanServ << ": " << u->GetMask() << " set UNSUSPEND for channel " << ci->name;
			notice_lang(Config.s_ChanServ, u, CHAN_UNSUSPEND_SUCCEEDED, chan.c_str());

			FOREACH_MOD(I_OnChanUnsuspend, OnChanUnsuspend(ci));
		}
		else
		{
			Alog() << Config.s_ChanServ << ": Valid UNSUSPEND for " << chan << " by " << u->nick << " failed";
			notice_lang(Config.s_ChanServ, u, CHAN_UNSUSPEND_FAILED, chan.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_SERVADMIN_HELP_UNSUSPEND);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "UNSUSPEND", CHAN_UNSUSPEND_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_UNSUSPEND);
	}
};

class CSSuspend : public Module
{
 public:
	CSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, new CommandCSSuspend());
		this->AddCommand(ChanServ, new CommandCSUnSuspend());
	}
};

MODULE_INIT(CSSuspend)
