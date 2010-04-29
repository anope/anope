/* Ratbox IRCD functions
 *
 * (C) 2003-2010 Anope Team
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

static char *TS6UPLINK = NULL; // XXX is this needed?

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
	 0,						 /* No Knock requires +i */
	 0,						 /* We support TOKENS */
	 0,						 /* TIME STAMPS are BASE64 */
	 0,						 /* Can remove User Channel Modes with SVSMODE */
	 0,						 /* Sglines are not enforced until user reconnects */
	 1,						 /* ts6 */
	 0,						 /* p10 */
	 0,						 /* CIDR channelbans */
	 "$$",					  /* TLD Prefix for Global */
	 4,					/* Max number of modes we can send per line */
	 }
	,
	{NULL}
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
		send_cmd(NULL, "SERVER %s %d :%s", server->GetName().c_str(), server->GetHops(), server->GetDescription().c_str());
	}

	void SendConnect()
	{
		Me = new Server(NULL, Config.ServerName, 0, Config.ServerDesc, TS6SID);
		ratbox_cmd_pass(uplink_server->password);
		ratbox_cmd_capab();
		/* Make myself known to myself in the serverlist */
		SendServer(Me);
		ratbox_cmd_svinfo();
	}

	void SendClientIntroduction(const std::string &nick, const std::string &user, const std::string &host, const std::string &real, const char *modes, const std::string &uid)
	{
		EnforceQlinedNick(nick, NULL);
		send_cmd(TS6SID, "UID %s 1 %ld %s %s %s 0 %s :%s", nick.c_str(), static_cast<long>(time(NULL)), modes, user.c_str(), host.c_str(), uid.c_str(), real.c_str());
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

		if (!u->Account())
			return;

		snprintf(svidbuf, sizeof(svidbuf), "%ld", static_cast<long>(u->timestamp));

		u->Account()->Shrink("authenticationtoken");
		u->Account()->Extend("authenticationtoken", new ExtensibleItemPointerArray<char>(sstrdup(svidbuf)));
	}

} ircd_proto;




int anope_event_sjoin(const char *source, int ac, const char **av)
{
	Channel *c = findchan(av[1]);
	time_t ts = atol(av[0]);
	bool was_created = false;
	bool keep_their_modes = true;

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

		User *u = find_byuid(buf);
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
	User *user;

	if (ac == 9)
	{
		Server *s = Server::Find(source ? source : "");
		/* Source is always the server */
		user = do_nick("", av[0], av[4], av[5], s->GetName().c_str(), av[8],
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
			Alog(LOG_DEBUG) << "TOPIC " << merge_args(ac - 1, av + 1) << " for nonexistent channel " << av[0];
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

	if (!c) 
	{
		Alog(LOG_DEBUG) << "debug: TOPIC " << merge_args(ac - 1, av + 1) << " for nonexistent channel " << av[0];
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
		anope_event_sjoin(source, ac, av);
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
		if (TS6UPLINK) {
			do_server(source, av[0], atoi(av[1]), av[2], TS6UPLINK);
		} else {
			do_server(source, av[0], atoi(av[1]), av[2], "");
		}
	} else {
		do_server(source, av[0], atoi(av[1]), av[2], "");
	}
	return MOD_CONT;
}

int anope_event_sid(const char *source, int ac, const char **av)
{
	/* :42X SID trystan.nomadirc.net 2 43X :ircd-ratbox test server */

	Server *s = Server::Find(source);

	do_server(s->GetName(), av[0], atoi(av[1]), av[3], av[2]);
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
	CapabParse(ac, av);
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
	if (ac >= 1)
		Alog(LOG_DEBUG) << av[0];
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
	ModeManager::AddUserMode(new UserMode(UMODE_ADMIN, 'a'));
	ModeManager::AddUserMode(new UserMode(UMODE_INVIS, 'i'));
	ModeManager::AddUserMode(new UserMode(UMODE_OPER, 'o'));
	ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, 's'));
	ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, 'w'));

	/* b/e/I */
	ModeManager::AddChannelMode(new ChannelModeBan('b'));
	ModeManager::AddChannelMode(new ChannelModeExcept('e'));
	ModeManager::AddChannelMode(new ChannelModeInvite('I'));

	/* v/h/o/a/q */
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, 'v', '+'));
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, 'o', '@'));

	/* Add channel modes */
	ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, 'i'));
	ModeManager::AddChannelMode(new ChannelModeKey('k'));
	ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, 'l'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, 'm'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, 'n'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, 'p'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, 's'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, 't'));
}

class ProtoRatbox : public Module
{
 public:
	ProtoRatbox(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(PROTOCOL);

		if (Config.Numeric)
			TS6SID = sstrdup(Config.Numeric);
		UseTSMODE = 1;  /* TMODE */

		pmodule_ircd_version("Ratbox IRCD 2.0+");
		pmodule_ircd_var(myIrcd);
		pmodule_ircd_useTSMode(1);

		CapabType c[] = { CAPAB_ZIP, CAPAB_TS5, CAPAB_QS, CAPAB_UID, CAPAB_KNOCK };
		for (unsigned i = 0; i < 5; ++i)
			Capab.SetFlag(c[i]);

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
