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
#include "hashcomp.h"

class SNLineDelCallback : public NumberList
{
	CommandSource &source;
	unsigned Deleted;
 public:
	SNLineDelCallback(CommandSource &_source, const Anope::string &numlist) : NumberList(numlist, true), source(_source), Deleted(0)
	{
	}

	~SNLineDelCallback()
	{
		if (!Deleted)
			source.Reply(OPER_SNLINE_NO_MATCH);
		else if (Deleted == 1)
			source.Reply(OPER_SNLINE_DELETED_ONE);
		else
			source.Reply(OPER_SNLINE_DELETED_SEVERAL, Deleted);
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = SNLine->GetEntry(Number - 1);

		if (!x)
			return;

		++Deleted;
		DoDel(source, x);
	}

	static void DoDel(CommandSource &source, XLine *x)
	{
		SNLine->DelXLine(x);
	}
};

class SNLineListCallback : public NumberList
{
 protected:
	CommandSource &source;
	bool SentHeader;
 public:
	SNLineListCallback(CommandSource &_source, const Anope::string &numlist) : NumberList(numlist, false), source(_source), SentHeader(false)
	{
	}

	~SNLineListCallback()
	{
		if (!SentHeader)
			source.Reply(OPER_SNLINE_NO_MATCH);
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = SNLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(OPER_SNLINE_LIST_HEADER);
		}

		DoList(source, x, Number - 1);
	}

	static void DoList(CommandSource &source, XLine *x, unsigned Number)
	{
		source.Reply(OPER_LIST_FORMAT, Number + 1, x->Mask.c_str(), x->Reason.c_str());
	}
};

class SNLineViewCallback : public SNLineListCallback
{
 public:
	SNLineViewCallback(CommandSource &_source, const Anope::string &numlist) : SNLineListCallback(_source, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = SNLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(OPER_SNLINE_VIEW_HEADER);
		}

		DoList(source, x, Number - 1);
	}

	static void DoList(CommandSource &source, XLine *x, unsigned Number)
	{
		Anope::string expirebuf = expire_left(source.u->Account(), x->Expires);
		source.Reply(OPER_VIEW_FORMAT, Number + 1, x->Mask.c_str(), x->By.c_str(), do_strftime(x->Created).c_str(), expirebuf.c_str(), x->Reason.c_str());
	}
};

class CommandOSSNLine : public Command
{
 private:
	CommandReturn OnAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
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
			source.Reply(BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += Anope::CurTime;

		if (param.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return MOD_CONT;
		}

		Anope::string rest = param;
		if (params.size() > last_param)
			rest += " " + params[last_param];

		if (rest.find(':') == Anope::string::npos)
		{
			this->OnSyntaxError(source, "ADD");
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
			unsigned int affected = 0;
			for (patricia_tree<User *, ci::ci_char_traits>::iterator it(UserListByNick); it.next();)
				if (Anope::Match((*it)->realname, mask))
					++affected;
			float percent = static_cast<float>(affected) / static_cast<float>(UserListByNick.size()) * 100.0;

			if (percent > 95)
			{
				source.Reply(USERHOST_MASK_TOO_WIDE, mask.c_str());
				Log(LOG_ADMIN, u, this) << "tried to SNLine " << percent << "% of the network (" << affected << " users)";
				return MOD_CONT;
			}

			XLine *x = SNLine->Add(OperServ, u, mask, expires, reason);

			if (!x)
				return MOD_CONT;

			source.Reply(OPER_SNLINE_ADDED, mask.c_str());

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

				ircdproto->SendGlobops(findbot(Config->s_OperServ), "%s added an SNLINE for %s (%s) [affects %i user(s) (%.2f%%)]", u->nick.c_str(), mask.c_str(), buf.c_str(), affected, percent);
			}

			if (readonly)
				source.Reply(READ_ONLY_MODE);

		}
		else
			this->OnSyntaxError(source, "ADD");

		return MOD_CONT;
	}

	CommandReturn OnDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		if (SNLine->GetList().empty())
		{
			source.Reply(OPER_SNLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return MOD_CONT;
		}

		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SNLineDelCallback list(source, mask);
			list.Process();
		}
		else
		{
			XLine *x = SNLine->HasEntry(mask);

			if (!x)
			{
				source.Reply(OPER_SNLINE_NOT_FOUND, mask.c_str());
				return MOD_CONT;
			}

			FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, x, X_SNLINE));

			SNLineDelCallback::DoDel(source, x);
			source.Reply(OPER_SNLINE_DELETED, mask.c_str());
		}

		if (readonly)
			source.Reply(READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn OnList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (SNLine->GetList().empty())
		{
			source.Reply(OPER_SNLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SNLineListCallback list(source, mask);
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
						source.Reply(OPER_SNLINE_LIST_HEADER);
					}

					SNLineListCallback::DoList(source, x, i);
				}
			}

			if (!SentHeader)
				source.Reply(OPER_SNLINE_NO_MATCH);
			else
				source.Reply(END_OF_ANY_LIST, "SNLine");
		}

		return MOD_CONT;
	}

	CommandReturn OnView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (SNLine->GetList().empty())
		{
			source.Reply(OPER_SNLINE_LIST_EMPTY);
			return MOD_CONT;
		}

		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SNLineViewCallback list(source, mask);
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
						source.Reply(OPER_SNLINE_VIEW_HEADER);
					}

					SNLineViewCallback::DoList(source, x, i);
				}
			}

			if (!SentHeader)
				source.Reply(OPER_SNLINE_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn OnClear(CommandSource &source)
	{
		User *u = source.u;
		FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, NULL, X_SNLINE));
		SNLine->Clear();
		source.Reply(OPER_SNLINE_CLEAR);

		return MOD_CONT;
	}
 public:
	CommandOSSNLine() : Command("SNLINE", 1, 3, "operserv/snline")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &cmd = params[0];

		if (cmd.equals_ci("ADD"))
			return this->OnAdd(source, params);
		else if (cmd.equals_ci("DEL"))
			return this->OnDel(source, params);
		else if (cmd.equals_ci("LIST"))
			return this->OnList(source, params);
		else if (cmd.equals_ci("VIEW"))
			return this->OnView(source, params);
		else if (cmd.equals_ci("CLEAR"))
			return this->OnClear(source);
		else
			this->OnSyntaxError(source, "");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(OPER_HELP_SNLINE);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SNLINE", OPER_SNLINE_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(OPER_HELP_CMD_SNLINE);
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
