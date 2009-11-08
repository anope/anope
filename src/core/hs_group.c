/* HostServ core functions
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

extern int do_hs_sync(NickCore *nc, char *vIdent, char *hostmask, char *creator, time_t time);

class CommandHSGroup : public Command
{
 public:
	CommandHSGroup() : Command("GROUP", 0, 0)
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		HostCore *tmp;
		char *vHost = NULL;
		char *vIdent = NULL;
		char *creator = NULL;
		HostCore *head = NULL;
		time_t time;
		bool found = false;

		head = hostCoreListHead();

		tmp = findHostCore(head, u->nick, &found);
		if (found)
		{
			if (!tmp)
				tmp = head; /* incase first in list */
			else if (tmp->next) /* we dont want the previous entry were not inserting! */
				tmp = tmp->next; /* jump to the next */

			vHost = sstrdup(tmp->vHost);
			if (tmp->vIdent)
				vIdent = sstrdup(tmp->vIdent);
			creator = sstrdup(tmp->creator);
			time = tmp->time;

			do_hs_sync(u->nc, vIdent, vHost, creator, time);
			if (tmp->vIdent)
				notice_lang(s_HostServ, u, HOST_IDENT_GROUP, u->nc->display, vIdent, vHost);
			else
				notice_lang(s_HostServ, u, HOST_GROUP, u->nc->display, vHost);
			delete [] vHost;
			if (vIdent)
				delete [] vIdent;
			delete [] creator;
		}
		else
			notice_lang(s_HostServ, u, HOST_NOT_ASSIGNED);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_HostServ, u, HOST_HELP_GROUP);
		return true;
	}
};

class HSGroup : public Module
{
 public:
	HSGroup(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(HOSTSERV, new CommandHSGroup());

		ModuleManager::Attach(I_OnHostServHelp, this);
	}
	void OnHostServHelp(User *u)
	{
		notice_lang(s_HostServ, u, HOST_HELP_CMD_GROUP);
	}
};

MODULE_INIT(HSGroup)
