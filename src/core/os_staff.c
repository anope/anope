/* OperServ core functions
 *
 * (C) 2003-2009 Anope Team
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

int opers_list_callback(SList *slist, int number, void *item, va_list args);
int opers_list(int number, NickCore *nc, User *u, char *level);

class CommandOSStaff : public Command
{
 public:
	CommandOSStaff() : Command("STAFF", 0, 0, "operserv/staff")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		User *au = NULL;
		NickCore *nc;
		NickAlias *na;
		int found;
		int i;
		std::list<std::pair<std::string, std::string> >::iterator it;

		notice_lang(Config.s_OperServ, u, OPER_STAFF_LIST_HEADER);

		for (it = Config.Opers.begin(); it != Config.Opers.end(); ++it)
		{
			found = 0;
			std::string nick = it->first, type = it->second;

			if ((au = finduser(nick)))
			{
				found = 1;
				notice_lang(Config.s_OperServ, u, OPER_STAFF_FORMAT, '*', type.c_str(), nick.c_str());
			}
			else if ((nc = findcore(nick.c_str())))
			{
				for (i = 0; i < nc->aliases.count; i++)
				{
					na = static_cast<NickAlias *>(nc->aliases.list[i]);
					if ((au = finduser(na->nick)))
					{
						found = 1;
						notice_lang(Config.s_OperServ, u, OPER_STAFF_AFORMAT, '*', type.c_str(), nick.c_str(), u->nick.c_str());
					}
				}
			}

			if (!found)
				notice_lang(Config.s_OperServ, u, OPER_STAFF_FORMAT, ' ', type.c_str(), nick.c_str());
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
