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

class CommandOSUserList : public Command
{
 public:
	CommandOSUserList() : Command("USERLIST", 0, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *pattern = params.size() > 0 ? params[0].c_str() : NULL;
		ci::string opt = params.size() > 1 ? params[1] : "";
		Channel *c;
		std::list<UserModeName> Modes;

		if (!opt.empty() && opt == "INVISIBLE")
			Modes.push_back(UMODE_INVIS);

		if (pattern && (c = findchan(pattern)))
		{
			notice_lang(Config.s_OperServ, u, OPER_USERLIST_HEADER_CHAN, pattern);

			for (CUserList::iterator cuit = c->users.begin(), cuit_end = c->users.end(); cuit != cuit_end; ++cuit)
			{
				UserContainer *uc = *cuit;

				if (!Modes.empty())
					for (std::list<UserModeName>::iterator it = Modes.begin(), it_end = Modes.end(); it != it_end; ++it)
						if (!uc->user->HasMode(*it))
							continue;

				notice_lang(Config.s_OperServ, u, OPER_USERLIST_RECORD, uc->user->nick.c_str(), uc->user->GetIdent().c_str(), uc->user->GetDisplayedHost().c_str());
			}
		}
		else
		{
			notice_lang(Config.s_OperServ, u, OPER_USERLIST_HEADER);

			for (user_map::const_iterator uit = UserListByNick.begin(), uit_end = UserListByNick.end(); uit != uit_end; ++uit)
			{
				User *u2 = uit->second;

				if (pattern)
				{
					char mask[BUFSIZE];
					snprintf(mask, sizeof(mask), "%s!%s@%s", u2->nick.c_str(), u2->GetIdent().c_str(), u2->GetDisplayedHost().c_str());
					if (!Anope::Match(mask, pattern, false))
						continue;
					if (!Modes.empty())
						for (std::list<UserModeName>::iterator it = Modes.begin(), it_end = Modes.end(); it != it_end; ++it)
							if (!u2->HasMode(*it))
								continue;
				}
				notice_lang(Config.s_OperServ, u, OPER_USERLIST_RECORD, u2->nick.c_str(), u2->GetIdent().c_str(), u2->GetDisplayedHost().c_str());
			}
		}

		notice_lang(Config.s_OperServ, u, OPER_USERLIST_END);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_USERLIST);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_USERLIST);
	}
};

class OSUserList : public Module
{
 public:
	OSUserList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSUserList());
	}
};

MODULE_INIT(OSUserList)
