/*
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2011 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"
#include "modules.h"
#include "commands.h"

void CommandSource::Reply(const char *message, ...)
{
	va_list args;
	char buf[4096]; // Messages can be really big.

	if (message)
	{
		va_start(args, message);
		vsnprintf(buf, sizeof(buf), message, args);

		this->Reply(Anope::string(buf));

		va_end(args);
	}
}

void CommandSource::Reply(const Anope::string &message)
{
	sepstream sep(message, '\n');
	Anope::string tok;
	while (sep.GetToken(tok))
	{
		const char *translated_message = translate(this->u, tok.c_str());
		this->reply.push_back(translated_message);
	}
}

void CommandSource::DoReply()
{
	for (std::list<Anope::string>::iterator it = this->reply.begin(), it_end = this->reply.end(); it != it_end; ++it)
	{
		const Anope::string &message = *it;

		// Send to the user if the reply is more than one line
		if (!this->c || !this->c->ci || this->reply.size() > 1)
			u->SendMessage(this->service, message);
		else if (this->c->ci->botflags.HasFlag(BS_MSG_PRIVMSG))
			ircdproto->SendPrivmsg(this->service, this->c->name, message.c_str());
		else if (this->c->ci->botflags.HasFlag(BS_MSG_NOTICE))
			ircdproto->SendNotice(this->service, this->c->name, message.c_str());
		else if (this->c->ci->botflags.HasFlag(BS_MSG_NOTICEOPS))
			ircdproto->SendNoticeChanops(this->service, this->c, message.c_str());
		else
			u->SendMessage(this->service, message);
	}
}

Command::Command(Module *o, const Anope::string &sname, size_t min_params, size_t max_params, const Anope::string &spermission) : Service(o, sname), Flags<CommandFlag>(CommandFlagStrings), MaxParams(max_params), MinParams(min_params), permission(spermission), module(owner)
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
	source.Reply("    %-14s %s", source.command.c_str(), translate(source.u, (this->GetDesc().c_str())));
}

bool Command::OnHelp(CommandSource &source, const Anope::string &subcommand) { return false; }

void Command::OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
{
	this->SendSyntax(source);
	source.Reply(MORE_INFO, Config->UseStrictPrivMsgString.c_str(), source.owner->nick.c_str(), source.command.c_str());
}

void Command::SetPermission(const Anope::string &reststr)
{
	this->permission = reststr;
}

