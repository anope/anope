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

class CommandOSStaff : public Command
{
 public:
	CommandOSStaff() : Command("STAFF", 0, 0, "operserv/staff")
	{
		this->SetDesc("Display Services staff and online status");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		source.Reply(_("On Level Nick"));

		for (std::list<std::pair<Anope::string, Anope::string> >::iterator it = Config->Opers.begin(), it_end = Config->Opers.end(); it != it_end; ++it)
		{
			Anope::string nick = it->first, type = it->second;

			NickAlias *na = findnick(nick);
			if (na)
			{
				NickCore *nc = na->nc;
				for (std::list<User *>::iterator uit = nc->Users.begin(); uit != nc->Users.end(); ++uit)
				{
					User *u2 = *uit;

					if (na->nick.equals_ci(u2->nick))
						source.Reply(_(" %c %s  %s"), '*', type.c_str(), u2->nick.c_str());
					else
						source.Reply(_(" %c %s  %s [%s]"), '*', type.c_str(), na->nick.c_str(), u2->nick.c_str());
				}
				if (nc->Users.empty())
					source.Reply(_(" %c %s  %s"), ' ', type.c_str(), na->nick.c_str());
			}
		}

		source.Reply(_(END_OF_ANY_LIST), "Staff");
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002STAFF\002\n"
				"Displays all Services Staff nicks along with level\n"
				"and on-line status."));
		return true;
	}
};

class OSStaff : public Module
{
	CommandOSStaff commandosstaff;

 public:
	OSStaff(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosstaff);
	}
};

MODULE_INIT(OSStaff)
