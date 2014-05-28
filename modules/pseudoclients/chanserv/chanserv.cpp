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
#include "channel.h"
#include "modules/cs_mode.h"
#include "modules/help.h"
#include "modules/bs_bot.h"
#include "modules/chanserv.h"
#include "modules/cs_info.h"
#include "modules/cs_akick.h"

class ChanServCore : public Module
	, public ChanServ::ChanServService
	, public EventHook<Event::ChannelCreate>
	, public EventHook<Event::BotDelete>
	, public EventHook<Event::BotPrivmsg>
	, public EventHook<Event::DelCore>
	, public EventHook<Event::DelChan>
	, public EventHook<Event::Help>
	, public EventHook<Event::CheckModes>
	, public EventHook<Event::CreateChan>
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
	Reference<BotInfo> ChanServ;
	std::vector<Anope::string> defaults;
	ExtensibleItem<bool> inhabit;
	ExtensibleRef<bool> persist;
	bool always_lower;
	EventHandlers<ChanServ::Event::PreChanExpire> OnPreChanExpire;
	EventHandlers<ChanServ::Event::ChanExpire> OnChanExpire;
	std::vector<ChanServ::Privilege> Privileges;
	std::vector<ChanServ::AccessProvider *> Providers;
	Serialize::Checker<ChanServ::registered_channel_map> registered_channels;
	Serialize::Type channel_type, access_type;

 public:
	ChanServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR)
		, ChanServService(this)
		, EventHook<Event::ChannelCreate>("OnChannelCreate")
		, EventHook<Event::BotDelete>("OnBotDelete")
		, EventHook<Event::BotPrivmsg>("OnBotPrivmsg")
		, EventHook<Event::DelCore>("OnDelCore")
		, EventHook<Event::DelChan>("OnDelChan")
		, EventHook<Event::Help>("OnHelp")
		, EventHook<Event::CheckModes>("OnCheckModes")
		, EventHook<Event::CreateChan>("OnCreateChan")
		, EventHook<Event::CanSet>("OnCanSet")
		, EventHook<Event::ChannelSync>("OnChannelSync")
		, EventHook<Event::Log>("OnLog")
		, EventHook<Event::ExpireTick>("OnExpireTick")
		, EventHook<Event::CheckDelete>("OnCheckDelete")
		, EventHook<Event::PreUplinkSync>("OnPreUplinkSync")
		, EventHook<Event::ChanRegistered>("OnChanRegistered")
		, EventHook<Event::JoinChannel>("OnJoinChannel")
		, EventHook<Event::ChannelModeSet>("OnChannelModeSet")
		, EventHook<Event::ChanInfo>("OnChanInfo")
		, EventHook<Event::SetCorrectModes>("OnSetCorrectModes")
		, inhabit(this, "inhabit")
		, persist("PERSIST")
		, always_lower(false)
		, OnPreChanExpire(this, "OnPreChanExpire")
		, OnChanExpire(this, "OnChanExpire")
		, registered_channels("ChannelInfo")
		, channel_type("ChannelInfo", ChannelImpl::Unserialize)
		, access_type("ChanAccess", Unserialize)
	{
	}

	ChanServ::Channel *Create(const Anope::string &name) override
	{
		return new ChannelImpl(name);
	}

	ChanServ::Channel *Create(const ChanServ::Channel &ci) override
	{
		return new ChannelImpl(ci);
	}

	ChanServ::Channel *Find(const Anope::string &name) override
	{
		auto it = registered_channels->find(name);
		return it != registered_channels->end() ? it->second : nullptr;
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
			Reference<BotInfo> &ChanServ;
			ExtensibleItem<bool> &inhabit;
			Reference<Channel> c;

		 public:
			/** Constructor
			 * @param chan The channel
			 */
			ChanServTimer(Reference<BotInfo> &cs, ExtensibleItem<bool> &i, Module *m, Channel *chan) : Timer(m, Config->GetModule(m)->Get<time_t>("inhabit", "15s")), ChanServ(cs), inhabit(i), c(chan)
			{
				if (!ChanServ || !c)
					return;
				inhabit.Set(c, true);
				if (!c->ci || !c->ci->bi)
					ChanServ->Join(c);
				else if (!c->FindUser(c->ci->bi))
					c->ci->bi->Join(c);

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

				if (!c->ci || !c->ci->bi)
				{
					if (ChanServ)
						ChanServ->Part(c);
				}
				/* If someone has rejoined this channel in the meantime, don't part the bot */
				else if (c->users.size() <= 1)
					c->ci->bi->Part(c);
			}
		};

		if (inhabit.HasExt(c))
			return;

		new ChanServTimer(ChanServ, inhabit, this->owner, c);
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
			ci->QueueUpdate();
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

	std::vector<ChanServ::AccessProvider *>& GetProviders() override
	{
		return Providers;
	}

	void Destruct(ChanServ::ChanAccess *access) override
	{
		if (access->ci)
		{
			std::vector<ChanServ::ChanAccess *>::iterator it = std::find(access->ci->access->begin(), access->ci->access->end(), access);
			if (it != access->ci->access->end())
				access->ci->access->erase(it);

			const NickServ::Nick *na = NickServ::FindNick(access->mask);
			if (na != NULL)
				na->nc->RemoveChannelReference(access->ci);
			else
			{
				ChanServ::Channel *c = this->Find(access->mask);
				if (c)
					c->RemoveChannelReference(access->ci->name);
			}
		}
	}

	void Serialize(const ChanServ::ChanAccess *, Serialize::Data &data) override
	{
	}

	bool Matches(const ChanServ::ChanAccess *access, const User *u, const NickServ::Account *acc, ChanServ::ChanAccess::Path &p) override
	{
		if (access->nc)
			return access->nc == acc;

		if (u)
		{
			bool is_mask = access->mask.find_first_of("!@?*") != Anope::string::npos;
			if (is_mask && Anope::Match(u->nick, access->mask))
				return true;
			else if (Anope::Match(u->GetDisplayedMask(), access->mask))
				return true;
		}

		if (acc)
		{
			for (unsigned i = 0; i < acc->aliases->size(); ++i)
			{
				const NickServ::Nick *na = acc->aliases->at(i);
				if (Anope::Match(na->nick, access->mask))
					return true;
			}
		}

		if (IRCD->IsChannelValid(access->mask))
		{
			ChanServ::Channel *tci = Find(access->mask);
			if (tci)
			{
				for (unsigned i = 0; i < tci->GetAccessCount(); ++i)
				{
					ChanServ::ChanAccess *a = tci->GetAccess(i);
					std::pair<const ChanServ::ChanAccess *, const ChanServ::ChanAccess *> pair = std::make_pair(access, a);

					std::pair<Set::iterator, Set::iterator> range = p.first.equal_range(access);
					for (; range.first != range.second; ++range.first)
						if (range.first->first == pair.first && range.first->second == pair.second)
							goto cont;

					p.first.insert(pair);
					if (a->Matches(u, acc, p))
						p.second.insert(pair);

					cont:;
				}

				return p.second.count(access) > 0;
			}
		}

		return false;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data)
	{
		Anope::string provider, chan;

		data["provider"] >> provider;
		data["ci"] >> chan;

		ServiceReference<ChanServ::AccessProvider> aprovider("AccessProvider", provider);
		ChanServ::Channel *ci = ChanServ::service->Find(chan);
		if (!aprovider || !ci)
			return NULL;

		ChanServ::ChanAccess *access;
		if (obj)
			access = anope_dynamic_static_cast<ChanServ::ChanAccess *>(obj);
		else
			access = aprovider->Create();
		access->ci = ci;
		data["mask"] >> access->mask;
		data["creator"] >> access->creator;
		data["last_seen"] >> access->last_seen;
		data["created"] >> access->created;

		Anope::string adata;
		data["data"] >> adata;
		access->AccessUnserialize(adata);

		if (!obj)
			ci->AddAccess(access);
		return access;
	}

	void OnReload(Configuration::Conf *conf) override
	{
		const Anope::string &channick = conf->GetModule(this)->Get<const Anope::string>("client");

		if (channick.empty())
			throw ConfigException(Module::name + ": <client> must be defined");

		BotInfo *bi = BotInfo::Find(channick, true);
		if (!bi)
			throw ConfigException(Module::name + ": no bot named " + channick);

		ChanServ = bi;

		ClearPrivileges();
		for (int i = 0; i < conf->CountBlock("privilege"); ++i)
		{
			Configuration::Block *privilege = conf->GetBlock("privilege", i);

			const Anope::string &nname = privilege->Get<const Anope::string>("name"),
						&desc = privilege->Get<const Anope::string>("desc");
			int rank = privilege->Get<int>("rank");

			AddPrivilege(ChanServ::Privilege(nname, desc, rank));
		}

		spacesepstream(conf->GetModule(this)->Get<const Anope::string>("defaults", "greet fantasy")).GetTokens(defaults);
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

	void OnBotDelete(BotInfo *bi) override
	{
		if (bi == ChanServ)
			ChanServ = NULL;
	}

	EventReturn OnBotPrivmsg(User *u, BotInfo *bi, Anope::string &message) override
	{
		if (bi == ChanServ && Config->GetModule(this)->Get<bool>("opersonly") && !u->HasMode("OPER"))
		{
			u->SendMessage(bi, ACCESS_DENIED);
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnDelCore(NickServ::Account *nc) override
	{
		std::deque<ChanServ::Channel *> chans;
		nc->GetChannelReferences(chans);
		int max_reg = Config->GetModule(this)->Get<int>("maxregistered");

		for (unsigned i = 0; i < chans.size(); ++i)
		{
			ChanServ::Channel *ci = chans[i];

			if (ci->GetFounder() == nc)
			{
				NickServ::Account *newowner = NULL;
				if (ci->GetSuccessor() && ci->GetSuccessor() != nc && (ci->GetSuccessor()->IsServicesOper() || !max_reg || ci->GetSuccessor()->channelcount < max_reg))
					newowner = ci->GetSuccessor();
				else
				{
					const ChanServ::ChanAccess *highest = NULL;
					for (unsigned j = 0; j < ci->GetAccessCount(); ++j)
					{
						const ChanServ::ChanAccess *ca = ci->GetAccess(j);
						const NickServ::Account *anc = NickServ::FindAccount(ca->mask);

						if (!anc || (!anc->IsServicesOper() && max_reg && anc->channelcount >= max_reg) || (anc == nc))
							continue;
						if (!highest || *ca > *highest)
							highest = ca;
					}
					if (highest)
						newowner = NickServ::FindAccount(highest->mask);
				}

				if (newowner)
				{
					::Log(LOG_NORMAL, "chanserv/drop", ChanServ) << "Transferring foundership of " << ci->name << " from deleted nick " << nc->display << " to " << newowner->display;
					ci->SetFounder(newowner);
					ci->SetSuccessor(NULL);
				}
				else
				{
					::Log(LOG_NORMAL, "chanserv/drop", ChanServ) << "Deleting channel " << ci->name << " owned by deleted nick " << nc->display;

					delete ci;
					continue;
				}
			}

			if (ci->GetSuccessor() == nc)
				ci->SetSuccessor(NULL);

			for (unsigned j = 0; j < ci->GetAccessCount(); ++j)
			{
				const ChanServ::ChanAccess *ca = ci->GetAccess(j);
				const NickServ::Account *anc = NickServ::FindAccount(ca->mask);

				if (anc && anc == nc)
				{
					delete ci->EraseAccess(j);
					break;
				}
			}

			for (unsigned j = 0; j < ci->GetAkickCount(); ++j)
			{
				const AutoKick *ak = ci->GetAkick(j);
				if (ak->nc == nc)
				{
					ci->EraseAkick(j);
					break;
				}
			}
		}
	}

	void OnDelChan(ChanServ::Channel *ci) override
	{
		/* remove access entries that are this channel */

		std::deque<Anope::string> chans;
		ci->GetChannelReferences(chans);

		for (unsigned i = 0; i < chans.size(); ++i)
		{
			ChanServ::Channel *c = ChanServ::Find(chans[i]);
			if (!c)
				continue;

			for (unsigned j = 0; j < c->GetAccessCount(); ++j)
			{
				ChanServ::ChanAccess *a = c->GetAccess(j);

				if (a->mask.equals_ci(ci->name))
				{
					delete a;
					break;
				}
			}
		}

		if (ci->c)
		{
			ci->c->RemoveMode(ci->WhoSends(), "REGISTERED", "", false);

			const Anope::string &require = Config->GetModule(this)->Get<const Anope::string>("require");
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

		const Anope::string &require = Config->GetModule(this)->Get<const Anope::string>("require");
		if (!require.empty())
		{
			if (c->ci)
				c->SetModes(c->ci->WhoSends(), false, "+%s", require.c_str());
			else
				c->SetModes(c->ci->WhoSends(), false, "-%s", require.c_str());
		}
	}

	void OnCreateChan(ChanServ::Channel *ci) override
	{
		/* Set default chan flags */
		for (unsigned i = 0; i < defaults.size(); ++i)
			ci->Extend<bool>(defaults[i].upper());
	}

	EventReturn OnCanSet(User *u, const ChannelMode *cm) override
	{
		if (Config->GetModule(this)->Get<const Anope::string>("nomlock").find(cm->mchar) != Anope::string::npos
			|| Config->GetModule(this)->Get<const Anope::string>("require").find(cm->mchar) != Anope::string::npos)
			return EVENT_STOP;
		return EVENT_CONTINUE;
	}

	void OnChannelSync(Channel *c) override
	{
		bool perm = c->HasMode("PERM") || (c->ci && persist && persist->HasExt(c->ci));
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

		for (auto it = registered_channels->begin(); it != registered_channels->end();)
		{
			ChanServ::Channel *ci = it->second;
			++it;

			bool expire = false;

			if (Anope::CurTime - ci->last_used >= chanserv_expire)
			{
				if (ci->c)
				{
					time_t last_used = ci->last_used;
					for (Channel::ChanUserList::const_iterator cit = ci->c->users.begin(), cit_end = ci->c->users.end(); cit != cit_end && last_used == ci->last_used; ++cit)
						ci->AccessFor(cit->second->user);
					expire = last_used == ci->last_used;
				}
				else
					expire = true;
			}

			this->OnPreChanExpire(&ChanServ::Event::PreChanExpire::OnPreChanExpire, ci, expire);

			if (expire)
			{
				::Log(LOG_NORMAL, "chanserv/expire", ChanServ) << "Expiring channel " << ci->name << " (founder: " << (ci->GetFounder() ? ci->GetFounder()->display : "(none)") << ")";
				this->OnChanExpire(&ChanServ::Event::ChanExpire::OnChanExpire, ci);
				delete ci;
			}
		}
	}

	EventReturn OnCheckDelete(Channel *c) override
	{
		/* Do not delete this channel if ChanServ/a BotServ bot is inhabiting it */
		if (inhabit.HasExt(c))
			return EVENT_STOP;

		/* Channel is persistent, it shouldn't be deleted and the service bot should stay */
		if (c->ci && persist && persist->Get(c->ci))
			return EVENT_STOP;

		return EVENT_CONTINUE;
	}

	void OnPreUplinkSync(Server *serv) override
	{
		if (!persist)
			return;
		/* Find all persistent channels and create them, as we are about to finish burst to our uplink */
		for (auto& it : *registered_channels)
		{
			ChanServ::Channel *ci = it.second;
			if (persist->Get(ci))
			{
				bool c;
				ci->c = Channel::FindOrCreate(ci->name, c, ci->time_registered);

				if (ModeManager::FindChannelModeByName("PERM") != NULL)
				{
					if (c)
						IRCD->SendChannel(ci->c);
					ci->c->SetMode(NULL, "PERM");
				}
				else
				{
					if (!ci->bi)
						ci->WhoSends()->Assign(NULL, ci);
					if (ci->c->FindUser(ci->bi) == NULL)
					{
						ChannelStatus status(Config->GetModule("botserv")->Get<const Anope::string>("botmodes"));
						ci->bi->Join(ci->c, &status);
					}
				}
			}
		}

	}

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		if (!persist || !ci->c)
			return;
		/* Mark the channel as persistent */
		if (ci->c->HasMode("PERM"))
			persist->Set(ci);
		/* Persist may be in def cflags, set it here */
		else if (persist->Get(ci))
			ci->c->SetMode(NULL, "PERM");
	}

	void OnJoinChannel(User *u, Channel *c) override
	{
		if (always_lower && c->ci && c->creation_time > c->ci->time_registered)
		{
			::Log(LOG_DEBUG) << "Changing TS of " << c->name << " from " << c->creation_time << " to " << c->ci->time_registered;
			c->creation_time = c->ci->time_registered;
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
		if (!ci->HasExt("CS_NO_EXPIRE") && chanserv_expire && !Anope::NoExpire && ci->last_used != Anope::CurTime)
			info[_("Expires")] = Anope::strftime(ci->last_used + chanserv_expire, source.GetAccount());
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

