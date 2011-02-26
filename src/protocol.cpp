#include "services.h"
#include "modules.h"

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
		send_cmd(ircd->ts6 ? Config->Numeric : Config->ServerName, "PING %s", who.c_str());
	else
		send_cmd(ircd->ts6 ? Config->Numeric : Config->ServerName, "PING %s %s", servname.c_str(), who.c_str());
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
		send_cmd(ircd->ts6 ? Config->Numeric : Config->ServerName, "PONG %s", who.c_str());
	else
		send_cmd(ircd->ts6 ? Config->Numeric : Config->ServerName, "PONG %s %s", servname.c_str(), who.c_str());
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

bool IRCdMessage::On436(const Anope::string &, const std::vector<Anope::string> &params)
{
	if (!params.empty())
		introduce_user(params[0]);
	return true;
}

bool IRCdMessage::OnAway(const Anope::string &source, const std::vector<Anope::string> &params)
{
	User *u = finduser(source);
	if (u && params.empty()) /* un-away */
		check_memos(u);
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
	if (!Config->s_BotServ.empty() && u->server == Me && (bi = dynamic_cast<BotInfo *>(u)))
	{
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
	const Anope::string &message = params.size() > 1 ? params[1] : "";

	/* Messages from servers can happen on some IRCds, check for . */
	if (source.empty() || receiver.empty() || message.empty() || source.find('.') != Anope::string::npos)
		return true;

	User *u = finduser(source);

	if (!u)
	{
		Log() << message << ": user record for " << source << " not found";

		BotInfo *bi = findbot(receiver);
		if (bi)
			ircdproto->SendMessage(bi, source, "%s", "Internal error - unable to process request.");

		return true;
	}

	if (receiver[0] == '#' && !Config->s_BotServ.empty())
	{
		ChannelInfo *ci = cs_findchan(receiver);
		/* Some paranoia checks */
		if (ci && !ci->HasFlag(CI_FORBIDDEN) && ci->c && ci->bi)
			botchanmsgs(u, ci, message);
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
			BotInfo *bi = findbot(receiver);
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
					ircdproto->SendCTCP(bi, u->nick, "VERSION Anope-%s %s :%s - (%s) -- %s", Anope::Version().c_str(), Config->ServerName.c_str(), ircd->name, Config->EncModuleList.begin()->c_str(), Anope::Build().c_str());
				}
			}
			else if (bi == ChanServ)
			{
				if (!u->HasMode(UMODE_OPER) && Config->CSOpersOnly)
					u->SendMessage(ChanServ, LanguageString::ACCESS_DENIED);
				else
					mod_run_cmd(bi, u, message, false);
			}
			else if (bi == HostServ)
			{
				if (!ircd->vhost)
					u->SendMessage(HostServ, _("%s is currently offline."), Config->s_HostServ.c_str());
				else
					mod_run_cmd(bi, u, message, false);
			}
			else if (bi == OperServ)
			{
				if (!u->HasMode(UMODE_OPER) && Config->OSOpersOnly)
				{
					u->SendMessage(OperServ, LanguageString::ACCESS_DENIED);
					if (Config->WallBadOS)
						ircdproto->SendGlobops(OperServ, "Denied access to %s from %s!%s@%s (non-oper)", Config->s_OperServ.c_str(), u->nick.c_str(), u->GetIdent().c_str(), u->host.c_str());
				}
				else
				{
					Log(OperServ) << u->nick << ": " <<  message;
					mod_run_cmd(bi, u, message, false);
				}
			}
			else
				mod_run_cmd(bi, u, message, false);
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
	if (na && !na->HasFlag(NS_FORBIDDEN) && !na->nc->HasFlag(NI_SUSPENDED) && (user->IsRecognized() || user->IsIdentified(true)))
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

	Anope::string buf;
	/* If this is a juped server, send a nice global to inform the online
	 * opers that we received it.
	 */
	if (s->HasFlag(SERVER_JUPED))
	{
		buf = "Received SQUIT for juped server " + s->GetName();
		ircdproto->SendGlobops(OperServ, "%s", buf.c_str());
	}

	buf = s->GetName() + " " + s->GetUplink()->GetName();

	if (s->GetUplink() == Me && Capab.HasFlag(CAPAB_UNCONNECT))
	{
		Log(LOG_DEBUG) << "Sending UNCONNECT SQUIT for " << s->GetName();
		/* need to fix */
		ircdproto->SendSquit(s->GetName(), buf);
	}

	s->Delete(buf);

	return true;
}

bool IRCdMessage::OnWhois(const Anope::string &source, const std::vector<Anope::string> &params)
{
	const Anope::string &who = params[0];

	if (!source.empty() && !who.empty())
	{
		User *u;
		BotInfo *bi = findbot(who);
		if (bi)
		{
			ircdproto->SendNumeric(Config->ServerName, 311, source, "%s %s %s * :%s", bi->nick.c_str(), bi->GetIdent().c_str(), bi->host.c_str(), bi->realname.c_str());
			ircdproto->SendNumeric(Config->ServerName, 307, source, "%s :is a registered nick", bi->nick.c_str());
			ircdproto->SendNumeric(Config->ServerName, 312, source, "%s %s :%s", bi->nick.c_str(), Config->ServerName.c_str(), Config->ServerDesc.c_str());
			ircdproto->SendNumeric(Config->ServerName, 317, source, "%s %ld %ld :seconds idle, signon time", bi->nick.c_str(), static_cast<long>(Anope::CurTime - bi->lastmsg), static_cast<long>(start_time));
			ircdproto->SendNumeric(Config->ServerName, 318, source, "%s :End of /WHOIS list.", who.c_str());
		}
		else if (!ircd->svshold && (u = finduser(who)) && u->server == Me)
		{
			ircdproto->SendNumeric(Config->ServerName, 311, source, "%s %s %s * :%s", u->nick.c_str(), u->GetIdent().c_str(), u->host.c_str(), u->realname.c_str());
			ircdproto->SendNumeric(Config->ServerName, 312, source, "%s %s :%s", u->nick.c_str(), Config->ServerName.c_str(), Config->ServerDesc.c_str());
			ircdproto->SendNumeric(Config->ServerName, 318, source, "%s :End of /WHOIS list.", u->nick.c_str());
		}
		else
			ircdproto->SendNumeric(Config->ServerName, 401, source, "%s :No such service.", who.c_str());
	}

	return true;
}

bool IRCdMessage::OnCapab(const Anope::string &, const std::vector<Anope::string> &params)
{
	for (unsigned i = 0; i < params.size(); ++i)
	{
		for (unsigned j = 0; !Capab_Info[j].Token.empty(); ++j)
		{
			if (Capab_Info[j].Token.equals_ci(params[i]))
			{
				Capab.SetFlag(Capab_Info[j].Flag);
				Log(LOG_DEBUG) << "Capab: Enabling " << Capab_Info[j].Token;
				break;
			}
		}
	}

	return true;
}

bool IRCdMessage::OnError(const Anope::string &, const std::vector<Anope::string> &params)
{
	if (!params.empty())
		Log(LOG_DEBUG) << params[0];
	
	return true;
}

