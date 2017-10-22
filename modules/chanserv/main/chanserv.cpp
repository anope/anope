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
#include "modules/chanserv/mode.h"
#include "modules/help.h"
#include "modules/botserv/bot.h"
#include "modules/chanserv.h"
#include "modules/chanserv/info.h"
#include "modules/chanserv/akick.h"
#include "channeltype.h"
#include "leveltype.h"
#include "modetype.h"
#include "chanaccesstype.h"
#include "modules/chanserv/main/chanaccess.h"

class ChanServCore : public Module
	, public ChanServ::ChanServService
	, public EventHook<Event::BotDelete>
	, public EventHook<Event::BotPrivmsg>
	, public EventHook<Event::DelCore>
	, public EventHook<Event::DelChan>
	, public EventHook<Event::Help>
	, public EventHook<Event::CheckModes>
	, public EventHook<Event::CanSet>
	, public EventHook<Event::ChannelSync>
	, public EventHook<Event::Log>
	, public EventHook<Event::ExpireTick>
	, public EventHook<Event::CheckDelete>
	, public EventHook<Event::PostInit>
	, public EventHook<Event::ChanRegistered>
	, public EventHook<Event::JoinChannel>
	, public EventHook<Event::ChanInfo>
{
	Reference<ServiceBot> ChanServ;
	std::vector<Anope::string> defaults;
	ExtensibleItem<bool> inhabit;
	std::vector<ChanServ::Privilege> Privileges;
	ChanServ::registered_channel_map registered_channels;
	ChannelType channel_type;
	LevelType level_type;
	CSModeType mode_type;

 public:
	ChanServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR)
		, ChanServService(this)
		, EventHook<Event::BotDelete>(this)
		, EventHook<Event::BotPrivmsg>(this)
		, EventHook<Event::DelCore>(this)
		, EventHook<Event::DelChan>(this)
		, EventHook<Event::Help>(this)
		, EventHook<Event::CheckModes>(this)
		, EventHook<Event::CanSet>(this)
		, EventHook<Event::ChannelSync>(this)
		, EventHook<Event::Log>(this)
		, EventHook<Event::ExpireTick>(this)
		, EventHook<Event::CheckDelete>(this)
		, EventHook<Event::PostInit>(this)
		, EventHook<Event::ChanRegistered>(this)
		, EventHook<Event::JoinChannel>(this)
		, EventHook<Event::ChanInfo>(this)
		, inhabit(this, "inhabit")
		, channel_type(this)
		, level_type(this)
		, mode_type(this)
	{
		ChanServ::service = this;
	}

	~ChanServCore()
	{
		ChanServ::service = nullptr;
	}

	ChanServ::Channel *Find(const Anope::string &name) override
	{
		return channel_type.FindChannel(name);
	}

	ChanServ::registered_channel_map& GetChannels() override
	{
		return registered_channels;
	}

	void Hold(Channel *c) override
	{
		/** A timer used to keep the BotServ bot/ChanServ in the channel
		 * after kicking the last user in a channel
		 */
		class ChanServTimer : public Timer
		{
			Reference<ServiceBot> &ChanServ;
			ExtensibleItem<bool> &inhabit;
			Reference<Channel> c;

		 public:
			/** Constructor
			 * @param chan The channel
			 */
			ChanServTimer(Reference<ServiceBot> &cs, ExtensibleItem<bool> &i, Module *m, Channel *chan) : Timer(m, Config->GetModule(m)->Get<time_t>("inhabit", "15s"))
				, ChanServ(cs)
				, inhabit(i)
				, c(chan)
			{
				if (!ChanServ || !c)
					return;

				inhabit.Set(c, true);

				ChanServ::Channel *ci = c->GetChannel();
				if (!ci || !ci->GetBot())
					ChanServ->Join(c);
				else if (!c->FindUser(ci->GetBot()))
					ci->GetBot()->Join(c);

				/* Set +ntsi to prevent rejoin */
				c->SetMode(NULL, "NOEXTERNAL");
				c->SetMode(NULL, "TOPIC");
				c->SetMode(NULL, "SECRET");
				c->SetMode(NULL, "INVITE");
			}

			/** Called when the delay is up
			 * @param The current time
			 */
			void Tick(time_t) override
			{
				if (!c)
					return;

				inhabit.Unset(c);

				/* In the event we don't part */
				c->RemoveMode(NULL, "SECRET");
				c->RemoveMode(NULL, "INVITE");

				ChanServ::Channel *ci = c->GetChannel();
				if (!ci || !ci->GetBot())
				{
					if (ChanServ)
						ChanServ->Part(c);
				}
				/* If someone has rejoined this channel in the meantime, don't part the bot */
				else if (c->users.size() <= 1)
				{
					ci->GetBot()->Part(c);
				}
			}
		};

		if (inhabit.HasExt(c))
			return;

		new ChanServTimer(ChanServ, inhabit, this, c);
	}

	void AddPrivilege(ChanServ::Privilege p) override
	{
		unsigned i;
		for (i = 0; i < Privileges.size(); ++i)
		{
			ChanServ::Privilege &priv = Privileges[i];

			if (priv.rank > p.rank)
				break;
		}

		Privileges.insert(Privileges.begin() + i, p);
	}

	void RemovePrivilege(ChanServ::Privilege &p) override
	{
		std::vector<ChanServ::Privilege>::iterator it = std::find(Privileges.begin(), Privileges.end(), p);
		if (it != Privileges.end())
			Privileges.erase(it);

		for (auto& cit : GetChannels())
		{
			ChanServ::Channel *ci = cit.second;
			ci->RemoveLevel(p.name);
		}
	}

	ChanServ::Privilege *FindPrivilege(const Anope::string &name) override
	{
		for (unsigned i = Privileges.size(); i > 0; --i)
			if (Privileges[i - 1].name.equals_ci(name))
				return &Privileges[i - 1];
		return NULL;
	}

	std::vector<ChanServ::Privilege> &GetPrivileges() override
	{
		return Privileges;
	}

	void ClearPrivileges() override
	{
		Privileges.clear();
	}

	void OnReload(Configuration::Conf *conf) override
	{
		const Anope::string &channick = conf->GetModule(this)->Get<Anope::string>("client");

		if (channick.empty())
			throw ConfigException(Module::name + ": <client> must be defined");

		ServiceBot *bi = ServiceBot::Find(channick, true);
		if (!bi)
			throw ConfigException(Module::name + ": no bot named " + channick);

		ChanServ = bi;

		ClearPrivileges();
		for (int i = 0; i < conf->CountBlock("privilege"); ++i)
		{
			Configuration::Block *privilege = conf->GetBlock("privilege", i);

			const Anope::string &nname = privilege->Get<Anope::string>("name"),
						&desc = privilege->Get<Anope::string>("desc");
			int rank = privilege->Get<int>("rank");
			Anope::string value = privilege->Get<Anope::string>("level");
			int level;
			if (value.equals_ci("founder"))
				level = ChanServ::ACCESS_FOUNDER;
			else if (value.equals_ci("disabled"))
				level = ChanServ::ACCESS_INVALID;
			else
				level = privilege->Get<int>("level");

			AddPrivilege(ChanServ::Privilege(nname, desc, rank, level));
		}

		spacesepstream(conf->GetModule(this)->Get<Anope::string>("defaults", "greet fantasy")).GetTokens(defaults);
		if (defaults.empty())
			defaults = { "keeptopic", "secure", "securefounder", "signkick" };
		else if (defaults[0].equals_ci("none"))
			defaults.clear();
	}

	void OnBotDelete(ServiceBot *bi) override
	{
		if (bi == ChanServ)
			ChanServ = NULL;
	}

	EventReturn OnBotPrivmsg(User *u, ServiceBot *bi, Anope::string &message) override
	{
		if (bi == ChanServ && Config->GetModule(this)->Get<bool>("opersonly") && !u->HasMode("OPER"))
		{
			u->SendMessage(bi, _("Access denied."));
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnDelCore(NickServ::Account *nc) override
	{
		unsigned int max_reg = Config->GetModule(this)->Get<unsigned int>("maxregistered");
		for (ChanServ::Channel *ci : nc->GetRefs<ChanServ::Channel *>())
		{
			if (ci->GetFounder() == nc)
			{
				NickServ::Account *newowner = NULL;
				if (ci->GetSuccessor() && ci->GetSuccessor() != nc && (ci->GetSuccessor()->GetOper() || !max_reg || ci->GetSuccessor()->GetChannelCount() < max_reg))
					newowner = ci->GetSuccessor();
				else
				{
					ChanServ::ChanAccess *highest = NULL;
					for (unsigned j = 0; j < ci->GetAccessCount(); ++j)
					{
						ChanServ::ChanAccess *ca = ci->GetAccess(j);
						NickServ::Account *anc = ca->GetAccount();

						if (!anc || (!anc->GetOper() && max_reg && anc->GetChannelCount() >= max_reg) || (anc == nc))
							continue;
						if (!highest || *ca > *highest)
							highest = ca;
					}
					if (highest)
						newowner = highest->GetAccount();
				}

				if (newowner)
				{
					ChanServ->logger.Category("chanserv/drop").Log(_("Transferring foundership of {0} from deleted account {1} to {2}"),
							ci->GetName(), nc->GetDisplay(), newowner->GetDisplay());

					ci->SetFounder(newowner);

					// Can't be both founder and successor
					if (ci->GetSuccessor() == newowner)
						ci->SetSuccessor(nullptr);
				}
				else
				{
					ChanServ->logger.Category("chanserv/drop").Log(_("Deleting channel {0} owned by deleted account {1}"),
							ci->GetName(), nc->GetDisplay());

					ci->Delete();
					continue;
				}
			}
		}
	}

	void OnDelChan(ChanServ::Channel *ci) override
	{
		Channel *c = ci->GetChannel();
		if (c == nullptr)
			return;

		c->RemoveMode(ci->WhoSends(), "REGISTERED", "", false);

		const Anope::string &require = Config->GetModule(this)->Get<Anope::string>("require");
		if (!require.empty())
			c->SetModes(ci->WhoSends(), false, "-%s", require.c_str());
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *ChanServ)
			return EVENT_CONTINUE;
		source.Reply(_("\002{0}\002 allows you to register and control various\n"
			"aspects of channels. {1} can often prevent\n"
			"malicious users from \"taking over\" channels by limiting\n"
			"who is allowed channel operator privileges. Available\n"
			"commands are listed below; to use them, type\n"
			"\002{2}{3} \037command\037\002. For more information on a\n"
			"specific command, type \002{4}{5} HELP \037command\037\002.\n"),
			ChanServ->nick, ChanServ->nick, Config->StrictPrivmsg, ChanServ->nick, Config->StrictPrivmsg, ChanServ->nick, ChanServ->nick, source.GetCommand());
		return EVENT_CONTINUE;
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *ChanServ)
			return;
		time_t chanserv_expire = Config->GetModule(this)->Get<time_t>("expire", "14d");
		if (chanserv_expire >= 86400)
			source.Reply(_(" \n"
				"Note that any channel which is not used for %d days\n"
				"(i.e. which no user on the channel's access list enters\n"
				"for that period of time) will be automatically dropped."), chanserv_expire / 86400);
		if (source.IsServicesOper())
			source.Reply(_(" \n"
				"Services Operators can also, depending on their access drop\n"
				"any channel, view (and modify) the access, levels and akick\n"
				"lists and settings for any channel."));
	}

	void OnCheckModes(Reference<Channel> &c) override
	{
		if (!c)
			return;

		ChanServ::Channel *ci = c->GetChannel();
		if (ci)
			c->SetMode(nullptr, "REGISTERED", "", false);
		else
			c->RemoveMode(nullptr, "REGISTERED", "", false);

		const Anope::string &require = Config->GetModule(this)->Get<Anope::string>("require");
		if (!require.empty())
		{
			if (ci)
				c->SetModes(nullptr, false, "+%s", require.c_str());
			else
				c->SetModes(nullptr, false, "-%s", require.c_str());
		}
	}

	EventReturn OnCanSet(User *u, const ChannelMode *cm) override
	{
		if (Config->GetModule(this)->Get<Anope::string>("nomlock").find(cm->mchar) != Anope::string::npos
			|| Config->GetModule(this)->Get<Anope::string>("require").find(cm->mchar) != Anope::string::npos)
			return EVENT_STOP;
		return EVENT_CONTINUE;
	}

	void OnChannelSync(Channel *c) override
	{
		ChanServ::Channel *ci = c->GetChannel();
		bool perm = c->HasMode("PERM") || (ci && ci->IsPersist());
		if (!perm && !c->botchannel && (c->users.empty() || (c->users.size() == 1 && c->users.begin()->second->user->server == Me)))
		{
			this->Hold(c);
		}
	}

	void OnLog(Logger *logger) override
	{
#warning ""
#if 0
		if (l->type == LogType::CHANNEL)
			l->bi = ChanServ;
#endif
	}

	void OnExpireTick() override
	{
		time_t chanserv_expire = Config->GetModule(this)->Get<time_t>("expire", "14d");

		if (!chanserv_expire || Anope::NoExpire || Anope::ReadOnly)
			return;

		for (ChanServ::Channel *ci : Serialize::GetObjects<ChanServ::Channel *>())
		{
			bool expire = false;

			if (Anope::CurTime - ci->GetLastUsed() >= chanserv_expire)
			{
				Channel *c = ci->GetChannel();
				if (c)
				{
					time_t last_used = ci->GetLastUsed();
					for (Channel::ChanUserList::const_iterator cit = c->users.begin(), cit_end = c->users.end(); cit != cit_end && last_used == ci->GetLastUsed(); ++cit)
						ci->AccessFor(cit->second->user);
					expire = last_used == ci->GetLastUsed();
				}
				else
					expire = true;
			}

			EventManager::Get()->Dispatch(&ChanServ::Event::PreChanExpire::OnPreChanExpire, ci, expire);

			if (expire)
			{
				ChanServ->logger.Category("chanserv/expire").Log(_("Expiring channel {0} (founder: {1})"),
						ci->GetName(), ci->GetFounder() ? ci->GetFounder()->GetDisplay() : "(none)");

				EventManager::Get()->Dispatch(&ChanServ::Event::ChanExpire::OnChanExpire, ci);
				ci->Delete();
			}
		}
	}

	EventReturn OnCheckDelete(Channel *c) override
	{
		/* Do not delete this channel if ChanServ/a BotServ bot is inhabiting it */
		if (inhabit.HasExt(c))
			return EVENT_STOP;

		return EVENT_CONTINUE;
	}

	void OnPostInit() override
	{
		ChannelMode *perm = ModeManager::FindChannelModeByName("PERM");

		/* Find all persistent channels and create them, so they will be bursted to the uplink */
		for (ChanServ::Channel *ci : Serialize::GetObjects<ChanServ::Channel *>())
		{
			if (!ci->IsPersist())
				continue;

			bool created;
			Channel *c = Channel::FindOrCreate(ci->GetName(), created, ci->GetTimeRegistered());
			c->syncing |= created;

			if (perm != nullptr)
			{
				c->SetMode(NULL, perm);
			}
			else
			{
				if (!ci->GetBot())
				{
					ServiceBot *bi = ci->WhoSends();
					if (bi != nullptr)
						bi->Assign(nullptr, ci);
				}

				if (ci->GetBot() != nullptr && c->FindUser(ci->GetBot()) == nullptr)
				{
					Anope::string botmodes = Config->GetModule("botserv/main")->Get<Anope::string>("botmodes",
							Config->GetModule("chanserv/main")->Get<Anope::string>("botmodes"));
					ChannelStatus status(botmodes);
					ci->GetBot()->Join(c, &status);
				}
			}
		}
	}

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		/* Set default chan flags */
		for (unsigned i = 0; i < defaults.size(); ++i)
			ci->SetS<bool>(defaults[i].upper(), true);

		Channel *c = ci->GetChannel();
		if (!c)
			return;
		/* Mark the channel as persistent */
		if (c->HasMode("PERM"))
			ci->SetPersist(true);
		/* Persist may be in def cflags, set it here */
		else if (ci->IsPersist())
			c->SetMode(NULL, "PERM");
	}

	void OnJoinChannel(User *u, Channel *c) override
	{
		ChanServ::Channel *ci = c->GetChannel();
		if (!ci)
			return;

		time_t ts = ci->GetChannelTS();
		if (ts == 0)
			ts = ci->GetTimeRegistered();

		if (c->creation_time > ts)
		{
			logger.Debug("Changing TS of {0} from {1} to {2}", c->name, c->creation_time, ts);
			c->creation_time = ts;
			IRCD->Send<messages::MessageChannel>(c);
			c->Reset();
		}
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool show_all) override
	{
		if (!show_all)
			return;

		time_t chanserv_expire = Config->GetModule(this)->Get<time_t>("expire", "14d");
		if (!ci->IsNoExpire() && chanserv_expire && !Anope::NoExpire && ci->GetLastUsed() != Anope::CurTime)
			info[_("Expires")] = Anope::strftime(ci->GetLastUsed() + chanserv_expire, source.GetAccount());
	}
};

MODULE_INIT(ChanServCore)

