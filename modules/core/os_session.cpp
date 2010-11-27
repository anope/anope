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
			source.Reply(OPER_EXCEPTION_NO_MATCH);
		else if (Deleted == 1)
			source.Reply(OPER_EXCEPTION_DELETED_ONE);
		else
			source.Reply(OPER_EXCEPTION_DELETED_SEVERAL, Deleted);
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
			source.Reply(OPER_EXCEPTION_LIST_HEADER);
			source.Reply(OPER_EXCEPTION_LIST_COLHEAD);
		}

		DoList(source, Number - 1);
	}

	static void DoList(CommandSource &source, unsigned index)
	{
		if (index >= exceptions.size())
			return;

		source.Reply(OPER_EXCEPTION_LIST_FORMAT, index + 1, exceptions[index]->limit, exceptions[index]->mask.c_str());
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
			source.Reply(OPER_EXCEPTION_LIST_HEADER);
		}

		DoList(source, Number - 1);
	}

	static void DoList(CommandSource &source, unsigned index)
	{
		if (index >= exceptions.size())
			return;

		Anope::string expirebuf = expire_left(source.u->Account(), exceptions[index]->expires);

		source.Reply(OPER_EXCEPTION_VIEW_FORMAT, index + 1, exceptions[index]->mask.c_str(), !exceptions[index]->who.empty() ? exceptions[index]->who.c_str() : "<unknown>", do_strftime((exceptions[index]->time ? exceptions[index]->time : Anope::CurTime)).c_str(), expirebuf.c_str(), exceptions[index]->limit, exceptions[index]->reason.c_str());
	}
};

class CommandOSSession : public Command
{
 private:
	CommandReturn DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Anope::string param = params[1];

		unsigned mincount = param.is_pos_number_only() ? convertTo<unsigned>(param) : 0;

		if (mincount <= 1)
			source.Reply(OPER_SESSION_INVALID_THRESHOLD);
		else
		{
			source.Reply(OPER_SESSION_LIST_HEADER, mincount);
			source.Reply(OPER_SESSION_LIST_COLHEAD);

			for (patricia_tree<Session *>::const_iterator it = SessionList.begin(), it_end = SessionList.end(); it != it_end; ++it)
			{
				Session *session = *it;

				if (session->count >= mincount)
					source.Reply(OPER_SESSION_LIST_FORMAT, session->count, session->host.c_str());
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Anope::string param = params[1];
		Session *session = findsession(param);

		if (!session)
			source.Reply(OPER_SESSION_NOT_FOUND, param.c_str());
		else
		{
			Exception *exception = find_host_exception(param);
			source.Reply(OPER_SESSION_VIEW_FORMAT, param.c_str(), session->count, exception ? exception-> limit : Config->DefSessionLimit);
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
			source.Reply(OPER_EXCEPTION_DISABLED);
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
		source.Reply(OPER_HELP_SESSION);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SESSION", OPER_SESSION_LIST_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(OPER_HELP_CMD_SESSION);
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
			source.Reply(BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += Anope::CurTime;

		int limit = !limitstr.empty() && limitstr.is_number_only() ? convertTo<int>(limitstr) : -1;

		if (limit < 0 || limit > static_cast<int>(Config->MaxSessionLimit))
		{
			source.Reply(OPER_EXCEPTION_INVALID_LIMIT, Config->MaxSessionLimit);
			return MOD_CONT;
		}
		else
		{
			if (mask.find('!') == Anope::string::npos || mask.find('@') == Anope::string::npos)
			{
				source.Reply(OPER_EXCEPTION_INVALID_HOSTMASK);
				return MOD_CONT;
			}

			x = exception_add(u, mask, limit, reason, u->nick, expires);

			if (x == 1)
				source.Reply(OPER_EXCEPTION_ADDED, mask.c_str(), limit);

			if (readonly)
				source.Reply(READ_ONLY_MODE);
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
					source.Reply(OPER_EXCEPTION_DELETED, mask.c_str());
					break;
				}
			if (i == end)
				source.Reply(OPER_EXCEPTION_NOT_FOUND, mask.c_str());
		}

		if (readonly)
			source.Reply(READ_ONLY_MODE);

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

		n1 = n1str.is_pos_number_only() ? convertTo<int>(n1str) - 1 : -1;
		n2 = n2str.is_pos_number_only() ? convertTo<int>(n2str) - 1 : -1;

		if (n1 >= 0 && n1 < exceptions.size() && n2 >= 0 && n2 < exceptions.size() && n1 != n2)
		{
			Exception *temp = exceptions[n1];
			exceptions[n1] = exceptions[n2];
			exceptions[n2] = temp;

			source.Reply(OPER_EXCEPTION_MOVED, exceptions[n1]->mask.c_str(), n1 + 1, n2 + 1);

			if (readonly)
				source.Reply(READ_ONLY_MODE);
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
						source.Reply(OPER_EXCEPTION_LIST_HEADER);
						source.Reply(OPER_EXCEPTION_LIST_COLHEAD);
					}

					ExceptionListCallback::DoList(source, i);
				}

			if (!SentHeader)
				source.Reply(OPER_EXCEPTION_NO_MATCH);
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
						source.Reply(OPER_EXCEPTION_LIST_HEADER);
					}

					ExceptionViewCallback::DoList(source, i);
				}

			if (!SentHeader)
				source.Reply(OPER_EXCEPTION_NO_MATCH);
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
			source.Reply(OPER_EXCEPTION_DISABLED);
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
		source.Reply(OPER_HELP_EXCEPTION);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "EXCEPTION", OPER_EXCEPTION_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(OPER_HELP_CMD_EXCEPTION);
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
