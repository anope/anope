/*
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2012 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"
#include "commands.h"
#include "extern.h"
#include "users.h"
#include "language.h"
#include "config.h"
#include "bots.h"
#include "opertype.h"
#include "access.h"
#include "regchannel.h"

CommandSource::CommandSource(const Anope::string &n, User *user, NickCore *core, CommandReply *r) : nick(n), u(user), nc(core), reply(r)
{
}

const Anope::string &CommandSource::GetNick() const
{
	return this->nick;
}

User *CommandSource::GetUser() const
{
	return this->u;
}

AccessGroup CommandSource::AccessFor(ChannelInfo *ci) const
{
	if (this->u)
		return ci->AccessFor(this->u);
	else if (this->nc)
		return ci->AccessFor(this->nc);
	else
		return AccessGroup();
}

bool CommandSource::IsFounder(ChannelInfo *ci) const
{
	if (this->u)
		return ::IsFounder(this->u, ci);
	else if (this->nc)
		return this->nc == ci->GetFounder();
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

bool CommandSource::IsServicesOper() const
{
	if (this->u)
		return this->u->IsServicesOper();
	else if (this->nc)
		return this->nc->IsServicesOper();
	return false;
}

bool CommandSource::IsOper() const
{
	if (this->u)
		return this->u->HasMode(UMODE_OPER);
	else if (this->nc)
		return this->nc->IsServicesOper();
	return false;
}

void CommandSource::Reply(const char *message, ...)
{
	va_list args;
	char buf[4096]; // Messages can be really big.

	const char *translated_message = translate(this->u, message);

	va_start(args, message);
	vsnprintf(buf, sizeof(buf), translated_message, args);

	this->Reply(Anope::string(buf));

	va_end(args);
}

void CommandSource::Reply(const Anope::string &message)
{
	const char *translated_message = translate(this->u, message.c_str());

	sepstream sep(translated_message, '\n');
	Anope::string tok;
	while (sep.GetToken(tok))
		this->reply->SendMessage(this->service, tok);
}

Command::Command(Module *o, const Anope::string &sname, size_t min_params, size_t max_params) : Service(o, "Command", sname), Flags<CommandFlag>(CommandFlagStrings), MaxParams(max_params), MinParams(min_params), module(owner)
{
}

Command::~Command()
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

void Command::SetSyntax(const Anope::string &s)
{
	this->syntax.push_back(s);
}

void Command::SendSyntax(CommandSource &source)
{
	if (!this->syntax.empty())
	{
		source.Reply(_("Syntax: \002%s %s\002"), source.command.c_str(), this->syntax[0].c_str());
		for (unsigned i = 1, j = this->syntax.size(); i < j; ++i)
			source.Reply("        \002%s %s\002", source.command.c_str(), this->syntax[i].c_str());
	}
}

void Command::SendSyntax(CommandSource &source, const Anope::string &syn)
{
	source.Reply(_("Syntax: \002%s %s\002"), source.command.c_str(), syn.c_str());
	source.Reply(MORE_INFO, Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str(), source.command.c_str());
}

const Anope::string &Command::GetDesc() const
{
	return this->desc;
}

void Command::OnServHelp(CommandSource &source)
{
	source.Reply("    %-14s %s", source.command.c_str(), translate(source.nc, (this->GetDesc().c_str())));
}

bool Command::OnHelp(CommandSource &source, const Anope::string &subcommand) { return false; }

void Command::OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
{
	this->SendSyntax(source);
	source.Reply(MORE_INFO, Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str(), source.command.c_str());
}

void RunCommand(CommandSource &source, const Anope::string &message)
{
	std::vector<Anope::string> params = BuildStringVector(message);
	bool has_help = source.service->commands.find("HELP") != source.service->commands.end();

	BotInfo::command_map::const_iterator it = source.service->commands.end();
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
		if (has_help)
			source.Reply(_("Unknown command \002%s\002. \"%s%s HELP\" for help."), message.c_str(), Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());
		else
			source.Reply(_("Unknown command \002%s\002."), message.c_str());
		return;
	}

	const CommandInfo &info = it->second;
	service_reference<Command> c("Command", info.name);
	if (!c)
	{
		if (has_help)
			source.Reply(_("Unknown command \002%s\002. \"%s%s HELP\" for help."), message.c_str(), Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());
		else
			source.Reply(_("Unknown command \002%s\002."), message.c_str());
		Log(source.service) << "Command " << it->first << " exists on me, but its service " << info.name << " was not found!";
		return;
	}

	// Command requires registered users only
	if (!c->HasFlag(CFLAG_ALLOW_UNREGISTERED) && !source.nc)
	{
		source.Reply(NICK_IDENTIFY_REQUIRED);
		if (source.GetUser())
			Log(LOG_NORMAL, "access_denied", source.service) << "Access denied for unregistered user " << source.GetUser()->GetMask() << " with command " << c->name;
		return;
	}

	for (unsigned i = 0, j = params.size() - (count - 1); i < j; ++i)
		params.erase(params.begin());

	while (c->MaxParams > 0 && params.size() > c->MaxParams)
	{
		params[c->MaxParams - 1] += " " + params[c->MaxParams];
		params.erase(params.begin() + c->MaxParams);
	}

	source.command = it->first;
	source.permission = info.permission;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnPreCommand, OnPreCommand(source, c, params));
	if (MOD_RESULT == EVENT_STOP)
		return;


	if (params.size() < c->MinParams)
	{
		c->OnSyntaxError(source, !params.empty() ? params[params.size() - 1] : "");
		return;
	}

	// If the command requires a permission, and they aren't registered or don't have the required perm, DENIED
	if (!info.permission.empty() && !source.HasCommand(info.permission))
	{
		source.Reply(ACCESS_DENIED);
		if (source.GetUser())
			Log(LOG_NORMAL, "access_denied", source.service) << "Access denied for user " << source.GetUser()->GetMask() << " with command " << c->name;
		return;
	}

	bool had_u = source.GetUser(), had_nc = source.nc;
	dynamic_reference<User> user_reference(source.GetUser());
	dynamic_reference<NickCore> nc_reference(source.nc);
	c->Execute(source, params);
	if (had_u == user_reference && had_nc == nc_reference)
	{
		FOREACH_MOD(I_OnPostCommand, OnPostCommand(source, c, params));
	}
}

