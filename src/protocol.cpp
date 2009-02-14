#include "services.h"

void IRCDProto::SendMessageInternal(BotInfo *bi, const char *dest, const char *buf)
{
	if (NSDefFlags & NI_MSG)
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

void IRCDProto::SendPartInternal(BotInfo *bi, const char *chan, const char *buf)
{
	if (buf)
		send_cmd(ircd->ts6 ? bi->uid : bi->nick, "PART %s :%s", chan, buf);
	else
		send_cmd(ircd->ts6 ? bi->uid : bi->nick, "PART %s", chan);
}

void IRCDProto::SendGlobopsInternal(const char *source, const char *buf)
{
	BotInfo *bi = findbot(source);
	if (bi)
		send_cmd(ircd->ts6 ? bi->uid : bi->nick, "GLOBOPS :%s", buf);
	else
		send_cmd(ServerName, "GLOBOPS :%s", buf);
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

void IRCDProto::SendSVSKill(const char *source, const char *user, const char *fmt, ...)
{
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

void IRCDProto::SendKick(BotInfo *bi, const char *chan, const char *user, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, fmt, args);
	va_end(args);
	SendKickInternal(bi, chan, user, buf);
}

void IRCDProto::SendNoticeChanops(BotInfo *bi, const char *dest, const char *fmt, ...)
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

void IRCDProto::SendGlobalNotice(BotInfo *bi, const char *dest, const char *msg)
{
	send_cmd(ircd->ts6 ? bi->uid : bi->nick, "NOTICE %s%s :%s", ircd->globaltldprefix, dest, msg);
}

void IRCDProto::SendGlobalPrivmsg(BotInfo *bi, const char *dest, const char *msg)
{
	send_cmd(ircd->ts6 ? bi->uid : bi->nick, "PRIVMSG %s%s :%s", ircd->globaltldprefix, dest, msg);
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
void IRCDProto::SendPong(const char *servname, const char *who)
{
	send_cmd(servname, "PONG %s", who);
}

void IRCDProto::SendInvite(BotInfo *bi, const char *chan, const char *nick)
{
	send_cmd(ircd->ts6 ? bi->uid : bi->nick, "INVITE %s %s", nick, chan);
}

void IRCDProto::SendPart(BotInfo *bi, const char *chan, const char *fmt, ...)
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

void IRCDProto::SendGlobops(const char *source, const char *fmt, ...)
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
	send_cmd(ircd->ts6 ? bi->uid : bi->nick, "NICK %s", newnick);
}
void IRCDProto::SendForceNickChange(const char *oldnick, const char *newnick, time_t when)
{
	send_cmd(NULL, "SVSNICK %s %s :%ld", oldnick, newnick, static_cast<long>(when));
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
	SendNumericInternal(source, numeric, dest, *buf ? buf : NULL);
}

