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

class CommandOSSession : public Command
{
 private:
	CommandReturn DoList(User *u, const std::vector<ci::string> &params)
	{
		Session *session;
		int mincount, i;
		const char *param = params[1].c_str();

		if ((mincount = atoi(param)) <= 1)
			notice_lang(Config.s_OperServ, u, OPER_SESSION_INVALID_THRESHOLD);
		else
		{
			notice_lang(Config.s_OperServ, u, OPER_SESSION_LIST_HEADER, mincount);
			notice_lang(Config.s_OperServ, u, OPER_SESSION_LIST_COLHEAD);
			for (i = 0; i < 1024; ++i)
			{
				for (session = sessionlist[i]; session; session = session->next)
				{
					if (session->count >= mincount)
						notice_lang(Config.s_OperServ, u, OPER_SESSION_LIST_FORMAT, session->count, session->host);
				}
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, const std::vector<ci::string> &params)
	{
		const char *param = params[1].c_str();
		Session *session = findsession(param);

		if (!session)
			notice_lang(Config.s_OperServ, u, OPER_SESSION_NOT_FOUND, param);
		else {
			Exception *exception = find_host_exception(param);
			notice_lang(Config.s_OperServ, u, OPER_SESSION_VIEW_FORMAT, param, session->count, exception ? exception-> limit : Config.DefSessionLimit);
		}

		return MOD_CONT;
	}
 public:
	CommandOSSession() : Command("SESSION", 2, 2, "operserv/session")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string cmd = params[0];

		if (!Config.LimitSessions)
		{
			notice_lang(Config.s_OperServ, u, OPER_SESSION_DISABLED);
			return MOD_CONT;
		}

		if (cmd == "LIST")
			return this->DoList(u, params);
		else if (cmd == "VIEW")
			return this->DoView(u, params);
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_SESSION);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "SESSION", OPER_SESSION_LIST_SYNTAX);
	}
};

static int exception_del(const int index)
{
	if (index < 0 || index >= nexceptions)
		return 0;

	delete [] exceptions[index].mask;
	delete [] exceptions[index].reason;
	--nexceptions;
	memmove(exceptions + index, exceptions + index + 1, sizeof(Exception) * (nexceptions - index));
	exceptions = static_cast<Exception *>(srealloc(exceptions, sizeof(Exception) * nexceptions));

	return 1;
}

/* We use the "num" property to keep track of the position of each exception
 * when deleting using ranges. This is because an exception's position changes
 * as others are deleted. The positions will be recalculated once the process
 * is complete. -TheShadow
 */

static int exception_del_callback(User *u, int num, va_list args)
{
	int i;
	int *last = va_arg(args, int *);

	*last = num;
	for (i = 0; i < nexceptions; ++i)
		if (num - 1 == exceptions[i].num)
			break;

	if (i < nexceptions)
		return exception_del(i);
	else
		return 0;
}

static int exception_list(User *u, const int index, int *sent_header)
{
	if (index < 0 || index >= nexceptions)
		return 0;
	if (!*sent_header) {
		notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_LIST_HEADER);
		notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_LIST_COLHEAD);
		*sent_header = 1;
	}
	notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_LIST_FORMAT, index + 1, exceptions[index].limit, exceptions[index].mask);
	return 1;
}

static int exception_list_callback(User *u, int num, va_list args)
{
	int *sent_header = va_arg(args, int *);

	return exception_list(u, num - 1, sent_header);
}

static int exception_view(User *u, const int index, int *sent_header)
{
	char timebuf[32], expirebuf[256];
	struct tm tm;
	time_t t = time(NULL);

	if (index < 0 || index >= nexceptions)
		return 0;
	if (!*sent_header) {
		notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_LIST_HEADER);
		*sent_header = 1;
	}

	tm = *localtime(exceptions[index].time ? &exceptions[index].time : &t);
	strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT, &tm);

	expire_left(u->nc, expirebuf, sizeof(expirebuf), exceptions[index].expires);

	notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_VIEW_FORMAT, index + 1, exceptions[index].mask, !exceptions[index].who.empty() ? exceptions[index].who.c_str() : "<unknown>", timebuf, expirebuf, exceptions[index].limit, exceptions[index].reason);
	return 1;
}

static int exception_view_callback(User *u, int num, va_list args)
{
	int *sent_header = va_arg(args, int *);

	return exception_view(u, num - 1, sent_header);
}

class CommandOSException : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<ci::string> &params)
	{
		const char *mask, *expiry, *limitstr;
		char reason[BUFSIZE];
		unsigned last_param = 3;
		int x;

		if (nexceptions >= 32767)
		{
			notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_TOO_MANY);
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

		if (params.size() <= last_param)
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}
		snprintf(reason, sizeof(reason), "%s%s%s", params[last_param].c_str(), last_param == 3 && params.size() > 4 ? " " : "", last_param == 3 && params.size() > 4 ? params[4].c_str() : "");

		if (!*reason)
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}

		time_t expires = expiry ? dotime(expiry) : Config.ExceptionExpiry;
		if (expires < 0)
		{
			notice_lang(Config.s_OperServ, u, BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += time(NULL);

		int limit = limitstr && isdigit(*limitstr) ? atoi(limitstr) : -1;

		if (limit < 0 || limit > static_cast<int>(Config.MaxSessionLimit))
		{
			notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_INVALID_LIMIT, Config.MaxSessionLimit);
			return MOD_CONT;
		}
		else
		{
			if (strchr(mask, '!') || strchr(mask, '@'))
			{
				notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_INVALID_HOSTMASK);
				return MOD_CONT;
			}

			x = exception_add(u, mask, limit, reason, u->nick.c_str(), expires);

			if (x == 1)
				notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_ADDED, mask, limit);

			if (readonly)
				notice_lang(Config.s_OperServ, u, READ_ONLY_MODE);
		}

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, const std::vector<ci::string> &params)
	{
		const char *mask = params.size() > 1 ? params[1].c_str() : NULL;
		int i;

		if (!mask)
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask))
		{
			int count, deleted, last = -1;
			deleted = process_numlist(mask, &count, exception_del_callback, u, &last);
			if (!deleted)
			{
				if (count == 1)
					notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_NO_SUCH_ENTRY, last);
				else
					notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_NO_MATCH);
			}
			else if (deleted == 1)
				notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_DELETED_ONE);
			else
				notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_DELETED_SEVERAL, deleted);
		}
		else
		{
			int deleted = 0;

			for (i = 0; i < nexceptions; ++i)
			{
				if (!stricmp(mask, exceptions[i].mask))
				{
					exception_del(i);
					notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_DELETED, mask);
					deleted = 1;
					break;
				}
			}
			if (!deleted && i == nexceptions)
				notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_NOT_FOUND, mask);
		}

		/* Renumber the exception list. I don't believe in having holes in
		 * lists - it makes code more complex, harder to debug and we end up
		 * with huge index numbers. Imho, fixed numbering is only beneficial
		 * when one doesn't have range capable manipulation. -TheShadow */

		for (i = 0; i < nexceptions; ++i)
			exceptions[i].num = i;

		if (readonly)
			notice_lang(Config.s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoMove(User *u, const std::vector<ci::string> &params)
	{
		Exception *exception;
		const char *n1str = params.size() > 1 ? params[1].c_str() : NULL; /* From position */
		const char *n2str = params.size() > 2 ? params[2].c_str() : NULL; /* To position */
		int n1, n2, i;

		if (!n2str)
		{
			this->OnSyntaxError(u, "MOVE");
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

			notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_MOVED, exceptions[n1].mask, n1 + 1, n2 + 1);

			/* Renumber the exception list. See DoDel() above for why. */
			for (i = 0; i < nexceptions; ++i)
				exceptions[i].num = i;

			if (readonly)
				notice_lang(Config.s_OperServ, u, READ_ONLY_MODE);
		}
		else
			this->OnSyntaxError(u, "MOVE");

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, const std::vector<ci::string> &params)
	{
		int sent_header = 0, i;
		expire_exceptions();
		const char *mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (mask && strspn(mask, "1234567890,-") == strlen(mask))
			process_numlist(mask, NULL, exception_list_callback, u, &sent_header);
		else
		{
			for (i = 0; i < nexceptions; ++i)
			{
				if (!mask || Anope::Match(exceptions[i].mask, mask, false))
					exception_list(u, i, &sent_header);
			}
		}
		if (!sent_header)
			notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_NO_MATCH);

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, const std::vector<ci::string> &params)
	{
		int sent_header = 0, i;
		expire_exceptions();
		const char *mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (mask && strspn(mask, "1234567890,-") == strlen(mask))
			process_numlist(mask, NULL, exception_view_callback, u, &sent_header);
		else
		{
			for (i = 0; i < nexceptions; ++i)
			{
				if (!mask || Anope::Match(exceptions[i].mask, mask, false))
					exception_view(u, i, &sent_header);
			}
		}
		if (!sent_header)
			notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_NO_MATCH);

		return MOD_CONT;
	}
 public:
	CommandOSException() : Command("EXCEPTION", 1, 5)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string cmd = params[0];

		if (!Config.LimitSessions)
		{
			notice_lang(Config.s_OperServ, u, OPER_EXCEPTION_DISABLED);
			return MOD_CONT;
		}

		if (cmd == "ADD")
			return this->DoAdd(u, params);
		else if (cmd == "DEL")
			return this->DoDel(u, params);
		else if (cmd == "MOVE")
			return this->DoMove(u, params);
		else if (cmd == "LIST")
			return this->DoList(u, params);
		else if (cmd == "VIEW")
			return this->DoView(u, params);
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_EXCEPTION);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "EXCEPTION", OPER_EXCEPTION_SYNTAX);
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

		this->AddCommand(OPERSERV, new CommandOSSession());
		this->AddCommand(OPERSERV, new CommandOSException());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_SESSION);
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_EXCEPTION);
	}
};

MODULE_INIT(OSSession)
