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

#include "module.h"
#include "modules/cs_suspend.h"

struct CSSuspendInfoImpl : CSSuspendInfo, Serializable
{
	CSSuspendInfoImpl(Extensible *) : Serializable("CSSuspendInfo") { }

	void Serialize(Serialize::Data &data) const anope_override
	{
		data["chan"] << chan;
		data["by"] << by;
		data["reason"] << reason;
		data["time"] << time;
		data["expires"] << expires;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data)
	{
		Anope::string schan;
		data["chan"] >> schan;

		CSSuspendInfoImpl *si;
		if (obj)
			si = anope_dynamic_static_cast<CSSuspendInfoImpl *>(obj);
		else
		{
			ChannelInfo *ci = ChannelInfo::Find(schan);
			if (!ci)
				return NULL;
			si = ci->Extend<CSSuspendInfoImpl>("CS_SUSPENDED");
			data["chan"] >> si->chan;
		}

		data["by"] >> si->by;
		data["reason"] >> si->reason;
		data["time"] >> si->time;
		data["expires"] >> si->expires;
		return si;
	}
};

class CommandCSSuspend : public Command
{
 public:
	CommandCSSuspend(Module *creator) : Command(creator, "chanserv/suspend", 2, 3)
	{ 
		this->SetDesc(_("Prevent a channel from being used preserving channel data and settings"));
		this->SetSyntax(_("\037channel\037 [+\037expiry\037] \037reason\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &chan = params[0];
		Anope::string expiry = params[1];
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

		if (ci->HasExt("CS_SUSPENDED"))
		{
			source.Reply(_("\2%s\2 is already suspended."), ci->name.c_str());
			return;
		}

		CSSuspendInfo *si = ci->Extend<CSSuspendInfo>("CS_SUSPENDED");
		si->chan = ci->name;
		si->by = source.GetNick();
		si->reason = reason;
		si->time = Anope::CurTime;
		si->expires = expiry_secs ? expiry_secs + Anope::CurTime : 0;

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

		Log(LOG_ADMIN, source, this, ci) << (!reason.empty() ? reason : "No reason") << ", expires in " << (expiry_secs ? Anope::strftime(Anope::CurTime + expiry_secs) : "never");
		source.Reply(_("Channel \002%s\002 is now suspended."), ci->name.c_str());

		FOREACH_MOD(OnChanSuspend, (ci));
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
		CSSuspendInfo *si = ci->GetExt<CSSuspendInfo>("CS_SUSPENDED");
		if (!si)
		{
			source.Reply(_("Channel \002%s\002 isn't suspended."), ci->name.c_str());
			return;
		}

		Log(LOG_ADMIN, source, this, ci) << " which was suspended by " << si->by << " for: " << (!si->reason.empty() ? si->reason : "No reason");

		ci->Shrink<CSSuspendInfo>("CS_SUSPENDED");

		source.Reply(_("Channel \002%s\002 is now released."), ci->name.c_str());

		FOREACH_MOD(OnChanUnsuspend, (ci));

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
	ExtensibleItem<CSSuspendInfoImpl> suspend;
	Serialize::Type suspend_type;

 public:
	CSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandcssuspend(this), commandcsunsuspend(this), suspend(this, "CS_SUSPENDED"),
		suspend_type("CSSuspendInfo", CSSuspendInfoImpl::Unserialize)
	{
	}

	void OnChanInfo(CommandSource &source, ChannelInfo *ci, InfoFormatter &info, bool show_hidden) anope_override
	{
		CSSuspendInfo *si = suspend.Get(ci);
		if (si)
		{
			info["Suspended"] = "This channel is \2suspended\2.";
			if (!si->by.empty())
				info["Suspended by"] = si->by;
			if (!si->reason.empty())
				info["Suspend reason"] = si->reason;
			if (si->time)
				info["Suspended on"] = Anope::strftime(si->time, source.GetAccount(), true);
			if (si->expires)
				info["Suspended expires"] = Anope::strftime(si->expires, source.GetAccount(), true);
		}
	}

	void OnPreChanExpire(ChannelInfo *ci, bool &expire) anope_override
	{
		CSSuspendInfo *si = suspend.Get(ci);
		if (!si)
			return;

		expire = false;

		if (!si->expires)
			return;

		if (si->expires < Anope::CurTime)
		{
			ci->last_used = Anope::CurTime;
			suspend.Unset(ci);

			Log(this) << "Expiring suspend for " << ci->name;
		}
	}

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) anope_override
	{
		if (u->HasMode("OPER") || !c->ci || !suspend.HasExt(c->ci))
			return EVENT_CONTINUE;

		reason = Language::Translate(u, _("This channel may not be used."));
		return EVENT_STOP;
	}

	EventReturn OnChanDrop(CommandSource &source, ChannelInfo *ci) anope_override
	{
		CSSuspendInfo *si = suspend.Get(ci);
		if (si && !source.HasCommand("chanserv/drop"))
		{
			source.Reply(CHAN_X_SUSPENDED, ci->name.c_str());
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(CSSuspend)
