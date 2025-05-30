/* NickServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/suspend.h"

static ServiceReference<NickServService> nickserv("NickServService", "NickServ");

struct NSSuspendInfo final
	: SuspendInfo
	, Serializable
{
	NSSuspendInfo(Extensible *) : Serializable("NSSuspendInfo") { }
};

struct NSSuspendInfoType final
	: Serialize::Type
{
	NSSuspendInfoType()
		: Serialize::Type("NSSuspendInfo")
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *si = static_cast<const NSSuspendInfo *>(obj);
		data.Store("nick", si->what);
		data.Store("by", si->by);
		data.Store("reason", si->reason);
		data.Store("time", si->when);
		data.Store("expires", si->expires);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		Anope::string snick;
		data["nick"] >> snick;

		NSSuspendInfo *si;
		if (obj)
			si = anope_dynamic_static_cast<NSSuspendInfo *>(obj);
		else
		{
			NickAlias *na = NickAlias::Find(snick);
			if (!na)
				return NULL;
			si = na->nc->Extend<NSSuspendInfo>("NS_SUSPENDED");
			data["nick"] >> si->what;
		}

		data["by"] >> si->by;
		data["reason"] >> si->reason;
		data["time"] >> si->when;
		data["expires"] >> si->expires;
		return si;
	}
};

class CommandNSSuspend final
	: public Command
{
public:
	CommandNSSuspend(Module *creator) : Command(creator, "nickserv/suspend", 2, 3)
	{
		this->SetDesc(_("Suspend a given nick"));
		this->SetSyntax(_("\037nickname\037 [+\037expiry\037] [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{

		const Anope::string &nick = params[0];
		Anope::string expiry = params[1];
		Anope::string reason = params.size() > 2 ? params[2] : "";
		time_t expiry_secs = Config->GetModule(this->owner).Get<time_t>("suspendexpire");

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

		if (expiry[0] != '+')
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
				source.Reply(BAD_EXPIRY_TIME);
				return;
			}
		}

		NickAlias *na = NickAlias::Find(nick);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
		}

		if (Config->GetModule("nickserv").Get<bool>("secureadmins", "yes") && na->nc->IsServicesOper())
		{
			source.Reply(_("You may not suspend other Services Operators' nicknames."));
			return;
		}

		if (na->nc->HasExt("NS_SUSPENDED"))
		{
			source.Reply(_("\002%s\002 is already suspended."), na->nc->display.c_str());
			return;
		}

		NickCore *nc = na->nc;

		NSSuspendInfo *si = nc->Extend<NSSuspendInfo>("NS_SUSPENDED");
		si->what = nc->display;
		si->by = source.GetNick();
		si->reason = reason;
		si->when = Anope::CurTime;
		si->expires = expiry_secs ? expiry_secs + Anope::CurTime : 0;

		for (auto *na2 : *nc->aliases)
		{
			if (na2 && *na2->nc == *na->nc)
			{
				na2->last_quit = reason;

				User *u2 = User::Find(na2->nick, true);
				if (u2)
				{
					IRCD->SendLogout(u2);
					u2->RemoveMode(source.service, "REGISTERED");
					u2->Logout();
					if (nickserv)
						nickserv->Collide(u2, na2);
				}
			}
		}

		Log(LOG_ADMIN, source, this) << "for " << nick << " (" << (!reason.empty() ? reason : "No reason") << "), expires on " << (expiry_secs ? Anope::strftime(Anope::CurTime + expiry_secs) : "never");
		source.Reply(_("Nick %s is now suspended."), nick.c_str());

		FOREACH_MOD(OnNickSuspend, (na));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Suspends a registered nickname, which prevents it from being used "
			"while keeping all the data for that nick. If an expiry is given "
			"the nick will be unsuspended after that period of time, else the "
			"default expiry from the configuration is used."
		));
		return true;
	}
};

class CommandNSUnSuspend final
	: public Command
{
public:
	CommandNSUnSuspend(Module *creator) : Command(creator, "nickserv/unsuspend", 1, 1)
	{
		this->SetDesc(_("Unsuspend a given nick"));
		this->SetSyntax(_("\037nickname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[0];

		if (Anope::ReadOnly)
			source.Reply(READ_ONLY_MODE);

		NickAlias *na = NickAlias::Find(nick);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
		}

		if (!na->nc->HasExt("NS_SUSPENDED"))
		{
			source.Reply(_("Nick %s is not suspended."), na->nick.c_str());
			return;
		}

		NSSuspendInfo *si = na->nc->GetExt<NSSuspendInfo>("NS_SUSPENDED");

		Log(LOG_ADMIN, source, this) << "for " << na->nick << " which was suspended by " << (!si->by.empty() ? si->by : "(none)") << " for: " << (!si->reason.empty() ? si->reason : "No reason");

		na->nc->Shrink<NSSuspendInfo>("NS_SUSPENDED");

		source.Reply(_("Nick %s is now released."), nick.c_str());

		FOREACH_MOD(OnNickUnsuspended, (na));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Unsuspends a nickname which allows it to be used again."));
		return true;
	}
};

class NSSuspend final
	: public Module
{
	CommandNSSuspend commandnssuspend;
	CommandNSUnSuspend commandnsunsuspend;
	ExtensibleItem<NSSuspendInfo> suspend;
	NSSuspendInfoType suspend_type;
	std::vector<Anope::string> show;

	struct trim final
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

	void Expire(NickAlias *na)
	{
		suspend.Unset(na->nc);
		Log(LOG_NORMAL, "nickserv/expire", Config->GetClient("NickServ")) << "Expiring suspend for " << na->nick;
	}

public:
	NSSuspend(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandnssuspend(this)
		, commandnsunsuspend(this)
		, suspend(this, "NS_SUSPENDED")
	{
	}

	void OnReload(Configuration::Conf &conf) override
	{
		Anope::string s = conf.GetModule(this).Get<Anope::string>("show");
		commasepstream(s).GetTokens(show);
		std::transform(show.begin(), show.end(), show.begin(), trim());
	}

	void OnNickInfo(CommandSource &source, NickAlias *na, InfoFormatter &info, bool show_hidden) override
	{
		NSSuspendInfo *s = suspend.Get(na->nc);
		if (!s)
			return;

		if (show_hidden || Show(source, "suspended"))
			info[_("Suspended")] = _("This nickname is \002suspended\002.");
		if (!s->by.empty() && (show_hidden || Show(source, "by")))
			info[_("Suspended by")] = s->by;
		if (!s->reason.empty() && (show_hidden || Show(source, "reason")))
			info[_("Suspend reason")] = s->reason;
		if (s->when && (show_hidden || Show(source, "on")))
			info[_("Suspended on")] = Anope::strftime(s->when, source.GetAccount());
		if (s->expires && (show_hidden || Show(source, "expires")))
			info[_("Suspension expires")] = Anope::strftime(s->expires, source.GetAccount());
	}

	void OnPreNickExpire(NickAlias *na, bool &expire) override
	{
		NSSuspendInfo *s = suspend.Get(na->nc);
		if (!s)
			return;

		expire = false;

		if (!Anope::NoExpire && s->expires && s->expires < Anope::CurTime)
		{
			na->last_seen = Anope::CurTime;
			Expire(na);
		}
	}

	EventReturn OnNickValidate(User *u, NickAlias *na) override
	{
		NSSuspendInfo *s = suspend.Get(na->nc);
		if (!s)
			return EVENT_CONTINUE;

		if (!Anope::NoExpire && s->expires && s->expires < Anope::CurTime)
		{
			Expire(na);
			return EVENT_CONTINUE;
		}

		u->SendMessage(Config->GetClient("NickServ"), NICK_X_SUSPENDED, u->nick.c_str());
		return EVENT_STOP;
	}
};

MODULE_INIT(NSSuspend)
