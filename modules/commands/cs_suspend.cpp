/* ChanServ core functions
 *
 * (C) 2003-2013 Anope Team
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
	CommandCSSuspend(Module *creator) : Command(creator, "chanserv/suspend", 1, 3)
	{ 
		this->SetDesc(_("Prevent a channel from being used preserving channel data and settings"));
		this->SetSyntax(_("\037channel\037 [+\037expiry\037] \037reason\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &chan = params[0];
		Anope::string expiry = params.size() > 1 ? params[1] : "";
		Anope::string reason = params.size() > 2 ? params[2] : "";
		time_t expiry_secs = Config->GetModule(this->owner)->Get<time_t>("expire");

		if (!expiry.empty() && expiry[0] != '+')
		{
			reason = expiry + " " + reason;
			reason.trim();
			expiry.clear();
		}
		else
			expiry_secs = Anope::DoTime(expiry);

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

		ChannelInfo *ci = ChannelInfo::Find(chan);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
			return;
		}

		ci->ExtendMetadata("SUSPENDED");
		ci->ExtendMetadata("suspend:by", source.GetNick());
		if (!reason.empty())
			ci->ExtendMetadata("suspend:reason", reason);
		ci->ExtendMetadata("suspend:time", stringify(Anope::CurTime));

		if (ci->c)
		{
			std::vector<User *> users;

			for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
			{
				ChanUserContainer *uc = it->second;
				User *user = uc->user;
				if (!user->HasMode("OPER") && user->server != Me)
					users.push_back(user);
			}

			for (unsigned i = 0; i < users.size(); ++i)
				ci->c->Kick(NULL, users[i], "%s", !reason.empty() ? reason.c_str() : Language::Translate(users[i], _("This channel has been suspended.")));
		}

		if (expiry_secs > 0)
			ci->ExtendMetadata("suspend:expire", stringify(Anope::CurTime + expiry_secs));

		Log(LOG_ADMIN, source, this, ci) << (!reason.empty() ? reason : "No reason") << ", expires in " << (expiry_secs ? Anope::strftime(Anope::CurTime + expiry_secs) : "never");
		source.Reply(_("Channel \002%s\002 is now suspended."), ci->name.c_str());

		FOREACH_MOD(I_OnChanSuspend, OnChanSuspend(ci));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Disallows anyone from using the given channel.\n"
				"May be cancelled by using the \002UNSUSPEND\002\n"
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		/* Only UNSUSPEND already suspended channels */
		if (!ci->HasExt("SUSPENDED"))
		{
			source.Reply(_("Channel \002%s\002 isn't suspended."), ci->name.c_str());
			return;
		}

		Anope::string *by = ci->GetExt<ExtensibleItemClass<Anope::string> *>("suspend:by"), *reason = ci->GetExt<ExtensibleItemClass<Anope::string> *>("suspend:reason");
		if (by != NULL)
			Log(LOG_ADMIN, source, this, ci) << " which was suspended by " << *by << " for: " << (reason && !reason->empty() ? *reason : "No reason");

		ci->Shrink("SUSPENDED");
		ci->Shrink("suspend:by");
		ci->Shrink("suspend:reason");
		ci->Shrink("suspend:expire");
		ci->Shrink("suspend:time");

		source.Reply(_("Channel \002%s\002 is now released."), ci->name.c_str());

		FOREACH_MOD(I_OnChanUnsuspend, OnChanUnsuspend(ci));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
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
	CSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandcssuspend(this), commandcsunsuspend(this)
	{

		Implementation i[] = { I_OnChanInfo, I_OnPreChanExpire, I_OnCheckKick };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnChanInfo(CommandSource &source, ChannelInfo *ci, InfoFormatter &info, bool show_hidden) anope_override
	{
		if (ci->HasExt("SUSPENDED"))
		{
			Anope::string *by = ci->GetExt<ExtensibleItemClass<Anope::string> *>("suspend:by"), *reason = ci->GetExt<ExtensibleItemClass<Anope::string> *>("suspend:reason"), *t = ci->GetExt<ExtensibleItemClass<Anope::string> *>("suspend:time");
			info["Suspended"] = "This channel is \2suspended\2.";
			if (by)
				info["Suspended by"] = *by;
			if (reason)
				info["Suspend reason"] = *reason;
			if (t)
				info["Suspended on"] = Anope::strftime(convertTo<time_t>(*t), source.GetAccount(), true);
		}
	}

	void OnPreChanExpire(ChannelInfo *ci, bool &expire) anope_override
	{
		if (!ci->HasExt("SUSPENDED"))
			return;

		expire = false;

		Anope::string *str = ci->GetExt<ExtensibleItemClass<Anope::string> *>("suspend:expire");
		if (str == NULL)
			return;

		try
		{
			time_t when = convertTo<time_t>(*str);
			if (when < Anope::CurTime)
			{
				ci->last_used = Anope::CurTime;
				ci->Shrink("SUSPENDED");
				ci->Shrink("suspend:expire");
				ci->Shrink("suspend:by");
				ci->Shrink("suspend:reason");
				ci->Shrink("suspend:time");

				Log(LOG_NORMAL, "expire", ChanServ) << "Expiring suspend for " << ci->name;
			}
		}
		catch (const ConvertException &) { }
	}

	EventReturn OnCheckKick(User *u, ChannelInfo *ci, Anope::string &mask, Anope::string &reason) anope_override
	{
		if (u->HasMode("OPER") || !ci->HasExt("SUSPENDED"))
			return EVENT_CONTINUE;

		reason = Language::Translate(u, _("This channel may not be used."));
		return EVENT_STOP;
	}
};

MODULE_INIT(CSSuspend)
