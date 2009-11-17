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

int akill_view_callback(SList *slist, int number, void *item, va_list args);
int akill_list_callback(SList *slist, int number, void *item, va_list args);
int akill_view(int number, Akill *ak, User *u, int *sent_header);
int akill_list(int number, Akill *ak, User *u, int *sent_header);

class CommandOSAKill : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<ci::string> &params)
	{
		int deleted = 0;
		unsigned last_param = 2;
		const char *expiry, *mask;
		char reason[BUFSIZE], realreason[BUFSIZE];
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

		expires = expiry ? dotime(expiry) : AutokillExpiry;
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
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}
		snprintf(reason, sizeof(reason), "%s%s%s", params[last_param].c_str(), last_param == 2 && params.size() > 3 ? " " : "", last_param == 2 && params.size() > 3 ? params[3].c_str() : "");
		if (mask && *reason) {
			/* We first do some sanity check on the proposed mask. */
			if (strchr(mask, '!'))
			{
				notice_lang(s_OperServ, u, OPER_AKILL_NO_NICK);
				return MOD_CONT;
			}

			if (!strchr(mask, '@'))
			{
				notice_lang(s_OperServ, u, BAD_USERHOST_MASK);
				return MOD_CONT;
			}

			if (mask && strspn(mask, "~@.*?") == strlen(mask))
			{
				notice_lang(s_OperServ, u, USERHOST_MASK_TOO_WIDE, mask);
				return MOD_CONT;
			}

			/**
			 * Changed sprintf() to snprintf()and increased the size of
			 * breason to match bufsize
			 * -Rob
			 **/
			if (AddAkiller)
				snprintf(realreason, sizeof(realreason), "[%s] %s", u->nick, reason);
			else
				snprintf(realreason, sizeof(realreason), "%s", reason);

			deleted = add_akill(u, mask, u->nick, expires, realreason);
			if (deleted < 0)
				return MOD_CONT;
			else if (deleted)
				notice_lang(s_OperServ, u, OPER_AKILL_DELETED_SEVERAL, deleted);
			notice_lang(s_OperServ, u, OPER_AKILL_ADDED, mask);

			if (WallOSAkill)
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

				ircdproto->SendGlobops(s_OperServ, "%s added an AKILL for %s (%s) (%s)", u->nick, mask, realreason, buf);
			}

			if (readonly)
				notice_lang(s_OperServ, u, READ_ONLY_MODE);
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

		if (!akills.count)
		{
			notice_lang(s_OperServ, u, OPER_AKILL_LIST_EMPTY);
			return MOD_CONT;
		}

		if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask))
		{
			/* Deleting a range */
			res = slist_delete_range(&akills, mask, NULL);
			if (!res)
			{
				notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
				return MOD_CONT;
			}
			else if (res == 1)
				notice_lang(s_OperServ, u, OPER_AKILL_DELETED_ONE);
			else
				notice_lang(s_OperServ, u, OPER_AKILL_DELETED_SEVERAL, res);
		}
		else
		{
			if ((res = slist_indexof(&akills, const_cast<void *>(static_cast<const void *>(mask)))) == -1) // XXX: possibly unsafe cast
			{
				notice_lang(s_OperServ, u, OPER_AKILL_NOT_FOUND, mask);
				return MOD_CONT;
			}

			slist_delete(&akills, res);
			notice_lang(s_OperServ, u, OPER_AKILL_DELETED, mask);
		}

		if (readonly)
			notice_lang(s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, const std::vector<ci::string> &params)
	{
		const char *mask;
		int res, sent_header = 0;

		if (!akills.count)
		{
			notice_lang(s_OperServ, u, OPER_AKILL_LIST_EMPTY);
			return MOD_CONT;
		}

		mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (!mask || (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)))
		{
			res = slist_enum(&akills, mask, &akill_list_callback, u, &sent_header);
			if (!res)
			{
				notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
				return MOD_CONT;
			}
			else
				notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Akill");
		}
		else
		{
			int i;
			char amask[BUFSIZE];

			for (i = 0; i < akills.count; ++i)
			{
				snprintf(amask, sizeof(amask), "%s@%s", (static_cast<Akill *>(akills.list[i]))->user, (static_cast<Akill *>(akills.list[i]))->host);
				if (!stricmp(mask, amask) || Anope::Match(amask, mask, false))
					akill_list(i + 1, static_cast<Akill *>(akills.list[i]), u, &sent_header);
			}

			if (!sent_header)
				notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
			else
				notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Akill");
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, const std::vector<ci::string> &params)
	{
		const char *mask;
		int res, sent_header = 0;

		if (!akills.count)
		{
			notice_lang(s_OperServ, u, OPER_AKILL_LIST_EMPTY);
			return MOD_CONT;
		}

		mask = params.size() > 1 ? params[1].c_str() : NULL;

		if (!mask || (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)))
		{
			res = slist_enum(&akills, mask, &akill_view_callback, u, &sent_header);
			if (!res)
			{
				notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
				return MOD_CONT;
			}
		}
		else
		{
			int i;
			char amask[BUFSIZE];

			for (i = 0; i < akills.count; ++i)
			{
				snprintf(amask, sizeof(amask), "%s@%s", (static_cast<Akill *>(akills.list[i]))->user, (static_cast<Akill *>(akills.list[i]))->host);
				if (!stricmp(mask, amask) || Anope::Match(amask, mask, false))
					akill_view(i + 1, static_cast<Akill *>(akills.list[i]), u, &sent_header);
			}

			if (!sent_header)
				notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u)
	{
		slist_clear(&akills, 1);
		notice_lang(s_OperServ, u, OPER_AKILL_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSAKill() : Command("AKILL", 1, 4, "operserv/akill")
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
		notice_help(s_OperServ, u, OPER_HELP_AKILL);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(s_OperServ, u, "AKILL", OPER_AKILL_SYNTAX);
	}
};

class OSAKill : public Module
{
 public:
	OSAKill(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(OPERSERV, new CommandOSAKill());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_AKILL);
	}
};

int akill_view(int number, Akill *ak, User *u, int *sent_header)
{
	char mask[BUFSIZE];
	char timebuf[32], expirebuf[256];
	struct tm tm;

	if (!ak)
		return 0;

	if (!*sent_header)
	{
		notice_lang(s_OperServ, u, OPER_AKILL_VIEW_HEADER);
		*sent_header = 1;
	}

	snprintf(mask, sizeof(mask), "%s@%s", ak->user, ak->host);
	tm = *localtime(&ak->seton);
	strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT, &tm);
	expire_left(u->nc, expirebuf, sizeof(expirebuf), ak->expires);
	notice_lang(s_OperServ, u, OPER_AKILL_VIEW_FORMAT, number, mask, ak->by, timebuf, expirebuf, ak->reason);

	return 1;
}

/* Lists an AKILL entry, prefixing it with the header if needed */
int akill_list_callback(SList *slist, int number, void *item, va_list args)
{
	User *u = va_arg(args, User *);
	int *sent_header = va_arg(args, int *);

	return akill_list(number, static_cast<Akill *>(item), u, sent_header);
}

/* Callback for enumeration purposes */
int akill_view_callback(SList *slist, int number, void *item, va_list args)
{
	User *u = va_arg(args, User *);
	int *sent_header = va_arg(args, int *);

	return akill_view(number, static_cast<Akill *>(item), u, sent_header);
}

/* Lists an AKILL entry, prefixing it with the header if needed */
int akill_list(int number, Akill *ak, User *u, int *sent_header)
{
	char mask[BUFSIZE];

	if (!ak)
		return 0;

	if (!*sent_header)
	{
		notice_lang(s_OperServ, u, OPER_AKILL_LIST_HEADER);
		*sent_header = 1;
	}

	snprintf(mask, sizeof(mask), "%s@%s", ak->user, ak->host);
	notice_lang(s_OperServ, u, OPER_AKILL_LIST_FORMAT, number, mask, ak->reason);

	return 1;
}

MODULE_INIT(OSAKill)
