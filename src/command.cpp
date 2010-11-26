/*
 * Copyright (C) 2008-2010 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"
#include "modules.h"

CommandSource::~CommandSource()
{
	for (std::list<Anope::string>::iterator it = this->reply.begin(), it_end = this->reply.end(); it != it_end; ++it)
	{
		const Anope::string &message = *it;

		// Send to the user if the reply is more than one line
		if (!this->fantasy || !this->ci || this->reply.size() > 1)
			u->SendMessage(this->service->nick, message);
		else if (this->ci->botflags.HasFlag(BS_MSG_PRIVMSG))
			ircdproto->SendPrivmsg(this->service, this->ci->name, message.c_str());
		else if (this->ci->botflags.HasFlag(BS_MSG_NOTICE))
			ircdproto->SendNotice(this->service, this->ci->name, message.c_str());
		else if (this->ci->botflags.HasFlag(BS_MSG_NOTICEOPS))
			ircdproto->SendNoticeChanops(this->service, this->ci->c, message.c_str());
		else
			u->SendMessage(this->service->nick, message);
	}
}

void CommandSource::Reply(LanguageString message, ...)
{
	Anope::string m = GetString(this->u, message);

	m = m.replace_all_cs("%S", this->owner->nick);

	if (m.length() >= 4096)
		Log() << "Warning, language string " << message << " is longer than 4096 bytes";

	va_list args;
	char buf[4096];
	va_start(args, message);
	vsnprintf(buf, sizeof(buf) - 1, m.c_str(), args);
	va_end(args);

	sepstream sep(buf, '\n');
	Anope::string line;

	while (sep.GetToken(line))
		this->Reply(line.empty() ? " " : line.c_str());
}

void CommandSource::Reply(const char *message, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";

	if (message)
	{
		va_start(args, message);
		vsnprintf(buf, BUFSIZE - 1, message, args);

		this->reply.push_back(message);

		va_end(args);
	}
}

Command::Command(const Anope::string &sname, size_t min_params, size_t max_params, const Anope::string &spermission) : MaxParams(max_params), MinParams(min_params), name(sname), permission(spermission)
{
	this->module = NULL;
	this->service = NULL;
}

Command::~Command()
{
	if (this->module)
		this->module->DelCommand(this->service, this);
}

void Command::OnServHelp(User *u) { }

bool Command::OnHelp(User *u, const Anope::string &subcommand) { return false; }

void Command::OnSyntaxError(User *u, const Anope::string &subcommand) { }

void Command::SetPermission(const Anope::string &reststr)
{
	this->permission = reststr;
}

bool Command::AddSubcommand(Command *c)
{
	return false;
}

bool Command::DelSubcommand(Command *c)
{
	return false;
}

Command *Command::FindSubcommand(const Anope::string &subcommand)
{
	return NULL;
}

