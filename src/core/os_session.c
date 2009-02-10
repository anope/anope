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

class CommandOSSession : public Command
{
 private:
	CommandReturn DoList(User *u, std::vector<std::string> &params)
	{
		Session *session;
		int mincount;
		const char *param = params[1].c_str();

		if ((mincount = atoi(param)) <= 1)
			notice_lang(s_OperServ, u, OPER_SESSION_INVALID_THRESHOLD);
		else
		{
			notice_lang(s_OperServ, u, OPER_SESSION_LIST_HEADER, mincount);
			notice_lang(s_OperServ, u, OPER_SESSION_LIST_COLHEAD);
			for (i = 0; i < 1024; ++i)
			{
				for (session = sessionlist[i]; session; session = session->next)
				{
					if (session->count >= mincount)
						notice_lang(s_OperServ, u, OPER_SESSION_LIST_FORMAT, session->count, session->host);
				}
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, std::vector<std::string> &params)
	{
		const char *param = params[1].c_str();
		Session *session = findsession(param);

		if (!session)
			notice_lang(s_OperServ, u, OPER_SESSION_NOT_FOUND, param);
		else {
			Exception *exception = find_host_exception(param);
			notice_lang(s_OperServ, u, OPER_SESSION_VIEW_FORMAT, param, session->count, exception ? exception-> limit : DefSessionLimit);
		}

		return MOD_CONT;
	}
 public:
	CommandOSSession() : Command("SESSION", 2, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *cmd = params[0].c_str();

		if (!LimitSessions)
		{
			notice_lang(s_OperServ, u, OPER_SESSION_DISABLED);
			return MOD_CONT;
		}

		if (!stricmp(cmd, "LIST"))
			return this->DoList(u);
		else if (!stricmp(cmd, "VIEW"))
			return this->DoView(u);
		else
			this->OnSyntaxError(u);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_oper(u))
			return false;

		notice_lang(s_OperServ, u, OPER_HELP_SESSION);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "SESSION", OPER_SESSION_LIST_SYNTAX);
	}
};

class CommandOSException : public Command
{
 private:
	CommandReturn DoAdd(User *u, std::vector<std::string> &params)
	{
		const char *mask, *expiry, *limitstr;
		char reason[BUFSIZE];
		int last_param = 3, x;

		if (nexceptions >= 32767)
		{
			notice_lang(s_OperServ, u, OPER_EXCEPTION_TOO_MANY);
			return MOD_CONT;
		}

		mask = params.size() > 1 ? params[1].c_str() : NULL;
		if (mask && *mask == '+')
		{
			expiry = mask;
			mask = params.size() > 2 ? params[2].c_str() : NULL;
			last_param = 4;
		}
		else
			expiry = NULL;

		limitstr = params.size() > last_param - 1 ? params[last_param - 1].c_str() : NULL;

		if (params.size() < last_param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}
		snprintf(reason, sizeof(reason), "%s%s%s", params[last_param].c_str(), last_param == 3 ? " " : "", last_param == 3 ? param[4].c_str() : "");

		if (!*reason)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		expires = expiry ? dotime(expiry) : ExceptionExpiry;
		if (expires < 0)
		{
			notice_lang(s_OperServ, u, BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += time(NULL);

		limit = limitstr && isdigit(*limitstr) ? atoi(limitstr) : -1;

		if (limit < 0 || limit > MaxSessionLimit)
		{
			notice_lang(s_OperServ, u, OPER_EXCEPTION_INVALID_LIMIT, MaxSessionLimit);
			return MOD_CONT;
		}
		else
		{
			if (strchr(mask, '!') || strchr(mask, '@'))
			{
				notice_lang(s_OperServ, u, OPER_EXCEPTION_INVALID_HOSTMASK);
				return MOD_CONT;
			}

			x = exception_add(u, mask, limit, reason, u->nick, expires);

			if (x == 1)
				notice_lang(s_OperServ, u, OPER_EXCEPTION_ADDED, mask, limit);

			if (readonly)
				notice_lang(s_OperServ, u, READ_ONLY_MODE);
		}

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, std::vector<std::string> &params)
	{
		const char *mask = params.size() > 1 ? params[1].c_str() : NULL;
		int i;

		if (!mask)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask))
		{
			int count, deleted, last = -1;
			deleted = process_numlist(mask, &count, exception_del_callback, u, &last);
			if (!deleted)
			{
				if (count == 1)
					notice_lang(s_OperServ, u, OPER_EXCEPTION_NO_SUCH_ENTRY, last);
				else
					notice_lang(s_OperServ, u, OPER_EXCEPTION_NO_MATCH);
			}
			else if (deleted == 1)
				notice_lang(s_OperServ, u, OPER_EXCEPTION_DELETED_ONE);
			else
				notice_lang(s_OperServ, u, OPER_EXCEPTION_DELETED_SEVERAL, deleted);
		}
		else
		{
			int deleted = 0;

			for (i = 0; i < nexceptions; ++i)
			{
				if (!stricmp(mask, exceptions[i].mask))
				{
					exception_del(i);
					notice_lang(s_OperServ, u, OPER_EXCEPTION_DELETED, mask);
					deleted = 1;
					break;
				}
			}
			if (!deleted && i == nexceptions)
				notice_lang(s_OperServ, u, OPER_EXCEPTION_NOT_FOUND, mask);
		}

		/* Renumber the exception list. I don't believe in having holes in
		 * lists - it makes code more complex, harder to debug and we end up
		 * with huge index numbers. Imho, fixed numbering is only beneficial
		 * when one doesn't have range capable manipulation. -TheShadow */

		for (i = 0; i < nexceptions; ++i)
			exceptions[i].num = i;

		if (readonly)
			notice_lang(s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoMove(User *u, std::vector<std::string> &params)
	{
		Exception *exception;
		const char *n1str = params.size() > 1 ? params[1].c_str() : NULL; /* From position */
		const char *n2str = params.size() > 2 ? params[2].c_str() : NULL; /* To position */
		int n1, n2;

		if (!n2str)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		n1 = atoi(n1str) - 1;
		n2 = atoi(n2str) - 1;

		if (n1 >= 0 && n1 < nexceptions && n2 >= 0 && n2 < nexceptions && n1 != n2)
		{
			exception = static_cast<Exception *>(smalloc(sizeof(Exception)));
			memcpy(exception, &exceptions[n1], sizeof(Exception));

			if (n1 < n2)
			{
				/* Shift upwards */
				memmove(&exceptions[n1], &exceptions[n1 + 1], sizeof(Exception) * (n2 - n1));
				memmove(&exceptions[n2], exception, sizeof(Exception));
			}
			else
			{
				/* Shift downwards */
				memmove(&exceptions[n2 + 1], &exceptions[n2], sizeof(Exception) * (n1 - n2));
				memmove(&exceptions[n2], exception, sizeof(Exception));
			}

			free(exception);

			notice_lang(s_OperServ, u, OPER_EXCEPTION_MOVED, exceptions[n1].mask, n1 + 1, n2 + 1);

			/* Renumber the exception list. See DoDel() above for why. */
			for (i = 0; i < nexceptions; ++i)
				exceptions[i].num = i;

			if (readonly)
				notice_lang(s_OperServ, u, READ_ONLY_MODE);
		}
		else
			this->OnSyntaxError(u);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, std::vector<std::string> &params)
	{
		int sent_header = 0;
		expire_exceptions();
		const char *mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (mask && strspn(mask, "1234567890,-") == strlen(mask))
			process_numlist(mask, NULL, exception_list_callback, u, &sent_header);
		else
		{
			for (i = 0; i < nexceptions; ++i)
			{
				if (!mask || match_wild_nocase(mask, exceptions[i].mask))
					exception_list(u, i, &sent_header);
			}
		}
		if (!sent_header)
			notice_lang(s_OperServ, u, OPER_EXCEPTION_NO_MATCH);

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, std::vector<std::string> &params)
	{
		int sent_header = 0;
		expire_exceptions();
		const char *mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (mask && strspn(mask, "1234567890,-") == strlen(mask))
			process_numlist(mask, NULL, exception_view_callback, u,
							&sent_header);
		else
		{
			for (i = 0; i < nexceptions; ++i)
			{
				if (!mask || match_wild_nocase(mask, exceptions[i].mask))
					exception_view(u, i, &sent_header);
			}
		}
		if (!sent_header)
			notice_lang(s_OperServ, u, OPER_EXCEPTION_NO_MATCH);

		return MOD_CONT;
	}
 public:
	CommandOSException() : Command("EXCEPTION", 1, 5)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *cmd = params[0].c_str();
		char *mask, *reason, *expiry, *limitstr;
		int limit, expires;
		int i;
		int x;

		if (!LimitSessions)
		{
			notice_lang(s_OperServ, u, OPER_EXCEPTION_DISABLED);
			return MOD_CONT;
		}

		if (!stricmp(cmd, "ADD"))
			return this->DoAdd(u, params);
		else if (!stricmp(cmd, "DEL"))
			return this->DoDel(u, params);
		else if (!stricmp(cmd, "MOVE"))
			return this->DoMove(u, params);
		else if (!stricmp(cmd, "LIST"))
			return this->DoList(u, params);
		else if (!stricmp(cmd, "VIEW"))
			return this->DoView(u, params);
		else {
			this->OnSyntaxError(u);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (!is_services_oper(u))
			return false;

		notice_lang(s_OperServ, u, OPER_HELP_EXCEPTION);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "EXCEPTION", OPER_EXCEPTION_SYNTAX);
	}
};

class OSSession : public Module
{
 public:
	OSSession(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSSession(), MOD_UNIQUE);
		this->AddCommand(OPERSERV, new CommandOSException(), MOD_UNIQUE);

		this->SetOperHelp(myOperServHelp);
	}
};

/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User *u)
{
	if (is_services_oper(u))
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_SESSION);
		notice_lang(s_OperServ, u, OPER_HELP_CMD_EXCEPTION);
	}
}

MODULE_INIT("os_session", OSSession)
