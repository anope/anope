/* Bahamut functions
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
	{"Bahamut 1.8.x",	/* ircd name */
	 "+",				/* Modes used by pseudoclients */
	 2,					/* Chan Max Symbols */
	 "+o",				/* Channel Umode used by Botserv bots */
	 1,					/* SVSNICK */
	 0,					/* Vhost */
	 1,					/* Supports SNlines */
	 1,					/* Supports SQlines */
	 1,					/* Supports SZlines */
	 3,					/* Number of server args */
	 0,					/* Join 2 Set */
	 0,					/* Join 2 Message */
	 0,					/* TS Topic Forward */
	 0,					/* TS Topci Backward */
	 1,					/* Chan SQlines */
	 1,					/* Quit on Kill */
	 1,					/* SVSMODE unban */
	 0,					/* Reverse */
	 0,					/* vidents */
	 1,					/* svshold */
	 1,					/* time stamp on mode */
	 1,					/* NICKIP */
	 0,					/* O:LINE */
	 1,					/* UMODE */
	 0,					/* VHOST ON NICK */
	 0,					/* Change RealName */
	 1,					/* No Knock requires +i */
	 0,					/* We support TOKENS */
	 0,					/* Can remove User Channel Modes with SVSMODE */
	 0,					/* Sglines are not enforced until user reconnects */
	 0,					/* ts6 */
	 0,					/* p10 */
	 0,					/* CIDR channelbans */
	 "$",				/* TLD Prefix for Global */
	 6,					/* Max number of modes we can send per line */
	 }
	,
	{NULL}
};

void bahamut_cmd_burst()
{
	send_cmd("", "BURST");
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
	send_cmd("", "SVINFO 3 1 0 :%ld", static_cast<long>(time(NULL)));
}

/* PASS */
void bahamut_cmd_pass(const Anope::string &pass)
{
	send_cmd("", "PASS %s :TS", pass.c_str());
}

/* CAPAB */
void bahamut_cmd_capab()
{
	send_cmd("", "CAPAB SSJOIN NOQUIT BURST UNCONNECT NICKIP TSMODE TS3");
}

class BahamutIRCdProto : public IRCDProto
{
	void SendModeInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf)
	{
		if (buf.empty())
			return;
		if (Capab.HasFlag(CAPAB_TSMODE))
			send_cmd(source->nick, "MODE %s 0 %s", dest->name.c_str(), buf.c_str());
		else
			send_cmd(source->nick, "MODE %s %s", dest->name.c_str(), buf.c_str());
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf)
	{
		if (buf.empty())
			return;
		send_cmd(bi ? bi->nick : Config.ServerName, "SVSMODE %s %ld %s", u->nick.c_str(), static_cast<long>(u->timestamp), buf.c_str());
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick)
	{
		send_cmd(Config.ServerName, "SVSHOLD %s %u :Being held for registered user", nick.c_str(), static_cast<unsigned>(Config.NSReleaseTimeout));
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick)
	{
		send_cmd(Config.ServerName, "SVSHOLD %s 0", nick.c_str());
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
			send_cmd(Config.ServerName, "SVSMODE %s %s %s", c->name.c_str(), mode.c_str(), nick.c_str());
		else
			send_cmd(Config.ServerName, "SVSMODE %s %s", c->name.c_str(), mode.c_str());
	}

	/* SQLINE */
	void SendSQLine(const XLine *x)
	{
		send_cmd("", "SQLINE %s :%s", x->Mask.c_str(), x->Reason.c_str());
	}

	/* UNSLINE */
	void SendSGLineDel(const XLine *x)
	{
		send_cmd("", "UNSGLINE 0 :%s", x->Mask.c_str());
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x)
	{
		/* this will likely fail so its only here for legacy */
		send_cmd("", "UNSZLINE 0 %s", x->Mask.c_str());
		/* this is how we are supposed to deal with it */
		send_cmd("", "RAKILL %s *", x->Mask.c_str());
	}

	/* SZLINE */
	void SendSZLine(const XLine *x)
	{
		/* this will likely fail so its only here for legacy */
		send_cmd("", "SZLINE %s :%s", x->Mask.c_str(), x->Reason.c_str());
		/* this is how we are supposed to deal with it */
		send_cmd("", "AKILL %s * %d %s %ld :%s", x->Mask.c_str(), 172800, x->By.c_str(), static_cast<long>(time(NULL)), x->Reason.c_str());
	}

	/* SVSNOOP */
	void SendSVSNOOP(const Anope::string &server, int set)
	{
		send_cmd("", "SVSNOOP %s %s", server.c_str(), set ? "+" : "-");
	}

	/* SGLINE */
	void SendSGLine(const XLine *x)
	{
		send_cmd("", "SGLINE %d :%s:%s", static_cast<int>(x->Mask.length()), x->Mask.c_str(), x->Reason.c_str());
	}

	/* RAKILL */
	void SendAkillDel(const XLine *x)
	{
		send_cmd("", "RAKILL %s %s", x->GetHost().c_str(), x->GetUser().c_str());
	}

	/* TOPIC */
	void SendTopic(const BotInfo *whosets, const Channel *c, const Anope::string &whosetit, const Anope::string &topic)
	{
		send_cmd(whosets->nick, "TOPIC %s %s %lu :%s", c->name.c_str(), whosetit.c_str(), static_cast<unsigned long>(c->topic_time), topic.c_str());
	}

	/* UNSQLINE */
	void SendSQLineDel(const XLine *x)
	{
		send_cmd("", "UNSQLINE %s", x->Mask.c_str());
	}

	/* JOIN - SJOIN */
	void SendJoin(const BotInfo *user, const Anope::string &channel, time_t chantime)
	{
		send_cmd(user->nick, "SJOIN %ld %s", static_cast<long>(chantime), channel.c_str());
	}

	void SendAkill(const XLine *x)
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - time(NULL);
		if (timeleft > 172800)
			timeleft = 172800;
		send_cmd("", "AKILL %s %s %d %s %ld :%s", x->GetHost().c_str(), x->GetUser().c_str(), static_cast<int>(timeleft), x->By.c_str(), static_cast<long>(time(NULL)), x->Reason.c_str());
	}

	/*
	  Note: if the stamp is null 0, the below usage is correct of Bahamut
	*/
	void SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf)
	{
		send_cmd(source ? source->nick : "", "SVSKILL %s :%s", user->nick.c_str(), buf.c_str());
	}

	/* SVSMODE */
	/* parv[0] - sender
	 * parv[1] - nick
	 * parv[2] - TS (or mode, depending on svs version)
	 * parv[3] - mode (or services id if old svs version)
	 * parv[4] - optional arguement (services id)
	 */
	void SendSVSMode(const User *u, int ac, const char **av)
	{
		this->SendModeInternal(NULL, u, merge_args(ac, av));
	}

	void SendEOB()
	{
		send_cmd("", "BURST 0");
	}

	void SendNoticeChanopsInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf)
	{
		if (buf.empty())
			return;
		send_cmd("", "NOTICE @%s :%s", dest->name.c_str(), buf.c_str());
	}

	void SendKickInternal(const BotInfo *source, const Channel *chan, const User *user, const Anope::string &buf)
	{
		if (!buf.empty())
			send_cmd(source->nick, "KICK %s %s :%s", chan->name.c_str(), user->nick.c_str(), buf.c_str());
		else
			send_cmd(source->nick, "KICK %s %s", chan->name.c_str(), user->nick.c_str());
	}

	void SendClientIntroduction(const Anope::string &nick, const Anope::string &user, const Anope::string &host, const Anope::string &real, const Anope::string &modes, const Anope::string &)
	{
		EnforceQlinedNick(nick, Config.s_BotServ);
		send_cmd("", "NICK %s 1 %ld %s %s %s %s 0 0 :%s", nick.c_str(), static_cast<long>(time(NULL)), modes.c_str(), user.c_str(), host.c_str(), Config.ServerName.c_str(), real.c_str());
	}

	/* SVSMODE +d */
	/* nc_change was = 1, and there is no na->status */
	void SendUnregisteredNick(const User *u)
	{
		ircdproto->SendMode(NickServ, u, "+d 1");
	}

	/* SERVER */
	void SendServer(const Server *server)
	{
		send_cmd("", "SERVER %s %d :%s", server->GetName().c_str(), server->GetHops(), server->GetDescription().c_str());
	}

	void SendConnect()
	{
		bahamut_cmd_pass(uplink_server->password);
		bahamut_cmd_capab();
		SendServer(Me);
		bahamut_cmd_svinfo();
		bahamut_cmd_burst();
	}

	void SetAutoIdentificationToken(User *u)
	{
		if (!u->Account())
			return;

		srand(time(NULL));
		Anope::string svidbuf = stringify(rand());

		u->Account()->Shrink("authenticationtoken");
		u->Account()->Extend("authenticationtoken", new ExtensibleItemRegular<Anope::string>(svidbuf));

		BotInfo *bi = NickServ;
		u->SetMode(bi, UMODE_REGISTERED);
		ircdproto->SendMode(bi, u, "+d %s", svidbuf.c_str());
	}
} ircd_proto;

/* EVENT: SJOIN */
int anope_event_sjoin(const Anope::string &source, int ac, const char **av)
{
	Channel *c = findchan(av[1]);
	time_t ts = Anope::string(av[0]).is_number_only() ? convertTo<time_t>(av[0]) : 0;
	bool was_created = false;
	bool keep_their_modes = false;

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

	/* If we need to keep their modes, and this SJOIN string contains modes */
	if (keep_their_modes && ac >= 4)
	{
		/* Set the modes internally */
		ChanSetInternalModes(c, ac - 3, av + 2);
	}

	/* For a reason unknown to me, bahamut will send a SJOIN from the user joining a channel
	 * if the channel already existed
	 */
	if (!was_created && ac == 2)
	{
		User *u = finduser(source);
		if (!u)
			Alog(LOG_DEBUG) << "SJOIN for nonexistant user " << source << " on " << c->name;
		else
		{
			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnPreJoinChannel, OnPreJoinChannel(u, c));

			/* Add the user to the channel */
			c->JoinUser(u);

			/* Now set whatever modes this user is allowed to have on the channel */
			chan_set_correct_modes(u, c, 1);

			/* Check to see if modules want the user to join, if they do
			 * check to see if they are allowed to join (CheckKick will kick/ban them)
			 * Don't trigger OnJoinChannel event then as the user will be destroyed
			 */
			if (MOD_RESULT == EVENT_STOP && (!c->ci || !c->ci->CheckKick(u)))
			{
				FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(u, c));
			}
		}
	}
	else
	{
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
int anope_event_nick(const Anope::string &source, int ac, const char **av)
{
	User *user;

	if (ac != 2)
	{
		user = do_nick(source, av[0], av[4], av[5], av[6], av[9], Anope::string(av[2]).is_number_only() ? convertTo<time_t>(av[2]) : 0, Anope::string(av[8]).is_number_only() ? convertTo<uint32>(av[8]) : 0, "", "");
		if (user)
		{
			/* Check to see if the user should be identified because their
			 * services id matches the one in their nickcore
			 */
			user->CheckAuthenticationToken(av[7]);

			UserSetInternalModes(user, 1, &av[3]);
		}
	}
	else
		do_nick(source, av[0], "", "", "", "", Anope::string(av[1]).is_number_only() ? convertTo<time_t>(av[1]) : 0, 0, "", "");
	return MOD_CONT;
}

/* EVENT : CAPAB */
int anope_event_capab(const Anope::string &source, int ac, const char **av)
{
	CapabParse(ac, av);
	return MOD_CONT;
}

/* EVENT : OS */
int anope_event_os(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	m_privmsg(source, Config.s_OperServ, av[0]);
	return MOD_CONT;
}

/* EVENT : NS */
int anope_event_ns(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	m_privmsg(source, Config.s_NickServ, av[0]);
	return MOD_CONT;
}

/* EVENT : MS */
int anope_event_ms(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	m_privmsg(source, Config.s_MemoServ, av[0]);
	return MOD_CONT;
}

/* EVENT : HS */
int anope_event_hs(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	m_privmsg(source, Config.s_HostServ, av[0]);
	return MOD_CONT;
}

/* EVENT : CS */
int anope_event_cs(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	m_privmsg(source, Config.s_ChanServ, av[0]);
	return MOD_CONT;
}

int anope_event_436(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;

	m_nickcoll(av[0]);
	return MOD_CONT;
}

/* EVENT : SERVER */
int anope_event_server(const Anope::string &source, int ac, const char **av)
{
	do_server(source, av[0], Anope::string(av[1]).is_number_only() ? convertTo<unsigned>(av[1]) : 0, av[2], "");
	return MOD_CONT;
}

/* EVENT : PRIVMSG */
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

/* EVENT: MODE */
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

/* EVENT: KILL */
int anope_event_kill(const Anope::string &source, int ac, const char **av)
{
	if (ac != 2)
		return MOD_CONT;

	m_kill(av[0], av[1]);
	return MOD_CONT;
}

/* EVENT: KICK */
int anope_event_kick(const Anope::string &source, int ac, const char **av)
{
	if (ac != 3)
		return MOD_CONT;
	do_kick(source, ac, av);
	return MOD_CONT;
}

/* EVENT: JOIN */
int anope_event_join(const Anope::string &source, int ac, const char **av)
{
	if (ac != 1)
		return MOD_CONT;
	do_join(source, ac, av);
	return MOD_CONT;
}

/* EVENT: MOTD */
int anope_event_motd(const Anope::string &source, int ac, const char **av)
{
	if (source.empty())
		return MOD_CONT;

	m_motd(source);
	return MOD_CONT;
}

int anope_event_away(const Anope::string &source, int ac, const char **av)
{
	if (source.empty())
		return MOD_CONT;
	m_away(source, ac ? av[0] : "");
	return MOD_CONT;
}

int anope_event_ping(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	ircdproto->SendPong(ac > 1 ? av[1] : Config.ServerName, av[0]);
	return MOD_CONT;
}

int anope_event_error(const Anope::string &source, int ac, const char **av)
{
	if (ac >= 1)
		Alog(LOG_DEBUG) << av[0];
	return MOD_CONT;
}

int anope_event_burst(const Anope::string &source, int ac, const char **av)
{
	Server *s = Server::Find(source);

	if (!ac)
	{
		/* for future use  - start burst */
	}
	else
	{
		/* If we found a server with the given source, that one just
		 * finished bursting. If there was no source, then our uplink
		 * server finished bursting. -GD
		 */
		if (!s)
			s = Me->GetUplink();
		if (s)
			s->Sync(true);
	}
	return MOD_CONT;
}

bool ChannelModeFlood::IsValid(const Anope::string &value) const
{
	Anope::string rest;
	if (!value.empty() && value[0] != ':' && convertTo<int>(value[0] == '*' ? value.substr(1) : value, rest, false) > 0 && rest[0] == ':' && rest.length() > 1 && convertTo<int>(rest.substr(1), rest, false) > 0 && rest.empty())
		return true;

	return false;
}

void moduleAddIRCDMsgs()
{
	Anope::AddMessage("436", anope_event_436);
	Anope::AddMessage("AWAY", anope_event_away);
	Anope::AddMessage("JOIN", anope_event_join);
	Anope::AddMessage("KICK", anope_event_kick);
	Anope::AddMessage("KILL", anope_event_kill);
	Anope::AddMessage("MODE", anope_event_mode);
	Anope::AddMessage("MOTD", anope_event_motd);
	Anope::AddMessage("NICK", anope_event_nick);
	Anope::AddMessage("PART", anope_event_part);
	Anope::AddMessage("PING", anope_event_ping);
	Anope::AddMessage("PRIVMSG", anope_event_privmsg);
	Anope::AddMessage("QUIT", anope_event_quit);
	Anope::AddMessage("SERVER", anope_event_server);
	Anope::AddMessage("SQUIT", anope_event_squit);
	Anope::AddMessage("TOPIC", anope_event_topic);
	Anope::AddMessage("WHOIS", anope_event_whois);
	Anope::AddMessage("SVSMODE", anope_event_mode);
	Anope::AddMessage("CAPAB", anope_event_capab);
	Anope::AddMessage("CS", anope_event_cs);
	Anope::AddMessage("HS", anope_event_hs);
	Anope::AddMessage("MS", anope_event_ms);
	Anope::AddMessage("NS", anope_event_ns);
	Anope::AddMessage("OS", anope_event_os);
	Anope::AddMessage("SJOIN", anope_event_sjoin);
	Anope::AddMessage("ERROR", anope_event_error);
	Anope::AddMessage("BURST", anope_event_burst);
}

static void AddModes()
{
	/* Add user modes */
	ModeManager::AddUserMode(new UserMode(UMODE_SERV_ADMIN, "UMODE_SERV_ADMIN", 'A'));
	ModeManager::AddUserMode(new UserMode(UMODE_REGPRIV, "UMODE_REGPRIV", 'R'));
	ModeManager::AddUserMode(new UserMode(UMODE_ADMIN, "UMODE_ADMIN", 'a'));
	ModeManager::AddUserMode(new UserMode(UMODE_INVIS, "UMODE_INVIS", 'i'));
	ModeManager::AddUserMode(new UserMode(UMODE_OPER, "UMODE_OPER", 'o'));
	ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, "UMODE_REGISTERED", 'r'));
	ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, "UMODE_SNOMASK", 's'));
	ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, "UMODE_WALLOPS", 'w'));
	ModeManager::AddUserMode(new UserMode(UMODE_DEAF, "UMODE_DEAF", 'd'));

	/* b/e/I */
	ModeManager::AddChannelMode(new ChannelModeBan('b'));

	/* v/h/o/a/q */
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, "CMODE_VOICE", 'v', '+'));
	ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, "CMODE_OP", 'o', '@'));

	/* Add channel modes */
	ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCOLOR, "CMODE_BLOCKCOLOR", 'c'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, "CMODE_INVITE", 'i'));
	ModeManager::AddChannelMode(new ChannelModeFlood('f'));
	ModeManager::AddChannelMode(new ChannelModeKey('k'));
	ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, "CMODE_LIMIT", 'l'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, "CMODE_MODERATED", 'm'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, "CMODE_NOEXTERNAL", 'n'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, "CMODE_PRIVATE", 'p'));
	ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, "CMODE_SECRET", 's'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, "CMODE_TOPIC", 't'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_REGMODERATED, "CMODE_REGMODERATED", 'M'));
	ModeManager::AddChannelMode(new ChannelModeOper('O'));
	ModeManager::AddChannelMode(new ChannelMode(CMODE_REGISTEREDONLY, "CMODE_REGISTEREDONLY", 'R'));
}

class ProtoBahamut : public Module
{
 public:
	ProtoBahamut(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(PROTOCOL);

		pmodule_ircd_version("BahamutIRCd 1.4.*/1.8.*");
		pmodule_ircd_var(myIrcd);
		pmodule_ircd_useTSMode(0);

		CapabType c[] = { CAPAB_NOQUIT, CAPAB_TSMODE, CAPAB_UNCONNECT, CAPAB_BURST, CAPAB_DKEY, CAPAB_DOZIP };
		for (unsigned i = 0; i < 6; ++i)
			Capab.SetFlag(c[i]);

		moduleAddIRCDMsgs();
		AddModes();

		pmodule_ircd_proto(&ircd_proto);

		ModuleManager::Attach(I_OnUserNickChange, this);
	}

	void OnUserNickChange(User *u, const Anope::string &)
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
	}
};

MODULE_INIT(ProtoBahamut)
