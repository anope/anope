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
#include "hashcomp.h"

int sgline_view_callback(SList *slist, int number, void *item, va_list args);
int sgline_list_callback(SList *slist, int number, void *item, va_list args);
int sgline_view(int number, SXLine *sx, User *u, int *sent_header);
int sgline_list(int number, SXLine *sx, User *u, int *sent_header);

class CommandOSSGLine : public Command
{
 private:
	CommandReturn OnAdd(User *u, const std::vector<ci::string> &params)
	{
		int deleted = 0;
		unsigned last_param = 2;
		const char *param, *expiry;
		char rest[BUFSIZE];
		time_t expires;

		param = params.size() > 1 ? params[1].c_str() : NULL;
		if (param && *param == '+')
		{
			expiry = param;
			param = params.size() > 2 ? params[2].c_str() : NULL;
			last_param = 3;
		}
		else
			expiry = NULL;

		expires = expiry ? dotime(expiry) : Config.SGLineExpiry;
		/* If the expiry given does not contain a final letter, it's in days,
		 * said the doc. Ah well.
		 */
		if (expiry && isdigit(expiry[strlen(expiry) - 1]))
			expires *= 86400;
		/* Do not allow less than a minute expiry time */
		if (expires && expires < 60)
		{
			notice_lang(Config.s_OperServ, u, BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += time(NULL);

		if (!param)
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}
		snprintf(rest, sizeof(rest), "%s%s%s", param, params.size() > last_param ? " " : "", params.size() > last_param ? params[last_param].c_str() : "");

		if (std::string(rest).find(':') == std::string::npos)
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}

		sepstream sep(rest, ':');
		std::string mask;
		sep.GetToken(mask);
		std::string reason = sep.GetRemaining();

		if (!mask.empty() && !reason.empty()) {
			/* Clean up the last character of the mask if it is a space
			 * See bug #761
			 */
			unsigned masklen = mask.size();
			if (mask[masklen - 1] == ' ')
				mask.erase(masklen - 1);

			const char *cmask = mask.c_str();

			/* We first do some sanity check on the proposed mask. */

			if (!mask.empty() && strspn(cmask, "*?") == strlen(cmask)) {
				notice_lang(Config.s_OperServ, u, USERHOST_MASK_TOO_WIDE, cmask);
				return MOD_CONT;
			}

			deleted = add_sgline(u, cmask, u->nick, expires, reason.c_str());
			if (deleted < 0)
				return MOD_CONT;
			else if (deleted)
				notice_lang(Config.s_OperServ, u, OPER_SGLINE_DELETED_SEVERAL, deleted);
			notice_lang(Config.s_OperServ, u, OPER_SGLINE_ADDED, cmask);

			if (Config.WallOSSGLine)
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

				ircdproto->SendGlobops(Config.s_OperServ, "%s added an SGLINE for %s (%s)", u->nick, cmask, buf);
			}

			if (readonly)
				notice_lang(Config.s_OperServ, u, READ_ONLY_MODE);

		}
		else
			this->OnSyntaxError(u, "ADD");

		return MOD_CONT;
	}

	CommandReturn OnDel(User *u, const std::vector<ci::string> &params)
	{
		const char *mask;
		int res = 0;

		mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (!mask)
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (!sglines.count)
		{
			notice_lang(Config.s_OperServ, u, OPER_SGLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask))
		{
			/* Deleting a range */
			res = slist_delete_range(&sglines, mask, NULL);
			if (!res)
			{
				notice_lang(Config.s_OperServ, u, OPER_SGLINE_NO_MATCH);
				return MOD_CONT;
			}
			else if (res == 1)
				notice_lang(Config.s_OperServ, u, OPER_SGLINE_DELETED_ONE);
			else
				notice_lang(Config.s_OperServ, u, OPER_SGLINE_DELETED_SEVERAL, res);
		}
		else {
			if ((res = slist_indexof(&sglines, const_cast<char *>(mask))) == -1)
			{
				notice_lang(Config.s_OperServ, u, OPER_SGLINE_NOT_FOUND, mask);
				return MOD_CONT;
			}

			slist_delete(&sglines, res);
			notice_lang(Config.s_OperServ, u, OPER_SGLINE_DELETED, mask);
		}

		if (readonly)
			notice_lang(Config.s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn OnList(User *u, const std::vector<ci::string> &params)
	{
		const char *mask;
		int res, sent_header = 0;

		if (!sglines.count) {
			notice_lang(Config.s_OperServ, u, OPER_SGLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (!mask || (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)))
		{
			res = slist_enum(&sglines, mask, &sgline_list_callback, u, &sent_header);
			if (!res)
			{
				notice_lang(Config.s_OperServ, u, OPER_SGLINE_NO_MATCH);
				return MOD_CONT;
			}
		}
		else
		{
			int i;
			char *amask;

			for (i = 0; i < sglines.count; ++i)
			{
				amask = (static_cast<SXLine *>(sglines.list[i]))->mask;
				if (!stricmp(mask, amask) || Anope::Match(amask, mask, false))
					sgline_list(i + 1, static_cast<SXLine *>(sglines.list[i]), u, &sent_header);
			}

			if (!sent_header)
				notice_lang(Config.s_OperServ, u, OPER_SGLINE_NO_MATCH);
			else
				notice_lang(Config.s_OperServ, u, END_OF_ANY_LIST, "SGLine");
		}

		return MOD_CONT;
	}

	CommandReturn OnView(User *u, const std::vector<ci::string> &params)
	{
		const char *mask;
		int res, sent_header = 0;

		if (!sglines.count)
		{
			notice_lang(Config.s_OperServ, u, OPER_SGLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (!mask || (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)))
		{
			res = slist_enum(&sglines, mask, &sgline_view_callback, u, &sent_header);
			if (!res)
			{
				notice_lang(Config.s_OperServ, u, OPER_SGLINE_NO_MATCH);
				return MOD_CONT;
			}
		}
		else
		{
			int i;
			char *amask;

			for (i = 0; i < sglines.count; ++i)
			{
				amask = (static_cast<SXLine *>(sglines.list[i]))->mask;
				if (!stricmp(mask, amask) || Anope::Match(amask, mask, false))
					sgline_view(i + 1, static_cast<SXLine *>(sglines.list[i]), u, &sent_header);
			}

			if (!sent_header)
				notice_lang(Config.s_OperServ, u, OPER_SGLINE_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn OnClear(User *u)
	{
		slist_clear(&sglines, 1);
		notice_lang(Config.s_OperServ, u, OPER_SGLINE_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSSGLine() : Command("SGLINE", 1, 3, "operserv/sgline")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string cmd = params[0];

		if (cmd == "ADD")
			return this->OnAdd(u, params);
		else if (cmd == "DEL")
			return this->OnDel(u, params);
		else if (cmd == "LIST")
			return this->OnList(u, params);
		else if (cmd == "VIEW")
			return this->OnView(u, params);
		else if (cmd == "CLEAR")
			return this->OnClear(u);
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_SGLINE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "SGLINE", OPER_SGLINE_SYNTAX);
	}
};

class OSSGLine : public Module
{
 public:
	OSSGLine(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSSGLine());

		if (!ircd->sgline)
			throw ModuleException("Your IRCd does not support SGLine");

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_SGLINE);
	}
};

/* Lists an SGLINE entry, prefixing it with the header if needed */
int sgline_view(int number, SXLine *sx, User *u, int *sent_header)
{
	char timebuf[32], expirebuf[256];
	struct tm tm;

	if (!sx)
		return 0;

	if (!*sent_header)
	{
		notice_lang(Config.s_OperServ, u, OPER_SGLINE_VIEW_HEADER);
		*sent_header = 1;
	}

	tm = *localtime(&sx->seton);
	strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT, &tm);
	expire_left(u->nc, expirebuf, sizeof(expirebuf), sx->expires);
	notice_lang(Config.s_OperServ, u, OPER_SGLINE_VIEW_FORMAT, number, sx->mask, sx->by, timebuf, expirebuf, sx->reason);

	return 1;
}

/* Callback for enumeration purposes */
int sgline_view_callback(SList *slist, int number, void *item, va_list args)
{
	User *u = va_arg(args, User *);
	int *sent_header = va_arg(args, int *);

	return sgline_view(number, static_cast<SXLine *>(item), u, sent_header);
}

/* Lists an SGLINE entry, prefixing it with the header if needed */
int sgline_list(int number, SXLine *sx, User *u, int *sent_header)
{
	if (!sx)
		return 0;

	if (!*sent_header)
	{
		notice_lang(Config.s_OperServ, u, OPER_SGLINE_LIST_HEADER);
		*sent_header = 1;
	}

	notice_lang(Config.s_OperServ, u, OPER_SGLINE_LIST_FORMAT, number, sx->mask, sx->reason);

	return 1;
}

/* Callback for enumeration purposes */
int sgline_list_callback(SList *slist, int number, void *item, va_list args)
{
	User *u = va_arg(args, User *);
	int *sent_header = va_arg(args, int *);

	return sgline_list(number, static_cast<SXLine *>(item), u, sent_header);
}

MODULE_INIT(OSSGLine)
