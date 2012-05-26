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
IRCDVar *ircd;
IRCdMessage *ircdmessage;

void pmodule_ircd_proto(IRCDProto *proto)
{
	ircdproto = proto;
}

void pmodule_ircd_var(IRCDVar *ircdvar)
{
	ircd = ircdvar;
}

void pmodule_ircd_message(IRCdMessage *message)
{
	ircdmessage = message;
}

void IRCDProto::SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf)
{
	UplinkSocket::Message(source) << "KILL " << (ircd->ts6 ? user->GetUID() : user->nick) << " :" << buf;
}

void IRCDProto::SendModeInternal(const BotInfo *bi, const Channel *dest, const Anope::string &buf)
{
	UplinkSocket::Message(bi) << "MODE " << dest->name << " " << buf;
}

void IRCDProto::SendKickInternal(const BotInfo *bi, const Channel *c, const User *u, const Anope::string &r)
{
	if (!r.empty())
		UplinkSocket::Message(bi) << "KICK " << c->name << " " << (ircd->ts6 ? u->GetUID() : u->nick) << " :" << r;
	else
		UplinkSocket::Message(bi) << "KICK " << c->name << " " << (ircd->ts6 ? u->GetUID() : u->nick);
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
	UplinkSocket::Message(bi) << "NOTICE " << ircd->globaltldprefix << dest->GetName() << " :" << msg;
}

void IRCDProto::SendGlobalPrivmsg(const BotInfo *bi, const Server *dest, const Anope::string &msg)
{
	UplinkSocket::Message(bi) << "PRIVMSG " << ircd->globaltldprefix << dest->GetName() << " :" << msg;
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
	UplinkSocket::Message(bi) << "INVITE " << (ircd->ts6 ? u->GetUID() : u->nick) << " " << c->name;
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

bool IRCdMessage::On436(const Anope::string &, const std::vector<Anope::string> &params)
{
	if (!params.empty())
		introduce_user(params[0]);
	return true;
}

bool IRCdMessage::OnAway(const Anope::string &source, const std::vector<Anope::string> &params)
{
	User *u = finduser(source);
	if (u)
	{
		FOREACH_MOD(I_OnUserAway, OnUserAway(u, params.empty() ? "" : params[0]));
	}
	return true;
}

bool IRCdMessage::OnJoin(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (!params.empty())
		do_join(source, params[0], params.size() > 1 ? params[1] : "");
	return true;
}

bool IRCdMessage::OnKick(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() > 2)
		do_kick(source, params[0], params[1], params[2]);
	return true;
}

/** Called on KILL
 * @params[0] The nick
 * @params[1] The reason
 */
bool IRCdMessage::OnKill(const Anope::string &source, const std::vector<Anope::string> &params)
{
	User *u = finduser(params[0]);
	BotInfo *bi;

	if (!u)
		return true;

	/* Recover if someone kills us. */
	if (u->server == Me && (bi = dynamic_cast<BotInfo *>(u)))
	{
		bi->introduced = false;
		introduce_user(bi->nick);
		bi->RejoinAll();
	}
	else
		do_kill(u, params[1]);
	
	
	return true;
}

bool IRCdMessage::OnUID(const Anope::string &source, const std::vector<Anope::string> &params)
{
	return true;
}

bool IRCdMessage::OnPart(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (!params.empty())
		do_part(source, params[0], params.size() > 1 ? params[1] : "");
	return true;
}

bool IRCdMessage::OnPing(const Anope::string &, const std::vector<Anope::string> &params)
{
	if (!params.empty())
		ircdproto->SendPong(params.size() > 1 ? params[1] : Config->ServerName, params[0]);
	return true;
}

bool IRCdMessage::OnPrivmsg(const Anope::string &source, const std::vector<Anope::string> &params)
{
	const Anope::string &receiver = params.size() > 0 ? params[0] : "";
	Anope::string message = params.size() > 1 ? params[1] : "";

	/* Messages from servers can happen on some IRCds, check for . */
	if (source.empty() || receiver.empty() || message.empty() || source.find('.') != Anope::string::npos)
		return true;

	User *u = finduser(source);

	if (!u)
	{
		Log() << message << ": user record for " << source << " not found";

		const BotInfo *bi = findbot(receiver);
		if (bi)
			ircdproto->SendMessage(bi, source, "%s", "Internal error - unable to process request.");

		return true;
	}

	if (receiver[0] == '#')
	{
		Channel *c = findchan(receiver);
		if (c)
		{
			FOREACH_MOD(I_OnPrivmsg, OnPrivmsg(u, c, message));
		}
	}
	else
	{
		/* If a server is specified (nick@server format), make sure it matches
		 * us, and strip it off. */
		Anope::string botname = receiver;
		size_t s = receiver.find('@');
		if (s != Anope::string::npos)
		{
			Anope::string servername(receiver.begin() + s + 1, receiver.end());
			botname = botname.substr(0, s);
			if (!servername.equals_ci(Config->ServerName))
				return true;
		}
		else if (Config->UseStrictPrivMsg)
		{
			const BotInfo *bi = findbot(receiver);
			if (!bi)
				return true;
			Log(LOG_DEBUG) << "Ignored PRIVMSG without @ from " << source;
			u->SendMessage(bi, _("\"/msg %s\" is no longer supported.  Use \"/msg %s@%s\" or \"/%s\" instead."), receiver.c_str(), receiver.c_str(), Config->ServerName.c_str(), receiver.c_str());
			return true;
		}

		BotInfo *bi = findbot(botname);

		if (bi)
		{
			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnBotPrivmsg, OnBotPrivmsg(u, bi, message));
			if (MOD_RESULT == EVENT_STOP)
				return true;

			if (message[0] == '\1' && message[message.length() - 1] == '\1')
			{
				if (message.substr(0, 6).equals_ci("\1PING "))
				{
					Anope::string buf = message;
					buf.erase(buf.begin());
					buf.erase(buf.end() - 1);
					ircdproto->SendCTCP(bi, u->nick, "%s", buf.c_str());
				}
				else if (message.substr(0, 9).equals_ci("\1VERSION\1"))
				{
					Module *enc = ModuleManager::FindFirstOf(ENCRYPTION);
					ircdproto->SendCTCP(bi, u->nick, "VERSION Anope-%s %s :%s - (%s) -- %s", Anope::Version().c_str(), Config->ServerName.c_str(), ircd->name, enc ? enc->name.c_str() : "unknown", Anope::VersionBuildString().c_str());
				}
				return true;
			}
			
			bi->OnMessage(u, message);
		}
	}

	return true;
}

bool IRCdMessage::OnQuit(const Anope::string &source, const std::vector<Anope::string> &params)
{
	const Anope::string &reason = !params.empty() ? params[0] : "";
	User *user = finduser(source);
	if (!user)
	{
		Log() << "user: QUIT from nonexistent user " << source << " (" << reason << ")";
		return true;
	}

	Log(user, "quit") << "quit (Reason: " << (!reason.empty() ? reason : "no reason") << ")";

	NickAlias *na = findnick(user->nick);
	if (na && !na->nc->HasFlag(NI_SUSPENDED) && (user->IsRecognized() || user->IsIdentified(true)))
	{
		na->last_seen = Anope::CurTime;
		na->last_quit = reason;
	}
	FOREACH_MOD(I_OnUserQuit, OnUserQuit(user, reason));
	delete user;

	return true;
}

bool IRCdMessage::OnSQuit(const Anope::string &source, const std::vector<Anope::string> &params)
{
	const Anope::string &server = params[0];

	Server *s = Server::Find(server);

	if (!s)
	{
		Log() << "SQUIT for nonexistent server " << server;
		return true;
	}

	FOREACH_MOD(I_OnServerQuit, OnServerQuit(s));

	Anope::string buf = s->GetName() + " " + s->GetUplink()->GetName();

	if (s->GetUplink() == Me && Capab.count("UNCONNECT") > 0)
	{
		Log(LOG_DEBUG) << "Sending UNCONNECT SQUIT for " << s->GetName();
		/* need to fix */
		ircdproto->SendSquit(s, buf);
	}

	s->Delete(buf);

	return true;
}

bool IRCdMessage::OnWhois(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (!source.empty() && !params.empty())
	{
		User *u = finduser(params[0]);
		if (u && u->server == Me)
		{
			const BotInfo *bi = findbot(u->nick);
			ircdproto->SendNumeric(311, source, "%s %s %s * :%s", u->nick.c_str(), u->GetIdent().c_str(), u->host.c_str(), u->realname.c_str());
			if (bi)
				ircdproto->SendNumeric(307, source, "%s :is a registered nick", bi->nick.c_str());
			ircdproto->SendNumeric(312, source, "%s %s :%s", u->nick.c_str(), Config->ServerName.c_str(), Config->ServerDesc.c_str());
			if (bi)
				ircdproto->SendNumeric(317, source, "%s %ld %ld :seconds idle, signon time", bi->nick.c_str(), static_cast<long>(Anope::CurTime - bi->lastmsg), static_cast<long>(start_time));
			ircdproto->SendNumeric(318, source, "%s :End of /WHOIS list.", params[0].c_str());
		}
		else
			ircdproto->SendNumeric(401, source, "%s :No such user.", params[0].c_str());
	}

	return true;
}

bool IRCdMessage::OnCapab(const Anope::string &, const std::vector<Anope::string> &params)
{
	for (unsigned i = 0; i < params.size(); ++i)
		Capab.insert(params[i]);
	return true;
}

bool IRCdMessage::OnError(const Anope::string &, const std::vector<Anope::string> &params)
{
	if (!params.empty())
	{
		Log(LOG_TERMINAL) << "ERROR: " << params[0];
		quitmsg = "Received ERROR from uplink: " + params[0];
	}
	
	return true;
}

