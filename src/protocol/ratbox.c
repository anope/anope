/* Ratbox IRCD functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#include "services.h"
#include "pseudo.h"

IRCDVar myIrcd[] = {
	{"Ratbox 2.0+",			 /* ircd name */
	 "+oi",					 /* Modes used by pseudoclients */
	 2,						 /* Chan Max Symbols	 */
	 "+o",					  /* Channel Umode used by Botserv bots */
	 0,						 /* SVSNICK */
	 0,						 /* Vhost  */
	 1,						 /* Supports SGlines	 */
	 1,						 /* Supports SQlines	 */
	 0,						 /* Supports SZlines	 */
	 3,						 /* Number of server args */
	 1,						 /* Join 2 Set		   */
	 1,						 /* Join 2 Message	   */
	 0,						 /* TS Topic Forward	 */
	 0,						 /* TS Topci Backward	*/
	 1,						 /* Chan SQlines		 */
	 0,						 /* Quit on Kill		 */
	 0,						 /* SVSMODE unban		*/
	 0,						 /* Reverse			  */
	 0,						 /* vidents			  */
	 0,						 /* svshold			  */
	 0,						 /* time stamp on mode   */
	 0,						 /* NICKIP			   */
	 0,						 /* UMODE				*/
	 0,						 /* O:LINE			   */
	 0,						 /* VHOST ON NICK		*/
	 0,						 /* Change RealName	  */
	 0,						 /* On nick change check if they could be identified */
	 0,						 /* No Knock requires +i */
	 NULL,					  /* CAPAB Chan Modes			 */
	 0,						 /* We support TOKENS */
	 0,						 /* TIME STAMPS are BASE64 */
	 0,						 /* SJOIN ban char */
	 0,						 /* SJOIN except char */
	 0,						 /* SJOIN invite char */
	 0,						 /* Can remove User Channel Modes with SVSMODE */
	 0,						 /* Sglines are not enforced until user reconnects */
	 1,						 /* ts6 */
	 0,						 /* p10 */
	 NULL,					  /* character set */
	 0,						 /* CIDR channelbans */
	 "$$",					  /* TLD Prefix for Global */
	 false,					/* Auth for users is sent after the initial NICK/UID command */
	 }
	,
	{NULL}
};

IRCDCAPAB myIrcdcap[] = {
	{
	 0,						 /* NOQUIT	   */
	 0,						 /* TSMODE	   */
	 0,						 /* UNCONNECT	*/
	 0,						 /* NICKIP	   */
	 0,						 /* SJOIN		*/
	 CAPAB_ZIP,				 /* ZIP		  */
	 0,						 /* BURST		*/
	 CAPAB_TS5,				 /* TS5		  */
	 0,						 /* TS3		  */
	 0,						 /* DKEY		 */
	 0,						 /* PT4		  */
	 0,						 /* SCS		  */
	 CAPAB_QS,				  /* QS		   */
	 CAPAB_UID,				 /* UID		  */
	 CAPAB_KNOCK,			   /* KNOCK		*/
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
	 0,						 /* DOZIP		*/
	 0, 0, 0}
};


/*
 * SVINFO
 *	  parv[0] = sender prefix
 *	  parv[1] = TS_CURRENT for the server
 *	  parv[2] = TS_MIN for the server
 *	  parv[3] = server is standalone or connected to non-TS only
 *	  parv[4] = server's idea of UTC time
 */
void ratbox_cmd_svinfo()
{
	send_cmd(NULL, "SVINFO 6 3 0 :%ld", static_cast<long>(time(NULL)));
}

void ratbox_cmd_svsinfo()
{

}

void ratbox_cmd_tmode(const char *source, const char *dest, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE];
	*buf = '\0';

	if (fmt) {
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);
		va_end(args);
	}
	if (!*buf) {
		return;
	}

	send_cmd(NULL, "MODE %s %s", dest, buf);
}


/* CAPAB */
/*
  QS	 - Can handle quit storm removal
  EX	 - Can do channel +e exemptions
  CHW	- Can do channel wall @#
  LL	 - Can do lazy links
  IE	 - Can do invite exceptions
  EOB	- Can do EOB message
  KLN	- Can do KLINE message
  GLN	- Can do GLINE message
  HUB	- This server is a HUB
  UID	- Can do UIDs
  ZIP	- Can do ZIPlinks
  ENC	- Can do ENCrypted links
  KNOCK  -  supports KNOCK
  TBURST - supports TBURST
  PARA	 - supports invite broadcasting for +p
  ENCAP	 - ?
*/
void ratbox_cmd_capab()
{
	send_cmd(NULL,
			 "CAPAB :QS EX CHW IE KLN GLN KNOCK TB UNKLN CLUSTER ENCAP");
}

/* PASS */
void ratbox_cmd_pass(const char *pass)
{
	send_cmd(NULL, "PASS %s TS 6 :%s", pass, TS6SID);
}

class RatboxProto : public IRCDTS6Proto
{
	void SendGlobopsInternal(BotInfo *source, const char *buf)
	{
		if (source)
			send_cmd(source->uid, "OPERWALL :%s", buf);
		else
			send_cmd(TS6SID, "OPERWALL :%s", buf);
	}

	void SendSQLine(const std::string &mask, const std::string &reason)
	{
		if (mask.empty() || reason.empty())
			return;
		send_cmd(TS6SID, "RESV * %s :%s", mask.c_str(), reason.c_str());
	}

	void SendSGLineDel(SXLine *sx)
	{
		BotInfo *bi = findbot(Config.s_OperServ);
		send_cmd(bi ? bi->uid : Config.s_OperServ, "UNXLINE * %s", sx->mask);
	}

	void SendSGLine(SXLine *sx)
	{
		BotInfo *bi = findbot(Config.s_OperServ);
		send_cmd(bi ? bi->uid : Config.s_OperServ, "XLINE * %s 0 :%s", sx->mask, sx->reason);
	}

	void SendAkillDel(Akill *ak)
	{
		BotInfo *bi = findbot(Config.s_OperServ);
		send_cmd(bi ? bi->uid : Config.s_OperServ, "UNKLINE * %s %s", ak->user, ak->host);
	}

	void SendSQLineDel(const std::string &user)
	{
		if (user.empty())
			return;
		send_cmd(TS6SID, "UNRESV * %s", user.c_str());
	}

	void SendJoin(BotInfo *user, const char *channel, time_t chantime)
	{
		send_cmd(NULL, "SJOIN %ld %s + :%s", static_cast<long>(chantime), channel, user->uid.c_str());
	}

	void SendAkill(Akill *ak)
	{
		BotInfo *bi = findbot(Config.s_OperServ);
		send_cmd(bi ? bi->uid : Config.s_OperServ, "KLINE * %ld %s %s :%s", static_cast<long>(ak->expires - time(NULL)), ak->user, ak->host, ak->reason);
	}

	void SendSVSKillInternal(BotInfo *source, User *user, const char *buf)
	{
		send_cmd(source ? source->uid : TS6SID, "KILL %s :%s", user->GetUID().c_str(), buf);
	}

	void SendSVSMode(User *u, int ac, const char **av)
	{
		this->SendModeInternal(NULL, u, merge_args(ac, av));
	}

	/* SERVER name hop descript */
	void SendServer(Server *server)
	{
		send_cmd(NULL, "SERVER %s %d :%s", server->name, server->hops, server->desc);
	}

	void SendConnect()
	{
		ratbox_cmd_pass(uplink_server->password);
		ratbox_cmd_capab();
		/* Make myself known to myself in the serverlist */
		me_server = new_server(NULL, Config.ServerName, Config.ServerDesc, SERVER_ISME, TS6SID);
		SendServer(me_server);
		ratbox_cmd_svinfo();
	}

	void SendClientIntroduction(const std::string &nick, const std::string &user, const std::string &host, const std::string &real, const char *modes, const std::string &uid)
	{
		EnforceQlinedNick(nick, NULL);
		send_cmd(TS6SID, "UID %s 1 %ld %s %s %s 0 %s :%s", nick.c_str(), static_cast<long>(time(NULL)), modes, user.c_str(), host.c_str(), uid.c_str(), real.c_str());
		SendSQLine(nick, "Reserved for services");
	}

	void SendPartInternal(BotInfo *bi, const char *chan, const char *buf)
	{
		if (buf)
			send_cmd(bi->uid, "PART %s :%s", chan, buf);
		else
			send_cmd(bi->uid, "PART %s", chan);
	}

	void SendNumericInternal(const char *source, int numeric, const char *dest, const char *buf)
	{
		// This might need to be set in the call to SendNumeric instead of here, will review later -- CyberBotX
		send_cmd(TS6SID, "%03d %s %s", numeric, dest, buf);
	}

	void SendModeInternal(BotInfo *bi, Channel *dest, const char *buf)
	{
		if (bi)
		{
			send_cmd(bi->uid, "MODE %s %s", dest->name.c_str(), buf);
		}
		else send_cmd(TS6SID, "MODE %s %s", dest->name.c_str(), buf);
	}

	void SendModeInternal(BotInfo *bi, User *u, const char *buf)
	{
		if (!buf) return;
		send_cmd(bi ? bi->uid : TS6SID, "SVSMODE %s %s", u->nick.c_str(), buf);
	}

	void SendKickInternal(BotInfo *bi, Channel *chan, User *user, const char *buf)
	{
		if (buf) send_cmd(bi->uid, "KICK %s %s :%s", chan->name.c_str(), user->GetUID().c_str(), buf);
		else send_cmd(bi->uid, "KICK %s %s", chan->name.c_str(), user->GetUID().c_str());
	}

	void SendNoticeChanopsInternal(BotInfo *source, Channel *dest, const char *buf)
	{
		send_cmd(NULL, "NOTICE @%s :%s", dest->name.c_str(), buf);
	}

	/* QUIT */
	void SendQuitInternal(BotInfo *bi, const char *buf)
	{
		if (buf) send_cmd(bi->uid, "QUIT :%s", buf);
		else send_cmd(bi->uid, "QUIT");
	}

	/* INVITE */
	void SendInvite(BotInfo *source, const char *chan, const char *nick)
	{
		User *u = finduser(nick);
		send_cmd(source->uid, "INVITE %s %s", u ? u->GetUID().c_str(): nick, chan);
	}

	void SendAccountLogin(User *u, NickCore *account)
	{
		send_cmd(TS6SID, "ENCAP * SU %s %s", u->GetUID().c_str(), account->display);
	}

	void SendAccountLogout(User *u, NickCore *account)
	{
		send_cmd(TS6SID, "ENCAP * SU %s", u->GetUID().c_str());
	}

	int IsNickValid(const char *nick)
	{
		/* TS6 Save extension -Certus */
		if (isdigit(*nick)) return 0;
		return 1;
	}

	void SendTopic(BotInfo *bi, Channel *c, const char *whosetit, const char *topic)
	{
		send_cmd(bi->uid, "TOPIC %s :%s", c->name.c_str(), topic);
	}

	void SetAutoIdentificationToken(User *u)
	{
		char svidbuf[15];

		if (!u->nc)
			return;

		snprintf(svidbuf, sizeof(svidbuf), "%ld", static_cast<long>(u->timestamp));

		u->nc->Shrink("authenticationtoken");
		u->nc->Extend("authenticationtoken", new ExtensibleItemPointerArray<char>(svidbuf));
	}

} ircd_proto;




int anope_event_sjoin(const char *source, int ac, const char **av)
{
	do_sjoin(source, ac, av);
	return MOD_CONT;
}

/*
   Non TS6

   av[0] = nick
   av[1] = hop
   av[2] = ts
   av[3] = modes
   av[4] = user
   av[5] = host
   av[6] = server
   av[7] = info

   TS6
   av[0] = nick
   av[1] = hop
   av[2] = ts
   av[3] = modes
   av[4] = user
   av[5] = host
   av[6] = IP
   av[7] = UID
   av[8] = info

*/
int anope_event_nick(const char *source, int ac, const char **av)
{
	Server *s;
	User *user;

	if (ac == 9)
	{
		s = findserver_uid(servlist, source);
		/* Source is always the server */
		user = do_nick("", av[0], av[4], av[5], s->name, av[8],
					   strtoul(av[2], NULL, 10), 0, "*", av[7]);
		if (user)
		{
			/* No usermode +d on ratbox so we use
			 * nick timestamp to check for auth - Adam
			 */
			user->CheckAuthenticationToken(av[2]);

			UserSetInternalModes(user, 1, &av[3]);
		}
	} else {
		if (ac == 2)
		{
			do_nick(source, av[0], NULL, NULL, NULL, NULL,
					strtoul(av[1], NULL, 10), 0, NULL, NULL);
		}
	}
	return MOD_CONT;
}

int anope_event_topic(const char *source, int ac, const char **av)
{
	User *u;

	if (ac == 4) {
		do_topic(source, ac, av);
	} else {
		Channel *c = findchan(av[0]);
		time_t topic_time = time(NULL);

		if (!c) {
			if (debug) {
				alog("debug: TOPIC %s for nonexistent channel %s",
					 merge_args(ac - 1, av + 1), av[0]);
			}
			return MOD_CONT;
		}

		if (check_topiclock(c, topic_time))
			return MOD_CONT;

		if (c->topic) {
			delete [] c->topic;
			c->topic = NULL;
		}
		if (ac > 1 && *av[1])
			c->topic = sstrdup(av[1]);

		u = find_byuid(source);
		c->topic_setter = u ? u->nick : source;
		c->topic_time = topic_time;

		record_topic(av[0]);

		if (ac > 1 && *av[1]) {
			FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(c, av[1]));
		}
		else {
			FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(c, ""));
		}
	}
	return MOD_CONT;
}

int anope_event_tburst(const char *source, int ac, const char **av)
{
	char *setter;
	Channel *c;
	time_t topic_time;

	if (ac != 4) {
		return MOD_CONT;
	}

	setter = myStrGetToken(av[2], '!', 0);

	c = findchan(av[0]);
	topic_time = strtol(av[1], NULL, 10);

	if (!c) {
		if (debug) {
			alog("debug: TOPIC %s for nonexistent channel %s",
				 merge_args(ac - 1, av + 1), av[0]);
		}
		if (setter)
			delete [] setter;
		return MOD_CONT;
	}

	if (check_topiclock(c, topic_time)) {
		if (setter)
			delete [] setter;
		return MOD_CONT;
	}

	if (c->topic) {
		delete [] c->topic;
		c->topic = NULL;
	}
	if (ac > 1 && *av[3])
		c->topic = sstrdup(av[3]);

	c->topic_setter = setter;
	c->topic_time = topic_time;

	record_topic(av[0]);
	if (setter)
		delete [] setter;
	return MOD_CONT;
}

int anope_event_436(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;

	m_nickcoll(av[0]);
	return MOD_CONT;
}


int anope_event_ping(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	ircdproto->SendPong(ac > 1 ? av[1] : Config.ServerName, av[0]);
	return MOD_CONT;
}

int anope_event_away(const char *source, int ac, const char **av)
{
	User *u = NULL;

	u = find_byuid(source);
	m_away(u ? u->nick.c_str() : source, (ac ? av[0] : NULL));
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
	if (ac != 1) {
		do_sjoin(source, ac, av);
		return MOD_CONT;
	} else {
		do_join(source, ac, av);
	}
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

int anope_event_privmsg(const char *source, int ac, const char **av)
{
	User *u;
	BotInfo *bi;

	if (ac != 2) {
		return MOD_CONT;
	}

	u = find_byuid(source);
	bi = findbot(av[0]);
	// XXX: this is really the same as charybdis
	m_privmsg(source, av[0], av[1]);
	return MOD_CONT;
}

int anope_event_part(const char *source, int ac, const char **av)
{
	User *u;

	if (ac < 1 || ac > 2) {
		return MOD_CONT;
	}

	u = find_byuid(source);
	do_part(u ? u->nick.c_str() : source, ac, av);

	return MOD_CONT;
}

int anope_event_whois(const char *source, int ac, const char **av)
{
	BotInfo *bi;

	if (source && ac >= 1) {
		bi = findbot(av[0]);
		m_whois(source, bi->uid.c_str());
	}
	return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(const char *source, int ac, const char **av)
{
	if (!stricmp(av[1], "1")) {
		uplink = sstrdup(av[0]);
		if (TS6UPLINK) {
			do_server(source, av[0], av[1], av[2], TS6UPLINK);
		} else {
			do_server(source, av[0], av[1], av[2], NULL);
		}
	} else {
		do_server(source, av[0], av[1], av[2], NULL);
	}
	return MOD_CONT;
}

int anope_event_sid(const char *source, int ac, const char **av)
{
	Server *s;

	/* :42X SID trystan.nomadirc.net 2 43X :ircd-ratbox test server */

	s = findserver_uid(servlist, source);

	do_server(s->name, av[0], av[1], av[3], av[2]);
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
	User *u;

	if (ac != 1) {
		return MOD_CONT;
	}

	u = find_byuid(source);

	do_quit(u ? u->nick.c_str() : source, ac, av);
	return MOD_CONT;
}

int anope_event_mode(const char *source, int ac, const char **av)
{
	User *u, *u2;

	if (ac < 2) {
		return MOD_CONT;
	}

	if (*av[0] == '#' || *av[0] == '&') {
		do_cmode(source, ac, av);
	} else {
		u = find_byuid(source);
		u2 = find_byuid(av[0]);
		av[0] = u2->nick.c_str();
		do_umode(u->nick.c_str(), ac, av);
	}
	return MOD_CONT;
}

int anope_event_tmode(const char *source, int ac, const char **av)
{
	if (*av[1] == '#' || *av[1] == '&') {
		do_cmode(source, ac, av);
	}
	return MOD_CONT;
}

/* Event: PROTOCTL */
int anope_event_capab(const char *source, int ac, const char **av)
{
	int argvsize = 8;
	int argc;
	const char **argv;
	char *str;

	if (ac < 1)
		return MOD_CONT;

	/* We get the params as one arg, we should split it for capab_parse */
	argv = static_cast<const char **>(scalloc(argvsize, sizeof(const char *)));
	argc = 0;
	while ((str = myStrGetToken(av[0], ' ', argc))) {
		if (argc == argvsize) {
			argvsize += 8;
			argv = static_cast<const char **>(srealloc(argv, argvsize * sizeof(const char *)));
		}
		argv[argc] = str;
		argc++;
	}

	capab_parse(argc, argv);

	/* Free our built ac/av */
	for (argvsize = 0; argvsize < argc; argvsize++) {
		delete [] argv[argvsize];
	}
	free(const_cast<char **>(argv));

	return MOD_CONT;
}

int anope_event_pass(const char *source, int ac, const char **av)
{
	TS6UPLINK = sstrdup(av[3]);
	return MOD_CONT;
}

int anope_event_bmask(const char *source, int ac, const char **av)
{
	Channel *c;
	char *bans;
	char *b;
	int count, i;
	ChannelModeList *cms;

	/* :42X BMASK 1106409026 #ircops b :*!*@*.aol.com */
	/*			 0		  1	  2   3			*/
	c = findchan(av[1]);

	if (c) {
		bans = sstrdup(av[3]);
		count = myNumToken(bans, ' ');
		for (i = 0; i <= count - 1; i++) {
			b = myStrGetToken(bans, ' ', i);
			if (!stricmp(av[2], "b")) {
				cms = dynamic_cast<ChannelModeList *>(ModeManager::FindChannelModeByChar('b'));
				cms->AddMask(c, b);
			}
			if (!stricmp(av[2], "e")) {
				cms = dynamic_cast<ChannelModeList *>(ModeManager::FindChannelModeByChar('e'));
				cms->AddMask(c, b);
			}
			if (!stricmp(av[2], "I")) {
				cms = dynamic_cast<ChannelModeList *>(ModeManager::FindChannelModeByChar('I'));
				cms->AddMask(c, b);
			}
			if (b)
				delete [] b;
		}
		delete [] bans;
	}
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

void moduleAddIRCDMsgs()
{
	Message *m;

	m = createMessage("436",	   anope_event_436); addCoreMessage(IRCD,m);
	m = createMessage("AWAY",	  anope_event_away); addCoreMessage(IRCD,m);
	m = createMessage("JOIN",	  anope_event_join); addCoreMessage(IRCD,m);
	m = createMessage("KICK",	  anope_event_kick); addCoreMessage(IRCD,m);
	m = createMessage("KILL",	  anope_event_kill); addCoreMessage(IRCD,m);
	m = createMessage("MODE",	  anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("TMODE",	 anope_event_tmode); addCoreMessage(IRCD,m);
	m = createMessage("MOTD",	  anope_event_motd); addCoreMessage(IRCD,m);
	m = createMessage("NICK",	  anope_event_nick); addCoreMessage(IRCD,m);
	m = createMessage("BMASK",	 anope_event_bmask); addCoreMessage(IRCD,m);
	m = createMessage("UID",	   anope_event_nick); addCoreMessage(IRCD,m);
	m = createMessage("PART",	  anope_event_part); addCoreMessage(IRCD,m);
	m = createMessage("PASS",	  anope_event_pass); addCoreMessage(IRCD,m);
	m = createMessage("PING",	  anope_event_ping); addCoreMessage(IRCD,m);
	m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
	m = createMessage("QUIT",	  anope_event_quit); addCoreMessage(IRCD,m);
	m = createMessage("SERVER",	anope_event_server); addCoreMessage(IRCD,m);
	m = createMessage("SQUIT",	 anope_event_squit); addCoreMessage(IRCD,m);
	m = createMessage("TOPIC",	 anope_event_topic); addCoreMessage(IRCD,m);
	m = createMessage("TB",		anope_event_tburst); addCoreMessage(IRCD,m);
	m = createMessage("WHOIS",	 anope_event_whois); addCoreMessage(IRCD,m);
	m = createMessage("CAPAB",	 anope_event_capab); addCoreMessage(IRCD,m);
	m = createMessage("SJOIN",	 anope_event_sjoin); addCoreMessage(IRCD,m);
	m = createMessage("ERROR",	 anope_event_error); addCoreMessage(IRCD,m);
	m = createMessage("SID",	   anope_event_sid); addCoreMessage(IRCD,m);
}

void moduleAddModes()
{
	/* Add user modes */
	ModeManager::AddUserMode('a', new UserMode(UMODE_ADMIN));
	ModeManager::AddUserMode('i', new UserMode(UMODE_INVIS));
	ModeManager::AddUserMode('o', new UserMode(UMODE_OPER));
	ModeManager::AddUserMode('r', new UserMode(UMODE_REGISTERED));
	ModeManager::AddUserMode('s', new UserMode(UMODE_SNOMASK));
	ModeManager::AddUserMode('w', new UserMode(UMODE_WALLOPS));

	/* b/e/I */
	ModeManager::AddChannelMode('b', new ChannelModeBan());
	ModeManager::AddChannelMode('e', new ChannelModeExcept());
	ModeManager::AddChannelMode('I', new ChannelModeInvite());

	/* v/h/o/a/q */
	ModeManager::AddChannelMode('v', new ChannelModeStatus(CMODE_VOICE, CUS_VOICE, '+'));
	ModeManager::AddChannelMode('o', new ChannelModeStatus(CMODE_OP, CUS_OP, '@', true));

	/* Add channel modes */
	ModeManager::AddChannelMode('i', new ChannelMode(CMODE_INVITE));
	ModeManager::AddChannelMode('k', new ChannelModeKey());
	ModeManager::AddChannelMode('l', new ChannelModeParam(CMODE_LIMIT));
	ModeManager::AddChannelMode('m', new ChannelMode(CMODE_MODERATED));
	ModeManager::AddChannelMode('n', new ChannelMode(CMODE_NOEXTERNAL));
	ModeManager::AddChannelMode('p', new ChannelMode(CMODE_PRIVATE));
	ModeManager::AddChannelMode('s', new ChannelMode(CMODE_SECRET));
	ModeManager::AddChannelMode('t', new ChannelMode(CMODE_TOPIC));
}

class ProtoRatbox : public Module
{
 public:
	ProtoRatbox(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(PROTOCOL);

		if (Config.Numeric)
			TS6SID = sstrdup(Config.Numeric);
		UseTSMODE = 1;  /* TMODE */

		pmodule_ircd_version("Ratbox IRCD 2.0+");
		pmodule_ircd_cap(myIrcdcap);
		pmodule_ircd_var(myIrcd);
		pmodule_ircd_useTSMode(1);

		moduleAddModes();

		pmodule_ircd_proto(&ircd_proto);
		moduleAddIRCDMsgs();
	}

	~ProtoRatbox()
	{
		delete [] TS6SID;
	}

};

MODULE_INIT(ProtoRatbox)
