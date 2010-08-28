/* Unreal IRCD 3.2.x functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "services.h"
#include "modules.h"

IRCDVar myIrcd[] = {
	{"UnrealIRCd 3.2.x",	/* ircd name */
	 "+Soi",				/* Modes used by pseudoclients */
	 5,						/* Chan Max Symbols */
	 "+ao",					/* Channel Umode used by Botserv bots */
	 1,						/* SVSNICK */
	 1,						/* Vhost */
	 1,						/* Supports SNlines */
	 1,						/* Supports SQlines */
	 1,						/* Supports SZlines */
	 3,						/* Number of server args */
	 0,						/* Join 2 Set */
	 0,						/* Join 2 Message */
	 1,						/* TS Topic Forward */
	 0,						/* TS Topci Backward */
	 0,						/* Chan SQlines */
	 0,						/* Quit on Kill */
	 1,						/* SVSMODE unban */
	 1,						/* Reverse */
	 1,						/* vidents */
	 1,						/* svshold */
	 1,						/* time stamp on mode */
	 1,						/* NICKIP */
	 1,						/* O:LINE */
	 1,						/* UMODE */
	 1,						/* VHOST ON NICK */
	 1,						/* Change RealName */
	 1,						/* No Knock requires +i */
	 1,						/* We support Unreal TOKENS */
	 1,						/* Can remove User Channel Modes with SVSMODE */
	 0,						/* Sglines are not enforced until user reconnects */
	 0,						/* ts6 */
	 0,						/* p10 */
	 0,						/* CIDR channelbans */
	 "$",					/* TLD Prefix for Global */
	 12,					/* Max number of modes we can send per line */
	 }
	,
	{NULL}
};

void unreal_cmd_netinfo(int ac, const char **av)
{
	send_cmd("", "AO %ld %ld %d %s 0 0 0 :%s", static_cast<long>(maxusercnt), static_cast<long>(time(NULL)), Anope::string(av[2]).is_number_only() ? convertTo<int>(av[2]) : 0, av[3], av[7]);
}

/* PROTOCTL */
/*
   NICKv2 = Nick Version 2
   VHP    = Sends hidden host
   UMODE2 = sends UMODE2 on user modes
   NICKIP = Sends IP on NICK
   TOKEN  = Use tokens to talk
   SJ3    = Supports SJOIN
   NOQUIT = No Quit
   TKLEXT = Extended TKL we don't use it but best to have it
   SJB64  = Base64 encoded time stamps
   VL     = Version Info
   NS     = Config->Numeric Server

*/
void unreal_cmd_capab()
{
	if (!Config->Numeric.empty())
		send_cmd("", "PROTOCTL NICKv2 VHP UMODE2 NICKIP TOKEN SJOIN SJOIN2 SJ3 NOQUIT TKLEXT VL");
	else
		send_cmd("", "PROTOCTL NICKv2 VHP UMODE2 NICKIP TOKEN SJOIN SJOIN2 SJ3 NOQUIT TKLEXT");
}

/* PASS */
void unreal_cmd_pass(const Anope::string &pass)
{
	send_cmd("", "PASS :%s", pass.c_str());
}

/* CHGHOST */
void unreal_cmd_chghost(const Anope::string &nick, const Anope::string &vhost)
{
	if (nick.empty() || vhost.empty())
		return;
	send_cmd(Config->ServerName, "AL %s %s", nick.c_str(), vhost.c_str());
}

/* CHGIDENT */
void unreal_cmd_chgident(const Anope::string &nick, const Anope::string &vIdent)
{
	if (nick.empty() || vIdent.empty())
		return;
	send_cmd(Config->ServerName, "AZ %s %s", nick.c_str(), vIdent.c_str());
}

class UnrealIRCdProto : public IRCDProto
{
	/* SVSNOOP */
	void SendSVSNOOP(const Anope::string &server, int set)
	{
		send_cmd("", "f %s %s", server.c_str(), set ? "+" : "-");
	}

	void SendAkillDel(const XLine *x)
	{
		send_cmd("", "BD - G %s %s %s", x->GetUser().c_str(), x->GetHost().c_str(), Config->s_OperServ.c_str());
	}

	void SendTopic(const BotInfo *whosets, const Channel *c, const Anope::string &whosetit, const Anope::string &topic)
	{
		send_cmd(whosets->nick, ") %s %s %lu :%s", c->name.c_str(), whosetit.c_str(), static_cast<unsigned long>(c->topic_time), topic.c_str());
	}

	void SendVhostDel(User *u)
	{
		u->RemoveMode(HostServ, UMODE_CLOAK);
		u->RemoveMode(HostServ, UMODE_VHOST);
		ModeManager::ProcessModes();
		u->SetMode(HostServ, UMODE_CLOAK);
		ModeManager::ProcessModes();
	}

	void SendAkill(const XLine *x)
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - time(NULL);
		if (timeleft > 172800)
			timeleft = 172800;
		send_cmd("", "BD + G %s %s %s %ld %ld :%s", x->GetUser().c_str(), x->GetHost().c_str(), x->By.c_str(), static_cast<long>(time(NULL) + timeleft), static_cast<long>(x->Expires), x->Reason.c_str());
	}

	void SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf)
	{
		send_cmd(source ? source->nick : Config->ServerName, "h %s :%s", user->nick.c_str(), buf.c_str());
	}

	/*
	 * m_svsmode() added by taz
	 * parv[0] - sender
	 * parv[1] - username to change mode for
	 * parv[2] - modes to change
	 * parv[3] - Service Stamp (if mode == d)
	 */
	void SendSVSMode(const User *u, int ac, const char **av)
	{
		if (ac >= 1)
		{
			if (!u || !av[0])
				return;
			this->SendModeInternal(NULL, u, merge_args(ac, av));
		}
	}

	void SendModeInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf)
	{
		if (buf.empty())
			return;
		send_cmd(source->nick, "G %s %s", dest->name.c_str(), buf.c_str());
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf)
	{
		if (buf.empty())
			return;
		send_cmd(bi ? bi->nick : Config->ServerName, "v %s %s", u->nick.c_str(), buf.c_str());
	}

	void SendClientIntroduction(const Anope::string &nick, const Anope::string &user, const Anope::string &host, const Anope::string &real, const Anope::string &modes, const Anope::string &)
	{
		EnforceQlinedNick(nick, Config->ServerName);
		send_cmd("", "& %s 1 %ld %s %s %s 0 %s %s%s :%s", nick.c_str(), static_cast<long>(time(NULL)), user.c_str(), host.c_str(), Config->ServerName.c_str(), modes.c_str(), host.c_str(), myIrcd->nickip ? " *" : "", real.c_str());
	}

	void SendKickInternal(const BotInfo *source, const Channel *chan, const User *user, const Anope::string &buf)
	{
		if (!buf.empty())
			send_cmd(source->nick, "H %s %s :%s", chan->name.c_str(), user->nick.c_str(), buf.c_str());
		else
			send_cmd(source->nick, "H %s %s", chan->name.c_str(), user->nick.c_str());
	}

	void SendNoticeChanopsInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf)
	{
		if (buf.empty())
			return;
		send_cmd(source->nick, "B @%s :%s", dest->name.c_str(), buf.c_str());
	}

	/* SERVER name hop descript */
	/* Unreal 3.2 actually sends some info about itself in the descript area */
	void SendServer(const Server *server)
	{
		if (!Config->Numeric.empty())
			send_cmd("", "SERVER %s %d :U0-*-%s %s", server->GetName().c_str(), server->GetHops(), Config->Numeric.c_str(), server->GetDescription().c_str());
		else
			send_cmd("", "SERVER %s %d :%s", server->GetName().c_str(), server->GetHops(), server->GetDescription().c_str());
	}

	/* JOIN */
	void SendJoin(const BotInfo *user, const Anope::string &channel, time_t chantime)
	{
		send_cmd(Config->ServerName, "~ %ld %s :%s", static_cast<long>(chantime), channel.c_str(), user->nick.c_str());
	}

	void SendJoin(BotInfo *user, const ChannelContainer *cc)
	{
		send_cmd(Config->ServerName, "~ %ld %s :%s%s", static_cast<long>(cc->chan->creation_time), cc->chan->name.c_str(), cc->Status->BuildModePrefixList().c_str(), user->nick.c_str());
		cc->chan->SetModes(user, false, "%s", cc->chan->GetModes(true, true).c_str());
	}

	/* unsqline
	*/
	void SendSQLineDel(const XLine *x)
	{
		send_cmd("", "d %s", x->Mask.c_str());
	}

	/* SQLINE */
	/*
	** - Unreal will translate this to TKL for us
	**
	*/
	void SendSQLine(const XLine *x)
	{
		send_cmd("", "c %s :%s", x->Mask.c_str(), x->Reason.c_str());
	}

	/*
	** svso
	**	  parv[0] = sender prefix
	**	  parv[1] = nick
	**	  parv[2] = options
	*/
	void SendSVSO(const Anope::string &source, const Anope::string &nick, const Anope::string &flag)
	{
		if (source.empty() || nick.empty() || flag.empty())
			return;
		send_cmd(source, "BB %s %s", nick.c_str(), flag.c_str());
	}

	/* NICK <newnick>  */
	void SendChangeBotNick(const BotInfo *oldnick, const Anope::string &newnick)
	{
		if (!oldnick || newnick.empty())
			return;
		send_cmd(oldnick->nick, "& %s %ld", newnick.c_str(), static_cast<long>(time(NULL)));
	}

	/* Functions that use serval cmd functions */

	void SendVhost(User *u, const Anope::string &vIdent, const Anope::string &vhost)
	{
		if (!vIdent.empty())
			unreal_cmd_chgident(u->nick, vIdent);
		if (!vhost.empty())
			unreal_cmd_chghost(u->nick, vhost);
	}

	void SendConnect()
	{
		unreal_cmd_capab();
		unreal_cmd_pass(uplink_server->password);
		SendServer(Me);
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick)
	{
		send_cmd("", "BD + Q H %s %s %ld %ld :Being held for registered user", nick.c_str(), Config->ServerName.c_str(), static_cast<long>(time(NULL) + Config->NSReleaseTimeout), static_cast<long>(time(NULL)));
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick)
	{
		send_cmd("", "BD - Q * %s %s", nick.c_str(), Config->ServerName.c_str());
	}

	/* UNSGLINE */
	/*
	 * SVSNLINE - :realname mask
	*/
	void SendSGLineDel(const XLine *x)
	{
		send_cmd("", "BR - :%s", x->Mask.c_str());
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x)
	{
		send_cmd("", "BD - Z * %s %s", x->Mask.c_str(), Config->s_OperServ.c_str());
	}

	/* SZLINE */
	void SendSZLine(const XLine *x)
	{
		send_cmd("", "BD + Z * %s %s %ld %ld :%s", x->Mask.c_str(), x->By.c_str(), static_cast<long>(time(NULL) + 172800), static_cast<long>(time(NULL)), x->Reason.c_str());
	}

	/* SGLINE */
	/*
	 * SVSNLINE + reason_where_is_space :realname mask with spaces
	*/
	void SendSGLine(const XLine *x)
	{
		Anope::string edited_reason = x->Reason;
		edited_reason = edited_reason.replace_all_cs(" ", "_");
		send_cmd("", "BR + %s :%s", edited_reason.c_str(), x->Mask.c_str());
	}

	/* SVSMODE -b */
	void SendBanDel(const Channel *c, const Anope::string &nick)
	{
		SendSVSModeChan(c, "-b", nick);
	}

	/* SVSMODE channel modes */

	void SendSVSModeChan(const Channel *c, const Anope::string &mode, const Anope::string &nick)
	{
		if (!nick.empty())
			send_cmd(Config->ServerName, "n %s %s %s", c->name.c_str(), mode.c_str(), nick.c_str());
		else
			send_cmd(Config->ServerName, "n %s %s", c->name.c_str(), mode.c_str());
	}

	/* svsjoin
		parv[0] - sender
		parv[1] - nick to make join
		parv[2] - channel to join
		parv[3] - (optional) channel key(s)
	*/
	/* In older Unreal SVSJOIN and SVSNLINE tokens were mixed so SVSJOIN and SVSNLINE are broken
	   when coming from a none TOKEN'd server
	*/
	void SendSVSJoin(const Anope::string &source, const Anope::string &nick, const Anope::string &chan, const Anope::string &param)
	{
		if (!param.empty())
			send_cmd(source, "BX %s %s :%s", nick.c_str(), chan.c_str(), param.c_str());
		else
			send_cmd(source, "BX %s :%s", nick.c_str(), chan.c_str());
	}

	/* svspart
		parv[0] - sender
		parv[1] - nick to make part
		parv[2] - channel(s) to part
	*/
	void SendSVSPart(const Anope::string &source, const Anope::string &nick, const Anope::string &chan)
	{
		send_cmd(source, "BT %s :%s", nick.c_str(), chan.c_str());
	}

	void SendSWhois(const Anope::string &source, const Anope::string &who, const Anope::string &mask)
	{
		send_cmd(source, "BA %s :%s", who.c_str(), mask.c_str());
	}

	void SendEOB()
	{
		send_cmd(Config->ServerName, "ES");
	}

	bool IsNickValid(const Anope::string &nick)
	{
		if (nick.equals_ci("ircd") || nick.equals_ci("irc"))
			return false;

		return true;
	}

	bool IsChannelValid(const Anope::string &chan)
	{
		if (chan.find(':') != Anope::string::npos || chan[0] != '#')
			return false;

		return true;
	}

	void SetAutoIdentificationToken(User *u)
	{
		if (!u->Account())
			return;

		u->SetMode(NickServ, UMODE_REGISTERED);
		ircdproto->SendMode(NickServ, u, "+d %d", u->timestamp);
	}

	void SendUnregisteredNick(const User *u)
	{
		ircdproto->SendMode(NickServ, u, "+d 1");
	}
} ircd_proto;

/* Event: PROTOCTL */
int anope_event_capab(const Anope::string &source, int ac, const char **av)
{
	for (int i = 0; i < ac; ++i)
	{
		Anope::string capab = av[i];

		if (capab.find("CHANMODES") != Anope::string::npos)
		{
			Anope::string modes(capab.begin() + 10, capab.end());
			commasepstream sep(modes);
			Anope::string modebuf;

			sep.GetToken(modebuf);
			for (size_t t = 0, end = modebuf.length(); t < end; ++t)
			{
				switch (modebuf[t])
				{
					case 'b':
						ModeManager::AddChannelMode(new ChannelModeBan('b'));
						continue;
					case 'e':
						ModeManager::AddChannelMode(new ChannelModeExcept('e'));
						continue;
					case 'I':
						ModeManager::AddChannelMode(new ChannelModeInvex('I'));
						continue;
					default:
						ModeManager::AddChannelMode(new ChannelModeList(CMODE_END, "", modebuf[t]));
				}
			}

			sep.GetToken(modebuf);
			for (size_t t = 0, end = modebuf.length(); t < end; ++t)
			{
				switch (modebuf[t])
				{
					case 'k':
						ModeManager::AddChannelMode(new ChannelModeKey('k'));
						continue;
					case 'f':
						ModeManager::AddChannelMode(new ChannelModeFlood('f'));
						continue;
					case 'L':
						ModeManager::AddChannelMode(new ChannelModeParam(CMODE_REDIRECT, "CMODE_REDIRECT", 'L'));
						continue;
					default:
						ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, "", modebuf[t]));
				}
			}

			sep.GetToken(modebuf);
			for (size_t t = 0, end = modebuf.length(); t < end; ++t)
			{
				switch (modebuf[t])
				{
					case 'l':
						ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, "CMODE_LIMIT", 'l', true));
						continue;
					case 'j':
						ModeManager::AddChannelMode(new ChannelModeParam(CMODE_JOINFLOOD, "CMODE_JOINFLOOD", 'j', true));
						continue;
					default:
						ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, "", modebuf[t], true));
				}
			}

			sep.GetToken(modebuf);
			for (size_t t = 0, end = modebuf.length(); t < end; ++t)
			{
				switch (modebuf[t])
				{
					case 'p':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, "CMODE_PRIVATE", 'p'));
						continue;
					case 's':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, "CMODE_SECRET", 's'));
						continue;
					case 'm':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, "CMODE_MODERATED", 'm'));
						continue;
					case 'n':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, "CMODE_NOEXTERNAL", 'n'));
						continue;
					case 't':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, "CMODE_TOPIC", 't'));
						continue;
					case 'i':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, "CMODE_INVITE", 'i'));
						continue;
					case 'r':
						ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
						continue;
					case 'R':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_REGISTEREDONLY, "CMODE_REGISTEREDONLY", 'R'));
						continue;
					case 'c':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCOLOR, "CMODE_BLOCKCOLOR", 'c'));
						continue;
					case 'O':
						ModeManager::AddChannelMode(new ChannelModeOper('O'));
						continue;
					case 'A':
						ModeManager::AddChannelMode(new ChannelModeAdmin('A'));
						continue;
					case 'Q':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKICK, "CMODE_NOKICK", 'Q'));
						continue;
					case 'K':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKNOCK, "CMODE_NOKNOCK", 'K'));
						continue;
					case 'V':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NOINVITE, "CMODE_NOINVITE", 'V'));
						continue;
					case 'C':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NOCTCP, "CMODE_NOCTCP", 'C'));
						continue;
					case 'u':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_AUDITORIUM, "CMODE_AUDITORIUM", 'u'));
						continue;
					case 'z':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_SSL, "CMODE_SSL", 'z'));
						continue;
					case 'N':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NONICK, "CMODE_NONICK", 'N'));
						continue;
					case 'S':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_STRIPCOLOR, "CMODE_STRIPCOLOR", 'S'));
						continue;
					case 'M':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_REGMODERATED, "CMODE_REGMODERATED", 'M'));
						continue;
					case 'T':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NONOTICE, "CMODE_NONOTICE", 'T'));
						continue;
					case 'G':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_FILTER, "CMODE_FILTER", 'G'));
						continue;
					default:
						ModeManager::AddChannelMode(new ChannelMode(CMODE_END, "", modebuf[t]));
				}
			}
		}
	}

	CapabParse(ac, av);

	return MOD_CONT;
}

/* Events */
int anope_event_ping(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	ircdproto->SendPong(ac > 1 ? av[1] : Config->ServerName, av[0]);
	return MOD_CONT;
}

/** This is here because:
 *
 * If we had servers three servers, A, B & C linked like so: A<->B<->C
 * If Anope is (linked to) A and B splits from A and then reconnects
 * B introduces itself, introduces C, sends EOS for C, introduces Bs clients
 * introduces Cs clients, sends EOS for B. This causes all of Cs clients to be introduced
 * with their server "not syncing". We now send a PING immediatly when receiving a new server
 * and then finish sync once we get a pong back from that server
 */
int anope_event_pong(const Anope::string &source, int ac, const char **av)
{
	Server *s = Server::Find(source);
	if (s && !s->IsSynced())
		s->Sync(false);
	return MOD_CONT;
}

/* netinfo
 *  argv[0] = max global count
 *  argv[1] = time of end sync
 *  argv[2] = unreal protocol using (numeric)
 *  argv[3] = cloak-crc (> u2302)
 *  argv[4] = free(**)
 *  argv[5] = free(**)
 *  argv[6] = free(**)
 *  argv[7] = ircnet
 */
int anope_event_netinfo(const Anope::string &source, int ac, const char **av)
{
	unreal_cmd_netinfo(ac, av);
	return MOD_CONT;
}

int anope_event_436(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;

	m_nickcoll(av[0]);
	return MOD_CONT;
}

/*
** away
**	  parv[0] = sender prefix
**	  parv[1] = away message
*/
int anope_event_away(const Anope::string &source, int ac, const char **av)
{
	if (source.empty())
		return MOD_CONT;
	m_away(source, ac ? av[0] : "");
	return MOD_CONT;
}

/*
** m_topic
**	parv[0] = sender prefix
**	parv[1] = topic text
**
**	For servers using TS:
**	parv[0] = sender prefix
**	parv[1] = channel name
**	parv[2] = topic nickname
**	parv[3] = topic time
**	parv[4] = topic text
*/
int anope_event_topic(const Anope::string &source, int ac, const char **av)
{
	if (ac != 4)
		return MOD_CONT;
	do_topic(source, ac, av);
	return MOD_CONT;
}

int anope_event_squit(const Anope::string &source, int ac, const char **av)
{
	if (ac != 2)
		return MOD_CONT;
	do_squit(source, ac, av);
	return MOD_CONT;
}

int anope_event_quit(const Anope::string &source, int ac, const char **av)
{
	if (ac != 1)
		return MOD_CONT;
	do_quit(source, ac, av);
	return MOD_CONT;
}

int anope_event_mode(const Anope::string &source, int ac, const char **av)
{
	if (ac < 2)
		return MOD_CONT;

	if (*av[0] == '#' || *av[0] == '&')
		do_cmode(source, ac, av);
	else
		do_umode(source, ac, av);
	return MOD_CONT;
}

/* This is used to strip the TS from the end of the mode stirng */
int anope_event_gmode(const Anope::string &source, int ac, const char **av)
{
	if (Server::Find(source))
		--ac;
	return anope_event_mode(source, ac, av);
}

/* Unreal sends USER modes with this */
/*
	umode2
	parv[0] - sender
	parv[1] - modes to change
*/
int anope_event_umode2(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;

	const char *newav[4];
	newav[0] = source.c_str();
	newav[1] = av[0];
	do_umode(source, ac, newav);
	return MOD_CONT;
}

int anope_event_kill(const Anope::string &source, int ac, const char **av)
{
	if (ac != 2)
		return MOD_CONT;

	m_kill(av[0], av[1]);
	return MOD_CONT;
}

int anope_event_kick(const Anope::string &source, int ac, const char **av)
{
	if (ac != 3)
		return MOD_CONT;
	do_kick(source, ac, av);
	return MOD_CONT;
}

int anope_event_join(const Anope::string &source, int ac, const char **av)
{
	if (ac != 1)
		return MOD_CONT;
	do_join(source, ac, av);
	return MOD_CONT;
}

int anope_event_motd(const Anope::string &source, int ac, const char **av)
{
	if (source.empty())
		return MOD_CONT;

	m_motd(source);
	return MOD_CONT;
}

int anope_event_setname(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "SETNAME for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetRealname(av[0]);
	return MOD_CONT;
}

int anope_event_chgname(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac != 2)
		return MOD_CONT;

	u = finduser(av[0]);
	if (!u)
	{
		Log(LOG_DEBUG) << "CHGNAME for nonexistent user " << av[0];
		return MOD_CONT;
	}

	u->SetRealname(av[1]);
	return MOD_CONT;
}

int anope_event_setident(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "SETIDENT for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetVIdent(av[0]);
	return MOD_CONT;
}

int anope_event_chgident(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac != 2)
		return MOD_CONT;

	u = finduser(av[0]);
	if (!u)
	{
		Log(LOG_DEBUG) << "CHGIDENT for nonexistent user " << av[0];
		return MOD_CONT;
	}

	u->SetVIdent(av[1]);
	return MOD_CONT;
}

int anope_event_sethost(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "SETHOST for nonexistent user " << source;
		return MOD_CONT;
	}

	/* When a user sets +x we recieve the new host and then the mode change */
	if (u->HasMode(UMODE_CLOAK))
		u->SetDisplayedHost(av[0]);
	else
		u->SetCloakedHost(av[0]);

	return MOD_CONT;
}

/*
** NICK - new
**	  source  = NULL
**	parv[0] = nickname
**	  parv[1] = hopcount
**	  parv[2] = timestamp
**	  parv[3] = username
**	  parv[4] = hostname
**	  parv[5] = servername
**  if NICK version 1:
**	  parv[6] = servicestamp
**	parv[7] = info
**  if NICK version 2:
**	parv[6] = servicestamp
**	  parv[7] = umodes
**	parv[8] = virthost, * if none
**	parv[9] = info
**  if NICKIP:
**	  parv[9] = ip
**	  parv[10] = info
**
** NICK - change
**	  source  = oldnick
**	parv[0] = new nickname
**	  parv[1] = hopcount
*/
/*
 do_nick(const char *source, char *nick, char *username, char *host,
			  char *server, char *realname, time_t ts,
			  uint32 ip, char *vhost, char *uid)
*/
int anope_event_nick(const Anope::string &source, int ac, const char **av)
{
	User *user;

	if (ac != 2)
	{
		if (ac == 7)
		{
			/*
			   <codemastr> that was a bug that is now fixed in 3.2.1
			   <codemastr> in  some instances it would use the non-nickv2 format
			   <codemastr> it's sent when a nick collision occurs
			   - so we have to leave it around for now -TSL
			 */
			do_nick(source, av[0], av[3], av[4], av[5], av[6], Anope::string(av[2]).is_pos_number_only() ? convertTo<time_t>(av[2]) : 0, 0, "*", "");
		}
		else if (ac == 11)
		{
			user = do_nick(source, av[0], av[3], av[4], av[5], av[10], Anope::string(av[2]).is_pos_number_only() ? convertTo<time_t>(av[2]) : 0, ntohl(decode_ip(av[9])), av[8], "");
			if (user)
			{
				UserSetInternalModes(user, 1, &av[7]);

				NickAlias *na = findnick(user->nick);

				if (na && user->timestamp == convertTo<time_t>(av[6]))
				{
					user->Login(na->nc);
					user->SetMode(NickServ, UMODE_REGISTERED);
				}
				else
					validate_user(user);
			}
		}
		else
		{
			/* NON NICKIP */
			user = do_nick(source, av[0], av[3], av[4], av[5], av[9], Anope::string(av[2]).is_pos_number_only() ? convertTo<time_t>(av[2]) : 0, 0, av[8], "");
			if (user)
			{
				UserSetInternalModes(user, 1, &av[7]);

				NickAlias *na = findnick(user->nick);

				if (na && user->timestamp == convertTo<time_t>(av[6]))
				{
					user->Login(na->nc);
					user->SetMode(NickServ, UMODE_REGISTERED);
				}
				else
					validate_user(user);
			}
		}
	}
	else
		do_nick(source, av[0], "", "", "", "", Anope::string(av[1]).is_pos_number_only() ? convertTo<time_t>(av[1]) : 0, 0, "", "");
	return MOD_CONT;
}

int anope_event_chghost(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac != 2)
		return MOD_CONT;

	u = finduser(av[0]);
	if (!u)
	{
		Log(LOG_DEBUG) << "debug: CHGHOST for nonexistent user " << av[0];
		return MOD_CONT;
	}

	u->SetDisplayedHost(av[1]);
	return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(const Anope::string &source, int ac, const char **av)
{
	if (!stricmp(av[1], "1"))
	{
		Anope::string vl = myStrGetToken(av[2], ' ', 0);
		Anope::string upnumeric = myStrGetToken(vl, '-', 2);
		Anope::string desc = myStrGetTokenRemainder(av[2], ' ', 1);
		do_server(source, av[0], Anope::string(av[1]).is_pos_number_only() ? convertTo<unsigned>(av[1]) : 0, desc, upnumeric);
	}
	else
		do_server(source, av[0], Anope::string(av[1]).is_pos_number_only() ? convertTo<unsigned>(av[1]) : 0, av[2], "");
	ircdproto->SendPing(Config->ServerName, av[0]);

	return MOD_CONT;
}

int anope_event_privmsg(const Anope::string &source, int ac, const char **av)
{
	if (ac != 2)
		return MOD_CONT;
	m_privmsg(source, av[0], av[1]);
	return MOD_CONT;
}

int anope_event_part(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1 || ac > 2)
		return MOD_CONT;
	do_part(source, ac, av);
	return MOD_CONT;
}

int anope_event_whois(const Anope::string &source, int ac, const char **av)
{
	if (!source.empty() && ac >= 1)
		m_whois(source, av[0]);
	return MOD_CONT;
}

int anope_event_error(const Anope::string &source, int ac, const char **av)
{
	if (av[0])
	{
		Log(LOG_DEBUG) << av[0];
		if (strstr(av[0], "No matching link configuration"))
			Log() << "Error: Your IRCD's link block may not be setup correctly, please check unrealircd.conf";
	}
	return MOD_CONT;
}

int anope_event_sdesc(const Anope::string &source, int ac, const char **av)
{
	Server *s = Server::Find(source);

	if (s)
		s->SetDescription(av[0]);

	return MOD_CONT;
}

int anope_event_sjoin(const Anope::string &source, int ac, const char **av)
{
	Channel *c = findchan(av[1]);
	time_t ts = Anope::string(av[0]).is_pos_number_only() ? convertTo<time_t>(av[0]) : 0;
	bool keep_their_modes = true;

	if (!c)
	{
		c = new Channel(av[1], ts);
		c->SetFlag(CH_SYNCING);
	}
	/* Our creation time is newer than what the server gave us */
	else if (c->creation_time > ts)
	{
		c->creation_time = ts;
		c->Reset();

		/* Reset mlock */
		check_modes(c);
	}
	/* Their TS is newer than ours, our modes > theirs, unset their modes if need be */
	else if (ts > c->creation_time)
		keep_their_modes = false;

	/* If we need to keep their modes, and this SJOIN string contains modes */
	if (keep_their_modes && ac >= 4)
	{
		/* Set the modes internally */
		ChanSetInternalModes(c, ac - 3, av + 2);
	}

	spacesepstream sep(av[ac - 1]);
	Anope::string buf;
	while (sep.GetToken(buf))
	{
		/* Ban */
		if (keep_their_modes && buf[0] == '&')
		{
			buf.erase(buf.begin());
			ChannelModeList *cml = debug_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_BAN));

			if (cml->IsValid(buf))
				cml->AddMask(c, buf);
		}
		/* Except */
		else if (keep_their_modes && buf[0] == '"')
		{
			buf.erase(buf.begin());
			ChannelModeList *cml = debug_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_EXCEPT));

			if (cml->IsValid(buf))
				cml->AddMask(c, buf);
		}
		/* Invex */
		else if (keep_their_modes && buf[0] == '\'')
		{
			buf.erase(buf.begin());
			ChannelModeList *cml = debug_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE));

			if (cml->IsValid(buf))
				cml->AddMask(c, buf);
		}
		else
		{
			std::list<ChannelMode *> Status;
			char ch;

			/* Get prefixes from the nick */
			while ((ch = ModeManager::GetStatusChar(buf[0])))
			{
				buf.erase(buf.begin());
				ChannelMode *cm = ModeManager::FindChannelModeByChar(ch);
				if (!cm)
				{
					Log() << "Received unknown mode prefix " << buf[0] << " in SJOIN string";
					continue;
				}

				if (keep_their_modes)
					Status.push_back(cm);
			}

			User *u = finduser(buf);
			if (!u)
			{
				Log(LOG_DEBUG) << "SJOIN for nonexistant user " << buf << " on " << c->name;
				continue;
			}

			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnPreJoinChannel, OnPreJoinChannel(u, c));

			/* Add the user to the channel */
			c->JoinUser(u);

			/* Update their status internally on the channel
			 * This will enforce secureops etc on the user
			 */
			for (std::list<ChannelMode *>::iterator it = Status.begin(), it_end = Status.end(); it != it_end; ++it)
				c->SetModeInternal(*it, buf);

			/* Now set whatever modes this user is allowed to have on the channel */
			chan_set_correct_modes(u, c, 1);

			/* Check to see if modules want the user to join, if they do
			 * check to see if they are allowed to join (CheckKick will kick/ban them)
			 * Don't trigger OnJoinChannel event then as the user will be destroyed
			 */
			if (MOD_RESULT != EVENT_STOP && c->ci && c->ci->CheckKick(u))
				continue;

			FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(u, c));
		}
	}

	/* Channel is done syncing */
	if (c->HasFlag(CH_SYNCING))
	{
		/* Unset the syncing flag */
		c->UnsetFlag(CH_SYNCING);
		c->Sync();
	}

	return MOD_CONT;
}

void moduleAddIRCDMsgs()
{
	Anope::AddMessage("436", anope_event_436);
	Anope::AddMessage("AWAY", anope_event_away);
	Anope::AddMessage("6", anope_event_away);
	Anope::AddMessage("JOIN", anope_event_join);
	Anope::AddMessage("C", anope_event_join);
	Anope::AddMessage("KICK", anope_event_kick);
	Anope::AddMessage("H", anope_event_kick);
	Anope::AddMessage("KILL", anope_event_kill);
	Anope::AddMessage(".", anope_event_kill);
	Anope::AddMessage("MODE", anope_event_mode);
	Anope::AddMessage("G", anope_event_gmode);
	Anope::AddMessage("MOTD", anope_event_motd);
	Anope::AddMessage("F", anope_event_motd);
	Anope::AddMessage("NICK", anope_event_nick);
	Anope::AddMessage("&", anope_event_nick);
	Anope::AddMessage("PART", anope_event_part);
	Anope::AddMessage("D", anope_event_part);
	Anope::AddMessage("PING", anope_event_ping);
	Anope::AddMessage("8", anope_event_ping);
	Anope::AddMessage("PONG", anope_event_pong);
	Anope::AddMessage("9", anope_event_pong);
	Anope::AddMessage("PRIVMSG", anope_event_privmsg);
	Anope::AddMessage("!", anope_event_privmsg);
	Anope::AddMessage("QUIT", anope_event_quit);
	Anope::AddMessage("),", anope_event_quit);
	Anope::AddMessage("SERVER", anope_event_server);
	Anope::AddMessage("'", anope_event_server);
	Anope::AddMessage("SQUIT", anope_event_squit);
	Anope::AddMessage("-", anope_event_squit);
	Anope::AddMessage("TOPIC", anope_event_topic);
	Anope::AddMessage(")", anope_event_topic);
	Anope::AddMessage("SVSMODE", anope_event_mode);
	Anope::AddMessage("n", anope_event_mode);
	Anope::AddMessage("SVS2MODE", anope_event_mode);
	Anope::AddMessage("v", anope_event_mode);
	Anope::AddMessage("WHOIS", anope_event_whois);
	Anope::AddMessage("#", anope_event_whois);
	Anope::AddMessage("PROTOCTL", anope_event_capab);
	Anope::AddMessage("_", anope_event_capab);
	Anope::AddMessage("CHGHOST", anope_event_chghost);
	Anope::AddMessage("AL", anope_event_chghost);
	Anope::AddMessage("CHGIDENT", anope_event_chgident);
	Anope::AddMessage("AZ", anope_event_chgident);
	Anope::AddMessage("CHGNAME", anope_event_chgname);
	Anope::AddMessage("BK", anope_event_chgname);
	Anope::AddMessage("NETINFO", anope_event_netinfo);
	Anope::AddMessage("AO", anope_event_netinfo);
	Anope::AddMessage("SETHOST", anope_event_sethost);
	Anope::AddMessage("AA", anope_event_sethost);
	Anope::AddMessage("SETIDENT", anope_event_setident);
	Anope::AddMessage("AD", anope_event_setident);
	Anope::AddMessage("SETNAME", anope_event_setname);
	Anope::AddMessage("AE", anope_event_setname);
	Anope::AddMessage("ERROR", anope_event_error);
	Anope::AddMessage("5", anope_event_error);
	Anope::AddMessage("UMODE2", anope_event_umode2);
	Anope::AddMessage("|", anope_event_umode2);
	Anope::AddMessage("SJOIN", anope_event_sjoin);
	Anope::AddMessage("~", anope_event_sjoin);
	Anope::AddMessage("SDESC", anope_event_sdesc);
	Anope::AddMessage("AG", anope_event_sdesc);

	/* The non token version of these is in messages.c */
	Anope::AddMessage("2", m_stats);
	Anope::AddMessage(">", m_time);
	Anope::AddMessage("+", m_version);
}

/* Borrowed part of this check from UnrealIRCd */
bool ChannelModeFlood::IsValid(const Anope::string &value) const
{
	if (value.empty())
		return false;
	Anope::string rest;
	if (value[0] != ':' && convertTo<unsigned>(value[0] == '*' ? value.substr(1) : value, rest, false) > 0 && rest[0] == ':' && rest.length() > 1 && convertTo<unsigned>(rest.substr(1), rest, false) > 0 && rest.empty())
		return true;
	else
	{
		/* '['<number><1 letter>[optional: '#'+1 letter],[next..]']'':'<number> */
		size_t end_bracket = value.find(']', 1);
		if (end_bracket == Anope::string::npos)
			return false;
		Anope::string xbuf = value.substr(0, end_bracket);
		if (value[end_bracket + 1] != ':')
			return false;
		commasepstream args(xbuf.substr(1));
		Anope::string arg;
		while (args.GetToken(arg))
		{
			/* <number><1 letter>[optional: '#'+1 letter] */
			size_t p = 0;
			while (p < arg.length() && isdigit(arg[p]))
				++p;
			if (p == arg.length() || !(arg[p] == 'c' || arg[p] == 'j' || arg[p] == 'k' || arg[p] == 'm' || arg[p] == 'n' || arg[p] == 't'))
				continue; /* continue instead of break for forward compatability. */
			int v = arg.substr(0, p).is_number_only() ? convertTo<int>(arg.substr(0, p)) : 0;
			if (v < 1 || v > 999)
				return false;
		}
		return true;
	}
}

static void AddModes()
{
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, "CMODE_VOICE", 'v', '+'));
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_HALFOP, "CMODE_HALFOP", 'h', '%'));
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, "CMODE_OP", 'o', '@'));
	/* Unreal sends +q as * and +a as ~ */
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT, "CMODE_PROTECT", 'a', '~'));
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, "CMODE_OWNER", 'q', '*'));

	/* Add user modes */
	ModeManager::AddUserMode(new UserMode(UMODE_SERV_ADMIN, "UMODE_SERV_ADMIN", 'A'));
	ModeManager::AddUserMode(new UserMode(UMODE_BOT, "UMODE_BOT", 'B'));
	ModeManager::AddUserMode(new UserMode(UMODE_CO_ADMIN, "UMODE_CO_ADMIN", 'C'));
	ModeManager::AddUserMode(new UserMode(UMODE_FILTER, "UMODE_FILTER", 'G'));
	ModeManager::AddUserMode(new UserMode(UMODE_HIDEOPER, "UMODE_HIDEOPER", 'H'));
	ModeManager::AddUserMode(new UserMode(UMODE_NETADMIN, "UMODE_NETADMIN", 'N'));
	ModeManager::AddUserMode(new UserMode(UMODE_REGPRIV, "UMODE_REGPRIV", 'R'));
	ModeManager::AddUserMode(new UserMode(UMODE_PROTECTED, "UMODE_PROTECTED", 'S'));
	ModeManager::AddUserMode(new UserMode(UMODE_NO_CTCP, "UMODE_NO_CTCP", 'T'));
	ModeManager::AddUserMode(new UserMode(UMODE_WEBTV, "UMODE_WEBTV", 'V'));
	ModeManager::AddUserMode(new UserMode(UMODE_WHOIS, "UMODE_WHOIS", 'W'));
	ModeManager::AddUserMode(new UserMode(UMODE_ADMIN, "UMODE_ADMIN", 'a'));
	ModeManager::AddUserMode(new UserMode(UMODE_DEAF, "UMODE_DEAF", 'd'));
	ModeManager::AddUserMode(new UserMode(UMODE_GLOBOPS, "UMODE_GLOBOPS", 'g'));
	ModeManager::AddUserMode(new UserMode(UMODE_HELPOP, "UMODE_HELPOP", 'h'));
	ModeManager::AddUserMode(new UserMode(UMODE_INVIS, "UMODE_INVIS", 'i'));
	ModeManager::AddUserMode(new UserMode(UMODE_OPER, "UMODE_OPER", 'o'));
	ModeManager::AddUserMode(new UserMode(UMODE_PRIV, "UMODE_PRIV", 'p'));
	ModeManager::AddUserMode(new UserMode(UMODE_GOD, "UMODE_GOD", 'q'));
	ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, "UMODE_REGISTERED", 'r'));
	ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, "UMODE_SNOMASK", 's'));
	ModeManager::AddUserMode(new UserMode(UMODE_VHOST, "UMODE_VHOST", 't'));
	ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, "UMODE_WALLOPS", 'w'));
	ModeManager::AddUserMode(new UserMode(UMODE_CLOAK, "UMODE_CLOAK", 'x'));
	ModeManager::AddUserMode(new UserMode(UMODE_SSL, "UMODE_SSL", 'z'));
}

class ProtoUnreal : public Module
{
 public:
	ProtoUnreal(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(PROTOCOL);

		pmodule_ircd_version("UnrealIRCd 3.2+");
		pmodule_ircd_var(myIrcd);
		pmodule_ircd_useTSMode(0);

		CapabType c[] = { CAPAB_NOQUIT, CAPAB_NICKIP, CAPAB_ZIP, CAPAB_TOKEN, CAPAB_SSJ3, CAPAB_NICK2, CAPAB_VL, CAPAB_TLKEXT, CAPAB_CHANMODE, CAPAB_NICKCHARS };
		for (unsigned i = 0; i < 10; ++i)
			Capab.SetFlag(c[i]);

		AddModes();

		pmodule_ircd_proto(&ircd_proto);
		moduleAddIRCDMsgs();

		ModuleManager::Attach(I_OnUserNickChange, this);
	}

	void OnUserNickChange(User *u, const Anope::string &)
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
	}
};

MODULE_INIT(ProtoUnreal)
