/* Bahamut functions
 *
 * (C) 2003-2009 Anope Team
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
	{"Bahamut 1.8.x", /* ircd name */
	 "+",					   /* Modes used by pseudoclients */
	 2,						 /* Chan Max Symbols	 */
	 "+o",					  /* Channel Umode used by Botserv bots */
	 1,						 /* SVSNICK */
	 0,						 /* Vhost  */
	 1,						 /* Supports SGlines	 */
	 1,						 /* Supports SQlines	 */
	 1,						 /* Supports SZlines	 */
	 3,						 /* Number of server args */
	 0,						 /* Join 2 Set		   */
	 0,						 /* Join 2 Message	   */
	 0,						 /* TS Topic Forward	 */
	 0,						 /* TS Topci Backward	*/
	 1,						 /* Chan SQlines		 */
	 1,						 /* Quit on Kill		 */
	 1,						 /* SVSMODE unban		*/
	 0,						 /* Reverse			  */
	 0,						 /* vidents			  */
	 1,						 /* svshold			  */
	 1,						 /* time stamp on mode   */
	 1,						 /* NICKIP			   */
	 0,						 /* O:LINE			   */
	 1,						 /* UMODE			   */
	 0,						 /* VHOST ON NICK		*/
	 0,						 /* Change RealName	  */
	 1,
	 1,						 /* No Knock requires +i */
	 NULL,					  /* CAPAB Chan Modes			 */
	 0,						 /* We support TOKENS */
	 0,						 /* TIME STAMPS are BASE64 */
	 0,						 /* SJOIN ban char */
	 0,						 /* SJOIN except char */
	 0,						 /* SJOIN invite char */
	 0,						 /* Can remove User Channel Modes with SVSMODE */
	 0,						 /* Sglines are not enforced until user reconnects */
	 0,						 /* ts6 */
	 0,						 /* p10 */
	 NULL,					  /* character set */
	 0,						 /* CIDR channelbans */
	 "$",					   /* TLD Prefix for Global */
	 false,					/* Auth for users is sent after the initial NICK/UID command */
	 }
	,
	{NULL}
};

IRCDCAPAB myIrcdcap[] = {
	{
	 CAPAB_NOQUIT,			  /* NOQUIT	   */
	 CAPAB_TSMODE,			  /* TSMODE	   */
	 CAPAB_UNCONNECT,		   /* UNCONNECT	*/
	 0,						 /* NICKIP	   */
	 0,						 /* SJOIN		*/
	 0,						 /* ZIP		  */
	 CAPAB_BURST,			   /* BURST		*/
	 0,						 /* TS5		  */
	 0,						 /* TS3		  */
	 CAPAB_DKEY,				/* DKEY		 */
	 0,						 /* PT4		  */
	 0,						 /* SCS		  */
	 0,						 /* QS		   */
	 0,						 /* UID		  */
	 0,						 /* KNOCK		*/
	 0,						 /* CLIENT	   */
	 0,						 /* IPV6		 */
	 0,						 /* SSJ5		 */
	 0,						 /* SN2		  */
	 0,						 /* TOKEN		*/
	 0,						 /* VHOST		*/
	 0,						 /* SSJ3		 */
	 0,						 /* NICK2		*/
	 0,						 /* VL		   */
	 0,						 /* TLKEXT	   */
	 0,						 /* DODKEY	   */
	 CAPAB_DOZIP,			   /* DOZIP		*/
	 0, 0, 0}
};


void bahamut_cmd_burst()
{
	send_cmd(NULL, "BURST");
}

/*
 * SVINFO
 *	   parv[0] = sender prefix
 *	   parv[1] = TS_CURRENT for the server
 *	   parv[2] = TS_MIN for the server
 *	   parv[3] = server is standalone or connected to non-TS only
 *	   parv[4] = server's idea of UTC time
 */
void bahamut_cmd_svinfo()
{
	send_cmd(NULL, "SVINFO 3 1 0 :%ld", static_cast<long>(time(NULL)));
}

/* PASS */
void bahamut_cmd_pass(const char *pass)
{
	send_cmd(NULL, "PASS %s :TS", pass);
}

/* CAPAB */
void bahamut_cmd_capab()
{
	send_cmd(NULL,
			 "CAPAB SSJOIN NOQUIT BURST UNCONNECT NICKIP TSMODE TS3");
}

/* this avoids "undefined symbol" messages of those whom try to load mods that
   call on this function */
void bahamut_cmd_chghost(const char *nick, const char *vhost)
{
	if (debug) {
		alog("debug: This IRCD does not support vhosting");
	}
}



class BahamutIRCdProto : public IRCDProto
{
	void SendModeInternal(BotInfo *source, const char *dest, const char *buf)
	{
		if (!buf) return;
		if (ircdcap->tsmode && (uplink_capab & ircdcap->tsmode)) send_cmd(source->nick, "MODE %s 0 %s", dest, buf);
		else send_cmd(source->nick, "MODE %s %s", dest, buf);
	}

	void SendModeInternal(User *u, const char *buf)
	{
		if (!buf) return;
		send_cmd(Config.ServerName, "SVSMODE %s %ld %s", u->nick, static_cast<long>(u->timestamp), buf);
	}

	/* SVSHOLD - set */
	void SendSVSHold(const char *nick)
	{
		send_cmd(Config.ServerName, "SVSHOLD %s %u :%s", nick, static_cast<unsigned>(Config.NSReleaseTimeout), "Being held for registered user");
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const char *nick)
	{
		send_cmd(Config.ServerName, "SVSHOLD %s 0", nick);
	}

	/* SVSMODE -b */
	void SendBanDel(Channel *c, const char *nick)
	{
		SendSVSModeChan(c, "-b", nick);
	}

	/* SVSMODE channel modes */
	void SendSVSModeChan(Channel *c, const char *mode, const char *nick)
	{
		if (nick) send_cmd(Config.ServerName, "SVSMODE %s %s %s", c->name, mode, nick);
		else send_cmd(Config.ServerName, "SVSMODE %s %s", c->name, mode);
	}

	/* SQLINE */
	void SendSQLine(const char *mask, const char *reason)
	{
		if (!mask || !reason) return;
		send_cmd(NULL, "SQLINE %s :%s", mask, reason);
	}

	/* UNSGLINE */
	void SendSGLineDel(SXLine *sx)
	{
		send_cmd(NULL, "UNSGLINE 0 :%s", sx->mask);
	}

	/* UNSZLINE */
	void SendSZLineDel(SXLine *sx)
	{
		/* this will likely fail so its only here for legacy */
		send_cmd(NULL, "UNSZLINE 0 %s", sx->mask);
		/* this is how we are supposed to deal with it */
		send_cmd(NULL, "RAKILL %s *", sx->mask);
	}

	/* SZLINE */
	void SendSZLine(SXLine *sx)
	{
		/* this will likely fail so its only here for legacy */
		send_cmd(NULL, "SZLINE %s :%s", sx->mask, sx->reason);
		/* this is how we are supposed to deal with it */
		send_cmd(NULL, "AKILL %s * %d %s %ld :%s", sx->mask, 172800, sx->by, static_cast<long>(time(NULL)), sx->reason);
	}

	/* SVSNOOP */
	void SendSVSNOOP(const char *server, int set)
	{
		send_cmd(NULL, "SVSNOOP %s %s", server, set ? "+" : "-");
	}

	/* SGLINE */
	void SendSGLine(SXLine *sx)
	{
		send_cmd(NULL, "SGLINE %d :%s:%s", static_cast<int>(strlen(sx->mask)), sx->mask, sx->reason);
	}

	/* RAKILL */
	void SendAkillDel(Akill *ak)
	{
		send_cmd(NULL, "RAKILL %s %s", ak->host, ak->user);
	}

	/* TOPIC */
	void SendTopic(BotInfo *whosets, Channel *c, const char *whosetit, const char *topic)
	{
		send_cmd(whosets->nick, "TOPIC %s %s %lu :%s", c->name, whosetit, static_cast<unsigned long>(c->topic_time), topic);
	}

	/* UNSQLINE */
	void SendSQLineDel(const char *user)
	{
		send_cmd(NULL, "UNSQLINE %s", user);
	}

	/* JOIN - SJOIN */
	void SendJoin(BotInfo *user, const char *channel, time_t chantime)
	{
		send_cmd(user->nick, "SJOIN %ld %s", static_cast<long>(chantime), channel);
	}

	void SendAkill(Akill *ak)
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = ak->expires - time(NULL);
		if (timeleft > 172800) timeleft = 172800;
		send_cmd(NULL, "AKILL %s %s %d %s %ld :%s", ak->host, ak->user, static_cast<int>(timeleft), ak->by, static_cast<long>(time(NULL)), ak->reason);
	}

	/*
	  Note: if the stamp is null 0, the below usage is correct of Bahamut
	*/
	void SendSVSKillInternal(BotInfo *source, User *user, const char *buf)
	{
		send_cmd(source ? source->nick : NULL, "SVSKILL %s :%s", user->nick, buf);
	}

	/* SVSMODE */
	/* parv[0] - sender
	 * parv[1] - nick
	 * parv[2] - TS (or mode, depending on svs version)
	 * parv[3] - mode (or services id if old svs version)
	 * parv[4] - optional arguement (services id)
	 */
	void SendSVSMode(User *u, int ac, const char **av)
	{
		this->SendModeInternal(u, merge_args(ac, av));
	}

	void SendEOB()
	{
		send_cmd(NULL, "BURST 0");
	}

	void SendNoticeChanopsInternal(BotInfo *source, Channel *dest, const char *buf)
	{
		if (!buf) return;
		send_cmd(NULL, "NOTICE @%s :%s", dest->name, buf);
	}

	void SendKickInternal(BotInfo *source, Channel *chan, User *user, const char *buf)
	{
		if (buf) send_cmd(source->nick, "KICK %s %s :%s", chan->name, user->nick, buf);
		else send_cmd(source->nick, "KICK %s %s", chan->name, user->nick);
	}

	void SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes, const char *uid)
	{
		EnforceQlinedNick(nick, Config.s_BotServ);
		send_cmd(NULL, "NICK %s 1 %ld %s %s %s %s 0 0 :%s", nick, static_cast<long>(time(NULL)), modes, user, host, Config.ServerName, real);
		SendSQLine(nick, "Reserved for services");
	}

	/* SVSMODE +d */
	/* nc_change was = 1, and there is no na->status */
	void SendUnregisteredNick(User *u)
	{
		u->RemoveMode(UMODE_REGISTERED);
		ircdproto->SendMode(u, "+d 1");
	}

	/* SERVER */
	void SendServer(Server *server)
	{
		send_cmd(NULL, "SERVER %s %d :%s", server->name, server->hops, server->desc);
	}

	void SendConnect()
	{
		bahamut_cmd_pass(uplink_server->password);
		bahamut_cmd_capab();
		me_server = new_server(NULL, Config.ServerName, Config.ServerDesc, SERVER_ISME, NULL);
		SendServer(me_server);
		bahamut_cmd_svinfo();
		bahamut_cmd_burst();
	}

	void SetAutoIdentificationToken(User *u)
	{
		char svidbuf[15], *c;

		if (!u->nc)
			return;

		srand(time(NULL));
		snprintf(svidbuf, sizeof(svidbuf), "%d", rand());

		if (u->nc->GetExt("authenticationtoken", c))
		{
			delete [] c;
			u->nc->Shrink("authenticationtoken");
		}

		u->nc->Extend("authenticationtoken", sstrdup(svidbuf));

		u->SetMode(UMODE_REGISTERED);
		ircdproto->SendMode(u, "+d %s", svidbuf);
	}

} ircd_proto;


/* EVENT: SJOIN */
int anope_event_sjoin(const char *source, int ac, const char **av)
{
	do_sjoin(source, ac, av);
	return MOD_CONT;
}

/*
** NICK - new
**	  source  = NULL
**	parv[0] = nickname
**	  parv[1] = hopcount
**	  parv[2] = timestamp
**	  parv[3] = modes
**	  parv[4] = username
**	  parv[5] = hostname
**	  parv[6] = server
**	parv[7] = servicestamp
**	  parv[8] = IP
**	parv[9] = info
** NICK - change
**	  source  = oldnick
**	parv[0] = new nickname
**	  parv[1] = hopcount
*/
int anope_event_nick(const char *source, int ac, const char **av)
{
	User *user;

	if (ac != 2) {
		user = do_nick(source, av[0], av[4], av[5], av[6], av[9],
					   strtoul(av[2], NULL, 10), strtoul(av[8], NULL, 0),
					   NULL, NULL);
		if (user) {
			/* Check to see if the user should be identified because their
			 * services id matches the one in their nickcore
			 */
			user->CheckAuthenticationToken(av[7]);

			UserSetInternalModes(user, 1, &av[3]);
		}
	} else {
		do_nick(source, av[0], NULL, NULL, NULL, NULL,
				strtoul(av[1], NULL, 10), 0, NULL, NULL);
	}
	return MOD_CONT;
}

/* EVENT : CAPAB */
int anope_event_capab(const char *source, int ac, const char **av)
{
	capab_parse(ac, av);
	return MOD_CONT;
}

/* EVENT : OS */
int anope_event_os(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	m_privmsg(source, Config.s_OperServ, av[0]);
	return MOD_CONT;
}

/* EVENT : NS */
int anope_event_ns(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	m_privmsg(source, Config.s_NickServ, av[0]);
	return MOD_CONT;
}

/* EVENT : MS */
int anope_event_ms(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	m_privmsg(source, Config.s_MemoServ, av[0]);
	return MOD_CONT;
}

/* EVENT : HS */
int anope_event_hs(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	m_privmsg(source, Config.s_HostServ, av[0]);
	return MOD_CONT;
}

/* EVENT : CS */
int anope_event_cs(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	m_privmsg(source, Config.s_ChanServ, av[0]);
	return MOD_CONT;
}

int anope_event_436(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;

	m_nickcoll(av[0]);
	return MOD_CONT;
}

/* EVENT : SERVER */
int anope_event_server(const char *source, int ac, const char **av)
{
	if (!stricmp(av[1], "1")) {
		uplink = sstrdup(av[0]);
	}

	do_server(source, av[0], av[1], av[2], NULL);
	return MOD_CONT;
}

/* EVENT : PRIVMSG */
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

/* EVENT: MODE */
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

/* EVENT: KILL */
int anope_event_kill(const char *source, int ac, const char **av)
{
	if (ac != 2)
		return MOD_CONT;

	m_kill(av[0], av[1]);
	return MOD_CONT;
}

/* EVENT: KICK */
int anope_event_kick(const char *source, int ac, const char **av)
{
	if (ac != 3)
		return MOD_CONT;
	do_kick(source, ac, av);
	return MOD_CONT;
}

/* EVENT: JOIN */
int anope_event_join(const char *source, int ac, const char **av)
{
	if (ac != 1)
		return MOD_CONT;
	do_join(source, ac, av);
	return MOD_CONT;
}

/* EVENT: MOTD */
int anope_event_motd(const char *source, int ac, const char **av)
{
	if (!source) {
		return MOD_CONT;
	}

	m_motd(source);
	return MOD_CONT;
}

int anope_event_away(const char *source, int ac, const char **av)
{
	if (!source) {
		return MOD_CONT;
	}
	m_away(source, (ac ? av[0] : NULL));
	return MOD_CONT;
}

int anope_event_ping(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	ircdproto->SendPong(ac > 1 ? av[1] : Config.ServerName, av[0]);
	return MOD_CONT;
}

int anope_event_error(const char *source, int ac, const char **av)
{
	if (ac >= 1) {
		if (debug) {
			alog("debug: %s", av[0]);
		}
	}
	return MOD_CONT;
}

int anope_event_burst(const char *source, int ac, const char **av)
{
	Server *s;
	s = findserver(servlist, source);
	if (!ac) {
		/* for future use  - start burst */
	} else {
		/* If we found a server with the given source, that one just
		 * finished bursting. If there was no source, then our uplink
		 * server finished bursting. -GD
		 */
		if (!s && serv_uplink)
			s = serv_uplink;
		finish_sync(s, 1);
	}
	return MOD_CONT;
}

bool ChannelModeFlood::IsValid(const char *value)
{
	char *dp, *end;

	if (value && *value != ':' && strtoul((*value == '*' ? value + 1 : value), &dp, 10) > 0 && *dp == ':' && *(++dp) && strtoul(dp, &end, 10) > 0 && !*end)
		return true;

	return false;
}

void moduleAddIRCDMsgs() {
	Message *m;

	/* now add the commands */
	m = createMessage("436",	   anope_event_436); addCoreMessage(IRCD,m);
	m = createMessage("AWAY",	  anope_event_away); addCoreMessage(IRCD,m);
	m = createMessage("JOIN",	  anope_event_join); addCoreMessage(IRCD,m);
	m = createMessage("KICK",	  anope_event_kick); addCoreMessage(IRCD,m);
	m = createMessage("KILL",	  anope_event_kill); addCoreMessage(IRCD,m);
	m = createMessage("MODE",	  anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("MOTD",	  anope_event_motd); addCoreMessage(IRCD,m);
	m = createMessage("NICK",	  anope_event_nick); addCoreMessage(IRCD,m);
	m = createMessage("PART",	  anope_event_part); addCoreMessage(IRCD,m);
	m = createMessage("PING",	  anope_event_ping); addCoreMessage(IRCD,m);
	m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
	m = createMessage("QUIT",	  anope_event_quit); addCoreMessage(IRCD,m);
	m = createMessage("SERVER",	anope_event_server); addCoreMessage(IRCD,m);
	m = createMessage("SQUIT",	 anope_event_squit); addCoreMessage(IRCD,m);
	m = createMessage("TOPIC",	 anope_event_topic); addCoreMessage(IRCD,m);
	m = createMessage("WHOIS",	 anope_event_whois); addCoreMessage(IRCD,m);
	m = createMessage("SVSMODE",   anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("CAPAB", 	   anope_event_capab); addCoreMessage(IRCD,m);
	m = createMessage("CS",		anope_event_cs); addCoreMessage(IRCD,m);
	m = createMessage("HS",		anope_event_hs); addCoreMessage(IRCD,m);
	m = createMessage("MS",		anope_event_ms); addCoreMessage(IRCD,m);
	m = createMessage("NS",		anope_event_ns); addCoreMessage(IRCD,m);
	m = createMessage("OS",		anope_event_os); addCoreMessage(IRCD,m);
	m = createMessage("SJOIN",	 anope_event_sjoin); addCoreMessage(IRCD,m);
	m = createMessage("ERROR",	 anope_event_error); addCoreMessage(IRCD,m);
	m = createMessage("BURST",	 anope_event_burst); addCoreMessage(IRCD,m);
}

void moduleAddModes()
{
	/* Add user modes */
	ModeManager::AddUserMode('A', new UserMode(UMODE_SERV_ADMIN));
	ModeManager::AddUserMode('R', new UserMode(UMODE_REGPRIV));
	ModeManager::AddUserMode('a', new UserMode(UMODE_ADMIN));
	ModeManager::AddUserMode('i', new UserMode(UMODE_INVIS));
	ModeManager::AddUserMode('o', new UserMode(UMODE_OPER));
	ModeManager::AddUserMode('r', new UserMode(UMODE_REGISTERED));
	ModeManager::AddUserMode('s', new UserMode(UMODE_SNOMASK));
	ModeManager::AddUserMode('w', new UserMode(UMODE_WALLOPS));
	ModeManager::AddUserMode('d', new UserMode(UMODE_DEAF));

	/* b/e/I */
	ModeManager::AddChannelMode('b', new ChannelModeBan());

	/* v/h/o/a/q */
	ModeManager::AddChannelMode('v', new ChannelModeStatus(CMODE_VOICE, CUS_VOICE, '+'));
	ModeManager::AddChannelMode('o', new ChannelModeStatus(CMODE_OP, CUS_OP, '@', true));

	/* Add channel modes */
	ModeManager::AddChannelMode('c', new ChannelMode(CMODE_BLOCKCOLOR));
	ModeManager::AddChannelMode('i', new ChannelMode(CMODE_INVITE));
	ModeManager::AddChannelMode('j', new ChannelModeFlood());
	ModeManager::AddChannelMode('k', new ChannelModeKey());
	ModeManager::AddChannelMode('l', new ChannelModeParam(CMODE_LIMIT));
	ModeManager::AddChannelMode('m', new ChannelMode(CMODE_MODERATED));
	ModeManager::AddChannelMode('n', new ChannelMode(CMODE_NOEXTERNAL));
	ModeManager::AddChannelMode('p', new ChannelMode(CMODE_PRIVATE));
	ModeManager::AddChannelMode('r', new ChannelModeRegistered());
	ModeManager::AddChannelMode('s', new ChannelMode(CMODE_SECRET));
	ModeManager::AddChannelMode('t', new ChannelMode(CMODE_TOPIC));
	ModeManager::AddChannelMode('M', new ChannelMode(CMODE_REGMODERATED));
	ModeManager::AddChannelMode('O', new ChannelModeOper());
	ModeManager::AddChannelMode('R', new ChannelMode(CMODE_REGISTEREDONLY));
}

class ProtoBahamut : public Module
{
 public:
	ProtoBahamut(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(PROTOCOL);

		pmodule_ircd_version("BahamutIRCd 1.4.*/1.8.*");
		pmodule_ircd_cap(myIrcdcap);
		pmodule_ircd_var(myIrcd);
		pmodule_ircd_useTSMode(0);

		moduleAddModes();

		pmodule_ircd_proto(&ircd_proto);
		moduleAddIRCDMsgs();

		ModuleManager::Attach(I_OnDelCore, this);
	}

	void OnDelCore(NickCore *nc)
	{
		char *c;

		if (nc->GetExt("authenticationtoken", c))
		{
			delete [] c;
			nc->Shrink("authenticationtoken");
		}
	}

};

MODULE_INIT(ProtoBahamut)
