/* OperServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class AkillDelCallback : public NumberList
{
	User *u;
	unsigned Deleted;
 public:
	AkillDelCallback(User *_u, const std::string &numlist) : NumberList(numlist, true), u(_u), Deleted(0)
	{
	}

	~AkillDelCallback()
	{
		if (!Deleted)
			notice_lang(Config.s_OperServ, u, OPER_AKILL_NO_MATCH);
		else if (Deleted == 1)
			notice_lang(Config.s_OperServ, u, OPER_AKILL_DELETED_ONE);
		else
			notice_lang(Config.s_OperServ, u, OPER_AKILL_DELETED_SEVERAL, Deleted);
	}

	void HandleNumber(unsigned Number)
	{
		XLine *x = SGLine->GetEntry(Number - 1);

		if (!x)
			return;

		++Deleted;
		DoDel(u, x);
	}

	static void DoDel(User *u, XLine *x)
	{
		SGLine->DelXLine(x);
	}
};

class AkillListCallback : public NumberList
{
 protected:
	User *u;
	bool SentHeader;
 public:
	AkillListCallback(User *_u, const std::string &numlist) : NumberList(numlist, false), u(_u), SentHeader(false)
	{
	}

	~AkillListCallback()
	{
		if (!SentHeader)
			notice_lang(Config.s_OperServ, u, OPER_AKILL_NO_MATCH);
		else
			notice_lang(Config.s_OperServ, u, END_OF_ANY_LIST, "Akill");
	}

	void HandleNumber(unsigned Number)
	{
		XLine *x = SGLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config.s_OperServ, u, OPER_AKILL_LIST_HEADER);
		}

		DoList(u, x, Number);
	}

	static void DoList(User *u, XLine *x, unsigned Number)
	{
		notice_lang(Config.s_OperServ, u, OPER_AKILL_LIST_FORMAT, Number + 1, x->Mask.c_str(), x->Reason.c_str());
	}
};

class AkillViewCallback : public AkillListCallback
{
 public:
	AkillViewCallback(User *_u, const std::string &numlist) : AkillListCallback(_u, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		XLine *x = SGLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config.s_OperServ, u, OPER_AKILL_VIEW_HEADER);
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

		notice_lang(Config.s_OperServ, u, OPER_AKILL_VIEW_FORMAT, Number + 1, x->Mask.c_str(), x->By.c_str(), timebuf, expirebuf, x->Reason.c_str());
	}
};

class CommandOSAKill : public Command
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

		expires = expiry ? dotime(expiry) : Config.AutokillExpiry;
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
			XLine *x = SGLine->Add(OperServ, u, mask, expires, reason);

			if (!x)
				return MOD_CONT;

			notice_lang(Config.s_OperServ, u, OPER_AKILL_ADDED, mask);

			if (Config.WallOSAkill)
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

				ircdproto->SendGlobops(OperServ, "%s added an AKILL for %s (%s) (%s)", u->nick.c_str(), mask, reason, buf);
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
		const ci::string mask = params.size() > 1 ? params[0] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (SGLine->GetList().empty())
		{
			notice_lang(Config.s_OperServ, u, OPER_AKILL_LIST_EMPTY);
			return MOD_CONT;
		}

		if (isdigit(mask[0]) && strspn(mask.c_str(), "1234567890,-") == mask.length())
		{
			AkillDelCallback list(u, mask.c_str());
			list.Process();
		}
		else
		{
			XLine *x = SGLine->HasEntry(mask);

			if (!x)
			{
				notice_lang(Config.s_OperServ, u, OPER_AKILL_NOT_FOUND, mask.c_str());
				return MOD_CONT;
			}

			FOREACH_MOD(I_OnDelAkill, OnDelAkill(u, x));

			AkillDelCallback::DoDel(u, x);
			notice_lang(Config.s_OperServ, u, OPER_AKILL_DELETED, mask.c_str());
		}

		if (readonly)
			notice_lang(Config.s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, const std::vector<ci::string> &params)
	{
		if (SGLine->GetList().empty())
		{
			notice_lang(Config.s_OperServ, u, OPER_AKILL_LIST_EMPTY);
			return MOD_CONT;
		}

		const ci::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && strspn(mask.c_str(), "1234567890,-") == mask.length())
		{
			AkillListCallback list(u, mask.c_str());
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = SGLine->GetCount(); i < end; ++i)
			{
				XLine *x = SGLine->GetEntry(i);

				if (mask.empty() || mask == x->Mask || Anope::Match(x->Mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						notice_lang(Config.s_OperServ, u, OPER_AKILL_LIST_HEADER);
					}

					AkillListCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				notice_lang(Config.s_OperServ, u, OPER_AKILL_NO_MATCH);
			else
				notice_lang(Config.s_OperServ, u, END_OF_ANY_LIST, "Akill");
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, const std::vector<ci::string> &params)
	{
		if (SGLine->GetList().empty())
		{
			notice_lang(Config.s_OperServ, u, OPER_AKILL_LIST_EMPTY);
			return MOD_CONT;
		}

		const ci::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && strspn(mask.c_str(), "1234567890,-") == mask.length())
		{
			AkillViewCallback list(u, mask.c_str());
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = SGLine->GetCount(); i < end; ++i)
			{
				XLine *x = SGLine->GetEntry(i);

				if (mask.empty() || mask == x->Mask || Anope::Match(x->Mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						notice_lang(Config.s_OperServ, u, OPER_AKILL_VIEW_HEADER);
					}

					AkillViewCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				notice_lang(Config.s_OperServ, u, OPER_AKILL_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u)
	{
		FOREACH_MOD(I_OnDelAkill, OnDelAkill(u, NULL));
		SGLine->Clear();
		notice_lang(Config.s_OperServ, u, OPER_AKILL_CLEAR);

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
		notice_help(Config.s_OperServ, u, OPER_HELP_AKILL);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "AKILL", OPER_AKILL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_AKILL);
	}
};

class OSAKill : public Module
{
 public:
	OSAKill(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSAKill());
	}
};

MODULE_INIT(OSAKill)
