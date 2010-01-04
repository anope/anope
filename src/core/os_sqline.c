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

int sqline_view_callback(SList *slist, int number, void *item, va_list args);
int sqline_list_callback(SList *slist, int number, void *item, va_list args);
int sqline_view(int number, SXLine *sx, User *u, int *sent_header);
int sqline_list(int number, SXLine *sx, User *u, int *sent_header);

class CommandOSSQLine : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<ci::string> &params)
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

		expires = expiry ? dotime(expiry) : Config.SQLineExpiry;
		/* If the expiry given does not contain a final letter, it's in days,
		 * said the doc. Ah well.
		 */
		if (expiry && isdigit(expiry[strlen(expiry) - 1]))
			expires *= 86400;
		/* Do not allow less than a minute expiry time */
		if (expires != 0 && expires < 60)
		{
			notice_lang(Config.s_OperServ, u, BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += time(NULL);

		if (params.size() <= last_param)
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}
		snprintf(reason, sizeof(reason), "%s%s%s", params[last_param].c_str(), last_param == 2 && params.size() > 3 ? " " : "", last_param == 2 && params.size() > 3 ? params[3].c_str() : "");
		if (mask && *reason)
		{
			/* We first do some sanity check on the proposed mask. */
			if (strspn(mask, "*") == strlen(mask))
			{
				notice_lang(Config.s_OperServ, u, USERHOST_MASK_TOO_WIDE, mask);
				return MOD_CONT;
			}

			/* Channel SQLINEs are only supported on Bahamut servers */
			if (*mask == '#' && !ircd->chansqline)
			{
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_CHANNELS_UNSUPPORTED);
				return MOD_CONT;
			}

			deleted = add_sqline(u, mask, u->nick.c_str(), expires, reason);
			if (deleted < 0)
				return MOD_CONT;
			else if (deleted)
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_DELETED_SEVERAL, deleted);
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_ADDED, mask);

			if (Config.WallOSSQLine)
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

				ircdproto->SendGlobops(findbot(Config.s_OperServ), "%s added an SQLINE for %s (%s)", u->nick.c_str(), mask, buf);
			}

			if (readonly)
				notice_lang(Config.s_OperServ, u, READ_ONLY_MODE);

		}
		else
			this->OnSyntaxError(u, "ADD");

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, const std::vector<ci::string> &params)
	{
		const char *mask;
		int res = 0;

		mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (!mask)
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (!sqlines.count)
		{
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask))
		{
			/* Deleting a range */
			res = slist_delete_range(&sqlines, mask, NULL);
			if (!res)
			{
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_NO_MATCH);
				return MOD_CONT;
			}
			else if (res == 1)
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_DELETED_ONE);
			else
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_DELETED_SEVERAL, res);
		}
		else {
			if ((res = slist_indexof(&sqlines, const_cast<char *>(mask))) == -1)
			{
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_NOT_FOUND, mask);
				return MOD_CONT;
			}

			slist_delete(&sqlines, res);
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_DELETED, mask);
		}

		if (readonly)
			notice_lang(Config.s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, const std::vector<ci::string> &params)
	{
		const char *mask;
		int res, sent_header = 0;

		if (!sqlines.count)
		{
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (!mask || (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)))
		{
			res = slist_enum(&sqlines, mask, &sqline_list_callback, u, &sent_header);
			if (!res)
			{
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_NO_MATCH);
				return MOD_CONT;
			}
		}
		else
		{
			int i;
			char *amask;

			for (i = 0; i < sqlines.count; ++i)
			{
				amask = (static_cast<SXLine *>(sqlines.list[i]))->mask;
				if (!stricmp(mask, amask) || Anope::Match(amask, mask, false))
					sqline_list(i + 1, static_cast<SXLine *>(sqlines.list[i]), u, &sent_header);
			}

			if (!sent_header)
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_NO_MATCH);
			else
				notice_lang(Config.s_OperServ, u, END_OF_ANY_LIST, "SQLine");
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, const std::vector<ci::string> &params)
	{
		const char *mask;
		int res, sent_header = 0;

		if (!sqlines.count)
		{
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (!mask || (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)))
		{
			res = slist_enum(&sqlines, mask, &sqline_view_callback, u, &sent_header);
			if (!res)
			{
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_NO_MATCH);
				return MOD_CONT;
			}
		}
		else
		{
			int i;
			char *amask;

			for (i = 0; i < sqlines.count; ++i)
			{
				amask = (static_cast<SXLine *>(sqlines.list[i]))->mask;
				if (!stricmp(mask, amask) || Anope::Match(amask, mask, false))
					sqline_view(i + 1, static_cast<SXLine *>(sqlines.list[i]), u, &sent_header);
			}

			if (!sent_header)
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u)
	{
		slist_clear(&sqlines, 1);
		notice_lang(Config.s_OperServ, u, OPER_SQLINE_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSSQLine() : Command("SQLINE", 1, 4, "operserv/sqline")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string cmd = params[0];

		if (cmd == "ADD")
			return this->DoAdd(u, params);
		else if (cmd == "DEL")
			return this->DoDel(u, params);
		else if (cmd == "LIST")
			return this->DoList(u, params);
		else if (cmd == "VIEW")
			return this->DoView(u, params);
		else if (cmd == "CLEAR")
			return this->DoClear(u);
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_SQLINE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "SQLINE", OPER_SQLINE_SYNTAX);
	}
};

class OSSQLine : public Module
{
 public:
	OSSQLine(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSSQLine());

		if (!ircd->sqline)
			throw ModuleException("Your IRCd does not support QLines.");

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_SQLINE);
	}
};

int sqline_view(int number, SXLine *sx, User *u, int *sent_header)
{
	char timebuf[32], expirebuf[256];
	struct tm tm;

	if (!sx)
		return 0;

	if (!*sent_header)
	{
		notice_lang(Config.s_OperServ, u, OPER_SQLINE_VIEW_HEADER);
		*sent_header = 1;
	}

	tm = *localtime(&sx->seton);
	strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT, &tm);
	expire_left(u->nc, expirebuf, sizeof(expirebuf), sx->expires);
	notice_lang(Config.s_OperServ, u, OPER_SQLINE_VIEW_FORMAT, number, sx->mask, sx->by, timebuf, expirebuf, sx->reason);

	return 1;
}

/* Callback for enumeration purposes */
int sqline_view_callback(SList *slist, int number, void *item, va_list args)
{
	User *u = va_arg(args, User *);
	int *sent_header = va_arg(args, int *);

	return sqline_view(number, static_cast<SXLine *>(item), u, sent_header);
}

/* Lists an SQLINE entry, prefixing it with the header if needed */
int sqline_list(int number, SXLine *sx, User *u, int *sent_header)
{
	if (!sx)
		return 0;

	if (!*sent_header)
	{
		notice_lang(Config.s_OperServ, u, OPER_SQLINE_LIST_HEADER);
		*sent_header = 1;
	}

	notice_lang(Config.s_OperServ, u, OPER_SQLINE_LIST_FORMAT, number, sx->mask, sx->reason);

	return 1;
}

/* Callback for enumeration purposes */
int sqline_list_callback(SList *slist, int number, void *item, va_list args)
{
	User *u = va_arg(args, User *);
	int *sent_header = va_arg(args, int *);

	return sqline_list(number, static_cast<SXLine *>(item), u, sent_header);
}

MODULE_INIT(OSSQLine)
