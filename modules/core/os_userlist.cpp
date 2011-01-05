/* OperServ core functions
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

class CommandOSUserList : public Command
{
 public:
	CommandOSUserList() : Command("USERLIST", 0, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &pattern = !params.empty() ? params[0] : "";
		const Anope::string &opt = params.size() > 1 ? params[1] : "";
		Channel *c;
		std::list<UserModeName> Modes;

		if (!opt.empty() && opt.equals_ci("INVISIBLE"))
			Modes.push_back(UMODE_INVIS);

		if (!pattern.empty() && (c = findchan(pattern)))
		{
			source.Reply(OPER_USERLIST_HEADER_CHAN, pattern.c_str());

			for (CUserList::iterator cuit = c->users.begin(), cuit_end = c->users.end(); cuit != cuit_end; ++cuit)
			{
				UserContainer *uc = *cuit;

				if (!Modes.empty())
					for (std::list<UserModeName>::iterator it = Modes.begin(), it_end = Modes.end(); it != it_end; ++it)
						if (!uc->user->HasMode(*it))
							continue;

				source.Reply(OPER_USERLIST_RECORD, uc->user->nick.c_str(), uc->user->GetIdent().c_str(), uc->user->GetDisplayedHost().c_str());
			}
		}
		else
		{
			source.Reply(OPER_USERLIST_HEADER);

			for (patricia_tree<User *>::const_iterator it = UserListByNick.begin(), it_end = UserListByNick.end(); it != it_end; ++it)
			{
				User *u2 = *it;

				if (!pattern.empty())
				{
					Anope::string mask = u2->nick + "!" + u2->GetIdent() + "@" + u2->GetDisplayedHost();
					if (!Anope::Match(mask, pattern))
						continue;
					if (!Modes.empty())
						for (std::list<UserModeName>::iterator mit = Modes.begin(), mit_end = Modes.end(); mit != mit_end; ++mit)
							if (!u2->HasMode(*mit))
								continue;
				}
				source.Reply(OPER_USERLIST_RECORD, u2->nick.c_str(), u2->GetIdent().c_str(), u2->GetDisplayedHost().c_str());
			}
		}

		source.Reply(OPER_USERLIST_END);
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(OPER_HELP_USERLIST);
		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(OPER_HELP_CMD_USERLIST);
	}
};

class OSUserList : public Module
{
	CommandOSUserList commandosuserlist;

 public:
	OSUserList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosuserlist);
	}
};

MODULE_INIT(OSUserList)
