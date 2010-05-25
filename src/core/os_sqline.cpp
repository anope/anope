/* OperServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class SQLineDelCallback : public NumberList
{
	User *u;
	unsigned Deleted;
 public:
	SQLineDelCallback(User *_u, const std::string &numlist) : NumberList(numlist, true), u(_u), Deleted(0)
	{
	}

	~SQLineDelCallback()
	{
		if (!Deleted)
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_NO_MATCH);
		else if (Deleted == 0)
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_DELETED_ONE);
		else
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_DELETED_SEVERAL, Deleted);
	}

	void HandleNumber(unsigned Number)
	{
		XLine *x = SQLine->GetEntry(Number - 1);

		if (!x)
			return;

		++Deleted;
		DoDel(u, x - 1);
	}

	static void DoDel(User *u, XLine *x)
	{
		SQLine->DelXLine(x);
	}
};

class SQLineListCallback : public NumberList
{
 protected:
	User *u;
	bool SentHeader;
 public:
	SQLineListCallback(User *_u, const std::string &numlist) : NumberList(numlist, false), u(_u), SentHeader(false)
	{
	}

	~SQLineListCallback()
	{
		if (!SentHeader)
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_NO_MATCH);
	}

	virtual void HandleNumber(unsigned Number)
	{
		XLine *x = SQLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_LIST_HEADER);
		}

		DoList(u, x, Number - 1);
	}

	static void DoList(User *u, XLine *x, unsigned Number)
	{
		notice_lang(Config.s_OperServ, u, OPER_SQLINE_LIST_FORMAT, Number + 1, x->Mask.c_str(), x->Reason.c_str());
	}
};

class SQLineViewCallback : public SQLineListCallback
{
 public:
	SQLineViewCallback(User *_u, const std::string &numlist) : SQLineListCallback(_u, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		XLine *x = SQLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_VIEW_HEADER);
		}

		DoList(u, x, Number);
	}

	static void DoList(User *u, XLine *x, unsigned Number)
	{
		char timebuf[32], expirebuf[256];
		struct tm tm;

		tm = *localtime(&x->Created);
		strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT, &tm);
		expire_left(u->Account(), expirebuf, sizeof(expirebuf), x->Expires);
		notice_lang(Config.s_OperServ, u, OPER_SQLINE_VIEW_FORMAT, Number + 1, x->Mask.c_str(), x->By.c_str(), timebuf, 
expirebuf, x->Reason.c_str());
	}
};


class CommandOSSQLine : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<ci::string> &params)
	{
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
			XLine *x = SQLine->Add(OperServ, u, mask, expires, reason);
			
			if (!x)
				return MOD_CONT;

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

				ircdproto->SendGlobops(OperServ, "%s added an SQLINE for %s (%s)", u->nick.c_str(), mask, buf);
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
		if (SQLine->GetList().empty())
		{
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		const ci::string mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (!mask.empty() && isdigit(mask[0]) && strspn(mask.c_str(), "1234567890,-") == mask.length())
			(new SQLineDelCallback(u, mask.c_str()))->Process();
		else
		{
			XLine *x = SQLine->HasEntry(mask);

			if (!x)
			{
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_NOT_FOUND, mask.c_str());
				return MOD_CONT;
			}

			FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, x, X_SQLINE));

			SQLineDelCallback::DoDel(u, x);
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_DELETED, mask.c_str());
		}

		if (readonly)
			notice_lang(Config.s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, const std::vector<ci::string> &params)
	{
		if (SQLine->GetList().empty())
		{
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		const ci::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && strspn(mask.c_str(), "1234567890,-") == mask.length())
			(new SQLineListCallback(u, mask.c_str()))->Process();
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0; i < SQLine->GetCount(); ++i)
			{
				XLine *x = SQLine->GetEntry(i);

				if (mask.empty() || (mask == x->Mask || Anope::Match(x->Mask, mask)))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						notice_lang(Config.s_OperServ, u, OPER_SQLINE_LIST_HEADER);
					}

					SQLineListCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_NO_MATCH);
			else
				notice_lang(Config.s_OperServ, u, END_OF_ANY_LIST, "SQLine");
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, const std::vector<ci::string> &params)
	{
		if (SQLine->GetList().empty())
		{
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		const ci::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && strspn(mask.c_str(), "1234567890,-") == mask.length())
			(new SQLineViewCallback(u, mask.c_str()))->Process();
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0; i < SQLine->GetCount(); ++i)
			{
				XLine *x = SQLine->GetEntry(i);

				if (mask.empty() || (mask == x->Mask || Anope::Match(x->Mask, mask)))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						notice_lang(Config.s_OperServ, u, OPER_SQLINE_VIEW_HEADER);
					}

					SQLineViewCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				notice_lang(Config.s_OperServ, u, OPER_SQLINE_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u)
	{
		FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, NULL, X_SQLINE));
		SGLine->Clear();
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
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSSQLine());

		if (!ircd->sqline)
			throw ModuleException("Your IRCd does not support QLines.");

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_SQLINE);
	}
};

MODULE_INIT(OSSQLine)
