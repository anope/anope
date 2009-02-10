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

void myOperServHelp(User *u);

class CommandOSIgnore : public Command
{
 private:
	CommandReturn DoAdd(User *u, std::vector<std::string> &params)
	{
		char *time = params.size() > 1 ? params[1].c_str() : NULL;
		char *nick = params.size() > 2 ? params[2].c_str() : NULL;
		int t;

		if (!time || !nick)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}
		else
		{
			t = dotime(time);

			if (t <= -1)
			{
				notice_lang(s_OperServ, u, OPER_IGNORE_VALID_TIME);
				return MOD_CONT;
			}
			else if (!t)
			{
				add_ignore(nick, t);
				notice_lang(s_OperServ, u, OPER_IGNORE_PERM_DONE, nick);
			}
			else
			{
				add_ignore(nick, t);
				notice_lang(s_OperServ, u, OPER_IGNORE_TIME_DONE, nick, time);
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, std::vector<std::string> &params)
	{
		IgnoreData *id;

		if (!ignore)
		{
			notice_lang(s_OperServ, u, OPER_IGNORE_LIST_EMPTY);
			return MOD_CONT;
		}

		notice_lang(s_OperServ, u, OPER_IGNORE_LIST);
		for (id = ignore; id; id = id->next)
			notice_user(s_OperServ, u, "%s", id->mask);

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, std::vector<std::string> &params)
	{
		char *nick = params.size() > 1 ? params[1].c_str() : NULL;
		if (!nick)
			this->OnSyntaxError(u);
		else
		{
			if (delete_ignore(nick))
			{
				notice_lang(s_OperServ, u, OPER_IGNORE_DEL_DONE, nick);
				return MOD_CONT;
			}
			notice_lang(s_OperServ, u, OPER_IGNORE_LIST_NOMATCH, nick);
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u, std::vector<std::string> &params)
	{
		clear_ignores();
		notice_lang(s_OperServ, u, OPER_IGNORE_LIST_CLEARED);

		return MOD_CONT;
	}
 public:
	CommandOSIgnore() : Command("IGNORE", 1, 4)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *cmd = params[0].c_str();
		int t;

		if (!stricmp(cmd, "ADD"))
			return this->DoAdd(u, params);
		else if (!stricmp(cmd, "LIST"))
			return this->DoList(u, params);
		else if (!stricmp(cmd, "DEL"))
			return this->DoDel(u, params);
		else if (!stricmp(cmd, "CLEAR"))
			return this->DoCelar(u, params);
		else
			this->OnSyntaxError(u);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_admin(u))
			return false;

		notice_lang(s_OperServ, u, OPER_HELP_IGNORE);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "IGNORE", OPER_IGNORE_SYNTAX);
	}
};

class OSIgnore : public Module
{
 public:
	OSIgnore(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(OPERSERV, new CommandOSIgnore(), MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};

/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	if (is_services_admin(u))
		notice_lang(s_OperServ, u, OPER_HELP_CMD_IGNORE);
}

MODULE_INIT("os_ignore", OSIgnore)
