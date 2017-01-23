/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
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
#include "modules/nickserv/suspend.h"
#include "modules/nickserv/info.h"
#include "modules/nickserv.h"

class NSSuspendInfoImpl : public NSSuspendInfo
{
	friend class NSSuspendType;

	Serialize::Storage<NickServ::Account *> account;
	Serialize::Storage<Anope::string> by, reason;
	Serialize::Storage<time_t> when, expires;

 public:
	using NSSuspendInfo::NSSuspendInfo;

	NickServ::Account *GetAccount() override;
	void SetAccount(NickServ::Account *) override;

	Anope::string GetBy() override;
	void SetBy(const Anope::string &by) override;

	Anope::string GetReason() override;
	void SetReason(const Anope::string &reason) override;

	time_t GetWhen() override;
	void SetWhen(const time_t &w) override;

	time_t GetExpires() override;
	void SetExpires(const time_t &e) override;
};

class NSSuspendType : public Serialize::Type<NSSuspendInfoImpl>
{
 public:
	Serialize::ObjectField<NSSuspendInfoImpl, NickServ::Account *> account;
	Serialize::Field<NSSuspendInfoImpl, Anope::string> by, reason;
	Serialize::Field<NSSuspendInfoImpl, time_t> when, expires;

	NSSuspendType(Module *me) : Serialize::Type<NSSuspendInfoImpl>(me)
		, account(this, "account", &NSSuspendInfoImpl::account, true)
		, by(this, "by", &NSSuspendInfoImpl::by)
		, reason(this, "reason", &NSSuspendInfoImpl::reason)
		, when(this, "when", &NSSuspendInfoImpl::when)
		, expires(this, "expires", &NSSuspendInfoImpl::expires)
	{
	}
};

NickServ::Account *NSSuspendInfoImpl::GetAccount()
{
	return Get(&NSSuspendType::account);
}

void NSSuspendInfoImpl::SetAccount(NickServ::Account *s)
{
	Set(&NSSuspendType::account, s);
}

Anope::string NSSuspendInfoImpl::GetBy()
{
	return Get(&NSSuspendType::by);
}

void NSSuspendInfoImpl::SetBy(const Anope::string &by)
{
	Set(&NSSuspendType::by, by);
}

Anope::string NSSuspendInfoImpl::GetReason()
{
	return Get(&NSSuspendType::reason);
}

void NSSuspendInfoImpl::SetReason(const Anope::string &reason)
{
	Set(&NSSuspendType::reason, reason);
}

time_t NSSuspendInfoImpl::GetWhen()
{
	return Get(&NSSuspendType::when);
}

void NSSuspendInfoImpl::SetWhen(const time_t &w)
{
	Set(&NSSuspendType::when, w);
}

time_t NSSuspendInfoImpl::GetExpires()
{
	return Get(&NSSuspendType::expires);
}

void NSSuspendInfoImpl::SetExpires(const time_t &e)
{
	Set(&NSSuspendType::expires, e);
}

class CommandNSSuspend : public Command
{
 public:
	CommandNSSuspend(Module *creator) : Command(creator, "nickserv/suspend", 2, 3)
	{
		this->SetDesc(_("Suspend a given nick"));
		this->SetSyntax(_("\037account\037 [+\037expiry\037] [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{

		const Anope::string &nick = params[0];
		Anope::string expiry = params[1];
		Anope::string reason = params.size() > 2 ? params[2] : "";
		time_t expiry_secs = Config->GetModule(this->GetOwner())->Get<time_t>("suspendexpire");

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		if (expiry[0] != '+')
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

		NickServ::Nick *na = NickServ::FindNick(nick);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		if (Config->GetModule("nickserv/main")->Get<bool>("secureadmins", "yes") && na->GetAccount()->GetOper())
		{
			source.Reply(_("You may not suspend other Services Operators' nicknames."));
			return;
		}

		NSSuspendInfo *si = na->GetAccount()->GetRef<NSSuspendInfo *>();
		if (!si)
		{
			source.Reply(_("\002%s\002 is already suspended."), na->GetAccount()->GetDisplay().c_str());
			return;
		}

		NickServ::Account *nc = na->GetAccount();

		si = Serialize::New<NSSuspendInfo *>();
		si->SetAccount(nc);
		si->SetBy(source.GetNick());
		si->SetReason(reason);
		si->SetWhen(Anope::CurTime);
		si->SetExpires(expiry_secs ? expiry_secs + Anope::CurTime : 0);

		for (NickServ::Nick *na2 : nc->GetRefs<NickServ::Nick *>())
		{
			na2->SetLastQuit(reason);

			User *u2 = User::Find(na2->GetNick(), true);
			if (u2)
			{
				u2->Logout();
				if (NickServ::service)
					NickServ::service->Collide(u2, na2);
			}
		}

		logger.Command(LogType::ADMIN, source, _("{source} used {command} for {0} ({1}), expires on {2}"),
				nick, !reason.empty() ? reason : "No reason", expiry_secs ? Anope::strftime(Anope::CurTime + expiry_secs) : "never");
		source.Reply(_("\002{0}\002 is now suspended."), na->GetNick());

		EventManager::Get()->Dispatch(&Event::NickSuspend::OnNickSuspend, na);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
#warning "show expiry"
		source.Reply(_("Suspends \037account\037, which prevents it from being used while keeping all the data for it."
		               " If an expiry is given the account will be unsuspended after that period of time, otherwise the default expiry from the configuration is used."));
		return true;
	}
};

class CommandNSUnSuspend : public Command
{
 public:
	CommandNSUnSuspend(Module *creator) : Command(creator, "nickserv/unsuspend", 1, 1)
	{
		this->SetDesc(_("Unsuspend a given nick"));
		this->SetSyntax(_("\037account\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[0];

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

		NickServ::Nick *na = NickServ::FindNick(nick);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		NSSuspendInfo *si = na->GetAccount()->GetRef<NSSuspendInfo *>();
		if (!si)
		{
			source.Reply(_("\002{0}\002 is not suspended."), na->GetNick());
			return;
		}

		logger.Command(LogType::ADMIN, source, _("{source} used {command} for {0}, which was suspended by {1} for: {2}"),
				!si->GetBy().empty() ? si->GetBy() : "(noone)", !si->GetReason().empty() ? si->GetReason() : "no reason");

		si->Delete();

		source.Reply(_("\002{0}\002 is now released."), na->GetNick());

		EventManager::Get()->Dispatch(&Event::NickUnsuspend::OnNickUnsuspend, na);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Unsuspends \037account\037, which allows it to be used again."));
		return true;
	}
};

class NSSuspend : public Module
	, public EventHook<Event::NickInfo>
	, public EventHook<NickServ::Event::PreNickExpire>
	, public EventHook<NickServ::Event::NickValidate>
{
	CommandNSSuspend commandnssuspend;
	CommandNSUnSuspend commandnsunsuspend;
	std::vector<Anope::string> show;
	NSSuspendType nst;

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
	NSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::NickInfo>(this)
		, EventHook<NickServ::Event::PreNickExpire>(this)
		, EventHook<NickServ::Event::NickValidate>(this)
		, commandnssuspend(this)
		, commandnsunsuspend(this)
		, nst(this)
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Anope::string s = conf->GetModule(this)->Get<Anope::string>("show");
		commasepstream(s).GetTokens(show);
		std::transform(show.begin(), show.end(), show.begin(), trim());
	}

	void OnNickInfo(CommandSource &source, NickServ::Nick *na, InfoFormatter &info, bool show_hidden) override
	{
		NSSuspendInfo *s = na->GetAccount()->GetRef<NSSuspendInfo *>();
		if (!s)
			return;

		if (show_hidden || Show(source, "suspended"))
			info[_("Suspended")] = _("This nickname is \002suspended\002.");
		if (!s->GetBy().empty() && (show_hidden || Show(source, "by")))
			info[_("Suspended by")] = s->GetBy();
		if (!s->GetReason().empty() && (show_hidden || Show(source, "reason")))
			info[_("Suspend reason")] = s->GetReason();
		if (s->GetWhen() && (show_hidden || Show(source, "on")))
			info[_("Suspended on")] = Anope::strftime(s->GetWhen(), source.GetAccount());
		if (s->GetExpires() && (show_hidden || Show(source, "expires")))
			info[_("Suspension expires")] = Anope::strftime(s->GetExpires(), source.GetAccount());
	}

	void OnPreNickExpire(NickServ::Nick *na, bool &expire) override
	{
		NSSuspendInfo *s = na->GetAccount()->GetRef<NSSuspendInfo *>();
		if (!s)
			return;

		expire = false;

		if (!s->GetExpires())
			return;

		if (s->GetExpires() < Anope::CurTime)
		{
			na->SetLastSeen(Anope::CurTime);
			s->Delete();

			logger.Category("nickserv/expire").Bot("NickServ").Log(_("Expiring suspend for {0}"), na->GetNick());
		}
	}

	EventReturn OnNickValidate(User *u, NickServ::Nick *na) override
	{
		NSSuspendInfo *s = na->GetAccount()->GetRef<NSSuspendInfo *>();
		if (!s)
			return EVENT_CONTINUE;

		u->SendMessage(Config->GetClient("NickServ"), _("\002{0}\002 is suspended."), u->nick);
		return EVENT_STOP;
	}
};

MODULE_INIT(NSSuspend)
