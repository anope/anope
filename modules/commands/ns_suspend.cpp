/* NickServ core functions
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
#include "modules/ns_suspend.h"
#include "modules/ns_info.h"
#include "modules/nickserv.h"

class NSSuspendInfoImpl : public NSSuspendInfo
{
 public:
	NSSuspendInfoImpl(Serialize::TypeBase *type) : NSSuspendInfo(type) { }
	NSSuspendInfoImpl(Serialize::TypeBase *type, Serialize::ID id) : NSSuspendInfo(type, id) { }

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

	NSSuspendType(Module *me) : Serialize::Type<NSSuspendInfoImpl>(me, "NSSuspendInfo")
		, account(this, "nick", true)
		, by(this, "by")
		, reason(this, "reason")
		, when(this, "time")
		, expires(this, "expires")
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
	EventHandlers<Event::NickSuspend> &onnicksuspend;

 public:
	CommandNSSuspend(Module *creator, EventHandlers<Event::NickSuspend> &event) : Command(creator, "nickserv/suspend", 2, 3), onnicksuspend(event)
	{
		this->SetDesc(_("Suspend a given nick"));
		this->SetSyntax(_("\037account\037 [+\037expiry\037] [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{

		const Anope::string &nick = params[0];
		Anope::string expiry = params[1];
		Anope::string reason = params.size() > 2 ? params[2] : "";
		time_t expiry_secs = Config->GetModule(this->owner)->Get<time_t>("suspendexpire");

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

		if (Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && na->GetAccount()->IsServicesOper())
		{
			source.Reply(_("You may not suspend other Services Operators' nicknames."));
			return;
		}

		NSSuspendInfo *si = na->GetAccount()->GetRef<NSSuspendInfo *>(nssuspendinfo);
		if (!si)
		{
			source.Reply(_("\002%s\002 is already suspended."), na->GetAccount()->GetDisplay().c_str());
			return;
		}

		NickServ::Account *nc = na->GetAccount();

		si = nssuspendinfo.Create();
		si->SetAccount(nc);
		si->SetBy(source.GetNick());
		si->SetReason(reason);
		si->SetWhen(Anope::CurTime);
		si->SetExpires(expiry_secs ? expiry_secs + Anope::CurTime : 0);

		for (NickServ::Nick *na2 : nc->GetRefs<NickServ::Nick *>(NickServ::nick))
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

		Log(LOG_ADMIN, source, this) << "for " << nick << " (" << (!reason.empty() ? reason : "No reason") << "), expires on " << (expiry_secs ? Anope::strftime(Anope::CurTime + expiry_secs) : "never");
		source.Reply(_("\002{0}\002 is now suspended."), na->GetNick());

		this->onnicksuspend(&Event::NickSuspend::OnNickSuspend, na);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Suspends \037account\037, which prevents it from being used while keeping all the data for it."
		               " If an expiry is given the account will be unsuspended after that period of time, otherwise the default expiry from the configuration is used.")); // XXX
		return true;
	}
};

class CommandNSUnSuspend : public Command
{
	EventHandlers<Event::NickUnsuspended> &onnickunsuspend;

 public:
	CommandNSUnSuspend(Module *creator, EventHandlers<Event::NickUnsuspended> &event) : Command(creator, "nickserv/unsuspend", 1, 1), onnickunsuspend(event)
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

		NSSuspendInfo *si = na->GetAccount()->GetRef<NSSuspendInfo *>(nssuspendinfo);
		if (!si)
		{
			source.Reply(_("\002{0}\002 is not suspended."), na->GetNick());
			return;
		}

		Log(LOG_ADMIN, source, this) << "for " << na->GetNick() << " which was suspended by " << (!si->GetBy().empty() ? si->GetBy() : "(none)") << " for: " << (!si->GetReason().empty() ? si->GetReason() : "No reason");

		si->Delete();

		source.Reply(_("\002{0}\002 is now released."), na->GetNick());

		this->onnickunsuspend(&Event::NickUnsuspended::OnNickUnsuspended, na);
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
	EventHandlers<Event::NickSuspend> onnicksuspend;
	EventHandlers<Event::NickUnsuspended> onnickunsuspend;
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
		, EventHook<Event::NickInfo>("OnNickInfo")
		, EventHook<NickServ::Event::PreNickExpire>("OnPreNickExpire")
		, EventHook<NickServ::Event::NickValidate>("OnNickValidate")
		, commandnssuspend(this, onnicksuspend)
		, commandnsunsuspend(this, onnickunsuspend)
		, onnicksuspend(this, "OnNickSuspend")
		, onnickunsuspend(this, "OnNickUnsuspended")
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
		NSSuspendInfo *s = na->GetAccount()->GetRef<NSSuspendInfo *>(nssuspendinfo);
		if (!s)
			return;

		if (show_hidden || Show(source, "suspended"))
			info[_("Suspended")] = _("This nickname is \002suspended\002.");
		if (!s->GetBy().empty() && (show_hidden || Show(source, "by")))
			info[_("Suspended by")] = s->GetBy();
		if (!s->GetReason().empty() && (show_hidden || Show(source, "reason")))
			info[_("Suspend reason")] = s->GetReason();
		if (s->GetWhen() && (show_hidden || Show(source, "on")))
			info[_("Suspended on")] = Anope::strftime(s->GetWhen(), source.GetAccount(), true);
		if (s->GetExpires() && (show_hidden || Show(source, "expires")))
			info[_("Suspension expires")] = Anope::strftime(s->GetExpires(), source.GetAccount(), true);
	}

	void OnPreNickExpire(NickServ::Nick *na, bool &expire) override
	{
		NSSuspendInfo *s = na->GetAccount()->GetRef<NSSuspendInfo *>(nssuspendinfo);
		if (!s)
			return;

		expire = false;

		if (!s->GetExpires())
			return;

		if (s->GetExpires() < Anope::CurTime)
		{
			na->SetLastSeen(Anope::CurTime);
			s->Delete();

			Log(LOG_NORMAL, "nickserv/expire", Config->GetClient("NickServ")) << "Expiring suspend for " << na->GetNick();
		}
	}

	EventReturn OnNickValidate(User *u, NickServ::Nick *na) override
	{
		NSSuspendInfo *s = na->GetAccount()->GetRef<NSSuspendInfo *>(nssuspendinfo);
		if (!s)
			return EVENT_CONTINUE;

		u->SendMessage(Config->GetClient("NickServ"), _("\002{0}\002 is suspended."), u->nick);
		return EVENT_STOP;
	}
};

MODULE_INIT(NSSuspend)
