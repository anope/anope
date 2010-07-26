#include "services.h"

void IRCDProto::SendMessageInternal(BotInfo *bi, const Anope::string &dest, const Anope::string &buf)
{
	if (Config.NSDefFlags.HasFlag(NI_MSG))
		SendPrivmsgInternal(bi, dest, buf);
	else
		SendNoticeInternal(bi, dest, buf);
}

void IRCDProto::SendNoticeInternal(BotInfo *bi, const Anope::string &dest, const Anope::string &msg)
{
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "NOTICE %s :%s", dest.c_str(), msg.c_str());
}

void IRCDProto::SendPrivmsgInternal(BotInfo *bi, const Anope::string &dest, const Anope::string &buf)
{
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "PRIVMSG %s :%s", dest.c_str(), buf.c_str());
}

void IRCDProto::SendQuitInternal(BotInfo *bi, const Anope::string &buf)
{
	if (!buf.empty())
		send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "QUIT :%s", buf.c_str());
	else
		send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "QUIT");
}

void IRCDProto::SendPartInternal(BotInfo *bi, Channel *chan, const Anope::string &buf)
{
	if (!buf.empty())
		send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "PART %s :%s", chan->name.c_str(), buf.c_str());
	else
		send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "PART %s", chan->name.c_str());
}

void IRCDProto::SendGlobopsInternal(BotInfo *source, const Anope::string &buf)
{
	if (source)
		send_cmd(ircd->ts6 ? source->GetUID() : source->nick, "GLOBOPS :%s", buf.c_str());
	else
		send_cmd(Config.ServerName, "GLOBOPS :%s", buf.c_str());
}

void IRCDProto::SendCTCPInternal(BotInfo *bi, const Anope::string &dest, const Anope::string &buf)
{
	Anope::string s = normalizeBuffer(buf);
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "NOTICE %s :\1%s\1", dest.c_str(), s.c_str());
}

void IRCDProto::SendNumericInternal(const Anope::string &source, int numeric, const Anope::string &dest, const Anope::string &buf)
{
	send_cmd(source, "%03d %s %s", numeric, dest.c_str(), buf.c_str());
}

void IRCDProto::SendSVSKill(BotInfo *source, User *user, const char *fmt, ...)
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

void IRCDProto::SendMode(BotInfo *bi, Channel *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendModeInternal(bi, dest, buf);
}

void IRCDProto::SendMode(BotInfo *bi, User *u, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendModeInternal(bi, u, buf);
}

void IRCDProto::SendKick(BotInfo *bi, Channel *chan, User *user, const char *fmt, ...)
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

void IRCDProto::SendNoticeChanops(BotInfo *bi, Channel *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendNoticeChanopsInternal(bi, dest, buf);
}

void IRCDProto::SendMessage(BotInfo *bi, const Anope::string &dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendMessageInternal(bi, dest, buf);
}

void IRCDProto::SendNotice(BotInfo *bi, const Anope::string &dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendNoticeInternal(bi, dest, buf);
}

void IRCDProto::SendAction(BotInfo *bi, const Anope::string &dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	Anope::string actionbuf = Anope::string("\1ACTION ") + buf + '\1';
	SendPrivmsgInternal(bi, dest, actionbuf);
}

void IRCDProto::SendPrivmsg(BotInfo *bi, const Anope::string &dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendPrivmsgInternal(bi, dest, buf);
}

void IRCDProto::SendGlobalNotice(BotInfo *bi, Server *dest, const Anope::string &msg)
{
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "NOTICE %s%s :%s", ircd->globaltldprefix, dest->GetName().c_str(), msg.c_str());
}

void IRCDProto::SendGlobalPrivmsg(BotInfo *bi, Server *dest, const Anope::string &msg)
{
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "PRIVMSG %s%s :%s", ircd->globaltldprefix, dest->GetName().c_str(), msg.c_str());
}

void IRCDProto::SendQuit(const Anope::string &nick, const Anope::string &)
{
	send_cmd(nick, "QUIT");
}

void IRCDProto::SendQuit(BotInfo *bi, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendQuitInternal(bi, buf);
}

void IRCDProto::SendPing(const Anope::string &servname, const Anope::string &who)
{
	if (servname.empty())
		send_cmd(ircd->ts6 ? TS6SID : Config.ServerName, "PING %s", who.c_str());
	else
		send_cmd(ircd->ts6 ? TS6SID : Config.ServerName, "PING %s %s", servname.c_str(), who.c_str());
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
		send_cmd(ircd->ts6 ? TS6SID : Config.ServerName, "PONG %s", who.c_str());
	else
		send_cmd(ircd->ts6 ? TS6SID : Config.ServerName, "PONG %s %s", servname.c_str(), who.c_str());
}

void IRCDProto::SendInvite(BotInfo *bi, const Anope::string &chan, const Anope::string &nick)
{
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "INVITE %s %s", nick.c_str(), chan.c_str());
}

void IRCDProto::SendPart(BotInfo *bi, Channel *chan, const char *fmt, ...)
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

void IRCDProto::SendGlobops(BotInfo *source, const char *fmt, ...)
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

void IRCDProto::SendChangeBotNick(BotInfo *bi, const Anope::string &newnick)
{
	send_cmd(ircd->ts6 ? bi->GetUID() : bi->nick, "NICK %s %ld", newnick.c_str(), static_cast<long>(time(NULL)));
}

void IRCDProto::SendForceNickChange(User *u, const Anope::string &newnick, time_t when)
{
	send_cmd("", "SVSNICK %s %s :%ld", u->nick.c_str(), newnick.c_str(), static_cast<long>(when));
}

void IRCDProto::SendCTCP(BotInfo *bi, const Anope::string &dest, const char *fmt, ...)
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
