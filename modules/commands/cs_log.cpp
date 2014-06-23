/* ChanServ core functions
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
#include "modules/cs_log.h"
#include "modules/memoserv.h"

struct LogSettingImpl : LogSetting, Serializable
{
	LogSettingImpl() : Serializable("LogSetting")
	{
	}

	~LogSettingImpl()
	{
		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci)
		{
			LogSettings *ls = ci->GetExt<LogSettings>("logsettings");
			if (ls)
			{
				LogSettings::iterator it = std::find((*ls)->begin(), (*ls)->end(), this);
				if (it != (*ls)->end())
					(*ls)->erase(it);
			}
		}
	}

	void Serialize(Serialize::Data &data) const override
	{
		data["ci"] << chan;
		data["service_name"] << service_name;
		data["command_service"] << command_service;
		data["command_name"] << command_name;
		data["method"] << method;
		data["extra"] << extra;
		data["creator"] << creator;
		data.SetType("created", Serialize::Data::DT_INT); data["created"] << created;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &data)
	{
		Anope::string sci;
		data["ci"] >> sci;

		ChanServ::Channel *ci = ChanServ::Find(sci);
		if (ci == NULL)
			return NULL;

		LogSettingImpl *ls;
		if (obj)
			ls = anope_dynamic_static_cast<LogSettingImpl *>(obj);
		else
		{
			LogSettings *lsettings = ci->Require<LogSettings>("logsettings");
			ls = new LogSettingImpl();
			(*lsettings)->push_back(ls);
		}

		ls->chan = ci->name;
		data["service_name"] >> ls->service_name;
		data["command_service"] >> ls->command_service;
		data["command_name"] >> ls->command_name;
		data["method"] >> ls->method;
		data["extra"] >> ls->extra;
		data["creator"] >> ls->creator;
		data["created"] >> ls->created;

		return ls;
	}
};

struct LogSettingsImpl : LogSettings
{
	LogSettingsImpl(Extensible *) { }

 	~LogSettingsImpl()
	{
		for (iterator it = (*this)->begin(); it != (*this)->end();)
		{
			LogSetting *ls = *it;
			++it;
			delete ls;
		}
	}

	LogSetting *Create() override
	{
		return new LogSettingImpl();
	}
};

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
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
			return;
		}

		if (params.size() == 1)
		{
			LogSettings *ls = ci->Require<LogSettings>("logsettings");
			if (!ls || (*ls)->empty())
				source.Reply(_("There currently are no logging configurations for \002{0}\002."), ci->name);
			else
			{
				ListFormatter list(source.GetAccount());
				list.AddColumn(_("Number")).AddColumn(_("Service")).AddColumn(_("Command")).AddColumn(_("Method")).AddColumn("");

				for (unsigned i = 0; i < (*ls)->size(); ++i)
				{
					const LogSetting *log = (*ls)->at(i);

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(i + 1);
					entry["Service"] = log->command_service;
					entry["Command"] = !log->command_name.empty() ? log->command_name : log->service_name;
					entry["Method"] = log->method;
					entry[""] = log->extra;
					list.AddEntry(entry);
				}

				source.Reply(_("Log list for \002{0}\002:"), ci->name);

				std::vector<Anope::string> replies;
				list.Process(replies);

				for (unsigned i = 0; i < replies.size(); ++i)
					source.Reply(replies[i]);
			}
		}
		else if (params.size() > 2)
		{
			if (Anope::ReadOnly)
			{
				source.Reply(_("Services are in read-only mode."));
				return;
			}

			LogSettings *ls = ci->Require<LogSettings>("logsettings");
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
			BotInfo *bi = BotInfo::Find(service, true);

			Anope::string service_name;

			/* Allow either a command name or a service name. */
			if (bi && bi->commands.count(command_name))
			{
				/* Get service name from command */
				service_name = bi->commands[command_name].name;
			}
			else if (ServiceReference<Command>("Command", command.lower()))
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

			for (unsigned i = (*ls)->size(); i > 0; --i)
			{
				LogSetting *log = (*ls)->at(i - 1);

				if (log->service_name == service_name && log->method.equals_ci(method) && command_name.equals_ci(log->command_name))
				{
					if (log->extra == extra)
					{
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to remove logging for " << command << " with method " << method << (extra == "" ? "" : " ") << extra;
						source.Reply(_("Logging for command \002{0}\002 on \002{1}\002 with log method \002{2}{3}{4}\002 has been removed."), !log->command_name.empty() ? log->command_name : log->service_name, !log->command_service.empty() ? log->command_service : "any service", method, extra.empty() ? "" : " ", extra);
						delete log;
					}
					else
					{
						log->extra = extra;
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to change logging for " << command << " to method " << method << (extra == "" ? "" : " ") << extra;
						source.Reply(_("Logging changed for command \002{0}\002 on \002{1}\002, now using log method \002{2}{3}{4]\002."), !log->command_name.empty() ? log->command_name : log->service_name, !log->command_service.empty() ? log->command_service : "any service", method, extra.empty() ? "" : " ", extra);
					}
					return;
				}
			}

			LogSetting *log = new LogSettingImpl();
			log->chan = ci->name;
			log->service_name = service_name;
			if (bi)
				log->command_service = bi->nick;
			log->command_name = command_name;
			log->method = method;
			log->extra = extra;
			log->created = Anope::CurTime;
			log->creator = source.GetNick();

			(*ls)->push_back(log);

			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to log " << command << " with method " << method << (extra == "" ? "" : " ") << extra;

			source.Reply(_("Logging is now active for command \002{0}\002 on \002{1}\002, using log method \002{2}{3}{4}\002."), !command_name.empty() ? command_name : service_name, bi ? bi->nick : "any service", method, extra.empty() ? "" : " ", extra);
		}
		else
			this->OnSyntaxError(source, "");
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
	ExtensibleItem<LogSettingsImpl> logsettings;
	Serialize::Type logsetting_type;

	struct LogDefault
	{
		Anope::string service, command, method;
	};

	std::vector<LogDefault> defaults;

 public:
	CSLog(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ChanRegistered>("OnChanRegistered")
		, EventHook<Event::Log>("OnLog")
		, commandcslog(this)
		, logsettings(this, "logsettings")
		, logsetting_type("LogSetting", LogSettingImpl::Unserialize)
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

			ld.service = def->Get<const Anope::string>("service");
			ld.command = def->Get<const Anope::string>("command");
			ld.method = def->Get<const Anope::string>("method");

			defaults.push_back(ld);
		}
	}

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		if (defaults.empty())
			return;

		LogSettings *ls = logsettings.Require(ci);
		for (unsigned i = 0; i < defaults.size(); ++i)
		{
			LogDefault &d = defaults[i];

			LogSetting *log = new LogSettingImpl();
			log->chan = ci->name;

			if (!d.service.empty())
			{
				log->service_name = d.service.lower() + "/" + d.command.lower();
				log->command_service = d.service;
				log->command_name = d.command;
			}
			else
				log->service_name = d.command;

			spacesepstream sep(d.method);
			sep.GetToken(log->method);
			log->extra = sep.GetRemaining();

			log->created = Anope::CurTime;
			log->creator = ci->GetFounder() ? ci->GetFounder()->display : "(default)";

			(*ls)->push_back(log);
		}
	}

	void OnLog(::Log *l) override
	{
		if (l->type != LOG_COMMAND || l->u == NULL || l->c == NULL || l->ci == NULL || !Me || !Me->IsSynced())
			return;

		LogSettings *ls = logsettings.Get(l->ci);
		if (ls)
			for (unsigned i = 0; i < (*ls)->size(); ++i)
			{
				const LogSetting *log = (*ls)->at(i);

				/* wrong command */
				if (log->service_name != l->c->name)
					continue;

				/* if a command name is given check the service and the command */
				if (!log->command_name.empty())
				{
					/* wrong service (only check if not a fantasy command, though) */
					if (!l->source->c && log->command_service != l->source->service->nick)
						continue;

					if (!log->command_name.equals_ci(l->source->command))
						continue;
				}

				Anope::string buffer = l->u->nick + " used " + l->source->command.upper() + " " + l->buf.str();

				if (log->method.equals_ci("MEMO") && MemoServ::service && l->ci->WhoSends() != NULL)
					MemoServ::service->Send(l->ci->WhoSends()->nick, l->ci->name, buffer, true);
				else if (l->source->c)
					/* Sending a channel message or notice in response to a fantasy command */;
				else if (log->method.equals_ci("MESSAGE") && l->ci->c)
				{
					IRCD->SendPrivmsg(l->ci->WhoSends(), log->extra + l->ci->c->name, "%s", buffer.c_str());
					//l->ci->WhoSends()->lastmsg = Anope::CurTime; XXX
				}
				else if (log->method.equals_ci("NOTICE") && l->ci->c)
					IRCD->SendNotice(l->ci->WhoSends(), log->extra + l->ci->c->name, "%s", buffer.c_str());
			}
	}
};

MODULE_INIT(CSLog)
