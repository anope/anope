#include "services.h"

void IRCDProto::SendMessageInternal(BotInfo *bi, const char *dest, const char *buf)
{
	if (Config.NSDefFlags.HasFlag(NI_MSG))
		SendPrivmsgInternal(bi, dest, buf);
	else
		SendNoticeInternal(bi, dest, buf);
}

void IRCDProto::SendNoticeInternal(BotInfo *bi, const char *dest, const char *msg)
{
	send_cmd(ircd->ts6 ? bi->uid : bi->nick, "NOTICE %s :%s", dest, msg);
}

void IRCDProto::SendPrivmsgInternal(BotInfo *bi, const char *dest, const char *buf)
{
	send_cmd(ircd->ts6 ? bi->uid : bi->nick, "PRIVMSG %s :%s", dest, buf);
}

void IRCDProto::SendQuitInternal(BotInfo *bi, const char *buf)
{
	if (buf)
		send_cmd(ircd->ts6 ? bi->uid : bi->nick, "QUIT :%s", buf);
	else
		send_cmd(ircd->ts6 ? bi->uid : bi->nick, "QUIT");
}

void IRCDProto::SendPartInternal(BotInfo *bi, Channel *chan, const char *buf)
{
	if (buf)
		send_cmd(ircd->ts6 ? bi->uid : bi->nick, "PART %s :%s", chan->name, buf);
	else
		send_cmd(ircd->ts6 ? bi->uid : bi->nick, "PART %s", chan->name);
}

void IRCDProto::SendGlobopsInternal(BotInfo *source, const char *buf)
{
	if (source)
		send_cmd(ircd->ts6 ? source->uid : source->nick, "GLOBOPS :%s", buf);
	else
		send_cmd(Config.ServerName, "GLOBOPS :%s", buf);
}

void IRCDProto::SendCTCPInternal(BotInfo *bi, const char *dest, const char *buf)
{
	char *s = normalizeBuffer(buf);
	send_cmd(ircd->ts6 ? bi->uid : bi->nick, "NOTICE %s :\1%s\1", dest, s);
	delete [] s;
}

void IRCDProto::SendNumericInternal(const char *source, int numeric, const char *dest, const char *buf)
{
	send_cmd(source, "%03d %s %s", numeric, dest, buf);
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

void IRCDProto::SendMode(BotInfo *bi, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendModeInternal(bi, dest, buf);
}

void IRCDProto::SendMode(User *u, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendModeInternal(u, buf);
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

void IRCDProto::SendMessage(BotInfo *bi, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendMessageInternal(bi, dest, buf);
}

void IRCDProto::SendNotice(BotInfo *bi, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendNoticeInternal(bi, dest, buf);
}

void IRCDProto::SendAction(BotInfo *bi, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "", actionbuf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	snprintf(actionbuf, BUFSIZE - 1, "%cACTION %s%c", 1, buf, 1);
	SendPrivmsgInternal(bi, dest, actionbuf);
}

void IRCDProto::SendPrivmsg(BotInfo *bi, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendPrivmsgInternal(bi, dest, buf);
}

void IRCDProto::SendGlobalNotice(BotInfo *bi, Server *dest, const char *msg)
{
	send_cmd(ircd->ts6 ? bi->uid : bi->nick, "NOTICE %s%s :%s", ircd->globaltldprefix, dest->name, msg);
}

void IRCDProto::SendGlobalPrivmsg(BotInfo *bi, Server *dest, const char *msg)
{
	send_cmd(ircd->ts6 ? bi->uid : bi->nick, "PRIVMSG %s%s :%s", ircd->globaltldprefix, dest->name, msg);
}

void IRCDProto::SendQuit(const char *nick, const char *)
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

/**
 * Send a PONG reply to a received PING.
 * servname should be left NULL to send a one param reply.
 * @param servname Daemon or client that is responding to the PING.
 * @param who Origin of the PING and destination of the PONG message.
 **/
void IRCDProto::SendPong(const char *servname, const char *who)
{
	if (!servname)
		send_cmd(ircd->ts6 ? TS6SID : Config.ServerName, "PONG %s", who);
	else 
		send_cmd(ircd->ts6 ? TS6SID : Config.ServerName, "PONG %s %s", servname, who);
}

void IRCDProto::SendInvite(BotInfo *bi, const char *chan, const char *nick)
{
	send_cmd(ircd->ts6 ? bi->uid : bi->nick, "INVITE %s %s", nick, chan);
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
	else SendPartInternal(bi, chan, NULL);
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

void IRCDProto::SendSquit(const char *servname, const char *message)
{
	send_cmd(NULL, "SQUIT %s :%s", servname, message);
}

void IRCDProto::SendChangeBotNick(BotInfo *bi, const char *newnick)
{
	send_cmd(ircd->ts6 ? bi->uid : bi->nick, "NICK %s %ld", newnick, static_cast<long>(time(NULL)));
}
void IRCDProto::SendForceNickChange(User *u, const char *newnick, time_t when)
{
	send_cmd(NULL, "SVSNICK %s %s :%ld", u->nick, newnick, static_cast<long>(when));
}

void IRCDProto::SendCTCP(BotInfo *bi, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendCTCPInternal(bi, dest, buf);
}

void IRCDProto::SendNumeric(const char *source, int numeric, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendNumericInternal(source, numeric, dest, buf);
}

int IRCDProto::IsChannelValid(const char *chan)
{
	if (*chan != '#')
		return 0;

	return 1;
}
