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

class ExceptionDelCallback : public NumberList
{
 protected:
	CommandSource &source;
	unsigned Deleted;
 public:
	ExceptionDelCallback(CommandSource &_source, const Anope::string &numlist) : NumberList(numlist, true), source(_source), Deleted(0)
	{
	}

	~ExceptionDelCallback()
	{
		if (!Deleted)
			source.Reply(_("No matching entries on session-limit exception list."));
		else if (Deleted == 1)
			source.Reply(_("Deleted 1 entry from session-limit exception list."));
		else
			source.Reply(_("Deleted %d entries from session-limit exception list."), Deleted);
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (!Number || Number > exceptions.size())
			return;

		++Deleted;

		DoDel(source, Number - 1);
	}

	static void DoDel(CommandSource &source, unsigned index)
	{
		FOREACH_MOD(I_OnExceptionDel, OnExceptionDel(source.u, exceptions[index]));

		delete exceptions[index];
		exceptions.erase(exceptions.begin() + index);
	}
};

class ExceptionListCallback : public NumberList
{
 protected:
	CommandSource &source;
	bool SentHeader;
 public:
	ExceptionListCallback(CommandSource &_source, const Anope::string &numlist) : NumberList(numlist, false), source(_source), SentHeader(false)
	{
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (!Number || Number > exceptions.size())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Current Session Limit Exception list:"));
			source.Reply(_("Num  Limit  Host"));
		}

		DoList(source, Number - 1);
	}

	static void DoList(CommandSource &source, unsigned index)
	{
		if (index >= exceptions.size())
			return;

		source.Reply(_("%3d  %4d   %s"), index + 1, exceptions[index]->limit, exceptions[index]->mask.c_str());
	}
};

class ExceptionViewCallback : public ExceptionListCallback
{
 public:
	ExceptionViewCallback(CommandSource &_source, const Anope::string &numlist) : ExceptionListCallback(_source, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > exceptions.size())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Current Session Limit Exception list:"));
		}

		DoList(source, Number - 1);
	}

	static void DoList(CommandSource &source, unsigned index)
	{
		if (index >= exceptions.size())
			return;

		Anope::string expirebuf = expire_left(source.u->Account(), exceptions[index]->expires);

		source.Reply(_("%3d.  %s  (by %s on %s; %s)\n " "    Limit: %-4d  - %s"), index + 1, exceptions[index]->mask.c_str(), !exceptions[index]->who.empty() ? exceptions[index]->who.c_str() : "<unknown>", do_strftime((exceptions[index]->time ? exceptions[index]->time : Anope::CurTime)).c_str(), expirebuf.c_str(), exceptions[index]->limit, exceptions[index]->reason.c_str());
	}
};

class CommandOSSession : public Command
{
 private:
	CommandReturn DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Anope::string param = params[1];

		unsigned mincount = 0;
		try
		{
			mincount = convertTo<unsigned>(param);
		}
		catch (const ConvertException &) { }

		if (mincount <= 1)
			source.Reply(_("Invalid threshold value. It must be a valid integer greater than 1."));
		else
		{
			source.Reply(_("Hosts with at least \002%d\002 sessions:"), mincount);
			source.Reply(_("Sessions  Host"));

			for (patricia_tree<Session *>::iterator it(SessionList); it.next();)
			{
				Session *session = *it;

				if (session->count >= mincount)
					source.Reply(_("%6d    %s"), session->count, session->host.c_str());
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Anope::string param = params[1];
		Session *session = findsession(param);

		if (!session)
			source.Reply(_("\002%s\002 not found on session list."), param.c_str());
		else
		{
			Exception *exception = find_host_exception(param);
			source.Reply(_("The host \002%s\002 currently has \002%d\002 sessions with a limit of \002%d\002."), param.c_str(), session->count, exception ? exception-> limit : Config->DefSessionLimit);
		}

		return MOD_CONT;
	}
 public:
	CommandOSSession() : Command("SESSION", 2, 2, "operserv/session")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &cmd = params[0];

		if (!Config->LimitSessions)
		{
			source.Reply(_("Session limiting is disabled."));
			return MOD_CONT;
		}

		if (cmd.equals_ci("LIST"))
			return this->DoList(source, params);
		else if (cmd.equals_ci("VIEW"))
			return this->DoView(source, params);
		else
			this->OnSyntaxError(source, "");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002SESSION LIST \037threshold\037\002\n"
				"        \002SESSION VIEW \037host\037\002\n"
				" \n"
				"Allows Services Operators to view the session list.\n"
				"\002SESSION LIST\002 lists hosts with at least \037threshold\037 sessions.\n"
				"The threshold must be a number greater than 1. This is to \n"
				"prevent accidental listing of the large number of single \n"
				"session hosts.\n"
				"\002SESSION VIEW\002 displays detailed information about a specific\n"
				"host - including the current session count and session limit.\n"
				"The \037host\037 value may not include wildcards.\n"
				"See the \002EXCEPTION\002 help for more information about session\n"
				"limiting and how to set session limits specific to certain\n"
				"hosts and groups thereof."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SESSION", _("SESSION LIST \037limit\037"));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    SESSION     View the list of host sessions"));
	}
};

class CommandOSException : public Command
{
 private:
	CommandReturn DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		Anope::string mask, expiry, limitstr;
		unsigned last_param = 3;
		int x;

		mask = params.size() > 1 ? params[1] : "";
		if (!mask.empty() && mask[0] == '+')
		{
			expiry = mask;
			mask = params.size() > 2 ? params[2] : "";
			last_param = 4;
		}

		limitstr = params.size() > last_param - 1 ? params[last_param - 1] : "";

		if (params.size() <= last_param)
		{
			this->OnSyntaxError(source, "ADD");
			return MOD_CONT;
		}

		Anope::string reason = params[last_param];
		if (last_param == 3 && params.size() > 4)
			reason += " " + params[4];
		if (reason.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return MOD_CONT;
		}

		time_t expires = !expiry.empty() ? dotime(expiry) : Config->ExceptionExpiry;
		if (expires < 0)
		{
			source.Reply(LanguageString::BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += Anope::CurTime;

		int limit = -1;
		try
		{
			limit = convertTo<int>(limitstr);
		}
		catch (const ConvertException &) { }

		if (limit < 0 || limit > static_cast<int>(Config->MaxSessionLimit))
		{
			source.Reply(_("Invalid session limit. It must be a valid integer greater than or equal to zero and less than \002%d\002."), Config->MaxSessionLimit);
			return MOD_CONT;
		}
		else
		{
			if (mask.find('!') != Anope::string::npos || mask.find('@') != Anope::string::npos)
			{
				source.Reply(_("Invalid hostmask. Only real hostmasks are valid as exceptions are not matched against nicks or usernames."));
				return MOD_CONT;
			}

			x = exception_add(u, mask, limit, reason, u->nick, expires);

			if (x == 1)
				source.Reply(_("Session limit for \002%s\002 set to \002%d\002."), mask.c_str(), limit);

			if (readonly)
				source.Reply(LanguageString::READ_ONLY_MODE);
		}

		return MOD_CONT;
	}

	CommandReturn DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return MOD_CONT;
		}

		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			ExceptionDelCallback list(source, mask);
			list.Process();
		}
		else
		{
			unsigned i = 0, end = exceptions.size();
			for (; i < end; ++i)
				if (mask.equals_ci(exceptions[i]->mask))
				{
					ExceptionDelCallback::DoDel(source, i);
					source.Reply(_("\002%s\002 deleted from session-limit exception list."), mask.c_str());
					break;
				}
			if (i == end)
				source.Reply(_("\002%s\002 not found on session-limit exception list."), mask.c_str());
		}

		if (readonly)
			source.Reply(LanguageString::READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoMove(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &n1str = params.size() > 1 ? params[1] : ""; /* From position */
		const Anope::string &n2str = params.size() > 2 ? params[2] : ""; /* To position */
		int n1, n2;

		if (n2str.empty())
		{
			this->OnSyntaxError(source, "MOVE");
			return MOD_CONT;
		}

		n1 = n2 = -1;
		try
		{
			n1 = convertTo<int>(n1str);
			n2 = convertTo<int>(n2str);
		}
		catch (const ConvertException &) { }

		if (n1 >= 0 && n1 < exceptions.size() && n2 >= 0 && n2 < exceptions.size() && n1 != n2)
		{
			Exception *temp = exceptions[n1];
			exceptions[n1] = exceptions[n2];
			exceptions[n2] = temp;

			source.Reply(_("Exception for \002%s\002 (#%d) moved to position \002%d\002."), exceptions[n1]->mask.c_str(), n1 + 1, n2 + 1);

			if (readonly)
				source.Reply(LanguageString::READ_ONLY_MODE);
		}
		else
			this->OnSyntaxError(source, "MOVE");

		return MOD_CONT;
	}

	CommandReturn DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		expire_exceptions();
		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			ExceptionListCallback list(source, mask);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = exceptions.size(); i < end; ++i)
				if (mask.empty() || Anope::Match(exceptions[i]->mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						source.Reply(_("Current Session Limit Exception list:"));
						source.Reply(_("Num  Limit  Host"));
					}

					ExceptionListCallback::DoList(source, i);
				}

			if (!SentHeader)
				source.Reply(_("No matching entries on session-limit exception list."));
		}

		return MOD_CONT;
	}

	CommandReturn DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		expire_exceptions();
		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			ExceptionViewCallback list(source, mask);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = exceptions.size(); i < end; ++i)
				if (mask.empty() || Anope::Match(exceptions[i]->mask, mask))
				{
					if (!SentHeader)
					{
						SentHeader = true;
						source.Reply(_("Current Session Limit Exception list:"));
					}

					ExceptionViewCallback::DoList(source, i);
				}

			if (!SentHeader)
				source.Reply(_("No matching entries on session-limit exception list."));
		}

		return MOD_CONT;
	}
 public:
	CommandOSException() : Command("EXCEPTION", 1, 5)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &cmd = params[0];

		if (!Config->LimitSessions)
		{
			source.Reply(_("Session limiting is disabled."));
			return MOD_CONT;
		}

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, params);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, params);
		else if (cmd.equals_ci("MOVE"))
			return this->DoMove(source, params);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, params);
		else if (cmd.equals_ci("VIEW"))
			return this->DoView(source, params);
		else
			this->OnSyntaxError(source, "");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002EXCEPTION ADD [\037+expiry\037] \037mask\037 \037limit\037 \037reason\037\002\n"
				"        \002EXCEPTION DEL {\037mask\037 | \037list\037}\002\n"
				"        \002EXCEPTION MOVE \037num\037 \037position\037\002\n"
				"        \002EXCEPTION LIST [\037mask\037 | \037list\037]\002\n"
				"        \002EXCEPTION VIEW [\037mask\037 | \037list\037]\002\n"
				" \n"
				"Allows Services Operators to manipulate the list of hosts that\n"
				"have specific session limits - allowing certain machines, \n"
				"such as shell servers, to carry more than the default number\n"
				"of clients at a time. Once a host reaches its session limit,\n"
				"all clients attempting to connect from that host will be\n"
				"killed. Before the user is killed, they are notified, via a\n"
				"/NOTICE from %s, of a source of help regarding session\n"
				"limiting. The content of this notice is a config setting.\n"
				" \n"
				"\002EXCEPTION ADD\002 adds the given host mask to the exception list.\n"
				"Note that \002nick!user@host\002 and \002user@host\002 masks are invalid!\n"
				"Only real host masks, such as \002box.host.dom\002 and \002*.host.dom\002,\n"
				"are allowed because sessions limiting does not take nick or\n"
				"user names into account. \037limit\037 must be a number greater than\n"
				"or equal to zero. This determines how many sessions this host\n"
				"may carry at a time. A value of zero means the host has an\n"
				"unlimited session limit. See the \002AKILL\002 help for details about\n"
				"the format of the optional \037expiry\037 parameter.\n"
				"\002EXCEPTION DEL\002 removes the given mask from the exception list.\n"
				"\002EXCEPTION MOVE\002 moves exception \037num\037 to \037position\037. The\n"
				"exceptions inbetween will be shifted up or down to fill the gap.\n"
				"\002EXCEPTION LIST\002 and \002EXCEPTION VIEW\002 show all current\n"
				"exceptions; if the optional mask is given, the list is limited\n"
				"to those exceptions matching the mask. The difference is that\n"
				"\002EXCEPTION VIEW\002 is more verbose, displaying the name of the\n"
				"person who added the exception, its session limit, reason, \n"
				"host mask and the expiry date and time.\n"
				" \n"
				"Note that a connecting client will \"use\" the first exception\n"
				"their host matches. Large exception lists and widely matching\n"
				"exception masks are likely to degrade services' performance."),
				OperServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "EXCEPTION", _("EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [\037params\037]"));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    EXCEPTION   Modify the session-limit exception list"));
	}
};

class OSSession : public Module
{
	CommandOSSession commandossession;
	CommandOSException commandosexception;

 public:
	OSSession(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandossession);
		this->AddCommand(OperServ, &commandosexception);
	}
};

MODULE_INIT(OSSession)
