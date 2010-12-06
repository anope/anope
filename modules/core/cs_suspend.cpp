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

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &reason = params.size() > 1 ? params[1] : "";

		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;

		/* Assumes that permission checking has already been done. */
		if (Config->ForceForbidReason && reason.empty())
		{
			this->OnSyntaxError(source, "");
			return MOD_CONT;
		}

		/* You should not SUSPEND a FORBIDEN channel */
		if (ci->HasFlag(CI_FORBIDDEN))
		{
			source.Reply(CHAN_MAY_NOT_BE_REGISTERED, ci->name.c_str());
			return MOD_CONT;
		}

		if (readonly)
			source.Reply(READ_ONLY_MODE);

		ci->SetFlag(CI_SUSPENDED);
		ci->forbidby = u->nick;
		if (!reason.empty())
			ci->forbidreason = reason;

		if (c)
		{
			for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
			{
				UserContainer *uc = *it++;

				if (uc->user->HasMode(UMODE_OPER))
					continue;

				c->Kick(NULL, uc->user, "%s", !reason.empty() ? reason.c_str() : GetString(uc->user->Account(), CHAN_SUSPEND_REASON).c_str());
			}
		}

		if (Config->WallForbid)
			ircdproto->SendGlobops(ChanServ, "\2%s\2 used SUSPEND on channel \2%s\2", u->nick.c_str(), ci->name.c_str());

		Log(LOG_ADMIN, u, this, ci) << (!reason.empty() ? reason : "No reason");
		source.Reply(CHAN_SUSPEND_SUCCEEDED, ci->name.c_str());

		FOREACH_MOD(I_OnChanSuspend, OnChanSuspend(ci));

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_SERVADMIN_HELP_SUSPEND);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SUSPEND", Config->ForceForbidReason ? CHAN_SUSPEND_SYNTAX_REASON : CHAN_SUSPEND_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_SUSPEND);
	}
};

class CommandCSUnSuspend : public Command
{
 public:
	CommandCSUnSuspend() : Command("UNSUSPEND", 1, 1, "chanserv/suspend")
	{
		this->SetFlag(CFLAG_ALLOW_SUSPENDED);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (readonly)
			source.Reply(READ_ONLY_MODE);

		/* Only UNSUSPEND already suspended channels */
		if (!ci->HasFlag(CI_SUSPENDED))
		{
			source.Reply(CHAN_UNSUSPEND_FAILED, ci->name.c_str());
			return MOD_CONT;
		}

		Log(LOG_ADMIN, u, this, ci) << " was suspended for: " << (!ci->forbidreason.empty() ? ci->forbidreason : "No reason");

		ci->UnsetFlag(CI_SUSPENDED);
		ci->forbidreason.clear();
		ci->forbidby.clear();

		if (Config->WallForbid)
			ircdproto->SendGlobops(ChanServ, "\2%s\2 used UNSUSPEND on channel \2%s\2", u->nick.c_str(), ci->name.c_str());

		source.Reply(CHAN_UNSUSPEND_SUCCEEDED, ci->name.c_str());

		FOREACH_MOD(I_OnChanUnsuspend, OnChanUnsuspend(ci));

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_SERVADMIN_HELP_UNSUSPEND);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "UNSUSPEND", CHAN_UNSUSPEND_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_UNSUSPEND);
	}
};

class CSSuspend : public Module
{
	CommandCSSuspend commandcssuspend;
	CommandCSUnSuspend commandcsunsuspend;

 public:
	CSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcssuspend);
		this->AddCommand(ChanServ, &commandcsunsuspend);
	}
};

MODULE_INIT(CSSuspend)
