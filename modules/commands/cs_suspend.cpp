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

struct ChanSuspend : ExtensibleItem, Serializable
{
	Anope::string chan;
	time_t when;

	ChanSuspend() : Serializable("ChanSuspend")
	{
	}

	serialized_data serialize()
	{
		serialized_data sd;

		sd["chan"] << this->chan;
		sd["when"] << this->when;

		return sd;
	}

	static void unserialize(serialized_data &sd)
	{
		ChannelInfo *ci = cs_findchan(sd["chan"].astr());
		if (ci == NULL)
			return;

		ChanSuspend *cs = new ChanSuspend();
		
		sd["chan"] >> cs->chan;
		sd["when"] >> cs->when;

		ci->Extend("ci_suspend_expire", cs);
	}
};

class CommandCSSuspend : public Command
{
 public:
	CommandCSSuspend(Module *creator) : Command(creator, "chanserv/suspend", 1, 3)
	{ 
		this->SetDesc(_("Prevent a channel from being used preserving channel data and settings"));
		this->SetSyntax(_("\037channel\037 [+\037expiry\037] [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		Anope::string expiry = params.size() > 1 ? params[1] : "";
		Anope::string reason = params.size() > 2 ? params[2] : "";
		time_t expiry_secs = Config->CSSuspendExpire;

		User *u = source.u;

		if (!expiry.empty() && expiry[0] != '+')
		{
			reason = expiry + " " + reason;
			reason.trim();
			expiry.clear();
		}
		else
			expiry_secs = dotime(expiry);

		if (Config->ForceForbidReason && reason.empty())
		{
			this->OnSyntaxError(source, "");
			return;
		}

		if (readonly)
			source.Reply(READ_ONLY_MODE);

		ChannelInfo *ci = cs_findchan(chan);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
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

		if (expiry_secs > 0)
		{
			ChanSuspend *cs = new ChanSuspend();
			cs->chan = ci->name;
			cs->when = Anope::CurTime + expiry_secs;

			ci->Extend("cs_suspend_expire", cs);
		}

		Log(LOG_ADMIN, u, this, ci) << (!reason.empty() ? reason : "No reason") << ", expires in " << (expiry_secs ? do_strftime(Anope::CurTime + expiry_secs) : "never");
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
				"If an expiry is given the channel will be unsuspended after\n"
				"that period of time, else the default expiry from the"
				"configuration is used.\n"
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

		Anope::string *by = ci->GetExt<ExtensibleString *>("suspend_by"), *reason = ci->GetExt<ExtensibleString *>("suspend_reason");
		if (by != NULL)
			Log(LOG_ADMIN, u, this, ci) << " which was suspended by " << *by << " for: " << (reason && !reason->empty() ? *reason : "No reason");

		ci->UnsetFlag(CI_SUSPENDED);
		ci->Shrink("suspend_by");
		ci->Shrink("suspend_reason");
		ci->Shrink("cs_suspend_expire");

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
	SerializeType chansuspend_type;
	CommandCSSuspend commandcssuspend;
	CommandCSUnSuspend commandcsunsuspend;

 public:
	CSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		chansuspend_type("ChanSuspend", ChanSuspend::unserialize), commandcssuspend(this), commandcsunsuspend(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnPreChanExpire };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	~CSSuspend()
	{
		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
		{
			it->second->Shrink("cs_suspend_expire");
			it->second->Shrink("suspend_by");
			it->second->Shrink("suspend_reason");
		}
	}

	void OnPreChanExpire(ChannelInfo *ci, bool &expire)
	{
		if (!ci->HasFlag(CI_SUSPENDED))
			return;

		expire = false;

		ChanSuspend *cs = ci->GetExt<ChanSuspend *>("cs_suspend_expire");
		if (cs != NULL && cs->when < Anope::CurTime)
		{
			ci->last_used = Anope::CurTime;
			ci->UnsetFlag(CI_SUSPENDED);
			ci->Shrink("cs_suspend_expire");
			ci->Shrink("suspend_by");
			ci->Shrink("suspend_reason");

			Log(LOG_NORMAL, "expire", findbot(Config->ChanServ)) << "Expiring suspend for " << ci->name;
		}
	}
};

MODULE_INIT(CSSuspend)
