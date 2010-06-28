/* inspircd 1.1 beta 6+ functions
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
#include "modules.h"
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
	 1,						 /* Supports SNlines	 */
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
	 1,						 /* No Knock requires +i */
	 0,						 /* We support inspircd TOKENS */
	 0,						 /* TIME STAMPS are BASE64 */
	 0,						 /* Can remove User Channel Modes with SVSMODE */
	 0,						 /* Sglines are not enforced until user reconnects */
	 0,						 /* ts6 */
	 0,						 /* p10 */
	 1,						 /* CIDR channelbans */
	 "$",					   /* TLD Prefix for Global */
	 20,					/* Max number of modes we can send per line */
	 }
	,
	{NULL}
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
	if (has_chghostmod == 1)
	{
		if (!nick || !vhost)
			return;
		send_cmd(Config.s_OperServ, "CHGHOST %s %s", nick, vhost);
	}
	else
		ircdproto->SendGlobops(OperServ, "CHGHOST not loaded!");
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
	void SendAkillDel(XLine *x)
	{
		send_cmd(Config.s_OperServ, "GLINE %s", x->Mask.c_str());
	}

	void SendTopic(BotInfo *whosets, Channel *c, const char *whosetit, const char *topic)
	{
		send_cmd(whosets->nick, "FTOPIC %s %lu %s :%s", c->name.c_str(), static_cast<unsigned long>(c->topic_time), whosetit, topic);
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

	void SendAkill(XLine *x)
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - time(NULL);
		if (timeleft > 172800)
			timeleft = 172800;
		send_cmd(Config.ServerName, "ADDLINE G %s %s %ld %ld :%s", x->Mask.c_str(), x->By.c_str(), static_cast<long>(time(NULL)), static_cast<long>(timeleft), x->Reason.c_str());
	}

	void SendSVSKillInternal(BotInfo *source, User *user, const char *buf)
	{
		send_cmd(source ? source->nick : Config.ServerName, "KILL %s :%s", user->nick.c_str(), buf);
	}

	void SendSVSMode(User *u, int ac, const char **av)
	{
		this->SendModeInternal(NULL, u, merge_args(ac, av));
	}

	void SendNumericInternal(const char *source, int numeric, const char *dest, const char *buf)
	{
		send_cmd(source, "PUSH %s ::%s %03d %s %s", dest, source, numeric, dest, buf);
	}

	void SendGuestNick(const char *nick, const char *user, const char *host, const char *real, const char *modes)
	{
		send_cmd(Config.ServerName, "NICK %ld %s %s %s %s +%s 0.0.0.0 :%s", static_cast<long>(time(NULL)), nick, host, host, user, modes, real);
	}

	void SendModeInternal(BotInfo *source, Channel *dest, const char *buf)
	{
		if (!buf) return;
		send_cmd(source ? source->nick : Config.s_OperServ, "FMODE %s %u %s", dest->name.c_str(), static_cast<unsigned>(dest->creation_time), buf);
	}

	void SendModeInternal(BotInfo *bi, User *u, const char *buf)
	{
		if (!buf) return;
		send_cmd(bi ? bi->nick : Config.ServerName, "MODE %s %s", u->nick.c_str(), buf);
	}

	void SendClientIntroduction(const std::string &nick, const std::string &user, const std::string &host, const std::string &real, const char *modes, const std::string &uid)
	{
		send_cmd(Config.ServerName, "NICK %ld %s %s %s %s %s 0.0.0.0 :%s", static_cast<long>(time(NULL)), nick.c_str(), host.c_str(), host.c_str(), user.c_str(), modes, real.c_str());
		send_cmd(nick, "OPERTYPE Service");
	}

	void SendKickInternal(BotInfo *source, Channel *chan, User *user, const char *buf)
	{
		if (buf) send_cmd(source->nick, "KICK %s %s :%s", chan->name.c_str(), user->nick.c_str(), buf);
		else send_cmd(source->nick, "KICK %s %s :%s", chan->name.c_str(), user->nick.c_str(), user->nick.c_str());
	}

	void SendNoticeChanopsInternal(BotInfo *source, Channel *dest, const char *buf)
	{
		if (!buf) return;
		send_cmd(Config.ServerName, "NOTICE @%s :%s", dest->name.c_str(), buf);
	}

	/* SERVER services-dev.chatspike.net password 0 :Description here */
	void SendServer(Server *server)
	{
		send_cmd(Config.ServerName, "SERVER %s %s %d :%s", server->GetName().c_str(), currentpass, server->GetHops(), server->GetDescription().c_str());
	}

	/* JOIN */
	void SendJoin(BotInfo *user, const char *channel, time_t chantime)
	{
		send_cmd(user->nick, "JOIN %s %ld", channel, static_cast<long>(chantime));
	}

	/* UNSQLINE */
	void SendSQLineDel(XLine *x)
	{
		send_cmd(Config.s_OperServ, "QLINE %s", x->Mask.c_str());
	}

	/* SQLINE */
	void SendSQLine(XLine *x)
	{
		send_cmd(Config.ServerName, "ADDLINE Q %s %s %ld 0 :%s", x->Mask.c_str(), Config.s_OperServ, static_cast<long>(time(NULL)), x->Reason.c_str());
	}

	/* SQUIT */
	void SendSquit(const char *servname, const char *message)
	{
		if (!servname || !message) return;
		send_cmd(Config.ServerName, "SQUIT %s :%s", servname, message);
	}

	/* Functions that use serval cmd functions */

	void SendVhost(User *u, const std::string &vIdent, const std::string &vhost)
	{
		if (!vIdent.empty())
			inspircd_cmd_chgident(u->nick.c_str(), vIdent.c_str());
		if (!vhost.empty())
			inspircd_cmd_chghost(u->nick.c_str(), vhost.c_str());
	}

	void SendConnect()
	{
		Me = new Server(NULL, Config.ServerName, 0, Config.ServerDesc, "");
		inspircd_cmd_pass(uplink_server->password);
		SendServer(Me);
		send_cmd(NULL, "BURST");
		send_cmd(Config.ServerName, "VERSION :Anope-%s %s :%s - (%s) -- %s", Anope::Version().c_str(), Config.ServerName, ircd->name, Config.EncModuleList.begin()->c_str(), Anope::Build().c_str());
	}

	/* CHGIDENT */
	void inspircd_cmd_chgident(const char *nick, const char *vIdent)
	{
		if (has_chgidentmod == 1) {
			if (!nick || !vIdent || !*vIdent) {
				return;
			}
			send_cmd(Config.s_OperServ, "CHGIDENT %s %s", nick, vIdent);
		} else {
			ircdproto->SendGlobops(OperServ, "CHGIDENT not loaded!");
		}
	}

	/* SVSHOLD - set */
	void SendSVSHold(const char *nick)
	{
		send_cmd(Config.s_OperServ, "SVSHOLD %s %ds :%s", nick, static_cast<int>(Config.NSReleaseTimeout), "Being held for registered user");
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const char *nick)
	{
		send_cmd(Config.s_OperServ, "SVSHOLD %s", nick);
	}

	/* UNSZLINE */
	void SendSZLineDel(XLine *x)
	{
		send_cmd(Config.s_OperServ, "ZLINE %s", x->Mask.c_str());
	}

	/* SZLINE */
	void SendSZLine(XLine *x)
	{
		send_cmd(Config.ServerName, "ADDLINE Z %s %s %ld 0 :%s", x->Mask.c_str(), x->By.c_str(), static_cast<long>(time(NULL)), x->Reason.c_str());
	}

	/* SVSMODE +- */
	void SendUnregisteredNick(User *u)
	{
		u->RemoveMode(NickServ, UMODE_REGISTERED);
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
		char svidbuf[15];

		if (!u->Account())
			return;

		snprintf(svidbuf, sizeof(svidbuf), "%ld", static_cast<long>(u->timestamp));

		u->Account()->Shrink("authenticationtoken");
		u->Account()->Extend("authenticationtoken", new ExtensibleItemPointerArray<char>(sstrdup(svidbuf)));

		u->SetMode(NickServ, UMODE_REGISTERED);
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
			Alog(LOG_DEBUG) << "Param: " << newav[o - 1];
		}
		n++;
	}

	return anope_event_mode(source, ac - 1, newav);
}

int anope_event_fjoin(const char *source, int ac, const char **av)
{
	Channel *c = findchan(av[0]);
	time_t ts = atol(av[1]);
	bool was_created = false;
	bool keep_their_modes = true;

	if (!c)
	{
		c = new Channel(av[0], ts);
		was_created = true;
	}
	/* Our creation time is newer than what the server gave us */
	else if (c->creation_time > ts)
	{
		c->creation_time = ts;

		/* Remove status from all of our users */
		for (std::list<Mode *>::const_iterator it = ModeManager::Modes.begin(), it_end = ModeManager::Modes.end(); it != it_end; ++it)
		{
			Mode *m = *it;

			if (m->Type != MODE_STATUS)
				continue;

			ChannelMode *cm = dynamic_cast<ChannelMode *>(m);

			for (CUserList::const_iterator uit = c->users.begin(), uit_end = c->users.end(); uit != uit_end; ++uit)
			{
				UserContainer *uc = *uit;

				c->RemoveMode(NULL, cm, uc->user->nick);
			}
		}
		if (c->ci)
		{
			/* Rejoin the bot to fix the TS */
			if (c->ci->bi)
			{
				c->ci->bi->Part(c, "TS reop");
				c->ci->bi->Join(c);
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

	spacesepstream sep(av[ac - 1]);
	std::string buf;
	while (sep.GetToken(buf))
	{
		std::list<ChannelMode *> Status;
		Status.clear();
		char ch;

		/* Loop through prefixes */
		while ((ch = ModeManager::GetStatusChar(buf[0])))
		{
			ChannelMode *cm = ModeManager::FindChannelModeByChar(ch);

			if (!cm)
			{
				Alog() << "Recieved unknown mode prefix " << buf[0] << " in FJOIN string";
				buf.erase(buf.begin());
				continue;
			}

			buf.erase(buf.begin());
			Status.push_back(cm);
		}

		User *u = finduser(buf);
		if (!u)
		{
			Alog(LOG_DEBUG) << "FJOIN for nonexistant user " << buf << " on " << c->name;
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

/* Events */
int anope_event_ping(const char *source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	ircdproto->SendPong(Config.ServerName, av[0]);
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

	if (!c)
	{
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

	c->topic_setter = source;
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
	if (ac > 1 && strcmp(Config.ServerName, av[0]) == 0)
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

	u = finduser(source);
	if (!u)
	{
		Alog(LOG_DEBUG) << "FNAME for nonexistent user " << source;
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

				UserSetInternalModes(user, 1, &av[5]);
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
	if (!u)
	{
		Alog(LOG_DEBUG) << "FHOST for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetDisplayedHost(av[0]);
	return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(const char *source, int ac, const char **av)
{
	do_server(source, av[0], atoi(av[1]), av[2], "");
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
	} else if (strcasecmp(av[0], "CAPABILITIES") == 0) {
		spacesepstream ssep(av[1]);
		std::string capab;
		while (ssep.GetToken(capab))
		{
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
							ModeManager::AddChannelMode(new ChannelModeInvex('I'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelModeList(CMODE_END, "", modebuf[t]));
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
						default:
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, "", modebuf[t]));

					}
				}

				sep.GetToken(modebuf);
				for (size_t t = 0; t < modebuf.size(); ++t)
				{
					switch (modebuf[t])
					{
						case 'f':
							ModeManager::AddChannelMode(new ChannelModeFlood('f'));
							continue;
						case 'l':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, "CMODE_LIMIT", 'l', true));
							continue;
						case 'L':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_REDIRECT, "CMODE_REDIRECT", 'L', true));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, "", modebuf[t], true));
					}
				}

				sep.GetToken(modebuf);
				for (size_t t = 0; t < modebuf.size(); ++t)
				{
					switch (modebuf[t])
					{
						case 'i':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, "CMODE_INVITE", 'i'));
							continue;
						case 'm':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, "CMODE_MODERATED", 'm'));
							continue;
						case 'n':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, "CMODE_NOEXTERNAL", 'n'));
							continue;
						case 'p':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, "CMODE_PRIVATE", 'p'));
							continue;
						case 's':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, "CMODE_SECRET", 's'));
							continue;
						case 't':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, "CMODE_TOPIC", 't'));
							continue;
						case 'r':
							ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
							continue;
						case 'c':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCOLOR, "CMODE_BLOCKCOLOR", 'c'));
							continue;
						case 'u':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_AUDITORIUM, "CMODE_AUDITORIUM", 'u'));
							continue;
						case 'z':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_SSL, "CMODE_SSL", 'z'));
							continue;
						case 'A':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_ALLINVITE, "CMODE_ALLINVITE", 'A'));
							continue;
						case 'C':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOCTCP, "CMODE_NOCTCP", 'C'));
							continue;
						case 'G':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_FILTER, "CMODE_FILTER", 'G'));
							continue;
						case 'K':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKNOCK, "CMODE_NOKNOCK", 'K'));
							continue;
						case 'N':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NONICK, "CMODE_NONICK", 'N'));
							continue;
						case 'O':
							ModeManager::AddChannelMode(new ChannelModeOper('O'));
							continue;
						case 'Q':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKICK, "CMODE_NOKICK", 'Q'));
							continue;
						case 'R':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_REGISTEREDONLY, "CMODE_REGISTEREDONLY", 'R'));
							continue;
						case 'S':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_STRIPCOLOR, "CMODE_STRIPCOLOR", 'S'));
							continue;
						case 'V':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOINVITE, "CMODE_NOINVITE", 'V'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelMode(CMODE_END, "", modebuf[t]));
					}
				}
			}
			else if (capab.find("PREIX=(") != std::string::npos)
			{
				std::string modes(capab.begin() + 8, capab.begin() + capab.find(")"));
				std::string chars(capab.begin() + capab.find(")") + 1, capab.end());

				for (size_t t = 0; t < modes.size(); ++t)
				{
					switch (modes[t])
					{
						case 'q':
							ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, "CMODE_OWNER", 'q', '~'));
							continue;
						case 'a':
							ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT, "CMODE_PROTECT", 'a', '&'));
							continue;
						case 'o':
							ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, "CMODE_OP", 'o', '@'));
							continue;
						case 'h':
							ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_HALFOP, "CMODE_HALFOP", 'h', '%'));
							continue;
						case 'v':
							ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, "CMODE_VOICE", 'v', '+'));
							continue;
					}
				}
			}
			else if (capab.find("MAXMODES=") != std::string::npos)
			{
				std::string maxmodes(capab.begin() + 9, capab.end());
				ircd->maxmodes = atoi(maxmodes.c_str());
			}
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
			ircdproto->SendGlobops(OperServ, "SVSHOLD missing, Usage disabled until module is loaded.");
		}
		if (!has_chghostmod) {
			ircdproto->SendGlobops(OperServ, "CHGHOST missing, Usage disabled until module is loaded.");
		}
		if (!has_chgidentmod) {
			ircdproto->SendGlobops(OperServ, "CHGIDENT missing, Usage disabled until module is loaded.");
		}
		ircd->svshold = has_svsholdmod;
	}
	
	CapabParse(ac, av);
	return MOD_CONT;
}

int anope_event_endburst(const char *source, int ac, const char **av)
{
	Me->GetUplink()->Sync(true);
	return MOD_CONT;
}


void moduleAddIRCDMsgs()
{
	Anope::AddMessage("ENDBURST", anope_event_endburst);
	Anope::AddMessage("436", anope_event_436);
	Anope::AddMessage("AWAY", anope_event_away);
	Anope::AddMessage("JOIN", anope_event_join);
	Anope::AddMessage("KICK", anope_event_kick);
	Anope::AddMessage("KILL", anope_event_kill);
	Anope::AddMessage("MODE", anope_event_mode);
	Anope::AddMessage("MOTD", anope_event_motd);
	Anope::AddMessage("NICK", anope_event_nick);
	Anope::AddMessage("CAPAB",anope_event_capab);
	Anope::AddMessage("PART", anope_event_part);
	Anope::AddMessage("PING", anope_event_ping);
	Anope::AddMessage("PRIVMSG", anope_event_privmsg);
	Anope::AddMessage("QUIT", anope_event_quit);
	Anope::AddMessage("SERVER", anope_event_server);
	Anope::AddMessage("SQUIT", anope_event_squit);
	Anope::AddMessage("RSQUIT", anope_event_rsquit);
	Anope::AddMessage("TOPIC", anope_event_topic);
	Anope::AddMessage("WHOIS", anope_event_whois);
	Anope::AddMessage("SVSMODE", anope_event_mode);
	Anope::AddMessage("FHOST", anope_event_chghost);
	Anope::AddMessage("CHGIDENT", anope_event_chgident);
	Anope::AddMessage("FNAME", anope_event_chgname);
	Anope::AddMessage("SETHOST", anope_event_sethost);
	Anope::AddMessage("SETIDENT", anope_event_setident);
	Anope::AddMessage("SETNAME", anope_event_setname);
	Anope::AddMessage("FJOIN", anope_event_fjoin);
	Anope::AddMessage("FMODE", anope_event_fmode);
	Anope::AddMessage("FTOPIC", anope_event_ftopic);
	Anope::AddMessage("OPERTYPE", anope_event_opertype);
	Anope::AddMessage("IDLE", anope_event_idle);
}

bool ChannelModeFlood::IsValid(const std::string &value)
{
	char *dp, *end;

	if (!value.empty() && value[0] != ':' && strtoul((value[0] == '*' ? value.c_str() + 1 : value.c_str()), &dp, 10) > 0 && *dp == ':' && *(++dp) && strtoul(dp, &end, 10) > 0 && !*end)
		return true;

	return false;
}

static void AddModes()
{
	ModeManager::AddUserMode(new UserMode(UMODE_CALLERID, "UMODE_CALLERID", 'g'));
	ModeManager::AddUserMode(new UserMode(UMODE_HELPOP, "UMODE_HELPOP", 'h'));
	ModeManager::AddUserMode(new UserMode(UMODE_INVIS, "UMODE_INVIS", 'i'));
	ModeManager::AddUserMode(new UserMode(UMODE_OPER, "UMODE_OPER", 'o'));
	ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, "UMODE_REGISTERED", 'r'));
	ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, "UMODE_WALLOPS", 'w'));
	ModeManager::AddUserMode(new UserMode(UMODE_CLOAK, "UMODE_CLOAK", 'x'));
}

class ProtoInspIRCd : public Module
{
 public:
	ProtoInspIRCd(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(PROTOCOL);

		pmodule_ircd_version("InspIRCd 1.1");
		pmodule_ircd_var(myIrcd);
		pmodule_ircd_useTSMode(0);

		CapabType c[] = { CAPAB_NOQUIT, CAPAB_SSJ3, CAPAB_NICK2, CAPAB_VL, CAPAB_TLKEXT };
		for (unsigned i = 0; i < 5; ++i)
			Capab.SetFlag(c[i]);

		AddModes();

		pmodule_ircd_proto(&ircd_proto);
		moduleAddIRCDMsgs();

		ModuleManager::Attach(I_OnUserNickChange, this);
	}

	void OnUserNickChange(User *u, const std::string &)
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
	}

};

MODULE_INIT(ProtoInspIRCd)
