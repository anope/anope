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
#include "hashcomp.h"

class SNLineDelCallback : public NumberList
{
	User *u;
	unsigned Deleted;
 public:
	SNLineDelCallback(User *_u, const Anope::string &numlist) : NumberList(numlist, true), u(_u), Deleted(0)
	{
	}

	~SNLineDelCallback()
	{
		if (!Deleted)
			notice_lang(Config->s_OperServ, u, OPER_SNLINE_NO_MATCH);
		else if (Deleted == 1)
			notice_lang(Config->s_OperServ, u, OPER_SNLINE_DELETED_ONE);
		else
			notice_lang(Config->s_OperServ, u, OPER_SNLINE_DELETED_SEVERAL, Deleted);
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
	SNLineListCallback(User *_u, const Anope::string &numlist) : NumberList(numlist, false), u(_u), SentHeader(false)
	{
	}

	~SNLineListCallback()
	{
		if (!SentHeader)
			notice_lang(Config->s_OperServ, u, OPER_SNLINE_NO_MATCH);
	}

	virtual void HandleNumber(unsigned Number)
	{
		XLine *x = SNLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config->s_OperServ, u, OPER_SNLINE_LIST_HEADER);
		}

		DoList(u, x, Number - 1);
	}

	static void DoList(User *u, XLine *x, unsigned Number)
	{
		notice_lang(Config->s_OperServ, u, OPER_SNLINE_LIST_FORMAT, Number + 1, x->Mask.c_str(), x->Reason.c_str());
	}
};

class SNLineViewCallback : public SNLineListCallback
{
 public:
	SNLineViewCallback(User *_u, const Anope::string &numlist) : SNLineListCallback(_u, numlist)
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
			notice_lang(Config->s_OperServ, u, OPER_SNLINE_VIEW_HEADER);
		}

		DoList(u, x, Number - 1);
	}

	static void DoList(User *u, XLine *x, unsigned Number)
	{
		char timebuf[32];
		struct tm tm;

		tm = *localtime(&x->Created);
		strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT, &tm);
		Anope::string expirebuf = expire_left(u->Account(), x->Expires);
		notice_lang(Config->s_OperServ, u, OPER_SNLINE_VIEW_FORMAT, Number + 1, x->Mask.c_str(), x->By.c_str(), timebuf, expirebuf.c_str(), x->Reason.c_str());
	}
};

class CommandOSSNLine : public Command
{
 private:
	CommandReturn OnAdd(User *u, const std::vector<Anope::string> &params)
	{
		unsigned last_param = 2;
		Anope::string param, expiry;
		time_t expires;

		param = params.size() > 1 ? params[1] : "";
		if (!param.empty() && param[0] == '+')
		{
			expiry = param;
			param = params.size() > 2 ? params[2] : "";
			last_param = 3;
		}

		expires = !expiry.empty() ? dotime(expiry) : Config->SNLineExpiry;
		/* If the expiry given does not contain a final letter, it's in days,
		 * said the doc. Ah well.
		 */
		if (!expiry.empty() && isdigit(expiry[expiry.length() - 1]))
			expires *= 86400;
		/* Do not allow less than a minute expiry time */
		if (expires && expires < 60)
		{
			notice_lang(Config->s_OperServ, u, BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += Anope::CurTime;

		if (param.empty())
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}

		Anope::string rest = param;
		if (params.size() > last_param)
			rest += " " + params[last_param];

		if (rest.find(':') == Anope::string::npos)
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}

		sepstream sep(rest, ':');
		Anope::string mask;
		sep.GetToken(mask);
		Anope::string reason = sep.GetRemaining();

		if (!mask.empty() && !reason.empty())
		{
			/* Clean up the last character of the mask if it is a space
			 * See bug #761
			 */
			unsigned masklen = mask.length();
			if (mask[masklen - 1] == ' ')
				mask.erase(masklen - 1);

			XLine *x = SNLine->Add(OperServ, u, mask, expires, reason);

			if (!x)
				return MOD_CONT;

			notice_lang(Config->s_OperServ, u, OPER_SNLINE_ADDED, mask.c_str());

			if (Config->WallOSSNLine)
			{
				Anope::string buf;

				if (!expires)
					buf = "does not expire";
				else
				{
					time_t wall_expiry = expires - Anope::CurTime;
					Anope::string s;

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

					buf = "expires in " + stringify(wall_expiry) + " " + s + (wall_expiry == 1 ? "" : "s");
				}

				ircdproto->SendGlobops(findbot(Config->s_OperServ), "%s added an SNLINE for %s (%s)", u->nick.c_str(), mask.c_str(), buf.c_str());
			}

			if (readonly)
				notice_lang(Config->s_OperServ, u, READ_ONLY_MODE);

		}
		else
			this->OnSyntaxError(u, "ADD");

		return MOD_CONT;
	}

	CommandReturn OnDel(User *u, const std::vector<Anope::string> &params)
	{
		if (SNLine->GetList().empty())
		{
			notice_lang(Config->s_OperServ, u, OPER_SNLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SNLineDelCallback list(u, mask);
			list.Process();
		}
		else
		{
			XLine *x = SNLine->HasEntry(mask);

			if (!x)
			{
				notice_lang(Config->s_OperServ, u, OPER_SNLINE_NOT_FOUND, mask.c_str());
				return MOD_CONT;
			}

			FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, x, X_SNLINE));

			SNLineDelCallback::DoDel(u, x);
			notice_lang(Config->s_OperServ, u, OPER_SNLINE_DELETED, mask.c_str());
		}

		if (readonly)
			notice_lang(Config->s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn OnList(User *u, const std::vector<Anope::string> &params)
	{
		if (SNLine->GetList().empty())
		{
			notice_lang(Config->s_OperServ, u, OPER_SNLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SNLineListCallback list(u, mask);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = SNLine->GetCount(); i < end; ++i)
			{
				XLine *x = SNLine->GetEntry(i);

				if (mask.empty() || mask.equals_ci(x->Mask) || Anope::Match(x->Mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						notice_lang(Config->s_OperServ, u, OPER_SNLINE_LIST_HEADER);
					}

					SNLineListCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				notice_lang(Config->s_OperServ, u, OPER_SNLINE_NO_MATCH);
			else
				notice_lang(Config->s_OperServ, u, END_OF_ANY_LIST, "SNLine");
		}

		return MOD_CONT;
	}

	CommandReturn OnView(User *u, const std::vector<Anope::string> &params)
	{
		if (SNLine->GetList().empty())
		{
			notice_lang(Config->s_OperServ, u, OPER_SNLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SNLineViewCallback list(u, mask);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = SNLine->GetCount(); i < end; ++i)
			{
				XLine *x = SNLine->GetEntry(i);

				if (mask.empty() || mask.equals_ci(x->Mask) || Anope::Match(x->Mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						notice_lang(Config->s_OperServ, u, OPER_SNLINE_VIEW_HEADER);
					}

					SNLineViewCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				notice_lang(Config->s_OperServ, u, OPER_SNLINE_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn OnClear(User *u)
	{
		FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, NULL, X_SNLINE));
		SNLine->Clear();
		notice_lang(Config->s_OperServ, u, OPER_SNLINE_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSSNLine() : Command("SNLINE", 1, 3, "operserv/snline")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string cmd = params[0];

		if (cmd.equals_ci("ADD"))
			return this->OnAdd(u, params);
		else if (cmd.equals_ci("DEL"))
			return this->OnDel(u, params);
		else if (cmd.equals_ci("LIST"))
			return this->OnList(u, params);
		else if (cmd.equals_ci("VIEW"))
			return this->OnView(u, params);
		else if (cmd.equals_ci("CLEAR"))
			return this->OnClear(u);
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_OperServ, u, OPER_HELP_SNLINE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_OperServ, u, "SNLINE", OPER_SNLINE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_OperServ, u, OPER_HELP_CMD_SNLINE);
	}
};

class OSSNLine : public Module
{
	CommandOSSNLine commandossnline;

 public:
	OSSNLine(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		if (!ircd->snline)
			throw ModuleException("Your IRCd does not support SNLine");

		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandossnline);
	}
};

MODULE_INIT(OSSNLine)
