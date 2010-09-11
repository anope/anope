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
	AkillDelCallback(User *_u, const Anope::string &numlist) : NumberList(numlist, true), u(_u), Deleted(0)
	{
	}

	~AkillDelCallback()
	{
		if (!Deleted)
			notice_lang(Config->s_OperServ, u, OPER_AKILL_NO_MATCH);
		else if (Deleted == 1)
			notice_lang(Config->s_OperServ, u, OPER_AKILL_DELETED_ONE);
		else
			notice_lang(Config->s_OperServ, u, OPER_AKILL_DELETED_SEVERAL, Deleted);
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
	AkillListCallback(User *_u, const Anope::string &numlist) : NumberList(numlist, false), u(_u), SentHeader(false)
	{
	}

	~AkillListCallback()
	{
		if (!SentHeader)
			notice_lang(Config->s_OperServ, u, OPER_AKILL_NO_MATCH);
		else
			notice_lang(Config->s_OperServ, u, END_OF_ANY_LIST, "Akill");
	}

	void HandleNumber(unsigned Number)
	{
		XLine *x = SGLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config->s_OperServ, u, OPER_AKILL_LIST_HEADER);
		}

		DoList(u, x, Number);
	}

	static void DoList(User *u, XLine *x, unsigned Number)
	{
		notice_lang(Config->s_OperServ, u, OPER_AKILL_LIST_FORMAT, Number + 1, x->Mask.c_str(), x->Reason.c_str());
	}
};

class AkillViewCallback : public AkillListCallback
{
 public:
	AkillViewCallback(User *_u, const Anope::string &numlist) : AkillListCallback(_u, numlist)
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
			notice_lang(Config->s_OperServ, u, OPER_AKILL_VIEW_HEADER);
		}

		DoList(u, x, Number);
	}

	static void DoList(User *u, XLine *x, unsigned Number)
	{
		char timebuf[32];
		struct tm tm;

		tm = *localtime(&x->Created);
		strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT, &tm);
		Anope::string expirebuf = expire_left(u->Account(), x->Expires);

		notice_lang(Config->s_OperServ, u, OPER_AKILL_VIEW_FORMAT, Number + 1, x->Mask.c_str(), x->By.c_str(), timebuf, expirebuf.c_str(), x->Reason.c_str());
	}
};

class CommandOSAKill : public Command
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

		expires = !expiry.empty() ? dotime(expiry) : Config->AutokillExpiry;
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
			XLine *x = SGLine->Add(OperServ, u, mask, expires, reason);

			if (!x)
				return MOD_CONT;

			notice_lang(Config->s_OperServ, u, OPER_AKILL_ADDED, mask.c_str());

			if (Config->WallOSAkill)
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

				ircdproto->SendGlobops(OperServ, "%s added an AKILL for %s (%s) (%s)", u->nick.c_str(), mask.c_str(), reason.c_str(), buf.c_str());
			}

			if (readonly)
				notice_lang(Config->s_OperServ, u, READ_ONLY_MODE);
		}
		else
			this->OnSyntaxError(u, "ADD");

		return MOD_CONT;
	}

	CommandReturn DoDel(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(u, "DEL");
			return MOD_CONT;
		}

		if (SGLine->GetList().empty())
		{
			notice_lang(Config->s_OperServ, u, OPER_AKILL_LIST_EMPTY);
			return MOD_CONT;
		}

		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkillDelCallback list(u, mask);
			list.Process();
		}
		else
		{
			XLine *x = SGLine->HasEntry(mask);

			if (!x)
			{
				notice_lang(Config->s_OperServ, u, OPER_AKILL_NOT_FOUND, mask.c_str());
				return MOD_CONT;
			}

			FOREACH_MOD(I_OnDelAkill, OnDelAkill(u, x));

			AkillDelCallback::DoDel(u, x);
			notice_lang(Config->s_OperServ, u, OPER_AKILL_DELETED, mask.c_str());
		}

		if (readonly)
			notice_lang(Config->s_OperServ, u, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, const std::vector<Anope::string> &params)
	{
		if (SGLine->GetList().empty())
		{
			notice_lang(Config->s_OperServ, u, OPER_AKILL_LIST_EMPTY);
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkillListCallback list(u, mask);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = SGLine->GetCount(); i < end; ++i)
			{
				XLine *x = SGLine->GetEntry(i);

				if (mask.empty() || mask.equals_ci(x->Mask) || Anope::Match(x->Mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						notice_lang(Config->s_OperServ, u, OPER_AKILL_LIST_HEADER);
					}

					AkillListCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				notice_lang(Config->s_OperServ, u, OPER_AKILL_NO_MATCH);
			else
				notice_lang(Config->s_OperServ, u, END_OF_ANY_LIST, "Akill");
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, const std::vector<Anope::string> &params)
	{
		if (SGLine->GetList().empty())
		{
			notice_lang(Config->s_OperServ, u, OPER_AKILL_LIST_EMPTY);
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkillViewCallback list(u, mask);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = SGLine->GetCount(); i < end; ++i)
			{
				XLine *x = SGLine->GetEntry(i);

				if (mask.empty() || mask.equals_ci(x->Mask) || Anope::Match(x->Mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						notice_lang(Config->s_OperServ, u, OPER_AKILL_VIEW_HEADER);
					}

					AkillViewCallback::DoList(u, x, i);
				}
			}

			if (!SentHeader)
				notice_lang(Config->s_OperServ, u, OPER_AKILL_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u)
	{
		FOREACH_MOD(I_OnDelAkill, OnDelAkill(u, NULL));
		SGLine->Clear();
		notice_lang(Config->s_OperServ, u, OPER_AKILL_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSAKill() : Command("AKILL", 1, 4, "operserv/akill")
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
		notice_help(Config->s_OperServ, u, OPER_HELP_AKILL);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_OperServ, u, "AKILL", OPER_AKILL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_OperServ, u, OPER_HELP_CMD_AKILL);
	}
};

class OSAKill : public Module
{
	CommandOSAKill commandosakill;

 public:
	OSAKill(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosakill);
	}
};

MODULE_INIT(OSAKill)
