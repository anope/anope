/* ChanServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/suspend.h"
#include "modules/cs_drop.h"
#include "modules/chanserv.h"
#include "modules/cs_info.h"

struct CSSuspendInfo : SuspendInfo, Serializable
{
	CSSuspendInfo(Extensible *) : Serializable("CSSuspendInfo") { }

	void Serialize(Serialize::Data &data) const override
	{
		data["chan"] << what;
		data["by"] << by;
		data["reason"] << reason;
		data["time"] << when;
		data["expires"] << expires;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data)
	{
		Anope::string schan;
		data["chan"] >> schan;

		CSSuspendInfo *si;
		if (obj)
			si = anope_dynamic_static_cast<CSSuspendInfo *>(obj);
		else
		{
			ChanServ::Channel *ci = ChanServ::Find(schan);
			if (!ci)
				return NULL;
			si = ci->Extend<CSSuspendInfo>("CS_SUSPENDED");
			data["chan"] >> si->what;
		}

		data["by"] >> si->by;
		data["reason"] >> si->reason;
		data["time"] >> si->when;
		data["expires"] >> si->expires;
		return si;
	}
};

class CommandCSSuspend : public Command
{
	EventHandlers<Event::ChanSuspend> &onchansuspend;
 public:
	CommandCSSuspend(Module *creator, EventHandlers<Event::ChanSuspend> &event) : Command(creator, "chanserv/suspend", 2, 3), onchansuspend(event)
	{
		this->SetDesc(_("Prevent a channel from being used preserving channel data and settings"));
		this->SetSyntax(_("\037channel\037 [+\037expiry\037] [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
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
		{
			expiry_secs = Anope::DoTime(expiry);
			if (expiry_secs == -1)
			{
				source.Reply(_("Invalid expiry time \002{0}\002."), expiry);
				return;
			}
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (ci->HasExt("CS_SUSPENDED"))
		{
			source.Reply(_("\002{0}\002 is already suspended."), ci->name);
			return;
		}

		CSSuspendInfo *si = ci->Extend<CSSuspendInfo>("CS_SUSPENDED");
		si->what = ci->name;
		si->by = source.GetNick();
		si->reason = reason;
		si->when = Anope::CurTime;
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

		Log(LOG_ADMIN, source, this, ci) << "(" << (!reason.empty() ? reason : "No reason") << "), expires on " << (expiry_secs ? Anope::strftime(Anope::CurTime + expiry_secs) : "never");
		source.Reply(_("Channel \002{0}\002 is now suspended."), ci->name);

		this->onchansuspend(&Event::ChanSuspend::OnChanSuspend, ci);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Disallows anyone from using the given channel."
		               " All channel settings are preserved while the channel is suspended."
		               "If \037expiry\037 is given the channel will be unsuspended after that period of time."));
		return true;
	}
};

class CommandCSUnSuspend : public Command
{
	EventHandlers<Event::ChanUnsuspend> &onchanunsuspend;

 public:
	CommandCSUnSuspend(Module *creator, EventHandlers<Event::ChanUnsuspend> &event) : Command(creator, "chanserv/unsuspend", 1, 1), onchanunsuspend(event)
	{
		this->SetDesc(_("Releases a suspended channel"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		/* Only UNSUSPEND already suspended channels */
		CSSuspendInfo *si = ci->GetExt<CSSuspendInfo>("CS_SUSPENDED");
		if (!si)
		{
			source.Reply(_("Channel \002{0}\002 isn't suspended."), ci->name.c_str());
			return;
		}

		Log(LOG_ADMIN, source, this, ci) << "which was suspended by " << si->by << " for: " << (!si->reason.empty() ? si->reason : "No reason");

		ci->Shrink<CSSuspendInfo>("CS_SUSPENDED");

		source.Reply(_("Channel \002%s\002 is now released."), ci->name.c_str());

		this->onchanunsuspend(&Event::ChanUnsuspend::OnChanUnsuspend, ci);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Releases a suspended channel. All data and settings are preserved from before the suspension."));
		return true;
	}
};

class CSSuspend : public Module
	, public EventHook<Event::ChanInfo>
	, public EventHook<ChanServ::Event::PreChanExpire>
	, public EventHook<Event::CheckKick>
	, public EventHook<Event::ChanDrop>
{
	CommandCSSuspend commandcssuspend;
	CommandCSUnSuspend commandcsunsuspend;
	ExtensibleItem<CSSuspendInfo> suspend;
	Serialize::Type suspend_type;
	EventHandlers<Event::ChanSuspend> onchansuspend;
	EventHandlers<Event::ChanUnsuspend> onchanunsuspend;

 public:
	CSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ChanInfo>("OnChanInfo")
		, EventHook<ChanServ::Event::PreChanExpire>("OnPreChanExpire")
		, EventHook<Event::CheckKick>("OnCheckKick")
		, EventHook<Event::ChanDrop>("OnChanDrop")
		, commandcssuspend(this, onchansuspend)
		, commandcsunsuspend(this, onchanunsuspend)
		, suspend(this, "CS_SUSPENDED")
		, suspend_type("CSSuspendInfo", CSSuspendInfo::Unserialize)
		, onchansuspend(this, "OnChanSuspend")
		, onchanunsuspend(this, "OnChanUnsuspend")
	{
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool show_hidden) override
	{
		CSSuspendInfo *si = suspend.Get(ci);
		if (si)
		{
			info[_("Suspended")] = _("This channel is \002suspended\002.");
			if (!si->by.empty())
				info[_("Suspended by")] = si->by;
			if (!si->reason.empty())
				info[_("Suspend reason")] = si->reason;
			if (si->when)
				info[_("Suspended on")] = Anope::strftime(si->when, source.GetAccount(), true);
			if (si->expires)
				info[_("Suspension expires")] = Anope::strftime(si->expires, source.GetAccount(), true);
		}
	}

	void OnPreChanExpire(ChanServ::Channel *ci, bool &expire) override
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

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) override
	{
		if (u->HasMode("OPER") || !c->ci || !suspend.HasExt(c->ci))
			return EVENT_CONTINUE;

		reason = Language::Translate(u, _("This channel may not be used."));
		return EVENT_STOP;
	}

	EventReturn OnChanDrop(CommandSource &source, ChanServ::Channel *ci) override
	{
		CSSuspendInfo *si = suspend.Get(ci);
		if (si && !source.HasCommand("chanserv/drop"))
		{
			source.Reply(_("Channel \002{0}\002 is currently suspended."), ci->name);
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(CSSuspend)
