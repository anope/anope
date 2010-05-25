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
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"
#include "hashcomp.h"

class SNLineDelCallback : public NumberList
{
	User *u;
	unsigned Deleted;
 public:
	SNLineDelCallback(User *_u, const std::string &numlist) : NumberList(numlist, true), u(_u), Deleted(0)
	{
	}

	~SNLineDelCallback()
	{
		if (!Deleted)
			notice_lang(Config.s_OperServ, u, OPER_SNLINE_NO_MATCH);
		else if (Deleted == 0)
			notice_lang(Config.s_OperServ, u, OPER_SNLINE_DELETED_ONE);
		else
			notice_lang(Config.s_OperServ, u, OPER_SNLINE_DELETED_SEVERAL, Deleted);
	}

	void HandleNumber(unsigned Number)
	{
		XLine *x = SNLine->GetEntry(Number - 1);

		if (!x)
			return;

		++Deleted;
		DoDel(u, x);
	}

	static void DoDel(User *u, XLine *x)
	{
		SNLine->DelXLine(x);
	}
};

class SNLineListCallback : public NumberList
{
 protected:
	User *u;
	bool SentHeader;
 public:
	SNLineListCallback(User *_u, const std::string &numlist) : NumberList(numlist, false), u(_u), SentHeader(false)
	{
	}

	~SNLineListCallback()
	{
		if (!SentHeader)
			notice_lang(Config.s_OperServ, u, OPER_SNLINE_NO_MATCH);
	}

	virtual void HandleNumber(unsigned Number)
	{
		XLine *x = SNLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config.s_OperServ, u, OPER_SNLINE_LIST_HEADER);
		}

		DoList(u, x, Number - 1);
	}

	static void DoList(User *u, XLine *x, unsigned Number)
	{
		notice_lang(Config.s_OperServ, u, OPER_SNLINE_LIST_FORMAT, Number + 1, x->Mask.c_str(), x->Reason.c_str());
	}
};

class SNLineViewCallback : public SNLineListCallback
{
 public:
	SNLineViewCallback(User *_u, const std::string &numlist) : SNLineListCallback(_u, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		XLine *x = SNLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config.s_OperServ, u, OPER_SNLINE_VIEW_HEADER);
		}

		DoList(u, x, Number - 1);
	}

	static void DoList(User *u, XLine *x, unsigned Number)
	{
		char timebuf[32], expirebuf[256];
		struct tm tm;

		tm = *localtime(&x->Created);
		strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT, &tm);
		expire_left(u->Account(), expirebuf, sizeof(expirebuf), x->Expires);
		notice_lang(Config.s_OperServ, u, OPER_SNLINE_VIEW_FORMAT, Number + 1, x->Mask.c_str(), x->By.c_str(), timebuf, expirebuf, x->Reason.c_str());
	}
};

class CommandOSSNLine : public Command
{
 private:
	CommandReturn OnAdd(User *u, const std::vector<ci::string> &params)
	{
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

		expires = expiry ? dotime(expiry) : Config.SNLineExpiry;
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
		ci::string mask;
		sep.GetToken(mask);
		std::string reason = sep.GetRemaining();

		if (!mask.empty() && !reason.empty()) {
			/* Clean up the last character of the mask if it is a space
			 * See bug #761
			 */
			unsigned masklen = mask.size();
			if (mask[masklen - 1] == ' ')
				mask.erase(masklen - 1);

			XLine *x = SNLine->Add(OperServ, u, mask, expires, reason);

			if (!x)
				return MOD_CONT;

			notice_lang(Config.s_OperServ, u, OPER_SNLINE_ADDED, mask.c_str());

			if (Config.WallOSSNLine)
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

				ircdproto->SendGlobops(findbot(Config.s_OperServ), "%s added an SNLINE for %s (%s)", u->nick.c_str(), mask.c_str(), buf);
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
		if (SNLine->GetList().empty())
		{
			notice_lang(Config.s_OperServ, u, OPER_SNLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		const ci::string mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (isdigit(mask[0]) && strspn(mask.c_str(), "1234567890,-") == mask.length())
			(new SNLineDelCallback(u, mask.c_str()))->Process();
		else
		{
			XLine *x = SNLine->HasEntry(mask);

			if (!x)
			{
				notice_lang(Config.s_OperServ, u, OPER_SNLINE_NOT_FOUND, mask.c_str());
				return MOD_CONT;
			}

			FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, x, X_SNLINE));

			SNLineDelCallback::DoDel(u, x);
			notice_lang(Config.s_OperServ, u, OPER_SNLINE_DELETED, mask.c_str());
		}

		if (readonly)
			notice_lang(Config.s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn OnList(User *u, const std::vector<ci::string> &params)
	{
		if (SNLine->GetList().empty())
		{
			notice_lang(Config.s_OperServ, u, OPER_SNLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		const ci::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && strspn(mask.c_str(), "1234567890,-") == mask.length())
			(new SNLineListCallback(u, mask.c_str()))->Process();
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0; i < SNLine->GetCount(); ++i)
			{
				XLine *x = SNLine->GetEntry(i);

				if (mask.empty() || (mask == x->Mask || Anope::Match(x->Mask, mask)))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						notice_lang(Config.s_OperServ, u, OPER_SNLINE_LIST_HEADER);
					}

					SNLineListCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				notice_lang(Config.s_OperServ, u, OPER_SNLINE_NO_MATCH);
			else
				notice_lang(Config.s_OperServ, u, END_OF_ANY_LIST, "SNLine");
		}

		return MOD_CONT;
	}

	CommandReturn OnView(User *u, const std::vector<ci::string> &params)
	{
		if (SNLine->GetList().empty())
		{
			notice_lang(Config.s_OperServ, u, OPER_SNLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		const ci::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && strspn(mask.c_str(), "1234567890,-") == mask.length())
			(new SNLineViewCallback(u, mask.c_str()))->Process();
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0; i < SNLine->GetCount(); ++i)
			{
				XLine *x = SNLine->GetEntry(i);

				if (mask.empty() || (mask == x->Mask || Anope::Match(x->Mask, mask)))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						notice_lang(Config.s_OperServ, u, OPER_SNLINE_VIEW_HEADER);
					}

					SNLineViewCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				notice_lang(Config.s_OperServ, u, OPER_SNLINE_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn OnClear(User *u)
	{
		FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, NULL, X_SNLINE));
		SNLine->Clear();
		notice_lang(Config.s_OperServ, u, OPER_SNLINE_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSSNLine() : Command("SNLINE", 1, 3, "operserv/snline")
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
		notice_help(Config.s_OperServ, u, OPER_HELP_SNLINE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "SNLINE", OPER_SNLINE_SYNTAX);
	}
};

class OSSNLine : public Module
{
 public:
	OSSNLine(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSSNLine());

		if (!ircd->snline)
			throw ModuleException("Your IRCd does not support SNLine");

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_SNLINE);
	}
};

MODULE_INIT(OSSNLine)
