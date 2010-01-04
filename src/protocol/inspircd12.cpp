/* inspircd 1.2 functions
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
#include "hashcomp.h"

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef _WIN32
#include <winsock.h>
int inet_aton(const char *name, struct in_addr *addr)
{
	uint32 a = inet_addr(name);
	addr->s_addr = a;
	return a != (uint32) - 1;
}
#endif

IRCDVar myIrcd[] = {
	{"InspIRCd 1.2",			/* ircd name */
	 "+I",					  /* Modes used by pseudoclients */
	 5,						 /* Chan Max Symbols	 */
	 "+ao",					 /* Channel Umode used by Botserv bots */
	 1,						 /* SVSNICK */
	 1,						 /* Vhost  */
	 0,						 /* Supports SGlines	 */
	 1,						 /* Supports SQlines	 */
	 1,						 /* Supports SZlines	 */
	 4,						 /* Number of server args */
	 0,						 /* Join 2 Set		   */
	 0,						 /* Join 2 Message	   */
	 1,						 /* TS Topic Forward	 */
	 0,						 /* TS Topci Backward	*/
	 0,						 /* Chan SQlines		 */
	 0,						 /* Quit on Kill		 */
	 0,						 /* SVSMODE unban		*/
	 1,						 /* Reverse			  */
	 1,						 /* vidents			  */
	 1,						 /* svshold			  */
	 0,						 /* time stamp on mode   */
	 0,						 /* NICKIP			   */
	 0,						 /* O:LINE			   */
	 1,						 /* UMODE			   */
	 1,						 /* VHOST ON NICK		*/
	 0,						 /* Change RealName	  */
	 0,
	 1,						 /* No Knock requires +i */
	 NULL,					  /* CAPAB Chan Modes			 */
	 0,						 /* We support inspircd TOKENS */
	 0,						 /* TIME STAMPS are BASE64 */
	 0,						 /* SJOIN ban char */
	 0,						 /* SJOIN except char */
	 0,						 /* SJOIN invite char */
	 0,						 /* Can remove User Channel Modes with SVSMODE */
	 0,						 /* Sglines are not enforced until user reconnects */
	 1,						 /* ts6 */
	 0,						 /* p10 */
	 NULL,					  /* character set */
	 1,						 /* CIDR channelbans */
	 "$",					   /* TLD Prefix for Global */
	 true,					/* Auth for users is sent after the initial NICK/UID command */
	 }
	,
	{NULL}
};


IRCDCAPAB myIrcdcap[] = {
	{
	 CAPAB_NOQUIT,			  /* NOQUIT	   */
	 0,						 /* TSMODE	   */
	 1,						 /* UNCONNECT	*/
	 0,						 /* NICKIP	   */
	 0,						 /* SJOIN		*/
	 0,						 /* ZIP		  */
	 0,						 /* BURST		*/
	 0,						 /* TS5		  */
	 0,						 /* TS3		  */
	 0,						 /* DKEY		 */
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
	 CAPAB_SSJ3,				/* SSJ3		 */
	 CAPAB_NICK2,			   /* NICK2		*/
	 CAPAB_VL,				  /* VL		   */
	 CAPAB_TLKEXT,			  /* TLKEXT	   */
	 0,						 /* DODKEY	   */
	 0,						 /* DOZIP		*/
	 0,
	 0, 0}
};

static int has_servicesmod = 0;
static int has_globopsmod = 0;
static int has_svsholdmod = 0;
static int has_chghostmod = 0;
static int has_chgidentmod = 0;
static int has_messagefloodmod = 0;
static int has_banexceptionmod = 0;
static int has_inviteexceptionmod = 0;
static int has_hidechansmod = 0;

/* Previously introduced user during burst */
static User *prev_u_intro;


/* CHGHOST */
void inspircd_cmd_chghost(const char *nick, const char *vhost)
{
	if (has_chghostmod != 1)
	{
		ircdproto->SendGlobops(findbot(Config.s_OperServ), "CHGHOST not loaded!");
		return;
	}

	BotInfo *bi = findbot(Config.s_OperServ);
	send_cmd(bi->uid, "CHGHOST %s %s", nick, vhost);
}

int anope_event_idle(const char *source, int ac, const char **av)
{
	BotInfo *bi = findbot(av[0]);
	if (!bi)
		return MOD_CONT;

	send_cmd(bi->uid, "IDLE %s %ld %ld", source, static_cast<long>(start_time), static_cast<long>(time(NULL) - bi->lastmsg));
	return MOD_CONT;
}

static char currentpass[1024];

/* PASS */
void inspircd_cmd_pass(const char *pass)
{
	strlcpy(currentpass, pass, sizeof(currentpass));
}


class InspIRCdProto : public IRCDProto
{
	void SendAkillDel(Akill *ak)
	{
		BotInfo *bi = findbot(Config.s_OperServ);
		send_cmd(bi->uid, "GLINE %s@%s", ak->user, ak->host);
	}

	void SendTopic(BotInfo *whosets, Channel *c, const char *whosetit, const char *topic)
	{
		send_cmd(whosets->uid, "FTOPIC %s %lu %s :%s", c->name.c_str(), static_cast<unsigned long>(c->topic_time), whosetit, topic);
	}

	void SendVhostDel(User *u)
	{
		if (u->HasMode(UMODE_CLOAK))
			inspircd_cmd_chghost(u->nick.c_str(), u->chost.c_str());
		else
			inspircd_cmd_chghost(u->nick.c_str(), u->host);

		if (has_chgidentmod && u->GetIdent() != u->GetVIdent())
		{
			inspircd_cmd_chgident(u->nick.c_str(), u->GetIdent().c_str());
		}
	}

	void SendAkill(Akill *ak)
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = ak->expires - time(NULL);
		if (timeleft > 172800 || !ak->expires)
			timeleft = 172800;
		BotInfo *bi = findbot(Config.s_OperServ);
		send_cmd(bi->uid, "ADDLINE G %s@%s %s %ld %ld :%s", ak->user, ak->host, ak->by, static_cast<long>(time(NULL)), static_cast<long>(timeleft), ak->reason);
	}

	void SendSVSKillInternal(BotInfo *source, User *user, const char *buf)
	{
		send_cmd(source ? source->uid : TS6SID, "KILL %s :%s", user->GetUID().c_str(), buf);
	}

	void SendSVSMode(User *u, int ac, const char **av)
	{
		this->SendModeInternal(NULL, u, merge_args(ac, av));
	}

	void SendNumericInternal(const char *source, int numeric, const char *dest, const char *buf)
	{
		send_cmd(TS6SID, "PUSH %s ::%s %03d %s %s", dest, source, numeric, dest, buf);
	}

	void SendGuestNick(const char *nick, const char *user, const char *host, const char *real, const char *modes)
	{
		send_cmd(TS6SID, "UID %ld %s %s %s %s +%s 0.0.0.0 :%s", static_cast<long>(time(NULL)), nick, host, host, user, modes, real);
	}

	void SendModeInternal(BotInfo *source, Channel *dest, const char *buf)
	{
		send_cmd(source ? source->uid : TS6SID, "FMODE %s %u %s", dest->name.c_str(), static_cast<unsigned>(dest->creation_time), buf);
	}

	void SendModeInternal(BotInfo *bi, User *u, const char *buf)
	{
		if (!buf) return;
		send_cmd(bi ? bi->uid : TS6SID, "MODE %s %s", u->GetUID().c_str(), buf);
	}

	void SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes, const char *uid)
	{
		send_cmd(TS6SID, "UID %s %ld %s %s %s %s 0.0.0.0 %ld %s :%s", uid, static_cast<long>(time(NULL)), nick, host, host, user, static_cast<long>(time(NULL)), modes, real);
	}

	void SendKickInternal(BotInfo *source, Channel *chan, User *user, const char *buf)
	{
		if (buf)
			send_cmd(source->uid, "KICK %s %s :%s", chan->name.c_str(), user->GetUID().c_str(), buf);
		else
			send_cmd(source->uid, "KICK %s %s :%s", chan->name.c_str(), user->GetUID().c_str(), user->nick.c_str());
	}

	void SendNoticeChanopsInternal(BotInfo *source, Channel *dest, const char *buf)
	{
		send_cmd(TS6SID, "NOTICE @%s :%s", dest->name.c_str(), buf);
	}

	/* SERVER services-dev.chatspike.net password 0 :Description here */
	void SendServer(Server *server)
	{
		send_cmd(NULL, "SERVER %s %s %d %s :%s", server->name, currentpass, server->hops, server->suid, server->desc);
	}

	/* JOIN */
	void SendJoin(BotInfo *user, const char *channel, time_t chantime)
	{
		send_cmd(NULL, "FJOIN %s %ld + :,%s", channel, static_cast<long>(chantime), user->uid.c_str());
	}

	/* UNSQLINE */
	void SendSQLineDel(const char *user)
	{
		send_cmd(TS6SID, "DELLINE Q %s", user);
	}

	/* SQLINE */
	void SendSQLine(const std::string &mask, const std::string &reason)
	{
		if (mask.empty() || reason.empty())
			return;
		send_cmd(TS6SID, "ADDLINE Q %s %s %ld 0 :%s", mask.c_str(), Config.s_OperServ, static_cast<long>(time(NULL)), reason.c_str());
	}

	/* SQUIT */
	void SendSquit(const char *servname, const char *message)
	{
		send_cmd(TS6SID, "SQUIT %s :%s", servname, message);
	}

	/* Functions that use serval cmd functions */

	void SendVhost(User *u, const char *vIdent, const char *vhost)
	{
		if (vIdent)
			inspircd_cmd_chgident(u->nick.c_str(), vIdent);
		inspircd_cmd_chghost(u->nick.c_str(), vhost);
	}

	void SendConnect()
	{
		inspircd_cmd_pass(uplink_server->password);
		me_server = new_server(NULL, Config.ServerName, Config.ServerDesc, SERVER_ISME, TS6SID);
		SendServer(me_server);
		send_cmd(TS6SID, "BURST");
		send_cmd(TS6SID, "VERSION :Anope-%s %s :%s - %s (%s) -- %s", version_number, Config.ServerName, ircd->name, version_flags, Config.EncModuleList.begin()->c_str(), version_build);
	}

	/* CHGIDENT */
	void inspircd_cmd_chgident(const char *nick, const char *vIdent)
	{
		if (has_chgidentmod == 0)
		{
			ircdproto->SendGlobops(findbot(Config.s_OperServ), "CHGIDENT not loaded!");
		}
		else
		{
			BotInfo *bi = findbot(Config.s_OperServ);
			send_cmd(bi->uid, "CHGIDENT %s %s", nick, vIdent);
		}
	}

	/* SVSHOLD - set */
	void SendSVSHold(const char *nick)
	{
		BotInfo *bi = findbot(Config.s_OperServ);
		send_cmd(bi->uid, "SVSHOLD %s %u :%s", nick, static_cast<unsigned>(Config.NSReleaseTimeout), "Being held for registered user");
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const char *nick)
	{
		BotInfo *bi = findbot(Config.s_OperServ);
		send_cmd(bi->uid, "SVSHOLD %s", nick);
	}

	/* UNSZLINE */
	void SendSZLineDel(SXLine *sx)
	{
		send_cmd(TS6SID, "DELLINE Z %s", sx->mask);
	}

	/* SZLINE */
	void SendSZLine(SXLine *sx)
	{
		send_cmd(TS6SID, "ADDLINE Z %s %s %ld 0 :%s", sx->mask, sx->by, static_cast<long>(time(NULL)), sx->reason);
	}

	/* SVSMODE -r */
	void SendUnregisteredNick(User *u)
	{
		u->RemoveMode(findbot(Config.s_NickServ), UMODE_REGISTERED);
	}

	void SendSVSJoin(const char *source, const char *nick, const char *chan, const char *param)
	{
		User *u = finduser(nick);
		BotInfo *bi = findbot(source);
		send_cmd(bi->uid, "SVSJOIN %s %s", u->GetUID().c_str(), chan);
	}

	void SendSVSPart(const char *source, const char *nick, const char *chan)
	{
		User *u = finduser(nick);
		BotInfo *bi = findbot(source);
		send_cmd(bi->uid, "SVSPART %s %s", u->GetUID().c_str(), chan);
	}

	void SendSWhois(const char *source, const char *who, const char *mask)
	{
		User *u = finduser(who);

		send_cmd(TS6SID, "METADATA %s swhois :%s", u->GetUID().c_str(), mask);
	}

	void SendEOB()
	{
		send_cmd(TS6SID, "ENDBURST");
	}

	void SendGlobopsInternal(BotInfo *source, const char *buf)
	{
		if (has_globopsmod)
			send_cmd(source ? source->uid : TS6SID, "SNONOTICE g :%s", buf);
		else
			send_cmd(source ? source->uid : TS6SID, "SNONOTICE A :%s", buf);
	}

	void SendAccountLogin(User *u, NickCore *account)
	{
		send_cmd(TS6SID, "METADATA %s accountname :%s", u->GetUID().c_str(), account->display);
	}

	void SendAccountLogout(User *u, NickCore *account)
	{
		send_cmd(TS6SID, "METADATA %s accountname :", u->GetUID().c_str());
	}

	int IsNickValid(const char *nick)
	{
		/* InspIRCd, like TS6, uses UIDs on collision, so... */
		if (isdigit(*nick))
			return 0;
		return 1;
	}

	void SetAutoIdentificationToken(User *u)
	{
		if (!u->nc)
			return;

		u->SetMode(findbot(Config.s_NickServ), UMODE_REGISTERED);
	}

} ircd_proto;






int anope_event_ftopic(const char *source, int ac, const char **av)
{
	/* :source FTOPIC channel ts setby :topic */
	const char *temp;
	if (ac < 4)
		return MOD_CONT;
	temp = av[1];			   /* temp now holds ts */
	av[1] = av[2];			  /* av[1] now holds set by */
	av[2] = temp;			   /* av[2] now holds ts */
	do_topic(source, ac, av);
	return MOD_CONT;
}

int anope_event_mode(const char *source, int ac, const char **av)
{
	if (*av[0] == '#' || *av[0] == '&')
	{
		do_cmode(source, ac, av);
	}
	else
	{
		/* InspIRCd lets opers change another
		   users modes, we have to kludge this
		   as it slightly breaks RFC1459
		 */
		User *u = find_byuid(source);
		User *u2 = find_byuid(av[0]);

		// This can happen with server-origin modes.
		if (u == NULL)
			u = u2;

		// if it's still null, drop it like fire.
		// most likely situation was that server introduced a nick which we subsequently akilled
		if (u == NULL)
			return MOD_CONT;

		av[0] = u2->nick.c_str();
		do_umode(u->nick.c_str(), ac, av);
	}
	return MOD_CONT;
}

int anope_event_opertype(const char *source, int ac, const char **av)
{
	/* opertype is equivalent to mode +o because servers
	   dont do this directly */
	User *u;
	u = finduser(source);
	if (u && !is_oper(u)) {
		const char *newav[2];
		newav[0] = source;
		newav[1] = "+o";
		return anope_event_mode(source, 2, newav);
	} else
		return MOD_CONT;
}

int anope_event_fmode(const char *source, int ac, const char **av)
{
	const char *newav[25];
	int n, o;
	Channel *c;

	/* :source FMODE #test 12345678 +nto foo */
	if (ac < 3)
		return MOD_CONT;

	/* Checking the TS for validity to avoid desyncs */
	if ((c = findchan(av[0]))) {
		if (c->creation_time > strtol(av[1], NULL, 10)) {
			/* Our TS is bigger, we should lower it */
			c->creation_time = strtol(av[1], NULL, 10);
		} else if (c->creation_time < strtol(av[1], NULL, 10)) {
			/* The TS we got is bigger, we should ignore this message. */
			return MOD_CONT;
		}
	} else {
		/* Got FMODE for a non-existing channel */
		return MOD_CONT;
	}

	/* TS's are equal now, so we can proceed with parsing */
	n = o = 0;
	while (n < ac) {
		if (n != 1) {
			newav[o] = av[n];
			o++;
			if (debug)
				alog("Param: %s", newav[o - 1]);
		}
		n++;
	}

	return anope_event_mode(source, ac - 1, newav);
}

/*
 * [Nov 03 22:31:57.695076 2009] debug: Received: :964 FJOIN #test 1223763723 +BPSnt :,964AAAAAB ,964AAAAAC ,966AAAAAA
 *
 * 0: name
 * 1: channel ts (when it was created, see protocol docs for more info)
 * 2: channel modes + params (NOTEL this may definitely be more than one param!)
 * last: users
 */
int anope_event_fjoin(const char *source, int ac, const char **av)
{
	const char *newav[30]; // hopefully 30 will do until the stupid ac/av stuff goes away.

	/* storing the current nick */
	char *curnick;

	/* these are used to generate the final string that is passed to ircservices' core */
	int nlen = 0;
	char nicklist[514];

	/* temporary buffer */
	char prefixandnick[60];

	*nicklist = '\0';
	*prefixandnick = '\0';

	if (ac <= 3)
		return MOD_CONT;

	spacesepstream nicks(av[ac - 1]);
	std::string nick;

	while (nicks.GetToken(nick))
	{
		curnick = sstrdup(nick.c_str());
		char *curnick_real = curnick;
		for (; *curnick; curnick++)
		{
			/* XXX: bleagh! -- w00t */
			switch (*curnick)
			{
				case 'q':
					prefixandnick[nlen++] = '~';
					break;
				case 'a':
					prefixandnick[nlen++] = '&';
					break;
				case 'o':
					prefixandnick[nlen++] = '@';
					break;
				case 'h':
					prefixandnick[nlen++] = '%';
					break;
				case 'v':
					prefixandnick[nlen++] = '+';
					break;
				case ',':
					curnick++;
					strncpy(prefixandnick + nlen, curnick, sizeof(prefixandnick) - nlen);
					goto endnick;
					break;
				default:
					alog("fjoin: unrecognised prefix: %c", *curnick);
					break;
			}
		}

// Much as I hate goto.. I can't `break 2' to get here.. XXX ugly
endnick:
		strlcat(nicklist, prefixandnick, sizeof(nicklist));
		strlcat(nicklist, " ", sizeof(nicklist));
		delete [] curnick_real;
		nlen = 0;
	}

	newav[0] = av[1];		   /* timestamp */
	newav[1] = av[0];		   /* channel name */

	int i;

	/* We want to replace the last string with our newly formatted user string */
	for (i = 2; i != ac - 1; i++)
	{
		newav[i] = av[i];
	}

	newav[i] = nicklist;
	i++;

	do_sjoin(source, i, newav);

	return MOD_CONT;
}

/* Events */
int anope_event_ping(const char *source, int ac, const char **av)
{
	if (ac == 1)
		ircdproto->SendPong(NULL, av[0]);

	if (ac == 2)
		ircdproto->SendPong(av[1], av[0]);

	return MOD_CONT;
}

int anope_event_time(const char *source, int ac, const char **av)
{
	if (ac !=2)
		return MOD_CONT;

	send_cmd(TS6SID, "TIME %s %s %ld", source, av[1], static_cast<long>(time(NULL)));

	/* We handled it, don't pass it on to the core..
	 * The core doesn't understand our syntax anyways.. ~ Viper */
	return MOD_STOP;
}

int anope_event_436(const char *source, int ac, const char **av)
{
	m_nickcoll(av[0]);
	return MOD_CONT;
}

int anope_event_away(const char *source, int ac, const char **av)
{
	m_away(source, (ac ? av[0] : NULL));
	return MOD_CONT;
}

/* Taken from hybrid.c, topic syntax is identical */
int anope_event_topic(const char *source, int ac, const char **av)
{
	Channel *c = findchan(av[0]);
	time_t topic_time = time(NULL);
	User *u = find_byuid(source);

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

	c->topic_setter = u ? u->nick : source;
	c->topic_time = topic_time;

	record_topic(av[0]);

	if (ac > 1 && *av[1]) {
		FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(c, av[0]));
	}
	else {
		FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(c, ""));
	}

	return MOD_CONT;
}

int anope_event_squit(const char *source, int ac, const char **av)
{
	do_squit(source, ac, av);
	return MOD_CONT;
}

int anope_event_rsquit(const char *source, int ac, const char **av)
{
	do_squit(source, ac, av);
	return MOD_CONT;
}

int anope_event_quit(const char *source, int ac, const char **av)
{
	do_quit(source, ac, av);
	return MOD_CONT;
}


int anope_event_kill(const char *source, int ac, const char **av)
{
	User *u = find_byuid(av[0]);
	BotInfo *bi = findbot(av[0]);
	m_kill(u ? u->nick.c_str() : (bi ? bi->nick : av[0]), av[1]);
	return MOD_CONT;
}

int anope_event_kick(const char *source, int ac, const char **av)
{
	do_kick(source, ac, av);
	return MOD_CONT;
}


int anope_event_join(const char *source, int ac, const char **av)
{
	do_join(source, ac, av);
	return MOD_CONT;
}

int anope_event_motd(const char *source, int ac, const char **av)
{
	m_motd(source);
	return MOD_CONT;
}

int anope_event_setname(const char *source, int ac, const char **av)
{
	User *u;

	u = finduser(source);
	if (!u) {
		if (debug) {
			alog("debug: SETNAME for nonexistent user %s", source);
		}
		return MOD_CONT;
	}

	u->SetRealname(av[0]);
	return MOD_CONT;
}

int anope_event_chgname(const char *source, int ac, const char **av)
{
	User *u;

	u = finduser(source);
	if (!u) {
		if (debug) {
			alog("debug: FNAME for nonexistent user %s", source);
		}
		return MOD_CONT;
	}

	u->SetRealname(av[0]);
	return MOD_CONT;
}

int anope_event_setident(const char *source, int ac, const char **av)
{
	User *u;

	u = finduser(source);
	if (!u) {
		if (debug) {
			alog("debug: SETIDENT for nonexistent user %s", source);
		}
		return MOD_CONT;
	}

	u->SetIdent(av[0]);
	return MOD_CONT;
}

int anope_event_chgident(const char *source, int ac, const char **av)
{
	User *u;

	u = finduser(av[0]);
	if (!u) {
		if (debug) {
			alog("debug: CHGIDENT for nonexistent user %s", av[0]);
		}
		return MOD_CONT;
	}

	u->SetIdent(av[1]);
	return MOD_CONT;
}

int anope_event_sethost(const char *source, int ac, const char **av)
{
	User *u;

	u = finduser(source);
	if (!u) {
		if (debug) {
			alog("debug: SETHOST for nonexistent user %s", source);
		}
		return MOD_CONT;
	}

	u->SetDisplayedHost(av[0]);
	return MOD_CONT;
}


int anope_event_nick(const char *source, int ac, const char **av)
{
	do_nick(source, av[0], NULL, NULL, NULL, NULL, 0, 0, NULL, NULL);
	return MOD_CONT;
}


/*
 * [Nov 03 22:09:58.176252 2009] debug: Received: :964 UID 964AAAAAC 1225746297 w00t2 localhost testnet.user w00t 127.0.0.1 1225746302 +iosw +ACGJKLNOQcdfgjklnoqtx :Robin Burchell <w00t@inspircd.org>
 * 0: uid
 * 1: ts
 * 2: nick
 * 3: host
 * 4: dhost
 * 5: ident
 * 6: ip
 * 7: signon
 * 8+: modes and params -- IMPORTANT, some modes (e.g. +s) may have parameters. So don't assume a fixed position of realname!
 * last: realname
*/

int anope_event_uid(const char *source, int ac, const char **av)
{
	User *user;
	NickAlias *na;
	struct in_addr addy;
	Server *s = findserver_uid(servlist, source);
	uint32 *ad = reinterpret_cast<uint32 *>(&addy);
	int ts = strtoul(av[1], NULL, 10);

	/* Check if the previously introduced user was Id'd for the nickgroup of the nick he s currently using.
	 * If not, validate the user.  ~ Viper*/
	user = prev_u_intro;
	prev_u_intro = NULL;
	if (user) na = findnick(user->nick);
	if (user && user->server->sync == SSYNC_IN_PROGRESS && (!na || na->nc != user->nc))
	{
		validate_user(user);
		if (user->HasMode(UMODE_REGISTERED))
			user->RemoveMode(findbot(Config.s_NickServ), UMODE_REGISTERED);
	}
	user = NULL;

	inet_aton(av[6], &addy);
	user = do_nick("", av[2],   /* nick */
			av[5],   /* username */
			av[3],   /* realhost */
			s->name,  /* server */
			av[ac - 1],   /* realname */
			ts, htonl(*ad), av[4], av[0]);
	if (user)
	{
		if (user->server->sync == SSYNC_IN_PROGRESS)
		{
			prev_u_intro = user;
		}
		UserSetInternalModes(user, 1, &av[8]);
		user->SetCloakedHost(av[4]);
	}

	return MOD_CONT;
}

int anope_event_chghost(const char *source, int ac, const char **av)
{
	User *u;

	u = finduser(source);
	if (!u) {
		if (debug) {
			alog("debug: FHOST for nonexistent user %s", source);
		}
		return MOD_CONT;
	}

	u->SetDisplayedHost(av[0]);
	return MOD_CONT;
}

/*
 * [Nov 04 00:08:46.308435 2009] debug: Received: SERVER irc.inspircd.com pass 0 964 :Testnet Central!
 * 0: name
 * 1: pass
 * 2: hops
 * 3: numeric
 * 4: desc
 */
int anope_event_server(const char *source, int ac, const char **av)
{
	if (!stricmp(av[2], "0"))
	{
		uplink = sstrdup(av[0]);
	}
	do_server(source, av[0], av[2], av[4], av[3]);
	return MOD_CONT;
}


int anope_event_privmsg(const char *source, int ac, const char **av)
{
	User *u = find_byuid(source);
	BotInfo *bi = findbot(av[0]);

	if (!u)
		return MOD_CONT; // likely a message from a server, which can happen.

	m_privmsg(u->nick.c_str(), bi ? bi->nick: av[0], av[1]);
	return MOD_CONT;
}

int anope_event_part(const char *source, int ac, const char **av)
{
	do_part(source, ac, av);
	return MOD_CONT;
}

int anope_event_whois(const char *source, int ac, const char **av)
{
	m_whois(source, av[0]);
	return MOD_CONT;
}

int anope_event_metadata(const char *source, int ac, const char **av)
{
	User *u;

	if (ac < 3)
		return MOD_CONT;
	else if (!strcmp(av[1], "accountname"))
	{
		if ((u = find_byuid(av[0])))
		{
			/* Identify the user for this account - Adam */
			u->AutoID(av[2]);
		}
	}

	return MOD_CONT;
}

int anope_event_capab(const char *source, int ac, const char **av)
{
	int argc;

	if (strcasecmp(av[0], "START") == 0) {
		/* reset CAPAB */
		has_servicesmod = 0;
		has_globopsmod = 0;
		has_svsholdmod = 0;
		has_chghostmod = 0;
		has_chgidentmod = 0;
		has_messagefloodmod = 0;
		has_banexceptionmod = 0;
		has_inviteexceptionmod = 0;
		has_hidechansmod = 0;

	} else if (strcasecmp(av[0], "MODULES") == 0) {
		if (strstr(av[1], "m_globops.so")) {
			has_globopsmod = 1;
		}
		if (strstr(av[1], "m_services_account.so")) {
			has_servicesmod = 1;
			ModeManager::AddChannelMode('r', new ChannelModeRegistered());
			ModeManager::AddChannelMode('R', new ChannelMode(CMODE_REGISTEREDONLY));
			ModeManager::AddChannelMode('M', new ChannelMode(CMODE_REGMODERATED));
			ModeManager::AddUserMode('r', new UserMode(UMODE_REGISTERED));
			ModeManager::AddUserMode('R', new UserMode(UMODE_REGPRIV));
		}
		if (strstr(av[1], "m_svshold.so")) {
			has_svsholdmod = 1;
		}
		if (strstr(av[1], "m_chghost.so")) {
			has_chghostmod = 1;
		}
		if (strstr(av[1], "m_chgident.so")) {
			has_chgidentmod = 1;
		}
		if (strstr(av[1], "m_messageflood.so")) {
			has_messagefloodmod = 1;
		}
		if (strstr(av[1], "m_banexception.so")) {
			has_banexceptionmod = 1;
		}
		if (strstr(av[1], "m_inviteexception.so")) {
			has_inviteexceptionmod = 1;
		}
		if (strstr(av[1], "m_hidechans.so")) {
			has_hidechansmod = 1;
			ModeManager::AddUserMode('I', new UserMode(UMODE_PRIV));
		}
		if (strstr(av[1], "m_servprotect.so")) {
			ircd->pseudoclient_mode = "+Ik";
			ModeManager::AddUserMode('k', new UserMode(UMODE_PROTECTED));
		}

		/* Add channel modes to Anope, but only if theyre supported by the IRCd */
		if (strstr(av[1], "m_blockcolor.so"))
			ModeManager::AddChannelMode('c', new ChannelMode(CMODE_BLOCKCOLOR));
		if (strstr(av[1], "m_auditorium.so"))
			ModeManager::AddChannelMode('u', new ChannelMode(CMODE_AUDITORIUM));
		if (strstr(av[1], "m_sslmodes.so"))
			ModeManager::AddChannelMode('z', new ChannelMode(CMODE_SSL));
		if (strstr(av[1], "m_allowinvite.so"))
			ModeManager::AddChannelMode('A', new ChannelMode(CMODE_ALLINVITE));
		if (strstr(av[1], "m_noctcp.so"))
			ModeManager::AddChannelMode('C', new ChannelMode(CMODE_NOCTCP));
		if (strstr(av[1], "m_censor.so"))
			ModeManager::AddChannelMode('G', new ChannelMode(CMODE_FILTER));
		if (strstr(av[1], "m_knock.so"))
			ModeManager::AddChannelMode('K', new ChannelMode(CMODE_NOKNOCK));
		if (strstr(av[1], "m_redirect.so"))
			ModeManager::AddChannelMode('L', new ChannelModeParam(CMODE_REDIRECT, true));
		if (strstr(av[1], "m_nonicks.so"))
			ModeManager::AddChannelMode('N', new ChannelMode(CMODE_NONICK));
		if (strstr(av[1], "m_operchans.so"))
			ModeManager::AddChannelMode('O', new ChannelModeOper());
		if (strstr(av[1], "m_nokicks.so"))
			ModeManager::AddChannelMode('Q', new ChannelMode(CMODE_NOKICK));
		if (strstr(av[1], "m_stripcolor.so"))
			ModeManager::AddChannelMode('S', new ChannelMode(CMODE_STRIPCOLOR));
		if (strstr(av[1], "m_callerid.so"))
			ModeManager::AddUserMode('g', new UserMode(UMODE_CALLERID));
		if (strstr(av[1], "m_helpop.so"))
			ModeManager::AddUserMode('h', new UserMode(UMODE_HELPOP));
		if (strstr(av[1], "m_cloaking.so"))
			ModeManager::AddUserMode('x', new UserMode(UMODE_CLOAK));
		if (strstr(av[1], "m_blockcaps.so"))
			ModeManager::AddChannelMode('B', new ChannelMode(CMODE_BLOCKCAPS));
		if (strstr(av[1], "m_nickflood.so"))
			ModeManager::AddChannelMode('F', new ChannelModeParam(CMODE_NICKFLOOD, true));
		if (strstr(av[1], "m_messageflood.so"))
			ModeManager::AddChannelMode('f', new ChannelModeParam(CMODE_FLOOD, true));
		if (strstr(av[1], "m_joinflood.so"))
			ModeManager::AddChannelMode('j', new ChannelModeParam(CMODE_JOINFLOOD, true));
		if (strstr(av[1], "m_permchannels.so"))
			ModeManager::AddChannelMode('P', new ChannelMode(CMODE_PERM));
		if (strstr(av[1], "m_nonotice.so"))
			ModeManager::AddChannelMode('T', new ChannelMode(CMODE_NONOTICE));

		/* User modes */
		if (strstr(av[1], "m_botmode.so"))
			ModeManager::AddUserMode('B', new UserMode(UMODE_BOT));
		if (strstr(av[1], "m_commonchans.so"))
			ModeManager::AddUserMode('c', new UserMode(UMODE_COMMONCHANS));
		if (strstr(av[1], "m_deaf.so"))
			ModeManager::AddUserMode('d', new UserMode(UMODE_DEAF));
		if (strstr(av[1], "m_censor.so"))
			ModeManager::AddUserMode('G', new UserMode(UMODE_FILTER));
		if (strstr(av[1], "m_hideoper.so"))
			ModeManager::AddUserMode('H', new UserMode(UMODE_HIDEOPER));
		if (strstr(av[1], "m_invisible.so"))
			ModeManager::AddUserMode('Q', new UserMode(UMODE_HIDDEN));
		if (strstr(av[1], "m_stripcolor.so"))
			ModeManager::AddUserMode('S', new UserMode(UMODE_STRIPCOLOR));
		if (strstr(av[1], "m_showwhois.so"))
			ModeManager::AddUserMode('W', new UserMode(UMODE_WHOIS));

	} else if (strcasecmp(av[0], "END") == 0) {
		if (!has_globopsmod) {
			send_cmd(NULL, "ERROR :m_globops is not loaded. This is required by Anope");
			quitmsg = "Remote server does not have the m_globops module loaded, and this is required.";
			quitting = 1;
			return MOD_STOP;
		}
		if (!has_servicesmod) {
			send_cmd(NULL, "ERROR :m_services_account.so is not loaded. This is required by Anope");
			quitmsg = "ERROR: Remote server does not have the m_services_account module loaded, and this is required.";
			quitting = 1;
			return MOD_STOP;
		}
		if (!has_hidechansmod) {
			send_cmd(NULL, "ERROR :m_hidechans.so is not loaded. This is required by Anope");
			quitmsg = "ERROR: Remote server does not have the m_hidechans module loaded, and this is required.";
			quitting = 1;
			return MOD_STOP;
		}
		if (!has_svsholdmod) {
			ircdproto->SendGlobops(findbot(Config.s_OperServ), "SVSHOLD missing, Usage disabled until module is loaded.");
		}
		if (!has_chghostmod) {
			ircdproto->SendGlobops(findbot(Config.s_OperServ), "CHGHOST missing, Usage disabled until module is loaded.");
		}
		if (!has_chgidentmod) {
			ircdproto->SendGlobops(findbot(Config.s_OperServ), "CHGIDENT missing, Usage disabled until module is loaded.");
		}
		if (has_messagefloodmod) {
			ModeManager::AddChannelMode('f', new ChannelModeFlood());
		}
		if (has_banexceptionmod) {
			ModeManager::AddChannelMode('e', new ChannelModeExcept());
		}
		if (has_inviteexceptionmod) {
			ModeManager::AddChannelMode('I', new ChannelModeInvite());
		}
		ircd->svshold = has_svsholdmod;

		/* Generate a fake capabs parsing call so things like NOQUIT work
		 * fine. It's ugly, but it works....
		 */
		argc = 6;
		const char *argv[] = {"NOQUIT", "SSJ3", "NICK2", "VL", "TLKEXT", "UNCONNECT"};

		capab_parse(argc, argv);
	}
	return MOD_CONT;
}

int anope_event_endburst(const char *source, int ac, const char **av)
{
	NickAlias *na;
	User *u = prev_u_intro;
	Server *s = findserver_uid(servlist, source);
	if (!s)
	{
		throw new CoreException("Got ENDBURST without a source");
	}

	/* Check if the previously introduced user was Id'd for the nickgroup of the nick he s currently using.
	 * If not, validate the user. ~ Viper*/
	prev_u_intro = NULL;
	if (u) na = findnick(u->nick);
	if (u && u->server->sync == SSYNC_IN_PROGRESS && (!na || na->nc != u->nc))
	{
		validate_user(u);
		if (u->HasMode(UMODE_REGISTERED))
			u->RemoveMode(findbot(Config.s_NickServ), UMODE_REGISTERED);
	}

	alog("Processed ENDBURST for %s", s->name);

	finish_sync(s, 1);
	return MOD_CONT;
}

void moduleAddIRCDMsgs() {
	Message *m;

	m = createMessage("ENDBURST",  anope_event_endburst); addCoreMessage(IRCD, m);
	m = createMessage("436",	   anope_event_436); addCoreMessage(IRCD,m);
	m = createMessage("AWAY",	  anope_event_away); addCoreMessage(IRCD,m);
	m = createMessage("JOIN",	  anope_event_join); addCoreMessage(IRCD,m);
	m = createMessage("KICK",	  anope_event_kick); addCoreMessage(IRCD,m);
	m = createMessage("KILL",	  anope_event_kill); addCoreMessage(IRCD,m);
	m = createMessage("MODE",	  anope_event_mode); addCoreMessage(IRCD,m);
	m = createMessage("MOTD",	  anope_event_motd); addCoreMessage(IRCD,m);
	m = createMessage("NICK",	  anope_event_nick); addCoreMessage(IRCD,m);
	m = createMessage("UID",	  anope_event_uid); addCoreMessage(IRCD,m);
	m = createMessage("CAPAB",	 anope_event_capab); addCoreMessage(IRCD,m);
	m = createMessage("PART",	  anope_event_part); addCoreMessage(IRCD,m);
	m = createMessage("PING",	  anope_event_ping); addCoreMessage(IRCD,m);
	m = createMessage("TIME",	  anope_event_time); addCoreMessage(IRCD,m);
	m = createMessage("PRIVMSG",   anope_event_privmsg); addCoreMessage(IRCD,m);
	m = createMessage("QUIT",	  anope_event_quit); addCoreMessage(IRCD,m);
	m = createMessage("SERVER",	anope_event_server); addCoreMessage(IRCD,m);
	m = createMessage("SQUIT",	 anope_event_squit); addCoreMessage(IRCD,m);
	m = createMessage("RSQUIT",	anope_event_rsquit); addCoreMessage(IRCD,m);
	m = createMessage("TOPIC",	 anope_event_topic); addCoreMessage(IRCD,m);
	m = createMessage("WHOIS",	 anope_event_whois); addCoreMessage(IRCD,m);
	m = createMessage("SVSMODE",   anope_event_mode) ;addCoreMessage(IRCD,m);
	m = createMessage("FHOST",	 anope_event_chghost); addCoreMessage(IRCD,m);
	m = createMessage("CHGIDENT",  anope_event_chgident); addCoreMessage(IRCD,m);
	m = createMessage("FNAME",	 anope_event_chgname); addCoreMessage(IRCD,m);
	m = createMessage("SETHOST",   anope_event_sethost); addCoreMessage(IRCD,m);
	m = createMessage("SETIDENT",  anope_event_setident); addCoreMessage(IRCD,m);
	m = createMessage("SETNAME",   anope_event_setname); addCoreMessage(IRCD,m);
	m = createMessage("FJOIN",	 anope_event_fjoin); addCoreMessage(IRCD,m);
	m = createMessage("FMODE",	 anope_event_fmode); addCoreMessage(IRCD,m);
	m = createMessage("FTOPIC",	anope_event_ftopic); addCoreMessage(IRCD,m);
	m = createMessage("OPERTYPE",  anope_event_opertype); addCoreMessage(IRCD,m);
	m = createMessage("IDLE",	  anope_event_idle); addCoreMessage(IRCD,m);
	m = createMessage("METADATA", anope_event_metadata); addCoreMessage(IRCD,m);
}

bool ChannelModeFlood::IsValid(const char *value)
{
	char *dp, *end;
	if (value && *value != ':' && strtoul((*value == '*' ? value + 1 : value), &dp, 10) > 0 && *dp == ':' && *(++dp) && strtoul(dp, &end, 10) > 0 && !*end) return 1;
	else return 0;
}

void moduleAddModes()
{
	/* Add user modes */
	ModeManager::AddUserMode('i', new UserMode(UMODE_INVIS));
	ModeManager::AddUserMode('o', new UserMode(UMODE_OPER));
	ModeManager::AddUserMode('w', new UserMode(UMODE_WALLOPS));

	/* b/e/I */
	ModeManager::AddChannelMode('b', new ChannelModeBan());

	/* v/h/o/a/q */
	ModeManager::AddChannelMode('v', new ChannelModeStatus(CMODE_VOICE, CUS_VOICE, '+'));
	ModeManager::AddChannelMode('h', new ChannelModeStatus(CMODE_HALFOP, CUS_HALFOP, '%'));
	ModeManager::AddChannelMode('o', new ChannelModeStatus(CMODE_OP, CUS_OP, '@', true));
	ModeManager::AddChannelMode('a', new ChannelModeStatus(CMODE_PROTECT, CUS_PROTECT, '&', true));
	ModeManager::AddChannelMode('q', new ChannelModeStatus(CMODE_OWNER, CUS_OWNER, '~'));

	/* Add channel modes */
	ModeManager::AddChannelMode('i', new ChannelMode(CMODE_INVITE));
	ModeManager::AddChannelMode('k', new ChannelModeKey());
	ModeManager::AddChannelMode('l', new ChannelModeParam(CMODE_LIMIT, true));
	ModeManager::AddChannelMode('m', new ChannelMode(CMODE_MODERATED));
	ModeManager::AddChannelMode('n', new ChannelMode(CMODE_NOEXTERNAL));
	ModeManager::AddChannelMode('p', new ChannelMode(CMODE_PRIVATE));
	ModeManager::AddChannelMode('s', new ChannelMode(CMODE_SECRET));
	ModeManager::AddChannelMode('t', new ChannelMode(CMODE_TOPIC));
}

class ProtoInspIRCd : public Module
{
 public:
	ProtoInspIRCd(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(PROTOCOL);

		if (Config.Numeric)
			TS6SID = sstrdup(Config.Numeric);

		pmodule_ircd_version("InspIRCd 1.2");
		pmodule_ircd_cap(myIrcdcap);
		pmodule_ircd_var(myIrcd);
		pmodule_ircd_useTSMode(0);

		moduleAddModes();

		pmodule_ircd_proto(&ircd_proto);
		moduleAddIRCDMsgs();
	}

	~ProtoInspIRCd()
	{
		delete [] TS6SID;
	}
};

MODULE_INIT(ProtoInspIRCd)
