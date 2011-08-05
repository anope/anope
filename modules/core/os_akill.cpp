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

static service_reference<XLineManager> akills("xlinemanager/sgline");

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
			source.Reply(_("No matching entries on the AKILL list."));
		else if (Deleted == 1)
			source.Reply(_("Deleted 1 entry from the AKILL list."));
		else
			source.Reply(_("Deleted %d entries from the AKILL list."), Deleted);
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = akills->GetEntry(Number - 1);

		if (!x)
			return;

		++Deleted;
		DoDel(source, x);
	}

	static void DoDel(CommandSource &source, XLine *x)
	{
		akills->DelXLine(x);
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
			source.Reply(_("No matching entries on the AKILL list."));
		else
			source.Reply(END_OF_ANY_LIST, "Akill");
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number)
			return;

		XLine *x = akills->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Current AKILL list:\n"
					"  Num   Mask                              Reason"));
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

		XLine *x = akills->GetEntry(Number - 1);

		if (!x)
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Current AKILL list:"));
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
	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
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
			return;
		}
		else if (expires > 0)
			expires += Anope::CurTime;

		if (params.size() <= last_param)
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		Anope::string reason = params[last_param];
		if (last_param == 2 && params.size() > 3)
			reason += " " + params[3];
		if (!mask.empty() && !reason.empty())
		{
			std::pair<int, XLine *> canAdd = akills->CanAdd(mask, expires);
			if (mask.find('!') != Anope::string::npos)
				source.Reply(_("\002Reminder\002: AKILL masks cannot contain nicknames; make sure you have \002not\002 included a nick portion in your mask."));
			else if (mask.find('@') == Anope::string::npos)
				source.Reply(BAD_USERHOST_MASK);
			else if (mask.find_first_not_of("~@.*?") == Anope::string::npos)
				source.Reply(USERHOST_MASK_TOO_WIDE, mask.c_str());
			else if (canAdd.first == 1)
				source.Reply(_("\002%s\002 already exists on the AKILL list."), canAdd.second->Mask.c_str());
			else if (canAdd.first == 2)
				source.Reply(_("Expiry time of \002%s\002 changed."), canAdd.second->Mask.c_str());
			else if (canAdd.first == 3)
				source.Reply(_("\002%s\002 is already covered by %s."), mask.c_str(), canAdd.second->Mask.c_str());
			else
			{
				User *user = finduser(mask);
				if (user)
					mask = "*@" + user->host;
				unsigned int affected = 0;
				for (Anope::insensitive_map<User *>::iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
					if (Anope::Match(it->second->GetIdent() + "@" + it->second->host, mask))
						++affected;
				float percent = static_cast<float>(affected) / static_cast<float>(UserListByNick.size()) * 100.0;

				if (percent > 95)
				{
					source.Reply(USERHOST_MASK_TOO_WIDE, mask.c_str());
					Log(LOG_ADMIN, u, this) << "tried to akill " << percent << "% of the network (" << affected << " users)";
					return;
				}

				XLine *x = akills->Add(mask, u->nick, expires, reason);

				EventReturn MOD_RESULT;
				FOREACH_RESULT(I_OnAddXLine, OnAddXLine(u, x, akills));
				if (MOD_RESULT == EVENT_STOP)
				{
					delete x;
					return;
				}

				source.Reply(_("\002%s\002 added to the AKILL list."), mask.c_str());

				Log(LOG_ADMIN, u, this) << "on " << mask << " (" << reason << ") expires in " << (expires ? duration(expires - Anope::CurTime) : "never") << " [affects " << affected << " user(s) (" << percent << "%)]";

				if (readonly)
					source.Reply(READ_ONLY_MODE);
			}
		}
		else
			this->OnSyntaxError(source, "ADD");

		return;
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		if (akills->GetList().empty())
		{
			source.Reply(_("AKILL list is empty."));
			return;
		}

		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkillDelCallback list(source, mask);
			list.Process();
		}
		else
		{
			XLine *x = akills->HasEntry(mask);

			if (!x)
			{
				source.Reply(_("\002%s\002 not found on the AKILL list."), mask.c_str());
				return;
			}

			FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, x, akills));

			AkillDelCallback::DoDel(source, x);
			source.Reply(_("\002%s\002 deleted from the AKILL list."), mask.c_str());
		}

		if (readonly)
			source.Reply(READ_ONLY_MODE);

		return;
	}

	void DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (akills->GetList().empty())
		{
			source.Reply(_("AKILL list is empty."));
			return;
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

			for (unsigned i = 0, end = akills->GetCount(); i < end; ++i)
			{
				XLine *x = akills->GetEntry(i);

				if (mask.empty() || mask.equals_ci(x->Mask) || Anope::Match(x->Mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						source.Reply(_("Current AKILL list:\n"
								"  Num   Mask                              Reason"));
					}

					AkillListCallback::DoList(source, x, i);
				}
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on the AKILL list."));
			else
				source.Reply(END_OF_ANY_LIST, "Akill");
		}

		return;
	}

	void DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (akills->GetList().empty())
		{
			source.Reply(_("AKILL list is empty."));
			return;
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

			for (unsigned i = 0, end = akills->GetCount(); i < end; ++i)
			{
				XLine *x = akills->GetEntry(i);

				if (mask.empty() || mask.equals_ci(x->Mask) || Anope::Match(x->Mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						source.Reply(_("Current AKILL list:"));
					}

					AkillViewCallback::DoList(source, x, i);
				}
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on the AKILL list."));
		}

		return;
	}

	void DoClear(CommandSource &source)
	{
		User *u = source.u;
		FOREACH_MOD(I_OnDelXLine, OnDelXLine(u, NULL, akills));
		akills->Clear();
		source.Reply(_("The AKILL list has been cleared."));

		return;
	}
 public:
	CommandOSAKill(Module *creator) : Command(creator, "operserv/akill", 1, 4)
	{
		this->SetDesc(_("Manipulate the AKILL list"));
		this->SetSyntax(_("ADD [+\037expiry\037] \037mask\037 \037reason\037"));
		this->SetSyntax(_("DEL {\037mask\037 | \037entry-num\037 | \037list\037}"));
		this->SetSyntax(_("LIST [\037mask\037 | \037list\037]"));
		this->SetSyntax(_("VIEW [\037mask\037 | \037list\037]"));
		this->SetSyntax(_("CLEAR"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &cmd = params[0];

		if (!akills)
			return;

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

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows Services operators to manipulate the AKILL list. If\n"
				"a user matching an AKILL mask attempts to connect, Services\n"
				"will issue a KILL for that user and, on supported server\n"
				"types, will instruct all servers to add a ban (K-line) for\n"
				"the mask which the user matched.\n"
				" \n"
				"\002AKILL ADD\002 adds the given nick or user@host/ip mask to the AKILL\n"
				"list for the given reason (which \002must\002 be given).\n"
				"\037expiry\037 is specified as an integer followed by one of \037d\037 \n"
				"(days), \037h\037 (hours), or \037m\037 (minutes). Combinations (such as \n"
				"\0371h30m\037) are not permitted. If a unit specifier is not \n"
				"included, the default is days (so \037+30\037 by itself means 30 \n"
				"days). To add an AKILL which does not expire, use \037+0\037. If the\n"
				"usermask to be added starts with a \037+\037, an expiry time must\n"
				"be given, even if it is the same as the default. The\n"
				"current AKILL default expiry time can be found with the\n"
				"\002STATS AKILL\002 command.\n"
				" \n"
				"The \002AKILL DEL\002 command removes the given mask from the\n"
				"AKILL list if it is present.  If a list of entry numbers is \n"
				"given, those entries are deleted.  (See the example for LIST \n"
				"below.)\n"
				" \n"
				"The \002AKILL LIST\002 command displays the AKILL list.  \n"
				"If a wildcard mask is given, only those entries matching the\n"
				"mask are displayed.  If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002AKILL LIST 2-5,7-9\002\n"
				"      Lists AKILL entries numbered 2 through 5 and 7 \n"
				"      through 9.\n"
				"      \n"
				"\002AKILL VIEW\002 is a more verbose version of \002AKILL LIST\002, and \n"
				"will show who added an AKILL, the date it was added, and when \n"
				"it expires, as well as the user@host/ip mask and reason.\n"
				" \n"
				"\002AKILL CLEAR\002 clears all entries of the AKILL list."));
		return true;
	}
};

class OSAKill : public Module
{
	CommandOSAKill commandosakill;

 public:
	OSAKill(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandosakill(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandosakill);
	}
};

MODULE_INIT(OSAKill)
