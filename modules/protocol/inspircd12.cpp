/* inspircd 1.2 functions
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
#include "hashcomp.h"

IRCDVar myIrcd[] = {
	{"InspIRCd 1.2",	/* ircd name */
	 "+I",				/* Modes used by pseudoclients */
	 5,					/* Chan Max Symbols */
	 "+ao",				/* Channel Umode used by Botserv bots */
	 1,					/* SVSNICK */
	 1,					/* Vhost */
	 0,					/* Supports SNlines */
	 1,					/* Supports SQlines */
	 1,					/* Supports SZlines */
	 4,					/* Number of server args */
	 0,					/* Join 2 Set */
	 0,					/* Join 2 Message */
	 1,					/* TS Topic Forward */
	 0,					/* TS Topci Backward */
	 0,					/* Chan SQlines */
	 0,					/* Quit on Kill */
	 0,					/* SVSMODE unban */
	 1,					/* Reverse */
	 1,					/* vidents */
	 1,					/* svshold */
	 0,					/* time stamp on mode */
	 0,					/* NICKIP */
	 0,					/* O:LINE */
	 1,					/* UMODE */
	 1,					/* VHOST ON NICK */
	 0,					/* Change RealName */
	 1,					/* No Knock requires +i */
	 0,					/* We support inspircd TOKENS */
	 0,					/* Can remove User Channel Modes with SVSMODE */
	 0,					/* Sglines are not enforced until user reconnects */
	 1,					/* ts6 */
	 0,					/* p10 */
	 1,					/* CIDR channelbans */
	 "$",				/* TLD Prefix for Global */
	 20,				/* Max number of modes we can send per line */
	 }
	,
	{NULL}
};

static bool has_servicesmod = false;
static bool has_globopsmod = false;
static bool has_svsholdmod = false;
static bool has_chghostmod = false;
static bool has_chgidentmod = false;
static bool has_hidechansmod = false;

/* Previously introduced user during burst */
static User *prev_u_intro = NULL;

/* CHGHOST */
void inspircd_cmd_chghost(const Anope::string &nick, const Anope::string &vhost)
{
	if (!has_chghostmod)
	{
		ircdproto->SendGlobops(OperServ, "CHGHOST not loaded!");
		return;
	}

	send_cmd(OperServ->GetUID(), "CHGHOST %s %s", nick.c_str(), vhost.c_str());
}

int anope_event_idle(const Anope::string &source, int ac, const char **av)
{
	BotInfo *bi = findbot(av[0]);
	if (!bi)
		return MOD_CONT;

	send_cmd(bi->GetUID(), "IDLE %s %ld %ld", source.c_str(), static_cast<long>(start_time), static_cast<long>(time(NULL) - bi->lastmsg));
	return MOD_CONT;
}

static Anope::string currentpass;

/* PASS */
void inspircd_cmd_pass(const Anope::string &pass)
{
	currentpass = pass;
}

class InspIRCdProto : public IRCDProto
{
	void SendAkillDel(const XLine *x)
	{
		send_cmd(OperServ->GetUID(), "GLINE %s", x->Mask.c_str());
	}

	void SendTopic(const BotInfo *whosets, const Channel *c, const Anope::string &whosetit, const Anope::string &topic)
	{
		send_cmd(whosets->GetUID(), "FTOPIC %s %lu %s :%s", c->name.c_str(), static_cast<unsigned long>(c->topic_time), whosetit.c_str(), topic.c_str());
	}

	void SendVhostDel(User *u)
	{
		if (u->HasMode(UMODE_CLOAK))
			inspircd_cmd_chghost(u->nick, u->chost);
		else
			inspircd_cmd_chghost(u->nick, u->host);

		if (has_chgidentmod && u->GetIdent() != u->GetVIdent())
			inspircd_cmd_chgident(u->nick, u->GetIdent());
	}

	void SendAkill(const XLine *x)
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - time(NULL);
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		send_cmd(OperServ->GetUID(), "ADDLINE G %s@%s %s %ld %ld :%s", x->GetUser().c_str(), x->GetHost().c_str(), x->By.c_str(), static_cast<long>(time(NULL)), static_cast<long>(timeleft), x->Reason.c_str());
	}

	void SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf)
	{
		send_cmd(source ? source->GetUID() : TS6SID, "KILL %s :%s", user->GetUID().c_str(), buf.c_str());
	}

	void SendSVSMode(const User *u, int ac, const char **av)
	{
		this->SendModeInternal(NULL, u, merge_args(ac, av));
	}

	void SendNumericInternal(const Anope::string &source, int numeric, const Anope::string &dest, const Anope::string &buf)
	{
		send_cmd(TS6SID, "PUSH %s ::%s %03d %s %s", dest.c_str(), source.c_str(), numeric, dest.c_str(), buf.c_str());
	}

	void SendModeInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf)
	{
		send_cmd(source ? source->GetUID() : TS6SID, "FMODE %s %u %s", dest->name.c_str(), static_cast<unsigned>(dest->creation_time), buf.c_str());
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf)
	{
		if (buf.empty())
			return;
		send_cmd(bi ? bi->GetUID() : TS6SID, "MODE %s %s", u->GetUID().c_str(), buf.c_str());
	}

	void SendClientIntroduction(const Anope::string &nick, const Anope::string &user, const Anope::string &host, const Anope::string &real, const Anope::string &modes, const Anope::string &uid)
	{
		send_cmd(TS6SID, "UID %s %ld %s %s %s %s 0.0.0.0 %ld %s :%s", uid.c_str(), static_cast<long>(time(NULL)), nick.c_str(), host.c_str(), host.c_str(), user.c_str(), static_cast<long>(time(NULL)), modes.c_str(), real.c_str());
	}

	void SendKickInternal(const BotInfo *source, const Channel *chan, const User *user, const Anope::string &buf)
	{
		if (!buf.empty())
			send_cmd(source->GetUID(), "KICK %s %s :%s", chan->name.c_str(), user->GetUID().c_str(), buf.c_str());
		else
			send_cmd(source->GetUID(), "KICK %s %s :%s", chan->name.c_str(), user->GetUID().c_str(), user->nick.c_str());
	}

	void SendNoticeChanopsInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf)
	{
		send_cmd(TS6SID, "NOTICE @%s :%s", dest->name.c_str(), buf.c_str());
	}

	/* SERVER services-dev.chatspike.net password 0 :Description here */
	void SendServer(const Server *server)
	{
		send_cmd("", "SERVER %s %s %d %s :%s", server->GetName().c_str(), currentpass.c_str(), server->GetHops(), server->GetSID().c_str(), server->GetDescription().c_str());
	}

	/* JOIN */
	void SendJoin(const BotInfo *user, const Anope::string &channel, time_t chantime)
	{
		send_cmd(TS6SID, "FJOIN %s %ld + :,%s", channel.c_str(), static_cast<long>(chantime), user->GetUID().c_str());
	}

	/* UNSQLINE */
	void SendSQLineDel(const XLine *x)
	{
		send_cmd(TS6SID, "DELLINE Q %s", x->Mask.c_str());
	}

	/* SQLINE */
	void SendSQLine(const XLine *x)
	{
		send_cmd(TS6SID, "ADDLINE Q %s %s %ld 0 :%s", x->Mask.c_str(), Config->s_OperServ.c_str(), static_cast<long>(time(NULL)), x->Reason.c_str());
	}

	/* SQUIT */
	void SendSquit(const Anope::string &servname, const Anope::string &message)
	{
		send_cmd(TS6SID, "SQUIT %s :%s", servname.c_str(), message.c_str());
	}

	/* Functions that use serval cmd functions */

	void SendVhost(User *u, const Anope::string &vIdent, const Anope::string &vhost)
	{
		if (!vIdent.empty())
			inspircd_cmd_chgident(u->nick, vIdent);
		if (!vhost.empty())
			inspircd_cmd_chghost(u->nick, vhost);
	}

	void SendConnect()
	{
		inspircd_cmd_pass(uplink_server->password);
		SendServer(Me);
		send_cmd(TS6SID, "BURST");
		send_cmd(TS6SID, "VERSION :Anope-%s %s :%s - (%s) -- %s", Anope::Version().c_str(), Config->ServerName.c_str(), ircd->name, Config->EncModuleList.begin()->c_str(), Anope::Build().c_str());
	}

	/* CHGIDENT */
	void inspircd_cmd_chgident(const Anope::string &nick, const Anope::string &vIdent)
	{
		if (!has_chgidentmod)
			ircdproto->SendGlobops(OperServ, "CHGIDENT not loaded!");
		else
			send_cmd(OperServ->GetUID(), "CHGIDENT %s %s", nick.c_str(), vIdent.c_str());
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick)
	{
		send_cmd(OperServ->GetUID(), "SVSHOLD %s %u :Being held for registered user", nick.c_str(), static_cast<unsigned>(Config->NSReleaseTimeout));
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick)
	{
		send_cmd(OperServ->GetUID(), "SVSHOLD %s", nick.c_str());
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x)
	{
		send_cmd(TS6SID, "DELLINE Z %s", x->Mask.c_str());
	}

	/* SZLINE */
	void SendSZLine(const XLine *x)
	{
		send_cmd(TS6SID, "ADDLINE Z %s %s %ld 0 :%s", x->Mask.c_str(), x->By.c_str(), static_cast<long>(time(NULL)), x->Reason.c_str());
	}

	void SendSVSJoin(const Anope::string &source, const Anope::string &nick, const Anope::string &chan, const Anope::string &)
	{
		User *u = finduser(nick);
		BotInfo *bi = findbot(source);
		send_cmd(bi->GetUID(), "SVSJOIN %s %s", u->GetUID().c_str(), chan.c_str());
	}

	void SendSVSPart(const Anope::string &source, const Anope::string &nick, const Anope::string &chan)
	{
		User *u = finduser(nick);
		BotInfo *bi = findbot(source);
		send_cmd(bi->GetUID(), "SVSPART %s %s", u->GetUID().c_str(), chan.c_str());
	}

	void SendSWhois(const Anope::string &, const Anope::string &who, const Anope::string &mask)
	{
		User *u = finduser(who);

		send_cmd(TS6SID, "METADATA %s swhois :%s", u->GetUID().c_str(), mask.c_str());
	}

	void SendEOB()
	{
		send_cmd(TS6SID, "ENDBURST");
	}

	void SendGlobopsInternal(BotInfo *source, const Anope::string &buf)
	{
		if (has_globopsmod)
			send_cmd(source ? source->GetUID() : TS6SID, "SNONOTICE g :%s", buf.c_str());
		else
			send_cmd(source ? source->GetUID() : TS6SID, "SNONOTICE A :%s", buf.c_str());
	}

	void SendAccountLogin(const User *u, const NickCore *account)
	{
		send_cmd(TS6SID, "METADATA %s accountname :%s", u->GetUID().c_str(), account->display.c_str());
	}

	void SendAccountLogout(const User *u, const NickCore *account)
	{
		send_cmd(TS6SID, "METADATA %s accountname :", u->GetUID().c_str());
	}

	bool IsNickValid(const Anope::string &nick)
	{
		/* InspIRCd, like TS6, uses UIDs on collision, so... */
		if (isdigit(nick[0]))
			return false;

		return true;
	}

	void SetAutoIdentificationToken(User *u)
	{
		if (!u->Account())
			return;

		u->SetMode(NickServ, UMODE_REGISTERED);
	}
} ircd_proto;

int anope_event_ftopic(const Anope::string &source, int ac, const char **av)
{
	/* :source FTOPIC channel ts setby :topic */
	const char *temp;
	if (ac < 4)
		return MOD_CONT;
	temp = av[1]; /* temp now holds ts */
	av[1] = av[2]; /* av[1] now holds set by */
	av[2] = temp; /* av[2] now holds ts */
	do_topic(source, ac, av);
	return MOD_CONT;
}

int anope_event_mode(const Anope::string &source, int ac, const char **av)
{
	if (*av[0] == '#' || *av[0] == '&')
		do_cmode(source, ac, av);
	else
	{
		/* InspIRCd lets opers change another
		   users modes, we have to kludge this
		   as it slightly breaks RFC1459
		 */
		User *u = finduser(source);
		User *u2 = finduser(av[0]);

		// This can happen with server-origin modes.
		if (!u)
			u = u2;

		// if it's still null, drop it like fire.
		// most likely situation was that server introduced a nick which we subsequently akilled
		if (!u)
			return MOD_CONT;

		av[0] = u2->nick.c_str();
		do_umode(u->nick, ac, av);
	}
	return MOD_CONT;
}

int anope_event_opertype(const Anope::string &source, int ac, const char **av)
{
	/* opertype is equivalent to mode +o because servers
	   dont do this directly */
	User *u;
	u = finduser(source);
	if (u && !is_oper(u))
	{
		const char *newav[2];
		newav[0] = source.c_str();
		newav[1] = "+o";
		return anope_event_mode(source, 2, newav);
	}
	else
		return MOD_CONT;
}

int anope_event_fmode(const Anope::string &source, int ac, const char **av)
{
	const char *newav[128];
	int n, o;
	Channel *c;

	/* :source FMODE #test 12345678 +nto foo */
	if (ac < 3)
		return MOD_CONT;

	/* Checking the TS for validity to avoid desyncs */
	if ((c = findchan(av[0])))
	{
		time_t ts = Anope::string(av[1]).is_number_only() ? convertTo<time_t>(av[1]) : 0;
		if (c->creation_time > ts)
			/* Our TS is bigger, we should lower it */
			c->creation_time = ts;
		else if (c->creation_time < ts)
			/* The TS we got is bigger, we should ignore this message. */
			return MOD_CONT;
	}
	else
		/* Got FMODE for a non-existing channel */
		return MOD_CONT;

	/* TS's are equal now, so we can proceed with parsing */
	n = o = 0;
	while (n < ac)
	{
		if (n != 1)
		{
			newav[o] = av[n];
			++o;
			Alog(LOG_DEBUG) << "Param: " << newav[o - 1];
		}
		++n;
	}

	return anope_event_mode(source, ac - 1, newav);
}

/*
 * [Nov 03 22:31:57.695076 2009] debug: Received: :964 FJOIN #test 1223763723 +BPSnt :,964AAAAAB ,964AAAAAC ,966AAAAAA
 *
 * 0: name
 * 1: channel ts (when it was created, see protocol docs for more info)
 * 2: channel modes + params (NOTE: this may definitely be more than one param!)
 * last: users
 */
int anope_event_fjoin(const Anope::string &source, int ac, const char **av)
{
	Channel *c = findchan(av[0]);
	time_t ts = Anope::string(av[1]).is_number_only() ? convertTo<time_t>(av[1]) : 0;
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

			ChannelMode *cm = debug_cast<ChannelMode *>(m);

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

	/* If we need to keep their modes, and this FJOIN string contains modes */
	if (keep_their_modes && ac >= 4)
	{
		/* Set the modes internally */
		ChanSetInternalModes(c, ac - 3, av + 2);
	}

	spacesepstream sep(av[ac - 1]);
	Anope::string buf;
	while (sep.GetToken(buf))
	{
		std::list<ChannelMode *> Status;

		/* Loop through prefixes and find modes for them */
		while (buf[0] != ',')
		{
			ChannelMode *cm = ModeManager::FindChannelModeByChar(buf[0]);
			if (!cm)
			{
				Alog() << "Receeved unknown mode prefix " << buf[0] << " in FJOIN string";
				buf.erase(buf.begin());
				continue;
			}

			buf.erase(buf.begin());
			Status.push_back(cm);
		}
		buf.erase(buf.begin());

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

	/* Channel is done syncing */
	if (was_created)
	{
		/* Unset the syncing flag */
		c->UnsetFlag(CH_SYNCING);
		c->Sync();
	}

	return MOD_CONT;
}

/* Events */
int anope_event_ping(const Anope::string &source, int ac, const char **av)
{
	if (ac == 1)
		ircdproto->SendPong("", av[0]);

	if (ac == 2)
		ircdproto->SendPong(av[1], av[0]);

	return MOD_CONT;
}

int anope_event_time(const Anope::string &source, int ac, const char **av)
{
	if (ac !=2)
		return MOD_CONT;

	send_cmd(TS6SID, "TIME %s %s %ld", source.c_str(), av[1], static_cast<long>(time(NULL)));

	/* We handled it, don't pass it on to the core..
	 * The core doesn't understand our syntax anyways.. ~ Viper */
	return MOD_STOP;
}

int anope_event_436(const Anope::string &source, int ac, const char **av)
{
	m_nickcoll(av[0]);
	return MOD_CONT;
}

int anope_event_away(const Anope::string &source, int ac, const char **av)
{
	m_away(source, ac ? av[0] : "");
	return MOD_CONT;
}

/* Taken from hybrid.c, topic syntax is identical */
int anope_event_topic(const Anope::string &source, int ac, const char **av)
{
	Channel *c = findchan(av[0]);
	time_t topic_time = time(NULL);
	User *u = finduser(source);

	if (!c)
	{
		Alog(LOG_DEBUG) << "debug: TOPIC " << merge_args(ac - 1, av + 1) << " for nonexistent channel " << av[0];
		return MOD_CONT;
	}

	if (check_topiclock(c, topic_time))
		return MOD_CONT;

	c->topic.clear();
	if (ac > 1 && *av[1])
		c->topic = av[1];

	c->topic_setter = u ? u->nick : source;
	c->topic_time = topic_time;

	record_topic(av[0]);

	if (ac > 1 && *av[1])
	{
		FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(c, av[0]));
	}
	else
	{
		FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(c, ""));
	}

	return MOD_CONT;
}

int anope_event_squit(const Anope::string &source, int ac, const char **av)
{
	do_squit(source, ac, av);
	return MOD_CONT;
}

int anope_event_rsquit(const Anope::string &source, int ac, const char **av)
{
	/* On InspIRCd we must send a SQUIT when we recieve RSQUIT for a server we have juped */
	Server *s = Server::Find(av[0]);
	if (s && s->HasFlag(SERVER_JUPED))
		send_cmd(TS6SID, "SQUIT %s :%s", s->GetSID().c_str(), ac > 1 ? av[1] : "");

	do_squit(source, ac, av);

	return MOD_CONT;
}

int anope_event_quit(const Anope::string &source, int ac, const char **av)
{
	do_quit(source, ac, av);
	return MOD_CONT;
}

int anope_event_kill(const Anope::string &source, int ac, const char **av)
{
	User *u = finduser(av[0]);
	BotInfo *bi = findbot(av[0]);
	m_kill(u ? u->nick : (bi ? bi->nick : av[0]), av[1]);
	return MOD_CONT;
}

int anope_event_kick(const Anope::string &source, int ac, const char **av)
{
	do_kick(source, ac, av);
	return MOD_CONT;
}

int anope_event_join(const Anope::string &source, int ac, const char **av)
{
	do_join(source, ac, av);
	return MOD_CONT;
}

int anope_event_motd(const Anope::string &source, int ac, const char **av)
{
	m_motd(source);
	return MOD_CONT;
}

int anope_event_setname(const Anope::string &source, int ac, const char **av)
{
	User *u;

	u = finduser(source);
	if (!u)
	{
		Alog(LOG_DEBUG) << "SETNAME for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetRealname(av[0]);
	return MOD_CONT;
}

int anope_event_chgname(const Anope::string &source, int ac, const char **av)
{
	User *u;

	u = finduser(source);
	if (!u)
	{
		Alog(LOG_DEBUG) << "FNAME for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetRealname(av[0]);
	return MOD_CONT;
}

int anope_event_setident(const Anope::string &source, int ac, const char **av)
{
	User *u;

	u = finduser(source);
	if (!u)
	{
		Alog(LOG_DEBUG) << "SETIDENT for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetIdent(av[0]);
	return MOD_CONT;
}

int anope_event_chgident(const Anope::string &source, int ac, const char **av)
{
	User *u;

	u = finduser(av[0]);
	if (!u)
	{
		Alog(LOG_DEBUG) << "CHGIDENT for nonexistent user " << av[0];
		return MOD_CONT;
	}

	u->SetIdent(av[1]);
	return MOD_CONT;
}

int anope_event_sethost(const Anope::string &source, int ac, const char **av)
{
	User *u;

	u = finduser(source);
	if (!u)
	{
		Alog(LOG_DEBUG) << "SETHOST for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetDisplayedHost(av[0]);
	return MOD_CONT;
}

int anope_event_nick(const Anope::string &source, int ac, const char **av)
{
	do_nick(source, av[0], "", "", "", "", 0, 0, "", "");
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

int anope_event_uid(const Anope::string &source, int ac, const char **av)
{
	User *user;
	NickAlias *na;
	Server *s = Server::Find(source);
	int ts = strtoul(av[1], NULL, 10);

	/* Check if the previously introduced user was Id'd for the nickgroup of the nick he s currently using.
	 * If not, validate the user.  ~ Viper*/
	user = prev_u_intro;
	prev_u_intro = NULL;
	if (user)
		na = findnick(user->nick);
	if (user && !user->server->IsSynced() && (!na || na->nc != user->Account()))
	{
		validate_user(user);
		if (user->HasMode(UMODE_REGISTERED))
			user->RemoveMode(NickServ, UMODE_REGISTERED);
	}
	user = NULL;

	user = do_nick("", av[2], av[5], av[3], s->GetName(), av[ac - 1], ts, 0, av[4], av[0]);
	if (user)
	{
		user->host = av[6];
		UserSetInternalModes(user, 1, &av[8]);
		user->SetCloakedHost(av[4]);
		if (!user->server->IsSynced())
			prev_u_intro = user;
		else
			validate_user(user);
	}

	return MOD_CONT;
}

int anope_event_chghost(const Anope::string &source, int ac, const char **av)
{
	User *u;

	u = finduser(source);
	if (!u)
	{
		Alog(LOG_DEBUG) << "FHOST for nonexistent user " << source;
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
int anope_event_server(const Anope::string &source, int ac, const char **av)
{
	do_server(source, av[0], Anope::string(av[2]).is_number_only() ? convertTo<unsigned>(av[2]) : 0, av[4], av[3]);
	return MOD_CONT;
}

int anope_event_privmsg(const Anope::string &source, int ac, const char **av)
{
	if (!finduser(source))
		return MOD_CONT; // likely a message from a server, which can happen.

	m_privmsg(source, av[0], av[1]);
	return MOD_CONT;
}

int anope_event_part(const Anope::string &source, int ac, const char **av)
{
	do_part(source, ac, av);
	return MOD_CONT;
}

int anope_event_whois(const Anope::string &source, int ac, const char **av)
{
	m_whois(source, av[0]);
	return MOD_CONT;
}

int anope_event_metadata(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac < 3)
		return MOD_CONT;
	else if (!strcmp(av[1], "accountname"))
	{
		if ((u = finduser(av[0])))
			/* Identify the user for this account - Adam */
			u->AutoID(av[2]);
	}

	return MOD_CONT;
}

int anope_event_capab(const Anope::string &source, int ac, const char **av)
{
	if (!strcasecmp(av[0], "START"))
	{
		/* reset CAPAB */
		has_servicesmod = false;
		has_globopsmod = false;
		has_svsholdmod = false;
		has_chghostmod = false;
		has_chgidentmod = false;
		has_hidechansmod = false;
	}
	else if (!strcasecmp(av[0], "MODULES"))
	{
		if (strstr(av[1], "m_globops.so"))
			has_globopsmod = true;
		if (strstr(av[1], "m_services_account.so"))
			has_servicesmod = true;
		if (strstr(av[1], "m_svshold.so"))
			has_svsholdmod = true;
		if (strstr(av[1], "m_chghost.so"))
			has_chghostmod = true;
		if (strstr(av[1], "m_chgident.so"))
			has_chgidentmod = true;
		if (strstr(av[1], "m_hidechans.so"))
			has_hidechansmod = true;
		if (strstr(av[1], "m_servprotect.so"))
			ircd->pseudoclient_mode = "+Ik";
	}
	else if (!strcasecmp(av[0], "CAPABILITIES"))
	{
		spacesepstream ssep(av[1]);
		Anope::string capab;
		while (ssep.GetToken(capab))
		{
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
						/* InspIRCd sends q and a here if they have no prefixes */
						case 'q':
							ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, "CMODE_OWNER", 'q', '@'));
							continue;
						case 'a':
							ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT , "CMODE_PROTECT", 'a', '@'));
							continue;
						// XXX list modes needs a bit of a rewrite, we need to be able to support +g here
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
						default:
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, "", modebuf[t]));
					}
				}

				sep.GetToken(modebuf);
				for (size_t t = 0, end = modebuf.length(); t < end; ++t)
				{
					switch (modebuf[t])
					{
						case 'F':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_NICKFLOOD, "CMODE_NICKFLOOD", 'F', true));
							continue;
						case 'J':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_NOREJOIN, "CMODE_NOREJOIN", 'J', true));
							continue;
						case 'L':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_REDIRECT, "CMODE_REDIRECT", 'L', true));
							continue;
						case 'f':
							ModeManager::AddChannelMode(new ChannelModeFlood('f', true));
							continue;
						case 'j':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_JOINFLOOD, "CMODE_JOINFLOOD", 'j', true));
							continue;
						case 'l':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, "CMODE_LIMIT", 'l', true));
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
						case 'A':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_ALLINVITE, "CMODE_ALLINVITE", 'A'));
							continue;
						case 'B':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCAPS, "CMODE_BLOCKCAPS", 'B'));
							continue;
						case 'C':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOCTCP, "CMODE_NOCTCP", 'C'));
							continue;
						case 'D':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_DELAYEDJOIN, "CMODE_DELAYEDJOIN", 'D'));
							continue;
						case 'G':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_FILTER, "CMODE_FILTER", 'G'));
							continue;
						case 'K':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKNOCK, "CMODE_NOKNOCK", 'K'));
							continue;
						case 'M':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_REGMODERATED, "CMODE_REGMODERATED", 'M'));
							continue;
						case 'N':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NONICK, "CMODE_NONICK", 'N'));
							continue;
						case 'O':
							ModeManager::AddChannelMode(new ChannelModeOper('O'));
							continue;
						case 'P':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_PERM, "CMODE_PERM", 'P'));
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
						case 'T':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NONOTICE, "CMODE_NONOTICE", 'T'));
							continue;
						case 'c':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCOLOR, "CMODE_BLOCKCOLOR", 'c'));
							continue;
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
						case 'r':
							ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
							continue;
						case 's':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, "CMODE_SECRET", 's'));
							continue;
						case 't':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, "CMODE_TOPIC", 't'));
							continue;
						case 'u':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_AUDITORIUM, "CMODE_AUDITORIUM", 'u'));
							continue;
						case 'z':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_SSL, "CMODE_SSL", 'z'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelMode(CMODE_END, "", modebuf[t]));
					}
				}
			}
			else if (capab.find("USERMODES") != Anope::string::npos)
			{
				Anope::string modes(capab.begin() + 10, capab.end());
				commasepstream sep(modes);
				Anope::string modebuf;

				while (sep.GetToken(modebuf))
				{
					for (size_t t = 0, end = modebuf.length(); t < end; ++t)
					{
						switch (modebuf[t])
						{
							case 'h':
								ModeManager::AddUserMode(new UserMode(UMODE_HELPOP, "UMODE_HELPOP", 'h'));
								continue;
							case 'B':
								ModeManager::AddUserMode(new UserMode(UMODE_BOT, "UMODE_BOT", 'B'));
								continue;
							case 'G':
								ModeManager::AddUserMode(new UserMode(UMODE_FILTER, "UMODE_FILTER", 'G'));
								continue;
							case 'H':
								ModeManager::AddUserMode(new UserMode(UMODE_HIDEOPER, "UMODE_HIDEOPER", 'H'));
								continue;
							case 'I':
								ModeManager::AddUserMode(new UserMode(UMODE_PRIV, "UMODE_PRIV", 'I'));
								continue;
							case 'Q':
								ModeManager::AddUserMode(new UserMode(UMODE_HIDDEN, "UMODE_HIDDEN", 'Q'));
								continue;
							case 'R':
								ModeManager::AddUserMode(new UserMode(UMODE_REGPRIV, "UMODE_REGPRIV", 'R'));
								continue;
							case 'S':
								ModeManager::AddUserMode(new UserMode(UMODE_STRIPCOLOR, "UMODE_STRIPCOLOR", 'S'));
								continue;
							case 'W':
								ModeManager::AddUserMode(new UserMode(UMODE_WHOIS, "UMODE_WHOIS", 'W'));
								continue;
							case 'c':
								ModeManager::AddUserMode(new UserMode(UMODE_COMMONCHANS, "UMODE_COMMONCHANS", 'c'));
								continue;
							case 'g':
								ModeManager::AddUserMode(new UserMode(UMODE_CALLERID, "UMODE_CALLERID", 'g'));
								continue;
							case 'i':
								ModeManager::AddUserMode(new UserMode(UMODE_INVIS, "UMODE_INVIS", 'i'));
								continue;
							case 'k':
								ModeManager::AddUserMode(new UserMode(UMODE_PROTECTED, "UMODE_PROTECTED", 'k'));
								continue;
							case 'o':
								ModeManager::AddUserMode(new UserMode(UMODE_OPER, "UMODE_OPER", 'o'));
								continue;
							case 'r':
								ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, "UMODE_REGISTERED", 'r'));
								continue;
							case 'w':
								ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, "UMODE_WALLOPS", 'w'));
								continue;
							case 'x':
								ModeManager::AddUserMode(new UserMode(UMODE_CLOAK, "UMODE_CLOAK", 'x'));
								continue;
							case 'd':
								ModeManager::AddUserMode(new UserMode(UMODE_DEAF, "UMODE_DEAF", 'd'));
								continue;
							default:
								ModeManager::AddUserMode(new UserMode(UMODE_END, "", modebuf[t]));
						}
					}
				}
			}
			else if (capab.find("PREFIX=(") != Anope::string::npos)
			{
				Anope::string modes(capab.begin() + 8, capab.begin() + capab.find(')'));
				Anope::string chars(capab.begin() + capab.find(')') + 1, capab.end());

				for (size_t t = 0, end = modes.length(); t < end; ++t)
				{
					switch (modes[t])
					{
						case 'q':
							ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, "CMODE_OWNER", 'q', chars[t]));
							continue;
						case 'a':
							ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT, "CMODE_PROTECT", 'a', chars[t]));
							continue;
						case 'o':
							ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, "CMODE_OP", 'o', chars[t]));
							continue;
						case 'h':
							ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_HALFOP, "CMODE_HALFOP", 'h', chars[t]));
							continue;
						case 'v':
							ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, "CMODE_VOICE", 'v', chars[t]));
							continue;
					}
				}
			}
			else if (capab.find("MAXMODES=") != Anope::string::npos)
			{
				Anope::string maxmodes(capab.begin() + 9, capab.end());
				ircd->maxmodes = maxmodes.is_number_only() ? convertTo<unsigned>(maxmodes) : 3;
			}
		}
	}
	else if (!strcasecmp(av[0], "END"))
	{
		if (!has_globopsmod)
		{
			send_cmd("", "ERROR :m_globops is not loaded. This is required by Anope");
			quitmsg = "Remote server does not have the m_globops module loaded, and this is required.";
			quitting = true;
			return MOD_STOP;
		}
		if (!has_servicesmod)
		{
			send_cmd("", "ERROR :m_services_account.so is not loaded. This is required by Anope");
			quitmsg = "ERROR: Remote server does not have the m_services_account module loaded, and this is required.";
			quitting = true;
			return MOD_STOP;
		}
		if (!has_hidechansmod)
		{
			send_cmd("", "ERROR :m_hidechans.so is not loaded. This is required by Anope");
			quitmsg = "ERROR: Remote server does not have the m_hidechans module loaded, and this is required.";
			quitting = true;
			return MOD_STOP;
		}
		if (!has_svsholdmod)
			ircdproto->SendGlobops(OperServ, "SVSHOLD missing, Usage disabled until module is loaded.");
		if (!has_chghostmod)
			ircdproto->SendGlobops(OperServ, "CHGHOST missing, Usage disabled until module is loaded.");
		if (!has_chgidentmod)
			ircdproto->SendGlobops(OperServ, "CHGIDENT missing, Usage disabled until module is loaded.");
		ircd->svshold = has_svsholdmod;
	}

	CapabParse(ac, av);

	return MOD_CONT;
}

int anope_event_endburst(const Anope::string &source, int ac, const char **av)
{
	NickAlias *na;
	User *u = prev_u_intro;
	Server *s = Server::Find(source);

	if (!s)
		throw new CoreException("Got ENDBURST without a source");

	/* Check if the previously introduced user was Id'd for the nickgroup of the nick he s currently using.
	 * If not, validate the user. ~ Viper*/
	prev_u_intro = NULL;
	if (u)
		na = findnick(u->nick);
	if (u && !u->server->IsSynced() && (!na || na->nc != u->Account()))
	{
		validate_user(u);
		if (u->HasMode(UMODE_REGISTERED))
			u->RemoveMode(NickServ, UMODE_REGISTERED);
	}

	Alog() << "Processed ENDBURST for " << s->GetName();

	s->Sync(true);
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
	Anope::AddMessage("UID", anope_event_uid);
	Anope::AddMessage("CAPAB", anope_event_capab);
	Anope::AddMessage("PART", anope_event_part);
	Anope::AddMessage("PING", anope_event_ping);
	Anope::AddMessage("TIME", anope_event_time);
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
	Anope::AddMessage("METADATA", anope_event_metadata);
}

bool ChannelModeFlood::IsValid(const Anope::string &value) const
{
	Anope::string rest;
	if (!value.empty() && value[0] != ':' && convertTo<int>(value[0] == '*' ? value.substr(1) : value, rest, false) > 0 && rest[0] == ':' && rest.length() > 1 && convertTo<int>(rest.substr(1), rest, false) > 0 && rest.empty())
		return true;
	else
		return false;
}

class ProtoInspIRCd : public Module
{
 public:
	ProtoInspIRCd(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(PROTOCOL);

		if (!Config->Numeric.empty())
			TS6SID = Config->Numeric;

		pmodule_ircd_version("InspIRCd 1.2");
		pmodule_ircd_var(myIrcd);
		pmodule_ircd_useTSMode(0);

		CapabType c[] = { CAPAB_NOQUIT, CAPAB_SSJ3, CAPAB_NICK2, CAPAB_VL, CAPAB_TLKEXT };
		for (unsigned i = 0; i < 5; ++i)
			Capab.SetFlag(c[i]);

		pmodule_ircd_proto(&ircd_proto);
		moduleAddIRCDMsgs();

		ModuleManager::Attach(I_OnUserNickChange, this);
	}

	void OnUserNickChange(User *u, const Anope::string &)
	{
		/* InspIRCd 1.2 doesn't set -r on nick change, remove -r here. Note that if we have to set +r later
		 * this will cancel out this -r, resulting in no mode changes.
		 */
		u->RemoveMode(NickServ, UMODE_REGISTERED);
	}
};

MODULE_INIT(ProtoInspIRCd)
