/*
 * Anope IRC Services
 *
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2017 Anope Team <team@anope.org>
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

#include "services.h"
#include "commands.h"
#include "users.h"
#include "language.h"
#include "config.h"
#include "opertype.h"
#include "channels.h"
#include "event.h"
#include "bots.h"
#include "protocol.h"
#include "modules/botserv.h"
#include "modules/chanserv.h"

CommandSource::CommandSource(const Anope::string &n, User *user, NickServ::Account *core, CommandReply *r, ServiceBot *bi)
	: nick(n)
	, u(user)
	, nc(core)
	, reply(r)
	, service(bi)
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

NickServ::Account *CommandSource::GetAccount()
{
	return this->nc;
}

Anope::string CommandSource::GetSource()
{
	if (u)
		if (nc)
			return this->u->GetMask() + " (" + this->nc->GetDisplay() + ")";
		else
			return this->u->GetMask();
	else if (nc)
		return nc->GetDisplay();
	else
		return this->nick;
}

const Anope::string &CommandSource::GetCommand() const
{
	return this->command.cname;
}

void CommandSource::SetCommand(const Anope::string &command)
{
	this->command.cname = command;
}

const Anope::string &CommandSource::GetPermission() const
{
	return this->command.permission;
}

const CommandInfo &CommandSource::GetCommandInfo() const
{
	return this->command;
}

void CommandSource::SetCommandInfo(const CommandInfo &ci)
{
	this->command = ci;
}

ChanServ::AccessGroup CommandSource::AccessFor(ChanServ::Channel *ci)
{
	if (this->u)
		return ci->AccessFor(this->u);
	else if (this->nc)
		return ci->AccessFor(this->nc);
	else
		return ChanServ::AccessGroup();
}

bool CommandSource::IsFounder(ChanServ::Channel *ci)
{
	if (this->u)
		return ci->IsFounder(this->u);
	else if (this->nc)
		return *this->nc == ci->GetFounder();
	return false;
}

bool CommandSource::HasCommand(const Anope::string &cmd)
{
	if (this->u)
		return this->u->HasCommand(cmd);

	if (this->nc && this->nc->GetOper())
		return this->nc->GetOper()->HasCommand(cmd);

	return false;
}

bool CommandSource::HasPriv(const Anope::string &cmd)
{
	if (this->u)
		return this->u->HasPriv(cmd);

	if (this->nc && this->nc->GetOper())
		return this->nc->GetOper()->HasPriv(cmd);

	return false;
}

bool CommandSource::IsServicesOper()
{
	if (this->u)
		return this->u->IsServicesOper();
	else if (this->nc)
		return this->nc->GetOper() != nullptr;
	return false;
}

bool CommandSource::IsOper()
{
	if (this->u)
		return this->u->HasMode("OPER");
	else if (this->nc)
		return this->nc->GetOper() != nullptr;
	return false;
}

bool CommandSource::HasOverridePriv(const Anope::string &priv)
{
	if (!HasPriv(priv))
		return false;

	override = true;
	return true;
}

bool CommandSource::HasOverrideCommand(const Anope::string &priv)
{
	if (!HasCommand(priv))
		return false;

	override = true;
	return true;
}

bool CommandSource::IsOverride() const
{
	return override;
}

void CommandSource::Reply(const Anope::string &message)
{
	const char *translated_message = Language::Translate(this->nc, message.c_str());

	sepstream sep(translated_message, '\n', true);
	Anope::string tok;
	while (sep.GetToken(tok))
		this->reply->SendMessage(*this->service, tok);
}

Command::Command(Module *o, const Anope::string &sname, size_t minparams, size_t maxparams) : Service(o, NAME, sname)
	, max_params(maxparams)
	, min_params(minparams)
	, module(o)
	, logger(this)
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
	Anope::string s = Language::Translate(source.GetAccount(), _("Syntax"));
	if (!this->syntax.empty())
	{
		source.Reply("{0}: \002{1} {2}\002", s, source.GetCommand(), Language::Translate(source.GetAccount(), this->syntax[0].c_str()));
		Anope::string spaces(s.length(), ' ');
		for (unsigned i = 1, j = this->syntax.size(); i < j; ++i)
			source.Reply("{0}  \002{1} {2}\002", spaces, source.GetCommand(), Language::Translate(source.GetAccount(), this->syntax[i].c_str()));
	}
	else
		source.Reply("{0}: \002{1}\002", s, source.GetCommand());
}

bool Command::AllowUnregistered() const
{
	return this->allow_unregistered;
}

void Command::AllowUnregistered(bool b)
{
	this->allow_unregistered = b;
}

bool Command::RequireUser() const
{
	return this->require_user;
}

void Command::RequireUser(bool b)
{
	this->require_user = b;
}

const Anope::string Command::GetDesc(CommandSource &) const
{
	return this->desc;
}

void Command::OnServHelp(CommandSource &source)
{
	source.Reply(Anope::printf("    %-14s %s", source.GetCommand().c_str(), Language::Translate(source.nc, this->GetDesc(source).c_str())));
}

bool Command::OnHelp(CommandSource &source, const Anope::string &subcommand) { return false; }

void Command::OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
{
	this->SendSyntax(source);
	bool has_help = source.service->commands.find("HELP") != source.service->commands.end();
	if (has_help)
		source.Reply(_("\002{0}{1} HELP {2}\002 for more information."), Config->StrictPrivmsg, source.service->nick, source.GetCommand());
}

void Command::Run(CommandSource &source, const Anope::string &message)
{
	std::vector<Anope::string> params;
	spacesepstream(message).GetTokens(params);
	bool has_help = source.service->commands.find("HELP") != source.service->commands.end();

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
		if (has_help)
			source.Reply(_("Unknown command \002{0}\002. \"{1}{2} HELP\" for help."), message, Config->StrictPrivmsg, source.service->nick);
		else
			source.Reply(_("Unknown command \002{0}\002."), message);
		return;
	}

	const CommandInfo &info = it->second;
	ServiceReference<Command> c(info.name);
	if (!c)
	{
		if (has_help)
			source.Reply(_("Unknown command \002{0}\002. \"{1}{2} HELP\" for help."), message, Config->StrictPrivmsg, source.service->nick);
		else
			source.Reply(_("Unknown command \002{0}\002."), message);
		source.service->logger.Log("Command {0} exists on me, but its service {1} was not found!", it->first, info.name);
		return;
	}

	for (unsigned i = 0, j = params.size() - (count - 1); i < j; ++i)
		params.erase(params.begin());

	while (c->max_params > 0 && params.size() > c->max_params)
	{
		params[c->max_params - 1] += " " + params[c->max_params];
		params.erase(params.begin() + c->max_params);
	}

	c->Run(source, it->first, info, params);
}

void Command::Run(CommandSource &source, const Anope::string &cmdname, const CommandInfo &info, std::vector<Anope::string> &params)
{
	if (this->RequireUser() && !source.GetUser())
		return;

	// Command requires registered users only
	if (!this->AllowUnregistered() && !source.nc)
	{
		source.Reply(_("Password authentication required for that command."));
		if (source.GetUser())
			Anope::Logger.User(source.service).Category("access_denied_unreg").Log(_("Access denied for unregistered user {0} with command {1}"), source.GetUser()->GetMask(), cmdname);
		return;
	}

	source.SetCommandInfo(info);

	EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&Event::PreCommand::OnPreCommand, source, this, params);
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (params.size() < this->min_params)
	{
		this->OnSyntaxError(source, !params.empty() ? params[params.size() - 1] : "");
		return;
	}

	// If the command requires a permission, and they aren't registered or don't have the required perm, DENIED
	if (!info.permission.empty() && !source.HasCommand(info.permission))
	{
		if (!source.IsOper())
			source.Reply(_("Access denied. You are not a Services Operator."));
		else
			source.Reply(_("Access denied. You do not have access to command \002{0}\002."), info.permission);
		if (source.GetUser())
			Anope::Logger.User(source.service).Category("access_denied").Log(_("Access denied for user {0} with command {1}"), source.GetUser()->GetMask(), cmdname);
		return;
	}

	this->Execute(source, params);
	EventManager::Get()->Dispatch(&Event::PostCommand::OnPostCommand, source, this, params);
}

bool Command::FindCommandFromService(const Anope::string &command_service, ServiceBot* &bot, Anope::string &name)
{
	bot = NULL;

	for (std::pair<Anope::string, User *> p : UserListByNick)
	{
		User *u = p.second;
		if (u->type != UserType::BOT)
			continue;

		ServiceBot *bi = anope_dynamic_static_cast<ServiceBot *>(u);

		for (CommandInfo::map::const_iterator cit = bi->commands.begin(), cit_end = bi->commands.end(); cit != cit_end; ++cit)
		{
			const Anope::string &c_name = cit->first;
			const CommandInfo &info = cit->second;

			if (info.name != command_service)
				continue;

			bot = bi;
			name = c_name;
			return true;
		}
	}

	return false;
}

