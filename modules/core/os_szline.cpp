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

class SZLineDelCallback : public NumberList
{
	CommandSource &source;
	unsigned Deleted;
 public:
	SZLineDelCallback(CommandSource &_source, const Anope::string &numlist) : NumberList(numlist, true), source(_source), Deleted(0)
	{
	}

	~SZLineDelCallback()
	{
		if (!Deleted)
			source.Reply(_("No matching entries on the SZLINE list."));
		else if (Deleted == 1)
			source.Reply(_("Deleted 1 entry from the SZLINE list."));
		else
			source.Reply(_("Deleted %d entries from the SZLINE list."), Deleted);
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = SZLine->GetEntry(Number - 1);

		if (!x)
			return;

		++Deleted;
		DoDel(source, x);
	}

	static void DoDel(CommandSource &source, XLine *x)
	{
		SZLine->DelXLine(x);
	}
};

class SZLineListCallback : public NumberList
{
 protected:
	CommandSource &source;
	bool SentHeader;
 public:
	SZLineListCallback(CommandSource &_source, const Anope::string &numlist) : NumberList(numlist, false), source(_source), SentHeader(false)
	{
	}

	~SZLineListCallback()
	{
		if (!SentHeader)
			source.Reply(_("No matching entries on the SZLINE list."));
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = SZLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Current SZLINE list:\n  Num   Mask                              Reason"));
		}

		DoList(source, x, Number - 1);
	}

	static void DoList(CommandSource &source, XLine *x, unsigned Number)
	{
		source.Reply(_(OPER_LIST_FORMAT), Number + 1, x->Mask.c_str(), x->Reason.c_str());
	}
};

class SZLineViewCallback : public SZLineListCallback
{
 public:
	SZLineViewCallback(CommandSource &_source, const Anope::string &numlist) : SZLineListCallback(_source, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = SZLine->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Current SZLINE list:"));
		}

		DoList(source, x, Number - 1);
	}

	static void DoList(CommandSource &source, XLine *x, unsigned Number)
	{
		Anope::string expirebuf = expire_left(source.u->Account(), x->Expires);
		source.Reply(_(OPER_VIEW_FORMAT), Number + 1, x->Mask.c_str(), x->By.c_str(), do_strftime(x->Created).c_str(), expirebuf.c_str(), x->Reason.c_str());
	}
};

class CommandOSSZLine : public Command
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

		expires = !expiry.empty() ? dotime(expiry) : Config->SZLineExpiry;
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
			User *user = finduser(mask);
			if (user && user->ip())
				mask = user->ip.addr();
			unsigned int affected = 0;
			for (Anope::insensitive_map<User *>::iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
				if (it->second->ip() && Anope::Match(it->second->ip.addr(), mask))
					++affected;
			float percent = static_cast<float>(affected) / static_cast<float>(UserListByNick.size()) * 100.0;

			if (percent > 95)
			{
				source.Reply(_(USERHOST_MASK_TOO_WIDE), mask.c_str());
				Log(LOG_ADMIN, u, this) << "tried to SZLine " << percent << "% of the network (" << affected << " users)";
				return MOD_CONT;
			}

			XLine *x = SZLine->Add(OperServ, u, mask, expires, reason);

			if (!x)
				return MOD_CONT;

			source.Reply(_("\002%s\002 added to the SZLINE list."), mask.c_str());
			Log(LOG_ADMIN, u, this) << "on " << mask << " (" << reason << ") expires in " << (expires ? duration(expires - Anope::CurTime) : "never") << " [affects " << affected << " user(s) (" << percent << "%)]";

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

		if (SZLine->GetList().empty())
		{
			source.Reply(_("SZLINE list is empty."));
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return MOD_CONT;
		}

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SZLineDelCallback list(source, mask);
			list.Process();
		}
		else
		{
			XLine *x = SZLine->HasEntry(mask);

			if (!x)
			{
				source.Reply(_("\002%s\002 not found on the SZLINE list."), mask.c_str());
				return MOD_CONT;
			}

			FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, x, X_SZLINE));

			SZLineDelCallback::DoDel(source, x);
			source.Reply(_("\002%s\002 deleted from the SZLINE list."), mask.c_str());
		}

		if (readonly)
			source.Reply(_(READ_ONLY_MODE));

		return MOD_CONT;
	}

	CommandReturn DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (SZLine->GetList().empty())
		{
			source.Reply(_("SZLINE list is empty."));
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SZLineListCallback list(source, mask);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = SZLine->GetCount(); i < end; ++i)
			{
				XLine *x = SZLine->GetEntry(i);

				if (mask.empty() || mask.equals_ci(x->Mask) || Anope::Match(x->Mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						source.Reply(_("Current SZLINE list:\n  Num   Mask                              Reason"));
					}

					SZLineListCallback::DoList(source, x, i);
				}
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on the SZLINE list."));
		}

		return MOD_CONT;
	}

	CommandReturn DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (SZLine->GetList().empty())
		{
			source.Reply(_("SZLINE list is empty."));
			return MOD_CONT;
		}

		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			SZLineViewCallback list(source, mask);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = SZLine->GetCount(); i < end; ++i)
			{
				XLine *x = SZLine->GetEntry(i);

				if (mask.empty() || mask.equals_ci(x->Mask) || Anope::Match(x->Mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						source.Reply(_("Current SZLINE list:"));
					}

					SZLineViewCallback::DoList(source, x, i);
				}
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on the SZLINE list."));
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(CommandSource &source)
	{
		User *u = source.u;
		FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, NULL, X_SZLINE));
		SZLine->Clear();
		source.Reply(_("The SZLINE list has been cleared."));

		return MOD_CONT;
	}
 public:
	CommandOSSZLine() : Command("SZLINE", 1, 4, "operserv/szline")
	{
		this->SetDesc(_("Manipulate the SZLINE list"));
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
		source.Reply(_("Syntax: \002SZLINE ADD [+\037expiry\037] \037mask\037 \037reason\037\002\n"
				"        \002SZLINE DEL {\037mask\037 | \037entry-num\037 | \037list\037}\002\n"
				"        \002SZLINE LIST [\037mask\037 | \037list\037]\002\n"
				"        \002SZLINE VIEW [\037mask\037 | \037list\037]\002\n"
				"        \002SZLINE CLEAR\002\n"
				" \n"
				"Allows Services operators to manipulate the SZLINE list.  If\n"
				"a user with an IP matching an SZLINE mask attempts to \n"
				"connect, Services will not allow it to pursue his IRC\n"
				"session (and this, whether the IP has a PTR RR or not).\n"
				" \n"
				"\002SZLINE ADD\002 adds the given (nick's) IP mask to the SZLINE\n"
				"list for the given reason (which \002must\002 be given).\n"
				"\037expiry\037 is specified as an integer followed by one of \037d\037 \n"
				"(days), \037h\037 (hours), or \037m\037 (minutes). Combinations (such as \n"
				"\0371h30m\037) are not permitted. If a unit specifier is not \n"
				"included, the default is days (so \037+30\037 by itself means 30 \n"
				"days). To add an SZLINE which does not expire, use \037+0\037. If the\n"
				"realname mask to be added starts with a \037+\037, an expiry time must\n"
				"be given, even if it is the same as the default. The\n"
				"current SZLINE default expiry time can be found with the\n"
				"\002STATS AKILL\002 command.\n"
				" \n"
				"The \002SZLINE DEL\002 command removes the given mask from the\n"
				"SZLINE list if it is present. If a list of entry numbers is \n"
				"given, those entries are deleted. (See the example for LIST \n"
				"below.)\n"
				" \n"
				"The \002SZLINE LIST\002 command displays the SZLINE list.\n"
				"If a wildcard mask is given, only those entries matching the\n"
				"mask are displayed. If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002SZLINE LIST 2-5,7-9\002\n"
				"      Lists SZLINE entries numbered 2 through 5 and 7 \n"
				"      through 9.\n"
				" \n"
				"\002SZLINE VIEW\002 is a more verbose version of \002SZLINE LIST\002, and \n"
				"will show who added an SZLINE, the date it was added, and when\n"
				"it expires, as well as the IP mask and reason.\n"
				" \n"
				"\002SZLINE CLEAR\002 clears all entries of the SZLINE list."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SZLINE", _("SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+\037expiry\037] {\037nick\037 | \037mask\037 | \037entry-list\037} [\037reason\037]]"));
	}
};

class OSSZLine : public Module
{
	CommandOSSZLine commandosszline;

 public:
	OSSZLine(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		if (!ircd->szline)
			throw ModuleException("Your IRCd does not support ZLINEs");

		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosszline);
	}
};

MODULE_INIT(OSSZLine)
