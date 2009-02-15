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

int oper_list_callback(SList *slist, int number, void *item, va_list args);
int oper_list(int number, NickCore *nc, User *u, int *sent_header);
void myOperServHelp(User *u);

class CommandOSOper : public Command
{
 private:
	CommandReturn DoAdd(User *u, std::vector<std::string> &params)
	{
		const char *nick = params.size() > 1 ? params[1].c_str() : NULL;
		NickAlias *na;
		int res = 0;

		if (!nick)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!is_services_root(u))
		{
			notice_lang(s_OperServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}

		if (!(na = findnick(nick)))
		{
			notice_lang(s_OperServ, u, NICK_X_NOT_REGISTERED, nick);
			return MOD_CONT;
		}

		if (na->status & NS_FORBIDDEN)
		{
			notice_lang(s_OperServ, u, NICK_X_FORBIDDEN, nick);
			return MOD_CONT;
		}

		if (na->nc->flags & NI_SERVICES_OPER || slist_indexof(&servopers, na->nc) != -1)
		{
			notice_lang(s_OperServ, u, OPER_OPER_EXISTS, nick);
			return MOD_CONT;
		}

		res = slist_add(&servopers, na->nc);
		if (res == -2)
		{
			notice_lang(s_OperServ, u, OPER_OPER_REACHED_LIMIT, nick);
			return MOD_CONT;
		}
		else
		{
			if (na->nc->flags & NI_SERVICES_ADMIN && (res = slist_indexof(&servadmins, na->nc)) != -1)
			{
				if (!is_services_root(u))
				{
					notice_lang(s_OperServ, u, PERMISSION_DENIED);
					return MOD_CONT;
				}
				slist_delete(&servadmins, res);
				na->nc->flags |= NI_SERVICES_OPER;
				notice_lang(s_OperServ, u, OPER_OPER_MOVED, nick);
			}
			else
			{
				na->nc->flags |= NI_SERVICES_OPER;
				notice_lang(s_OperServ, u, OPER_OPER_ADDED, nick);
			}
		}

		if (readonly)
			notice_lang(s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, std::vector<std::string> &params)
	{
		const char *nick = params.size() > 1 ? params[1].c_str() : NULL;
		NickAlias *na;
		int res = 0;

		if (!nick)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!is_services_root(u))
		{
			notice_lang(s_OperServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}

		if (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick))
		{
			/* Deleting a range */
			res = slist_delete_range(&servopers, nick, NULL);
			if (res == 0)
			{
				notice_lang(s_OperServ, u, OPER_OPER_NO_MATCH);
				return MOD_CONT;
			}
			else if (res == 1)
				notice_lang(s_OperServ, u, OPER_OPER_DELETED_ONE);
			else
				notice_lang(s_OperServ, u, OPER_OPER_DELETED_SEVERAL, res);
		}
		else
		{
			if (!(na = findnick(nick)))
			{
				notice_lang(s_OperServ, u, NICK_X_NOT_REGISTERED, nick);
				return MOD_CONT;
			}

			if (na->status & NS_FORBIDDEN)
			{
				notice_lang(s_OperServ, u, NICK_X_FORBIDDEN, nick);
				return MOD_CONT;
			}

			if (!(na->nc->flags & NI_SERVICES_OPER) || (res = slist_indexof(&servopers, na->nc)) == -1)
			{
				notice_lang(s_OperServ, u, OPER_OPER_NOT_FOUND, nick);
				return MOD_CONT;
			}

			slist_delete(&servopers, res);
			notice_lang(s_OperServ, u, OPER_OPER_DELETED, nick);
		}

		if (readonly)
			notice_lang(s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, std::vector<std::string> &params)
	{
		int sent_header = 0;
		const char *nick = params.size() > 1 ? params[1].c_str() : NULL;
		int res = 0;

		if (!is_oper(u))
		{
			notice_lang(s_OperServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}

		if (!servopers.count)
		{
			notice_lang(s_OperServ, u, OPER_OPER_LIST_EMPTY);
			return MOD_CONT;
		}

		if (!nick || (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick)))
		{
			int res = slist_enum(&servopers, nick, &oper_list_callback, u, &sent_header);
			if (!res)
			{
				notice_lang(s_OperServ, u, OPER_OPER_NO_MATCH);
				return MOD_CONT;
			}
			else
				notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Oper");
		}
		else
		{
			int i;

			for (i = 0; i < servopers.count; ++i)
			{
				if (!stricmp(nick, (static_cast<NickCore *>(servopers.list[i]))->display) || match_wild_nocase(nick, (static_cast<NickCore *>(servopers.list[i]))->display))
					oper_list(i + 1, static_cast<NickCore *>(servopers.list[i]), u, &sent_header);
			}

			if (!sent_header)
				notice_lang(s_OperServ, u, OPER_OPER_NO_MATCH);
			else
				notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Oper");
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u, std::vector<std::string> &params)
	{
		if (!is_services_root(u))
		{
			notice_lang(s_OperServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}

		if (!servopers.count)
		{
			notice_lang(s_OperServ, u, OPER_OPER_LIST_EMPTY);
			return MOD_CONT;
		}

		slist_clear(&servopers, 1);
		notice_lang(s_OperServ, u, OPER_OPER_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSOper() : Command("OPER", 1, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *cmd = params[0].c_str();

		if (!stricmp(cmd, "ADD"))
			return this->DoAdd(u, params);
		else if (!stricmp(cmd, "DEL"))
			return this->DoDel(u, params);
		else if (!stricmp(cmd, "LIST"))
			return this->DoList(u, params);
		else if (!stricmp(cmd, "CLEAR"))
			return this->DoClear(u, params);
		else
			this->OnSyntaxError(u);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_OPER, s_NickServ);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "OPER", OPER_OPER_SYNTAX);
	}
};

class OSOper : public Module
{
 public:
	OSOper(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(OPERSERV, new CommandOSOper(), MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	notice_lang(s_OperServ, u, OPER_HELP_CMD_OPER);
}

/* Lists an oper entry, prefixing it with the header if needed */
int oper_list(int number, NickCore *nc, User *u, int *sent_header)
{
	if (!nc)
		return 0;

	if (!*sent_header)
	{
		notice_lang(s_OperServ, u, OPER_OPER_LIST_HEADER);
		*sent_header = 1;
	}

	notice_lang(s_OperServ, u, OPER_OPER_LIST_FORMAT, number, nc->display);
	return 1;
}

/* Callback for enumeration purposes */
int oper_list_callback(SList *slist, int number, void *item, va_list args)
{
	User *u = va_arg(args, User *);
	int *sent_header = va_arg(args, int *);

	return oper_list(number, static_cast<NickCore *>(item), u, sent_header);
}

MODULE_INIT("os_oper", OSOper)
