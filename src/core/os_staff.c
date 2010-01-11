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

class CommandOSStaff : public Command
{
 public:
	CommandOSStaff() : Command("STAFF", 0, 0, "operserv/staff")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		notice_lang(Config.s_OperServ, u, OPER_STAFF_LIST_HEADER);

		for (std::list<std::pair<std::string, std::string> >::iterator it = Config.Opers.begin(); it != Config.Opers.end(); ++it)
		{
			int found = 0;
			std::string nick = it->first, type = it->second;

			NickAlias *na = findnick(nick);
			if (na)
			{
				/* We have to loop all users as some may be logged into an account but not a nick */
				for (User *u2 = firstuser(); u2; u2 = nextuser())
				{
					if (u2->nc && u2->nc == na->nc)
					{
						found = 1;
						if (na->nick == u2->nick)
							notice_lang(Config.s_OperServ, u, OPER_STAFF_FORMAT, '*', type.c_str(), u2->nick.c_str());
						else
							notice_lang(Config.s_OperServ, u, OPER_STAFF_AFORMAT, '*', type.c_str(), na->nick, u2->nick.c_str());
					}
				}
				if (!found)
				{
					notice_lang(Config.s_OperServ, u, OPER_STAFF_FORMAT, ' ', type.c_str(), na->nick);
				}
			}
		}

		notice_lang(Config.s_OperServ, u, END_OF_ANY_LIST, "Staff");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_STAFF);
		return true;
	}
};

class OSStaff : public Module
{
 public:
	OSStaff(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSStaff());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_STAFF);
	}
};

MODULE_INIT(OSStaff)
