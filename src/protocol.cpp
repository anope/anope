#include "services.h"

void IRCDProto::SendMessageInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &buf)
{
	if (Config->NSDefFlags.HasFlag(NI_MSG))
		SendPrivmsgInternal(bi, dest, buf);
	else
		SendNoticeInternal(bi, dest, buf);
}

void IRCDProto::SendNoticeInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &msg)
{
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "NOTICE %s :%s", dest.c_str(), msg.c_str());
}

void IRCDProto::SendPrivmsgInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &buf)
{
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "PRIVMSG %s :%s", dest.c_str(), buf.c_str());
}

void IRCDProto::SendQuitInternal(const User *u, const Anope::string &buf)
{
	if (!buf.empty())
		send_cmd(ircd->ts6 ? u->GetUID() : u->nick, "QUIT :%s", buf.c_str());
	else
		send_cmd(ircd->ts6 ? u->GetUID() : u->nick, "QUIT");
}

void IRCDProto::SendPartInternal(const BotInfo *bi, const Channel *chan, const Anope::string &buf)
{
	if (!buf.empty())
		send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "PART %s :%s", chan->name.c_str(), buf.c_str());
	else
		send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "PART %s", chan->name.c_str());
}

void IRCDProto::SendGlobopsInternal(const BotInfo *source, const Anope::string &buf)
{
	if (source)
		send_cmd(ircd->ts6 ? source->GetUID() : source->nick, "GLOBOPS :%s", buf.c_str());
	else
		send_cmd(Config->ServerName, "GLOBOPS :%s", buf.c_str());
}

void IRCDProto::SendCTCPInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &buf)
{
	Anope::string s = normalizeBuffer(buf);
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "NOTICE %s :\1%s\1", dest.c_str(), s.c_str());
}

void IRCDProto::SendNumericInternal(const Anope::string &source, int numeric, const Anope::string &dest, const Anope::string &buf)
{
	send_cmd(source, "%03d %s %s", numeric, dest.c_str(), buf.c_str());
}

void IRCDProto::SendSVSKill(const BotInfo *source, const User *user, const char *fmt, ...)
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

void IRCDProto::SendNoticeChanops(const BotInfo *bi, const Channel *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendNoticeChanopsInternal(bi, dest, buf);
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

void IRCDProto::SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg)
{
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "NOTICE %s%s :%s", ircd->globaltldprefix, dest->GetName().c_str(), msg.c_str());
}

void IRCDProto::SendGlobalPrivmsg(const BotInfo *bi, const Server *dest, const Anope::string &msg)
{
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "PRIVMSG %s%s :%s", ircd->globaltldprefix, dest->GetName().c_str(), msg.c_str());
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
		send_cmd(ircd->ts6 ? TS6SID : Config->ServerName, "PING %s", who.c_str());
	else
		send_cmd(ircd->ts6 ? TS6SID : Config->ServerName, "PING %s %s", servname.c_str(), who.c_str());
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
		send_cmd(ircd->ts6 ? TS6SID : Config->ServerName, "PONG %s", who.c_str());
	else
		send_cmd(ircd->ts6 ? TS6SID : Config->ServerName, "PONG %s %s", servname.c_str(), who.c_str());
}

void IRCDProto::SendJoin(BotInfo *bi, const ChannelContainer *cc)
{
	SendJoin(bi, cc->chan->name, cc->chan->creation_time);
}

void IRCDProto::SendInvite(const BotInfo *bi, const Anope::string &chan, const Anope::string &nick)
{
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "INVITE %s %s", nick.c_str(), chan.c_str());
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

void IRCDProto::SendSquit(const Anope::string &servname, const Anope::string &message)
{
	send_cmd("", "SQUIT %s :%s", servname.c_str(), message.c_str());
}

void IRCDProto::SendChangeBotNick(const BotInfo *bi, const Anope::string &newnick)
{
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "NICK %s %ld", newnick.c_str(), static_cast<long>(Anope::CurTime));
}

void IRCDProto::SendForceNickChange(const User *u, const Anope::string &newnick, time_t when)
{
	send_cmd("", "SVSNICK %s %s :%ld", u->nick.c_str(), newnick.c_str(), static_cast<long>(when));
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

void IRCDProto::SendNumeric(const Anope::string &source, int numeric, const Anope::string &dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendNumericInternal(source, numeric, dest, buf);
}

bool IRCDProto::IsChannelValid(const Anope::string &chan)
{
	if (chan[0] != '#')
		return false;

	return true;
}
