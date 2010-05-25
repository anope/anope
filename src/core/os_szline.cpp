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

class SZLineDelCallback : public NumberList
{
	User *u;
	unsigned Deleted;
 public:
	SZLineDelCallback(User *_u, const std::string &numlist) : NumberList(numlist), u(_u), Deleted(0)
	{
	}

	~SZLineDelCallback()
	{
		if (!Deleted)
			notice_lang(Config.s_OperServ, u, OPER_SZLINE_NO_MATCH);
		else if (Deleted == 0)
			notice_lang(Config.s_OperServ, u, OPER_SZLINE_DELETED_ONE);
		else
			notice_lang(Config.s_OperServ, u, OPER_SZLINE_DELETED_SEVERAL, Deleted);
	}

	void HandleNumber(unsigned Number)
	{
		XLine *x = SZLine->GetEntry(Number - 1);

		if (!x)
			return;

		++Deleted;
		DoDel(u, x);
	}

	static void DoDel(User *u, XLine *x)
	{
		SZLine->DelXLine(x);
	}
};

class SZLineListCallback : public NumberList
{
 protected:
	User *u;
	bool SentHeader;
 public:
	SZLineListCallback(User *_u, const std::string &numlist) : NumberList(numlist), u(_u), SentHeader(false)
	{
	}

	~SZLineListCallback()
	{
		if (!SentHeader)
			notice_lang(Config.s_OperServ, u, OPER_SZLINE_NO_MATCH);
	}

	virtual void HandleNumber(unsigned Number)
	{
		XLine *x = SZLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config.s_OperServ, u, OPER_SZLINE_LIST_HEADER);
		}

		DoList(u, x, Number);
	}

	static void DoList(User *u, XLine *x, unsigned Number)
	{
		notice_lang(Config.s_OperServ, u, OPER_SZLINE_LIST_FORMAT, Number + 1, x->Mask.c_str(), x->Reason.c_str());
	}
};

class SZLineViewCallback : public SZLineListCallback
{
 public:
	SZLineViewCallback(User *_u, const std::string &numlist) : SZLineListCallback(_u, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		XLine *x = SZLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config.s_OperServ, u, OPER_SZLINE_VIEW_HEADER);
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
		notice_lang(Config.s_OperServ, u, OPER_SZLINE_VIEW_FORMAT, Number + 1, x->Mask.c_str(), x->By.c_str(), timebuf,
expirebuf, x->Reason.c_str());
	}
};


class CommandOSSZLine : public Command
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

		expires = expiry ? dotime(expiry) : Config.SZLineExpiry;
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
			XLine *x = SZLine->Add(OperServ, u, mask, expires, reason);

			if (!x)
				return MOD_CONT;

			notice_lang(Config.s_OperServ, u, OPER_SZLINE_ADDED, mask);

			if (Config.WallOSSZLine)
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

				ircdproto->SendGlobops(OperServ, "%s added an SZLINE for %s (%s)", u->nick.c_str(), mask, buf);
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
		if (SZLine->GetList().empty())
		{
			notice_lang(Config.s_OperServ, u, OPER_SZLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		const ci::string mask = params.size() > 1 ? params[1].c_str() : "";

		if (mask.empty())
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (!mask.empty() && isdigit(mask[0]) && strspn(mask.c_str(), "1234567890,-") == mask.length())
			(new SZLineDelCallback(u, mask.c_str()))->Process();
		else
		{
			XLine *x = SZLine->HasEntry(mask);

			if (!x)
			{
				notice_lang(Config.s_OperServ, u, OPER_SZLINE_NOT_FOUND, mask.c_str());
				return MOD_CONT;
			}

			FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, x, X_SZLINE));

			SZLineDelCallback::DoDel(u, x);
			notice_lang(Config.s_OperServ, u, OPER_SZLINE_DELETED, mask.c_str());
		}

		if (readonly)
			notice_lang(Config.s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, const std::vector<ci::string> &params)
	{
		if (SZLine->GetList().empty())
		{
			notice_lang(Config.s_OperServ, u, OPER_SZLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		const ci::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && strspn(mask.c_str(), "1234567890,-") == mask.length())
			(new SZLineListCallback(u, mask.c_str()))->Process();
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0; i < SZLine->GetCount(); ++i)
			{
				XLine *x = SZLine->GetEntry(i);

				if (mask.empty() || (mask == x->Mask || Anope::Match(x->Mask, mask)))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						notice_lang(Config.s_OperServ, u, OPER_SZLINE_LIST_HEADER);
					}

					SZLineListCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				notice_lang(Config.s_OperServ, u, OPER_SZLINE_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, const std::vector<ci::string> &params)
	{
		if (SZLine->GetList().empty())
		{
			notice_lang(Config.s_OperServ, u, OPER_SZLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		const ci::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && strspn(mask.c_str(), "1234567890,-") == mask.length())
			(new SZLineViewCallback(u, mask.c_str()))->Process();
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0; i < SZLine->GetCount(); ++i)
			{
				XLine *x = SZLine->GetEntry(i);

				if (mask.empty() || (mask == x->Mask || Anope::Match(x->Mask, mask)))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						notice_lang(Config.s_OperServ, u, OPER_SZLINE_VIEW_HEADER);
					}

					SZLineViewCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				notice_lang(Config.s_OperServ, u, OPER_SZLINE_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u)
	{
		FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, NULL, X_SZLINE));
		SZLine->Clear();
		notice_lang(Config.s_OperServ, u, OPER_SZLINE_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSSZLine() : Command("SZLINE", 1, 4, "operserv/szline")
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
		notice_help(Config.s_OperServ, u, OPER_HELP_SZLINE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "SZLINE", OPER_SZLINE_SYNTAX);
	}
};

class OSSZLine : public Module
{
 public:
	OSSZLine(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSSZLine());

		if (!ircd->szline)
			throw ModuleException("Your IRCd does not support ZLINEs");

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_SZLINE);
	}
};

MODULE_INIT(OSSZLine)
