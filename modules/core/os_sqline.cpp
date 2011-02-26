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

class SQLineDelCallback : public NumberList
{
	CommandSource &source;
	unsigned Deleted;
 public:
	SQLineDelCallback(CommandSource &_source, const Anope::string &numlist) : NumberList(numlist, true), source(_source), Deleted(0)
	{
	}

	~SQLineDelCallback()
	{
		if (!Deleted)
			source.Reply(_("No matching entries on the SQLINE list."));
		else if (Deleted == 1)
			source.Reply(_("Deleted 1 entry from the SQLINE list."));
		else
			source.Reply(_("Deleted %d entries from the SQLINE list."), Deleted);
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = SQLine->GetEntry(Number - 1);

		if (!x)
			return;

		++Deleted;
		DoDel(source, x - 1);
	}

	static void DoDel(CommandSource &source, XLine *x)
	{
		SQLine->DelXLine(x);
	}
};

class SQLineListCallback : public NumberList
{
 protected:
	CommandSource &source;
	bool SentHeader;
 public:
	SQLineListCallback(CommandSource &_source, const Anope::string &numlist) : NumberList(numlist, false), source(_source), SentHeader(false)
	{
	}

	~SQLineListCallback()
	{
		if (!SentHeader)
			source.Reply(_("No matching entries on the SQLINE list."));
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = SQLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Current SQLINE list:\n  Num   Mask                              Reason"));
		}

		DoList(source, x, Number - 1);
	}

	static void DoList(CommandSource &source, XLine *x, unsigned Number)
	{
		source.Reply(_(OPER_LIST_FORMAT), Number + 1, x->Mask.c_str(), x->Reason.c_str());
	}
};

class SQLineViewCallback : public SQLineListCallback
{
 public:
	SQLineViewCallback(CommandSource &_source, const Anope::string &numlist) : SQLineListCallback(_source, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = SQLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Current SQLINE list:"));
		}

		DoList(source, x, Number);
	}

	static void DoList(CommandSource &source, XLine *x, unsigned Number)
	{
		Anope::string expirebuf = expire_left(source.u->Account(), x->Expires);
		source.Reply(_(OPER_VIEW_FORMAT), Number + 1, x->Mask.c_str(), x->By.c_str(), do_strftime(x->Created).c_str(), expirebuf.c_str(), x->Reason.c_str());
	}
};

class CommandOSSQLine : public Command
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

		expires = !expiry.empty() ? dotime(expiry) : Config->SQLineExpiry;
		/* If the expiry given does not contain a final letter, it's in days,
		 * said the doc. Ah well.
		 */
		if (!expiry.empty() && isdigit(expiry[expiry.length() - 1]))
			expires *= 86400;
		/* Do not allow less than a minute expiry time */
		if (expires && expires < 60)
		{
			source.Reply(_(BAD_EXPIRY_TIME));
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
			unsigned int affected = 0;
			for (patricia_tree<User *, ci::ci_char_traits>::iterator it(UserListByNick); it.next();)
				if (Anope::Match((*it)->nick, mask))
					++affected;
			float percent = static_cast<float>(affected) / static_cast<float>(UserListByNick.size()) * 100.0;

			if (percent > 95)
			{
				source.Reply(_(USERHOST_MASK_TOO_WIDE), mask.c_str());
				Log(LOG_ADMIN, u, this) << "tried to SQLine " << percent << "% of the network (" << affected << " users)";
				return MOD_CONT;
			}
			XLine *x = SQLine->Add(OperServ, u, mask, expires, reason);

			if (!x)
				return MOD_CONT;

			source.Reply(_("\002%s\002 added to the SQLINE list."), mask.c_str());
			Log(LOG_ADMIN, u, this) << "on " << mask << " (" << reason << ") expires in " << duration(NULL, expires - Anope::CurTime) << " [affects " << affected << " user(s) (" << percent << "%)]";

			if (readonly)
				source.Reply(_(READ_ONLY_MODE));

		}
		else
			this->OnSyntaxError(source, "ADD");

		return MOD_CONT;
	}

	CommandReturn DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		if (SQLine->GetList().empty())
		{
			source.Reply(_("SQLINE list is empty."));
			return MOD_CONT;
		}

		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return MOD_CONT;
		}

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SQLineDelCallback list(source, mask);
			list.Process();
		}
		else
		{
			XLine *x = SQLine->HasEntry(mask);

			if (!x)
			{
				source.Reply(_("\002%s\002 not found on the SQLINE list."), mask.c_str());
				return MOD_CONT;
			}

			FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, x, X_SQLINE));

			SQLineDelCallback::DoDel(source, x);
			source.Reply(_("\002%s\002 deleted from the SQLINE list."), mask.c_str());
		}

		if (readonly)
			source.Reply(_(READ_ONLY_MODE));

		return MOD_CONT;
	}

	CommandReturn DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (SQLine->GetList().empty())
		{
			source.Reply(_("SQLINE list is empty."));
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SQLineListCallback list(source, mask);
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
						source.Reply(_("Current SQLINE list:\n  Num   Mask                              Reason"));
					}

					SQLineListCallback::DoList(source, x, i);
				}
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on the SQLINE list."));
			else
				source.Reply(_(END_OF_ANY_LIST), "SQLine");
		}

		return MOD_CONT;
	}

	CommandReturn DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (SQLine->GetList().empty())
		{
			source.Reply(_("SQLINE list is empty."));
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SQLineViewCallback list(source, mask);
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
						source.Reply(_("Current SQLINE list:"));
					}

					SQLineViewCallback::DoList(source, x, i);
				}
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on the SQLINE list."));
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(CommandSource &source)
	{
		User *u = source.u;
		FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, NULL, X_SQLINE));
		SQLine->Clear();
		source.Reply(_("The SQLINE list has been cleared."));

		return MOD_CONT;
	}
 public:
	CommandOSSQLine() : Command("SQLINE", 1, 4, "operserv/sqline")
	{
		this->SetDesc("Manipulate the SQLINE list");
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
		source.Reply(_("Syntax: \002SQLINE ADD [+\037expiry\037] \037mask\037 \037reason\037\002\n"
				"        \002SQLINE DEL {\037mask\037 | \037entry-num\037 | \037list\037}\002\n"
				"        \002SQLINE LIST [\037mask\037 | \037list\037]\002\n"
				"        \002SQLINE VIEW [\037mask\037 | \037list\037]\002\n"
				"        \002SQLINE CLEAR\002\n"
				" \n"
				"Allows Services operators to manipulate the SQLINE list.  If\n"
				"a user with a nick matching an SQLINE mask attempts to \n"
				"connect, Services will not allow it to pursue his IRC\n"
				"session.\n"
				"If the first character of the mask is #, services will \n"
				"prevent the use of matching channels (on IRCds that \n"
				"support it).\n"
				" \n"
				"\002SQLINE ADD\002 adds the given (nick's) mask to the SQLINE\n"
				"list for the given reason (which \002must\002 be given).\n"
				"\037expiry\037 is specified as an integer followed by one of \037d\037 \n"
				"(days), \037h\037 (hours), or \037m\037 (minutes). Combinations (such as \n"
				"\0371h30m\037) are not permitted. If a unit specifier is not \n"
				"included, the default is days (so \037+30\037 by itself means 30 \n"
				"days). To add an SQLINE which does not expire, use \037+0\037. \n"
				"If the mask to be added starts with a \037+\037, an expiry time \n"
				"must be given, even if it is the same as the default. The\n"
				"current SQLINE default expiry time can be found with the\n"
				"\002STATS AKILL\002 command.\n"
				" \n"
				"The \002SQLINE DEL\002 command removes the given mask from the\n"
				"SQLINE list if it is present. If a list of entry numbers is \n"
				"given, those entries are deleted. (See the example for LIST \n"
				"below.)\n"
				" \n"
				"The \002SQLINE LIST\002 command displays the SQLINE list. \n"
				"If a wildcard mask is given, only those entries matching the\n"
				"mask are displayed. If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002SQLINE LIST 2-5,7-9\002\n"
				"      Lists SQLINE entries numbered 2 through 5 and 7 \n"
				"      through 9.\n"
				" \n"
				"\002SQLINE VIEW\002 is a more verbose version of \002SQLINE LIST\002, and \n"
				"will show who added an SQLINE, the date it was added, and when \n"
				"it expires, as well as the mask and reason.\n"
				" \n"
				"\002SQLINE CLEAR\002 clears all entries of the SQLINE list."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SQLINE", _("SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+\037expiry\037] {\037nick\037 | \037mask\037 | \037entry-list\037} [\037reason\037]]"));
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
