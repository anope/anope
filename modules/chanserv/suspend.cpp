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
#include "modules/chanserv/suspend.h"
#include "modules/chanserv/drop.h"
#include "modules/chanserv.h"
#include "modules/chanserv/info.h"

class CSSuspendInfoImpl : public CSSuspendInfo
{
	friend class CSSuspendType;

	ChanServ::Channel *channel = nullptr;
	Anope::string by, reason;
	time_t when = 0, expires = 0;

 public:
	CSSuspendInfoImpl(Serialize::TypeBase *type) : CSSuspendInfo(type) { }
	CSSuspendInfoImpl(Serialize::TypeBase *type, Serialize::ID id) : CSSuspendInfo(type, id) { }

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *s) override;

	Anope::string GetBy() override;
	void SetBy(const Anope::string &by) override;

	Anope::string GetReason() override;
	void SetReason(const Anope::string &reason) override;

	time_t GetWhen() override;
	void SetWhen(const time_t &w) override;

	time_t GetExpires() override;
	void SetExpires(const time_t &e) override;
};

class CSSuspendType : public Serialize::Type<CSSuspendInfoImpl>
{
 public:
	Serialize::ObjectField<CSSuspendInfoImpl, ChanServ::Channel *> channel;
	Serialize::Field<CSSuspendInfoImpl, Anope::string> by, reason;
	Serialize::Field<CSSuspendInfoImpl, time_t> when, expires;

	CSSuspendType(Module *me) : Serialize::Type<CSSuspendInfoImpl>(me)
		, channel(this, "chan", &CSSuspendInfoImpl::channel, true)
		, by(this, "by", &CSSuspendInfoImpl::by)
		, reason(this, "reason", &CSSuspendInfoImpl::reason)
		, when(this, "time", &CSSuspendInfoImpl::when)
		, expires(this, "expires", &CSSuspendInfoImpl::expires)
	{
	}
};

ChanServ::Channel *CSSuspendInfoImpl::GetChannel()
{
	return Get(&CSSuspendType::channel);
}

void CSSuspendInfoImpl::SetChannel(ChanServ::Channel *s)
{
	Set(&CSSuspendType::channel, s);
}

Anope::string CSSuspendInfoImpl::GetBy()
{
	return Get(&CSSuspendType::by);
}

void CSSuspendInfoImpl::SetBy(const Anope::string &by)
{
	Set(&CSSuspendType::by, by);
}

Anope::string CSSuspendInfoImpl::GetReason()
{
	return Get(&CSSuspendType::reason);
}

void CSSuspendInfoImpl::SetReason(const Anope::string &reason)
{
	Set(&CSSuspendType::reason, reason);
}

time_t CSSuspendInfoImpl::GetWhen()
{
	return Get(&CSSuspendType::when);
}

void CSSuspendInfoImpl::SetWhen(const time_t &w)
{
	Set(&CSSuspendType::when, w);
}

time_t CSSuspendInfoImpl::GetExpires()
{
	return Get(&CSSuspendType::expires);
}

void CSSuspendInfoImpl::SetExpires(const time_t &e)
{
	Set(&CSSuspendType::expires, e);
}

class CommandCSSuspend : public Command
{
 public:
	CommandCSSuspend(Module *creator) : Command(creator, "chanserv/suspend", 2, 3)
	{
		this->SetDesc(_("Prevent a channel from being used preserving channel data and settings"));
		this->SetSyntax(_("\037channel\037 [+\037expiry\037] [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		Anope::string expiry = params[1];
		Anope::string reason = params.size() > 2 ? params[2] : "";
		time_t expiry_secs = Config->GetModule(this->GetOwner())->Get<time_t>("expire");

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

		CSSuspendInfo *si = ci->GetRef<CSSuspendInfo *>();
		if (si)
		{
			source.Reply(_("\002{0}\002 is already suspended."), ci->GetName());
			return;
		}

		si = Serialize::New<CSSuspendInfo *>();
		si->SetChannel(ci);
		si->SetBy(source.GetNick());
		si->SetReason(reason);
		si->SetWhen(Anope::CurTime);
		si->SetExpires(expiry_secs ? expiry_secs + Anope::CurTime : 0);

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
		source.Reply(_("Channel \002{0}\002 is now suspended."), ci->GetName());

		EventManager::Get()->Dispatch(&Event::ChanSuspend::OnChanSuspend, ci);
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
 public:
	CommandCSUnSuspend(Module *creator) : Command(creator, "chanserv/unsuspend", 1, 1)
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

		CSSuspendInfo *si = ci->GetRef<CSSuspendInfo *>();
		if (!si)
		{
			source.Reply(_("Channel \002{0}\002 isn't suspended."), ci->GetName());
			return;
		}

		Log(LOG_ADMIN, source, this, ci) << "which was suspended by " << si->GetBy() << " for: " << (!si->GetReason().empty() ? si->GetReason() : "No reason");

		si->Delete();

		source.Reply(_("Channel \002%s\002 is now released."), ci->GetName().c_str());

		EventManager::Get()->Dispatch(&Event::ChanUnsuspend::OnChanUnsuspend, ci);
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
	std::vector<Anope::string> show;
	CSSuspendType cst;

	struct trim
	{
		Anope::string operator()(Anope::string s) const
		{
			return s.trim();
		}
	};

	bool Show(CommandSource &source, const Anope::string &what) const
	{
		return source.IsOper() || std::find(show.begin(), show.end(), what) != show.end();
	}

 public:
	CSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ChanInfo>(this)
		, EventHook<ChanServ::Event::PreChanExpire>(this)
		, EventHook<Event::CheckKick>(this)
		, EventHook<Event::ChanDrop>(this)
		, commandcssuspend(this)
		, commandcsunsuspend(this)
		, cst(this)
	{
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool show_hidden) override
	{
		CSSuspendInfo *si = ci->GetRef<CSSuspendInfo *>();
		if (!si)
			return;

		if (show_hidden || Show(source, "suspended"))
			info[_("Suspended")] = _("This channel is \002suspended\002.");
		if (!si->GetBy().empty() && (show_hidden || Show(source, "by")))
			info[_("Suspended by")] = si->GetBy();
		if (!si->GetReason().empty() && (show_hidden || Show(source, "reason")))
			info[_("Suspend reason")] = si->GetReason();
		if (si->GetWhen() && (show_hidden || Show(source, "on")))
			info[_("Suspended on")] = Anope::strftime(si->GetWhen(), source.GetAccount());
		if (si->GetExpires() && (show_hidden || Show(source, "expires")))
			info[_("Suspension expires")] = Anope::strftime(si->GetExpires(), source.GetAccount());
	}

	void OnPreChanExpire(ChanServ::Channel *ci, bool &expire) override
	{
		CSSuspendInfo *si = ci->GetRef<CSSuspendInfo *>();
		if (!si)
			return;

		expire = false;

		if (!si->GetExpires())
			return;

		if (si->GetExpires() < Anope::CurTime)
		{
			ci->SetLastUsed(Anope::CurTime);
			si->Delete();

			Log(this) << "Expiring suspend for " << ci->GetName();
		}
	}

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) override
	{
		if (u->HasMode("OPER") || !c->ci || !c->ci->GetRef<CSSuspendInfo *>())
			return EVENT_CONTINUE;

		reason = Language::Translate(u, _("This channel may not be used."));
		return EVENT_STOP;
	}

	EventReturn OnChanDrop(CommandSource &source, ChanServ::Channel *ci) override
	{
		CSSuspendInfo *si = ci->GetRef<CSSuspendInfo *>();
		if (si && !source.HasCommand("chanserv/drop"))
		{
			source.Reply(_("Channel \002{0}\002 is currently suspended."), ci->GetName());
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(CSSuspend)
