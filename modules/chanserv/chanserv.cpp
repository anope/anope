/* ChanServ core functions
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
#include "modules/chanserv/mode.h"

inline static Anope::string BotModes()
{
	return Config->GetModule("botserv").Get<Anope::string>("botmodes",
		Config->GetModule("chanserv").Get<Anope::string>("botmodes", "o")
	);
}

class ChanServCore final
	: public Module
	, public ChanServService
{
	Reference<BotInfo> ChanServ;
	std::vector<Anope::string> defaults;
	ExtensibleItem<bool> inhabit;
	ExtensibleRef<bool> persist;
	bool always_lower = false;

public:
	ChanServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR),
		ChanServService(this), inhabit(this, "inhabit"), persist("PERSIST")
	{
	}

	void Hold(Channel *c) override
	{
		/** A timer used to keep the BotServ bot/ChanServ in the channel
		 * after kicking the last user in a channel
		 */
		class ChanServTimer final
			: public Timer
		{
			Reference<BotInfo> &ChanServ;
			ExtensibleItem<bool> &inhabit;
			Reference<Channel> c;

		public:
			/** Constructor
			 * @param chan The channel
			 */
			ChanServTimer(Reference<BotInfo> &cs, ExtensibleItem<bool> &i, Module *m, Channel *chan)
				: Timer(m, Config->GetModule(m).Get<time_t>("inhabit", "1m"))
				, ChanServ(cs)
				, inhabit(i)
				, c(chan)
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
			void Tick() override
			{
				if (!c)
					return;

				/* In the event we don't part */
				c->RemoveMode(NULL, "SECRET");
				c->RemoveMode(NULL, "INVITE");

				inhabit.Unset(c); /* now we're done changing modes, unset inhabit */

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

	void OnReload(Configuration::Conf &conf) override
	{
		const Anope::string &channick = conf.GetModule(this).Get<const Anope::string>("client");

		if (channick.empty())
			throw ConfigException(Module::name + ": <client> must be defined");

		BotInfo *bi = BotInfo::Find(channick, true);
		if (!bi)
			throw ConfigException(Module::name + ": no bot named " + channick);

		ChanServ = bi;

		spacesepstream(conf.GetModule(this).Get<const Anope::string>("defaults")).GetTokens(defaults);
		if (defaults.empty())
		{
			defaults = {
				"CS_KEEP_MODES",
				"KEEPTOPIC",
				"PEACE",
				"SECUREFOUNDER",
				"SIGNKICK",
			};
		}
		else if (defaults[0].equals_ci("none"))
			defaults.clear();

		always_lower = conf.GetModule(this).Get<bool>("always_lower_ts");
	}

	void OnBotDelete(BotInfo *bi) override
	{
		if (bi == ChanServ)
			ChanServ = NULL;
	}

	EventReturn OnBotPrivmsg(User *u, BotInfo *bi, Anope::string &message, const Anope::map<Anope::string> &tags) override
	{
		if (bi == ChanServ && Config->GetModule(this).Get<bool>("opersonly") && !u->HasMode("OPER"))
		{
			u->SendMessage(bi, ACCESS_DENIED);
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnDelCore(NickCore *nc) override
	{
		std::deque<ChannelInfo *> chans;
		nc->GetChannelReferences(chans);
		auto max_reg = Config->GetModule(this).Get<uint16_t>("maxregistered");

		for (auto *ci : chans)
		{
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
						NickCore *anc = ca->GetAccount();

						if (!anc || (!anc->IsServicesOper() && max_reg && anc->channelcount >= max_reg) || (anc == nc))
							continue;
						if (!highest || *ca > *highest)
							highest = ca;
					}
					if (highest)
						newowner = highest->GetAccount();
				}

				if (newowner)
				{
					Log(LOG_NORMAL, "chanserv/drop", ChanServ) << "Transferring foundership of " << ci->name << " from deleted nick " << nc->display << " to " << newowner->display;
					ci->SetFounder(newowner);
					ci->SetSuccessor(NULL);
				}
				else
				{
					Log(LOG_NORMAL, "chanserv/drop", ChanServ) << "Deleting channel " << ci->name << " owned by deleted nick " << nc->display;

					delete ci;
					continue;
				}
			}

			if (ci->GetSuccessor() == nc)
				ci->SetSuccessor(NULL);

			for (unsigned j = 0; j < ci->GetAccessCount(); ++j)
			{
				const ChanAccess *ca = ci->GetAccess(j);

				if (ca->GetAccount() == nc)
				{
					delete ci->EraseAccess(j);
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

	void OnDelChan(ChannelInfo *ci) override
	{
		/* remove access entries that are this channel */

		std::deque<Anope::string> chans;
		ci->GetChannelReferences(chans);

		for (const auto &chan : chans)
		{
			ChannelInfo *c = ChannelInfo::Find(chan);
			if (!c)
				continue;

			for (unsigned j = 0; j < c->GetAccessCount(); ++j)
			{
				ChanAccess *a = c->GetAccess(j);

				if (a->Mask().equals_ci(ci->name))
				{
					delete a;
					break;
				}
			}
		}

		if (ci->c)
		{
			ci->c->RemoveMode(ci->WhoSends(), "REGISTERED", "", false);

			const Anope::string &require = Config->GetModule(this).Get<const Anope::string>("require");
			if (!require.empty())
				ci->c->SetModes(ci->WhoSends(), false, "-%s", require.c_str());
		}
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *ChanServ)
			return EVENT_CONTINUE;
		source.Reply(_(
				"\002%s\002 allows you to register and control various "
				"aspects of channels. %s can often prevent "
				"malicious users from \"taking over\" channels by limiting "
				"who is allowed channel operator privileges. Available "
				"commands are listed below; to use them, type "
				"\002%s\032\037command\037\002. For more information on a "
				"specific command, type \002%s\032\037command\037\002."
			),
			ChanServ->nick.c_str(),
			ChanServ->nick.c_str(),
			ChanServ->GetQueryCommand().c_str(),
			ChanServ->GetQueryCommand("generic/help").c_str());
		return EVENT_CONTINUE;
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *ChanServ)
			return;

		time_t chanserv_expire = Config->GetModule(this).Get<time_t>("expire", "30d");
		if (chanserv_expire)
		{
			source.Reply(" ");
			source.Reply(_(
					"Note that any channel which is not used for %s "
					"(i.e. which no user on the channel's access list enters "
					"for that period of time) will be automatically dropped."
				),
				Anope::Duration(chanserv_expire, source.nc).c_str());
		}
		if (source.IsServicesOper())
		{
			source.Reply(" ");
			source.Reply(_(
				"Services Operators can also, depending on their access drop "
				"any channel, view (and modify) the access, levels and akick "
				"lists and settings for any channel."
			));
		}
	}

	void OnCheckModes(Reference<Channel> &c) override
	{
		if (!c)
			return;

		if (c->ci)
			c->SetMode(c->ci->WhoSends(), "REGISTERED", {}, false);
		else
			c->RemoveMode(c->WhoSends(), "REGISTERED", "", false);

		const Anope::string &require = Config->GetModule(this).Get<const Anope::string>("require");
		if (!require.empty())
		{
			if (c->ci)
				c->SetModes(c->ci->WhoSends(), false, "+%s", require.c_str());
			else
				c->SetModes(c->WhoSends(), false, "-%s", require.c_str());
		}
	}

	void OnCreateChan(ChannelInfo *ci) override
	{
		/* Set default chan flags */
		for (const auto &def : defaults)
			ci->Extend<bool>(def.upper());
	}

	EventReturn OnCanSet(User *u, const ChannelMode *cm) override
	{
		if (Config->GetModule(this).Get<const Anope::string>("nomlock").find(cm->mchar) != Anope::string::npos
			|| Config->GetModule(this).Get<const Anope::string>("require").find(cm->mchar) != Anope::string::npos)
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

	void OnLog(Log *l) override
	{
		if (l->type == LOG_CHANNEL)
			l->bi = ChanServ;
	}

	void OnExpireTick() override
	{
		time_t chanserv_expire = Config->GetModule(this).Get<time_t>("expire", "30d");

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

			FOREACH_MOD(OnPreChanExpire, (ci, expire));

			if (expire)
			{
				Log(LOG_NORMAL, "chanserv/expire", ChanServ) << "Expiring channel " << ci->name << " (founder: " << (ci->GetFounder() ? ci->GetFounder()->display : "(none)") << ")";
				FOREACH_MOD(OnChanExpire, (ci));
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

	void OnUplinkSync(Server* s) override
	{
		// We need to do this when the uplink is synced as we may not know if
		// the mode exists before then on some IRCds (e.g. InspIRCd).

		if (!persist)
			return;

		ChannelMode *perm = ModeManager::FindChannelModeByName("PERM");

		/* Find all persistent channels and create them, as we are about to finish burst to our uplink */
		for (const auto &[_, ci] : *RegisteredChannelList)
		{
			if (!persist->HasExt(ci))
				continue;

			bool c;
			ci->c = Channel::FindOrCreate(ci->name, c, ci->registered);
			ci->c->syncing |= created;

			if (perm)
			{
				ci->c->SetMode(NULL, perm);
			}
			else
			{
				if (!ci->bi)
					ci->WhoSends()->Assign(NULL, ci);
				if (ci->c->FindUser(ci->bi) == NULL)
				{
					ChannelStatus status(BotModes());
					ci->bi->Join(ci->c, &status);
				}
			}
		}

	}

	void OnChanRegistered(ChannelInfo *ci) override
	{
		if (!persist || !ci->c)
			return;
		/* Mark the channel as persistent */
		if (ci->c->HasMode("PERM"))
			persist->Set(ci);
		/* Persist may be in def cflags, set it here */
		else if (persist->HasExt(ci))
			ci->c->SetMode(NULL, "PERM");
	}

	void OnJoinChannel(User *u, Channel *c) override
	{
		if (always_lower && c->ci && c->created > c->ci->registered)
		{
			Log(LOG_DEBUG) << "Changing TS of " << c->name << " from " << c->created << " to " << c->ci->registered;
			c->created = c->ci->registered;
			IRCD->SendChannel(c);
			c->Reset();
		}
	}

	EventReturn OnChannelModeSet(Channel *c, MessageSource &setter, ChannelMode *mode, const ModeData &data) override
	{
		if (!always_lower && Anope::CurTime == c->created && c->ci && setter.GetUser() && !setter.GetUser()->server->IsULined())
		{
			ChanUserContainer *cu = c->FindUser(setter.GetUser());
			ChannelMode *cm = ModeManager::FindChannelModeByName("OP");
			if (cu && cm && !cu->status.HasMode(cm->mchar))
			{
				/* Our -o and their mode change crossing, bounce their mode */
				c->RemoveMode(c->ci->WhoSends(), mode, data.value);
				/* We don't set mlocks until after the join has finished processing, it will stack with this change,
				 * so there isn't much for the user to remove except -nt etc which is likely locked anyway.
				 */
			}
		}

		return EVENT_CONTINUE;
	}

	void OnChanInfo(CommandSource &source, ChannelInfo *ci, InfoFormatter &info, bool show_all) override
	{
		if (!show_all)
			return;

		time_t chanserv_expire = Config->GetModule(this).Get<time_t>("expire", "30d");
		if (!ci->HasExt("CS_NO_EXPIRE") && chanserv_expire && !Anope::NoExpire && ci->last_used != Anope::CurTime)
			info[_("Expires")] = Anope::strftime(ci->last_used + chanserv_expire, source.GetAccount());
	}

	void OnSetCorrectModes(User *user, Channel *chan, AccessGroup &access, bool &give_modes, bool &take_modes) override
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
