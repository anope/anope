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

int admin_list_callback(SList *slist, int number, void *item, va_list args);
int admin_list(int number, NickCore *nc, User *u, int *sent_header);

class CommandOSAdmin : public Command
{
 private:
	CommandReturn DoAdd(User *u, std::vector<std::string> &params)
	{
		NickAlias *na;
		const char *nick = params.size() > 1 ? params[1].c_str() : NULL;
		int res = 0;

		if (!nick)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!u->nc->HasCommand("operserv/admin"))
		{
			notice_lang(s_OperServ, u, ACCESS_DENIED);
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

		if (na->nc->flags & NI_SERVICES_ADMIN || slist_indexof(&servadmins, na->nc) != -1)
		{
			notice_lang(s_OperServ, u, OPER_ADMIN_EXISTS, nick);
			return MOD_CONT;
		}

		res = slist_add(&servadmins, na->nc);
		if (res == -2)
		{
			notice_lang(s_OperServ, u, OPER_ADMIN_REACHED_LIMIT, nick);
			return MOD_CONT;
		}
		else
		{
			if (na->nc->flags & NI_SERVICES_OPER && (res = slist_indexof(&servopers, na->nc)) != -1)
			{
				slist_delete(&servopers, res);
				na->nc->flags |= NI_SERVICES_ADMIN;
				notice_lang(s_OperServ, u, OPER_ADMIN_MOVED, nick);
			}
			else
			{
				na->nc->flags |= NI_SERVICES_ADMIN;
				notice_lang(s_OperServ, u, OPER_ADMIN_ADDED, nick);
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

		if (!servadmins.count)
		{
			notice_lang(s_OperServ, u, OPER_ADMIN_LIST_EMPTY);
			return MOD_CONT;
		}

		if (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick))
		{
			/* Deleting a range */
			res = slist_delete_range(&servadmins, nick, NULL);
			if (!res)
			{
				notice_lang(s_OperServ, u, OPER_ADMIN_NO_MATCH);
				return MOD_CONT;
			}
			else if (res == 1)
				notice_lang(s_OperServ, u, OPER_ADMIN_DELETED_ONE);
			else
				notice_lang(s_OperServ, u, OPER_ADMIN_DELETED_SEVERAL, res);
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

			if (!(na->nc->flags & NI_SERVICES_ADMIN) || (res = slist_indexof(&servadmins, na->nc)) == -1)
			{
				notice_lang(s_OperServ, u, OPER_ADMIN_NOT_FOUND, nick);
				return MOD_CONT;
			}

			slist_delete(&servadmins, res);
			notice_lang(s_OperServ, u, OPER_ADMIN_DELETED, nick);
		}

		if (readonly)
			notice_lang(s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, std::vector<std::string> &params)
	{
		const char *nick = params.size() > 1 ? params[1].c_str() : NULL;
		int sent_header = 0, res = 0;

		if (!servadmins.count)
		{
			notice_lang(s_OperServ, u, OPER_ADMIN_LIST_EMPTY);
			return MOD_CONT;
		}

		if (!nick || (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick)))
		{
			res = slist_enum(&servadmins, nick, &admin_list_callback, u, &sent_header);
			if (!res)
			{
				notice_lang(s_OperServ, u, OPER_ADMIN_NO_MATCH);
				return MOD_CONT;
			}
			else
				notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Admin");
		}
		else
		{
			int i;

			for (i = 0; i < servadmins.count; ++i) {
				if (!stricmp(nick, (static_cast<NickCore *>(servadmins.list[i]))->display) || Anope::Match((static_cast<NickCore *>(servadmins.list[i]))->display, nick, false))
					admin_list(i + 1, static_cast<NickCore *>(servadmins.list[i]), u, &sent_header);
			}

			if (!sent_header)
				notice_lang(s_OperServ, u, OPER_ADMIN_NO_MATCH);
			else
				notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Admin");
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u, std::vector<std::string> &params)
	{
		if (!servadmins.count)
		{
			notice_lang(s_OperServ, u, OPER_ADMIN_LIST_EMPTY);
			return MOD_CONT;
		}

		slist_clear(&servadmins, 1);
		notice_lang(s_OperServ, u, OPER_ADMIN_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSAdmin() : Command("ADMIN", 1, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *cmd = params[0].c_str();

		if (!u->nc->HasCommand("operserv/admin"))
		{
			notice_lang(s_OperServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

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
		notice_help(s_OperServ, u, OPER_HELP_ADMIN, s_NickServ);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "ADMIN", OPER_ADMIN_SYNTAX);
	}
};

class OSAdmin : public Module
{
 public:
	OSAdmin(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSAdmin(), MOD_UNIQUE);
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_ADMIN);
	}
};

int admin_list_callback(SList *slist, int number, void *item, va_list args)
{
	User *u = va_arg(args, User *);
	int *sent_header = va_arg(args, int *);

	return admin_list(number, static_cast<NickCore *>(item), u, sent_header);
}

int admin_list(int number, NickCore *nc, User *u, int *sent_header)
{
	if (!nc)
		return 0;

	if (!*sent_header)
	{
		notice_lang(s_OperServ, u, OPER_ADMIN_LIST_HEADER);
		*sent_header = 1;
	}

	notice_lang(s_OperServ, u, OPER_ADMIN_LIST_FORMAT, number, nc->display);
	return 1;
}

MODULE_INIT("os_admin", OSAdmin)
