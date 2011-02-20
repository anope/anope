/*
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2011 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"
#include "modules.h"

void CommandSource::Reply(const char *message, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";

	if (message)
	{
		va_start(args, message);
		vsnprintf(buf, BUFSIZE - 1, message, args);

		this->Reply(Anope::string(buf));

		va_end(args);
	}
}

void CommandSource::Reply(const Anope::string &message)
{
	sepstream sep(message, '\n');
	Anope::string tok;
	while (sep.GetToken(tok))
		this->reply.push_back(tok);
}

void CommandSource::DoReply()
{
	for (std::list<Anope::string>::iterator it = this->reply.begin(), it_end = this->reply.end(); it != it_end; ++it)
	{
		const Anope::string &message = *it;

		// Send to the user if the reply is more than one line
		if (!this->fantasy || !this->ci || this->reply.size() > 1)
			u->SendMessage(this->service, message);
		else if (this->ci->botflags.HasFlag(BS_MSG_PRIVMSG))
			ircdproto->SendPrivmsg(this->service, this->ci->name, message.c_str());
		else if (this->ci->botflags.HasFlag(BS_MSG_NOTICE))
			ircdproto->SendNotice(this->service, this->ci->name, message.c_str());
		else if (this->ci->botflags.HasFlag(BS_MSG_NOTICEOPS))
			ircdproto->SendNoticeChanops(this->service, this->ci->c, message.c_str());
		else
			u->SendMessage(this->service, message);
	}
}

Command::Command(const Anope::string &sname, size_t min_params, size_t max_params, const Anope::string &spermission) : Flags<CommandFlag>(CommandFlagStrings), MaxParams(max_params), MinParams(min_params), name(sname), permission(spermission)
{
	this->module = NULL;
	this->service = NULL;
}

Command::~Command()
{
	if (this->module)
		this->module->DelCommand(this->service, this);
}

void Command::SetDesc(const Anope::string &d)
{
	this->desc = d;
}

void Command::OnServHelp(CommandSource &source)
{
	source.Reply("    %-14s %s", this->name.c_str(), _(this->desc.c_str()));
}

bool Command::OnHelp(CommandSource &source, const Anope::string &subcommand) { return false; }

void Command::OnSyntaxError(CommandSource &source, const Anope::string &subcommand) { }

void Command::SetPermission(const Anope::string &reststr)
{
	this->permission = reststr;
}

bool Command::AddSubcommand(Module *creator, Command *c)
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

