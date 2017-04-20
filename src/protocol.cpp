/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
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
#include "users.h"
#include "servers.h"
#include "config.h"
#include "uplink.h"
#include "bots.h"
#include "channels.h"
#include "numeric.h"

IRCDProto *IRCD = nullptr;

IRCDProto::IRCDProto(Module *creator, const Anope::string &p) : Service(creator, NAME, p)
	, proto_name(p)
{
}

IRCDProto::~IRCDProto()
{
}

const Anope::string &IRCDProto::GetProtocolName() const
{
	return this->proto_name;
}

void IRCDProto::SendCTCPReply(const MessageSource &source, const Anope::string &dest, const Anope::string &buf)
{
	Anope::string s = Anope::NormalizeBuffer(buf);
	this->SendNotice(source, dest, "\1" + s + "\1");
}

void IRCDProto::SendNumeric(int numeric, User *dest, IRCMessage &message)
{
	Uplink::SendMessage(message);
}

void IRCDProto::SendAction(const MessageSource &source, const Anope::string &dest, const Anope::string &message)
{
	Anope::string actionbuf = "\1ACTION ";
	actionbuf.append(message);
	actionbuf.append('\1');

	SendPrivmsg(source, dest, actionbuf);
}

bool IRCDProto::IsNickValid(const Anope::string &nick)
{
	/**
	 * RFC: defination of a valid nick
	 * nickname =  ( letter / special ) ( letter / digit / special / "-" )
	 * letter   =  A-Z / a-z
	 * digit    =  0-9
	 * special  =  [, ], \, `, _, ^, {, |, }
	 **/

	 if (nick.empty())
	 	return false;

	Anope::string special = "[]\\`_^{|}";

	for (unsigned int i = 0; i < nick.length(); ++i)
		if (!(nick[i] >= 'A' && nick[i] <= 'Z') && !(nick[i] >= 'a' && nick[i] <= 'z')
		  && special.find(nick[i]) == Anope::string::npos
		  && (Config && Config->NickChars.find(nick[i]) == Anope::string::npos)
		  && (!i || (!(nick[i] >= '0' && nick[i] <= '9') && nick[i] != '-')))
			return false;

	return true;
}

bool IRCDProto::IsChannelValid(const Anope::string &chan)
{
	if (chan.empty() || chan[0] != '#' || chan.length() > Config->GetBlock("networkinfo")->Get<unsigned>("chanlen"))
		return false;

	if (chan.find_first_of(" ,") != Anope::string::npos)
		return false;

	return true;
}

bool IRCDProto::IsIdentValid(const Anope::string &ident)
{
	if (ident.empty() || ident.length() > Config->GetBlock("networkinfo")->Get<unsigned>("userlen"))
		return false;

	for (unsigned int i = 0; i < ident.length(); ++i)
	{
		const char &c = ident[i];

		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-')
			continue;

		return false;
	}

	return true;
}

bool IRCDProto::IsHostValid(const Anope::string &host)
{
	if (host.empty() || host.length() > Config->GetBlock("networkinfo")->Get<unsigned>("hostlen"))
		return false;

	const Anope::string &vhostdisablebe = Config->GetBlock("networkinfo")->Get<Anope::string>("disallow_start_or_end"),
		vhostchars = Config->GetBlock("networkinfo")->Get<Anope::string>("vhost_chars");

	if (vhostdisablebe.find_first_of(host[0]) != Anope::string::npos)
		return false;
	else if (vhostdisablebe.find_first_of(host[host.length() - 1]) != Anope::string::npos)
		return false;

	int dots = 0;
	for (unsigned int i = 0; i < host.length(); ++i)
	{
		if (host[i] == '.')
			++dots;
		if (vhostchars.find_first_of(host[i]) == Anope::string::npos)
			return false;
	}

	return dots > 0 || Config->GetBlock("networkinfo")->Get<bool>("allow_undotted_vhosts");
}

unsigned int IRCDProto::GetMaxListFor(Channel *c)
{
	return c->HasMode("LBAN") ? 0 : Config->GetBlock("networkinfo")->Get<int>("modelistsize");
}

Anope::string IRCDProto::NormalizeMask(const Anope::string &mask)
{
	if (IsExtbanValid(mask))
		return mask;
	return Entry("", mask).GetNUHMask();
}

MessageSource::MessageSource(const Anope::string &src) : source(src)
{
	/* no source for incoming message is our uplink */
	if (src.empty())
		this->s = Servers::GetUplink();
	else if (IRCD->RequiresID || src.find('.') != Anope::string::npos)
		this->s = Server::Find(src);
	if (this->s == NULL)
		this->u = User::Find(src);
}

MessageSource::MessageSource(User *_u) : source(_u ? _u->nick : ""), u(_u)
{
}

MessageSource::MessageSource(Server *_s) : source(_s ? _s->GetName() : ""), s(_s)
{
}

const Anope::string &MessageSource::GetName() const
{
	if (this->s)
		return this->s->GetName();
	else if (this->u)
		return this->u->nick;
	else
		return this->source;
}

const Anope::string &MessageSource::GetUID() const
{
	if (this->s)
		return this->s->GetSID();
	if (this->u)
		return this->u->GetUID();
	return this->source;
}

const Anope::string &MessageSource::GetSource() const
{
	return this->source;
}

User *MessageSource::GetUser() const
{
	return this->u;
}

ServiceBot *MessageSource::GetBot() const
{
	return ServiceBot::Find(this->GetName(), true);
}

Server *MessageSource::GetServer() const
{
	return this->s;
}

IRCDMessage::IRCDMessage(Module *o, const Anope::string &n, unsigned int p) : Service(o, NAME, o->name + "/" + n.lower())
	, name(n)
	, param_count(p)
{
}

unsigned int IRCDMessage::GetParamCount() const
{
	return this->param_count;
}

