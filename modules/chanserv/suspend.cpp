/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/chanserv/suspend.h"
#include "modules/chanserv/drop.h"
#include "modules/chanserv.h"
#include "modules/chanserv/info.h"

class CSSuspendInfoImpl : public CSSuspendInfo
{
	friend class CSSuspendType;

	Serialize::Storage<ChanServ::Channel *> channel;
	Serialize::Storage<Anope::string> by, reason;
	Serialize::Storage<time_t> when, expires;

 public:
	using CSSuspendInfo::CSSuspendInfo;

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

void CSSuspendInfoImpl::SetBy(const Anope::string &b)
{
	Set(&CSSuspendType::by, b);
}

Anope::string CSSuspendInfoImpl::GetReason()
{
	return Get(&CSSuspendType::reason);
}

void CSSuspendInfoImpl::SetReason(const Anope::string &r)
{
	Set(&CSSuspendType::reason, r);
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
			if (expiry_secs < 0)
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

		if (Channel *c = ci->GetChannel())
		{
			std::vector<User *> users;

			for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
			{
				ChanUserContainer *uc = it->second;
				User *user = uc->user;
				if (!user->HasMode("OPER") && user->server != Me)
					users.push_back(user);
			}

			for (unsigned i = 0; i < users.size(); ++i)
				c->Kick(NULL, users[i], !reason.empty() ? reason : Language::Translate(users[i], _("This channel has been suspended.")));
		}

		logger.Admin(source, ci, _("{source} used {command} on {channel} ({0}), expires on {1}"),
				!reason.empty() ? reason : "No reason", expiry_secs ? Anope::strftime(Anope::CurTime + expiry_secs) : "never");

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

		logger.Admin(source, ci, _("{source} used {command} on {channel} which was suspended by {0} for: {1}"),
				si->GetBy(), !si->GetReason().empty() ? si->GetReason() : "No reason");

		si->Delete();

		source.Reply(_("Channel \002{0}\002 is now released."), ci->GetName());

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

			logger.Channel(ci).Log(_("Expiring suspend for {0}"), ci->GetName());
		}
	}

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) override
	{
		ChanServ::Channel *ci = c->GetChannel();
		if (u->HasMode("OPER") || !ci || !ci->GetRef<CSSuspendInfo *>())
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
