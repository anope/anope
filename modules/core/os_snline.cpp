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
#include "operserv.h"

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
			source.Reply(_("No matching entries on the SNLINE list."));
		else if (Deleted == 1)
			source.Reply(_("Deleted 1 entry from the SNLINE list."));
		else
			source.Reply(_("Deleted %d entries from the SNLINE list."), Deleted);
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
			source.Reply(_("No matching entries on the SNLINE list."));
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
			source.Reply(_("Current SNLINE list:\n  Num   Mask                              Reason"));
		}

		DoList(source, x, Number - 1);
	}

	static void DoList(CommandSource &source, XLine *x, unsigned Number)
	{
		source.Reply(_(OPER_LIST_FORMAT), Number + 1, x->Mask.c_str(), x->Reason.c_str());
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
			source.Reply(_("Current SNLINE list:"));
		}

		DoList(source, x, Number - 1);
	}

	static void DoList(CommandSource &source, XLine *x, unsigned Number)
	{
		Anope::string expirebuf = expire_left(source.u->Account(), x->Expires);
		source.Reply(_(OPER_VIEW_FORMAT), Number + 1, x->Mask.c_str(), x->By.c_str(), do_strftime(x->Created).c_str(), expirebuf.c_str(), x->Reason.c_str());
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
			source.Reply(_(BAD_EXPIRY_TIME));
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
			std::pair<int, XLine *> canAdd = SNLine->CanAdd(mask, expires);
			if (mask.find_first_not_of("*?") == Anope::string::npos)
				source.Reply(_(USERHOST_MASK_TOO_WIDE), mask.c_str());
			else if (canAdd.first == 1)
				source.Reply(_("\002%s\002 already exists on the SNLINE list."), canAdd.second->Mask.c_str());
			else if (canAdd.first == 2)
				source.Reply(_("Expiry time of \002%s\002 changed."), canAdd.second->Mask.c_str());
			else if (canAdd.first == 3)
				source.Reply(_("\002%s\002 is already covered by %s."), mask.c_str(), canAdd.second->Mask.c_str());
			else
			{
				/* Clean up the last character of the mask if it is a space
				 * See bug #761
				 */
				unsigned masklen = mask.length();
				if (mask[masklen - 1] == ' ')
					mask.erase(masklen - 1);
				unsigned int affected = 0;
				for (Anope::insensitive_map<User *>::iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
					if (Anope::Match(it->second->realname, mask))
						++affected;
				float percent = static_cast<float>(affected) / static_cast<float>(UserListByNick.size()) * 100.0;

				if (percent > 95)
				{
					source.Reply(_(USERHOST_MASK_TOO_WIDE), mask.c_str());
					Log(LOG_ADMIN, u, this) << "tried to SNLine " << percent << "% of the network (" << affected << " users)";
					return MOD_CONT;
				}

				XLine *x = SNLine->Add(mask, u->nick, expires, reason);

				if (!x)
					return MOD_CONT;

				source.Reply(_("\002%s\002 added to the SNLINE list."), mask.c_str());
				Log(LOG_ADMIN, u, this) << "on " << mask << " (" << reason << ") expires in " << (expires ? duration(expires - Anope::CurTime) : "never") << " [affects " << affected << " user(s) (" << percent << "%)]";

				if (readonly)
					source.Reply(_(READ_ONLY_MODE));
			}

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
			source.Reply(_("SNLINE list is empty."));
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
				source.Reply(_("\002%s\002 not found on the SNLINE list."), mask.c_str());
				return MOD_CONT;
			}

			FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, x, X_SNLINE));

			SNLineDelCallback::DoDel(source, x);
			source.Reply(_("\002%s\002 deleted from the SNLINE list."), mask.c_str());
		}

		if (readonly)
			source.Reply(_(READ_ONLY_MODE));

		return MOD_CONT;
	}

	CommandReturn OnList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (SNLine->GetList().empty())
		{
			source.Reply(_("SNLINE list is empty."));
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
						source.Reply(_("Current SNLINE list:\n  Num   Mask                              Reason"));
					}

					SNLineListCallback::DoList(source, x, i);
				}
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on the SNLINE list."));
			else
				source.Reply(_(END_OF_ANY_LIST), "SNLine");
		}

		return MOD_CONT;
	}

	CommandReturn OnView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (SNLine->GetList().empty())
		{
			source.Reply(_("SNLINE list is empty."));
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
						source.Reply(_("Current SNLINE list:"));
					}

					SNLineViewCallback::DoList(source, x, i);
				}
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on the SNLINE list."));
		}

		return MOD_CONT;
	}

	CommandReturn OnClear(CommandSource &source)
	{
		User *u = source.u;
		FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, NULL, X_SNLINE));
		SNLine->Clear();
		source.Reply(_("The SNLINE list has been cleared."));

		return MOD_CONT;
	}
 public:
	CommandOSSNLine() : Command("SNLINE", 1, 3, "operserv/snline")
	{
		this->SetDesc(_("Manipulate the SNLINE list"));
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
		source.Reply(_("Syntax: \002SNLINE ADD [+\037expiry\037] \037mask\037:\037reason\037\002\n"
				"        \002SNLINE DEL {\037mask\037 | \037entry-num\037 | \037list\037}\002\n"
				"        \002SNLINE LIST [\037mask\037 | \037list\037]\002\n"
				"        \002SNLINE VIEW [\037mask\037 | \037list\037]\002\n"
				"        \002SNLINE CLEAR\002\n"
				" \n"
				"Allows Services operators to manipulate the SNLINE list.  If\n"
				"a user with a realname matching an SNLINE mask attempts to \n"
				"connect, Services will not allow it to pursue his IRC\n"
				"session.\n"
				" \n"
				"\002SNLINE ADD\002 adds the given realname mask to the SNLINE\n"
				"list for the given reason (which \002must\002 be given).\n"
				"\037expiry\037 is specified as an integer followed by one of \037d\037 \n"
				"(days), \037h\037 (hours), or \037m\037 (minutes).  Combinations (such as \n"
				"\0371h30m\037) are not permitted.  If a unit specifier is not \n"
				"included, the default is days (so \037+30\037 by itself means 30 \n"
				"days).  To add an SNLINE which does not expire, use \037+0\037.  If the\n"
				"realname mask to be added starts with a \037+\037, an expiry time must\n"
				"be given, even if it is the same as the default.  The\n"
				"current SNLINE default expiry time can be found with the\n"
				"\002STATS AKILL\002 command.\n"
				"Note: because the realname mask may contain spaces, the\n"
				"separator between it and the reason is a colon.\n"
				" \n"
				"The \002SNLINE DEL\002 command removes the given mask from the\n"
				"SNLINE list if it is present.  If a list of entry numbers is \n"
				"given, those entries are deleted.  (See the example for LIST \n"
				"below.)\n"
				" \n"
				"The \002SNLINE LIST\002 command displays the SNLINE list.  \n"
				"If a wildcard mask is given, only those entries matching the\n"
				"mask are displayed.  If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002SNLINE LIST 2-5,7-9\002\n"
				"      Lists SNLINE entries numbered 2 through 5 and 7 \n"
				"      through 9.\n"
				" \n"
				"\002SNLINE VIEW\002 is a more verbose version of \002SNLINE LIST\002, and \n"
				"will show who added an SNLINE, the date it was added, and when \n"
				"it expires, as well as the realname mask and reason.\n"
				" \n"
				"\002SNLINE CLEAR\002 clears all entries of the SNLINE list."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SNLINE", _("SNLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+\037expiry\037] {\037mask\037 | \037entry-list\037}[:\037reason\037]]"));
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

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(operserv->Bot(), &commandossnline);
	}
};

MODULE_INIT(OSSNLine)
