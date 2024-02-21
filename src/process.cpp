/* Main processing code for Services.
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "protocol.h"
#include "servers.h"
#include "users.h"
#include "regchannel.h"

void Anope::Process(const Anope::string &buffer)
{
	/* If debugging, log the buffer */
	Log(LOG_RAWIO) << "Received: " << buffer;

	if (buffer.empty())
		return;

	Anope::map<Anope::string> tags;
	Anope::string source, command;
	std::vector<Anope::string> params;

	if (!IRCD->Parse(buffer, tags, source, command, params))
		return;

	if (Anope::ProtocolDebug)
	{
		if (tags.empty())
			Log() << "No tags";
		else
			for (Anope::map<Anope::string>::const_iterator it = tags.begin(); it != tags.end(); ++it)
				Log() << "tags " << it->first << ": " << it->second;

		Log() << "Source : " << (source.empty() ? "No source" : source);
		Log() << "Command: " << command;

		if (params.empty())
			Log() << "No params";
		else
			for (unsigned i = 0; i < params.size(); ++i)
				Log() << "params " << i << ": " << params[i];
	}

	static const Anope::string proto_name = ModuleManager::FindFirstOf(PROTOCOL) ? ModuleManager::FindFirstOf(PROTOCOL)->name : "";

	MessageSource src(source);

	EventReturn MOD_RESULT;
	FOREACH_RESULT(OnMessage, MOD_RESULT, (src, command, params, tags));
	if (MOD_RESULT == EVENT_STOP)
		return;

	ServiceReference<IRCDMessage> m("IRCDMessage", proto_name + "/" + command.lower());
	if (!m)
	{
		Log(LOG_DEBUG) << "unknown message from server (" << buffer << ")";
		return;
	}

	if (m->HasFlag(IRCDMESSAGE_SOFT_LIMIT) ? (params.size() < m->GetParamCount()) : (params.size() != m->GetParamCount()))
		Log(LOG_DEBUG) << "invalid parameters for " << command << ": " << params.size() << " != " << m->GetParamCount();
	else if (m->HasFlag(IRCDMESSAGE_REQUIRE_USER) && !src.GetUser())
		Log(LOG_DEBUG) << "unexpected non-user source " << source << " for " << command;
	else if (m->HasFlag(IRCDMESSAGE_REQUIRE_SERVER) && !source.empty() && !src.GetServer())
		Log(LOG_DEBUG) << "unexpected non-server source " << source << " for " << command;
	else
		m->Run(src, params, tags);
}

bool IRCDProto::Parse(const Anope::string &buffer, Anope::map<Anope::string> &tags, Anope::string &source, Anope::string &command, std::vector<Anope::string> &params)
{
	MessageTokenizer tokens(buffer);

	// This will always exist because of the check in Anope::Process.
	Anope::string token;
	tokens.GetMiddle(token);

	if (token[0] == '@')
	{
		// The line begins with message tags.
		sepstream tagstream(token.substr(1), ';');
		while (tagstream.GetToken(token))
		{
			const Anope::string::size_type valsep = token.find('=');
			if (valsep == Anope::string::npos)
			{
				// Tag has no value.
				tags[token];
			}
			else
			{
				// Tag has a value
				tags[token.substr(0, valsep)] = token.substr(valsep + 1);
			}
		}

		if (!tokens.GetMiddle(token))
			return false;
	}

	if (token[0] == ':')
	{
		source = token.substr(1);
		if (!tokens.GetMiddle(token))
			return false;
	}

	// Store the command name.
	command = token;

	// Retrieve all of the parameters.
	while (tokens.GetTrailing(token))
		params.push_back(token);

	return true;
}

bool IRCDProto::Format(Anope::string &message, const Anope::map<Anope::string> &tags, const MessageSource &source, const Anope::string &command, const std::vector<Anope::string> &params)
{
	std::stringstream buffer;
	if (CanSendTags && !tags.empty())
	{
		char separator = '@';
		for (const auto &[tname, tvalue] : tags)
		{
			buffer << separator << tname;
			if (!tvalue.empty())
				buffer << '=' << tvalue;
			separator = ';';
		}
		buffer << ' ';
	}

	if (source.GetServer())
	{
		const auto *s = source.GetServer();
		if (s != Me && !s->IsJuped())
		{
			Log(LOG_DEBUG) << "Attempted to send \"" << command << "\" from " << s->GetName() << " who is not from me";
			return false;
		}

		buffer << ':' << s->GetSID() << ' ';
	}
	else if (source.GetUser())
	{
		const auto *u = source.GetUser();
		if (u->server != Me && !u->server->IsJuped())
		{
			Log(LOG_DEBUG) << "Attempted to send \"" << command << "\" from " << u->nick << " who is not from me";
			return false;
		}

		const auto *bi = source.GetBot();
		if (bi && !bi->introduced)
		{
			Log(LOG_DEBUG) << "Attempted to send \"" << command << "\" from " << bi->nick << " when not introduced";
			return false;
		}

		buffer << ':' << u->GetUID() << ' ';
	}

	buffer << command;
	if (!params.empty())
	{
		buffer << ' ';
		for (auto it = params.begin(); it != params.end() - 1; ++it)
			buffer << *it << ' ';


		const auto &last = params.back();
		if (last.empty() || last[0] == ':' || last.find(' ') != std::string::npos)
			buffer << ':';
		buffer << last;
	}

	message = buffer.str();
	return true;
}

MessageTokenizer::MessageTokenizer(const Anope::string &msg)
	: message(msg)
{
}

bool MessageTokenizer::GetMiddle(Anope::string &token)
{
	// If we are past the end of the string we can't do anything.
	if (position >= message.length())
	{
		token.clear();
		return false;
	}

	// If we can't find another separator this is the last token in the message.
	Anope::string::size_type separator = message.find(' ', position);
	if (separator == Anope::string::npos)
	{
		token = message.substr(position);
		position = message.length();
		return true;
	}

	token = message.substr(position, separator - position);
	position = message.find_first_not_of(' ', separator);
	return true;
}

bool MessageTokenizer::GetTrailing(Anope::string &token)
{
	// If we are past the end of the string we can't do anything.
	if (position >= message.length())
	{
		token.clear();
		return false;
	}

	// If this is true then we have a <trailing> token!
	if (message[position] == ':')
	{
		token = message.substr(position + 1);
		position = message.length();
		return true;
	}

	// There is no <trailing> token so it must be a <middle> token.
	return GetMiddle(token);
}
