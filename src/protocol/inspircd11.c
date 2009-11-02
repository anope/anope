/* inspircd 1.1 beta 6+ functions
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
	{"InspIRCd 1.1",			/* ircd name */
	 "+I",					  /* Modes used by pseudoclients */
	 5,						 /* Chan Max Symbols	 */
	 "+ao",					 /* Channel Umode used by Botserv bots */
	 1,						 /* SVSNICK */
	 1,						 /* Vhost  */
	 "-r",					  /* Mode on UnReg */
	 1,						 /* Supports SGlines	 */
	 1,						 /* Supports SQlines	 */
	 1,						 /* Supports SZlines	 */
	 4,						 /* Number of server args */
	 0,						 /* Join 2 Set		   */
	 1,						 /* Join 2 Message	   */
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
	 1,						 /* O:LINE			   */
	 1,						 /* UMODE			   */
	 1,						 /* VHOST ON NICK		*/
	 0,						 /* Change RealName	  */
	 0,
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
	 0,						 /* ts6 */
	 0,						 /* p10 */
	 NULL,					  /* character set */
	 1,						 /* CIDR channelbans */
	 "$",					   /* TLD Prefix for Global */
	 false,					/* Auth for users is sent after the initial NICK/UID command */
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


/* CHGHOST */
void inspircd_cmd_chghost(const char *nick, const char *vhost)
{
	if (has_chghostmod == 1) {
	if (!nick || !vhost) {
		return;
	}
	send_cmd(s_OperServ, "CHGHOST %s %s", nick, vhost);
	} else {
	ircdproto->SendGlobops(s_OperServ, "CHGHOST not loaded!");
	}
}

int anope_event_idle(const char *source, int ac, const char **av)
{
	if (ac == 1) {
		send_cmd(av[0], "IDLE %s %ld 0", source, static_cast<long>(time(NULL)));
	}
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

	void ProcessUsermodes(User *user, int ac, const char **av)
	{
		int add = 1; /* 1 if adding modes, 0 if deleting */
		const char *modes = av[0];
		--ac;
		if (debug) alog("debug: Changing mode for %s to %s", user->nick, modes);
		while (*modes) {
			if (add)
				user->SetMode(*modes);
			else
				user->RemoveMode(*modes);

			switch (*modes++) {
				case '+':
					add = 1;
					break;
				case '-':
					add = 0;
					break;
				case 'o':
					if (add) {
						++opcnt;
						if (WallOper) ircdproto->SendGlobops(s_OperServ, "\2%s\2 is now an IRC operator.", user->nick);
					}
					else --opcnt;
					break;
				case 'r':
					if (add && !nick_identified(user))
						SendUnregisteredNick(user);
					break;
				case 'x':
					if (add && user->vhost)
					{
						/* If +x is recieved then User::vhost IS the cloaked host,
						 * set the cloaked host correctly and destroy the vhost - Adam
						 */
						user->SetCloakedHost(user->vhost);
						delete [] user->vhost;
						user->vhost = NULL;
					}
					else
					{
						if (user->vhost)
							delete [] user->vhost;
						user->vhost = NULL;
					}
					user->UpdateHost();
			}
		}
	}

	/* *INDENT-ON* */

	void SendAkillDel(const char *user, const char *host)
	{
		send_cmd(s_OperServ, "GLINE %s@%s", user, host);
	}

	void SendTopic(BotInfo *whosets, const char *chan, const char *whosetit, const char *topic, time_t when)
	{
		send_cmd(whosets->nick, "FTOPIC %s %lu %s :%s", chan, static_cast<unsigned long>(when), whosetit, topic);
	}

	void SendVhostDel(User *u)
	{
		if (u->HasMode(UMODE_CLOAK))
			inspircd_cmd_chghost(u->nick, u->chost.c_str());
		else
			inspircd_cmd_chghost(u->nick, u->host);

		if (has_chgidentmod && u->GetIdent() != u->GetVIdent())
		{
			inspircd_cmd_chgident(u->nick, u->GetIdent().c_str());
		}
	}

	void SendAkill(const char *user, const char *host, const char *who, time_t when, time_t expires, const char *reason)
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = expires - time(NULL);
		if (timeleft > 172800)
			timeleft = 172800;
		send_cmd(ServerName, "ADDLINE G %s@%s %s %ld %ld :%s", user, host, who, static_cast<long>(time(NULL)), static_cast<long>(timeleft), reason);
	}

	void SendSVSKillInternal(const char *source, const char *user, const char *buf)
	{
		if (!buf || !source || !user) return;
		send_cmd(source, "KILL %s :%s", user, buf);
	}

	void SendSVSMode(User *u, int ac, const char **av)
	{
		send_cmd(s_NickServ, "MODE %s %s", u->nick, merge_args(ac, av));
	}

	void SendNumericInternal(const char *source, int numeric, const char *dest, const char *buf)
	{
		send_cmd(source, "PUSH %s ::%s %03d %s %s", dest, source, numeric, dest, buf);
	}

	void SendGuestNick(const char *nick, const char *user, const char *host, const char *real, const char *modes)
	{
		send_cmd(ServerName, "NICK %ld %s %s %s %s +%s 0.0.0.0 :%s", static_cast<long>(time(NULL)), nick, host, host, user, modes, real);
	}

	void SendModeInternal(BotInfo *source, const char *dest, const char *buf)
	{
		if (!buf) return;
		Channel *c = findchan(dest);
		send_cmd(source ? source->nick : s_OperServ, "FMODE %s %u %s", dest, static_cast<unsigned>(c ? c->creation_time : time(NULL)), buf);
	}

	void SendClientIntroduction(const char *nick, const char *user, const char *host, const char *real, const char *modes, const char *uid)
	{
		send_cmd(ServerName, "NICK %ld %s %s %s %s %s 0.0.0.0 :%s", static_cast<long>(time(NULL)), nick, host, host, user, modes, real);
		send_cmd(nick, "OPERTYPE Service");
	}

	void SendKickInternal(BotInfo *source, const char *chan, const char *user, const char *buf)
	{
		if (buf) send_cmd(source->nick, "KICK %s %s :%s", chan, user, buf);
		else send_cmd(source->nick, "KICK %s %s :%s", chan, user, user);
	}

	void SendNoticeChanopsInternal(BotInfo *source, const char *dest, const char *buf)
	{
		if (!buf) return;
		send_cmd(ServerName, "NOTICE @%s :%s", dest, buf);
	}

	void SendBotOp(const char *nick, const char *chan)
	{
		SendMode(findbot(nick), chan, "%s %s %s", ircd->botchanumode, nick, nick);
	}


	/* SERVER services-dev.chatspike.net password 0 :Description here */
	void SendServer(Server *server)
	{
		send_cmd(ServerName, "SERVER %s %s %d :%s", server->name, currentpass, server->hops, server->desc);
	}

	/* JOIN */
	void SendJoin(BotInfo *user, const char *channel, time_t chantime)
	{
		send_cmd(user->nick, "JOIN %s %ld", channel, static_cast<long>(chantime));
	}

	/* UNSQLINE */
	void SendSQLineDel(const char *user)
	{
		if (!user) return;
		send_cmd(s_OperServ, "QLINE %s", user);
	}

	/* SQLINE */
	void SendSQLine(const char *mask, const char *reason)
	{
		if (!mask || !reason) return;
		send_cmd(ServerName, "ADDLINE Q %s %s %ld 0 :%s", mask, s_OperServ, static_cast<long>(time(NULL)), reason);
	}

	/* SQUIT */
	void SendSquit(const char *servname, const char *message)
	{
		if (!servname || !message) return;
		send_cmd(ServerName, "SQUIT %s :%s", servname, message);
	}

	/* Functions that use serval cmd functions */

	void SendVhost(const char *nick, const char *vIdent, const char *vhost)
	{
		if (!nick) return;
		if (vIdent) inspircd_cmd_chgident(nick, vIdent);
		inspircd_cmd_chghost(nick, vhost);
	}

	void SendConnect()
	{
		inspircd_cmd_pass(uplink_server->password);
		me_server = new_server(NULL, ServerName, ServerDesc, SERVER_ISME, NULL);
		SendServer(me_server);
		send_cmd(NULL, "BURST");
		send_cmd(ServerName, "VERSION :Anope-%s %s :%s - %s (%s) -- %s", version_number, ServerName, ircd->name, version_flags, EncModuleList[0], version_build);
	}

	/* CHGIDENT */
	void inspircd_cmd_chgident(const char *nick, const char *vIdent)
	{
		if (has_chgidentmod == 1) {
			if (!nick || !vIdent || !*vIdent) {
				return;
			}
			send_cmd(s_OperServ, "CHGIDENT %s %s", nick, vIdent);
		} else {
			ircdproto->SendGlobops(s_OperServ, "CHGIDENT not loaded!");
		}
	}

	/* SVSHOLD - set */
	void SendSVSHold(const char *nick)
	{
		send_cmd(s_OperServ, "SVSHOLD %s %ds :%s", nick, static_cast<int>(NSReleaseTimeout), "Being held for registered user");
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const char *nick)
	{
		send_cmd(s_OperServ, "SVSHOLD %s", nick);
	}

	/* UNSZLINE */
	void SendSZLineDel(const char *mask)
	{
		send_cmd(s_OperServ, "ZLINE %s", mask);
	}

	/* SZLINE */
	void SendSZLine(const char *mask, const char *reason, const char *whom)
	{
		send_cmd(ServerName, "ADDLINE Z %s %s %ld 0 :%s", mask, whom, static_cast<long>(time(NULL)), reason);
	}

	/* SVSMODE +- */
	void SendUnregisteredNick(User *u)
	{
		if (u->HasMode(UMODE_REGISTERED))
			common_svsmode(u, "-r", NULL);
	}

	void SendSVSJoin(const char *source, const char *nick, const char *chan, const char *param)
	{
		send_cmd(source, "SVSJOIN %s %s", nick, chan);
	}

	void SendSVSPart(const char *source, const char *nick, const char *chan)
	{
		send_cmd(source, "SVSPART %s %s", nick, chan);
	}

	void SendEOB()
	{
		send_cmd(NULL, "ENDBURST");
	}

	void SetAutoIdentificationToken(User *u)
	{
		char svidbuf[15], *c;

		if (!u->nc)
			return;

		snprintf(svidbuf, sizeof(svidbuf), "%ld", static_cast<long>(u->timestamp));

		if (u->nc->GetExt("authenticationtoken", c))
		{
			delete [] c;
			u->nc->Shrink("authenticationtoken");
		}

		u->nc->Extend("authenticationtoken", sstrdup(svidbuf));
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
	if (ac < 2)
		return MOD_CONT;

	if (*av[0] == '#' || *av[0] == '&') {
		do_cmode(source, ac, av);
	} else {
		/* InspIRCd lets opers change another
		   users modes, we have to kludge this
		   as it slightly breaks RFC1459
		 */
		if (!strcasecmp(source, av[0])) {
			do_umode(source, ac, av);
		} else {
			do_umode(av[0], ac, av);
		}
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

int anope_event_fjoin(const char *source, int ac, const char **av)
{
	const char *newav[10];

	/* storing the current nick */
	char *curnick;

	/* these are used to generate the final string that is passed to ircservices' core */
	int nlen = 0;
	char nicklist[514];

	/* temporary buffer */
	char prefixandnick[60];

	*nicklist = '\0';
	*prefixandnick = '\0';

	if (ac < 3)
		return MOD_CONT;

	spacesepstream nicks(av[2]);
	std::string nick;

	while (nicks.GetToken(nick)) {
		curnick = sstrdup(nick.c_str());
		char *curnick_real = curnick;
		for (; *curnick; curnick++) {
			/* I bet theres a better way to do this... */
			if ((*curnick == '&') ||
				(*curnick == '~') || (*curnick == '@') || (*curnick == '%')
				|| (*curnick == '+')) {
				prefixandnick[nlen++] = *curnick;
				continue;
			} else {
				if (*curnick == ',') {
					curnick++;
					strncpy(prefixandnick + nlen, curnick,
							sizeof(prefixandnick) - nlen);
					break;
				} else {
					alog("fjoin: unrecognised prefix: %c", *curnick);
				}
			}
		}
		strlcat(nicklist, prefixandnick, sizeof(nicklist));
		strlcat(nicklist, " ", sizeof(nicklist));
		delete [] curnick_real;
		nlen = 0;
	}

	newav[0] = av[1];		   /* timestamp */
	newav[1] = av[0];		   /* channel name */
	newav[2] = "+";			 /* channel modes */
	newav[3] = nicklist;
	do_sjoin(source, 4, newav);

	return MOD_CONT;
}

/* Events */
int anope_event_ping(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	ircdproto->SendPong(ServerName, av[0]);
	return MOD_CONT;
}

int anope_event_436(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;

	m_nickcoll(av[0]);
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

/* Taken from hybrid.c, topic syntax is identical */

int anope_event_topic(const char *source, int ac, const char **av)
{
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

	strscpy(c->topic_setter, source, sizeof(c->topic_setter));
	c->topic_time = topic_time;

	record_topic(av[0]);

	if (ac > 1 && *av[1]) {
		FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(c, av[1]));
	}
	else {
		FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(c, ""));
	}

	return MOD_CONT;
}

int anope_event_squit(const char *source, int ac, const char **av)
{
	if (ac != 2)
		return MOD_CONT;
	do_squit(source, ac, av);
	return MOD_CONT;
}

int anope_event_rsquit(const char *source, int ac, const char **av)
{
	if (ac < 1 || ac > 3)
		return MOD_CONT;

	/* Horrible workaround to an insp bug (#) in how RSQUITs are sent - mark */
	if (ac > 1 && strcmp(ServerName, av[0]) == 0)
		do_squit(source, ac - 1, av + 1);
	else
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
	if (ac != 2)
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

	if (ac != 2)
		return MOD_CONT;

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

	if (ac != 1)
		return MOD_CONT;

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

	if (ac != 2)
		return MOD_CONT;

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

	if (ac != 1)
		return MOD_CONT;

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
	User *user;
	struct in_addr addy;
	uint32 *ad = reinterpret_cast<uint32 *>(&addy);

	if (ac != 1) {
		if (ac == 8) {
			int ts = strtoul(av[0], NULL, 10);

			inet_aton(av[6], &addy);
			user = do_nick("", av[1],   /* nick */
						   av[4],   /* username */
						   av[2],   /* realhost */
						   source,  /* server */
						   av[7],   /* realname */
						   ts, htonl(*ad), av[3], NULL);
			if (user) {
				/* InspIRCd1.1 has no user mode +d so we
				 * use nick timestamp to check for auth - Adam
				 */
				user->CheckAuthenticationToken(av[0]);

				ircdproto->ProcessUsermodes(user, 1, &av[5]);
				user->SetCloakedHost(av[3]);
			}
		}
	} else {
		do_nick(source, av[0], NULL, NULL, NULL, NULL, 0, 0, NULL, NULL);
	}
	return MOD_CONT;
}


int anope_event_chghost(const char *source, int ac, const char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

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

/* EVENT: SERVER */
int anope_event_server(const char *source, int ac, const char **av)
{
	if (!stricmp(av[1], "1")) {
		uplink = sstrdup(av[0]);
	}
	do_server(source, av[0], av[1], av[2], NULL);
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
		if (strstr(av[1], "m_services.so")) {
			has_servicesmod = 1;
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
		}
	} else if (strcasecmp(av[0], "END") == 0) {
		if (!has_globopsmod) {
			send_cmd(NULL, "ERROR :m_globops is not loaded. This is required by Anope");
			quitmsg = "ERROR: Remote server does not have the m_globops module loaded, and this is required.";
			quitting = 1;
			return MOD_STOP;
		}
		if (!has_servicesmod) {
			send_cmd(NULL, "ERROR :m_services is not loaded. This is required by Anope");
			quitmsg = "ERROR: Remote server does not have the m_services module loaded, and this is required.";
			quitting = 1;
			return MOD_STOP;
		}
		if (!has_hidechansmod) {
			send_cmd(NULL, "ERROR :m_hidechans.so is not loaded. This is required by Anope");
			quitmsg = "ERROR: Remote server deos not have the m_hidechans module loaded, and this is required.";
			quitting = 1;
			return MOD_STOP;
		}
		if (!has_svsholdmod) {
			ircdproto->SendGlobops(s_OperServ, "SVSHOLD missing, Usage disabled until module is loaded.");
		}
		if (!has_chghostmod) {
			ircdproto->SendGlobops(s_OperServ, "CHGHOST missing, Usage disabled until module is loaded.");
		}
		if (!has_chgidentmod) {
			ircdproto->SendGlobops(s_OperServ, "CHGIDENT missing, Usage disabled until module is loaded.");
		}
		if (has_messagefloodmod) {
			ModeManager::AddChannelMode('f', new ChannelModeFlood());
		}
		if (has_banexceptionmod) {
			ModeManager::AddChannelMode('e', new ChannelModeExcept());
		}
		if (has_inviteexceptionmod) {
			ModeManager::AddChannelMode('e', new ChannelModeInvite());
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
	finish_sync(serv_uplink, 1);
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
	m = createMessage("CAPAB",	 anope_event_capab); addCoreMessage(IRCD,m);
	m = createMessage("PART",	  anope_event_part); addCoreMessage(IRCD,m);
	m = createMessage("PING",	  anope_event_ping); addCoreMessage(IRCD,m);
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
}

bool ChannelModeFlood::IsValid(const char *value)
{
	char *dp, *end;

	if (value && *value != ':' && strtoul((*value == '*' ? value + 1 : value), &dp, 10) > 0 && *dp == ':' && *(++dp) && strtoul(dp, &end, 10) > 0 && !*end)
		return true;

	return false;
}

void moduleAddModes()
{
	/* Add user modes */
	ModeManager::AddUserMode('g', new UserMode(UMODE_CALLERID));
	ModeManager::AddUserMode('h', new UserMode(UMODE_HELPOP));
	ModeManager::AddUserMode('i', new UserMode(UMODE_INVIS));
	ModeManager::AddUserMode('o', new UserMode(UMODE_OPER));
	ModeManager::AddUserMode('r', new UserMode(UMODE_REGISTERED));
	ModeManager::AddUserMode('w', new UserMode(UMODE_WALLOPS));
	ModeManager::AddUserMode('x', new UserMode(UMODE_CLOAK));

	/* b/e/I */
	ModeManager::AddChannelMode('b', new ChannelModeBan());

	/* v/h/o/a/q */
	ModeManager::AddChannelMode('v', new ChannelModeStatus(CMODE_VOICE, CUS_VOICE, '+'));
	ModeManager::AddChannelMode('h', new ChannelModeStatus(CMODE_HALFOP, CUS_HALFOP, '%'));
	ModeManager::AddChannelMode('o', new ChannelModeStatus(CMODE_OP, CUS_OP, '@', true));
	ModeManager::AddChannelMode('a', new ChannelModeStatus(CMODE_PROTECT, CUS_PROTECT, '&', true));
	ModeManager::AddChannelMode('q', new ChannelModeStatus(CMODE_OWNER, CUS_OWNER, '~'));

	/* Add channel modes */
	ModeManager::AddChannelMode('c', new ChannelMode(CMODE_BLOCKCOLOR));
	ModeManager::AddChannelMode('i', new ChannelMode(CMODE_INVITE));
	ModeManager::AddChannelMode('k', new ChannelModeKey());
	ModeManager::AddChannelMode('l', new ChannelModeParam(CMODE_LIMIT));
	ModeManager::AddChannelMode('m', new ChannelMode(CMODE_MODERATED));
	ModeManager::AddChannelMode('n', new ChannelMode(CMODE_NOEXTERNAL));
	ModeManager::AddChannelMode('p', new ChannelMode(CMODE_PRIVATE));
	ModeManager::AddChannelMode('r', new ChannelModeRegistered());
	ModeManager::AddChannelMode('s', new ChannelMode(CMODE_SECRET));
	ModeManager::AddChannelMode('t', new ChannelMode(CMODE_TOPIC));
	ModeManager::AddChannelMode('u', new ChannelMode(CMODE_AUDITORIUM));
	ModeManager::AddChannelMode('z', new ChannelMode(CMODE_SSL));
	ModeManager::AddChannelMode('A', new ChannelMode(CMODE_ALLINVITE));
	ModeManager::AddChannelMode('C', new ChannelMode(CMODE_NOCTCP));
	ModeManager::AddChannelMode('G', new ChannelMode(CMODE_FILTER));
	ModeManager::AddChannelMode('K', new ChannelMode(CMODE_NOKNOCK));
	ModeManager::AddChannelMode('L', new ChannelModeParam(CMODE_REDIRECT));
	ModeManager::AddChannelMode('N', new ChannelMode(CMODE_NONICK));
	ModeManager::AddChannelMode('O', new ChannelModeOper());
	ModeManager::AddChannelMode('Q', new ChannelMode(CMODE_NOKICK));
	ModeManager::AddChannelMode('R', new ChannelMode(CMODE_REGISTEREDONLY));
	ModeManager::AddChannelMode('S', new ChannelMode(CMODE_STRIPCOLOR));
	ModeManager::AddChannelMode('V', new ChannelMode(CMODE_NOINVITE));
}

class ProtoInspIRCd : public Module
{
 public:
	ProtoInspIRCd(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(PROTOCOL);

		pmodule_ircd_version("inspircdIRCd 1.1");
		pmodule_ircd_cap(myIrcdcap);
		pmodule_ircd_var(myIrcd);
		pmodule_ircd_useTSMode(0);

		moduleAddModes();

		ircd->DefMLock[(size_t)CMODE_NOEXTERNAL] = true;
		ircd->DefMLock[(size_t)CMODE_TOPIC] = true;
		ircd->DefMLock[(size_t)CMODE_REGISTERED] = true;

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

MODULE_INIT(ProtoInspIRCd)
