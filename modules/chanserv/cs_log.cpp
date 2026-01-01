// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"
#include "modules/chanserv/log.h"
#include "modules/memoserv/service.h"

struct LogSettingImpl final
	: ChanServ::LogSetting
	, Serializable
{
	LogSettingImpl()
		: Serializable(CHANSERV_LOG_SETTING_TYPE)
	{
	}

	~LogSettingImpl() override
	{
		ChannelInfo *ci = ChannelInfo::Find(chan);
		if (ci)
		{
			auto *ls = ci->GetExt<ChanServ::LogSettings>(CHANSERV_LOG_SETTING_EXT);
			if (ls)
			{
				auto it = std::find((*ls)->begin(), (*ls)->end(), this);
				if (it != (*ls)->end())
					(*ls)->erase(it);
			}
		}
	}
};

struct LogSettingTypeImpl final
	: Serialize::Type
{
	LogSettingTypeImpl()
		: Serialize::Type(CHANSERV_LOG_SETTING_TYPE)
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *ls = static_cast<const LogSettingImpl *>(obj);
		data.Store("ci", ls->chan);
		data.Store("service_name", ls->service_name);
		data.Store("command_service", ls->command_service);
		data.Store("command_name", ls->command_name);
		data.Store("method", ls->method);
		data.Store("extra", ls->extra);
		data.Store("creator", ls->creator);
		data.Store("created", ls->created);
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		Anope::string sci;
		data["ci"] >> sci;

		ChannelInfo *ci = ChannelInfo::Find(sci);
		if (ci == NULL)
			return NULL;

		LogSettingImpl *ls;
		if (obj)
			ls = anope_dynamic_static_cast<LogSettingImpl *>(obj);
		else
		{
			auto *lsettings = ci->Require<ChanServ::LogSettings>(CHANSERV_LOG_SETTING_EXT);
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

struct LogSettingsImpl final
	: ChanServ::LogSettings
{
	LogSettingsImpl(Extensible *) { }

	~LogSettingsImpl() override
	{
		for (auto it = (*this)->begin(); it != (*this)->end();)
		{
			auto *ls = *it;
			++it;
			delete ls;
		}
	}

	ChanServ::LogSetting *Create() override
	{
		return new LogSettingImpl();
	}
};

class CommandCSLog final
	: public Command
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

		ChannelInfo *ci = ChannelInfo::Find(channel);
		if (ci == NULL)
			source.Reply(CHAN_X_NOT_REGISTERED, channel.c_str());
		else if (!source.AccessFor(ci).HasPriv("SET") && !source.HasPriv("chanserv/administration"))
			source.Reply(ACCESS_DENIED);
		else if (params.size() == 1)
		{
			auto *ls = ci->Require<ChanServ::LogSettings>(CHANSERV_LOG_SETTING_EXT);
			if (!ls || (*ls)->empty())
				source.Reply(_("There currently are no logging configurations for %s."), ci->name.c_str());
			else
			{
				ListFormatter list(source.GetAccount());
				list.AddColumn(_("Number")).AddColumn(_("Service")).AddColumn(_("Command")).AddColumn(_("Method"));
				list.SetFlexible(_("{number}: {command} on {service}: {method}"));

				for (unsigned i = 0; i < (*ls)->size(); ++i)
				{
					const auto *log = (*ls)->at(i);

					ListFormatter::ListEntry entry;
					entry["Number"] = Anope::ToString(i + 1);
					entry["Service"] = log->command_service;
					entry["Command"] = !log->command_name.empty() ? log->command_name.upper() : log->service_name;
					entry["Method"] = log->method + (log->extra.empty() ? "" : " ") + log->extra;
					list.AddEntry(entry);
				}

				source.Reply(_("Log list for %s:"), ci->name.c_str());
				list.SendTo(source);
			}
		}
		else if (params.size() > 2)
		{
			if (Anope::ReadOnly)
			{
				source.Reply(READ_ONLY_MODE);
				return;
			}

			auto *ls = ci->Require<ChanServ::LogSettings>(CHANSERV_LOG_SETTING_EXT);
			const Anope::string &command = params[1];
			const Anope::string &method = params[2];
			const Anope::string &extra = params.size() > 3 ? params[3] : "";

			size_t sl = command.find('/');
			if (sl == Anope::string::npos)
			{
				source.Reply(_("%s is not a valid command."), command.c_str());
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
				source.Reply(_("%s is not a valid command."), command.c_str());
				return;
			}

			if (!method.equals_ci("MESSAGE") && !method.equals_ci("NOTICE") && !method.equals_ci("MEMO"))
			{
				source.Reply(_("%s is not a valid logging method."), method.c_str());
				return;
			}

			for (auto chr : extra)
			{
				if (ModeManager::GetStatusChar(chr) == 0)
				{
					source.Reply(_("%c is an unknown status mode."), chr);
					return;
				}
			}

			bool override = !source.AccessFor(ci).HasPriv("SET");

			for (unsigned i = (*ls)->size(); i > 0; --i)
			{
				auto *log = (*ls)->at(i - 1);

				if (log->service_name == service_name && log->method.equals_ci(method) && command_name.equals_ci(log->command_name))
				{
					if (log->extra == extra)
					{
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to remove logging for " << command << " with method " << method << (extra == "" ? "" : " ") << extra;
						source.Reply(_("Logging for command %s on %s with log method %s%s%s has been removed."), !log->command_name.empty() ? log->command_name.c_str() : log->service_name.c_str(), !log->command_service.empty() ? log->command_service.c_str() : "any service", method.c_str(), extra.empty() ? "" : " ", extra.empty() ? "" : extra.c_str());
						delete log;
					}
					else
					{
						log->extra = extra;
						Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to change logging for " << command << " to method " << method << (extra == "" ? "" : " ") << extra;
						source.Reply(_("Logging changed for command %s on %s, now using log method %s%s%s."), !log->command_name.empty() ? log->command_name.c_str() : log->service_name.c_str(), !log->command_service.empty() ? log->command_service.c_str() : "any service", method.c_str(), extra.empty() ? "" : " ", extra.empty() ? "" : extra.c_str());
					}
					return;
				}
			}

			auto *log = new LogSettingImpl();
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

			source.Reply(_("Logging is now active for command %s on %s, using log method %s%s%s."), !command_name.empty() ? command_name.c_str() : service_name.c_str(), bi ? bi->nick.c_str() : "any service", method.c_str(), extra.empty() ? "" : " ", extra.empty() ? "" : extra.c_str());
		}
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"The %s command allows users to configure logging settings "
				"for their channel. If no parameters are given this command "
				"lists the current logging methods in place for this channel."
				"\n\n"
				"Otherwise, \037command\037 must be a command name, and \037method\037 "
				"is one of the following logging methods:"
				"\n\n"
				" MESSAGE\032[status], NOTICE\032[status], MEMO"
				"\n\n"
				"Which are used to message, notice, and memo the channel respectively. "
				"With MESSAGE or NOTICE you must have a service bot assigned to and joined "
				"to your channel. Status may be a channel status such as @ or +."
				"\n\n"
				"To remove a logging method use the same syntax as you would to add it."
				"\n\n"
				"Example:\n"
				" %s\032#anope\032chanserv/access\032MESSAGE\032@\n"
				" Would message any channel operators whenever someone used the "
				"ACCESS command on ChanServ on the channel."
			),
			source.command.nobreak().c_str(),
			source.command.nobreak().c_str());

		return true;
	}
};

class CSLog final
	: public Module
{
	CommandCSLog commandcslog;
	ExtensibleItem<LogSettingsImpl> logsettings;
	LogSettingTypeImpl logsetting_type;

	struct LogDefault final
	{
		Anope::string service, command, method;
	};

	std::vector<LogDefault> defaults;

public:
	CSLog(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandcslog(this)
		, logsettings(this, CHANSERV_LOG_SETTING_EXT)
	{
	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &block = conf.GetModule(this);
		defaults.clear();

		for (int i = 0; i < block.CountBlock("default"); ++i)
		{
			const auto &def = block.GetBlock("default", i);

			LogDefault ld;

			ld.service = def.Get<const Anope::string>("service");
			ld.command = def.Get<const Anope::string>("command");
			ld.method = def.Get<const Anope::string>("method");

			defaults.push_back(ld);
		}
	}

	void OnChanRegistered(ChannelInfo *ci) override
	{
		if (defaults.empty())
			return;

		auto *ls = logsettings.Require(ci);
		for (auto &d : defaults)
		{
			auto *log = new LogSettingImpl();
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

	void OnLog(Log *l) override
	{
		if (l->type != LOG_COMMAND || l->u == NULL || l->c == NULL || l->ci == NULL || !Me || !Me->IsSynced())
			return;

		auto *ls = logsettings.Get(l->ci);
		if (ls)
		{
			for (auto *log : *(*ls))
			{
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
					IRCD->SendPrivmsg(l->ci->WhoSends(), log->extra + l->ci->c->name, buffer);
					l->ci->WhoSends()->lastmsg = Anope::CurTime;
				}
				else if (log->method.equals_ci("NOTICE") && l->ci->c)
					IRCD->SendNotice(l->ci->WhoSends(), log->extra + l->ci->c->name, buffer);
			}
		}
	}
};

MODULE_INIT(CSLog)
