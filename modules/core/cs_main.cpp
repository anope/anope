/* ChanServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class ChanServCore : public Module
{
 public:
	ChanServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		BotInfo *ChanServ = findbot(Config->ChanServ);
		if (ChanServ == NULL)
			throw ModuleException("No bot named " + Config->ChanServ);

		Implementation i[] = { I_OnDelChan, I_OnPreHelp, I_OnPostHelp };
		ModuleManager::Attach(i, this, 3);
	}

	void OnDelCore(NickCore *nc)
	{
		// XXX this is slightly inefficient
		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end;)
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
					ChanAccess *highest = NULL;
					for (unsigned j = 0; j < ci->GetAccessCount(); ++j)
					{
						ChanAccess *ca = ci->GetAccess(j);
					
						if (!ca->nc || (!ca->nc->IsServicesOper() && Config->CSMaxReg && ca->nc->channelcount >= Config->CSMaxReg) || (ca->nc == nc))
							continue;
						if (!highest || ca->level > highest->level)
							highest = ca;
					}
					if (highest)
						newowner = highest->nc;
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

					delete ci;
					continue;
				}
			}

			if (ci->successor == nc)
				ci->successor = NULL;

			ChanAccess *access = ci->GetAccess(nc);
			if (access)
				ci->EraseAccess(access);

			for (unsigned j = ci->GetAkickCount(); j > 0; --j)
			{
				AutoKick *akick = ci->GetAkick(j - 1);
				if (akick->HasFlag(AK_ISNICK) && akick->nc == nc)
					ci->EraseAkick(j - 1);
			}
		}
	}

	void OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!params.empty() || source.owner->nick != Config->ChanServ)
			return;
		source.Reply(_("\002%s\002 allows you to register and control various\n"
			"aspects of channels. %s can often prevent\n"
			"malicious users from \"taking over\" channels by limiting\n"
			"who is allowed channel operator privileges. Available\n"
			"commands are listed below; to use them, type\n"
			"\002%s%s \037command\037\002. For more information on a\n"
			"specific command, type \002%s%s HELP \037command\037\002.\n "),
			Config->ChanServ.c_str(), Config->ChanServ.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->ChanServ.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->ChanServ.c_str(), Config->ChanServ.c_str(), source.command.c_str());
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!params.empty() || source.owner->nick != Config->ChanServ)
			return;
		if (Config->CSExpire >= 86400)
			source.Reply(_(" \n"
				"Note that any channel which is not used for %d days\n"
				"(i.e. which no user on the channel's access list enters\n"
				"for that period of time) will be automatically dropped."), Config->CSExpire / 86400);
		if (source.u->IsServicesOper())
			source.Reply(_(" \n"
				"Services Operators can also drop any channel without needing\n"
				"to identify via password, and may view the access, akick,\n"
				"and level setting lists for any channel."));
	}
};

MODULE_INIT(ChanServCore)

