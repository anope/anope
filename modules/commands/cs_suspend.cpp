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

class CommandCSSuspend : public Command
{
 public:
	CommandCSSuspend(Module *creator) : Command(creator, "chanserv/suspend", 1, 2)
	{ 
		this->SetDesc(_("Prevent a channel from being used preserving channel data and settings"));
		this->SetSyntax(_("\037channel\037 [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &reason = params.size() > 1 ? params[1] : "";

		User *u = source.u;

		if (Config->ForceForbidReason && reason.empty())
		{
			this->OnSyntaxError(source, "");
			return;
		}

		if (readonly)
			source.Reply(READ_ONLY_MODE);

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		ci->SetFlag(CI_SUSPENDED);
		ci->Extend("suspend_by", new ExtensibleString(u->nick));
		if (!reason.empty())
			ci->Extend("suspend_reason", new ExtensibleString(reason));

		if (ci->c)
		{
			for (CUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; )
			{
				UserContainer *uc = *it++;

				if (uc->user->HasMode(UMODE_OPER))
					continue;

				ci->c->Kick(NULL, uc->user, "%s", !reason.empty() ? reason.c_str() : translate(uc->user, _("This channel has been suspended.")));
			}
		}

		Log(LOG_ADMIN, u, this, ci) << (!reason.empty() ? reason : "No reason");
		source.Reply(_("Channel \002%s\002 is now suspended."), ci->name.c_str());

		FOREACH_MOD(I_OnChanSuspend, OnChanSuspend(ci));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Disallows anyone from using the given channel.\n"
				"May be cancelled by using the UNSUSPEND\n"
				"command to preserve all previous channel data/settings.\n"
				" \n"
				"Reason may be required on certain networks."));
		return true;
	}
};

class CommandCSUnSuspend : public Command
{
 public:
	CommandCSUnSuspend(Module *creator) : Command(creator, "chanserv/unsuspend", 1, 1)
	{
		this->SetDesc(_("Releases a suspended channel"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		if (readonly)
			source.Reply(READ_ONLY_MODE);

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		/* Only UNSUSPEND already suspended channels */
		if (!ci->HasFlag(CI_SUSPENDED))
		{
			source.Reply(_("Couldn't release channel \002%s\002!"), ci->name.c_str());
			return;
		}

		Anope::string *by = ci->GetExt<Anope::string *>("suspend_by"), *reason = ci->GetExt<Anope::string *>("suspend_reason");
		if (by != NULL)
			Log(LOG_ADMIN, u, this, ci) << " which was suspended by " << *by << " for: " << (reason && !reason->empty() ? *reason : "No reason");

		ci->UnsetFlag(CI_SUSPENDED);
		ci->Shrink("suspend_by");
		ci->Shrink("suspend_reason");

		source.Reply(_("Channel \002%s\002 is now released."), ci->name.c_str());

		FOREACH_MOD(I_OnChanUnsuspend, OnChanUnsuspend(ci));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Releases a suspended channel. All data and settings\n"
				"are preserved from before the suspension."));
		return true;
	}
};

class CSSuspend : public Module
{
	CommandCSSuspend commandcssuspend;
	CommandCSUnSuspend commandcsunsuspend;

 public:
	CSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssuspend(this), commandcsunsuspend(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSuspend)
