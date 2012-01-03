/* ChanServ core functions
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
#include "memoserv.h"

class CommandCSLog : public Command
{
public:
	CommandCSLog(Module *creator) : Command(creator, "chanserv/log", 1, 4)
	{
		this->SetDesc(_("Configures channel logging settings"));
		this->SetSyntax(_("\037channel\037"));
		this->SetSyntax(_("\037channel\037 \037command\037 \037method\037 [\037status\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &channel = params[0];

		User *u = source.u;
		ChannelInfo *ci = cs_findchan(channel);
		if (ci == NULL)
			source.Reply(CHAN_X_NOT_REGISTERED, channel.c_str());
		else if (!ci->AccessFor(u).HasPriv("SET"))
			source.Reply(ACCESS_DENIED);
		else if (params.size() == 1)
		{
			if (ci->log_settings.empty())
				source.Reply(_("There currently are no logging configurations for %s."), ci->name.c_str());
			else
			{
				ListFormatter list;
				list.addColumn("Number").addColumn("Service").addColumn("Command").addColumn("Method").addColumn("");

				for (unsigned i = 0; i < ci->log_settings.size(); ++i)
				{
					LogSetting &log = ci->log_settings[i];

					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(i + 1);
					entry["Service"] = log.command_service;
					entry["Command"] = log.command_name;
					entry["Method"] = log.method;
					entry[""] = log.extra;
					list.addEntry(entry);
				}

				source.Reply(_("Log list for %s:"), ci->name.c_str());

				std::vector<Anope::string> replies;
				list.Process(replies);

				for (unsigned i = 0; i < replies.size(); ++i)
					source.Reply(replies[i]);
			}
		}
		else if (params.size() > 2)
		{
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
			BotInfo *bi = findbot(service);

			if (bi == NULL || bi->commands.count(command_name) == 0)
			{
				source.Reply(_("%s is not a valid command."), command.c_str());
				return;
			}

			service_reference<Command> c_service(bi->commands[command_name].name);
			if (!c_service)
			{
				source.Reply(_("%s is not a valid command."), command.c_str());
				return;
			}

			if (!method.equals_ci("MESSAGE") && !method.equals_ci("NOTICE") && !method.equals_ci("MEMO"))
			{
				source.Reply(_("%s is not a valid logging method."), method.c_str());
				return;
			}

			for (unsigned i = 0; i < extra.length(); ++i)
				if (ModeManager::GetStatusChar(extra[i]) == 0)
				{
					source.Reply(_("%c is an unknown status mode."), extra[i]);
					return;
				}

			for (unsigned i = ci->log_settings.size(); i > 0; --i)
			{
				LogSetting &log = ci->log_settings[i - 1];

				if (log.service_name == bi->commands[command_name].name && log.method.equals_ci(method))
				{
					if (log.extra == extra)
					{
						ci->log_settings.erase(ci->log_settings.begin() + i - 1);
						source.Reply(_("Logging for command %s on %s with method %s%s has been removed."), command_name.c_str(), bi->nick.c_str(), method.c_str(), extra.empty() ? "" : extra.c_str());
					}
					else
					{
						log.extra = extra;
						source.Reply(_("Logging changed for command %s on %s, using log method %s %s"), command_name.c_str(), bi->nick.c_str(), method.c_str(), extra.c_str());
					}
					return;
				}
			}

			LogSetting log;
			log.ci = ci;
			log.service_name = bi->commands[command_name].name;
			log.command_service = bi->nick;
			log.command_name = command_name;
			log.method = method;
			log.extra = extra;
			log.created = Anope::CurTime;
			log.creator = u->nick;

			ci->log_settings.push_back(log);
			Log(LOG_COMMAND, u, this, ci) << command << " " << method << " " << extra;

			source.Reply(_("Logging is now active for command %s on %s, using log method %s %s"), command_name.c_str(), bi->nick.c_str(), method.c_str(), extra.c_str());
		}
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("The %s command allows users to configure logging settings\n"
				"for their channel. If no parameters are given this command\n"
				"lists the current logging methods in place for this channel.\n"
				" \n"
				"Otherwise, \037command\037 must be a command name, and \037method\037\n"
				"is one of the following logging methods:\n"
				" \n"
				" MESSAGE [status], NOTICE [status], MEMO\n"
				" \n"
				"Which are used to message, notice, and memo the channel respectively.\n"
				"With MESSAGE or NOTICE you must have a service bot assigned to and joined\n"
				"to your channel. Status may be a channel status such as @ or +.\n"
				" \n"
				"To remove a logging method use the same syntax as you would to add it.\n"
				" \n"
				"Example:\n"
				" %s #anope chanserv/access MESSAGE @%\n"
				" Would message any channel operators whenever someone used the\n"
				" ACCESS command on ChanServ on the channel."),
				source.command.c_str(), source.command.c_str());
		return true;
	}
};

class CSLog : public Module
{
	CommandCSLog commandcslog;

 public:
	CSLog(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), commandcslog(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnLog };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnLog(Log *l)
	{
		if (l->Type != LOG_COMMAND || l->u == NULL || l->c == NULL || l->ci == NULL || !Me || !Me->IsSynced())
			return;

		for (unsigned i = l->ci->log_settings.size(); i > 0; --i)
		{
			LogSetting &log = l->ci->log_settings[i - 1];

			if (log.service_name == l->c->name)
			{
				Anope::string buffer = l->u->nick + " used " + log.command_name + " " + l->buf.str();

				if (log.method.equals_ci("MESSAGE") && l->ci->c && l->ci->bi && l->ci->c->FindUser(l->ci->bi) != NULL)
					ircdproto->SendPrivmsg(l->ci->bi, log.extra + l->ci->c->name, "%s", buffer.c_str());
				else if (log.method.equals_ci("NOTICE") && l->ci->c && l->ci->bi && l->ci->c->FindUser(l->ci->bi) != NULL)
					ircdproto->SendNotice(l->ci->bi, log.extra + l->ci->c->name, "%s", buffer.c_str());
				else if (log.method.equals_ci("MEMO") && memoserv && l->ci->WhoSends() != NULL)
					memoserv->Send(l->ci->WhoSends()->nick, l->ci->name, buffer, true);
			}
		}
	}
};

MODULE_INIT(CSLog)
