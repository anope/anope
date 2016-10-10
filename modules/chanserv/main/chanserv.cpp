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
	, public EventHook<Event::ChannelCreate>
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
	, public EventHook<Event::PreUplinkSync>
	, public EventHook<Event::ChanRegistered>
	, public EventHook<Event::JoinChannel>
	, public EventHook<Event::ChannelModeSet>
	, public EventHook<Event::ChanInfo>
	, public EventHook<Event::SetCorrectModes>
{
	Reference<ServiceBot> ChanServ;
	std::vector<Anope::string> defaults;
	ExtensibleItem<bool> inhabit;
	bool always_lower;
	std::vector<ChanServ::Privilege> Privileges;
	ChanServ::registered_channel_map registered_channels;
	ChannelType channel_type;
	LevelType level_type;
	CSModeType mode_type;

 public:
	ChanServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR)
		, ChanServService(this)
		, EventHook<Event::ChannelCreate>(this)
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
		, EventHook<Event::PreUplinkSync>(this)
		, EventHook<Event::ChanRegistered>(this)
		, EventHook<Event::JoinChannel>(this)
		, EventHook<Event::ChannelModeSet>(this)
		, EventHook<Event::ChanInfo>(this)
		, EventHook<Event::SetCorrectModes>(this)
		, inhabit(this, "inhabit")
		, always_lower(false)
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
			ChanServTimer(Reference<ServiceBot> &cs, ExtensibleItem<bool> &i, Module *m, Channel *chan) : Timer(m, Config->GetModule(m)->Get<time_t>("inhabit", "15s")), ChanServ(cs), inhabit(i), c(chan)
			{
				if (!ChanServ || !c)
					return;
				inhabit.Set(c, true);
				if (!c->ci || !c->ci->GetBot())
					ChanServ->Join(c);
				else if (!c->FindUser(c->ci->GetBot()))
					c->ci->GetBot()->Join(c);

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

				if (!c->ci || !c->ci->GetBot())
				{
					if (ChanServ)
						ChanServ->Part(c);
				}
				/* If someone has rejoined this channel in the meantime, don't part the bot */
				else if (c->users.size() <= 1)
					c->ci->GetBot()->Part(c);
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
		{
			defaults.push_back("KEEPTOPIC");
			defaults.push_back("CS_SECURE");
			defaults.push_back("SECUREFOUNDER");
			defaults.push_back("SIGNKICK");
		}
		else if (defaults[0].equals_ci("none"))
			defaults.clear();

		always_lower = conf->GetModule(this)->Get<bool>("always_lower_ts");
	}

	void OnChannelCreate(Channel *c) override
	{
		c->ci = Find(c->name);
		if (c->ci)
			c->ci->c = c;
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
				if (ci->GetSuccessor() && ci->GetSuccessor() != nc && (ci->GetSuccessor()->IsServicesOper() || !max_reg || ci->GetSuccessor()->GetChannelCount() < max_reg))
					newowner = ci->GetSuccessor();
				else
				{
					ChanServ::ChanAccess *highest = NULL;
					for (unsigned j = 0; j < ci->GetAccessCount(); ++j)
					{
						ChanServ::ChanAccess *ca = ci->GetAccess(j);
						NickServ::Account *anc = ca->GetAccount();

						if (!anc || (!anc->IsServicesOper() && max_reg && anc->GetChannelCount() >= max_reg) || (anc == nc))
							continue;
						if (!highest || *ca > *highest)
							highest = ca;
					}
					if (highest)
						newowner = highest->GetAccount();
				}

				if (newowner)
				{
					::Log(LOG_NORMAL, "chanserv/drop", ChanServ) << "Transferring foundership of " << ci->GetName() << " from deleted nick " << nc->GetDisplay() << " to " << newowner->GetDisplay();
					ci->SetFounder(newowner);
					ci->SetSuccessor(NULL);
				}
				else
				{
					::Log(LOG_NORMAL, "chanserv/drop", ChanServ) << "Deleting channel " << ci->GetName() << " owned by deleted nick " << nc->GetDisplay();

					delete ci;
					continue;
				}
			}

			if (ci->GetSuccessor() == nc)
				ci->SetSuccessor(NULL);

			/* are these necessary? */
			for (unsigned j = 0; j < ci->GetAccessCount(); ++j)
			{
				ChanServ::ChanAccess *ca = ci->GetAccess(j);
				NickServ::Account *anc = ca->GetAccount();

				if (anc && anc == nc)
				{
					delete ca;
					break;
				}
			}

			for (unsigned j = 0; j < ci->GetAkickCount(); ++j)
			{
				AutoKick *ak = ci->GetAkick(j);
				if (ak->GetAccount() == nc)
				{
					delete ak;
					break;
				}
			}
		}
	}

	void OnDelChan(ChanServ::Channel *ci) override
	{
		/* remove access entries that are this channel */

		for (ChanServ::Channel *c : ci->GetRefs<ChanServ::Channel *>())
		{
			for (unsigned j = 0; j < c->GetAccessCount(); ++j)
			{
				ChanServ::ChanAccess *a = c->GetAccess(j);

				if (a->Mask().equals_ci(ci->GetName()))
				{
					delete a;
					break;
				}
			}
		}

		if (ci->c)
		{
			ci->c->RemoveMode(ci->WhoSends(), "REGISTERED", "", false);

			const Anope::string &require = Config->GetModule(this)->Get<Anope::string>("require");
			if (!require.empty())
				ci->c->SetModes(ci->WhoSends(), false, "-%s", require.c_str());
		}
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *ChanServ)
			return EVENT_CONTINUE;
		source.Reply(_("\002%s\002 allows you to register and control various\n"
			"aspects of channels. %s can often prevent\n"
			"malicious users from \"taking over\" channels by limiting\n"
			"who is allowed channel operator privileges. Available\n"
			"commands are listed below; to use them, type\n"
			"\002%s%s \037command\037\002. For more information on a\n"
			"specific command, type \002%s%s HELP \037command\037\002.\n"),
			ChanServ->nick.c_str(), ChanServ->nick.c_str(), Config->StrictPrivmsg.c_str(), ChanServ->nick.c_str(), Config->StrictPrivmsg.c_str(), ChanServ->nick.c_str(), ChanServ->nick.c_str(), source.command.c_str());
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

		if (c->ci)
			c->SetMode(c->ci->WhoSends(), "REGISTERED", "", false);
		else
			c->RemoveMode(c->ci->WhoSends(), "REGISTERED", "", false);

		const Anope::string &require = Config->GetModule(this)->Get<Anope::string>("require");
		if (!require.empty())
		{
			if (c->ci)
				c->SetModes(c->ci->WhoSends(), false, "+%s", require.c_str());
			else
				c->SetModes(c->ci->WhoSends(), false, "-%s", require.c_str());
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
		bool perm = c->HasMode("PERM") || (c->ci && c->ci->HasFieldS("PERSIST"));
		if (!perm && !c->botchannel && (c->users.empty() || (c->users.size() == 1 && c->users.begin()->second->user->server == Me)))
		{
			this->Hold(c);
		}
	}

	void OnLog(::Log *l) override
	{
		if (l->type == LOG_CHANNEL)
			l->bi = ChanServ;
	}

	void OnExpireTick() override
	{
		time_t chanserv_expire = Config->GetModule(this)->Get<time_t>("expire", "14d");

		if (!chanserv_expire || Anope::NoExpire || Anope::ReadOnly)
			return;

		for (ChanServ::Channel *ci : channel_type.List<ChanServ::Channel *>())
		{
			bool expire = false;

			if (Anope::CurTime - ci->GetLastUsed() >= chanserv_expire)
			{
				if (ci->c)
				{
					time_t last_used = ci->GetLastUsed();
					for (Channel::ChanUserList::const_iterator cit = ci->c->users.begin(), cit_end = ci->c->users.end(); cit != cit_end && last_used == ci->GetLastUsed(); ++cit)
						ci->AccessFor(cit->second->user);
					expire = last_used == ci->GetLastUsed();
				}
				else
					expire = true;
			}

			EventManager::Get()->Dispatch(&ChanServ::Event::PreChanExpire::OnPreChanExpire, ci, expire);

			if (expire)
			{
				::Log(LOG_NORMAL, "chanserv/expire", ChanServ) << "Expiring channel " << ci->GetName() << " (founder: " << (ci->GetFounder() ? ci->GetFounder()->GetDisplay() : "(none)") << ")";
				EventManager::Get()->Dispatch(&ChanServ::Event::ChanExpire::OnChanExpire, ci);
				delete ci;
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

	void OnPreUplinkSync(Server *serv) override
	{
		/* Find all persistent channels and create them, as we are about to finish burst to our uplink */
		for (ChanServ::Channel *ci : channel_type.List<ChanServ::Channel *>())
		{
			if (ci->HasFieldS("PERSIST"))
			{
				bool c;
				ci->c = Channel::FindOrCreate(ci->GetName(), c, ci->GetTimeRegistered());

				if (ModeManager::FindChannelModeByName("PERM") != NULL)
				{
					if (c)
						IRCD->SendChannel(ci->c);
					ci->c->SetMode(NULL, "PERM");
				}
				else
				{
					if (!ci->GetBot())
						ci->WhoSends()->Assign(NULL, ci);
					if (ci->c->FindUser(ci->GetBot()) == NULL)
					{
						Anope::string botmodes = Config->GetModule("botserv/main")->Get<Anope::string>("botmodes",
								Config->GetModule("chanserv/main")->Get<Anope::string>("botmodes"));
						ChannelStatus status(botmodes);
						ci->GetBot()->Join(ci->c, &status);
					}
				}
			}
		}

	}

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		/* Set default chan flags */
		for (unsigned i = 0; i < defaults.size(); ++i)
			ci->SetS<bool>(defaults[i].upper(), true);

		if (!ci->c)
			return;
		/* Mark the channel as persistent */
		if (ci->c->HasMode("PERM"))
			ci->SetS("PERSIST", true);
		/* Persist may be in def cflags, set it here */
		else if (ci->HasFieldS("PERSIST"))
			ci->c->SetMode(NULL, "PERM");
	}

	void OnJoinChannel(User *u, Channel *c) override
	{
		if (always_lower && c->ci && c->creation_time > c->ci->GetTimeRegistered())
		{
			::Log(LOG_DEBUG) << "Changing TS of " << c->name << " from " << c->creation_time << " to " << c->ci->GetTimeRegistered();
			c->creation_time = c->ci->GetTimeRegistered();
			IRCD->SendChannel(c);
			c->Reset();
		}
	}

	EventReturn OnChannelModeSet(Channel *c, const MessageSource &setter, ChannelMode *mode, const Anope::string &param) override
	{
		if (!always_lower && Anope::CurTime == c->creation_time && c->ci && setter.GetUser() && !setter.GetUser()->server->IsULined())
		{
			ChanUserContainer *cu = c->FindUser(setter.GetUser());
			ChannelMode *cm = ModeManager::FindChannelModeByName("OP");
			if (cu && cm && !cu->status.HasMode(cm->mchar))
			{
				/* Our -o and their mode change crossing, bounce their mode */
				c->RemoveMode(c->ci->WhoSends(), mode, param);
				/* We don't set mlocks until after the join has finished processing, it will stack with this change,
				 * so there isn't much for the user to remove except -nt etc which is likely locked anyway.
				 */
			}
		}

		return EVENT_CONTINUE;
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool show_all) override
	{
		if (!show_all)
			return;

		time_t chanserv_expire = Config->GetModule(this)->Get<time_t>("expire", "14d");
		if (!ci->HasFieldS("CS_NO_EXPIRE") && chanserv_expire && !Anope::NoExpire && ci->GetLastUsed() != Anope::CurTime)
			info[_("Expires")] = Anope::strftime(ci->GetLastUsed() + chanserv_expire, source.GetAccount());
	}

	void OnSetCorrectModes(User *user, Channel *chan, ChanServ::AccessGroup &access, bool &give_modes, bool &take_modes) override
	{
		if (always_lower)
			// Since we always lower the TS, the other side will remove the modes if the channel ts lowers, so we don't
			// have to worry about it
			take_modes = false;
		else if (ModeManager::FindChannelModeByName("REGISTERED"))
			// Otherwise if the registered channel mode exists, we should remove modes if the channel is not +r
			take_modes = !chan->HasMode("REGISTERED");
	}
};

MODULE_INIT(ChanServCore)

