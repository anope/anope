/* ChanServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class ExpireCallback : public Timer
{
 public:
	ExpireCallback(Module *o) : Timer(o, Config->GetBlock("options")->Get<time_t>("expiretimeout"), Anope::CurTime, true) { }

	void Tick(time_t) anope_override
	{
		time_t chanserv_expire = Config->GetModule("chanserv")->Get<time_t>("expire", "14d");

		if (!chanserv_expire || Anope::NoExpire || Anope::ReadOnly)
			return;

		for (registered_channel_map::const_iterator it = RegisteredChannelList->begin(), it_end = RegisteredChannelList->end(); it != it_end; )
		{
			ChannelInfo *ci = it->second;
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

			if (ci->HasExt("NO_EXPIRE"))
				expire = false;

			FOREACH_MOD(I_OnPreChanExpire, OnPreChanExpire(ci, expire));

			if (expire)
			{
				Anope::string extra;
				if (ci->HasExt("SUSPENDED"))
					extra = "suspended ";

				Log(LOG_NORMAL, "chanserv/expire") << "Expiring " << extra  << "channel " << ci->name << " (founder: " << (ci->GetFounder() ? ci->GetFounder()->display : "(none)") << ")";
				FOREACH_MOD(I_OnChanExpire, OnChanExpire(ci));
				delete ci;
			}
		}
	}
};

class MyChanServService : public ChanServService
{
 public:
	MyChanServService(Module *m) : ChanServService(m) { }

	void Hold(Channel *c) anope_override
	{
		/** A timer used to keep the BotServ bot/ChanServ in the channel
		 * after kicking the last user in a channel
		 */
		class ChanServTimer : public Timer
		{
			Reference<Channel> c;

		 public:
		 	/** Constructor
			 * @param chan The channel
			 */
			ChanServTimer(Module *m, Channel *chan) : Timer(m, Config->GetModule(m)->Get<time_t>("inhabit", "15s")), c(chan)
			{
				if (!ChanServ || !c)
					return;
				c->Extend("INHABIT");
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
			void Tick(time_t) anope_override
			{
				if (!c)
					return;

				c->Shrink("INHABIT");

				if (!c->ci || !c->ci->bi)
				{
					if (ChanServ)
						ChanServ->Part(c);
				}
				else
					c->ci->bi->Part(c);
			}
		};

		if (c->HasExt("INHABIT"))
			return;

		new ChanServTimer(this->owner, c);
	}
};

class ChanServCore : public Module
{
	MyChanServService chanserv;
	ExpireCallback expires;
	std::vector<Anope::string> defaults;

 public:
	ChanServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR),
		chanserv(this), expires(this)
	{
		Implementation i[] = { I_OnReload, I_OnBotDelete, I_OnBotPrivmsg, I_OnDelCore,
			I_OnPreHelp, I_OnPostHelp, I_OnCheckModes, I_OnCreateChan, I_OnCanSet,
			I_OnChannelSync, I_OnBotKick };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	~ChanServCore()
	{
		ChanServ = NULL;
	}

	void OnReload(Configuration::Conf *conf) anope_override
	{
		const Anope::string &channick = conf->GetModule(this)->Get<const Anope::string>("client");

		if (channick.empty())
			throw ConfigException(this->name + ": <client> must be defined");

		BotInfo *bi = BotInfo::Find(channick, true);
		if (!bi)
			throw ConfigException(this->name + ": no bot named " + channick);

		ChanServ = bi;

		spacesepstream(conf->GetModule(this)->Get<const Anope::string>("defaults", "greet fantasy")).GetTokens(defaults);
		if (defaults.empty())
		{
			defaults.push_back("KEEPTOPIC");
			defaults.push_back("SECURE");
			defaults.push_back("SECUREFOUNDER");
			defaults.push_back("SIGNKICK");
		}
		else if (defaults[0].equals_ci("none"))
			defaults.erase(defaults.begin());
	}

	void OnBotDelete(BotInfo *bi) anope_override
	{
		if (bi == ChanServ)
			ChanServ = NULL;
	}

	EventReturn OnBotPrivmsg(User *u, BotInfo *bi, Anope::string &message) anope_override
	{
		if (bi == ChanServ && Config->GetModule(this)->Get<bool>("operonly") && !u->HasMode("OPER"))
		{
			u->SendMessage(bi, ACCESS_DENIED);
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnDelCore(NickCore *nc) anope_override
	{
		std::deque<ChannelInfo *> chans;
		nc->GetChannelReferences(chans);
		int max_reg = Config->GetModule(this)->Get<int>("maxregistered");

		for (unsigned i = 0; i < chans.size(); ++i)
		{
			ChannelInfo *ci = chans[i];

			if (ci->GetFounder() == nc)
			{
				NickCore *newowner = NULL;
				if (ci->GetSuccessor() && ci->GetSuccessor() != nc && (ci->GetSuccessor()->IsServicesOper() || !max_reg || ci->GetSuccessor()->channelcount < max_reg))
					newowner = ci->GetSuccessor();
				else
				{
					const ChanAccess *highest = NULL;
					for (unsigned j = 0; j < ci->GetAccessCount(); ++j)
					{
						const ChanAccess *ca = ci->GetAccess(j);
						const NickCore *anc = NickCore::Find(ca->mask);

						if (!anc || (!anc->IsServicesOper() && max_reg && anc->channelcount >= max_reg) || (anc == nc))
							continue;
						if (!highest || *ca > *highest)
							highest = ca;
					}
					if (highest)
						newowner = NickCore::Find(highest->mask);
				}

				if (newowner)
				{
					Log(LOG_NORMAL, "chanserv/expire") << "Transferring foundership of " << ci->name << " from deleted nick " << nc->display << " to " << newowner->display;
					ci->SetFounder(newowner);
					ci->SetSuccessor(NULL);
				}
				else
				{
					Log(LOG_NORMAL, "chanserv/expire") << "Deleting channel " << ci->name << " owned by deleted nick " << nc->display;

					delete ci;
					continue;
				}
			}

			if (ci->GetSuccessor() == nc)
				ci->SetSuccessor(NULL);

			for (unsigned j = 0; j < ci->GetAccessCount(); ++j)
			{
				const ChanAccess *ca = ci->GetAccess(j);
				const NickCore *anc = NickCore::Find(ca->mask);

				if (anc && anc == nc)
				{
					ci->EraseAccess(j);
					break;
				}
			}

			for (unsigned j = 0; j < ci->GetAkickCount(); ++j)
			{
				const AutoKick *akick = ci->GetAkick(j);
				if (akick->nc == nc)
				{
					ci->EraseAkick(j);
					break;
				}
			}
		}
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!params.empty() || source.c || source.service != ChanServ)
			return EVENT_CONTINUE;
		source.Reply(_("\002%s\002 allows you to register and control various\n"
			"aspects of channels. %s can often prevent\n"
			"malicious users from \"taking over\" channels by limiting\n"
			"who is allowed channel operator privileges. Available\n"
			"commands are listed below; to use them, type\n"
			"\002%s%s \037command\037\002. For more information on a\n"
			"specific command, type \002%s%s HELP \037command\037\002.\n "),
			ChanServ->nick.c_str(), ChanServ->nick.c_str(), Config->StrictPrivmsg.c_str(), ChanServ->nick.c_str(), Config->StrictPrivmsg.c_str(), ChanServ->nick.c_str(), ChanServ->nick.c_str(), source.command.c_str());
		return EVENT_CONTINUE;
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!params.empty() || source.c || source.service != ChanServ)
			return;
		time_t expire = Config->GetModule(this)->Get<time_t>("expire", "14d");
		if (expire >= 86400)
			source.Reply(_(" \n"
				"Note that any channel which is not used for %d days\n"
				"(i.e. which no user on the channel's access list enters\n"
				"for that period of time) will be automatically dropped."), expire / 86400);
		if (source.IsServicesOper())
			source.Reply(_(" \n"
				"Services Operators can also, depending on their access drop\n"
				"any channel, view (and modify) the access, levels and akick\n"
				"lists and settings for any channel."));
	}

	EventReturn OnCheckModes(Channel *c) anope_override
	{
		const Anope::string &require = Config->GetModule(this)->Get<const Anope::string>("require", "r");
		if (!require.empty())
		{
			if (c->ci)
				c->SetModes(NULL, false, "+%s", require.c_str());
			else
				c->SetModes(NULL, false, "-%s", require.c_str());
		}

		return EVENT_CONTINUE;
	}

	void OnCreateChan(ChannelInfo *ci) anope_override
	{
		ci->bantype = Config->GetModule(this)->Get<int>("defbantype", "2");

		/* Set default chan flags */
		for (unsigned i = 0; i < defaults.size(); ++i)
			this->ExtendMetadata(defaults[i]);

		{
			Anope::string modes;
			spacesepstream sep(Config->GetModule(this)->Get<const Anope::string>("mlock", "+nrt"));
			if (sep.GetToken(modes))
			{
				bool add = true;
				for (unsigned i = 0; i < modes.length(); ++i)
				{
					if (modes[i] == '+')
						add = true;
					else if (modes[i] == '-')
						add = false;
					else
					{
						ChannelMode *cm = ModeManager::FindChannelModeByChar(modes[i]);
						Anope::string param;
						if (cm && (cm->type == MODE_REGULAR || sep.GetToken(param)))
							ci->SetMLock(cm, add, param);
					}
				}
			}
		}
	}

	EventReturn OnCanSet(User *u, const ChannelMode *cm) anope_override
	{
		if (Config->GetModule(this)->Get<const Anope::string>("nomlock").find(cm->mchar) != Anope::string::npos
			|| Config->GetModule(this)->Get<const Anope::string>("require", "r").find(cm->mchar) || Anope::string::npos)
			return EVENT_STOP;
		return EVENT_CONTINUE;
	}

	void OnChannelSync(Channel *c) anope_override
	{
		if (!c->HasMode("PERM") && (c->users.empty() || (c->users.size() == 1 && c->ci && c->ci->bi && *c->ci->bi == c->users.begin()->second->user)))
		{
			chanserv.Hold(c);
		}
		if (c->ci)
		{
			c->CheckModes();

			if (Me && Me->IsSynced())
				c->ci->RestoreTopic();
		}
	}

	EventReturn OnBotKick(BotInfo *bi, Channel *c, User *u, const Anope::string &reason)
	{
		/* If the channel isn't syncing and doesn't have any users, join ChanServ
		 * Note that the user AND POSSIBLY the botserv bot exist here
		 * ChanServ always enforces channels like this to keep people from deleting bots etc
		 * that are holding channels.
		 */
		if (c->ci && c->users.size() == (c->ci->bi && c->FindUser(c->ci->bi) ? 2 : 1) && !c->HasExt("INHABIT") && !c->HasExt("SYNCING"))
		{
			/* Join ChanServ and set a timer for this channel to part ChanServ later */
			chanserv.Hold(c);
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ChanServCore)

