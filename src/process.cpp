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

#include "services.h"
#include "modules.h"
#include "protocol.h"
#include "servers.h"
#include "users.h"
#include "event.h"

void Anope::Process(const Anope::string &buffer)
{
	/* If debugging, log the buffer */
	Anope::Logger.RawIO("Received: {0}", buffer);

	if (buffer.empty())
		return;

	Anope::string source, command;
	std::vector<Anope::string> params;

	IRCD->Parse(buffer, source, command, params);

	if (Anope::ProtocolDebug)
	{
		Anope::Logger.Log("Source : {0}", source.empty() ? "No source" : source);
		Anope::Logger.Log("Command: {0}", command);

		if (params.empty())
			Anope::Logger.Log("No params");
		else
			for (unsigned int i = 0; i < params.size(); ++i)
				Anope::Logger.Log("params {0}: {1}", i, params[i]);
	}

	if (command.empty())
	{
		Anope::Logger.Debug("No command? {0}", buffer);
		return;
	}

	MessageSource src(source);

	EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&Event::Message::OnMessage, src, command, params);

	ProcessCommand(src, command, params);
}

void Anope::ProcessCommand(MessageSource &src, const Anope::string &command, const std::vector<Anope::string> &params)
{
	static const Anope::string proto_name = ModuleManager::FindFirstOf(PROTOCOL) ? ModuleManager::FindFirstOf(PROTOCOL)->name : "";

	ServiceReference<IRCDMessage> m(proto_name + "/" + command.lower());
	if (!m)
	{
		Anope::string buffer = "[" + src.GetSource() + "] " + command;
		if (!params.empty())
		{
			for (unsigned int i = 0; i < params.size() - 1; ++i)
				buffer += " " + params[i];
			buffer += " :" + params[params.size() - 1];
		}

		Anope::Logger.Debug("unknown command from server ({0})", buffer);
		return;
	}

	if (m->HasFlag(IRCDMESSAGE_SOFT_LIMIT) ? (params.size() < m->GetParamCount()) : (params.size() != m->GetParamCount()))
		Anope::Logger.Debug("invalid parameters for {0}: {1} != {2}", command, params.size(), m->GetParamCount());
	else if (m->HasFlag(IRCDMESSAGE_REQUIRE_USER) && !src.GetUser())
		Anope::Logger.Debug("unexpected non-user source {0} for {1}", src.GetSource(), command);
	else if (m->HasFlag(IRCDMESSAGE_REQUIRE_SERVER) && !src.GetServer())
		Anope::Logger.Debug("unexpected non-server source {0} for {1}", src.GetSource().empty() ? "(no source)" : src.GetSource(), command);
	else
		m->Run(src, params);
}

void IRCDProto::Parse(const Anope::string &buffer, Anope::string &source, Anope::string &command, std::vector<Anope::string> &params)
{
	spacesepstream sep(buffer);

	if (buffer[0] == ':')
	{
		sep.GetToken(source);
		source.erase(0, 1);
	}

	sep.GetToken(command);
	
	for (Anope::string token; sep.GetToken(token);)
	{
		if (token[0] == ':')
		{
			if (!sep.StreamEnd())
				params.push_back(token.substr(1) + " " + sep.GetRemaining());
			else
				params.push_back(token.substr(1));
			break;
		}
		else
			params.push_back(token);
	}
}

Anope::string IRCDProto::Format(IRCMessage &message)
{
	std::stringstream buffer;
	
	const Anope::string &source = message.GetSource().GetUID();
	if (!source.empty())
		buffer << ":" << source << " ";
	
	buffer << message.GetCommand();

	for (unsigned int i = 0; i < message.GetParameters().size(); ++i)
	{
		buffer << " ";

		if (i + 1 == message.GetParameters().size())
			buffer << ":";

		buffer << message.GetParameters()[i];
	}

	return buffer.str();
}

