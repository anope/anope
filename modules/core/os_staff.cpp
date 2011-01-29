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
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		source.Reply(OPER_STAFF_LIST_HEADER);

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
						source.Reply(OPER_STAFF_FORMAT, '*', type.c_str(), u2->nick.c_str());
					else
						source.Reply(OPER_STAFF_AFORMAT, '*', type.c_str(), na->nick.c_str(), u2->nick.c_str());
				}
				if (nc->Users.empty())
					source.Reply(OPER_STAFF_FORMAT, ' ', type.c_str(), na->nick.c_str());
			}
		}

		source.Reply(END_OF_ANY_LIST, "Staff");
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(OPER_HELP_STAFF);
		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(OPER_HELP_CMD_STAFF);
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
