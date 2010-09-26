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

class SQLineDelCallback : public NumberList
{
	User *u;
	unsigned Deleted;
 public:
	SQLineDelCallback(User *_u, const Anope::string &numlist) : NumberList(numlist, true), u(_u), Deleted(0)
	{
	}

	~SQLineDelCallback()
	{
		if (!Deleted)
			u->SendMessage(OperServ, OPER_SQLINE_NO_MATCH);
		else if (Deleted == 1)
			u->SendMessage(OperServ, OPER_SQLINE_DELETED_ONE);
		else
			u->SendMessage(OperServ, OPER_SQLINE_DELETED_SEVERAL, Deleted);
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
	SQLineListCallback(User *_u, const Anope::string &numlist) : NumberList(numlist, false), u(_u), SentHeader(false)
	{
	}

	~SQLineListCallback()
	{
		if (!SentHeader)
			u->SendMessage(OperServ, OPER_SQLINE_NO_MATCH);
	}

	virtual void HandleNumber(unsigned Number)
	{
		XLine *x = SQLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			u->SendMessage(OperServ, OPER_SQLINE_LIST_HEADER);
		}

		DoList(u, x, Number - 1);
	}

	static void DoList(User *u, XLine *x, unsigned Number)
	{
		u->SendMessage(OperServ, OPER_LIST_FORMAT, Number + 1, x->Mask.c_str(), x->Reason.c_str());
	}
};

class SQLineViewCallback : public SQLineListCallback
{
 public:
	SQLineViewCallback(User *_u, const Anope::string &numlist) : SQLineListCallback(_u, numlist)
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
			u->SendMessage(OperServ, OPER_SQLINE_VIEW_HEADER);
		}

		DoList(u, x, Number);
	}

	static void DoList(User *u, XLine *x, unsigned Number)
	{
		Anope::string expirebuf = expire_left(u->Account(), x->Expires);
		u->SendMessage(OperServ, OPER_VIEW_FORMAT, Number + 1, x->Mask.c_str(), x->By.c_str(), do_strftime(x->Created).c_str(), expirebuf.c_str(), x->Reason.c_str());
	}
};

class CommandOSSQLine : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<Anope::string> &params)
	{
		unsigned last_param = 2;
		Anope::string expiry, mask;
		time_t expires;

		mask = params.size() > 1 ? params[1] : "";
		if (!mask.empty() && mask[0] == '+')
		{
			expiry = mask;
			mask = params.size() > 2 ? params[2] : "";
			last_param = 3;
		}

		expires = !expiry.empty() ? dotime(expiry) : Config->SQLineExpiry;
		/* If the expiry given does not contain a final letter, it's in days,
		 * said the doc. Ah well.
		 */
		if (!expiry.empty() && isdigit(expiry[expiry.length() - 1]))
			expires *= 86400;
		/* Do not allow less than a minute expiry time */
		if (expires && expires < 60)
		{
			u->SendMessage(OperServ, BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += Anope::CurTime;

		if (params.size() <= last_param)
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}

		Anope::string reason = params[last_param];
		if (last_param == 2 && params.size() > 3)
			reason += " " + params[3];
		if (!mask.empty() && !reason.empty())
		{
			XLine *x = SQLine->Add(OperServ, u, mask, expires, reason);

			if (!x)
				return MOD_CONT;

			u->SendMessage(OperServ, OPER_SQLINE_ADDED, mask.c_str());

			if (Config->WallOSSQLine)
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

				ircdproto->SendGlobops(OperServ, "%s added an SQLINE for %s (%s)", u->nick.c_str(), mask.c_str(), buf.c_str());
			}

			if (readonly)
				u->SendMessage(OperServ, READ_ONLY_MODE);

		}
		else
			this->OnSyntaxError(u, "ADD");

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, const std::vector<Anope::string> &params)
	{
		if (SQLine->GetList().empty())
		{
			u->SendMessage(OperServ, OPER_SQLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SQLineDelCallback list(u, mask);
			list.Process();
		}
		else
		{
			XLine *x = SQLine->HasEntry(mask);

			if (!x)
			{
				u->SendMessage(OperServ, OPER_SQLINE_NOT_FOUND, mask.c_str());
				return MOD_CONT;
			}

			FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, x, X_SQLINE));

			SQLineDelCallback::DoDel(u, x);
			u->SendMessage(OperServ, OPER_SQLINE_DELETED, mask.c_str());
		}

		if (readonly)
			u->SendMessage(OperServ, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, const std::vector<Anope::string> &params)
	{
		if (SQLine->GetList().empty())
		{
			u->SendMessage(OperServ, OPER_SQLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SQLineListCallback list(u, mask);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = SQLine->GetCount(); i < end; ++i)
			{
				XLine *x = SQLine->GetEntry(i);

				if (mask.empty() || mask.equals_ci(x->Mask) || Anope::Match(x->Mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						u->SendMessage(OperServ, OPER_SQLINE_LIST_HEADER);
					}

					SQLineListCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				u->SendMessage(OperServ, OPER_SQLINE_NO_MATCH);
			else
				u->SendMessage(OperServ, END_OF_ANY_LIST, "SQLine");
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, const std::vector<Anope::string> &params)
	{
		if (SQLine->GetList().empty())
		{
			u->SendMessage(OperServ, OPER_SQLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SQLineViewCallback list(u, mask);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = SQLine->GetCount(); i < end; ++i)
			{
				XLine *x = SQLine->GetEntry(i);

				if (mask.empty() || mask.equals_ci(x->Mask) || Anope::Match(x->Mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						u->SendMessage(OperServ, OPER_SQLINE_VIEW_HEADER);
					}

					SQLineViewCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				u->SendMessage(OperServ, OPER_SQLINE_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u)
	{
		FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, NULL, X_SQLINE));
		SGLine->Clear();
		u->SendMessage(OperServ, OPER_SQLINE_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSSQLine() : Command("SQLINE", 1, 4, "operserv/sqline")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string cmd = params[0];

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(u, params);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(u, params);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(u, params);
		else if (cmd.equals_ci("VIEW"))
			return this->DoView(u, params);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(u);
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(OperServ, OPER_HELP_SQLINE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(OperServ, u, "SQLINE", OPER_SQLINE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(OperServ, OPER_HELP_CMD_SQLINE);
	}
};

class OSSQLine : public Module
{
	CommandOSSQLine commandossqline;

 public:
	OSSQLine(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		if (!ircd->sqline)
			throw ModuleException("Your IRCd does not support QLines.");

		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandossqline);
	}
};

MODULE_INIT(OSSQLine)
