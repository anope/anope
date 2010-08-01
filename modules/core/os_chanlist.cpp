/* OperServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandOSChanList : public Command
{
 public:
	CommandOSChanList() : Command("CHANLIST", 0, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string pattern = !params.empty() ? params[0] : "";
		Anope::string opt = params.size() > 1 ? params[1] : "";
		std::list<ChannelModeName> Modes;
		User *u2;

		if (!opt.empty() && opt.equals_ci("SECRET"))
		{
			Modes.push_back(CMODE_SECRET);
			Modes.push_back(CMODE_PRIVATE);
		}

		if (!pattern.empty() && (u2 = finduser(pattern)))
		{
			notice_lang(Config.s_OperServ, u, OPER_CHANLIST_HEADER_USER, u2->nick.c_str());

			for (UChannelList::iterator uit = u2->chans.begin(), uit_end = u2->chans.end(); uit != uit_end; ++uit)
			{
				ChannelContainer *cc = *uit;

				if (!Modes.empty())
					for (std::list<ChannelModeName>::iterator it = Modes.begin(), it_end = Modes.end(); it != it_end; ++it)
						if (!cc->chan->HasMode(*it))
							continue;

				notice_lang(Config.s_OperServ, u, OPER_CHANLIST_RECORD, cc->chan->name.c_str(), cc->chan->users.size(), chan_get_modes(cc->chan, 1, 1).c_str(), !cc->chan->topic.empty() ? cc->chan->topic.c_str() : "");
			}
		}
		else
		{
			notice_lang(Config.s_OperServ, u, OPER_CHANLIST_HEADER);

			for (channel_map::const_iterator cit = ChannelList.begin(), cit_end = ChannelList.end(); cit != cit_end; ++cit)
			{
				Channel *c = cit->second;

				if (!pattern.empty() && !Anope::Match(c->name, pattern))
					continue;
				if (!Modes.empty())
					for (std::list<ChannelModeName>::iterator it = Modes.begin(), it_end = Modes.end(); it != it_end; ++it)
						if (!c->HasMode(*it))
							continue;

				notice_lang(Config.s_OperServ, u, OPER_CHANLIST_RECORD, c->name.c_str(), c->users.size(), chan_get_modes(c, 1, 1).c_str(), !c->topic.empty() ? c->topic.c_str() : "");
			}
		}

		notice_lang(Config.s_OperServ, u, OPER_CHANLIST_END);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_CHANLIST);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_CHANLIST);
	}
};

class OSChanList : public Module
{
	CommandOSChanList commandoschanlist;

 public:
	OSChanList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandoschanlist);
	}
};

MODULE_INIT(OSChanList)
