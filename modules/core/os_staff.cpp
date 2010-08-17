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

class CommandOSStaff : public Command
{
 public:
	CommandOSStaff() : Command("STAFF", 0, 0, "operserv/staff")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		notice_lang(Config->s_OperServ, u, OPER_STAFF_LIST_HEADER);

		for (std::list<std::pair<Anope::string, Anope::string> >::iterator it = Config->Opers.begin(), it_end = Config->Opers.end(); it != it_end; ++it)
		{
			int found = 0;
			Anope::string nick = it->first, type = it->second;

			NickAlias *na = findnick(nick);
			if (na)
			{
				/* We have to loop all users as some may be logged into an account but not a nick */
				for (user_map::iterator uit = UserListByNick.begin(), uit_end = UserListByNick.end(); uit != uit_end; ++uit)
				{
					User *u2 = uit->second;

					if (u2->Account() && u2->Account() == na->nc)
					{
						found = 1;
						if (na->nick.equals_ci(u2->nick))
							notice_lang(Config->s_OperServ, u, OPER_STAFF_FORMAT, '*', type.c_str(), u2->nick.c_str());
						else
							notice_lang(Config->s_OperServ, u, OPER_STAFF_AFORMAT, '*', type.c_str(), na->nick.c_str(), u2->nick.c_str());
					}
				}
				if (!found)
					notice_lang(Config->s_OperServ, u, OPER_STAFF_FORMAT, ' ', type.c_str(), na->nick.c_str());
			}
		}

		notice_lang(Config->s_OperServ, u, END_OF_ANY_LIST, "Staff");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_OperServ, u, OPER_HELP_STAFF);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_OperServ, u, OPER_HELP_CMD_STAFF);
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
