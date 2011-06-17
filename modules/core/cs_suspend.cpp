/* ChanServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "chanserv.h"

class CommandCSSuspend : public Command
{
 public:
	CommandCSSuspend() : Command("SUSPEND", 1, 2, "chanserv/suspend")
	{ 
		this->SetDesc(_("Prevent a channel from being used preserving channel data and settings"));
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

		if (readonly)
			source.Reply(_(READ_ONLY_MODE));

		ci->SetFlag(CI_SUSPENDED);
		ci->Extend("suspend_by", new ExtensibleItemRegular<Anope::string>(u->nick));
		if (!reason.empty())
			ci->Extend("suspend_reason", new ExtensibleItemRegular<Anope::string>(u->nick));

		if (c)
		{
			for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
			{
				UserContainer *uc = *it++;

				if (uc->user->HasMode(UMODE_OPER))
					continue;

				c->Kick(NULL, uc->user, "%s", !reason.empty() ? reason.c_str() : translate(uc->user, _("This channel has been suspended.")));
			}
		}

		Log(LOG_ADMIN, u, this, ci) << (!reason.empty() ? reason : "No reason");
		source.Reply(_("Channel \002%s\002 is now suspended."), ci->name.c_str());

		FOREACH_MOD(I_OnChanSuspend, OnChanSuspend(ci));

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002SUSPEND \037channel\037 [\037reason\037]\002\n"
				" \n"
				"Disallows anyone from registering or using the given\n"
				"channel.  May be cancelled by using the UNSUSPEND\n"
				"command to preserve all previous channel data/settings.\n"
				" \n"
				"Reason may be required on certain networks."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SUSPEND", Config->ForceForbidReason ? _("SUSPEND \037channel\037 \037reason\037") : _("SUSPEND \037channel\037 \037freason\037"));
	}
};

class CommandCSUnSuspend : public Command
{
 public:
	CommandCSUnSuspend() : Command("UNSUSPEND", 1, 1, "chanserv/suspend")
	{
		this->SetFlag(CFLAG_ALLOW_SUSPENDED);
		this->SetDesc(_("Releases a suspended channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (readonly)
			source.Reply(_(READ_ONLY_MODE));

		/* Only UNSUSPEND already suspended channels */
		if (!ci->HasFlag(CI_SUSPENDED))
		{
			source.Reply(_("Couldn't release channel \002%s\002!"), ci->name.c_str());
			return MOD_CONT;
		}

		Anope::string by, reason;
		ci->GetExtRegular("suspend_by", by);
		ci->GetExtRegular("suspend_reason", reason);
		Log(LOG_ADMIN, u, this, ci) << " which was suspended by " << by << " for: " << (!reason.empty() ? reason : "No reason");

		ci->UnsetFlag(CI_SUSPENDED);
		ci->Shrink("suspend_by");
		ci->Shrink("suspend_reason");

		source.Reply(_("Channel \002%s\002 is now released."), ci->name.c_str());

		FOREACH_MOD(I_OnChanUnsuspend, OnChanUnsuspend(ci));

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002UNSUSPEND \037channel\037\002\n"
				" \n"
				"Releases a suspended channel. All data and settings\n"
				"are preserved from before the suspension."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "UNSUSPEND", _("UNSUSPEND \037channel\037"));
	}
};

class CSSuspend : public Module
{
	CommandCSSuspend commandcssuspend;
	CommandCSUnSuspend commandcsunsuspend;

 public:
	CSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		this->AddCommand(chanserv->Bot(), &commandcssuspend);
		this->AddCommand(chanserv->Bot(), &commandcsunsuspend);
	}
};

MODULE_INIT(CSSuspend)
