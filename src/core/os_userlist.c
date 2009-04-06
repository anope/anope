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

class CommandOSUserList : public Command
{
 public:
	CommandOSUserList() : Command("USERLIST", 0, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *pattern = params.size() > 0 ? params[0].c_str() : NULL;
		const char *opt = params.size() > 1 ? params[1].c_str() : NULL;

		Channel *c;
		int modes = 0;

		if (opt && !stricmp(opt, "INVISIBLE"))
			modes |= anope_get_invis_mode();

		if (pattern && (c = findchan(pattern)))
		{
			struct c_userlist *cu;

			notice_lang(s_OperServ, u, OPER_USERLIST_HEADER_CHAN, pattern);

			for (cu = c->users; cu; cu = cu->next)
			{
				if (modes && !(cu->user->mode & modes))
					continue;
				notice_lang(s_OperServ, u, OPER_USERLIST_RECORD, cu->user->nick, cu->user->GetIdent().c_str(), cu->user->GetDisplayedHost().c_str());
			}
		}
		else
		{
			char mask[BUFSIZE];
			int i;
			User *u2;

			notice_lang(s_OperServ, u, OPER_USERLIST_HEADER);

			for (i = 0; i < 1024; ++i)
			{
				for (u2 = userlist[i]; u2; u2 = u2->next)
				{
					if (pattern)
					{
						snprintf(mask, sizeof(mask), "%s!%s@%s", u2->nick, u2->GetIdent().c_str(), u2->GetDisplayedHost().c_str());
						if (!Anope::Match(mask, pattern, false))
							continue;
						if (modes && !(u2->mode & modes))
							continue;
					}
					notice_lang(s_OperServ, u, OPER_USERLIST_RECORD, u2->nick, u2->GetIdent().c_str(), u2->GetDisplayedHost().c_str());
				}
			}
		}

		notice_lang(s_OperServ, u, OPER_USERLIST_END);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_USERLIST);
		return true;
	}
};

class OSUserList : public Module
{
 public:
	OSUserList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSUserList(), MOD_UNIQUE);
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_USERLIST);
	}
};

MODULE_INIT("os_userlist", OSUserList)
