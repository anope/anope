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

class CommandOSIgnore : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<ci::string> &params)
	{
		const char *time = params.size() > 1 ? params[1].c_str() : NULL;
		const char *nick = params.size() > 2 ? params[2].c_str() : NULL;
		time_t t;

		if (!time || !nick)
		{
			this->OnSyntaxError(u, "ADD");
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

	CommandReturn DoList(User *u)
	{
		IgnoreData *id;

		if (!ignore)
		{
			notice_lang(s_OperServ, u, OPER_IGNORE_LIST_EMPTY);
			return MOD_CONT;
		}

		notice_lang(s_OperServ, u, OPER_IGNORE_LIST);
		for (id = ignore; id; id = id->next)
			u->SendMessage(s_OperServ, "%s", id->mask);

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, const std::vector<ci::string> &params)
	{
		const char *nick = params.size() > 1 ? params[1].c_str() : NULL;
		if (!nick)
			this->OnSyntaxError(u, "DEL");
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

	CommandReturn DoClear(User *u)
	{
		clear_ignores();
		notice_lang(s_OperServ, u, OPER_IGNORE_LIST_CLEARED);

		return MOD_CONT;
	}
 public:
	CommandOSIgnore() : Command("IGNORE", 1, 3, "operserv/ignore")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string cmd = params[0];

		if (cmd == "ADD")
			return this->DoAdd(u, params);
		else if (cmd == "LIST")
			return this->DoList(u);
		else if (cmd == "DEL")
			return this->DoDel(u, params);
		else if (cmd == "CLEAR")
			return this->DoClear(u);
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_IGNORE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
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
		this->AddCommand(OPERSERV, new CommandOSIgnore());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_IGNORE);
	}
};

MODULE_INIT(OSIgnore)
