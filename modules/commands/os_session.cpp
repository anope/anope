/* OperServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/os_session.h"

namespace
{
	/* The default session limit */
	unsigned session_limit;
	/* How many times to kill before adding an AKILL */
	unsigned max_session_kill;
	/* How long session akills should last */
	time_t session_autokill_expiry;
	/* Reason to use for session kills */
	Anope::string sle_reason;
	/* Optional second reason */
	Anope::string sle_detailsloc;

	/* Max limit that can be used for exceptions */
	unsigned max_exception_limit;
	/* How long before exceptions expire by default */
	time_t exception_expiry;

	/* Number of bits to use when comparing session IPs */
	unsigned ipv4_cidr;
	unsigned ipv6_cidr;

	EventHandlers<Event::Exception> *events;
}

class MySessionService : public SessionService
{
	SessionMap Sessions;
	Serialize::Checker<ExceptionVector> Exceptions;
 public:
	MySessionService(Module *m) : SessionService(m), Exceptions("Exception") { }

	Exception *CreateException() override
	{
		return new Exception();
	}

	void AddException(Exception *e) override
	{
		this->Exceptions->push_back(e);
	}

	void DelException(Exception *e) override
	{
		ExceptionVector::iterator it = std::find(this->Exceptions->begin(), this->Exceptions->end(), e);
		if (it != this->Exceptions->end())
			this->Exceptions->erase(it);
	}

	Exception *FindException(User *u) override
	{
		for (std::vector<Exception *>::const_iterator it = this->Exceptions->begin(), it_end = this->Exceptions->end(); it != it_end; ++it)
		{
			Exception *e = *it;
			if (Anope::Match(u->host, e->mask) || Anope::Match(u->ip.addr(), e->mask))
				return e;
			
			if (cidr(e->mask).match(u->ip))
				return e;
		}
		return NULL;
	}

	Exception *FindException(const Anope::string &host) override
	{
		for (std::vector<Exception *>::const_iterator it = this->Exceptions->begin(), it_end = this->Exceptions->end(); it != it_end; ++it)
		{
			Exception *e = *it;
			if (Anope::Match(host, e->mask))
				return e;

			if (cidr(e->mask).match(sockaddrs(host)))
				return e;
		}

		return NULL;
	}

	ExceptionVector &GetExceptions() override
	{
		return this->Exceptions;
	}

	void DelSession(Session *s)
	{
		this->Sessions.erase(s->addr);
	}

	Session *FindSession(const Anope::string &ip) override
	{
		cidr c(ip, ip.find(':') != Anope::string::npos ? ipv6_cidr : ipv4_cidr);
		if (!c.valid())
			return NULL;
		SessionMap::iterator it = this->Sessions.find(c);
		if (it != this->Sessions.end())
			return it->second;
		return NULL;
	}

	SessionMap::iterator FindSessionIterator(const sockaddrs &ip)
	{
		cidr c(ip, ip.ipv6() ? ipv6_cidr : ipv4_cidr);
		if (!c.valid())
			return this->Sessions.end();
		return this->Sessions.find(c);
	}

	Session* &FindOrCreateSession(const cidr &ip)
	{
		return this->Sessions[ip];
	}

	SessionMap &GetSessions() override
	{
		return this->Sessions;
	}
};

class ExceptionDelCallback : public NumberList
{
 protected:
	CommandSource &source;
	unsigned deleted;
	Command *cmd;
 public:
	ExceptionDelCallback(CommandSource &_source, const Anope::string &numlist, Command *c) : NumberList(numlist, true), source(_source), deleted(0), cmd(c)
	{
	}

	~ExceptionDelCallback()
	{
		if (!deleted)
			source.Reply(_("No matching entries on session-limit exception list."));
		else if (deleted == 1)
			source.Reply(_("Deleted \0021\002 entry from session-limit exception list."));
		else
			source.Reply(_("Deleted \002{0}\002 entries from session-limit exception list."), deleted);
	}

	virtual void HandleNumber(unsigned number) override
	{
		if (!number || number > session_service->GetExceptions().size())
			return;

		Log(LOG_ADMIN, source, cmd) << "to remove the session limit exception for " << session_service->GetExceptions()[number - 1]->mask;

		++deleted;
		DoDel(source, number - 1);
	}

	static void DoDel(CommandSource &source, unsigned index)
	{
		Exception *e = session_service->GetExceptions()[index];
		(*events)(&Event::Exception::OnExceptionDel, source, e);

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
			ListFormatter list(source.GetAccount());
			list.AddColumn(_("Session")).AddColumn(_("Host"));

			for (SessionService::SessionMap::iterator it = session_service->GetSessions().begin(), it_end = session_service->GetSessions().end(); it != it_end; ++it)
			{
				Session *session = it->second;

				if (session->count >= mincount)
				{
					ListFormatter::ListEntry entry;
					entry["Session"] = stringify(session->count);
					entry["Host"] = session->addr.mask();
					list.AddEntry(entry);
				}
			}

			source.Reply(_("Hosts with at least \002{0}\002 sessions:"), mincount);

			std::vector<Anope::string> replies;
			list.Process(replies);


			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
	}

	void DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Anope::string param = params[1];
		Session *session = session_service->FindSession(param);

		Exception *exception = session_service->FindException(param);
		Anope::string entry = "no entry";
		unsigned limit = session_limit;
		if (exception)
		{
			if (!exception->limit)
				limit = 0;
			else if (exception->limit > limit)
				limit = exception->limit;
			entry = exception->mask;
		}

		if (!session)
			source.Reply(_("\002{0}\002 not found on the session list, but has a limit of \002{1}\002 because it matches entry: \002{2}\002."), param, limit, entry);
		else
			source.Reply(_("The host \002{0}\002 currently has \002{1}\002 sessions with a limit of \002{2}\002 because it matches entry: \002{3}\002."), session->addr.mask(), session->count, limit, entry);
	}
 public:
	CommandOSSession(Module *creator) : Command(creator, "operserv/session", 2, 2)
	{
		this->SetDesc(_("View the list of host sessions"));
		this->SetSyntax(_("LIST \037threshold\037"));
		this->SetSyntax(_("VIEW \037host\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];

		Log(LOG_ADMIN, source, this) << cmd << " " << params[1];

		if (!session_limit)
			source.Reply(_("Session limiting is disabled."));
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, params);
		else if (cmd.equals_ci("VIEW"))
			return this->DoView(source, params);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows Services Operators to view the session list.\n"
		               "\n"
		               "\002{0} LIST\002 lists hosts with at least \037threshold\037 sessions. The threshold must be a number greater than 1.\n"
		               "\n"
		               "\002{0} VIEW\002 displays detailed information about a specific host - including the current session count and session limit.\n"
		               "\n"
		               "See the \002EXCEPTION\002 help for more information about session limiting and how to set session limits specific to certain hosts and groups thereof."), // XXX
		               source.command);
		return true;
	}
};

class CommandOSException : public Command
{
 private:
	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
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

		time_t expires = !expiry.empty() ? Anope::DoTime(expiry) : exception_expiry;
		if (expires < 0)
		{
			source.Reply(_("Invalid expiry time \002{0}\002."), expiry);
			return;
		}
		else if (expires > 0)
			expires += Anope::CurTime;

		unsigned limit = -1;
		try
		{
			limit = convertTo<unsigned>(limitstr);
		}
		catch (const ConvertException &) { }

		if (limit > max_exception_limit)
		{
			source.Reply(_("Invalid session limit. It must be a valid integer greater than or equal to zero and less than \002{0}\002."), max_exception_limit);
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
						source.Reply(_("Exception for \002{0}\002 has been updated to \002{1}\002."), mask, e->limit);
					}
					else
						source.Reply(_("\002{0}\002 already exists on the session-limit exception list."), mask);
					return;
				}
			}

			Exception *exception = new Exception();
			exception->mask = mask;
			exception->limit = limit;
			exception->reason = reason;
			exception->time = Anope::CurTime;
			exception->who = source.GetNick();
			exception->expires = expires;

			EventReturn MOD_RESULT;
			MOD_RESULT = (*events)(&Event::Exception::OnExceptionAdd, exception);
			if (MOD_RESULT == EVENT_STOP)
				delete exception;
			else
			{
				Log(LOG_ADMIN, source, this) << "to set the session limit for " << mask << " to " << limit;
				session_service->AddException(exception);
				source.Reply(_("Session limit for \002{0}\002 set to \002{1}\002."), mask, limit);
				if (Anope::ReadOnly)
					source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
			}
		}
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
			ExceptionDelCallback list(source, mask, this);
			list.Process();
		}
		else
		{
			unsigned i = 0, end = session_service->GetExceptions().size();
			for (; i < end; ++i)
				if (mask.equals_ci(session_service->GetExceptions()[i]->mask))
				{
					Log(LOG_ADMIN, source, this) << "to remove the session limit exception for " << mask;
					ExceptionDelCallback::DoDel(source, i);
					source.Reply(_("\002{0}\002 deleted from session-limit exception list."), mask);
					break;
				}
			if (i == end)
				source.Reply(_("\002{0}\002 not found on session-limit exception list."), mask);
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
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

		if (n1 >= 0 && static_cast<unsigned>(n1) < session_service->GetExceptions().size() && n2 >= 0 && static_cast<unsigned>(n2) < session_service->GetExceptions().size() && n1 != n2)
		{
			Exception *temp = session_service->GetExceptions()[n1];
			session_service->GetExceptions()[n1] = session_service->GetExceptions()[n2];
			session_service->GetExceptions()[n2] = temp;

			Log(LOG_ADMIN, source, this) << "to move exception " << session_service->GetExceptions()[n1]->mask << " from position " << n1 + 1 << " to position " << n2 + 1;
			source.Reply(_("Exception for \002{0}\002 (#{1}) moved to position \002{2}\002."), session_service->GetExceptions()[n1]->mask, n1 + 1, n2 + 1);

			if (Anope::ReadOnly)
				source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
		}
		else
			this->OnSyntaxError(source, "MOVE");
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
				CommandSource &source;
				ListFormatter &list;
			 public:
				ExceptionListCallback(CommandSource &_source, ListFormatter &_list, const Anope::string &numlist) : NumberList(numlist, false), source(_source), list(_list)
				{
				}

				void HandleNumber(unsigned Number) override
				{
					if (!Number || Number > session_service->GetExceptions().size())
						return;

					Exception *e = session_service->GetExceptions()[Number - 1];

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(Number);
					entry["Mask"] = e->mask;
					entry["By"] = e->who;
					entry["Created"] = Anope::strftime(e->time, NULL, true);
					entry["Expires"] = Anope::Expires(e->expires, source.GetAccount());
					entry["Limit"] = stringify(e->limit);
					entry["Reason"] = e->reason;
					this->list.AddEntry(entry);
				}
			}
			nl_list(source, list, mask);
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
					entry["Created"] = Anope::strftime(e->time, NULL, true);
					entry["Expires"] = Anope::Expires(e->expires, source.GetAccount());
					entry["Limit"] = stringify(e->limit);
					entry["Reason"] = e->reason;
					list.AddEntry(entry);
				}
			}
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on session-limit exception list."));
		else
		{
			source.Reply(_("Session-limit exception list:"));

			std::vector<Anope::string> replies;
			list.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
	}

	void DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Limit")).AddColumn(_("Mask"));

		this->ProcessList(source, params, list);
	}

	void DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ListFormatter list(source.GetAccount());
		list.AddColumn(_("Number")).AddColumn(_("Mask")).AddColumn(_("By")).AddColumn(_("Created")).AddColumn(_("Expires")).AddColumn(_("Limit")).AddColumn(_("Reason"));

		this->ProcessList(source, params, list);
	}

 public:
	CommandOSException(Module *creator) : Command(creator, "operserv/exception", 1, 5)
	{
		this->SetDesc(_("Modify the session-limit exception list"));
		this->SetSyntax(_("ADD [\037+expiry\037] \037mask\037 \037limit\037 \037reason\037"));
		this->SetSyntax(_("DEL {\037mask\037 | \037entry-num\037 | \037list\037}"));
		this->SetSyntax(_("MOVE \037num\037 \037position\037"));
		this->SetSyntax(_("LIST [\037mask\037 | \037list\037]"));
		this->SetSyntax(_("VIEW [\037mask\037 | \037list\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];

		if (!session_limit)
			source.Reply(_("Session limiting is disabled."));
		else if (cmd.equals_ci("ADD"))
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
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.equals_ci("ADD"))
			source.Reply(_("\002{0} ADD\002 adds the given hostmask to the exception list."
			               " Note that \002nick!user@host\002 and \002user@host\002 masks are invalid!\n"
			               " Only real host masks, such as \002box.host.dom\002 and \002*.host.dom\002, are allowed because sessions limiting does not take nickname or user names into account."
			               " \037limit\037 must be a number greater than or equal to zero."
			               " This determines how many sessions this host may carry at a time."
			               " A value of zero means the host has an unlimited session limit."
			               " If more than one entry matches a client, the first matching enty will be used."),
			               source.command);
		else
			source.Reply(_("Allows you to manipulate the list of hosts that have specific session limits - allowing certain machines, such as shell servers, to carry more than the default number of clients at a time."
			               " Once a host reaches its session limit, all clients attempting to connect from that host will be killed."
			               " Before the user is killed, they are notified, of a source of help regarding session limiting. The content of this notice is a config setting.\n"
			               "\n"
			               "\002{0} ADD\002 adds the given host mask to the exception list.\n"
			               "\002{msg}{service} {help} {command} ADD\002 for more information.\n"
			               "\n"
			               "\002{0} DEL\002 removes the given mask from the exception list.\n"
			               "\n"
			               "\002{0} MOVE\002 moves exception \037num\037 to \037position\037."
			               " The sessions inbetween will be shifted up or down to fill the gap.\n"
			               "\n"
			               "\002{0} LIST\002 and \002{0} VIEW\002 show all current sessions if the optional mask is given, the list is limited to those sessions matching the mask."
			               " The difference is that \002{0} VIEW\002 is more verbose, displaying the name of the person who added the exception, its session limit, reason, host mask and the expiry date and time.\n"));
		return true;
	}
};

class OSSession : public Module
	, public EventHook<Event::UserConnect>
	, public EventHook<Event::UserQuit>
	, public EventHook<Event::ExpireTick>
{
	Serialize::Type exception_type;
	MySessionService ss;
	CommandOSSession commandossession;
	CommandOSException commandosexception;
	ServiceReference<XLineManager> akills;
	EventHandlers<Event::Exception> exceptionevents;

 public:
	OSSession(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::UserConnect>("OnUserConnect", EventHook<Event::UserConnect>::Priority::FIRST)
		, EventHook<Event::UserQuit>("OnUserQuit", EventHook<Event::UserQuit>::Priority::FIRST)
		, EventHook<Event::ExpireTick>("OnExpireTick", EventHook<Event::ExpireTick>::Priority::FIRST)
		, exception_type("Exception", Exception::Unserialize)
		, ss(this)
		, commandossession(this)
		, commandosexception(this)
		, akills("XLineManager", "xlinemanager/sgline")
		, exceptionevents(this, "Exception")
	{
		this->SetPermanent(true);

		events = &exceptionevents;
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *block = Config->GetModule(this);

		session_limit = block->Get<int>("defaultsessionlimit");
		max_session_kill = block->Get<int>("maxsessionkill");
		session_autokill_expiry = block->Get<time_t>("sessionautokillexpiry");
		sle_reason = block->Get<const Anope::string>("sessionlimitexceeded");
		sle_detailsloc = block->Get<const Anope::string>("sessionlimitdetailsloc");

		max_exception_limit = block->Get<int>("maxsessionlimit");
		exception_expiry = block->Get<time_t>("exceptionexpiry");

		ipv4_cidr = block->Get<unsigned>("session_ipv4_cidr", "32");
		ipv6_cidr = block->Get<unsigned>("session_ipv6_cidr", "128");

		if (ipv4_cidr > 32 || ipv6_cidr > 128)
			throw ConfigException(this->name + ": session CIDR value out of range");
	}

	void OnUserConnect(User *u, bool &exempt) override
	{
		if (u->Quitting() || !session_limit || exempt || !u->server || u->server->IsULined())
			return;

		cidr u_ip(u->ip, u->ip.ipv6() ? ipv6_cidr : ipv4_cidr);
		if (!u_ip.valid())
			return;

		Session* &session = this->ss.FindOrCreateSession(u_ip);

		if (session)
		{
			bool kill = false;
			if (session->count >= session_limit)
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

			++session->count;

			if (kill && !exempt)
			{
				BotInfo *OperServ = Config->GetClient("OperServ");
				if (OperServ)
				{
					if (!sle_reason.empty())
					{
						Anope::string message = sle_reason.replace_all_cs("%IP%", u->ip.addr());
						u->SendMessage(OperServ, message);
					}
					if (!sle_detailsloc.empty())
						u->SendMessage(OperServ, sle_detailsloc);
				}

				++session->hits;
				if (max_session_kill && session->hits >= max_session_kill && akills)
				{
					const Anope::string &akillmask = "*@" + session->addr.mask();
					XLine *x = new XLine(akillmask, OperServ ? OperServ->nick : "", Anope::CurTime + session_autokill_expiry, "Session limit exceeded", XLineManager::GenerateUID());
					akills->AddXLine(x);
					akills->Send(NULL, x);
					Log(OperServ, "akill/session") << "Added a temporary AKILL for \002" << akillmask << "\002 due to excessive connections";
				}
				else
				{
					u->Kill(OperServ ? OperServ->nick : "", "Session limit exceeded");
				}
			}
		}
		else
		{
			session = new Session(u->ip, u->ip.ipv6() ? ipv6_cidr : ipv4_cidr);
		}
	}

	void OnUserQuit(User *u, const Anope::string &msg) override
	{
		if (!session_limit || !u->server || u->server->IsULined())
			return;

		SessionService::SessionMap &sessions = this->ss.GetSessions();
		SessionService::SessionMap::iterator sit = this->ss.FindSessionIterator(u->ip);

		if (sit == sessions.end())
			return;

		Session *session = sit->second;

		if (session->count > 1)
		{
			--session->count;
			return;
		}

		delete session;
		sessions.erase(sit);
	}

	void OnExpireTick() override
	{
		if (Anope::NoExpire)
			return;
		for (unsigned i = this->ss.GetExceptions().size(); i > 0; --i)
		{
			Exception *e = this->ss.GetExceptions()[i - 1];

			if (!e->expires || e->expires > Anope::CurTime)
				continue;
			BotInfo *OperServ = Config->GetClient("OperServ");
			Log(OperServ, "expire/exception") << "Session exception for " << e->mask << " has expired.";
			this->ss.DelException(e);
			delete e;
		}
	}
};

MODULE_INIT(OSSession)
