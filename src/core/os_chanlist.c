/* OperServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandOSChanList : public Command
{
 public:
	CommandOSChanList() : Command("CHANLIST", 0, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *pattern = params.size() > 0 ? params[0].c_str() : NULL;
		ci::string opt = params.size() > 1 ? params[1] : "";
		std::list<ChannelModeName> Modes;
		User *u2;

		if (!opt.empty() && opt == "SECRET")
		{
			Modes.push_back(CMODE_SECRET);
			Modes.push_back(CMODE_PRIVATE);
		}

		if (pattern && (u2 = finduser(pattern)))
		{
			notice_lang(Config.s_OperServ, u, OPER_CHANLIST_HEADER_USER, u2->nick.c_str());

			for (UChannelList::iterator it = u2->chans.begin(); it != u2->chans.end(); ++it)
			{
				ChannelContainer *cc = *it;

				if (!Modes.empty())
				{
					for (std::list<ChannelModeName>::iterator it = Modes.begin(); it != Modes.end(); ++it)
					{
						if (!cc->chan->HasMode(*it))
							continue;
					}
				}

				notice_lang(Config.s_OperServ, u, OPER_CHANLIST_RECORD, cc->chan->name.c_str(), cc->chan->users.size(), chan_get_modes(cc->chan, 1, 1), cc->chan->topic ? cc->chan->topic : "");
			}
		}
		else
		{
			int i;
			Channel *c;

			notice_lang(Config.s_OperServ, u, OPER_CHANLIST_HEADER);

			for (i = 0; i < 1024; ++i)
			{
				for (c = chanlist[i]; c; c = c->next)
				{
					if (pattern && !Anope::Match(c->name, pattern, false))
						continue;
					if (!Modes.empty())
					{
						for (std::list<ChannelModeName>::iterator it = Modes.begin(); it != Modes.end(); ++it)
						{
							if (!c->HasMode(*it))
								continue;
						}
					}
					notice_lang(Config.s_OperServ, u, OPER_CHANLIST_RECORD, c->name.c_str(), c->users.size(), chan_get_modes(c, 1, 1), c->topic ? c->topic : "");
				}
			}
		}

		notice_lang(Config.s_OperServ, u, OPER_CHANLIST_END);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_CHANLIST);
		return true;
	}
};

class OSChanList : public Module
{
 public:
	OSChanList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSChanList());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_CHANLIST);
	}
};

MODULE_INIT(OSChanList)
