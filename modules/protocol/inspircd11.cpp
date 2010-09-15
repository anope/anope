/* inspircd 1.1 beta 6+ functions
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
	{"InspIRCd 1.1",	/* ircd name */
	 "+I",				/* Modes used by pseudoclients */
	 5,					/* Chan Max Symbols */
	 1,					/* SVSNICK */
	 1,					/* Vhost */
	 1,					/* Supports SNlines */
	 1,					/* Supports SQlines */
	 1,					/* Supports SZlines */
	 1,					/* Join 2 Message */
	 0,					/* Chan SQlines */
	 0,					/* Quit on Kill */
	 0,					/* SVSMODE unban */
	 1,					/* Reverse */
	 1,					/* vidents */
	 1,					/* svshold */
	 0,					/* time stamp on mode */
	 1,					/* O:LINE */
	 1,					/* UMODE */
	 1,					/* VHOST ON NICK */
	 1,					/* No Knock requires +i */
	 0,					/* Can remove User Channel Modes with SVSMODE */
	 0,					/* Sglines are not enforced until user reconnects */
	 0,					/* ts6 */
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

/* CHGHOST */
void inspircd_cmd_chghost(const Anope::string &nick, const Anope::string &vhost)
{
	if (has_chghostmod)
	{
		if (nick.empty() || vhost.empty())
			return;
		send_cmd(Config->s_OperServ, "CHGHOST %s %s", nick.c_str(), vhost.c_str());
	}
	else
		ircdproto->SendGlobops(OperServ, "CHGHOST not loaded!");
}

int anope_event_idle(const Anope::string &source, int ac, const char **av)
{
	if (ac == 1)
		send_cmd(av[0], "IDLE %s %ld 0", source.c_str(), static_cast<long>(Anope::CurTime));
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
		send_cmd(Config->s_OperServ, "GLINE %s", x->Mask.c_str());
	}

	void SendTopic(BotInfo *whosets, Channel *c)
	{
		send_cmd(whosets->nick, "FTOPIC %s %lu %s :%s", c->name.c_str(), static_cast<unsigned long>(c->topic_time + 1), c->topic_setter.c_str(), c->topic.c_str());
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
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800)
			timeleft = 172800;
		send_cmd(Config->ServerName, "ADDLINE G %s %s %ld %ld :%s", x->Mask.c_str(), x->By.c_str(), static_cast<long>(Anope::CurTime), static_cast<long>(timeleft), x->Reason.c_str());
	}

	void SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf)
	{
		send_cmd(source ? source->nick : Config->ServerName, "KILL %s :%s", user->nick.c_str(), buf.c_str());
	}

	void SendSVSMode(const User *u, int ac, const char **av)
	{
		this->SendModeInternal(NULL, u, merge_args(ac, av));
	}

	void SendNumericInternal(const Anope::string &source, int numeric, const Anope::string &dest, const Anope::string &buf)
	{
		send_cmd(source, "PUSH %s ::%s %03d %s %s", dest.c_str(), source.c_str(), numeric, dest.c_str(), buf.c_str());
	}

	void SendModeInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf)
	{
		if (buf.empty())
			return;
		send_cmd(source ? source->nick : Config->s_OperServ, "FMODE %s %u %s", dest->name.c_str(), static_cast<unsigned>(dest->creation_time), buf.c_str());
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf)
	{
		if (buf.empty())
			return;
		send_cmd(bi ? bi->nick : Config->ServerName, "MODE %s %s", u->nick.c_str(), buf.c_str());
	}

	void SendClientIntroduction(const User *u, const Anope::string &modes)
	{
		send_cmd(Config->ServerName, "NICK %ld %s %s %s %s %s 0.0.0.0 :%s", u->timestamp, u->nick.c_str(), u->host.c_str(), u->host.c_str(), u->GetIdent().c_str(), modes.c_str(), u->realname.c_str());
		send_cmd(u->nick, "OPERTYPE Service");
	}

	void SendKickInternal(const BotInfo *source, const Channel *chan, const User *user, const Anope::string &buf)
	{
		if (!buf.empty())
			send_cmd(source->nick, "KICK %s %s :%s", chan->name.c_str(), user->nick.c_str(), buf.c_str());
		else
			send_cmd(source->nick, "KICK %s %s :%s", chan->name.c_str(), user->nick.c_str(), user->nick.c_str());
	}

	void SendNoticeChanopsInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf)
	{
		if (buf.empty())
			return;
		send_cmd(Config->ServerName, "NOTICE @%s :%s", dest->name.c_str(), buf.c_str());
	}

	/* SERVER services-dev.chatspike.net password 0 :Description here */
	void SendServer(const Server *server)
	{
		send_cmd(Config->ServerName, "SERVER %s %s %d :%s", server->GetName().c_str(), currentpass.c_str(), server->GetHops(), server->GetDescription().c_str());
	}

	/* JOIN */
	void SendJoin(const BotInfo *user, const Anope::string &channel, time_t chantime)
	{
		send_cmd(user->nick, "JOIN %s %ld", channel.c_str(), static_cast<long>(chantime));
	}

	void SendJoin(BotInfo *user, const ChannelContainer *cc)
	{
		SendJoin(user, cc->chan->name, cc->chan->creation_time);
		for (std::map<char, ChannelMode *>::iterator it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
		{
			if (cc->Status->HasFlag(it->second->Name))
			{
				cc->chan->SetMode(user, it->second, user->nick);
			}
		}
		cc->chan->SetModes(user, false, "%s", cc->chan->GetModes(true, true).c_str());
	}

	/* UNSQLINE */
	void SendSQLineDel(const XLine *x)
	{
		send_cmd(Config->s_OperServ, "QLINE %s", x->Mask.c_str());
	}

	/* SQLINE */
	void SendSQLine(const XLine *x)
	{
		send_cmd(Config->ServerName, "ADDLINE Q %s %s %ld 0 :%s", x->Mask.c_str(), Config->s_OperServ.c_str(), static_cast<long>(Anope::CurTime), x->Reason.c_str());
	}

	/* SQUIT */
	void SendSquit(const Anope::string &servname, const Anope::string &message)
	{
		if (servname.empty() || message.empty())
			return;
		send_cmd(Config->ServerName, "SQUIT %s :%s", servname.c_str(), message.c_str());
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
		send_cmd("", "BURST");
		send_cmd(Config->ServerName, "VERSION :Anope-%s %s :%s - (%s) -- %s", Anope::Version().c_str(), Config->ServerName.c_str(), ircd->name, Config->EncModuleList.begin()->c_str(), Anope::Build().c_str());
	}

	/* CHGIDENT */
	void inspircd_cmd_chgident(const Anope::string &nick, const Anope::string &vIdent)
	{
		if (has_chgidentmod)
		{
			if (nick.empty() || vIdent.empty())
				return;
			send_cmd(Config->s_OperServ, "CHGIDENT %s %s", nick.c_str(), vIdent.c_str());
		}
		else
			ircdproto->SendGlobops(OperServ, "CHGIDENT not loaded!");
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick)
	{
		send_cmd(Config->s_OperServ, "SVSHOLD %s %ds :Being held for registered user", nick.c_str(), static_cast<int>(Config->NSReleaseTimeout));
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick)
	{
		send_cmd(Config->s_OperServ, "SVSHOLD %s", nick.c_str());
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x)
	{
		send_cmd(Config->s_OperServ, "ZLINE %s", x->Mask.c_str());
	}

	/* SZLINE */
	void SendSZLine(const XLine *x)
	{
		send_cmd(Config->ServerName, "ADDLINE Z %s %s %ld 0 :%s", x->Mask.c_str(), x->By.c_str(), static_cast<long>(Anope::CurTime), x->Reason.c_str());
	}

	void SendSVSJoin(const Anope::string &source, const Anope::string &nick, const Anope::string &chan, const Anope::string &)
	{
		send_cmd(source, "SVSJOIN %s %s", nick.c_str(), chan.c_str());
	}

	void SendSVSPart(const Anope::string &source, const Anope::string &nick, const Anope::string &chan)
	{
		send_cmd(source, "SVSPART %s %s", nick.c_str(), chan.c_str());
	}

	void SendBOB()
	{
		send_cmd("", "BURST %ld", Anope::CurTime);
	}

	void SendEOB()
	{
		send_cmd("", "ENDBURST");
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

int anope_event_ftopic(const Anope::string &source, int ac, const char **av)
{
	/* :source FTOPIC channel ts setby :topic */
	if (ac < 4)
		return MOD_CONT;
	
	Channel *c = findchan(av[0]);
	if (!c)
	{
		Log() << "TOPIC for nonexistant channel " << av[0];
		return MOD_CONT;
	}

	c->ChangeTopicInternal(av[2], av[3], Anope::string(av[1]).is_pos_number_only() ? convertTo<time_t>(av[1]) : Anope::CurTime);

	return MOD_CONT;
}

int anope_event_mode(const Anope::string &source, int ac, const char **av)
{
	if (ac < 2)
		return MOD_CONT;

	if (*av[0] == '#' || *av[0] == '&')
		do_cmode(source, ac, av);
	else
	{
		/* InspIRCd lets opers change another
		   users modes, we have to kludge this
		   as it slightly breaks RFC1459
		 */
		if (source.equals_ci(av[0]))
			do_umode(source, ac, av);
		else
			do_umode(av[0], ac, av);
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
		time_t ts = Anope::string(av[1]).is_pos_number_only() ? convertTo<time_t>(av[1]) : 0;
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
			Log(LOG_DEBUG) << "Param: " << newav[o - 1];
		}
		++n;
	}

	return anope_event_mode(source, ac - 1, newav);
}

int anope_event_fjoin(const Anope::string &source, int ac, const char **av)
{
	Channel *c = findchan(av[0]);
	time_t ts = Anope::string(av[1]).is_pos_number_only() ? convertTo<time_t>(av[1]) : 0;
	bool keep_their_modes = true;

	if (!c)
	{
		c = new Channel(av[0], ts);
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

	spacesepstream sep(av[ac - 1]);
	Anope::string buf;
	while (sep.GetToken(buf))
	{
		std::list<ChannelMode *> Status;
		char ch;

		/* Loop through prefixes */
		while ((ch = ModeManager::GetStatusChar(buf[0])))
		{
			ChannelMode *cm = ModeManager::FindChannelModeByChar(ch);

			if (!cm)
			{
				Log() << "Received unknown mode prefix " << buf[0] << " in FJOIN string";
				buf.erase(buf.begin());
				continue;
			}

			buf.erase(buf.begin());
			if (keep_their_modes)
				Status.push_back(cm);
		}

		User *u = finduser(buf);
		if (!u)
		{
			Log(LOG_DEBUG) << "FJOIN for nonexistant user " << buf << " on " << c->name;
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

/* Events */
int anope_event_ping(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;
	ircdproto->SendPong(Config->ServerName, av[0]);
	return MOD_CONT;
}

int anope_event_436(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1)
		return MOD_CONT;

	m_nickcoll(av[0]);
	return MOD_CONT;
}

int anope_event_away(const Anope::string &source, int ac, const char **av)
{
	if (source.empty())
		return MOD_CONT;
	m_away(source, ac ? av[0] : "");
	return MOD_CONT;
}

/* Taken from hybrid.c, topic syntax is identical */

int anope_event_topic(const Anope::string &source, int ac, const char **av)
{
	Channel *c = findchan(av[0]);

	if (!c)
	{
		Log() << "TOPIC " << merge_args(ac - 1, av + 1) << " for nonexistent channel " << av[0];
		return MOD_CONT;
	}

	c->ChangeTopicInternal(source, (ac > 1 && *av[1] ? av[1] : ""), Anope::CurTime);

	return MOD_CONT;
}

int anope_event_squit(const Anope::string &source, int ac, const char **av)
{
	if (ac != 2)
		return MOD_CONT;
	do_squit(source, ac, av);
	return MOD_CONT;
}

int anope_event_rsquit(const Anope::string &source, int ac, const char **av)
{
	if (ac < 1 || ac > 3)
		return MOD_CONT;

	/* Horrible workaround to an insp bug (#) in how RSQUITs are sent - mark */
	if (ac > 1 && Config->ServerName.equals_cs(av[0]))
		do_squit(source, ac - 1, av + 1);
	else
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
	if (ac != 2)
		return MOD_CONT;
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

int anope_event_setname(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "SETNAME for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetRealname(av[0]);
	return MOD_CONT;
}

int anope_event_chgname(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac != 2)
		return MOD_CONT;

	u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "FNAME for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetRealname(av[0]);
	return MOD_CONT;
}

int anope_event_setident(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "SETIDENT for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetIdent(av[0]);
	return MOD_CONT;
}

int anope_event_chgident(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac != 2)
		return MOD_CONT;

	u = finduser(av[0]);
	if (!u)
	{
		Log(LOG_DEBUG) << "CHGIDENT for nonexistent user " << av[0];
		return MOD_CONT;
	}

	u->SetIdent(av[1]);
	return MOD_CONT;
}

int anope_event_sethost(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "SETHOST for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetDisplayedHost(av[0]);
	return MOD_CONT;
}


int anope_event_nick(const Anope::string &source, int ac, const char **av)
{
	User *user;

	if (ac != 1)
	{
		if (ac == 8)
		{
			time_t ts = Anope::string(av[0]).is_pos_number_only() ? convertTo<time_t>(av[0]) : 0;

			user = do_nick("", av[1], av[4], av[2], source, av[7], ts, av[6], av[3], "");
			if (user)
			{
				UserSetInternalModes(user, 1, &av[5]);
				user->SetCloakedHost(av[3]);

				NickAlias *na = findnick(user->nick);
				Anope::string svidbuf;
				if (na && na->nc->GetExtRegular("authenticationtoken", svidbuf) && svidbuf == av[0])
				{
					user->Login(na->nc);
					user->SetMode(NickServ, UMODE_REGISTERED);
				}
				else
					validate_user(user);
			}
		}
	}
	else
		do_nick(source, av[0], "", "", "", "", 0, "", "", "");
	return MOD_CONT;
}


int anope_event_chghost(const Anope::string &source, int ac, const char **av)
{
	User *u;

	if (ac != 1)
		return MOD_CONT;

	u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "FHOST for nonexistent user " << source;
		return MOD_CONT;
	}

	u->SetDisplayedHost(av[0]);
	return MOD_CONT;
}

/* EVENT: SERVER */
int anope_event_server(const Anope::string &source, int ac, const char **av)
{
	do_server(source, av[0], Anope::string(av[1]).is_pos_number_only() ? convertTo<unsigned>(av[1]) : 0, av[2], "");
	return MOD_CONT;
}

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
		if (strstr(av[1], "m_services.so"))
			has_servicesmod = true;
		if (strstr(av[1], "m_svshold.so"))
			has_svsholdmod = true;
		if (strstr(av[1], "m_chghost.so"))
			has_chghostmod = true;
		if (strstr(av[1], "m_chgident.so"))
			has_chgidentmod = true;
		if (strstr(av[1], "m_hidechans.so"))
			has_hidechansmod = true;
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
				for (size_t t = 0, end = modebuf.length(); t < end; ++t)
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
			else if (capab.find("PREIX=(") != Anope::string::npos)
			{
				Anope::string modes(capab.begin() + 8, capab.begin() + capab.find(')'));
				Anope::string chars(capab.begin() + capab.find(')') + 1, capab.end());

				for (size_t t = 0, end = modes.length(); t < end; ++t)
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
				Anope::string maxmodes(capab.begin() + 9, capab.end());
				ircd->maxmodes = maxmodes.is_pos_number_only() ? convertTo<unsigned>(maxmodes) : 3;
			}
		}
	}
	else if (!strcasecmp(av[0], "END"))
	{
		if (!has_globopsmod)
		{
			send_cmd("", "ERROR :m_globops is not loaded. This is required by Anope");
			quitmsg = "ERROR: Remote server does not have the m_globops module loaded, and this is required.";
			quitting = true;
			return MOD_STOP;
		}
		if (!has_servicesmod)
		{
			send_cmd("", "ERROR :m_services is not loaded. This is required by Anope");
			quitmsg = "ERROR: Remote server does not have the m_services module loaded, and this is required.";
			quitting = true;
			return MOD_STOP;
		}
		if (!has_hidechansmod)
		{
			send_cmd("", "ERROR :m_hidechans.so is not loaded. This is required by Anope");
			quitmsg = "ERROR: Remote server deos not have the m_hidechans module loaded, and this is required.";
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
	Me->GetLinks().front()->Sync(true);
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
	Anope::AddMessage("CAPAB", anope_event_capab);
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

bool ChannelModeFlood::IsValid(const Anope::string &value) const
{
	Anope::string rest;
	if (!value.empty() && value[0] != ':' && convertTo<int>(value[0] == '*' ? value.substr(1) : value, rest, false) > 0 && rest[0] == ':' && rest.length() > 1 && convertTo<int>(rest.substr(1), rest, false) > 0 && rest.empty())
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
	ProtoInspIRCd(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(PROTOCOL);

		pmodule_ircd_var(myIrcd);

		CapabType c[] = { CAPAB_NOQUIT, CAPAB_SSJ3, CAPAB_NICK2, CAPAB_VL, CAPAB_TLKEXT };
		for (unsigned i = 0; i < 5; ++i)
			Capab.SetFlag(c[i]);

		AddModes();

		pmodule_ircd_proto(&ircd_proto);
		moduleAddIRCDMsgs();

		ModuleManager::Attach(I_OnUserNickChange, this);
	}

	void OnUserNickChange(User *u, const Anope::string &)
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
	}
};

MODULE_INIT(ProtoInspIRCd)
