/* Ratbox IRCD functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"

static Anope::string TS6UPLINK; // XXX is this needed?

IRCDVar myIrcd[] = {
	{"Ratbox 2.0+",	/* ircd name */
	 "+oi",			/* Modes used by pseudoclients */
	 2,				/* Chan Max Symbols */
	 "+o",			/* Channel Umode used by Botserv bots */
	 0,				/* SVSNICK */
	 0,				/* Vhost */
	 1,				/* Supports SNlines */
	 1,				/* Supports SQlines */
	 0,				/* Supports SZlines */
	 3,				/* Number of server args */
	 1,				/* Join 2 Set */
	 1,				/* Join 2 Message */
	 0,				/* TS Topic Forward */
	 0,				/* TS Topci Backward */
	 1,				/* Chan SQlines */
	 0,				/* Quit on Kill */
	 0,				/* SVSMODE unban */
	 0,				/* Reverse */
	 0,				/* vidents */
	 0,				/* svshold */
	 0,				/* time stamp on mode */
	 0,				/* NICKIP */
	 0,				/* UMODE */
	 0,				/* O:LINE */
	 0,				/* VHOST ON NICK */
	 0,				/* Change RealName */
	 0,				/* No Knock requires +i */
	 0,				/* We support TOKENS */
	 0,				/* Can remove User Channel Modes with SVSMODE */
	 0,				/* Sglines are not enforced until user reconnects */
	 1,				/* ts6 */
	 0,				/* p10 */
	 0,				/* CIDR channelbans */
	 "$$",			/* TLD Prefix for Global */
	 4,				/* Max number of modes we can send per line */
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
	send_cmd("", "SVINFO 6 3 0 :%ld", static_cast<long>(time(NULL)));
}

void ratbox_cmd_svsinfo()
{
}

/* CAPAB */
/*
  QS     - Can handle quit storm removal
  EX     - Can do channel +e exemptions
  CHW    - Can do channel wall @#
  LL     - Can do lazy links
  IE     - Can do invite exceptions
  EOB    - Can do EOB message
  KLN    - Can do KLINE message
  GLN    - Can do GLINE message
  HUB    - This server is a HUB
  UID    - Can do UIDs
  ZIP    - Can do ZIPlinks
  ENC    - Can do ENCrypted links
  KNOCK  -  supports KNOCK
  TBURST - supports TBURST
  PARA   - supports invite broadcasting for +p
  ENCAP  - ?
*/
void ratbox_cmd_capab()
{
	send_cmd("", "CAPAB :QS EX CHW IE KLN GLN KNOCK TB UNKLN CLUSTER ENCAP");
}

/* PASS */
void ratbox_cmd_pass(const Anope::string &pass)
{
	send_cmd("", "PASS %s TS 6 :%s", pass.c_str(), TS6SID.c_str());
}

class RatboxProto : public IRCDProto
{
	void SendGlobopsInternal(BotInfo *source, const Anope::string &buf)
	{
		if (source)
			send_cmd(source->GetUID(), "OPERWALL :%s", buf.c_str());
		else
			send_cmd(TS6SID, "OPERWALL :%s", buf.c_str());
	}

	void SendSQLine(const XLine *x)
	{
		send_cmd(TS6SID, "RESV * %s :%s", x->Mask.c_str(), x->Reason.c_str());
	}

	void SendSGLineDel(const XLine *x)
	{
		BotInfo *bi = OperServ;
		send_cmd(bi ? bi->GetUID() : Config->s_OperServ, "UNXLINE * %s", x->Mask.c_str());
	}

	void SendSGLine(const XLine *x)
	{
		BotInfo *bi = OperServ;
		send_cmd(bi ? bi->GetUID() : Config->s_OperServ, "XLINE * %s 0 :%s", x->Mask.c_str(), x->Reason.c_str());
	}

	void SendAkillDel(const XLine *x)
	{
		BotInfo *bi = OperServ;
		send_cmd(bi ? bi->GetUID() : Config->s_OperServ, "UNKLINE * %s %s", x->GetUser().c_str(), x->GetHost().c_str());
	}

	void SendSQLineDel(const XLine *x)
	{
		send_cmd(TS6SID, "UNRESV * %s", x->Mask.c_str());
	}

	void SendJoin(const BotInfo *user, const Anope::string &channel, time_t chantime)
	{
		send_cmd("", "SJOIN %ld %s + :%s", static_cast<long>(chantime), channel.c_str(), user->GetUID().c_str());
	}

	void SendJoin(const BotInfo *user, const ChannelContainer *cc)
	{
		send_cmd("", "SJOIN %ld %s +%s :%s%s", static_cast<long>(cc->chan->creation_time), cc->chan->name.c_str(), cc->chan->GetModes(true, true).c_str(), cc->Status->BuildModePrefixList().c_str(), user->GetUID().c_str());
	}

	void SendAkill(const XLine *x)
	{
		BotInfo *bi = OperServ;
		send_cmd(bi ? bi->GetUID() : Config->s_OperServ, "KLINE * %ld %s %s :%s", static_cast<long>(x->Expires - time(NULL)), x->GetUser().c_str(), x->GetHost().c_str(), x->Reason.c_str());
	}

	void SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf)
	{
		send_cmd(source ? source->GetUID() : TS6SID, "KILL %s :%s", user->GetUID().c_str(), buf.c_str());
	}

	void SendSVSMode(const User *u, int ac, const char **av)
	{
		this->SendModeInternal(NULL, u, merge_args(ac, av));
	}

	/* SERVER name hop descript */
	void SendServer(const Server *server)
	{
		send_cmd("", "SERVER %s %d :%s", server->GetName().c_str(), server->GetHops(), server->GetDescription().c_str());
	}

	void SendConnect()
	{
		ratbox_cmd_pass(uplink_server->password);
		ratbox_cmd_capab();
		/* Make myself known to myself in the serverlist */
		SendServer(Me);
		ratbox_cmd_svinfo();
	}

	void SendClientIntroduction(const Anope::string &nick, const Anope::string &user, const Anope::string &host, const Anope::string &real, const Anope::string &modes, const Anope::string &uid)
	{
		EnforceQlinedNick(nick, "");
		send_cmd(TS6SID, "UID %s 1 %ld %s %s %s 0 %s :%s", nick.c_str(), static_cast<long>(time(NULL)), modes.c_str(), user.c_str(), host.c_str(), uid.c_str(), real.c_str());
	}

	void SendPartInternal(const BotInfo *bi, const Channel *chan, const Anope::string &buf)
	{
		if (!buf.empty())
			send_cmd(bi->GetUID(), "PART %s :%s", chan->name.c_str(), buf.c_str());
		else
			send_cmd(bi->GetUID(), "PART %s", chan->name.c_str());
	}

	void SendNumericInternal(const Anope::string &, int numeric, const Anope::string &dest, const Anope::string &buf)
	{
		// This might need to be set in the call to SendNumeric instead of here, will review later -- CyberBotX
		send_cmd(TS6SID, "%03d %s %s", numeric, dest.c_str(), buf.c_str());
	}

	void SendModeInternal(const BotInfo *bi, const Channel *dest, const Anope::string &buf)
	{
		if (bi)
			send_cmd(bi->GetUID(), "MODE %s %s", dest->name.c_str(), buf.c_str());
		else
			send_cmd(TS6SID, "MODE %s %s", dest->name.c_str(), buf.c_str());
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf)
	{
		if (buf.empty())
			return;
		send_cmd(bi ? bi->GetUID() : TS6SID, "SVSMODE %s %s", u->nick.c_str(), buf.c_str());
	}

	void SendKickInternal(const BotInfo *bi, const Channel *chan, const User *user, const Anope::string &buf)
	{
		if (!buf.empty())
			send_cmd(bi->GetUID(), "KICK %s %s :%s", chan->name.c_str(), user->GetUID().c_str(), buf.c_str());
		else
			send_cmd(bi->GetUID(), "KICK %s %s", chan->name.c_str(), user->GetUID().c_str());
	}

	void SendNoticeChanopsInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf)
	{
		send_cmd("", "NOTICE @%s :%s", dest->name.c_str(), buf.c_str());
	}

	/* QUIT */
	void SendQuitInternal(BotInfo *bi, const Anope::string &buf)
	{
		if (!buf.empty())
			send_cmd(bi->GetUID(), "QUIT :%s", buf.c_str());
		else
			send_cmd(bi->GetUID(), "QUIT");
	}

	/* INVITE */
	void SendInvite(BotInfo *source, const Anope::string &chan, const Anope::string &nick)
	{
		User *u = finduser(nick);
		send_cmd(source->GetUID(), "INVITE %s %s", u ? u->GetUID().c_str() : nick.c_str(), chan.c_str());
	}

	void SendAccountLogin(const User *u, const NickCore *account)
	{
		send_cmd(TS6SID, "ENCAP * SU %s %s", u->GetUID().c_str(), account->display.c_str());
	}

	void SendAccountLogout(const User *u, const NickCore *account)
	{
		send_cmd(TS6SID, "ENCAP * SU %s", u->GetUID().c_str());
	}

	bool IsNickValid(const Anope::string &nick)
	{
		/* TS6 Save extension -Certus */
		if (isdigit(nick[0]))
			return false;

		return true;
	}

	void SendTopic(const BotInfo *bi, const Channel *c, const Anope::string &, const Anope::string &topic)
	{
		send_cmd(bi->GetUID(), "TOPIC %s :%s", c->name.c_str(), topic.c_str());
	}

	void SetAutoIdentificationToken(User *u)
	{

		if (!u->Account())
			return;

		Anope::string svidbuf = stringify(u->timestamp);

		u->Account()->Shrink("authenticationtoken");
		u->Account()->Extend("authenticationtoken", new ExtensibleItemRegular<Anope::string>(svidbuf));
	}
} ircd_proto;

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

	/* Channel is done syncing */
	if (c->HasFlag(CH_SYNCING))
	{
		/* Unset the syncing flag */
		c->UnsetFlag(CH_SYNCING);
		c->Sync();
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
int anope_event_nick(const Anope::string &source, int ac, const char **av)
{
	User *user;

	if (ac == 9)
	{
		Server *s = Server::Find(source);
		/* Source is always the server */
		user = do_nick("", av[0], av[4], av[5], s->GetName(), av[8], Anope::string(av[2]).is_pos_number_only() ? convertTo<time_t>(av[2]) : 0, 0, "*", av[7]);
		if (user)
		{
			UserSetInternalModes(user, 1, &av[3]);

			NickAlias *na = findnick(user->nick);
			Anope::string svidbuf;
			if (na && na->nc->GetExtRegular("authenticationtoken", svidbuf) && svidbuf == av[2])
			{
				user->Login(na->nc);
			}
			else
				validate_user(user);
		}
	}
	else if (ac == 2)
		do_nick(source, av[0], "", "", "", "", Anope::string(av[1]).is_pos_number_only() ? convertTo<time_t>(av[1]) : 0, 0, "", "");
	return MOD_CONT;
}

int anope_event_topic(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac == 4)
		do_topic(source, ac, av);
	else
	{
		Channel *c = findchan(av[0]);
		time_t topic_time = time(NULL);

		if (!c)
		{
			Log(LOG_DEBUG) << "TOPIC " << merge_args(ac - 1, av + 1) << " for nonexistent channel " << av[0];
			return MOD_CONT;
		}

		if (check_topiclock(c, topic_time))
			return MOD_CONT;

		c->topic.clear();
		if (ac > 1 && *av[1])
			c->topic = av[1];

		u = finduser(source);
		c->topic_setter = u ? u->nick : source;
		c->topic_time = topic_time;

		record_topic(av[0]);

		if (ac > 1 && *av[1])
		{
			FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(c, av[1]));
		}
		else
		{
			FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(c, ""));
		}
	}
	return MOD_CONT;
}

int anope_event_tburst(const Anope::string &source, int ac, const char **av)
{
	Channel *c;
	time_t topic_time;

	if (ac != 4)
		return MOD_CONT;

	Anope::string setter = myStrGetToken(av[2], '!', 0);

	c = findchan(av[0]);
	topic_time = Anope::string(av[1]).is_pos_number_only() ? convertTo<time_t>(av[1]) : 0;

	if (!c)
	{
		Log(LOG_DEBUG) << "debug: TOPIC " << merge_args(ac - 1, av + 1) << " for nonexistent channel " << av[0];
		return MOD_CONT;
	}

	if (check_topiclock(c, topic_time))
		return MOD_CONT;

	c->topic.clear();
	if (ac > 1 && *av[3])
		c->topic = av[3];

	c->topic_setter = setter;
	c->topic_time = topic_time;

	record_topic(av[0]);
	return MOD_CONT;
}

int anope_event_436(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;

	m_nickcoll(av[0]);
	return MOD_CONT;
}

int anope_event_ping(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	ircdproto->SendPong(ac > 1 ? av[1] : Config->ServerName, av[0]);
	return MOD_CONT;
}

int anope_event_away(const Anope::string &source, int ac, const char **av)
{
	User *u = NULL;

	u = finduser(source);
	m_away(u ? u->nick : source, ac ? av[0] : "");
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
	{
		anope_event_sjoin(source, ac, av);
		return MOD_CONT;
	}
	else
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

int anope_event_privmsg(const Anope::string &source, int ac, const char **av)
{
	if (ac != 2)
		return MOD_CONT;

	// XXX: this is really the same as charybdis
	m_privmsg(source, av[0], av[1]);
	return MOD_CONT;
}

int anope_event_part(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac < 1 || ac > 2)
		return MOD_CONT;

	u = finduser(source);
	do_part(u ? u->nick : source, ac, av);

	return MOD_CONT;
}

int anope_event_whois(const Anope::string &source, int ac, const char **av)
{
	if (!source.empty() && ac >= 1)
	{
		BotInfo *bi = findbot(av[0]);
		m_whois(source, bi->GetUID());
	}
	return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(const Anope::string &source, int ac, const char **av)
{
	if (!stricmp(av[1], "1"))
		do_server(source, av[0], Anope::string(av[1]).is_pos_number_only() ? convertTo<unsigned>(av[1]) : 0, av[2], TS6UPLINK);
	else
		do_server(source, av[0], Anope::string(av[1]).is_pos_number_only() ? convertTo<unsigned>(av[1]) : 0, av[2], "");
	return MOD_CONT;
}

int anope_event_sid(const Anope::string &source, int ac, const char **av)
{
	/* :42X SID trystan.nomadirc.net 2 43X :ircd-ratbox test server */
	Server *s = Server::Find(source);

	do_server(s->GetName(), av[0], Anope::string(av[1]).is_pos_number_only() ? convertTo<unsigned>(av[1]) : 0, av[3], av[2]);
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
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = finduser(source);

	do_quit(u ? u->nick : source, ac, av);
	return MOD_CONT;
}

int anope_event_mode(const Anope::string &source, int ac, const char **av)
{
	User *u, *u2;

	if (ac < 2)
		return MOD_CONT;

	if (*av[0] == '#' || *av[0] == '&')
		do_cmode(source, ac, av);
	else
	{
		u = finduser(source);
		u2 = finduser(av[0]);
		av[0] = u2->nick.c_str();
		do_umode(u->nick, ac, av);
	}
	return MOD_CONT;
}

int anope_event_tmode(const Anope::string &source, int ac, const char **av)
{
	if (*av[1] == '#' || *av[1] == '&')
		do_cmode(source, ac, av);
	return MOD_CONT;
}

/* Event: PROTOCTL */
int anope_event_capab(const Anope::string &source, int ac, const char **av)
{
	CapabParse(ac, av);
	return MOD_CONT;
}

int anope_event_pass(const Anope::string &source, int ac, const char **av)
{
	TS6UPLINK = av[3];
	return MOD_CONT;
}

int anope_event_bmask(const Anope::string &source, int ac, const char **av)
{
	Channel *c;
	ChannelModeList *cms;

	/* :42X BMASK 1106409026 #ircops b :*!*@*.aol.com */
	/*            0          1       2  3             */
	c = findchan(av[1]);

	if (c)
	{
		Anope::string bans = av[3];
		int count = myNumToken(bans, ' '), i;
		for (i = 0; i <= count - 1; ++i)
		{
			Anope::string b = myStrGetToken(bans, ' ', i);
			if (!stricmp(av[2], "b"))
			{
				cms = debug_cast<ChannelModeList *>(ModeManager::FindChannelModeByChar('b'));
				cms->AddMask(c, b);
			}
			if (!stricmp(av[2], "e"))
			{
				cms = debug_cast<ChannelModeList *>(ModeManager::FindChannelModeByChar('e'));
				cms->AddMask(c, b);
			}
			if (!stricmp(av[2], "I"))
			{
				cms = debug_cast<ChannelModeList *>(ModeManager::FindChannelModeByChar('I'));
				cms->AddMask(c, b);
			}
		}
	}
	return MOD_CONT;
}

int anope_event_error(const Anope::string &source, int ac, const char **av)
{
	if (ac >= 1)
		Log(LOG_DEBUG) << av[0];
	return MOD_CONT;
}

void moduleAddIRCDMsgs()
{
	Anope::AddMessage("436", anope_event_436);
	Anope::AddMessage("AWAY", anope_event_away);
	Anope::AddMessage("JOIN", anope_event_join);
	Anope::AddMessage("KICK", anope_event_kick);
	Anope::AddMessage("KILL", anope_event_kill);
	Anope::AddMessage("MODE", anope_event_mode);
	Anope::AddMessage("TMODE", anope_event_tmode);
	Anope::AddMessage("MOTD", anope_event_motd);
	Anope::AddMessage("NICK", anope_event_nick);
	Anope::AddMessage("BMASK", anope_event_bmask);
	Anope::AddMessage("UID", anope_event_nick);
	Anope::AddMessage("PART", anope_event_part);
	Anope::AddMessage("PASS", anope_event_pass);
	Anope::AddMessage("PING", anope_event_ping);
	Anope::AddMessage("PRIVMSG", anope_event_privmsg);
	Anope::AddMessage("QUIT", anope_event_quit);
	Anope::AddMessage("SERVER", anope_event_server);
	Anope::AddMessage("SQUIT", anope_event_squit);
	Anope::AddMessage("TOPIC", anope_event_topic);
	Anope::AddMessage("TB", anope_event_tburst);
	Anope::AddMessage("WHOIS", anope_event_whois);
	Anope::AddMessage("CAPAB", anope_event_capab);
	Anope::AddMessage("SJOIN", anope_event_sjoin);
	Anope::AddMessage("ERROR", anope_event_error);
	Anope::AddMessage("SID", anope_event_sid);
}

static void AddModes()
{
	/* Add user modes */
	ModeManager::AddUserMode(new UserMode(UMODE_ADMIN, "UMODE_ADMIN", 'a'));
	ModeManager::AddUserMode(new UserMode(UMODE_INVIS, "UMODE_INVIS", 'i'));
	ModeManager::AddUserMode(new UserMode(UMODE_OPER, "UMODE_OPER", 'o'));
	ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, "UMODE_SNOMASK", 's'));
	ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, "UMODE_WALLOPS", 'w'));

	/* b/e/I */
	ModeManager::AddChannelMode(new ChannelModeBan('b'));
	ModeManager::AddChannelMode(new ChannelModeExcept('e'));
	ModeManager::AddChannelMode(new ChannelModeInvex('I'));

	/* v/h/o/a/q */
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, "CMODE_VOICE", 'v', '+'));
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, "CMODE_OP", 'o', '@'));

	/* Add channel modes */
	ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, "CMODE_INVITE", 'i'));
	ModeManager::AddChannelMode(new ChannelModeKey('k'));
	ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, "CMODE_LIMIT", 'l'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, "CMODE_MODERATED", 'm'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, "CMODE_NOEXTERNAL", 'n'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, "CMODE_PRIVATE", 'p'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, "CMODE_SECRET", 's'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, "CMODE_TOPIC", 't'));
}

class ProtoRatbox : public Module
{
 public:
	ProtoRatbox(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(PROTOCOL);

		if (!Config->Numeric.empty())
			TS6SID = Config->Numeric;
		UseTSMODE = 1;  /* TMODE */

		pmodule_ircd_version("Ratbox IRCD 2.0+");
		pmodule_ircd_var(myIrcd);
		pmodule_ircd_useTSMode(1);

		CapabType c[] = { CAPAB_ZIP, CAPAB_TS5, CAPAB_QS, CAPAB_UID, CAPAB_KNOCK };
		for (unsigned i = 0; i < 5; ++i)
			Capab.SetFlag(c[i]);

		AddModes();

		pmodule_ircd_proto(&ircd_proto);
		moduleAddIRCDMsgs();
	}
};

MODULE_INIT(ProtoRatbox)
