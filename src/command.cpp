/*
 *
 * (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * (C) 2008-2025 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"
#include "commands.h"
#include "users.h"
#include "language.h"
#include "config.h"
#include "bots.h"
#include "opertype.h"
#include "access.h"
#include "regchannel.h"
#include "channels.h"

void CommandReply::SendMessage(CommandSource &source, const Anope::string &msg)
{
	SendMessage(source.service, msg);
}

CommandSource::CommandSource(const Anope::string &n, User *user, NickCore *core, CommandReply *r, BotInfo *bi, const Anope::string &m)
	: nick(n)
	, u(user)
	, nc(core)
	, ip(user ? user->ip.addr() : "")
	, reply(r)
	, service(bi)
	, msgid(m)
{
}

const Anope::string &CommandSource::GetNick() const
{
	return this->nick;
}

User *CommandSource::GetUser()
{
	return this->u;
}

NickCore *CommandSource::GetAccount()
{
	return this->nc;
}

AccessGroup CommandSource::AccessFor(ChannelInfo *ci)
{
	if (this->u)
		return ci->AccessFor(this->u);
	else if (this->nc)
		return ci->AccessFor(this->nc);
	else
		return AccessGroup();
}

bool CommandSource::IsFounder(ChannelInfo *ci)
{
	if (this->u)
		return ::IsFounder(this->u, ci);
	else if (this->nc)
		return *this->nc == ci->GetFounder();
	return false;
}

bool CommandSource::HasCommand(const Anope::string &cmd)
{
	if (this->u)
		return this->u->HasCommand(cmd);
	else if (this->nc && this->nc->o)
		return this->nc->o->ot->HasCommand(cmd);
	return false;
}

bool CommandSource::HasPriv(const Anope::string &cmd)
{
	if (this->u)
		return this->u->HasPriv(cmd);
	else if (this->nc && this->nc->o)
		return this->nc->o->ot->HasPriv(cmd);
	return false;
}

bool CommandSource::IsServicesOper()
{
	if (this->u)
		return this->u->IsServicesOper();
	else if (this->nc)
		return this->nc->IsServicesOper();
	return false;
}

bool CommandSource::IsOper()
{
	if (this->u)
		return this->u->HasMode("OPER");
	else if (this->nc)
		return this->nc->IsServicesOper();
	return false;
}

void CommandSource::Reply(const char *message, ...)
{
	va_list args;
	char buf[4096]; // Messages can be really big.

	const char *translated_message = Language::Translate(this->nc, message);

	va_start(args, message);
	vsnprintf(buf, sizeof(buf), translated_message, args);

	this->reply->SendMessage(*this, buf);

	va_end(args);
}

void CommandSource::Reply(int count, const char *single, const char *plural, ...)
{
	va_list args;
	char buf[4096]; // Messages can be really big.

	const char *translated_message = Language::Translate(this->nc, count, single, plural);

	va_start(args, plural);
	vsnprintf(buf, sizeof(buf), translated_message, args);

	this->reply->SendMessage(*this, buf);

	va_end(args);
}

void CommandSource::Reply(const Anope::string &message)
{
	const char *translated_message = Language::Translate(this->nc, message.c_str());
	this->reply->SendMessage(*this, translated_message);
}

Command::Command(Module *o, const Anope::string &sname, size_t minparams, size_t maxparams) : Service(o, "Command", sname), max_params(maxparams), min_params(minparams), module(o)
{
}

void Command::SetDesc(const Anope::string &d)
{
	this->desc = d;
}

void Command::ClearSyntax()
{
	this->syntax.clear();
}

void Command::SetSyntax(const Anope::string &s, const std::function<bool(CommandSource&)> &p)
{
	this->syntax.emplace_back(s, p);
}

void Command::SendSyntax(CommandSource &source)
{
	auto first = true;
	Anope::string prefix = Language::Translate(source.GetAccount(), _("Syntax"));
	for (const auto &[syntax, predicate] : this->syntax)
	{
		if (predicate && !predicate(source))
			continue; // Not for this user.

		if (first)
		{
			first = false;
			source.Reply("%s: \002%s %s\002", prefix.c_str(), source.command.nobreak().c_str(),
				Language::Translate(source.GetAccount(), syntax.c_str()));
		}
		else
		{
			source.Reply("%-*s  \002%s %s\002", (int)prefix.length(), "", source.command.nobreak().c_str(),
				Language::Translate(source.GetAccount(), syntax.c_str()));
		}
	}

	if (first)
		source.Reply("%s: \002%s\002", prefix.c_str(), source.command.nobreak().c_str());
}

void Command::AllowUnregistered(bool b)
{
	this->allow_unregistered = b;
}

void Command::RequireUser(bool b)
{
	this->require_user = b;
}

const Anope::string Command::GetDesc(CommandSource &) const
{
	return this->desc;
}

void Command::OnServHelp(CommandSource &source, HelpWrapper &help)
{
	help.AddEntry(source.command, this->GetDesc(source));
}

bool Command::OnHelp(CommandSource &source, const Anope::string &subcommand) { return false; }

void Command::OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
{
	this->SendSyntax(source);

	auto it = std::find_if(source.service->commands.begin(), source.service->commands.end(), [](const auto &cmd)
	{
		// The help command may not be called HELP.
		return cmd.second.name == "generic/help";
	});
	if (it != source.service->commands.end())
		source.Reply(MORE_INFO, source.service->GetQueryCommand("generic/help", source.command).c_str());
}

namespace
{
	void HandleUnknownCommand(CommandSource& source, const Anope::string &message)
	{
		// Try to find a similar command.
		size_t distance = Config->GetBlock("options").Get<size_t>("didyoumeandifference", "4");
		Anope::string similar;
		auto umessage = message.upper();
		for (const auto &[command, info] : source.service->commands)
		{
			if (info.hide || command == message)
				continue; // Don't suggest a hidden alias or a missing command.

			size_t dist = Anope::Distance(umessage, command);
			if (dist < distance)
			{
				distance = dist;
				similar = command;
			}
		}

		bool has_help = source.service->commands.find("HELP") != source.service->commands.end();
		if (has_help && similar.empty())
		{
			source.Reply(_("Unknown command \002%s\002. Type \"%s\" for help."), message.c_str(),
				source.service->GetQueryCommand("generic/help").c_str());
		}
		else if (has_help)
		{
			source.Reply(_("Unknown command \002%s\002. Did you mean \002%s\002? Type \"%s\" for help."),
				message.c_str(), similar.c_str(), source.service->GetQueryCommand("generic/help").c_str());
		}
		else if (similar.empty())
		{
			source.Reply(_("Unknown command \002%s\002. Did you mean \002%s\002?"), message.c_str(), similar.c_str());
		}
		else
		{
			source.Reply(_("Unknown command \002%s\002."), message.c_str());
		}
	}
}

bool Command::Run(CommandSource &source, const Anope::string &message)
{
	std::vector<Anope::string> params;
	spacesepstream(message).GetTokens(params);

	CommandInfo::map::const_iterator it = source.service->commands.end();
	unsigned count = 0;
	for (unsigned max = params.size(); it == source.service->commands.end() && max > 0; --max)
	{
		Anope::string full_command;
		for (unsigned i = 0; i < max; ++i)
			full_command += " " + params[i];
		full_command.erase(full_command.begin());

		++count;
		it = source.service->commands.find(full_command);
	}

	if (it == source.service->commands.end())
	{
		HandleUnknownCommand(source, message);
		return false;
	}

	const CommandInfo &info = it->second;
	ServiceReference<Command> c("Command", info.name);
	if (!c)
	{
		HandleUnknownCommand(source, message);
		Log(source.service) << "Command " << it->first << " exists on me, but its service " << info.name << " was not found!";
		return false;
	}

	for (unsigned i = 0, j = params.size() - (count - 1); i < j; ++i)
		params.erase(params.begin());

	while (c->max_params > 0 && params.size() > c->max_params)
	{
		params[c->max_params - 1] += " " + params[c->max_params];
		params.erase(params.begin() + c->max_params);
	}

	return c->Run(source, it->first, info, params);
}

bool Command::Run(CommandSource &source, const Anope::string &cmdname, const CommandInfo &info, std::vector<Anope::string> &params)
{
	if (this->RequireUser() && !source.GetUser())
		return false;

	// Command requires registered users only
	if (!this->AllowUnregistered() && !source.nc)
	{
		source.Reply(NICK_IDENTIFY_REQUIRED);
		if (source.GetUser())
			Log(LOG_NORMAL, "access_denied_unreg", source.service) << "Access denied for unregistered user " << source.GetUser()->GetMask() << " with command " << cmdname;
		return false;
	}

	source.command = cmdname;
	source.permission = info.permission;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(OnPreCommand, MOD_RESULT, (source, this, params));
	if (MOD_RESULT == EVENT_STOP)
		return false;

	if (params.size() < this->min_params)
	{
		this->OnSyntaxError(source, !params.empty() ? params[params.size() - 1] : "");
		return false;
	}

	// If the command requires a permission, and they aren't registered or don't have the required perm, DENIED
	if (!info.permission.empty() && !source.HasCommand(info.permission))
	{
		source.Reply(ACCESS_DENIED);
		if (source.GetUser())
			Log(LOG_NORMAL, "access_denied", source.service) << "Access denied for user " << source.GetUser()->GetMask() << " with command " << cmdname;
		return false;
	}

	this->Execute(source, params);
	FOREACH_MOD(OnPostCommand, (source, this, params));
	return true;
}

bool Command::FindCommandFromService(const Anope::string &command_service, BotInfo *&bot, Anope::string &name)
{
	bot = NULL;

	for (const auto &[_, bi] : *BotListByNick)
	{
		for (const auto &[c_name, info] : bi->commands)
		{
			if (info.name != command_service)
				continue;

			bot = bi;
			name = c_name;
			return true;
		}
	}

	return false;
}
