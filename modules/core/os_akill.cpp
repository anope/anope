/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
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
	CommandSource &source;
	unsigned Deleted;
 public:
	AkillDelCallback(CommandSource &_source, const Anope::string &numlist) : NumberList(numlist, true), source(_source), Deleted(0)
	{
	}

	~AkillDelCallback()
	{
		if (!Deleted)
			source.Reply(OPER_AKILL_NO_MATCH);
		else if (Deleted == 1)
			source.Reply(OPER_AKILL_DELETED_ONE);
		else
			source.Reply(OPER_AKILL_DELETED_SEVERAL, Deleted);
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = SGLine->GetEntry(Number - 1);

		if (!x)
			return;

		++Deleted;
		DoDel(source, x);
	}

	static void DoDel(CommandSource &source, XLine *x)
	{
		SGLine->DelXLine(x);
	}
};

class AkillListCallback : public NumberList
{
 protected:
	CommandSource &source;
	bool SentHeader;
 public:
	AkillListCallback(CommandSource &_source, const Anope::string &numlist) : NumberList(numlist, false), source(_source), SentHeader(false)
	{
	}

	~AkillListCallback()
	{
		if (!SentHeader)
			source.Reply(OPER_AKILL_NO_MATCH);
		else
			source.Reply(END_OF_ANY_LIST, "Akill");
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = SGLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(OPER_AKILL_LIST_HEADER);
		}

		DoList(source, x, Number);
	}

	static void DoList(CommandSource &source, XLine *x, unsigned Number)
	{
		source.Reply(OPER_LIST_FORMAT, Number + 1, x->Mask.c_str(), x->Reason.c_str());
	}
};

class AkillViewCallback : public AkillListCallback
{
 public:
	AkillViewCallback(CommandSource &_source, const Anope::string &numlist) : AkillListCallback(_source, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = SGLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(OPER_AKILL_VIEW_HEADER);
		}

		DoList(source, x, Number);
	}

	static void DoList(CommandSource &source, XLine *x, unsigned Number)
	{
		source.Reply(OPER_VIEW_FORMAT, Number + 1, x->Mask.c_str(), x->By.c_str(), do_strftime(x->Created).c_str(), expire_left(source.u->Account(), x->Expires).c_str(), x->Reason.c_str());
	}
};

class CommandOSAKill : public Command
{
 private:
	CommandReturn DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
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
			source.Reply(BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += Anope::CurTime;

		if (params.size() <= last_param)
		{
			this->OnSyntaxError(source, "ADD");
			return MOD_CONT;
		}

		Anope::string reason = params[last_param];
		if (last_param == 2 && params.size() > 3)
			reason += " " + params[3];
		if (!mask.empty() && !reason.empty())
		{
			User *user = finduser(mask);
			if (user)
				mask = "*@" + user->host;
			unsigned int affected = 0;
			for (patricia_tree<User *, ci::ci_char_traits>::iterator it(UserListByNick); it.next();)
				if (Anope::Match((*it)->GetIdent() + "@" + (*it)->host, mask))
					++affected;
			float percent = static_cast<float>(affected) / static_cast<float>(UserListByNick.size()) * 100.0;

			if (percent > 95)
			{
				source.Reply(USERHOST_MASK_TOO_WIDE, mask.c_str());
				Log(LOG_ADMIN, u, this) << "tried to akill " << percent << "% of the network (" << affected << " users)";
				return MOD_CONT;
			}

			XLine *x = SGLine->Add(OperServ, u, mask, expires, reason);

			if (!x)
				return MOD_CONT;

			source.Reply(OPER_AKILL_ADDED, mask.c_str());

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

				ircdproto->SendGlobops(OperServ, "%s added an AKILL for %s (%s) (%s) [affects %i user(s) (%.2f%%)]", u->nick.c_str(), mask.c_str(), reason.c_str(), buf.c_str(), affected, percent);
			}

			if (readonly)
				source.Reply(READ_ONLY_MODE);
		}
		else
			this->OnSyntaxError(source, "ADD");

		return MOD_CONT;
	}

	CommandReturn DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return MOD_CONT;
		}

		if (SGLine->GetList().empty())
		{
			source.Reply(OPER_LIST_EMPTY);
			return MOD_CONT;
		}

		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkillDelCallback list(source, mask);
			list.Process();
		}
		else
		{
			XLine *x = SGLine->HasEntry(mask);

			if (!x)
			{
				source.Reply(OPER_AKILL_NOT_FOUND, mask.c_str());
				return MOD_CONT;
			}

			FOREACH_MOD(I_OnDelAkill, OnDelAkill(u, x));

			AkillDelCallback::DoDel(source, x);
			source.Reply(OPER_AKILL_DELETED, mask.c_str());
		}

		if (readonly)
			source.Reply(READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (SGLine->GetList().empty())
		{
			source.Reply(OPER_LIST_EMPTY);
			return MOD_CONT;
		}

		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkillListCallback list(source, mask);
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
						source.Reply(OPER_AKILL_LIST_HEADER);
					}

					AkillListCallback::DoList(source, x, i);
				}
			}

			if (!SentHeader)
				source.Reply(OPER_AKILL_NO_MATCH);
			else
				source.Reply(END_OF_ANY_LIST, "Akill");
		}

		return MOD_CONT;
	}

	CommandReturn DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (SGLine->GetList().empty())
		{
			source.Reply(OPER_LIST_EMPTY);
			return MOD_CONT;
		}

		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkillViewCallback list(source, mask);
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
						source.Reply(OPER_AKILL_VIEW_HEADER);
					}

					AkillViewCallback::DoList(source, x, i);
				}
			}

			if (!SentHeader)
				source.Reply(OPER_AKILL_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(CommandSource &source)
	{
		User *u = source.u;
		FOREACH_MOD(I_OnDelAkill, OnDelAkill(u, NULL));
		SGLine->Clear();
		source.Reply(OPER_AKILL_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSAKill() : Command("AKILL", 1, 4, "operserv/akill")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &cmd = params[0];

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, params);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, params);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, params);
		else if (cmd.equals_ci("VIEW"))
			return this->DoView(source, params);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(source);
		else
			this->OnSyntaxError(source, "");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(OPER_HELP_AKILL);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "AKILL", OPER_AKILL_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(OPER_HELP_CMD_AKILL);
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
