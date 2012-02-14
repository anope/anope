/* OperServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "os_session.h"

class MySessionService : public SessionService
{
	SessionMap Sessions;
	ExceptionVector Exceptions;
 public:
	MySessionService(Module *m) : SessionService(m) { }

	void AddException(Exception *e)
	{
		this->Exceptions.push_back(e);
	}

	void DelException(Exception *e)
	{
		ExceptionVector::iterator it = std::find(this->Exceptions.begin(), this->Exceptions.end(), e);
		if (it != this->Exceptions.end())
			this->Exceptions.erase(it);
	}

	Exception *FindException(User *u)
	{
		for (std::vector<Exception *>::const_iterator it = this->Exceptions.begin(), it_end = this->Exceptions.end(); it != it_end; ++it)
		{
			Exception *e = *it;
			if (Anope::Match(u->host, e->mask) || Anope::Match(u->ip, e->mask))
				return e;
		}
		return NULL;
	}

	Exception *FindException(const Anope::string &host)
	{
		for (std::vector<Exception *>::const_iterator it = this->Exceptions.begin(), it_end = this->Exceptions.end(); it != it_end; ++it)
		{
			Exception *e = *it;
			if (Anope::Match(host, e->mask))
				return e;
		}

		return NULL;
	}

	ExceptionVector &GetExceptions()
	{
		return this->Exceptions;
	}

	void AddSession(Session *s)
	{
		this->Sessions[s->host] = s;
	}

	void DelSession(Session *s)
	{
		this->Sessions.erase(s->host);
	}

	Session *FindSession(const Anope::string &mask)
	{
		SessionMap::iterator it = this->Sessions.find(mask);
		if (it != this->Sessions.end())
			return it->second;
		return NULL;
	}

	SessionMap &GetSessions()
	{
		return this->Sessions;
	}
};

class ExpireTimer : public Timer
{
 public:
	ExpireTimer() : Timer(Config->ExpireTimeout, Anope::CurTime, true) { }

	void Tick(time_t)
	{
		if (!session_service)
			return;
		for (unsigned i = session_service->GetExceptions().size(); i > 0; --i)
		{
			Exception *e = session_service->GetExceptions()[i - 1];

			if (!e->expires || e->expires > Anope::CurTime)
				continue;
			Log(findbot(Config->OperServ), "expire/exception") << "Session exception for " << e->mask << "has expired.";
			session_service->DelException(e);
			delete e;
		}
	}
};

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
		if (!Number || Number > session_service->GetExceptions().size())
			return;

		++Deleted;

		DoDel(source, Number - 1);
	}

	static void DoDel(CommandSource &source, unsigned index)
	{
		Exception *e = session_service->GetExceptions()[index];
		FOREACH_MOD(I_OnExceptionDel, OnExceptionDel(source.u, e));

		session_service->DelException(e);
		delete e;
	}
};

class CommandOSSession : public Command
{
 private:
	void DoList(CommandSource &source, const std::vector<Anope::string> &params)
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
			ListFormatter list;
			list.addColumn("Session").addColumn("Host");

			for (SessionService::SessionMap::iterator it = session_service->GetSessions().begin(), it_end = session_service->GetSessions().end(); it != it_end; ++it)
			{
				Session *session = it->second;

				if (session->count >= mincount)
				{
					ListFormatter::ListEntry entry;
					entry["Session"] = stringify(session->count);
					entry["Host"] = session->host;
					list.addEntry(entry);
				}
			}

			source.Reply(_("Hosts with at least \002%d\002 sessions:"), mincount);

			std::vector<Anope::string> replies;
			list.Process(replies);

	
			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}

		return;
	}

	void DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Anope::string param = params[1];
		Session *session = session_service->FindSession(param);

		if (!session)
			source.Reply(_("\002%s\002 not found on session list."), param.c_str());
		else
		{
			Exception *exception = session_service->FindException(param);
			source.Reply(_("The host \002%s\002 currently has \002%d\002 sessions with a limit of \002%d\002."), param.c_str(), session->count, exception && exception->limit > Config->DefSessionLimit ? exception->limit : Config->DefSessionLimit);
		}

		return;
	}
 public:
	CommandOSSession(Module *creator) : Command(creator, "operserv/session", 2, 2)
	{
		this->SetDesc(_("View the list of host sessions"));
		this->SetSyntax(_("LIST \037threshold\037"));
		this->SetSyntax(_("VIEW \037host\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &cmd = params[0];

		if (!Config->LimitSessions)
		{
			source.Reply(_("Session limiting is disabled."));
			return;
		}

		if (cmd.equals_ci("LIST"))
			return this->DoList(source, params);
		else if (cmd.equals_ci("VIEW"))
			return this->DoView(source, params);
		else
			this->OnSyntaxError(source, "");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows Services Operators to view the session list.\n"
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
};

class CommandOSException : public Command
{
 private:
	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		Anope::string mask, expiry, limitstr;
		unsigned last_param = 3;

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
			return;
		}

		Anope::string reason = params[last_param];
		if (last_param == 3 && params.size() > 4)
			reason += " " + params[4];
		if (reason.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		time_t expires = !expiry.empty() ? dotime(expiry) : Config->ExceptionExpiry;
		if (expires < 0)
		{
			source.Reply(BAD_EXPIRY_TIME);
			return;
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
			return;
		}
		else
		{
			if (mask.find('!') != Anope::string::npos || mask.find('@') != Anope::string::npos)
			{
				source.Reply(_("Invalid hostmask. Only real hostmasks are valid, as exceptions are not matched against nicks or usernames."));
				return;
			}

			for (std::vector<Exception *>::iterator it = session_service->GetExceptions().begin(), it_end = session_service->GetExceptions().end(); it != it_end; ++it)
			{
				Exception *e = *it;
				if (e->mask.equals_ci(mask))
				{
					if (e->limit != limit)
					{
						e->limit = limit;
						source.Reply(_("Exception for \002%s\002 has been updated to %d."), mask.c_str(), e->limit);
					}
					else
						source.Reply(_("\002%s\002 already exists on the EXCEPTION list."), mask.c_str());
					return;
				}
			}

			Exception *exception = new Exception();
			exception->mask = mask;
			exception->limit = limit;
			exception->reason = reason;
			exception->time = Anope::CurTime;
			exception->who = u->nick;
			exception->expires = expires;

			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnExceptionAdd, OnExceptionAdd(exception));
			if (MOD_RESULT == EVENT_STOP)
				delete exception;
			else
			{
				session_service->AddException(exception);
				source.Reply(_("Session limit for \002%s\002 set to \002%d\002."), mask.c_str(), limit);
				if (readonly)
					source.Reply(READ_ONLY_MODE);
			}
		}

		return;
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			ExceptionDelCallback list(source, mask);
			list.Process();
		}
		else
		{
			unsigned i = 0, end = session_service->GetExceptions().size();
			for (; i < end; ++i)
				if (mask.equals_ci(session_service->GetExceptions()[i]->mask))
				{
					ExceptionDelCallback::DoDel(source, i);
					source.Reply(_("\002%s\002 deleted from session-limit exception list."), mask.c_str());
					break;
				}
			if (i == end)
				source.Reply(_("\002%s\002 not found on session-limit exception list."), mask.c_str());
		}

		if (readonly)
			source.Reply(READ_ONLY_MODE);

		return;
	}

	void DoMove(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &n1str = params.size() > 1 ? params[1] : ""; /* From position */
		const Anope::string &n2str = params.size() > 2 ? params[2] : ""; /* To position */
		int n1, n2;

		if (n2str.empty())
		{
			this->OnSyntaxError(source, "MOVE");
			return;
		}

		n1 = n2 = -1;
		try
		{
			n1 = convertTo<int>(n1str);
			n2 = convertTo<int>(n2str);
		}
		catch (const ConvertException &) { }

		if (n1 >= 0 && n1 < session_service->GetExceptions().size() && n2 >= 0 && n2 < session_service->GetExceptions().size() && n1 != n2)
		{
			Exception *temp = session_service->GetExceptions()[n1];
			session_service->GetExceptions()[n1] = session_service->GetExceptions()[n2];
			session_service->GetExceptions()[n2] = temp;

			source.Reply(_("Exception for \002%s\002 (#%d) moved to position \002%d\002."), session_service->GetExceptions()[n1]->mask.c_str(), n1 + 1, n2 + 1);

			if (readonly)
				source.Reply(READ_ONLY_MODE);
		}
		else
			this->OnSyntaxError(source, "MOVE");

		return;
	}

	void ProcessList(CommandSource &source, const std::vector<Anope::string> &params, ListFormatter &list)
	{
		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		if (session_service->GetExceptions().empty())
		{
			source.Reply(_("The session exception list is empty."));
			return;
		}

		if (!mask.empty() && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class ExceptionListCallback : public NumberList
			{
				ListFormatter &list;
			 public:
				ExceptionListCallback(ListFormatter &_list, const Anope::string &numlist) : NumberList(numlist, false), list(_list)
				{
				}

				void HandleNumber(unsigned Number)
				{
					if (!Number || Number > session_service->GetExceptions().size())
						return;

					Exception *e = session_service->GetExceptions()[Number - 1];

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(Number);
					entry["Mask"] = e->mask;
					entry["By"] = e->who;
					entry["Created"] = do_strftime(e->time);
					entry["Limit"] = stringify(e->limit);
					entry["Reason"] = e->reason;
					this->list.addEntry(entry);
				}
			}
			nl_list(list, mask);
			nl_list.Process();
		}
		else
		{
			for (unsigned i = 0, end = session_service->GetExceptions().size(); i < end; ++i)
			{
				Exception *e = session_service->GetExceptions()[i];
				if (mask.empty() || Anope::Match(e->mask, mask))
				{
					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(i + 1);
					entry["Mask"] = e->mask;
					entry["By"] = e->who;
					entry["Created"] = do_strftime(e->time);
					entry["Limit"] = stringify(e->limit);
					entry["Reason"] = e->reason;
					list.addEntry(entry);
				}
			}
		}

		if (list.isEmpty())
			source.Reply(_("No matching entries on session-limit exception list."));
		else
		{
			source.Reply(_("Current Session Limit Exception list:"));
		
			std::vector<Anope::string> replies;
			list.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
	}

	void DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ListFormatter list;
		list.addColumn("Number").addColumn("Limit").addColumn("Mask");

		this->ProcessList(source, params, list);
	}

	void DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ListFormatter list;
		list.addColumn("Number").addColumn("Mask").addColumn("By").addColumn("Created").addColumn("Limit").addColumn("Reason");

		this->ProcessList(source, params, list);
	}

 public:
	CommandOSException(Module *creator) : Command(creator, "operserv/exception", 1, 5)
	{
		this->SetDesc(_("Modify the session-limit exception list"));
		this->SetSyntax(_("ADD [\037+expiry\037] \037mask\037 \037limit\037 \037reason\037"));
		this->SetSyntax(_("DEL {\037mask\037 | \037list\037}"));
		this->SetSyntax(_("MOVE \037num\037 \037position\037"));
		this->SetSyntax(_("LIST [\037mask\037 | \037list\037]"));
		this->SetSyntax(_("VIEW [\037mask\037 | \037list\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &cmd = params[0];

		if (!Config->LimitSessions)
		{
			source.Reply(_("Session limiting is disabled."));
			return;
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

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows Services Operators to manipulate the list of hosts that\n"
				"have specific session limits - allowing certain machines, \n"
				"such as shell servers, to carry more than the default number\n"
				"of clients at a time. Once a host reaches its session limit,\n"
				"all clients attempting to connect from that host will be\n"
				"killed. Before the user is killed, they are notified, via a\n"
				"/NOTICE from %s, of a source of help regarding session\n"
				"limiting. The content of this notice is a config setting.\n"),
				Config->OperServ.c_str());
		source.Reply(" ");
		source.Reply(_("\002EXCEPTION ADD\002 adds the given host mask to the exception list.\n"
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
				"sessions inbetween will be shifted up or down to fill the gap.\n"
				"\002EXCEPTION LIST\002 and \002EXCEPTION VIEW\002 show all current\n"
				"sessions if the optional mask is given, the list is limited\n"
				"to those sessions matching the mask. The difference is that\n"
				"\002EXCEPTION VIEW\002 is more verbose, displaying the name of the\n"
				"person who added the exception, its session limit, reason, \n"
				"host mask and the expiry date and time.\n"
				" \n"
				"Note that a connecting client will \"use\" the first exception\n"
				"their host matches."));
		return true;
	}
};

class OSSession : public Module
{
	SerializeType exception_type;
	MySessionService ss;
	ExpireTimer expiretimer;
	CommandOSSession commandossession;
	CommandOSException commandosexception;
	service_reference<XLineManager> akills;

	void AddSession(User *u, bool exempt)
	{
		Session *session = this->ss.FindSession(u->host);

		if (session)
		{
			bool kill = false;
			if (Config->DefSessionLimit && session->count >= Config->DefSessionLimit)
			{
				kill = true;
				Exception *exception = this->ss.FindException(u);
				if (exception)
				{
					kill = false;
					if (exception->limit && session->count >= exception->limit)
						kill = true;
				}
			}

			/* Previously on IRCds that send a QUIT (InspIRCD) when a user is killed, the session for a host was
			 * decremented in do_quit, which caused problems and fixed here
			 *
			 * Now, we create the user struture before calling this to fix some user tracking issues,
			 * so we must increment this here no matter what because it will either be
			 * decremented in do_kill or in do_quit - Adam
			 */
			++session->count;
	
			if (kill && !exempt)
			{
				BotInfo *bi = findbot(Config->OperServ);
				if (bi)
				{
					if (!Config->SessionLimitExceeded.empty())
						u->SendMessage(bi, Config->SessionLimitExceeded.c_str(), u->host.c_str());
					if (!Config->SessionLimitDetailsLoc.empty())
						u->SendMessage(bi, "%s", Config->SessionLimitDetailsLoc.c_str());
				}

				++session->hits;
				if (Config->MaxSessionKill && session->hits >= Config->MaxSessionKill && akills)
				{
					const Anope::string &akillmask = "*@" + u->host;
					XLine *x = new XLine(akillmask, Config->OperServ, Anope::CurTime + Config->SessionAutoKillExpiry, "Session limit exceeded", XLineManager::GenerateUID());
					akills->AddXLine(x);
					akills->Send(NULL, x);
					Log(bi, "akill/session") << "Added a temporary AKILL for \2" << akillmask << "\2 due to excessive connections";
				}
				else
				{
					u->Kill(Config->OperServ, "Session limit exceeded");
					u = NULL; /* No guarentee u still exists */
				}
			}
		}
		else
		{
			session = new Session();
			session->host = u->host;
			session->count = 1;
			session->hits = 0;

			this->ss.AddSession(session);
		}
	}

	void DelSession(User *u)
	{
		Session *session = this->ss.FindSession(u->host);
		if (!session)
		{
			if (debug)
				Log() << "session: Tried to delete non-existant session: " << u->host;
			return;
		}

		if (session->count > 1)
		{
			--session->count;
			return;
		}

		this->ss.DelSession(session);
		delete session;
	}

 public:
	OSSession(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		exception_type("Exception", Exception::unserialize), ss(this), commandossession(this), commandosexception(this), akills("XLineManager", "xlinemanager/sgline")
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnUserConnect, I_OnUserLogoff };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
		ModuleManager::SetPriority(this, PRIORITY_FIRST);
	}

	void OnUserConnect(dynamic_reference<User> &user, bool &exempt)
	{
		if (user && Config->LimitSessions)
			this->AddSession(user, exempt);
	}

	void OnUserLogoff(User *u)
	{
		if (Config->LimitSessions && (!u->server || !u->server->IsULined()))
			this->DelSession(u);
	}
};

MODULE_INIT(OSSession)
