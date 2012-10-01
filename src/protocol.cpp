/*
 *
 * (C) 2003-2012 Anope Team
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
#include "users.h"
#include "servers.h"
#include "config.h"
#include "uplink.h"
#include "bots.h"
#include "extern.h"
#include "channels.h"

IRCDProto *ircdproto;

IRCDProto::IRCDProto(const Anope::string &p) : proto_name(p)
{
	DefaultPseudoclientModes = "+io";
	CanSVSNick = CanSetVHost = CanSetVIdent = CanSNLine = CanSQLine = CanSQLineChannel = CanSZLine = CanSVSHold =
		CanSVSO = CanCertFP = RequiresID = false;
	MaxModes = 3;

	ircdproto = this;
}

IRCDProto::~IRCDProto()
{
	if (ircdproto == this)
		ircdproto = NULL;
}

const Anope::string &IRCDProto::GetProtocolName()
{
	return this->proto_name;
}

void IRCDProto::SendSVSKillInternal(const BotInfo *source, User *user, const Anope::string &buf)
{
	UplinkSocket::Message(source) << "KILL " << user->GetUID() << " :" << buf;
}

void IRCDProto::SendModeInternal(const BotInfo *bi, const Channel *dest, const Anope::string &buf)
{
	UplinkSocket::Message(bi) << "MODE " << dest->name << " " << buf;
}

void IRCDProto::SendKickInternal(const BotInfo *bi, const Channel *c, const User *u, const Anope::string &r)
{
	if (!r.empty())
		UplinkSocket::Message(bi) << "KICK " << c->name << " " << u->GetUID() << " :" << r;
	else
		UplinkSocket::Message(bi) << "KICK " << c->name << " " << u->GetUID();
}

void IRCDProto::SendMessageInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &buf)
{
	if (Config->NSDefFlags.HasFlag(NI_MSG))
		SendPrivmsgInternal(bi, dest, buf);
	else
		SendNoticeInternal(bi, dest, buf);
}

void IRCDProto::SendNoticeInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &msg)
{
	UplinkSocket::Message(bi) << "NOTICE " << dest << " :" << msg;
}

void IRCDProto::SendPrivmsgInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &buf)
{
	UplinkSocket::Message(bi) << "PRIVMSG " << dest << " :" << buf;
}

void IRCDProto::SendQuitInternal(const User *u, const Anope::string &buf)
{
	if (!buf.empty())
		UplinkSocket::Message(u) << "QUIT :" << buf;
	else
		UplinkSocket::Message(u) << "QUIT";
}

void IRCDProto::SendPartInternal(const BotInfo *bi, const Channel *chan, const Anope::string &buf)
{
	if (!buf.empty())
		UplinkSocket::Message(bi) << "PART " << chan->name << " :" << buf;
	else
		UplinkSocket::Message(bi) << "PART " << chan->name;
}

void IRCDProto::SendGlobopsInternal(const BotInfo *source, const Anope::string &buf)
{
	if (source)
		UplinkSocket::Message(source) << "GLOBOPS :" << buf;
	else
		UplinkSocket::Message(Me) << "GLOBOPS :" << buf;
}

void IRCDProto::SendCTCPInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &buf)
{
	Anope::string s = normalizeBuffer(buf);
	this->SendNoticeInternal(bi, dest, "\1" + s + "\1");
}

void IRCDProto::SendNumericInternal(int numeric, const Anope::string &dest, const Anope::string &buf)
{
	Anope::string n = stringify(numeric);
	if (numeric < 10)
		n = "0" + n;
	if (numeric < 100)
		n = "0" + n;
	UplinkSocket::Message(Me) << n << " " << dest << " " << buf;
}

void IRCDProto::SendTopic(BotInfo *bi, Channel *c)
{
	UplinkSocket::Message(bi) << "TOPIC " << c->name << " :" << c->topic;
}

void IRCDProto::SendSVSKill(const BotInfo *source, User *user, const char *fmt, ...)
{
	if (!user || !fmt)
		return;

	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendSVSKillInternal(source, user, buf);
}

void IRCDProto::SendMode(const BotInfo *bi, const Channel *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendModeInternal(bi, dest, buf);
}

void IRCDProto::SendMode(const BotInfo *bi, const User *u, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendModeInternal(bi, u, buf);
}

void IRCDProto::SendKick(const BotInfo *bi, const Channel *chan, const User *user, const char *fmt, ...)
{
	if (!bi || !chan || !user)
		return;

	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendKickInternal(bi, chan, user, buf);
}

void IRCDProto::SendMessage(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendMessageInternal(bi, dest, buf);
}

void IRCDProto::SendNotice(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendNoticeInternal(bi, dest, buf);
}

void IRCDProto::SendAction(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	Anope::string actionbuf = Anope::string("\1ACTION ") + buf + '\1';
	SendPrivmsgInternal(bi, dest, actionbuf);
}

void IRCDProto::SendPrivmsg(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendPrivmsgInternal(bi, dest, buf);
}

void IRCDProto::SendQuit(const User *u, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendQuitInternal(u, buf);
}

void IRCDProto::SendPing(const Anope::string &servname, const Anope::string &who)
{
	if (servname.empty())
		UplinkSocket::Message(Me) << "PING " << who;
	else
		UplinkSocket::Message(Me) << "PING " << servname << " " << who;
}

/**
 * Send a PONG reply to a received PING.
 * servname should be left NULL to send a one param reply.
 * @param servname Daemon or client that is responding to the PING.
 * @param who Origin of the PING and destination of the PONG message.
 **/
void IRCDProto::SendPong(const Anope::string &servname, const Anope::string &who)
{
	if (servname.empty())
		UplinkSocket::Message(Me) << "PONG " << who;
	else
		UplinkSocket::Message(Me) << "PONG " << servname << " " << who;
}

void IRCDProto::SendInvite(const BotInfo *bi, const Channel *c, const User *u)
{
	UplinkSocket::Message(bi) << "INVITE " << u->GetUID() << " " << c->name;
}

void IRCDProto::SendPart(const BotInfo *bi, const Channel *chan, const char *fmt, ...)
{
	if (fmt)
	{
		va_list args;
		char buf[BUFSIZE] = "";
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
		SendPartInternal(bi, chan, buf);
	}
	else
		SendPartInternal(bi, chan, "");
}

void IRCDProto::SendGlobops(const BotInfo *source, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendGlobopsInternal(source, buf);
}

void IRCDProto::SendSquit(Server *s, const Anope::string &message)
{
	UplinkSocket::Message() << "SQUIT " << s->GetName() << " :" << message;
}

void IRCDProto::SendChangeBotNick(const BotInfo *bi, const Anope::string &newnick)
{
	UplinkSocket::Message(bi) << "NICK " << newnick << " " << Anope::CurTime;
}

void IRCDProto::SendForceNickChange(const User *u, const Anope::string &newnick, time_t when)
{
	UplinkSocket::Message() << "SVSNICK " << u->nick << " " << newnick << " " << when;
}

void IRCDProto::SendCTCP(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendCTCPInternal(bi, dest, buf);
}

void IRCDProto::SendNumeric(int numeric, const Anope::string &dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendNumericInternal(numeric, dest, buf);
}

bool IRCDProto::IsChannelValid(const Anope::string &chan)
{
	if (chan[0] != '#')
		return false;

	return true;
}

MessageSource::MessageSource(const Anope::string &src) : source(src), u(NULL), s(NULL)
{
	if (src.empty())
		this->s = !Me->GetLinks().empty() ? Me->GetLinks().front() : NULL;
	else if (ircdproto->RequiresID || src.find('.') != Anope::string::npos)
		this->s = Server::Find(src);
	if (this->s == NULL)
		this->u = finduser(src);
}

MessageSource::MessageSource(User *_u) : source(_u->nick), u(_u), s(NULL)
{
}

MessageSource::MessageSource(Server *_s) : source(_s->GetName()), u(NULL), s(_s)
{
}

const Anope::string MessageSource::GetName()
{
	if (this->s)
		return this->s->GetName();
	else if (this->u)
		return this->u->nick;
	else
		return this->source;
}

const Anope::string &MessageSource::GetSource()
{
	return this->source;
}

User *MessageSource::GetUser()
{
	return this->u;
}

Server *MessageSource::GetServer()
{
	return this->s;
}

std::map<Anope::string, std::vector<IRCDMessage *> > IRCDMessage::messages;

const std::vector<IRCDMessage *> *IRCDMessage::Find(const Anope::string &name)
{
	std::map<Anope::string, std::vector<IRCDMessage *> >::iterator it = messages.find(name);
	if (it != messages.end())
		return &it->second;
	return NULL;
}

IRCDMessage::IRCDMessage(const Anope::string &n, unsigned p) : name(n), param_count(p)
{
	messages[n].insert(messages[n].begin(), this);
}

IRCDMessage::~IRCDMessage()
{
	std::vector<IRCDMessage *>::iterator it = std::find(messages[this->name].begin(), messages[this->name].end(), this);
	if (it != messages[this->name].end())
		messages[this->name].erase(it);
}

unsigned IRCDMessage::GetParamCount() const
{
	return this->param_count;
}

