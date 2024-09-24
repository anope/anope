/* ChanServ core functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/suspend.h"

struct CSSuspendInfo final
	: SuspendInfo
	, Serializable
{
	CSSuspendInfo(Extensible *) : Serializable("CSSuspendInfo") { }

	void Serialize(Serialize::Data &data) const override
	{
		data.Store("chan", what);
		data.Store("by", by);
		data.Store("reason", reason);
		data.Store("time", when);
		data.Store("expires", expires);
	}

	static Serializable *Unserialize(Serializable *obj, Serialize::Data &data)
	{
		Anope::string schan;
		data["chan"] >> schan;

		CSSuspendInfo *si;
		if (obj)
			si = anope_dynamic_static_cast<CSSuspendInfo *>(obj);
		else
		{
			ChannelInfo *ci = ChannelInfo::Find(schan);
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

class CommandCSSuspend final
	: public Command
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
		time_t expiry_secs = Config->GetModule(this->owner)->Get<time_t>("suspendexpire");

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
				source.Reply(BAD_EXPIRY_TIME);
				return;
			}
		}

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
			source.Reply(_("\002%s\002 is already suspended."), ci->name.c_str());
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

			for (const auto &[_, uc] : ci->c->users)
			{
				User *user = uc->user;
				if (!user->HasMode("OPER") && user->server != Me)
					users.push_back(user);
			}

			for (auto *user : users)
				ci->c->Kick(NULL, user, "%s", !reason.empty() ? reason.c_str() : Language::Translate(user, _("This channel has been suspended.")));
		}

		Log(LOG_ADMIN, source, this, ci) << "(" << (!reason.empty() ? reason : "No reason") << "), expires on " << (expiry_secs ? Anope::strftime(Anope::CurTime + expiry_secs) : "never");
		source.Reply(_("Channel \002%s\002 is now suspended."), ci->name.c_str());

		FOREACH_MOD(OnChanSuspend, (ci));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Disallows anyone from using the given channel.\n"
				"May be cancelled by using the \002UNSUSPEND\002\n"
				"command to preserve all previous channel data/settings.\n"
				"If an expiry is given the channel will be unsuspended after\n"
				"that period of time, else the default expiry from the\n"
				"configuration is used.\n"
				" \n"
				"Reason may be required on certain networks."));
		return true;
	}
};

class CommandCSUnSuspend final
	: public Command
{
public:
	CommandCSUnSuspend(Module *creator) : Command(creator, "chanserv/unsuspend", 1, 1)
	{
		this->SetDesc(_("Releases a suspended channel"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
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

		Log(LOG_ADMIN, source, this, ci) << "which was suspended by " << si->by << " for: " << (!si->reason.empty() ? si->reason : "No reason");

		ci->Shrink<CSSuspendInfo>("CS_SUSPENDED");

		source.Reply(_("Channel \002%s\002 is now released."), ci->name.c_str());

		FOREACH_MOD(OnChanUnsuspend, (ci));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Releases a suspended channel. All data and settings\n"
				"are preserved from before the suspension."));
		return true;
	}
};

class CSSuspend final
	: public Module
{
	CommandCSSuspend commandcssuspend;
	CommandCSUnSuspend commandcsunsuspend;
	ExtensibleItem<CSSuspendInfo> suspend;
	Serialize::Type suspend_type;
	std::vector<Anope::string> show;

	struct trim final
	{
		Anope::string operator()(Anope::string s) const
		{
			return s.trim();
		}
	};

	void Expire(ChannelInfo *ci)
	{
		suspend.Unset(ci);
		Log(this) << "Expiring suspend for " << ci->name;
	}


	bool Show(CommandSource &source, const Anope::string &what) const
	{
		return source.IsOper() || std::find(show.begin(), show.end(), what) != show.end();
	}

public:
	CSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandcssuspend(this), commandcsunsuspend(this), suspend(this, "CS_SUSPENDED"),
		suspend_type("CSSuspendInfo", CSSuspendInfo::Unserialize)
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Anope::string s = conf->GetModule(this)->Get<Anope::string>("show");
		commasepstream(s).GetTokens(show);
		std::transform(show.begin(), show.end(), show.begin(), trim());
	}

	void OnChanInfo(CommandSource &source, ChannelInfo *ci, InfoFormatter &info, bool show_hidden) override
	{
		CSSuspendInfo *si = suspend.Get(ci);
		if (!si)
			return;

		if (show_hidden || Show(source, "suspended"))
			info[_("Suspended")] = _("This channel is \002suspended\002.");
		if (!si->by.empty() && (show_hidden || Show(source, "by")))
			info[_("Suspended by")] = si->by;
		if (!si->reason.empty() && (show_hidden || Show(source, "reason")))
			info[_("Suspend reason")] = si->reason;
		if (si->when && (show_hidden || Show(source, "on")))
			info[_("Suspended on")] = Anope::strftime(si->when, source.GetAccount());
		if (si->expires && (show_hidden || Show(source, "expires")))
			info[_("Suspension expires")] = Anope::strftime(si->expires, source.GetAccount());
	}

	void OnPreChanExpire(ChannelInfo *ci, bool &expire) override
	{
		CSSuspendInfo *si = suspend.Get(ci);
		if (!si)
			return;

		expire = false;

		if (!Anope::NoExpire && si->expires && si->expires < Anope::CurTime)
		{
			ci->last_used = Anope::CurTime;
			Expire(ci);
		}
	}

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) override
	{
		if (u->HasMode("OPER") || !c->ci)
			return EVENT_CONTINUE;

		CSSuspendInfo *si = suspend.Get(c->ci);
		if (!si)
			return EVENT_CONTINUE;

		if (!Anope::NoExpire && si->expires && si->expires < Anope::CurTime)
		{
			Expire(c->ci);
			return EVENT_CONTINUE;
		}

		reason = Language::Translate(u, _("This channel may not be used."));
		return EVENT_STOP;
	}

	EventReturn OnChanDrop(CommandSource &source, ChannelInfo *ci) override
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
