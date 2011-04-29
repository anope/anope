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
#include "operserv.h"

class CommandOSUserList : public Command
{
 public:
	CommandOSUserList() : Command("USERLIST", 0, 2)
	{
		this->SetDesc(_("Lists all user records"));
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
			source.Reply(_("\002%s\002 users list:\n"
					"Nick                 Mask"), pattern.c_str());

			for (CUserList::iterator cuit = c->users.begin(), cuit_end = c->users.end(); cuit != cuit_end; ++cuit)
			{
				UserContainer *uc = *cuit;

				if (!Modes.empty())
					for (std::list<UserModeName>::iterator it = Modes.begin(), it_end = Modes.end(); it != it_end; ++it)
						if (!uc->user->HasMode(*it))
							continue;

				source.Reply(_("%-20s %s@%s"), uc->user->nick.c_str(), uc->user->GetIdent().c_str(), uc->user->GetDisplayedHost().c_str());
			}
		}
		else
		{
			source.Reply(_("Users list:\n"
					"Nick                 Mask"));

			for (Anope::insensitive_map<User *>::iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			{
				User *u2 = it->second;

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
				source.Reply(_("%-20s %s@%s"), u2->nick.c_str(), u2->GetIdent().c_str(), u2->GetDisplayedHost().c_str());
			}
		}

		source.Reply(_("End of users list."));
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002USERLIST [{\037pattern | channel\037} [\037INVISIBLE\037]]\002\n"
				" \n"
				"Lists all users currently online on the IRC network, whether their\n"
				"nick is registered or not.\n"
				" \n"
				"If \002pattern\002 is given, lists only users that match it (it must be in\n"
				"the format nick!user@host). If \002channel\002 is given, lists only users\n"
				"that are on the given channel. If INVISIBLE is specified, only users\n"
				"with the +i flag will be listed."));
		return true;
	}
};

class OSUserList : public Module
{
	CommandOSUserList commandosuserlist;

 public:
	OSUserList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->AddCommand(operserv->Bot(), &commandosuserlist);
	}
};

MODULE_INIT(OSUserList)
