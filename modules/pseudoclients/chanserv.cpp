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
	ExpireCallback(Module *o) : Timer(o, Config->ExpireTimeout, Anope::CurTime, true) { }

	void Tick(time_t) anope_override
	{
		if (!Config->CSExpire || Anope::NoExpire || Anope::ReadOnly)
			return;

		for (registered_channel_map::const_iterator it = RegisteredChannelList->begin(), it_end = RegisteredChannelList->end(); it != it_end; )
		{
			ChannelInfo *ci = it->second;
			++it;

			bool expire = false;

			if (Anope::CurTime - ci->last_used >= Config->CSExpire)
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

class ChanServCore : public Module
{
	ExpireCallback expires;

 public:
	ChanServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR), expires(this)
	{

		ChanServ  = BotInfo::Find(Config->ChanServ);
		if (!ChanServ)
			throw ModuleException("No bot named " + Config->ChanServ);

		Implementation i[] = { I_OnBotDelete, I_OnBotPrivmsg, I_OnDelCore, I_OnPreHelp, I_OnPostHelp, I_OnCheckModes };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	~ChanServCore()
	{
		ChanServ = NULL;
	}

	void OnBotDelete(BotInfo *bi) anope_override
	{
		if (bi == ChanServ)
			ChanServ = NULL;
	}

	EventReturn OnBotPrivmsg(User *u, BotInfo *bi, Anope::string &message) anope_override
	{
		if (Config->CSOpersOnly && bi == ChanServ && !u->HasMode("OPER"))
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

		for (unsigned i = 0; i < chans.size(); ++i)
		{
			ChannelInfo *ci = chans[i];

			if (ci->GetFounder() == nc)
			{
				NickCore *newowner = NULL;
				if (ci->GetSuccessor() && ci->GetSuccessor() != nc && (ci->GetSuccessor()->IsServicesOper() || !Config->CSMaxReg || ci->GetSuccessor()->channelcount < Config->CSMaxReg))
					newowner = ci->GetSuccessor();
				else
				{
					const ChanAccess *highest = NULL;
					for (unsigned j = 0; j < ci->GetAccessCount(); ++j)
					{
						const ChanAccess *ca = ci->GetAccess(j);
						const NickCore *anc = NickCore::Find(ca->mask);

						if (!anc || (!anc->IsServicesOper() && Config->CSMaxReg && anc->channelcount >= Config->CSMaxReg) || (anc == nc))
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
		if (!params.empty() || source.c || source.service->nick != Config->ChanServ)
			return EVENT_CONTINUE;
		source.Reply(_("\002%s\002 allows you to register and control various\n"
			"aspects of channels. %s can often prevent\n"
			"malicious users from \"taking over\" channels by limiting\n"
			"who is allowed channel operator privileges. Available\n"
			"commands are listed below; to use them, type\n"
			"\002%s%s \037command\037\002. For more information on a\n"
			"specific command, type \002%s%s HELP \037command\037\002.\n "),
			Config->ChanServ.c_str(), Config->ChanServ.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->ChanServ.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->ChanServ.c_str(), Config->ChanServ.c_str(), source.command.c_str());
		return EVENT_CONTINUE;
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!params.empty() || source.c || source.service->nick != Config->ChanServ)
			return;
		if (Config->CSExpire >= 86400)
			source.Reply(_(" \n"
				"Note that any channel which is not used for %d days\n"
				"(i.e. which no user on the channel's access list enters\n"
				"for that period of time) will be automatically dropped."), Config->CSExpire / 86400);
		if (source.IsServicesOper())
			source.Reply(_(" \n"
				"Services Operators can also, depending on their access drop\n"
				"any channel, view (and modify) the access, levels and akick\n"
				"lists and settings for any channel."));
	}

	EventReturn OnCheckModes(Channel *c) anope_override
	{
		if (!Config->CSRequire.empty())
		{
			if (c->ci)
				c->SetModes(NULL, false, "+%s", Config->CSRequire.c_str());
			else
				c->SetModes(NULL, false, "-%s", Config->CSRequire.c_str());
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ChanServCore)

