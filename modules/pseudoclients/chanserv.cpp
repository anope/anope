/* ChanServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class ExpireCallback : public CallBack
{
 public:
	ExpireCallback(Module *owner) : CallBack(owner, Config->ExpireTimeout, Anope::CurTime, true) { }

	void Tick(time_t) anope_override
	{
		if (!Config->CSExpire || Anope::NoExpire || Anope::ReadOnly)
			return;

		for (registered_channel_map::const_iterator it = RegisteredChannelList->begin(), it_end = RegisteredChannelList->end(); it != it_end; )
		{
			ChannelInfo *ci = it->second;
			++it;

			bool expire = false;

			if (!ci->c && Anope::CurTime - ci->last_used >= Config->CSExpire)
				expire = true;

			if (ci->HasFlag(CI_NO_EXPIRE))
				expire = false;

			FOREACH_MOD(I_OnPreChanExpire, OnPreChanExpire(ci, expire));

			if (expire)
			{
				Anope::string extra;
				if (ci->HasFlag(CI_SUSPENDED))
					extra = "suspended ";

				Log(LOG_NORMAL, "chanserv/expire") << "Expiring " << extra  << "channel " << ci->name << " (founder: " << (ci->GetFounder() ? ci->GetFounder()->display : "(none)") << ")";
				FOREACH_MOD(I_OnChanExpire, OnChanExpire(ci));
				ci->Destroy();
			}
		}
	}
};

class ChanServCore : public Module
{
	ExpireCallback expires;

 public:
	ChanServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), expires(this)
	{
		this->SetAuthor("Anope");

		ChanServ = BotInfo::Find(Config->ChanServ);
		if (!ChanServ)
			throw ModuleException("No bot named " + Config->ChanServ);

		Implementation i[] = { I_OnBotPrivmsg, I_OnDelCore, I_OnPreHelp, I_OnPostHelp, I_OnCheckModes };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	EventReturn OnBotPrivmsg(User *u, BotInfo *bi, Anope::string &message) anope_override
	{
		if (Config->CSOpersOnly && !u->HasMode(UMODE_OPER) && bi->nick == Config->ChanServ)
		{
			u->SendMessage(bi, ACCESS_DENIED);
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnDelCore(NickCore *nc) anope_override
	{
		// XXX this is slightly inefficient
		for (registered_channel_map::const_iterator it = RegisteredChannelList->begin(), it_end = RegisteredChannelList->end(); it != it_end;)
		{
			ChannelInfo *ci = it->second;
			++it;

			if (ci->GetFounder() == nc)
			{
				NickCore *newowner = NULL;
				if (ci->successor && (ci->successor->IsServicesOper() || !Config->CSMaxReg || ci->successor->channelcount < Config->CSMaxReg))
					newowner = ci->successor;
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
					ci->successor = NULL;
				}
				else
				{
					Log(LOG_NORMAL, "chanserv/expire") << "Deleting channel " << ci->name << " owned by deleted nick " << nc->display;

					ci->Destroy();
					continue;
				}
			}

			if (ci->successor == nc)
				ci->successor = NULL;

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

			for (unsigned j = ci->GetAkickCount(); j > 0; --j)
			{
				const AutoKick *akick = ci->GetAkick(j - 1);
				if (akick->HasFlag(AK_ISNICK) && akick->nc == nc)
					ci->EraseAkick(j - 1);
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

