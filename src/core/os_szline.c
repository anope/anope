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

int szline_view_callback(SList *slist, int number, void *item, va_list args);
int szline_list_callback(SList *slist, int number, void *item, va_list args);
int szline_view(int number, SXLine *sx, User *u, int *sent_header);
int szline_list(int number, SXLine *sx, User *u, int *sent_header);

class CommandOSSZLine : public Command
{
 private:
	CommandReturn DoAdd(User *u, std::vector<std::string> &params)
	{
		int deleted = 0;
		unsigned last_param = 2;
		const char *expiry, *mask;
		char reason[BUFSIZE];
		time_t expires;

		mask = params.size() > 1 ? params[1].c_str() : NULL;
		if (mask && *mask == '+')
		{
			expiry = mask;
			mask = params.size() > 2 ? params[2].c_str() : NULL;
			last_param = 3;
		}
		else
			expiry = NULL;

		expires = expiry ? dotime(expiry) : SZLineExpiry;
		/* If the expiry given does not contain a final letter, it's in days,
		 * said the doc. Ah well.
		 */
		if (expiry && isdigit(expiry[strlen(expiry) - 1]))
			expires *= 86400;
		/* Do not allow less than a minute expiry time */
		if (expires != 0 && expires < 60)
		{
			notice_lang(s_OperServ, u, BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += time(NULL);

		if (params.size() <= last_param)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}
		snprintf(reason, sizeof(reason), "%s%s%s", params[last_param].c_str(), last_param == 2 && params.size() > 3 ? " " : "", last_param == 2 && params.size() > 3 ? params[3].c_str() : "");
		if (mask && *reason)
		{
			/* We first do some sanity check on the proposed mask. */

			if (strchr(mask, '!') || strchr(mask, '@'))
			{
				notice_lang(s_OperServ, u, OPER_SZLINE_ONLY_IPS);
				return MOD_CONT;
			}

			if (strspn(mask, "*?") == strlen(mask))
			{
				notice_lang(s_OperServ, u, USERHOST_MASK_TOO_WIDE, mask);
				return MOD_CONT;
			}

			deleted = add_szline(u, mask, u->nick, expires, reason);
			if (deleted < 0)
				return MOD_CONT;
			else if (deleted)
				notice_lang(s_OperServ, u, OPER_SZLINE_DELETED_SEVERAL, deleted);
			notice_lang(s_OperServ, u, OPER_SZLINE_ADDED, mask);

			if (WallOSSZLine)
			{
				char buf[128];

				if (!expires)
					strcpy(buf, "does not expire");
				else
				{
					int wall_expiry = expires - time(NULL);
					const char *s = NULL;

					if (wall_expiry >= 86400)
					{
						wall_expiry /= 86400;
						s = "day";
					}
					else if (wall_expiry >= 3600)
					{
						wall_expiry /= 3600;
						s = "hour";
					}
					else if (wall_expiry >= 60)
					{
						wall_expiry /= 60;
						s = "minute";
					}

					snprintf(buf, sizeof(buf), "expires in %d %s%s", wall_expiry, s, wall_expiry == 1 ? "" : "s");
				}

				ircdproto->SendGlobops(s_OperServ, "%s added an SZLINE for %s (%s)", u->nick, mask, buf);
			}

			if (readonly)
				notice_lang(s_OperServ, u, READ_ONLY_MODE);

		}
		else
			this->OnSyntaxError(u);

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, std::vector<std::string> &params)
	{
		const char *mask;
		int res = 0;

		mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (!mask)
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!szlines.count)
		{
			notice_lang(s_OperServ, u, OPER_SZLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask))
		{
			/* Deleting a range */
			res = slist_delete_range(&szlines, mask, NULL);
			if (!res)
			{
				notice_lang(s_OperServ, u, OPER_SZLINE_NO_MATCH);
				return MOD_CONT;
			}
			else if (res == 1)
				notice_lang(s_OperServ, u, OPER_SZLINE_DELETED_ONE);
			else
				notice_lang(s_OperServ, u, OPER_SZLINE_DELETED_SEVERAL, res);
		}
		else
		{
			if ((res = slist_indexof(&szlines, const_cast<char *>(mask))) == -1)
			{
				notice_lang(s_OperServ, u, OPER_SZLINE_NOT_FOUND, mask);
				return MOD_CONT;
			}

			slist_delete(&szlines, res);
			notice_lang(s_OperServ, u, OPER_SZLINE_DELETED, mask);
		}

		if (readonly)
			notice_lang(s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, std::vector<std::string> &params)
	{
		const char *mask;
		int res, sent_header = 0;

		if (!szlines.count)
		{
			notice_lang(s_OperServ, u, OPER_SZLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (!mask || (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)))
		{
			res = slist_enum(&szlines, mask, &szline_list_callback, u, &sent_header);
			if (!res)
			{
				notice_lang(s_OperServ, u, OPER_SZLINE_NO_MATCH);
				return MOD_CONT;
			}
		}
		else
		{
			int i;
			char *amask;

			for (i = 0; i < szlines.count; ++i)
			{
				amask = (static_cast<SXLine *>(szlines.list[i]))->mask;
				if (!stricmp(mask, amask) || Anope::Match(amask, mask, false))
					szline_list(i + 1, static_cast<SXLine *>(szlines.list[i]), u, &sent_header);
			}

			if (!sent_header)
				notice_lang(s_OperServ, u, OPER_SZLINE_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, std::vector<std::string> &params)
	{
		const char *mask;
		int res, sent_header = 0;

		if (!szlines.count)
		{
			notice_lang(s_OperServ, u, OPER_SZLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (!mask || (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)))
		{
			res = slist_enum(&szlines, mask, &szline_view_callback, u, &sent_header);
			if (!res)
			{
				notice_lang(s_OperServ, u, OPER_SZLINE_NO_MATCH);
				return MOD_CONT;
			}
		}
		else
		{
			int i;
			char *amask;

			for (i = 0; i < szlines.count; ++i)
			{
				amask = (static_cast<SXLine *>(szlines.list[i]))->mask;
				if (!stricmp(mask, amask) || Anope::Match(amask, mask, false))
					szline_view(i + 1, static_cast<SXLine *>(szlines.list[i]), u, &sent_header);
			}

			if (!sent_header)
				notice_lang(s_OperServ, u, OPER_SZLINE_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u, std::vector<std::string> &params)
	{
		slist_clear(&szlines, 1);
		notice_lang(s_OperServ, u, OPER_SZLINE_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSSZLine() : Command("SZLINE", 1, 4, "operserv/szline")
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
		else if (!stricmp(cmd, "VIEW"))
			return this->DoView(u, params);
		else if (!stricmp(cmd, "CLEAR"))
			return this->DoClear(u, params);
		else
			this->OnSyntaxError(u);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_SZLINE);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "SZLINE", OPER_SZLINE_SYNTAX);
	}
};

class OSSZLine : public Module
{
 public:
	OSSZLine(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSSZLine(), MOD_UNIQUE);

		if (!ircd->szline)
			throw ModuleException("Your IRCd does not support ZLINEs");
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_SZLINE);
	}
};

int szline_view(int number, SXLine *sx, User *u, int *sent_header)
{
	char timebuf[32], expirebuf[256];
	struct tm tm;

	if (!sx)
		return 0;

	if (!*sent_header)
	{
		notice_lang(s_OperServ, u, OPER_SZLINE_VIEW_HEADER);
		*sent_header = 1;
	}

	tm = *localtime(&sx->seton);
	strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT, &tm);
	expire_left(u->nc, expirebuf, sizeof(expirebuf), sx->expires);
	notice_lang(s_OperServ, u, OPER_SZLINE_VIEW_FORMAT, number, sx->mask, sx->by, timebuf, expirebuf, sx->reason);

	return 1;
}

/* Callback for enumeration purposes */
int szline_view_callback(SList *slist, int number, void *item, va_list args)
{
	User *u = va_arg(args, User *);
	int *sent_header = va_arg(args, int *);

	return szline_view(number, static_cast<SXLine *>(item), u, sent_header);
}

/* Callback for enumeration purposes */
int szline_list_callback(SList *slist, int number, void *item, va_list args)
{
	User *u = va_arg(args, User *);
	int *sent_header = va_arg(args, int *);

	return szline_list(number, static_cast<SXLine *>(item), u, sent_header);
}

/* Lists an SZLINE entry, prefixing it with the header if needed */
int szline_list(int number, SXLine *sx, User *u, int *sent_header)
{
	if (!sx)
		return 0;

	if (!*sent_header)
	{
		notice_lang(s_OperServ, u, OPER_SZLINE_LIST_HEADER);
		*sent_header = 1;
	}

	notice_lang(s_OperServ, u, OPER_SZLINE_LIST_FORMAT, number, sx->mask, sx->reason);

	return 1;
}

MODULE_INIT("os_szline", OSSZLine)
