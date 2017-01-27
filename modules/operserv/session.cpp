/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/operserv/session.h"

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
}

class ExceptionImpl : public Exception
{
	friend class ExceptionType;

	Serialize::Storage<Anope::string> mask, who, reason;
	Serialize::Storage<unsigned int> limit;
	Serialize::Storage<time_t> time, expires;

 public:
	using Exception::Exception;

	Anope::string GetMask() override;
	void SetMask(const Anope::string &) override;

	unsigned int GetLimit() override;
	void SetLimit(unsigned int) override;
	
	Anope::string GetWho() override;
	void SetWho(const Anope::string &) override;

	Anope::string GetReason() override;
	void SetReason(const Anope::string &) override;

	time_t GetTime() override;
	void SetTime(const time_t &) override;

	time_t GetExpires() override;
	void SetExpires(const time_t &) override;
};

class ExceptionType : public Serialize::Type<ExceptionImpl>
{
 public:
	Serialize::Field<ExceptionImpl, Anope::string> mask, who, reason;
	Serialize::Field<ExceptionImpl, unsigned int> limit;
	Serialize::Field<ExceptionImpl, time_t> time, expires;

	ExceptionType(Module *me) : Serialize::Type<ExceptionImpl>(me)
			, mask(this, "mask", &ExceptionImpl::mask)
			, who(this, "who", &ExceptionImpl::who)
			, reason(this, "reason", &ExceptionImpl::reason)
			, limit(this, "limit", &ExceptionImpl::limit)
			, time(this, "time", &ExceptionImpl::time)
			, expires(this, "expires", &ExceptionImpl::expires)
	{
	}
};

Anope::string ExceptionImpl::GetMask()
{
	return Get(&ExceptionType::mask);
}

void ExceptionImpl::SetMask(const Anope::string &m)
{
	Set(&ExceptionType::mask, m);
}

unsigned int ExceptionImpl::GetLimit()
{
	return Get(&ExceptionType::limit);
}

void ExceptionImpl::SetLimit(unsigned int l)
{
	Set(&ExceptionType::limit, l);
}

Anope::string ExceptionImpl::GetWho()
{
	return Get(&ExceptionType::who);
}

void ExceptionImpl::SetWho(const Anope::string &w)
{
	Set(&ExceptionType::who, w);
}

Anope::string ExceptionImpl::GetReason()
{
	return Get(&ExceptionType::reason);
}

void ExceptionImpl::SetReason(const Anope::string &r)
{
	Set(&ExceptionType::reason, r);
}

time_t ExceptionImpl::GetTime()
{
	return Get(&ExceptionType::time);
}

void ExceptionImpl::SetTime(const time_t &t)
{
	Set(&ExceptionType::time, t);
}

time_t ExceptionImpl::GetExpires()
{
	return Get(&ExceptionType::expires);
}

void ExceptionImpl::SetExpires(const time_t &e)
{
	Set(&ExceptionType::expires, e);
}

class MySessionService : public SessionService
{
	SessionMap Sessions;

 public:
	MySessionService(Module *m) : SessionService(m) { }

	Exception *FindException(User *u) override
	{
#warning "these need weighting"
		for (Exception *e : Serialize::GetObjects<Exception *>())
		{
			if (Anope::Match(u->host, e->GetMask()) || Anope::Match(u->ip.addr(), e->GetMask()))
				return e;
			
			if (cidr(e->GetMask()).match(u->ip))
				return e;
		}
		return nullptr;
	}

	Exception *FindException(const Anope::string &host) override
	{
		for (Exception *e : Serialize::GetObjects<Exception *>())
		{
			if (Anope::Match(host, e->GetMask()))
				return e;

			if (cidr(e->GetMask()).match(sockaddrs(host)))
				return e;
		}

		return nullptr;
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

class CommandOSSession : public Command
{
	ServiceReference<SessionService> session_service;
		
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
		{
			source.Reply(_("Invalid threshold value. It must be a valid integer greater than 1."));
			return;
		}

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

	void DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Anope::string param = params[1];
		Session *session = session_service->FindSession(param);

		Exception *e = session_service->FindException(param);
		Anope::string entry = "no entry";
		unsigned limit = session_limit;
		if (e)
		{
			if (!e->GetLimit())
				limit = 0;
			else if (e->GetLimit() > limit)
				limit = e->GetLimit();
			entry = e->GetMask();
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

		logger.Command(LogType::ADMIN, source, _("{source} used {command} {0} {1}"), cmd, params[1]);

		if (!session_limit)
			source.Reply(_("Session limiting is disabled."));
		else if (cmd.equals_ci("LIST"))
			this->DoList(source, params);
		else if (cmd.equals_ci("VIEW"))
			this->DoView(source, params);
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
		               source.GetCommand());
		return true;
	}
};

class CommandOSException : public Command
{
	static void DoDel(CommandSource &source, Exception *e)
	{
		EventManager::Get()->Dispatch(&Event::Exception::OnExceptionDel, source, e);
		e->Delete();
	}

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

			for (Exception *e : Serialize::GetObjects<Exception *>())
				if (e->GetMask().equals_ci(mask))
				{
					if (e->GetLimit() != limit)
					{
						e->SetLimit(limit);
						source.Reply(_("Exception for \002{0}\002 has been updated to \002{1}\002."), mask, e->GetLimit());
					}
					else
						source.Reply(_("\002{0}\002 already exists on the session-limit exception list."), mask);
					return;
				}

			Exception *e = Serialize::New<Exception *>();
			e->SetMask(mask);
			e->SetLimit(limit);
			e->SetReason(reason);
			e->SetTime(Anope::CurTime);
			e->SetWho(source.GetNick());
			e->SetExpires(expires);

			EventReturn MOD_RESULT;
			MOD_RESULT = EventManager::Get()->Dispatch(&Event::Exception::OnExceptionAdd, e);
			if (MOD_RESULT == EVENT_STOP) 
				return;

			logger.Command(LogType::ADMIN, source, _("{source} used {command} to set the session limit for {0} to {1}"), mask, limit);

			source.Reply(_("Session limit for \002{0}\002 set to \002{1}\002."), mask, limit);
			if (Anope::ReadOnly)
				source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
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
			unsigned int deleted = 0;

			NumberList(mask, true,
				[&](unsigned int number)
				{
					std::vector<Exception *> exceptions = Serialize::GetObjects<Exception *>();
					if (!number || number > exceptions.size())
						return;

					Exception *e = exceptions[number - 1];

					logger.Command(LogType::ADMIN, source, _("{source} used {command} to remove the session limit exception for {0}"), e->GetMask());

					++deleted;
					DoDel(source, e);
				},
				[&]()
				{
					if (!deleted)
						source.Reply(_("No matching entries on session-limit exception list."));
					else if (deleted == 1)
						source.Reply(_("Deleted \0021\002 entry from session-limit exception list."));
					else
						source.Reply(_("Deleted \002{0}\002 entries from session-limit exception list."), deleted);
				});
		}
		else
		{
			bool found = false;
			for (Exception *e : Serialize::GetObjects<Exception *>())
				if (mask.equals_ci(e->GetMask()))
				{
					logger.Command(LogType::ADMIN, source, _("{source} used {command} to remove the session limit exception for {0}"), e->GetMask());

					DoDel(source, e);
					source.Reply(_("\002{0}\002 deleted from session-limit exception list."), mask);
					found = true;
					break;
				}
			if (!found)
				source.Reply(_("\002{0}\002 not found on session-limit exception list."), mask);
		}

		if (Anope::ReadOnly)
			source.Reply(_("Services are in read-only mode. Any changes made may not persist."));
	}

	void ProcessList(CommandSource &source, const std::vector<Anope::string> &params, ListFormatter &list)
	{
		const Anope::string &mask = params.size() > 1 ? params[1] : "";
		std::vector<Exception *> exceptions = Serialize::GetObjects<Exception *>();

		if (exceptions.empty())
		{
			source.Reply(_("The session exception list is empty."));
			return;
		}

		if (!mask.empty() && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			NumberList(mask, false,
				[&](unsigned int number)
				{
					if (!number || number > exceptions.size())
						return;

					Exception *e = exceptions[number - 1];

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(number);
					entry["Mask"] = e->GetMask();
					entry["By"] = e->GetWho();
					entry["Created"] = Anope::strftime(e->GetTime(), NULL, true);
					entry["Expires"] = Anope::Expires(e->GetExpires(), source.GetAccount());
					entry["Limit"] = stringify(e->GetLimit());
					entry["Reason"] = e->GetReason();
					list.AddEntry(entry);
				},
				[]{});
		}
		else
		{
			unsigned int i = 0;
			for (Exception *e : exceptions)
			{
				if (mask.empty() || Anope::Match(e->GetMask(), mask))
				{
					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(++i);
					entry["Mask"] = e->GetMask();
					entry["By"] = e->GetWho();
					entry["Created"] = Anope::strftime(e->GetTime(), NULL, true);
					entry["Expires"] = Anope::Expires(e->GetExpires(), source.GetAccount());
					entry["Limit"] = stringify(e->GetLimit());
					entry["Reason"] = e->GetReason();
					list.AddEntry(entry);
				}
			}
		}

		if (list.IsEmpty())
		{
			source.Reply(_("No matching entries on session-limit exception list."));
			return;
		}

		source.Reply(_("Session-limit exception list:"));

		std::vector<Anope::string> replies;
		list.Process(replies);

		for (const Anope::string &r : replies)
			source.Reply(r);
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
			               source.GetCommand());
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
	MySessionService ss;
	CommandOSSession commandossession;
	CommandOSException commandosexception;
	ServiceReference<XLineManager> akills;
	ExceptionType etype;

 public:
	OSSession(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::UserConnect>(this, EventHook<Event::UserConnect>::Priority::FIRST)
		, EventHook<Event::UserQuit>(this, EventHook<Event::UserQuit>::Priority::FIRST)
		, EventHook<Event::ExpireTick>(this, EventHook<Event::ExpireTick>::Priority::FIRST)
		, ss(this)
		, commandossession(this)
		, commandosexception(this)
		, akills("xlinemanager/sgline")
		, etype(this)
	{
		this->SetPermanent(true);
	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *block = Config->GetModule(this);

		session_limit = block->Get<int>("defaultsessionlimit");
		max_session_kill = block->Get<int>("maxsessionkill");
		session_autokill_expiry = block->Get<time_t>("sessionautokillexpiry");
		sle_reason = block->Get<Anope::string>("sessionlimitexceeded");
		sle_detailsloc = block->Get<Anope::string>("sessionlimitdetailsloc");

		max_exception_limit = block->Get<int>("maxsessionlimit");
		exception_expiry = block->Get<time_t>("exceptionexpiry");

		ipv4_cidr = block->Get<unsigned>("session_ipv4_cidr", "32");
		ipv6_cidr = block->Get<unsigned>("session_ipv6_cidr", "128");

		if (ipv4_cidr > 32 || ipv6_cidr > 128)
			throw ConfigException(Module::name + ": session CIDR value out of range");
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
				Exception *e = this->ss.FindException(u);
				if (e)
				{
					kill = false;
					if (e->GetLimit() && session->count >= e->GetLimit())
						kill = true;
				}
			}

			++session->count;

			if (kill && !exempt)
			{
				ServiceBot *OperServ = Config->GetClient("OperServ");
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

				const Anope::string &akillmask = "*@" + session->addr.mask();
				if (max_session_kill && session->hits >= max_session_kill && akills && !akills->HasEntry(akillmask))
				{
					XLine *x = Serialize::New<XLine *>();
					x->SetMask(akillmask);
					x->SetBy(OperServ ? OperServ->nick : "");
					x->SetExpires(Anope::CurTime + session_autokill_expiry);
					x->SetReason("Session limit exceeded");
					x->SetID(XLineManager::GenerateUID());

					akills->AddXLine(x);
					akills->Send(NULL, x);

					logger.Bot(OperServ).Category("akill/session").Log(_("Added a temporary AKILL for \002{0}\002 due to excessive connections"), akillmask);
				}
				else
				{
					u->Kill(OperServ, "Session limit exceeded");
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

		for (Exception *e : Serialize::GetObjects<Exception *>())
		{
			if (!e->GetExpires() || e->GetExpires() > Anope::CurTime)
				continue;

			ServiceBot *OperServ = Config->GetClient("OperServ");
			logger.Bot(OperServ).Category("expire/exception").Log(_("Session exception for {0} has expired."), e->GetMask());
			e->Delete();
		}
	}
};

MODULE_INIT(OSSession)
