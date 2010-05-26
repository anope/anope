/* Unreal IRCD 3.2.x functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

/*************************************************************************/

#include "services.h"
#include "pseudo.h"

IRCDVar myIrcd[] = {
	{"UnrealIRCd 3.2.x",		/* ircd name */
	 "+Soi",					/* Modes used by pseudoclients */
	 5,						 /* Chan Max Symbols	 */
	 "+ao",					 /* Channel Umode used by Botserv bots */
	 1,						 /* SVSNICK */
	 1,						 /* Vhost  */
	 1,						 /* Supports SGlines	 */
	 1,						 /* Supports SQlines	 */
	 1,						 /* Supports SZlines	 */
	 3,						 /* Number of server args */
	 0,						 /* Join 2 Set		   */
	 0,						 /* Join 2 Message	   */
	 1,						 /* TS Topic Forward	 */
	 0,						 /* TS Topci Backward	*/
	 0,						 /* Chan SQlines		 */
	 0,						 /* Quit on Kill		 */
	 1,						 /* SVSMODE unban		*/
	 1,						 /* Reverse			  */
	 1,						 /* vidents			  */
	 1,						 /* svshold			  */
	 1,						 /* time stamp on mode   */
	 1,						 /* NICKIP			   */
	 1,						 /* O:LINE			   */
	 1,						 /* UMODE			   */
	 1,						 /* VHOST ON NICK		*/
	 1,						 /* Change RealName	  */
	 0,						 /* On nick change check if they could be identified */
	 1,						 /* No Knock requires +i */
	 1,						 /* We support Unreal TOKENS */
	 1,						 /* TIME STAMPS are BASE64 */
	 1,						 /* Can remove User Channel Modes with SVSMODE */
	 0,						 /* Sglines are not enforced until user reconnects */
	 0,						 /* ts6 */
	 0,						 /* p10 */
	 0,						 /* CIDR channelbans */
	 "$",					   /* TLD Prefix for Global */
	 false,					/* Auth for users is sent after the initial NICK/UID command */
	 12,					/* Max number of modes we can send per line */
	 }
	,
	{NULL}
};

/* svswatch
 * parv[0] - sender
 * parv[1] - target nick
 * parv[2] - parameters
 */
void unreal_cmd_svswatch(const char *sender, const char *nick, const char *parm)
{
	send_cmd(sender, "Bw %s :%s", nick, parm);
}

void unreal_cmd_netinfo(int ac, const char **av)
{
	send_cmd(NULL, "AO %ld %ld %d %s 0 0 0 :%s", static_cast<long>(maxusercnt), static_cast<long>(time(NULL)), atoi(av[2]), av[3], av[7]);
}
/* PROTOCTL */
/*
   NICKv2 = Nick Version 2
   VHP	= Sends hidden host
   UMODE2 = sends UMODE2 on user modes
   NICKIP = Sends IP on NICK
   TOKEN  = Use tokens to talk
   SJ3	= Supports SJOIN
   NOQUIT = No Quit
   TKLEXT = Extended TKL we don't use it but best to have it
   SJB64  = Base64 encoded time stamps
   VL	 = Version Info
   NS	 = Config.Numeric Server

*/
void unreal_cmd_capab()
{
	if (Config.Numeric)
	{
		send_cmd(NULL, "PROTOCTL NICKv2 VHP UMODE2 NICKIP TOKEN SJOIN SJOIN2 SJ3 NOQUIT TKLEXT SJB64 VL");
	}
	else
	{
		send_cmd(NULL, "PROTOCTL NICKv2 VHP UMODE2 NICKIP TOKEN SJOIN SJOIN2 SJ3 NOQUIT TKLEXT SJB64");
	}
}

/* PASS */
void unreal_cmd_pass(const char *pass)
{
	send_cmd(NULL, "PASS :%s", pass);
}

/* CHGHOST */
void unreal_cmd_chghost(const char *nick, const char *vhost)
{
	if (!nick || !vhost) {
		return;
	}
	send_cmd(Config.ServerName, "AL %s %s", nick, vhost);
}

/* CHGIDENT */
void unreal_cmd_chgident(const char *nick, const char *vIdent)
{
	if (!nick || !vIdent) {
		return;
	}
	send_cmd(Config.ServerName, "AZ %s %s", nick, vIdent);
}



class UnrealIRCdProto : public IRCDProto
{
	/* SVSNOOP */
	void SendSVSNOOP(const char *server, int set)
	{
		send_cmd(NULL, "f %s %s", server, set ? "+" : "-");
	}

	void SendAkillDel(Akill *ak)
	{
		send_cmd(NULL, "BD - G %s %s %s", ak->user, ak->host, Config.s_OperServ);
	}

	void SendTopic(BotInfo *whosets, Channel *c, const char *whosetit, const char *topic)
	{
		send_cmd(whosets->nick, ") %s %s %lu :%s", c->name.c_str(), whosetit, static_cast<unsigned long>(c->topic_time), topic);
	}

	void SendVhostDel(User *u)
	{
		BotInfo *bi = findbot(Config.s_HostServ);
		u->RemoveMode(bi, UMODE_CLOAK);
		u->RemoveMode(bi, UMODE_VHOST);
		ModeManager::ProcessModes();
		u->SetMode(bi, UMODE_CLOAK);
		ModeManager::ProcessModes();
	}

	void SendAkill(Akill *ak)
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = ak->expires - time(NULL);
		if (timeleft > 172800) timeleft = 172800;
		send_cmd(NULL, "BD + G %s %s %s %ld %ld :%s", ak->user, ak->host, ak->by, static_cast<long>(time(NULL) + timeleft), static_cast<long>(ak->expires), ak->reason);
	}

	void SendSVSKillInternal(BotInfo *source, User *user, const char *buf)
	{
		send_cmd(source ? source->nick : Config.ServerName, "h %s :%s", user->nick.c_str(), buf);
	}

	/*
	 * m_svsmode() added by taz
	 * parv[0] - sender
	 * parv[1] - username to change mode for
	 * parv[2] - modes to change
	 * parv[3] - Service Stamp (if mode == d)
	 */
	void SendSVSMode(User *u, int ac, const char **av)
	{
		if (ac >= 1) {
			if (!u || !av[0]) return;
			this->SendModeInternal(NULL, u, merge_args(ac, av));
		}
	}

	void SendModeInternal(BotInfo *source, Channel *dest, const char *buf)
	{
		if (!buf) return;
		send_cmd(source->nick, "G %s %s", dest->name.c_str(), buf);
	}

	void SendModeInternal(BotInfo *bi, User *u, const char *buf)
	{
		if (!buf) return;
		send_cmd(bi ? bi->nick : Config.ServerName, "v %s %s", u->nick.c_str(), buf);
	}

	void SendClientIntroduction(const std::string &nick, const std::string &user, const std::string &host, const std::string &real, const char *modes, const std::string &uid)
	{
		EnforceQlinedNick(nick, Config.ServerName);
		send_cmd(NULL, "& %s 1 %ld %s %s %s 0 %s %s%s :%s", nick.c_str(), static_cast<long>(time(NULL)), user.c_str(), host.c_str(), Config.ServerName, modes, host.c_str(), myIrcd->nickip ? " *" : " ", real.c_str());
	}

	void SendKickInternal(BotInfo *source, Channel *chan, User *user, const char *buf)
	{
		if (buf) send_cmd(source->nick, "H %s %s :%s", chan->name.c_str(), user->nick.c_str(), buf);
		else send_cmd(source->nick, "H %s %s", chan->name.c_str(), user->nick.c_str());
	}

	void SendNoticeChanopsInternal(BotInfo *source, Channel *dest, const char *buf)
	{
		if (!buf) return;
		send_cmd(source->nick, "B @%s :%s", dest->name.c_str(), buf);
	}

	/* SERVER name hop descript */
	/* Unreal 3.2 actually sends some info about itself in the descript area */
	void SendServer(Server *server)
	{
		if (Config.Numeric)
			send_cmd(NULL, "SERVER %s %d :U0-*-%s %s", server->name, server->hops, Config.Numeric, server->desc);
		else
			send_cmd(NULL, "SERVER %s %d :%s", server->name, server->hops, server->desc);
	}

	/* JOIN */
	void SendJoin(BotInfo *user, const char *channel, time_t chantime)
	{
		send_cmd(Config.ServerName, "~ !%s %s :%s", base64enc(static_cast<long>(chantime)), channel, user->nick.c_str());
	}

	/* unsqline
	**	parv[0] = sender
	**	parv[1] = nickmask
	*/
	void SendSQLineDel(const std::string &user)
	{
		if (user.empty())
			return;
		send_cmd(NULL, "d %s", user.c_str());
	}


	/* SQLINE */
	/*
	**	parv[0] = sender
	**	parv[1] = nickmask
	**	parv[2] = reason
	**
	** - Unreal will translate this to TKL for us
	**
	*/
	void SendSQLine(const std::string &mask, const std::string &reason)
	{
		if (mask.empty() || reason.empty())
			return;
		send_cmd(NULL, "c %s :%s", mask.c_str(), reason.c_str());
	}

	/*
	** svso
	**	  parv[0] = sender prefix
	**	  parv[1] = nick
	**	  parv[2] = options
	*/
	void SendSVSO(const char *source, const char *nick, const char *flag)
	{
		if (!source || !nick || !flag) return;
		send_cmd(source, "BB %s %s", nick, flag);
	}

	/* NICK <newnick>  */
	void SendChangeBotNick(BotInfo *oldnick, const char *newnick)
	{
		if (!oldnick || !newnick) return;
		send_cmd(oldnick->nick, "& %s %ld", newnick, static_cast<long>(time(NULL)));
	}

	/* Functions that use serval cmd functions */

	void SendVhost(User *u, const std::string &vIdent, const std::string &vhost)
	{
		if (!vIdent.empty()) 
			unreal_cmd_chgident(u->nick.c_str(), vIdent.c_str());
		if (!vhost.empty())
			unreal_cmd_chghost(u->nick.c_str(), vhost.c_str());
	}

	void SendConnect()
	{
		unreal_cmd_capab();
		unreal_cmd_pass(uplink_server->password);
		me_server = new_server(NULL, Config.ServerName, Config.ServerDesc, SERVER_ISME, (Config.Numeric ? Config.Numeric : ""));
		SendServer(me_server);
	}

	/* SVSHOLD - set */
	void SendSVSHold(const char *nick)
	{
		send_cmd(NULL, "BD + Q H %s %s %ld %ld :%s", nick, Config.ServerName, static_cast<long>(time(NULL) + Config.NSReleaseTimeout),
			static_cast<long>(time(NULL)), "Being held for registered user");
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const char *nick)
	{
		send_cmd(NULL, "BD - Q * %s %s", nick, Config.ServerName);
	}

	/* UNSGLINE */
	/*
	 * SVSNLINE - :realname mask
	*/
	void SendSGLineDel(SXLine *sx)
	{
		send_cmd(NULL, "BR - :%s", sx->mask);
	}

	/* UNSZLINE */
	void SendSZLineDel(SXLine *sx)
	{
		send_cmd(NULL, "BD - Z * %s %s", sx->mask, Config.s_OperServ);
	}

	/* SZLINE */
	void SendSZLine(SXLine *sx)
	{
		send_cmd(NULL, "BD + Z * %s %s %ld %ld :%s", sx->mask, sx->by, static_cast<long>(time(NULL) + 172800), static_cast<long>(time(NULL)), sx->reason);
	}

	/* SGLINE */
	/*
	 * SVSNLINE + reason_where_is_space :realname mask with spaces
	*/
	void SendSGLine(SXLine *sx)
	{
		char edited_reason[BUFSIZE];
		strlcpy(edited_reason, sx->reason, BUFSIZE);
		strnrepl(edited_reason, BUFSIZE, " ", "_");
		send_cmd(NULL, "BR + %s :%s", edited_reason, sx->mask);
	}

	/* SVSMODE -b */
	void SendBanDel(Channel *c, const std::string &nick)
	{
		SendSVSModeChan(c, "-b", nick.empty() ? NULL : nick.c_str());
	}


	/* SVSMODE channel modes */

	void SendSVSModeChan(Channel *c, const char *mode, const char *nick)
	{
		if (nick) send_cmd(Config.ServerName, "n %s %s %s", c->name.c_str(), mode, nick);
		else send_cmd(Config.ServerName, "n %s %s", c->name.c_str(), mode);
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
	void SendSVSJoin(const char *source, const char *nick, const char *chan, const char *param)
	{
		if (param) send_cmd(source, "BX %s %s :%s", nick, chan, param);
		else send_cmd(source, "BX %s :%s", nick, chan);
	}

	/* svspart
		parv[0] - sender
		parv[1] - nick to make part
		parv[2] - channel(s) to part
	*/
	void SendSVSPart(const char *source, const char *nick, const char *chan)
	{
		send_cmd(source, "BT %s :%s", nick, chan);
	}

	void SendSWhois(const char *source, const char *who, const char *mask)
	{
		send_cmd(source, "BA %s :%s", who, mask);
	}

	void SendEOB()
	{
		send_cmd(Config.ServerName, "ES");
	}

	/*
	  1 = valid nick
	  0 = nick is in valid
	*/
	int IsNickValid(const char *nick)
	{
		if (!stricmp("ircd", nick) || !stricmp("irc", nick))
			return 0;
		return 1;
	}

	int IsChannelValid(const char *chan)
	{
		if (strchr(chan, ':') || *chan != '#') return 0;
		return 1;
	}

	void SetAutoIdentificationToken(User *u)
	{
		char svidbuf[15];

		if (!u->Account())
			return;

		srand(time(NULL));
		snprintf(svidbuf, sizeof(svidbuf), "%d", rand());

		u->Account()->Shrink("authenticationtoken");
		u->Account()->Extend("authenticationtoken", new ExtensibleItemPointerArray<char>(sstrdup(svidbuf)));

		BotInfo *bi = findbot(Config.s_NickServ);
		u->SetMode(bi, UMODE_REGISTERED);
		ircdproto->SendMode(bi, u, "+d %s", svidbuf);
	}

	void SendUnregisteredNick(User *u)
	{
		BotInfo *bi = findbot(Config.s_NickServ);
		u->RemoveMode(bi, UMODE_REGISTERED);
		ircdproto->SendMode(bi, u, "+d 1");
	}

} ircd_proto;



/* Event: PROTOCTL */
int anope_event_capab(const char *source, int ac, const char **av)
{
	for (int i = 0; i < ac; ++i)
	{
		std::string capab = av[i];

		if (capab.find("CHANMODES") != std::string::npos)
		{
			std::string modes(capab.begin() + 10, capab.end());
			commasepstream sep(modes);
			std::string modebuf;

			sep.GetToken(modebuf);
			for (size_t t = 0; t < modebuf.size(); ++t)
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
						ModeManager::AddChannelMode(new ChannelModeInvite('I'));
						continue;
					default:
						ModeManager::AddChannelMode(new ChannelModeList(CMODE_END, modebuf[t]));
				}
			}

			sep.GetToken(modebuf);
			for (size_t t = 0; t < modebuf.size(); ++t)
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
						ModeManager::AddChannelMode(new ChannelModeParam(CMODE_REDIRECT, 'L'));
						continue;
					default:
						ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, modebuf[t]));
				}
			}

			sep.GetToken(modebuf);
			for (size_t t = 0; t < modebuf.size(); ++t)
			{
				switch (modebuf[t])
				{
					case 'l':
						ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, 'l', true));
						continue;
					case 'j':
						ModeManager::AddChannelMode(new ChannelModeParam(CMODE_JOINFLOOD, 'j', true));
						continue;
					default:
						ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, modebuf[t], true));
				}
			}

			sep.GetToken(modebuf);
			for (size_t t = 0; t < modebuf.size(); ++t)
			{
				switch (modebuf[t])
				{
					case 'p':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, 'p'));
						continue;
					case 's':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, 's'));
						continue;
					case 'm':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, 'm'));
						continue;
					case 'n':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, 'n'));
						continue;
					case 't':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, 't'));
						continue;
					case 'i':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, 'i'));
						continue;
					case 'r':
						ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
						continue;
					case 'R':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_REGISTEREDONLY, 'R'));
						continue;
					case 'c':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCOLOR, 'c'));
						continue;
					case 'O':
						ModeManager::AddChannelMode(new ChannelModeOper('O'));
						continue;
					case 'A':
						ModeManager::AddChannelMode(new ChannelModeAdmin('A'));
						continue;
					case 'Q':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKICK, 'Q'));
						continue;
					case 'K':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKNOCK, 'K'));
						continue;
					case 'V':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NOINVITE, 'V'));
						continue;
					case 'C':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NOCTCP, 'C'));
						continue;
					case 'u':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_AUDITORIUM, 'u'));
						continue;
					case 'z':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_SSL, 'z'));
						continue;
					case 'N':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NONICK, 'N'));
						continue;
					case 'S':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_STRIPCOLOR, 'S'));
						continue;
					case 'M':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_REGMODERATED, 'M'));
						continue;
					case 'T':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_NONOTICE, 'T'));
						continue;
					case 'G':
						ModeManager::AddChannelMode(new ChannelMode(CMODE_FILTER, 'G'));
						continue;
					default:
						ModeManager::AddChannelMode(new ChannelMode(CMODE_END, modebuf[t]));
				}
			}
		}
	}

	CapabParse(ac, av);

	return MOD_CONT;
}

/* Events */
int anope_event_ping(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	ircdproto->SendPong(ac > 1 ? av[1] : Config.ServerName, av[0]);
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
int anope_event_netinfo(const char *source, int ac, const char **av)
{
	unreal_cmd_netinfo(ac, av);
	return MOD_CONT;
}

int anope_event_eos(const char *source, int ac, const char **av)
{
	Server *s;
	s = findserver(servlist, source);
	/* If we found a server with the given source, that one just
	 * finished bursting. If there was no source, then our uplink
	 * server finished bursting. -GD
	 */
	if (!s && serv_uplink)
		s = serv_uplink;
	finish_sync(s, 1);
	return MOD_CONT;
}

int anope_event_436(const char *source, int ac, const char **av)
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
int anope_event_away(const char *source, int ac, const char **av)
{
	if (!source) {
		return MOD_CONT;
	}
	m_away(source, (ac ? av[0] : NULL));
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
int anope_event_topic(const char *source, int ac, const char **av)
{
	if (ac != 4)
		return MOD_CONT;
	do_topic(source, ac, av);
	return MOD_CONT;
}

int anope_event_squit(const char *source, int ac, const char **av)
{
	if (ac != 2)
		return MOD_CONT;
	do_squit(source, ac, av);
	return MOD_CONT;
}

int anope_event_quit(const char *source, int ac, const char **av)
{
	if (ac != 1)
		return MOD_CONT;
	do_quit(source, ac, av);
	return MOD_CONT;
}

int anope_event_mode(const char *source, int ac, const char **av)
{
	if (ac < 2)
		return MOD_CONT;

	if (*av[0] == '#' || *av[0] == '&') {
		do_cmode(source, ac, av);
	} else {
		do_umode(source, ac, av);
	}
	return MOD_CONT;
}

/* This is used to strip the TS from the end of the mode stirng */
int anope_event_gmode(const char *source, int ac, const char **av)
{
        if (findserver(servlist, source))
                --ac;
        return anope_event_mode(source, ac, av);
}

/* Unreal sends USER modes with this */
/*
	umode2
	parv[0] - sender
	parv[1] - modes to change
*/
int anope_event_umode2(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;

	const char *newav[4];
	newav[0] = source;
	newav[1] = av[0];
	do_umode(source, ac, newav);
	return MOD_CONT;
}

int anope_event_kill(const char *source, int ac, const char **av)
{
	if (ac != 2)
		return MOD_CONT;

	m_kill(av[0], av[1]);
	return MOD_CONT;
}

int anope_event_kick(const char *source, int ac, const char **av)
{
	if (ac != 3)
		return MOD_CONT;
	do_kick(source, ac, av);
	return MOD_CONT;
}


int anope_event_join(const char *source, int ac, const char **av)
{
	if (ac != 1)
		return MOD_CONT;
	do_join(source, ac, av);
	return MOD_CONT;
}

int anope_event_motd(const char *source, int ac, const char **av)
{
	if (!source) {
		return MOD_CONT;
	}

	m_motd(source);
	return MOD_CONT;
}

int anope_event_setname(const char *source, int ac, const char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = finduser(source);
	if (!u) 
	{
		Alog(LOG_DEBUG) << "SETNAME for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetRealname(av[0]);
	return MOD_CONT;
}

int anope_event_chgname(const char *source, int ac, const char **av)
{
	User *u;

	if (ac != 2)
		return MOD_CONT;

	u = finduser(av[0]);
	if (!u)
	{
		Alog(LOG_DEBUG) << "CHGNAME for nonexistent user " << av[0];
		return MOD_CONT;
	}

	u->SetRealname(av[1]);
	return MOD_CONT;
}

int anope_event_setident(const char *source, int ac, const char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = finduser(source);
	if (!u) 
	{
		Alog(LOG_DEBUG) << "SETIDENT for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetIdent(av[0]);
	return MOD_CONT;
}
int anope_event_chgident(const char *source, int ac, const char **av)
{
	User *u;

	if (ac != 2)
		return MOD_CONT;

	u = finduser(av[0]);
	if (!u)
	{
		Alog(LOG_DEBUG) << "CHGIDENT for nonexistent user " << av[0];
		return MOD_CONT;
	}

	u->SetIdent(av[1]);
	return MOD_CONT;
}

int anope_event_sethost(const char *source, int ac, const char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = finduser(source);
	if (!u) 
	{
		Alog(LOG_DEBUG) << "SETHOST for nonexistent user " << source;
		return MOD_CONT;
	}

	/* If a user has a custom host and a server splits and reconnects
	 * Unreal does not send the users cloaked host to Anope.. so we do not know it.
	 * However, they will be +t if this is the case, so we will set their vhost
	 * to the sethost value (which really is their vhost) and clear the chost.
	 * The chost will be request later (if needed) - Adam
	 */
	if (u->HasMode(UMODE_VHOST))
	{
		u->SetDisplayedHost(av[0]);
		u->chost.clear();
	}
	/* If the new host doesn't match the real host or ip.. set it
	 * else we will not set a cloaked host, and request it later if needed
	 */
	else if ((u->host && strcmp(av[0], u->host)) || (u->hostip && strcmp(av[0], u->hostip)))
	{
		u->SetCloakedHost(av[0]);
	}

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
int anope_event_nick(const char *source, int ac, const char **av)
{
	User *user;

	if (ac != 2) {
		if (ac == 7) {
			/*
			   <codemastr> that was a bug that is now fixed in 3.2.1
			   <codemastr> in  some instances it would use the non-nickv2 format
			   <codemastr> it's sent when a nick collision occurs
			   - so we have to leave it around for now -TSL
			 */
			do_nick(source, av[0], av[3], av[4], av[5], av[6],
					strtoul(av[2], NULL, 10), 0, "*", NULL);

		} else if (ac == 11) {
			user = do_nick(source, av[0], av[3], av[4], av[5], av[10],
						   strtoul(av[2], NULL, 10), ntohl(decode_ip(av[9])), av[8], NULL);
			if (user)
			{
				/* Check to see if the user should be identified because their
				 * services id matches the one in their nickcore
				 */
				user->CheckAuthenticationToken(av[6]);

				UserSetInternalModes(user, 1, &av[7]);
			}

		} else {
			/* NON NICKIP */
			user = do_nick(source, av[0], av[3], av[4], av[5], av[9],
						   strtoul(av[2], NULL, 10), 0, av[8],
						   NULL);
			if (user)
			{
				/* Check to see if the user should be identified because their
				 * services id matches the one in their nickcore
				 */
				user->CheckAuthenticationToken(av[6]);

				UserSetInternalModes(user, 1, &av[7]);
			}
		}
	} else {
		do_nick(source, av[0], NULL, NULL, NULL, NULL,
				strtoul(av[1], NULL, 10), 0, NULL, NULL);
	}
	return MOD_CONT;
}


int anope_event_chghost(const char *source, int ac, const char **av)
{
	User *u;

	if (ac != 2)
		return MOD_CONT;

	u = finduser(av[0]);
	if (!u)
	{
		Alog(LOG_DEBUG) << "debug: CHGHOST for nonexistent user " << av[0];
		return MOD_CONT;
	}

	u->SetDisplayedHost(av[1]);
	return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(const char *source, int ac, const char **av)
{
	char *desc;
	char *vl;
	char *upnumeric;

	if (!stricmp(av[1], "1")) {
		uplink = sstrdup(av[0]);
		vl = myStrGetToken(av[2], ' ', 0);
		upnumeric = myStrGetToken(vl, '-', 2);
		desc = myStrGetTokenRemainder(av[2], ' ', 1);
		do_server(source, av[0], av[1], desc, upnumeric);
		delete [] vl;
		delete [] desc;
		delete [] upnumeric;
	} else {
		do_server(source, av[0], av[1], av[2], "");
	}

	return MOD_CONT;
}

int anope_event_privmsg(const char *source, int ac, const char **av)
{
	if (ac != 2)
		return MOD_CONT;
	m_privmsg(source, av[0], av[1]);
	return MOD_CONT;
}

int anope_event_part(const char *source, int ac, const char **av)
{
	if (ac < 1 || ac > 2)
		return MOD_CONT;
	do_part(source, ac, av);
	return MOD_CONT;
}

int anope_event_whois(const char *source, int ac, const char **av)
{
	if (source && ac >= 1) {
		m_whois(source, av[0]);
	}
	return MOD_CONT;
}


int anope_event_error(const char *source, int ac, const char **av)
{
	if (av[0])
	{
		Alog(LOG_DEBUG) << av[0];
		if(strstr(av[0],"No matching link configuration")!=0) 
			Alog() << "Error: Your IRCD's link block may not be setup correctly, please check unrealircd.conf";
	}
	return MOD_CONT;

}

int anope_event_sdesc(const char *source, int ac, const char **av)
{
	Server *s;
	s = findserver(servlist, source);

	if (s) {
		s->desc = const_cast<char *>(av[0]); // XXX Unsafe cast -- CyberBotX
	}

	return MOD_CONT;
}

int anope_event_userhost(const char *source, int ac, const char **av)
{
	/** Hack to get around Unreal:
	 * This is the USERHOST reply, we only send a request if we do not know the users cloaked host
	 * (they got introducted using a vhost) - Adam
	 */
	if (ac < 2 || !av[1] || !*av[1])
		return MOD_CONT;

	std::string reply = av[1];
	std::string user = std::string(reply.begin(), std::find(reply.begin(), reply.end(), '='));
	if (user[user.length() - 1] == '*')
		user.erase(user.length() - 1);
	std::string host = std::string(std::find(reply.begin(), reply.end(), '@'), reply.end());
	host.erase(host.begin());

	User *u = finduser(user);
	if (u)
	{
		u->SetCloakedHost(host);
	}

	return MOD_CONT;
}

int anope_event_sjoin(const char *source, int ac, const char **av)
{
	Channel *c = findchan(av[1]);
	time_t ts = base64dects(av[0]);
	bool keep_their_modes = true;
	bool was_created = false;

	if (!c)
	{
		c = new Channel(av[1], ts);
		was_created = true;
	}
	/* Our creation time is newer than what the server gave us */
	else if (c->creation_time > ts)
	{
		c->creation_time = ts;

		/* Remove status from all of our users */
		for (CUserList::iterator it = c->users.begin(); it != c->users.end(); ++it)
		{
			UserContainer *uc = *it;

			c->RemoveMode(NULL, CMODE_OWNER, uc->user->nick);
			c->RemoveMode(NULL, CMODE_PROTECT, uc->user->nick);
			c->RemoveMode(NULL, CMODE_OP, uc->user->nick);
			c->RemoveMode(NULL, CMODE_HALFOP, uc->user->nick);
			c->RemoveMode(NULL, CMODE_VOICE, uc->user->nick);
		}
		if (c->ci)
		{
			/* Rejoin the bot to fix the TS */
			if (c->ci->bi)
			{
				ircdproto->SendPart(c->ci->bi, c, "TS reop");
				bot_join(c->ci);
			}
			/* Reset mlock */
			check_modes(c);
		}
	}
	/* Their TS is newer than ours, our modes > theirs, unset their modes if need be */
	else
		keep_their_modes = false;
	
	/* Mark the channel as syncing */
	if (was_created)
		c->SetFlag(CH_SYNCING);

	/* If we need to keep their modes, and this SJOIN string contains modes */
	if (keep_their_modes && ac >= 4)
	{
		/* Set the modes internally */
		ChanSetInternalModes(c, ac - 3, av + 2);
	}

	spacesepstream sep(av[ac - 1]);
	std::string buf;
	while (sep.GetToken(buf))
	{
		/* Ban */
		if (keep_their_modes && buf[0] == '&')
		{
			buf.erase(buf.begin());
			ChannelModeList *cml = dynamic_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_BAN));
			if (cml->IsValid(buf))
				cml->AddMask(c, buf.c_str());
		}
		/* Except */
		else if (keep_their_modes && buf[0] == '"')
		{
			buf.erase(buf.begin());
			ChannelModeList *cml = dynamic_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_EXCEPT));

			if (cml->IsValid(buf))
				cml->AddMask(c, buf.c_str());
		}
		/* Invex */
		else if (keep_their_modes && buf[0] == '\'')
		{
			buf.erase(buf.begin());
			ChannelModeList *cml = dynamic_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE));

			if (cml->IsValid(buf))
				cml->AddMask(c, buf.c_str());
		}
		else
		{
			std::list<ChannelMode *> Status;
			Status.clear();
			char ch;

			/* Get prefixes from the nick */
			while ((ch = ModeManager::GetStatusChar(buf[0])))
			{
				buf.erase(buf.begin());
				ChannelMode *cm = ModeManager::FindChannelModeByChar(ch);
				if (!cm)
				{
					Alog() << "Recieved unknown mode prefix " << buf[0] << " in SJOIN string";
					continue;
				}

				Status.push_back(cm);
			}

			User *u = finduser(buf);
			if (!u)
			{
				Alog(LOG_DEBUG) << "SJOIN for nonexistant user " << buf << " on " << c->name;
				continue;
			}

			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnPreJoinChannel, OnPreJoinChannel(u, c));

			/* Add the user to the channel */
			c->JoinUser(u);

			/* Update their status internally on the channel
			 * This will enforce secureops etc on the user
			 */
			for (std::list<ChannelMode *>::iterator it = Status.begin(); it != Status.end(); ++it)
			{
				c->SetModeInternal(*it, buf);
			}
			
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
	if (was_created)
	{
		/* Unset the syncing flag */
		c->UnsetFlag(CH_SYNCING);

		/* If there are users in the channel they are allowed to be, set topic mlock etc. */
		if (!c->users.empty())
			c->Sync();
		/* If there are no users in the channel, there is a ChanServ timer set to part the service bot
		 * and destroy the channel soon
		 */
	}

	return MOD_CONT;
}

void moduleAddIRCDMsgs() {
	Message *m;

	m = createMessage("436",	   anope_event_436); addCoreMessage(IRCD,m);
	m = createMessage("AWAY",	  anope_event_away); addCoreMessage(IRCD,m);
	m = createMessage("6",		anope_event_away); addCoreMessage(IRCD,m);
	m = createMessage("JOIN",	  anope_event_join); addCoreMessage(IRCD,m);
	m = createMessage("C",		anope_event_join); addCoreMessage(IRCD,m);
	m = createMessage("KICK",	  anope_event_kick); addCoreMessage(IRCD,m);
	m = createMessage("H",		anope_event_kick); addCoreMessage(IRCD,m);
	m = createMessage("KILL",	  anope_event_kill); addCoreMessage(IRCD,m);
	m = createMessage(".",		anope_event_kill); addCoreMessage(IRCD,m);
	m = createMessage("MODE",	  anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("G",		anope_event_gmode); addCoreMessage(IRCD,m);
	m = createMessage("MOTD",	  anope_event_motd); addCoreMessage(IRCD,m);
	m = createMessage("F",		anope_event_motd); addCoreMessage(IRCD,m);
	m = createMessage("NICK",	  anope_event_nick); addCoreMessage(IRCD,m);
	m = createMessage("&",		anope_event_nick); addCoreMessage(IRCD,m);
	m = createMessage("PART",	  anope_event_part); addCoreMessage(IRCD,m);
	m = createMessage("D",		anope_event_part); addCoreMessage(IRCD,m);
	m = createMessage("PING",	  anope_event_ping); addCoreMessage(IRCD,m);
	m = createMessage("8",		anope_event_ping); addCoreMessage(IRCD,m);
	m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
	m = createMessage("!",		anope_event_privmsg); addCoreMessage(IRCD,m);
	m = createMessage("QUIT",	  anope_event_quit); addCoreMessage(IRCD,m);
	m = createMessage(",",		anope_event_quit); addCoreMessage(IRCD,m);
	m = createMessage("SERVER",	anope_event_server); addCoreMessage(IRCD,m);
	m = createMessage("'",		anope_event_server); addCoreMessage(IRCD,m);
	m = createMessage("SQUIT",	 anope_event_squit); addCoreMessage(IRCD,m);
	m = createMessage("-",		anope_event_squit); addCoreMessage(IRCD,m);
	m = createMessage("TOPIC",	 anope_event_topic); addCoreMessage(IRCD,m);
	m = createMessage(")",		anope_event_topic); addCoreMessage(IRCD,m);
	m = createMessage("SVSMODE",   anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("n",		anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("SVS2MODE",   anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("v",	   anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("WHOIS",	 anope_event_whois); addCoreMessage(IRCD,m);
	m = createMessage("#",		anope_event_whois); addCoreMessage(IRCD,m);
	m = createMessage("PROTOCTL",  anope_event_capab); addCoreMessage(IRCD,m);
	m = createMessage("_",		anope_event_capab); addCoreMessage(IRCD,m);
	m = createMessage("CHGHOST",   anope_event_chghost); addCoreMessage(IRCD,m);
	m = createMessage("AL",		anope_event_chghost); addCoreMessage(IRCD,m);
	m = createMessage("CHGIDENT",  anope_event_chgident); addCoreMessage(IRCD,m);
	m = createMessage("AZ",		anope_event_chgident); addCoreMessage(IRCD,m);
	m = createMessage("CHGNAME",   anope_event_chgname); addCoreMessage(IRCD,m);
	m = createMessage("BK",		anope_event_chgname); addCoreMessage(IRCD,m);
	m = createMessage("NETINFO",   anope_event_netinfo); addCoreMessage(IRCD,m);
	m = createMessage("AO",	   anope_event_netinfo); addCoreMessage(IRCD,m);
	m = createMessage("SETHOST",   anope_event_sethost); addCoreMessage(IRCD,m);
	m = createMessage("AA",		anope_event_sethost); addCoreMessage(IRCD,m);
	m = createMessage("SETIDENT",  anope_event_setident); addCoreMessage(IRCD,m);
	m = createMessage("AD",		anope_event_setident); addCoreMessage(IRCD,m);
	m = createMessage("SETNAME",   anope_event_setname); addCoreMessage(IRCD,m);
	m = createMessage("AE",		anope_event_setname); addCoreMessage(IRCD,m);
	m = createMessage("EOS", 	   anope_event_eos); addCoreMessage(IRCD,m);
	m = createMessage("ES",	   anope_event_eos); addCoreMessage(IRCD,m);
	m = createMessage("ERROR", 	   anope_event_error); addCoreMessage(IRCD,m);
	m = createMessage("5",		anope_event_error); addCoreMessage(IRCD,m);
	m = createMessage("UMODE2",	anope_event_umode2); addCoreMessage(IRCD,m);
	m = createMessage("|",		anope_event_umode2); addCoreMessage(IRCD,m);
	m = createMessage("SJOIN",	  anope_event_sjoin); addCoreMessage(IRCD,m);
	m = createMessage("~",		anope_event_sjoin); addCoreMessage(IRCD,m);
	m = createMessage("SDESC",	  anope_event_sdesc); addCoreMessage(IRCD,m);
	m = createMessage("AG",	   anope_event_sdesc); addCoreMessage(IRCD,m);
	m = createMessage("302",	anope_event_userhost); addCoreMessage(IRCD,m);

	/* The non token version of these is in messages.c */
	m = createMessage("2",		 m_stats); addCoreMessage(IRCD,m);
	m = createMessage(">",		 m_time); addCoreMessage(IRCD,m);
	m = createMessage("+",		 m_version); addCoreMessage(IRCD,m);
}

/* Borrowed part of this check from UnrealIRCd */
bool ChannelModeFlood::IsValid(const std::string &value2)
{
	const char *value = value2.c_str();
	char *dp, *end;
	/* NEW +F */
	char xbuf[256], *p, *p2, *x = xbuf + 1;
	int v;
	if (!value) return 0;
	if (*value != ':' && strtoul((*value == '*' ? value + 1 : value), &dp, 10) > 0 && *dp == ':' && *(++dp) && strtoul(dp, &end, 10) > 0 && !*end) return 1;
	else {
		/* '['<number><1 letter>[optional: '#'+1 letter],[next..]']'':'<number> */
		strlcpy(xbuf, value, sizeof(xbuf));
		p2 = strchr(xbuf + 1, ']');
		if (!p2) return 0;
		*p2 = '\0';
		if (*(p2 + 1) != ':') return 0;
		for (x = strtok(xbuf + 1, ","); x; x = strtok(NULL, ",")) {
			/* <number><1 letter>[optional: '#'+1 letter] */
			p = x;
			while (isdigit(*p)) ++p;
			if (!*p || !(*p == 'c' || *p == 'j' || *p == 'k' || *p == 'm' || *p == 'n' || *p == 't')) continue; /* continue instead of break for forward compatability. */
			*p = '\0';
			v = atoi(x);
			if (v < 1 || v > 999) return 0;
			++p;
		}
		return 1;
	}
}

static void AddModes()
{
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, 'v', '+'));
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_HALFOP, 'h', '%'));
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, 'o', '@'));
	/* Unreal sends +q as * and +a as ~ */
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT, 'a', '~'));
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, 'q', '*'));

	/* Add user modes */
	ModeManager::AddUserMode(new UserMode(UMODE_SERV_ADMIN, 'A'));
	ModeManager::AddUserMode(new UserMode(UMODE_BOT, 'B'));
	ModeManager::AddUserMode(new UserMode(UMODE_CO_ADMIN, 'C'));
	ModeManager::AddUserMode(new UserMode(UMODE_FILTER, 'G'));
	ModeManager::AddUserMode(new UserMode(UMODE_HIDEOPER, 'H'));
	ModeManager::AddUserMode(new UserMode(UMODE_NETADMIN, 'N'));
	ModeManager::AddUserMode(new UserMode(UMODE_REGPRIV, 'R'));
	ModeManager::AddUserMode(new UserMode(UMODE_PROTECTED, 'S'));
	ModeManager::AddUserMode(new UserMode(UMODE_NO_CTCP, 'T'));
	ModeManager::AddUserMode(new UserMode(UMODE_WEBTV, 'V'));
	ModeManager::AddUserMode(new UserMode(UMODE_WHOIS, 'W'));
	ModeManager::AddUserMode(new UserMode(UMODE_ADMIN, 'a'));
	ModeManager::AddUserMode(new UserMode(UMODE_DEAF, 'd'));
	ModeManager::AddUserMode(new UserMode(UMODE_GLOBOPS, 'g'));
	ModeManager::AddUserMode(new UserMode(UMODE_HELPOP, 'h'));
	ModeManager::AddUserMode(new UserMode(UMODE_INVIS, 'i'));
	ModeManager::AddUserMode(new UserMode(UMODE_OPER, 'o'));
	ModeManager::AddUserMode(new UserMode(UMODE_PRIV, 'p'));
	ModeManager::AddUserMode(new UserMode(UMODE_GOD, 'q'));
	ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, 'r'));
	ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, 's'));
	ModeManager::AddUserMode(new UserMode(UMODE_VHOST, 't'));
	ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, 'w'));
	ModeManager::AddUserMode(new UserMode(UMODE_CLOAK, 'x'));
	ModeManager::AddUserMode(new UserMode(UMODE_SSL, 'z'));
}

class ProtoUnreal : public Module
{
 public:
	ProtoUnreal(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(PROTOCOL);

		pmodule_ircd_version("UnrealIRCd 3.2+");
		pmodule_ircd_var(myIrcd);
		pmodule_ircd_useTSMode(0);

		CapabType c[] = { CAPAB_NOQUIT, CAPAB_NICKIP, CAPAB_ZIP, CAPAB_TOKEN, CAPAB_SSJ3, CAPAB_NICK2, CAPAB_VL, CAPAB_TLKEXT, CAPAB_CHANMODE, CAPAB_SJB64, CAPAB_NICKCHARS };
		for (unsigned i = 0; i < 11; ++i)
			Capab.SetFlag(c[i]);

		AddModes();

		pmodule_ircd_proto(&ircd_proto);
		moduleAddIRCDMsgs();
	}

};

MODULE_INIT(ProtoUnreal)
