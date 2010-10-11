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
	User *u;
	unsigned Deleted;
 public:
	ExceptionDelCallback(User *_u, const Anope::string &numlist) : NumberList(numlist, true), u(_u), Deleted(0)
	{
	}

	~ExceptionDelCallback()
	{
		if (!Deleted)
			u->SendMessage(OperServ, OPER_EXCEPTION_NO_MATCH);
		else if (Deleted == 1)
			u->SendMessage(OperServ, OPER_EXCEPTION_DELETED_ONE);
		else
			u->SendMessage(OperServ, OPER_EXCEPTION_DELETED_SEVERAL, Deleted);
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (Number > exceptions.size())
			return;

		++Deleted;

		DoDel(u, Number - 1);
	}

	static void DoDel(User *u, unsigned index)
	{
		FOREACH_MOD(I_OnExceptionDel, OnExceptionDel(u, exceptions[index]));

		delete exceptions[index];
		exceptions.erase(exceptions.begin() + index);
	}
};

class ExceptionListCallback : public NumberList
{
 protected:
	User *u;
	bool SentHeader;
 public:
	ExceptionListCallback(User *_u, const Anope::string &numlist) : NumberList(numlist, false), u(_u), SentHeader(false)
	{
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (Number > exceptions.size())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			u->SendMessage(OperServ, OPER_EXCEPTION_LIST_HEADER);
			u->SendMessage(OperServ, OPER_EXCEPTION_LIST_COLHEAD);
		}

		DoList(u, Number - 1);
	}

	static void DoList(User *u, unsigned index)
	{
		if (index >= exceptions.size())
			return;

		u->SendMessage(OperServ, OPER_EXCEPTION_LIST_FORMAT, index + 1, exceptions[index]->limit, exceptions[index]->mask.c_str());
	}
};

class ExceptionViewCallback : public ExceptionListCallback
{
 public:
	ExceptionViewCallback(User *_u, const Anope::string &numlist) : ExceptionListCallback(_u, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (Number > exceptions.size())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			u->SendMessage(OperServ, OPER_EXCEPTION_LIST_HEADER);
		}

		DoList(u, Number - 1);
	}

	static void DoList(User *u, unsigned index)
	{
		if (index >= exceptions.size())
			return;

		Anope::string expirebuf = expire_left(u->Account(), exceptions[index]->expires);

		u->SendMessage(OperServ, OPER_EXCEPTION_VIEW_FORMAT, index + 1, exceptions[index]->mask.c_str(), !exceptions[index]->who.empty() ? exceptions[index]->who.c_str() : "<unknown>", do_strftime((exceptions[index]->time ? exceptions[index]->time : Anope::CurTime)).c_str(), expirebuf.c_str(), exceptions[index]->limit, exceptions[index]->reason.c_str());
	}
};

class CommandOSSession : public Command
{
 private:
	CommandReturn DoList(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string param = params[1];

		unsigned mincount = param.is_pos_number_only() ? convertTo<unsigned>(param) : 0;

		if (mincount <= 1)
			u->SendMessage(OperServ, OPER_SESSION_INVALID_THRESHOLD);
		else
		{
			u->SendMessage(OperServ, OPER_SESSION_LIST_HEADER, mincount);
			u->SendMessage(OperServ, OPER_SESSION_LIST_COLHEAD);

			for (session_map::const_iterator it = SessionList.begin(), it_end = SessionList.end(); it != it_end; ++it)
			{
				Session *session = it->second;

				if (session->count >= mincount)
					u->SendMessage(OperServ, OPER_SESSION_LIST_FORMAT, session->count, session->host.c_str());
			}
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string param = params[1];
		Session *session = findsession(param);

		if (!session)
			u->SendMessage(OperServ, OPER_SESSION_NOT_FOUND, param.c_str());
		else
		{
			Exception *exception = find_host_exception(param);
			u->SendMessage(OperServ, OPER_SESSION_VIEW_FORMAT, param.c_str(), session->count, exception ? exception-> limit : Config->DefSessionLimit);
		}

		return MOD_CONT;
	}
 public:
	CommandOSSession() : Command("SESSION", 2, 2, "operserv/session")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string cmd = params[0];

		if (!Config->LimitSessions)
		{
			u->SendMessage(OperServ, OPER_EXCEPTION_DISABLED);
			return MOD_CONT;
		}

		if (cmd.equals_ci("LIST"))
			return this->DoList(u, params);
		else if (cmd.equals_ci("VIEW"))
			return this->DoView(u, params);
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(OperServ, OPER_HELP_SESSION);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(OperServ, u, "SESSION", OPER_SESSION_LIST_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(OperServ, OPER_HELP_CMD_SESSION);
	}
};

class CommandOSException : public Command
{
 private:
	CommandReturn DoAdd(User *u, const std::vector<Anope::string> &params)
	{
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
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}

		Anope::string reason = params[last_param];
		if (last_param == 3 && params.size() > 4)
			reason += " " + params[4];
		if (reason.empty())
		{
			this->OnSyntaxError(u, "ADD");
			return MOD_CONT;
		}

		time_t expires = !expiry.empty() ? dotime(expiry) : Config->ExceptionExpiry;
		if (expires < 0)
		{
			u->SendMessage(OperServ, BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += Anope::CurTime;

		int limit = !limitstr.empty() && limitstr.is_number_only() ? convertTo<int>(limitstr) : -1;

		if (limit < 0 || limit > static_cast<int>(Config->MaxSessionLimit))
		{
			u->SendMessage(OperServ, OPER_EXCEPTION_INVALID_LIMIT, Config->MaxSessionLimit);
			return MOD_CONT;
		}
		else
		{
			if (mask.find('!') == Anope::string::npos || mask.find('@') == Anope::string::npos)
			{
				u->SendMessage(OperServ, OPER_EXCEPTION_INVALID_HOSTMASK);
				return MOD_CONT;
			}

			x = exception_add(u, mask, limit, reason, u->nick, expires);

			if (x == 1)
				u->SendMessage(OperServ, OPER_EXCEPTION_ADDED, mask.c_str(), limit);

			if (readonly)
				u->SendMessage(OperServ, READ_ONLY_MODE);
		}

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

		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			ExceptionDelCallback list(u, mask);
			list.Process();
		}
		else
		{
			unsigned i = 0, end = exceptions.size();
			for (; i < end; ++i)
				if (mask.equals_ci(exceptions[i]->mask))
				{
					ExceptionDelCallback::DoDel(u, i);
					u->SendMessage(OperServ, OPER_EXCEPTION_DELETED, mask.c_str());
					break;
				}
			if (i == end)
				u->SendMessage(OperServ, OPER_EXCEPTION_NOT_FOUND, mask.c_str());
		}

		if (readonly)
			u->SendMessage(OperServ, READ_ONLY_MODE);

		return MOD_CONT;
	}

	CommandReturn DoMove(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string n1str = params.size() > 1 ? params[1] : ""; /* From position */
		Anope::string n2str = params.size() > 2 ? params[2] : ""; /* To position */
		int n1, n2;

		if (n2str.empty())
		{
			this->OnSyntaxError(u, "MOVE");
			return MOD_CONT;
		}

		n1 = n1str.is_pos_number_only() ? convertTo<int>(n1str) - 1 : -1;
		n2 = n2str.is_pos_number_only() ? convertTo<int>(n2str) - 1 : -1;

		if (n1 >= 0 && n1 < exceptions.size() && n2 >= 0 && n2 < exceptions.size() && n1 != n2)
		{
			Exception *temp = exceptions[n1];
			exceptions[n1] = exceptions[n2];
			exceptions[n2] = temp;

			u->SendMessage(OperServ, OPER_EXCEPTION_MOVED, exceptions[n1]->mask.c_str(), n1 + 1, n2 + 1);

			if (readonly)
				u->SendMessage(OperServ, READ_ONLY_MODE);
		}
		else
			this->OnSyntaxError(u, "MOVE");

		return MOD_CONT;
	}

	CommandReturn DoList(User *u, const std::vector<Anope::string> &params)
	{
		expire_exceptions();
		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			ExceptionListCallback list(u, mask);
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
						u->SendMessage(OperServ, OPER_EXCEPTION_LIST_HEADER);
						u->SendMessage(OperServ, OPER_EXCEPTION_LIST_COLHEAD);
					}

					ExceptionListCallback::DoList(u, i);
				}

			if (!SentHeader)
				u->SendMessage(OperServ, OPER_EXCEPTION_NO_MATCH);
		}

		return MOD_CONT;
	}

	CommandReturn DoView(User *u, const std::vector<Anope::string> &params)
	{
		expire_exceptions();
		Anope::string mask = params.size() > 1 ? params[1] : "";

		if (!mask.empty() && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			ExceptionViewCallback list(u, mask);
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
						u->SendMessage(OperServ, OPER_EXCEPTION_LIST_HEADER);
					}

					ExceptionViewCallback::DoList(u, i);
				}

			if (!SentHeader)
				u->SendMessage(OperServ, OPER_EXCEPTION_NO_MATCH);
		}

		return MOD_CONT;
	}
 public:
	CommandOSException() : Command("EXCEPTION", 1, 5)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string cmd = params[0];

		if (!Config->LimitSessions)
		{
			u->SendMessage(OperServ, OPER_EXCEPTION_DISABLED);
			return MOD_CONT;
		}

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(u, params);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(u, params);
		else if (cmd.equals_ci("MOVE"))
			return this->DoMove(u, params);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(u, params);
		else if (cmd.equals_ci("VIEW"))
			return this->DoView(u, params);
		else
			this->OnSyntaxError(u, "");
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(OperServ, OPER_HELP_EXCEPTION);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(OperServ, u, "EXCEPTION", OPER_EXCEPTION_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(OperServ, OPER_HELP_CMD_EXCEPTION);
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
