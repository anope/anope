/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
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
#include "modules/chanserv/log.h"
#include "modules/memoserv.h"

class LogSettingImpl : public LogSetting
{
	friend class LogSettingType;

	ChanServ::Channel *channel = nullptr;
	Anope::string service_name, command_service, command_name, method, extra, creator;
	time_t created = 0;

 public:
	LogSettingImpl(Serialize::TypeBase *type) : LogSetting(type) { }
	LogSettingImpl(Serialize::TypeBase *type, Serialize::ID id) : LogSetting(type, id) { }

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *) override;

	Anope::string GetServiceName() override;
	void SetServiceName(const Anope::string &) override;

	Anope::string GetCommandService() override;
	void SetCommandService(const Anope::string &) override;

	Anope::string GetCommandName() override;
	void SetCommandName(const Anope::string &) override;

	Anope::string GetMethod() override;
	virtual void SetMethod(const Anope::string &) override;

	Anope::string GetExtra() override;
	void SetExtra(const Anope::string &) override;

	Anope::string GetCreator() override;
	void SetCreator(const Anope::string &) override;

	time_t GetCreated() override;
	void SetCreated(const time_t &) override;
};

class LogSettingType : public Serialize::Type<LogSettingImpl>
{
 public:
	Serialize::ObjectField<LogSettingImpl, ChanServ::Channel *> channel;
	Serialize::Field<LogSettingImpl, Anope::string> service_name, command_service, command_name, method, extra, creator;
	Serialize::Field<LogSettingImpl, time_t> created;

	LogSettingType(Module *me) : Serialize::Type<LogSettingImpl>(me)
		, channel(this, "channel", &LogSettingImpl::channel, true)
		, service_name(this, "service_name", &LogSettingImpl::service_name)
		, command_service(this, "command_service", &LogSettingImpl::command_service)
		, command_name(this, "command_name", &LogSettingImpl::command_name)
		, method(this, "method", &LogSettingImpl::method)
		, extra(this, "extra", &LogSettingImpl::extra)
		, creator(this, "creator", &LogSettingImpl::creator)
		, created(this, "created", &LogSettingImpl::created)
	{
	}
};

ChanServ::Channel *LogSettingImpl::GetChannel()
{
	return Get(&LogSettingType::channel);
}

void LogSettingImpl::SetChannel(ChanServ::Channel *ci)
{
	Set(&LogSettingType::channel, ci);
}

Anope::string LogSettingImpl::GetServiceName()
{
	return Get(&LogSettingType::service_name);
}

void LogSettingImpl::SetServiceName(const Anope::string &s)
{
	Set(&LogSettingType::service_name, s);
}

Anope::string LogSettingImpl::GetCommandService()
{
	return Get(&LogSettingType::command_service);
}

void LogSettingImpl::SetCommandService(const Anope::string &s)
{
	Set(&LogSettingType::command_service, s);
}

Anope::string LogSettingImpl::GetCommandName()
{
	return Get(&LogSettingType::command_name);
}

void LogSettingImpl::SetCommandName(const Anope::string &s)
{
	Set(&LogSettingType::command_name, s);
}

Anope::string LogSettingImpl::GetMethod()
{
	return Get(&LogSettingType::method);
}

void LogSettingImpl::SetMethod(const Anope::string &m)
{
	Set(&LogSettingType::method, m);
}

Anope::string LogSettingImpl::GetExtra()
{
	return Get(&LogSettingType::extra);
}

void LogSettingImpl::SetExtra(const Anope::string &e)
{
	Set(&LogSettingType::extra, e);
}

Anope::string LogSettingImpl::GetCreator()
{
	return Get(&LogSettingType::creator);
}

void LogSettingImpl::SetCreator(const Anope::string &creator)
{
	Set(&LogSettingType::extra, creator);
}

time_t LogSettingImpl::GetCreated()
{
	return Get(&LogSettingType::created);
}

void LogSettingImpl::SetCreated(const time_t &t)
{
	Set(&LogSettingType::created, t);
}

class CommandCSLog : public Command
{
public:
	CommandCSLog(Module *creator) : Command(creator, "chanserv/log", 1, 4)
	{
		this->SetDesc(_("Configures channel logging settings"));
		this->SetSyntax(_("\037channel\037"));
		this->SetSyntax(_("\037channel\037 \037command\037 \037method\037 [\037status\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &channel = params[0];

		ChanServ::Channel *ci = ChanServ::Find(channel);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), channel);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("SET") && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (params.size() == 1)
		{
			std::vector<LogSetting *> ls = ci->GetRefs<LogSetting *>();
			if (ls.empty())
			{
				source.Reply(_("There currently are no logging configurations for \002{0}\002."), ci->GetName());
				return;
			}

			ListFormatter list(source.GetAccount());
			list.AddColumn(_("Number")).AddColumn(_("Service")).AddColumn(_("Command")).AddColumn(_("Method")).AddColumn("");

			for (unsigned i = 0; i < ls.size(); ++i)
			{
				LogSetting *log = ls[i];

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				entry["Service"] = log->GetCommandService();
				entry["Command"] = !log->GetCommandName().empty() ? log->GetCommandName() : log->GetServiceName();
				entry["Method"] = log->GetMethod();
				entry[""] = log->GetExtra();
				list.AddEntry(entry);
			}

			source.Reply(_("Log list for \002{0}\002:"), ci->GetName());

			std::vector<Anope::string> replies;
			list.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
		else if (params.size() > 2)
		{
			if (Anope::ReadOnly)
			{
				source.Reply(_("Services are in read-only mode."));
				return;
			}

			const Anope::string &command = params[1];
			const Anope::string &method = params[2];
			const Anope::string &extra = params.size() > 3 ? params[3] : "";

			size_t sl = command.find('/');
			if (sl == Anope::string::npos)
			{
				source.Reply(_("\002{0}\002 is not a valid command."), command);
				return;
			}

			Anope::string service = command.substr(0, sl),
				command_name = command.substr(sl + 1);
			ServiceBot *bi = ServiceBot::Find(service, true);

			Anope::string service_name;

			/* Allow either a command name or a service name. */
			if (bi && bi->commands.count(command_name))
			{
				/* Get service name from command */
				service_name = bi->commands[command_name].name;
			}
			else if (ServiceReference<Command>(command.lower()))
			{
				/* This is the service name, don't use any specific command */
				service_name = command;
				bi = NULL;
				command_name.clear();
			}
			else
			{
				source.Reply(_("\002{0}\002 is not a valid command."), command);
				return;
			}

			if (!method.equals_ci("MESSAGE") && !method.equals_ci("NOTICE") && !method.equals_ci("MEMO"))
			{
				source.Reply(_("\002%s\002 is not a valid logging method."));
				return;
			}

			for (unsigned i = 0; i < extra.length(); ++i)
				if (ModeManager::GetStatusChar(extra[i]) == 0)
				{
					source.Reply(_("\002%c\002 is an unknown status mode."), extra[i]);
					return;
				}

			bool override = !source.AccessFor(ci).HasPriv("SET");

			std::vector<LogSetting *> ls = ci->GetRefs<LogSetting *>();
			for (unsigned i = ls.size(); i > 0; --i)
			{
				LogSetting *log = ls[i - 1];

				if (log->GetServiceName() == service_name && log->GetMethod().equals_ci(method) && command_name.equals_ci(log->GetCommandName()))
				{
					if (log->GetExtra() == extra)
					{
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to remove logging for " << command << " with method " << method << (extra == "" ? "" : " ") << extra;
						source.Reply(_("Logging for command \002{0}\002 on \002{1}\002 with log method \002{2}{3}{4}\002 has been removed."), !log->GetCommandName().empty() ? log->GetCommandName() : log->GetServiceName(), !log->GetCommandService().empty() ? log->GetCommandService() : "any service", method, extra.empty() ? "" : " ", extra);
						delete log;
					}
					else
					{
						log->SetExtra(extra);
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to change logging for " << command << " to method " << method << (extra == "" ? "" : " ") << extra;
						source.Reply(_("Logging changed for command \002{0}\002 on \002{1}\002, now using log method \002{2}{3}{4]\002."), !log->GetCommandName().empty() ? log->GetCommandName() : log->GetServiceName(), !log->GetCommandService().empty() ? log->GetCommandService() : "any service", method, extra.empty() ? "" : " ", extra);
					}
					return;
				}
			}

			LogSetting *log = Serialize::New<LogSetting *>();
			log->SetChannel(ci);
			log->SetServiceName(service_name);
			if (bi)
				log->SetCommandService(bi->nick);
			log->SetCommandName(command_name);
			log->SetMethod(method);
			log->SetExtra(extra);
			log->SetCreated(Anope::CurTime);
			log->SetCreator(source.GetNick());

			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to log " << command << " with method " << method << (extra == "" ? "" : " ") << extra;

			source.Reply(_("Logging is now active for command \002{0}\002 on \002{1}\002, using log method \002{2}{3}{4}\002."), !command_name.empty() ? command_name : service_name, bi ? bi->nick : "any service", method, extra.empty() ? "" : " ", extra);
		}
		else
		{
			this->OnSyntaxError(source, "");
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("The {0} command allows users to configure logging settings for \037channel\037."
		               " If no parameters are given this command lists the current logging methods in place on \037channel\037."
		               " Otherwise, \037command\037 must be a command name, and \037method\037 must be one of the following logging methods:\n"
		               "\n"
		               " MESSAGE [status], NOTICE [status], MEMO\n"
		               "\n"
		               "Which are used to message, notice, and memo the channel respectively."
		               " With MESSAGE or NOTICE you must have a service bot assigned to and joined to your channel."
		               " Status may be a channel status such as @ or +.\n"
	                       "\n"
		               "To remove a logging method use the same syntax as you would to add it.\n"
		               "\n"
		               "Use of this command requires the \002{1}\002 privilege on \037channel\037."
		               "\n"
		               "Example:\n"
		               "         {command} #anope chanserv/access MESSAGE @\n"
		               "         Would message any channel operators of \"#anope\" whenever someone used the \"ACCESS\" command on ChanServ for \"#anope\"."),
		               source.command, "SET");
		return true;
	}
};

class CSLog : public Module
	, public EventHook<Event::ChanRegistered>
	, public EventHook<Event::Log>
{
	CommandCSLog commandcslog;
	LogSettingType logtype;
	ServiceReference<MemoServ::MemoServService> memoserv;

	struct LogDefault
	{
		Anope::string service, command, method;
	};

	std::vector<LogDefault> defaults;

 public:
	CSLog(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ChanRegistered>(this)
		, EventHook<Event::Log>(this)
		, commandcslog(this)
		, logtype(this)
	{

	}

	void OnReload(Configuration::Conf *conf) override
	{
		Configuration::Block *block = conf->GetModule(this);
		defaults.clear();

		for (int i = 0; i < block->CountBlock("default"); ++i)
		{
			Configuration::Block *def = block->GetBlock("default", i);

			LogDefault ld;

			ld.service = def->Get<Anope::string>("service");
			ld.command = def->Get<Anope::string>("command");
			ld.method = def->Get<Anope::string>("method");

			defaults.push_back(ld);
		}
	}

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		if (defaults.empty())
			return;

		for (unsigned i = 0; i < defaults.size(); ++i)
		{
			LogDefault &d = defaults[i];

			LogSetting *log = Serialize::New<LogSetting *>();
			log->SetChannel(ci);

			if (!d.service.empty())
			{
				log->SetServiceName(d.service.lower() + "/" + d.command.lower());
				log->SetCommandService(d.service);
				log->SetCommandName(d.command);
			}
			else
			{
				log->SetServiceName(d.command);
			}

			spacesepstream sep(d.method);
			Anope::string method, extra;
			sep.GetToken(method);
			extra = sep.GetRemaining();

			log->SetMethod(method);
			log->SetExtra(extra);
			log->SetCreated(Anope::CurTime);
			log->SetCreator(ci->GetFounder() ? ci->GetFounder()->GetDisplay() : "(default)");
		}
	}

	void OnLog(::Log *l) override
	{
		if (l->type != LOG_COMMAND || l->u == NULL || l->c == NULL || l->ci == NULL || !Me || !Me->IsSynced())
			return;

		std::vector<LogSetting *> ls = l->ci->GetRefs<LogSetting *>();
		for (unsigned i = 0; i < ls.size(); ++i)
		{
			LogSetting *log = ls[i];

			/* wrong command */
			if (log->GetServiceName() != l->c->GetName())
				continue;

			/* if a command name is given check the service and the command */
			if (!log->GetCommandName().empty())
			{
				/* wrong service (only check if not a fantasy command, though) */
				if (!l->source->c && log->GetCommandService() != l->source->service->nick)
					continue;

				if (!log->GetCommandName().equals_ci(l->source->command))
					continue;
			}

			Anope::string buffer = l->u->nick + " used " + l->source->command.upper() + " " + l->buf.str();

			if (log->GetMethod().equals_ci("MEMO") && memoserv && l->ci->WhoSends() != NULL)
				memoserv->Send(l->ci->WhoSends()->nick, l->ci->GetName(), buffer, true);
			else if (l->source->c)
				/* Sending a channel message or notice in response to a fantasy command */;
			else if (log->GetMethod().equals_ci("MESSAGE") && l->ci->c)
			{
				IRCD->SendPrivmsg(l->ci->WhoSends(), log->GetExtra() + l->ci->c->name, "%s", buffer.c_str());
#warning "fix idletimes"
				//l->ci->WhoSends()->lastmsg = Anope::CurTime;
			}
			else if (log->GetMethod().equals_ci("NOTICE") && l->ci->c)
				IRCD->SendNotice(l->ci->WhoSends(), log->GetExtra() + l->ci->c->name, "%s", buffer.c_str());
		}
	}
};

MODULE_INIT(CSLog)
