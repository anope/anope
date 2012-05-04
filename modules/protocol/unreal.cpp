/* Unreal IRCD 3.2.x functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

IRCDVar myIrcd = {
	"UnrealIRCd 3.2.x",	/* ircd name */
	 "+Soiq",				/* Modes used by pseudoclients */
	 1,						/* SVSNICK */
	 1,						/* Vhost */
	 1,						/* Supports SNlines */
	 1,						/* Supports SQlines */
	 1,						/* Supports SZlines */
	 0,						/* Join 2 Message */
	 0,						/* Chan SQlines */
	 0,						/* Quit on Kill */
	 1,						/* vidents */
	 1,						/* svshold */
	 1,						/* time stamp on mode */
	 1,						/* O:LINE */
	 1,						/* UMODE */
	 1,						/* No Knock requires +i */
	 1,						/* Can remove User Channel Modes with SVSMODE */
	 0,						/* Sglines are not enforced until user reconnects */
	 0,						/* ts6 */
	 "$",					/* TLD Prefix for Global */
	 12,					/* Max number of modes we can send per line */
	 0					/* IRCd sends a SSL users certificate fingerprint */
};

class UnrealIRCdProto : public IRCDProto
{
	/* SVSNOOP */
	void SendSVSNOOP(const Server *server, bool set) anope_override
	{
		UplinkSocket::Message() << "f " << server->GetName() << " " << (set ? "+" : "-");
	}

	void SendAkillDel(const XLine *x) anope_override
	{
		if (x->IsRegex() || x->HasNickOrReal())
			return;

		/* ZLine if we can instead */
		try
		{
			if (x->GetUser() == "*")
			{
				sockaddrs(x->GetHost());
				ircdproto->SendSZLineDel(x);
				return;
			}
		}
		catch (const SocketException &) { }

		UplinkSocket::Message() << "BD - G " << x->GetUser() << " " << x->GetHost() << " " << Config->OperServ;
	}

	void SendTopic(BotInfo *whosets, Channel *c) anope_override
	{
		UplinkSocket::Message(whosets) << ") " << c->name << " " << c->topic_setter << " " << c->topic_time + 1 << " :" << c->topic;
	}

	void SendVhostDel(User *u) anope_override
	{
		const BotInfo *bi = findbot(Config->HostServ);
		u->RemoveMode(bi, UMODE_CLOAK);
		u->RemoveMode(bi, UMODE_VHOST);
		ModeManager::ProcessModes();
		u->SetMode(bi, UMODE_CLOAK);
	}

	void SendAkill(User *u, XLine *x) anope_override
	{
		if (x->IsRegex() || x->HasNickOrReal())
		{
			if (!u)
			{
				/* No user (this akill was just added), and contains nick and/or realname. Find users that match and ban them */
				for (Anope::insensitive_map<User *>::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
					if (x->manager->Check(it->second, x))
						this->SendAkill(it->second, x);
				return;
			}

			const XLine *old = x;

			if (old->manager->HasEntry("*@" + u->host))
				return;

			/* We can't akill x as it has a nick and/or realname included, so create a new akill for *@host */
			XLine *xline = new XLine("*@" + u->host, old->By, old->Expires, old->Reason, old->UID);
			old->manager->AddXLine(xline);
			x = xline;

			Log(findbot(Config->OperServ), "akill") << "AKILL: Added an akill for " << x->Mask << " because " << u->GetMask() << "#" << u->realname << " matches " << old->Mask;
		}

		/* ZLine if we can instead */
		try
		{
			if (x->GetUser() == "*")
			{
				sockaddrs(x->GetHost());
				ircdproto->SendSZLine(u, x);
				return;
			}
		}
		catch (const SocketException &) { }

		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		UplinkSocket::Message() << "BD + G " << x->GetUser() << " " << x->GetHost() << " " << x->By << " " << Anope::CurTime + timeleft << " " << x->Created << " :" << x->GetReason();
	}

	void SendSVSKillInternal(const BotInfo *source, const User *user, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "h " << user->nick << " :" << buf;
	}

	void SendModeInternal(const BotInfo *source, const Channel *dest, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(source) << "G " << dest->name << " " << buf;
	}

	void SendModeInternal(const BotInfo *bi, const User *u, const Anope::string &buf) anope_override
	{
		UplinkSocket::Message(bi) << "v " << u->nick <<" " << buf;
	}

	void SendClientIntroduction(const User *u) anope_override
	{
		Anope::string modes = "+" + u->GetModes();
		UplinkSocket::Message() << "& " << u->nick << " 1 " << u->timestamp << " " << u->GetIdent() << " " << u->host << " " << u->server->GetName() << " 0 " << modes << " " << u->host << " * :" << u->realname;
	}

	void SendKickInternal(const BotInfo *source, const Channel *chan, const User *user, const Anope::string &buf) anope_override
	{
		if (!buf.empty())
			UplinkSocket::Message(source) << "H " << chan->name << " " << user->nick << " :" << buf;
		else
			UplinkSocket::Message(source) << "H " << chan->name << " " << user->nick;
	}

	/* SERVER name hop descript */
	/* Unreal 3.2 actually sends some info about itself in the descript area */
	void SendServer(const Server *server) anope_override
	{
		if (!Config->Numeric.empty())
			UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :U0-*-" << Config->Numeric << " " << server->GetDescription();
		else
			UplinkSocket::Message() << "SERVER " << server->GetName() << " " << server->GetHops() << " :" << server->GetDescription();
	}

	/* JOIN */
	void SendJoin(const User *user, Channel *c, const ChannelStatus *status) anope_override
	{
		UplinkSocket::Message(Me) << "~ " << c->creation_time << " " << c->name << " :" << user->nick;
		if (status)
		{
			/* First save the channel status incase uc->Status == status */
			ChannelStatus cs = *status;
			/* If the user is internally on the channel with flags, kill them so that
			 * the stacker will allow this.
			 */
			UserContainer *uc = c->FindUser(user);
			if (uc != NULL)
				uc->Status->ClearFlags();

			BotInfo *setter = findbot(user->nick);
			for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
				if (cs.HasFlag(ModeManager::ChannelModes[i]->Name))
					c->SetMode(setter, ModeManager::ChannelModes[i], user->nick, false);
		}
	}

	/* unsqline
	*/
	void SendSQLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message() << "d " << x->Mask;
	}

	/* SQLINE */
	/*
	** - Unreal will translate this to TKL for us
	**
	*/
	void SendSQLine(User *, const XLine *x) anope_override
	{
		UplinkSocket::Message() << "c " << x->Mask << " :" << x->GetReason();
	}

	/*
	** svso
	**	  parv[0] = sender prefix
	**	  parv[1] = nick
	**	  parv[2] = options
	*/
	void SendSVSO(const BotInfo *source, const Anope::string &nick, const Anope::string &flag) anope_override
	{
		UplinkSocket::Message(source) << "BB " << nick << " " << flag;
	}

	/* NICK <newnick>  */
	void SendChangeBotNick(const BotInfo *oldnick, const Anope::string &newnick) anope_override
	{
		UplinkSocket::Message(oldnick) << "& " << newnick << " " << Anope::CurTime;
	}

	/* Functions that use serval cmd functions */

	void SendVhost(User *u, const Anope::string &vIdent, const Anope::string &vhost) anope_override
	{
		if (!vIdent.empty())
			UplinkSocket::Message(Me) << "AZ " << u->nick << " " << vIdent;
		if (!vhost.empty())
			UplinkSocket::Message(Me) << "AL " << u->nick << " " << vhost;
	}

	void SendConnect() anope_override
	{
		/*
		   NICKv2 = Nick Version 2
		   VHP    = Sends hidden host
		   UMODE2 = sends UMODE2 on user modes
		   NICKIP = Sends IP on NICK
		   TOKEN  = Use tokens to talk
		   SJ3    = Supports SJOIN
		   NOQUIT = No Quit
		   TKLEXT = Extended TKL we don't use it but best to have it
		   SJB64  = Base64 encoded time stamps
		   ESVID  = Allows storing account names as services stamp
		   MLOCK  = Supports the MLOCK server command
		   VL     = Version Info
		   NS     = Config->Numeric Server
		*/
		if (!Config->Numeric.empty())
			UplinkSocket::Message() << "PROTOCTL NICKv2 VHP UMODE2 NICKIP TOKEN SJOIN SJOIN2 SJ3 NOQUIT TKLEXT ESVID MLOCK VL";
		else
			UplinkSocket::Message() << "PROTOCTL NICKv2 VHP UMODE2 NICKIP TOKEN SJOIN SJOIN2 SJ3 NOQUIT TKLEXT ESVID MLOCK";
		UplinkSocket::Message() << "PASS :" << Config->Uplinks[CurrentUplink]->password;
		SendServer(Me);
	}

	/* SVSHOLD - set */
	void SendSVSHold(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message() << "BD + Q H " << nick << " " << Config->ServerName << " " << Anope::CurTime + Config->NSReleaseTimeout << " " << Anope::CurTime << " :Being held for registered user";
	}

	/* SVSHOLD - release */
	void SendSVSHoldDel(const Anope::string &nick) anope_override
	{
		UplinkSocket::Message() << "BD - Q * " << nick << " " << Config->ServerName;
	}

	/* UNSGLINE */
	/*
	 * SVSNLINE - :realname mask
	*/
	void SendSGLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message() << "BR - :" << x->Mask;
	}

	/* UNSZLINE */
	void SendSZLineDel(const XLine *x) anope_override
	{
		UplinkSocket::Message() << "BD - Z * " << x->GetHost() << " " << Config->OperServ;
	}

	/* SZLINE */
	void SendSZLine(User *, const XLine *x) anope_override
	{
		// Calculate the time left before this would expire, capping it at 2 days
		time_t timeleft = x->Expires - Anope::CurTime;
		if (timeleft > 172800 || !x->Expires)
			timeleft = 172800;
		UplinkSocket::Message() << "BD + Z * " << x->GetHost() << " " << x->By << " " << Anope::CurTime + timeleft << " " << x->Created << " :" << x->GetReason();
	}

	/* SGLINE */
	/*
	 * SVSNLINE + reason_where_is_space :realname mask with spaces
	*/
	void SendSGLine(User *, const XLine *x) anope_override
	{
		Anope::string edited_reason = x->GetReason();
		edited_reason = edited_reason.replace_all_cs(" ", "_");
		UplinkSocket::Message() << "BR + " << edited_reason << " :" << x->Mask;
	}

	/* svsjoin
		parv[0] - sender
		parv[1] - nick to make join
		parv[2] - channel to join
		parv[3] - (optional) channel key(s)
	*/
	/* In older Unreal SVSJOIN and SVSNLINE tokens were mixed so SVSJOIN and SVSNLINE are broken
	   when coming from a none TOKEN'd server
	*/
	void SendSVSJoin(const BotInfo *source, const Anope::string &nick, const Anope::string &chan, const Anope::string &param) anope_override
	{
		if (!param.empty())
			UplinkSocket::Message(source) << "BX " << nick << " " << chan << " :" << param;
		else
			UplinkSocket::Message(source) << "BX " << nick << " :" << chan;
	}

	void SendSWhois(const BotInfo *source, const Anope::string &who, const Anope::string &mask) anope_override
	{
		UplinkSocket::Message(source) << "BA " << who << " :" << mask;
	}

	void SendEOB() anope_override
	{
		UplinkSocket::Message(Me) << "ES";
	}

	bool IsNickValid(const Anope::string &nick) anope_override
	{
		if (nick.equals_ci("ircd") || nick.equals_ci("irc"))
			return false;

		return true;
	}

	bool IsChannelValid(const Anope::string &chan) anope_override
	{
		if (chan.find(':') != Anope::string::npos || chan[0] != '#')
			return false;

		return true;
	}

	void SendLogin(User *u) anope_override
	{
		if (!u->Account())
			return;

		const BotInfo *ns = findbot(Config->NickServ);
		if (Capab.count("ESVID") > 0)
			ircdproto->SendMode(ns, u, "+d %s", u->Account()->display.c_str());
		else
			ircdproto->SendMode(ns, u, "+d %d", u->timestamp);
	}

	void SendLogout(User *u) anope_override
	{
		const BotInfo *ns = findbot(Config->NickServ);
		ircdproto->SendMode(ns, u, "+d 1");
	}

	void SendChannel(Channel *c) anope_override
	{
		/* Unreal does not support updating a channels TS without actually joining a user,
		 * so we will join and part us now
		 */
		BotInfo *bi = c->ci->WhoSends();
		if (!bi)
			;
		else if (c->FindUser(bi) == NULL)
		{
			bi->Join(c);
			bi->Part(c);
		}
		else
		{
			bi->Part(c);
			bi->Join(c);
		}
	}
};

class UnrealExtBan : public ChannelModeList
{
 public:
	UnrealExtBan(ChannelModeName mName, char modeChar) : ChannelModeList(mName, modeChar) { }

	bool Matches(const User *u, const Entry *e) anope_override
	{
		const Anope::string &mask = e->mask;

		if (mask.find("~c:") == 0)
		{
			Anope::string channel = mask.substr(3);

			ChannelMode *cm = NULL;
			if (channel[0] != '#')
			{
				char modeChar = ModeManager::GetStatusChar(channel[0]);
				channel.erase(channel.begin());
				cm = ModeManager::FindChannelModeByChar(modeChar);
				if (cm != NULL && cm->Type != MODE_STATUS)
					cm = NULL;
			}

			Channel *c = findchan(channel);
			if (c != NULL)
			{
				UserContainer *uc = c->FindUser(u);
				if (uc != NULL)
					if (cm == NULL || uc->Status->HasFlag(cm->Name))
						return true;
			}
		}
		else if (mask.find("~j:") == 0 || mask.find("~n:") == 0 || mask.find("~q:") == 0)
		{
			Anope::string real_mask = mask.substr(3);

			Entry en(this->Name, real_mask);
			if (en.Matches(u))
				return true;
		}
		else if (mask.find("~r:") == 0)
		{
			Anope::string real_mask = mask.substr(3);

			if (Anope::Match(u->realname, real_mask))
				return true;
		}
		else if (mask.find("~R:") == 0)
		{
			if (u->HasMode(UMODE_REGISTERED) && mask.equals_ci(u->nick))
				return true;
		}
		else if (mask.find("~a:") == 0)
		{
			Anope::string real_mask = mask.substr(3);

			if (u->Account() && Anope::Match(u->Account()->display, real_mask))
				return true;
		}
	
		return false;
	}
};

class ChannelModeFlood : public ChannelModeParam
{
 public:
	ChannelModeFlood(char modeChar, bool minusNoArg) : ChannelModeParam(CMODE_FLOOD, modeChar, minusNoArg) { }

	/* Borrowed part of this check from UnrealIRCd */
	bool IsValid(const Anope::string &value) const anope_override
	{
		if (value.empty())
			return false;
		try
		{
			Anope::string rest;
			if (value[0] != ':' && convertTo<unsigned>(value[0] == '*' ? value.substr(1) : value, rest, false) > 0 && rest[0] == ':' && rest.length() > 1 && convertTo<unsigned>(rest.substr(1), rest, false) > 0 && rest.empty())
				return true;
		}
		catch (const ConvertException &) { }
	
		/* '['<number><1 letter>[optional: '#'+1 letter],[next..]']'':'<number> */
		size_t end_bracket = value.find(']', 1);
		if (end_bracket == Anope::string::npos)
			return false;
		Anope::string xbuf = value.substr(0, end_bracket);
		if (value[end_bracket + 1] != ':')
			return false;
		commasepstream args(xbuf.substr(1));
		Anope::string arg;
		while (args.GetToken(arg))
		{
			/* <number><1 letter>[optional: '#'+1 letter] */
			size_t p = 0;
			while (p < arg.length() && isdigit(arg[p]))
				++p;
			if (p == arg.length() || !(arg[p] == 'c' || arg[p] == 'j' || arg[p] == 'k' || arg[p] == 'm' || arg[p] == 'n' || arg[p] == 't'))
				continue; /* continue instead of break for forward compatability. */
			int v = arg.substr(0, p).is_number_only() ? convertTo<int>(arg.substr(0, p)) : 0;
			if (v < 1 || v > 999)
				return false;
		}

		return true;
	}
};

class ChannelModeUnrealSSL : public ChannelMode
{
 public:
	ChannelModeUnrealSSL(ChannelModeName n, char c) : ChannelMode(n, c)
	{
	}

	bool CanSet(User *u) const anope_override
	{
		return false;
	}
};

class Unreal32IRCdMessage : public IRCdMessage
{
 public:
	bool OnMode(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() < 2)
			return true;
	
		bool server_source = Server::Find(source) != NULL;
		Anope::string modes = params[1];
		for (unsigned i = 2; i < params.size() - (server_source ? 1 : 0); ++i)
			modes += " " + params[i];

		if (params[0][0] == '#' || params[0][0] == '&')
			do_cmode(source, params[0], modes, server_source ? params[params.size() - 1] : "");
		else
			do_umode(params[0], modes);

		return true;
	}

	/*
	** NICK - new
	**	  source  = NULL
	**	  parv[0] = nickname
	**	  parv[1] = hopcount
	**	  parv[2] = timestamp
	**	  parv[3] = username
	**	  parv[4] = hostname
	**	  parv[5] = servername
	**  if NICK version 1:
	**	  parv[6] = servicestamp
	**	  parv[7] = info
	**  if NICK version 2:
	**	  parv[6] = servicestamp
	**	  parv[7] = umodes
	**	  parv[8] = virthost, * if none
	**	  parv[9] = info
	**  if NICKIP:
	**	  parv[9] = ip
	**	  parv[10] = info
	**
	** NICK - change
	**	  source  = oldnick
	**	  parv[0] = new nickname
	**	  parv[1] = hopcount
	*/
	bool OnNick(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() == 7)
		{
			/*
			   <codemastr> that was a bug that is now fixed in 3.2.1
			   <codemastr> in  some instances it would use the non-nickv2 format
			   <codemastr> it's sent when a nick collision occurs
			   - so we have to leave it around for now -TSL
			 */
			do_nick(source, params[0], params[3], params[4], params[5], params[6], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : 0, "", "*", "", "");
		}
		else if (params.size() == 11)
		{
			Anope::string decoded_ip;
			Anope::B64Decode(params[9], decoded_ip);

			sockaddrs ip;
			ip.ntop(params[9].length() == 8 ? AF_INET : AF_INET6, decoded_ip.c_str());

			Anope::string vhost = params[8];
			if (vhost.equals_cs("*"))
				vhost.clear();
		
			User *user = do_nick(source, params[0], params[3], params[4], params[5], params[10], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : 0, ip.addr(), vhost, "", params[7]);
			if (user)
			{
				const NickAlias *na = NULL;

				if (params[6] == "0")
					;
				else if (params[6].is_pos_number_only())
				{
					if (convertTo<time_t>(params[6]) == user->timestamp)
						na = findnick(user->nick);
				}
				else
				{
					na = findnick(params[6]);
				}

				if (na)
				{
					user->Login(na->nc);
					if (!Config->NoNicknameOwnership && na->nc->HasFlag(NI_UNCONFIRMED) == false && nickserv)
						user->SetMode(findbot(Config->NickServ), UMODE_REGISTERED);
				}
				else if (nickserv)
					nickserv->Validate(user);
			}
		}
		else if (params.size() != 2)
		{
			Anope::string vhost = params[8];
			if (vhost.equals_cs("*"))
				vhost.clear();

			/* NON NICKIP */
			User *user = do_nick(source, params[0], params[3], params[4], params[5], params[9], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : 0, "", vhost, "", params[7]);
			if (user)
			{
				const NickAlias *na = NULL;

				if (params[6] == "0")
					;
				else if (params[6].is_pos_number_only())
				{
					if (convertTo<time_t>(params[6]) == user->timestamp)
						na = findnick(user->nick);
				}
				else
				{
					na = findnick(params[6]);
				}

				if (na)
				{
					user->Login(na->nc);
					if (!Config->NoNicknameOwnership && na->nc->HasFlag(NI_UNCONFIRMED) == false && nickserv)
						user->SetMode(findbot(Config->NickServ), UMODE_REGISTERED);
				}
				else if (nickserv)
					nickserv->Validate(user);
			}
		}
		else
			do_nick(source, params[0], "", "", "", "", Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : 0, "", "", "", "");

		return true;
	}

	bool OnServer(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params[1].equals_cs("1"))
		{
			Anope::string vl = myStrGetToken(params[2], ' ', 0);
			Anope::string upnumeric = myStrGetToken(vl, '-', 2);
			Anope::string desc = myStrGetTokenRemainder(params[2], ' ', 1);
			do_server(source, params[0], Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0, desc, upnumeric);
		}
		else
			do_server(source, params[0], Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0, params[2], "");

		ircdproto->SendPing(Config->ServerName, params[0]);

		return true;
	}

	/*
	**	source = sender prefix
	**	parv[0] = channel name
	**	parv[1] = topic nickname
	**	parv[2] = topic time
	**	parv[3] = topic text
	*/
	bool OnTopic(const Anope::string &, const std::vector<Anope::string> &params) anope_override
	{
		if (params.size() != 4)
			return true;
	
		Channel *c = findchan(params[0]);
		if (!c)
		{
			Log() << "TOPIC for nonexistant channel " << params[0];
			return true;
		}

		c->ChangeTopicInternal(params[1], params[3], Anope::string(params[2]).is_pos_number_only() ? convertTo<time_t>(params[2]) : Anope::CurTime);

		return true;
	}

	bool OnCapab(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		for (unsigned i = 0; i < params.size(); ++i)
		{
			Anope::string capab = params[i];

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
							ModeManager::AddChannelMode(new UnrealExtBan(CMODE_BAN, 'b'));
							continue;
						case 'e':
							ModeManager::AddChannelMode(new UnrealExtBan(CMODE_EXCEPT, 'e'));
							continue;
						case 'I':
							ModeManager::AddChannelMode(new UnrealExtBan(CMODE_INVITEOVERRIDE, 'I'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelModeList(CMODE_END, modebuf[t]));
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
						case 'f':
							ModeManager::AddChannelMode(new ChannelModeFlood('f', false));
							continue;
						case 'L':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_REDIRECT, 'L'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, modebuf[t]));
					}
				}

				sep.GetToken(modebuf);
				for (size_t t = 0, end = modebuf.length(); t < end; ++t)
				{
					switch (modebuf[t])
					{
						case 'l':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, 'l', true));
							continue;
						case 'j':
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_JOINFLOOD, 'j', true));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, modebuf[t], true));
					}
				}

				sep.GetToken(modebuf);
				for (size_t t = 0, end = modebuf.length(); t < end; ++t)
				{
					switch (modebuf[t])
					{
						case 'p':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, 'p'));
							continue;
						case 's':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, 's'));
							continue;
						case 'm':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, 'm'));
							continue;
						case 'n':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, 'n'));
							continue;
						case 't':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, 't'));
							continue;
						case 'i':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, 'i'));
							continue;
						case 'r':
							ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
							continue;
						case 'R':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_REGISTEREDONLY, 'R'));
							continue;
						case 'c':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCOLOR, 'c'));
							continue;
						case 'O':
							ModeManager::AddChannelMode(new ChannelModeOper('O'));
							continue;
						case 'A':
							ModeManager::AddChannelMode(new ChannelModeAdmin('A'));
							continue;
						case 'Q':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKICK, 'Q'));
							continue;
						case 'K':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKNOCK, 'K'));
							continue;
						case 'V':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOINVITE, 'V'));
							continue;
						case 'C':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NOCTCP, 'C'));
							continue;
						case 'u':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_AUDITORIUM, 'u'));
							continue;
						case 'z':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_SSL, 'z'));
							continue;
						case 'N':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NONICK, 'N'));
							continue;
						case 'S':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_STRIPCOLOR, 'S'));
							continue;
						case 'M':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_REGMODERATED, 'M'));
							continue;
						case 'T':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_NONOTICE, 'T'));
							continue;
						case 'G':
							ModeManager::AddChannelMode(new ChannelMode(CMODE_FILTER, 'G'));
							continue;
						case 'Z':
							ModeManager::AddChannelMode(new ChannelModeUnrealSSL(CMODE_END, 'Z'));
							continue;
						default:
							ModeManager::AddChannelMode(new ChannelMode(CMODE_END, modebuf[t]));
					}
				}
			}
		}

		IRCdMessage::OnCapab(source, params);
		
		return true;
	}

	bool OnSJoin(const Anope::string &source, const std::vector<Anope::string> &params) anope_override
	{
		Channel *c = findchan(params[1]);
		time_t ts = Anope::string(params[0]).is_pos_number_only() ? convertTo<time_t>(params[0]) : 0;
		bool keep_their_modes = true;

		if (!c)
		{
			c = new Channel(params[1], ts);
			c->SetFlag(CH_SYNCING);
		}
		/* Our creation time is newer than what the server gave us */
		else if (c->creation_time > ts)
		{
			c->creation_time = ts;
			c->Reset();
		}
		/* Their TS is newer than ours, our modes > theirs, unset their modes if need be */
		else if (ts > c->creation_time)
			keep_their_modes = false;

		/* If we need to keep their modes, and this SJOIN string contains modes */
		if (keep_their_modes && params.size() >= 4)
		{
			Anope::string modes;
			for (unsigned i = 2; i < params.size() - 1; ++i)
				modes += " " + params[i];
			if (!modes.empty())
				modes.erase(modes.begin());
			/* Set the modes internally */
			c->SetModesInternal(NULL, modes);
		}

		ChannelMode *ban = ModeManager::FindChannelModeByName(CMODE_BAN),
			*except = ModeManager::FindChannelModeByName(CMODE_EXCEPT),
			*invex = ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE);
		spacesepstream sep(params[params.size() - 1]);
		Anope::string buf;
		while (sep.GetToken(buf))
		{
			/* Ban */
			if (keep_their_modes && ban && buf[0] == '&')
			{
				buf.erase(buf.begin());
				c->SetModeInternal(NULL, ban, buf);
			}
			/* Except */
			else if (keep_their_modes && except && buf[0] == '"')
			{
				buf.erase(buf.begin());
				c->SetModeInternal(NULL, except, buf);
			}
			/* Invex */
			else if (keep_their_modes && invex && buf[0] == '\'')
			{
				buf.erase(buf.begin());
				c->SetModeInternal(NULL, invex, buf);
			}
			else
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
						Log() << "Received unknown mode prefix " << ch << " in SJOIN string";
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
					c->SetModeInternal(NULL, *it, buf);

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
		if (c->HasFlag(CH_SYNCING))
		{
			/* Unset the syncing flag */
			c->UnsetFlag(CH_SYNCING);
			c->Sync();
		}

		return true;
	}
};


/** This is here because:
 *
 * If we had three servers, A, B & C linked like so: A<->B<->C
 * If Anope is linked to A and B splits from A and then reconnects
 * B introduces itself, introduces C, sends EOS for C, introduces Bs clients
 * introduces Cs clients, sends EOS for B. This causes all of Cs clients to be introduced
 * with their server "not syncing". We now send a PING immediately when receiving a new server
 * and then finish sync once we get a pong back from that server.
 */
bool event_pong(const Anope::string &source, const std::vector<Anope::string> &params)
{
	Server *s = Server::Find(source);
	if (s && !s->IsSynced())
		s->Sync(false);
	return true;
}

/* netinfo
 *  argv[0] = max global count
 *  argv[1] = time of end sync
 *  argv[2] = unreal protocol using (numeric)
 *  argv[3] = cloak-crc (> u2302)
 *  argv[4] = free(**)
 *  argv[5] = free(**)
 *  argv[6] = free(**)
 *  argv[7] = ircnet
 */
bool event_netinfo(const Anope::string &source, const std::vector<Anope::string> &params)
{
	UplinkSocket::Message() << "AO " << maxusercnt << " " << Anope::CurTime << " " << convertTo<int>(params[2]) << " " << params[3] << " 0 0 0 :" << params[7];
	return true;
}

/* Unreal sends USER modes with this */
/*
	umode2
	parv[0] - sender
	parv[1] - modes to change
*/
bool event_umode2(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() < 1)
		return true;

	do_umode(source, params[0]);
	return true;
}


bool event_setname(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() != 1)
		return true;

	User *u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "SETNAME for nonexistent user " << source;
		return true;
	}

	u->SetRealname(params[0]);
	return true;
}

bool event_chgname(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() != 2)
		return true;

	User *u = finduser(params[0]);
	if (!u)
	{
		Log(LOG_DEBUG) << "CHGNAME for nonexistent user " << params[0];
		return true;
	}

	u->SetRealname(params[1]);
	return true;
}

bool event_setident(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() != 1)
		return true;

	User *u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "SETIDENT for nonexistent user " << source;
		return true;
	}

	u->SetVIdent(params[0]);
	return true;
}

bool event_chgident(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() != 2)
		return true;

	User *u = finduser(params[0]);
	if (!u)
	{
		Log(LOG_DEBUG) << "CHGIDENT for nonexistent user " << params[0];
		return true;
	}

	u->SetVIdent(params[1]);
	return true;
}

bool event_sethost(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() != 1)
		return true;

	User *u = finduser(source);
	if (!u)
	{
		Log(LOG_DEBUG) << "SETHOST for nonexistent user " << source;
		return true;
	}

	/* When a user sets +x we recieve the new host and then the mode change */
	if (u->HasMode(UMODE_CLOAK))
		u->SetDisplayedHost(params[0]);
	else
		u->SetCloakedHost(params[0]);

	return true;
}

bool event_chghost(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() != 2)
		return true;

	User *u = finduser(params[0]);
	if (!u)
	{
		Log(LOG_DEBUG) << "debug: CHGHOST for nonexistent user " << params[0];
		return true;
	}

	u->SetDisplayedHost(params[1]);
	return true;
}

bool event_sdesc(const Anope::string &source, const std::vector<Anope::string> &params)
{
	Server *s = Server::Find(source);

	if (s)
		s->SetDescription(params[0]);

	return true;
}

class ProtoUnreal : public Module
{
	Message message_away, message_join, message_kick,
		message_kill, message_mode, message_nick, message_part,
		message_ping, message_pong, message_pong2, message_privmsg,
		message_quit, message_server, message_squit, message_topic,
		message_svsmode, message_svsmode2, message_svs2mode, message_svs2mode2, message_whois,
		message_capab, message_capab2, message_chghost, message_chghost2, message_chgident, message_chgident2,
		message_chgname, message_chgname2, message_netinfo, message_netinfo2, message_sethost, message_sethost2,
		message_setident, message_setident2, message_setname, message_setname2, message_error,
		message_umode2, message_umode22, message_sjoin, message_sdesc, message_sdesc2;
	
	/* Non-token of these in messages.cpp */
	Message message_stats, message_time, message_version;

	UnrealIRCdProto ircd_proto;
	Unreal32IRCdMessage ircd_message;

	void AddModes()
	{
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, 'v', '+', 0));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_HALFOP, 'h', '%', 1));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, 'o', '@', 2));
		/* Unreal sends +q as * and +a as ~ */
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT, 'a', '~', 3));
		ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, 'q', '*', 4));

		/* Add user modes */
		ModeManager::AddUserMode(new UserMode(UMODE_SERV_ADMIN, 'A'));
		ModeManager::AddUserMode(new UserMode(UMODE_BOT, 'B'));
		ModeManager::AddUserMode(new UserMode(UMODE_CO_ADMIN, 'C'));
		ModeManager::AddUserMode(new UserMode(UMODE_FILTER, 'G'));
		ModeManager::AddUserMode(new UserMode(UMODE_HIDEOPER, 'H'));
		ModeManager::AddUserMode(new UserMode(UMODE_HIDEIDLE, 'I'));
		ModeManager::AddUserMode(new UserMode(UMODE_NETADMIN, 'N'));
		ModeManager::AddUserMode(new UserMode(UMODE_REGPRIV, 'R'));
		ModeManager::AddUserMode(new UserMode(UMODE_PROTECTED, 'S'));
		ModeManager::AddUserMode(new UserMode(UMODE_NOCTCP, 'T'));
		ModeManager::AddUserMode(new UserMode(UMODE_WEBTV, 'V'));
		ModeManager::AddUserMode(new UserMode(UMODE_WHOIS, 'W'));
		ModeManager::AddUserMode(new UserMode(UMODE_ADMIN, 'a'));
		ModeManager::AddUserMode(new UserMode(UMODE_DEAF, 'd'));
		ModeManager::AddUserMode(new UserMode(UMODE_GLOBOPS, 'g'));
		ModeManager::AddUserMode(new UserMode(UMODE_HELPOP, 'h'));
		ModeManager::AddUserMode(new UserMode(UMODE_INVIS, 'i'));
		ModeManager::AddUserMode(new UserMode(UMODE_OPER, 'o'));
		ModeManager::AddUserMode(new UserMode(UMODE_PRIV, 'p'));
		ModeManager::AddUserMode(new UserMode(UMODE_GOD, 'q'));
		ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, 'r'));
		ModeManager::AddUserMode(new UserMode(UMODE_SNOMASK, 's'));
		ModeManager::AddUserMode(new UserMode(UMODE_VHOST, 't'));
		ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, 'w'));
		ModeManager::AddUserMode(new UserMode(UMODE_CLOAK, 'x'));
		ModeManager::AddUserMode(new UserMode(UMODE_SSL, 'z'));
	}

 public:
	ProtoUnreal(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL),
		message_away("6", OnAway),
		message_join("C", OnJoin),
		message_kick("H", OnKick), message_kill(".", OnKill),
		message_mode("G", OnMode),
		message_nick("&", OnNick), message_part("D", OnPart),
		message_ping("8", OnPing), message_pong("PONG", event_pong),
		message_pong2("9", event_pong), message_privmsg("!", ::OnPrivmsg),
		message_quit(",", OnQuit),
		message_server("'", OnServer), message_squit("-", OnSQuit),
		message_topic(")", OnTopic), message_svsmode("SVSMODE", OnMode),
		message_svsmode2("n", OnMode), message_svs2mode("SVS2MODE", OnMode), message_svs2mode2("v", OnMode),
		message_whois("#", OnWhois), message_capab("PROTOCTL", OnCapab),
		message_capab2("_", OnCapab), message_chghost("CHGHOST", event_chghost), message_chghost2("AL", event_chghost),
		message_chgident("CHGIDENT", event_chgident), message_chgident2("AZ", event_chgident),
		message_chgname("CHGNAME", event_chgname), message_chgname2("BK", event_chgname),
		message_netinfo("NETINFO", event_netinfo), message_netinfo2("AO", event_netinfo),
		message_sethost("SETHOST", event_sethost), message_sethost2("AA", event_sethost),
		message_setident("SETIDENT", event_setident), message_setident2("AD", event_setident),
		message_setname("SETNAME", event_setname), message_setname2("AE", event_setname),
		message_error("5", OnError), message_umode2("UMODE2", event_umode2),
		message_umode22("|", event_umode2), message_sjoin("~", OnSJoin),
		message_sdesc("SDESC", event_sdesc), message_sdesc2("AG", event_sdesc),
		
		message_stats("2", OnStats), message_time(">", OnTime), message_version("+", OnVersion)
	{
		this->SetAuthor("Anope");

		pmodule_ircd_var(&myIrcd);
		pmodule_ircd_proto(&this->ircd_proto);
		pmodule_ircd_message(&this->ircd_message);
		
		this->AddModes();

		Implementation i[] = { I_OnUserNickChange, I_OnChannelCreate, I_OnMLock, I_OnUnMLock };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
		ModuleManager::SetPriority(this, PRIORITY_FIRST);
	}

	~ProtoUnreal()
	{
		pmodule_ircd_var(NULL);
		pmodule_ircd_proto(NULL);
		pmodule_ircd_message(NULL);
	}


	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
	}

	void OnChannelCreate(Channel *c) anope_override
	{
		if (Capab.count("MLOCK") > 0 && c->ci)
		{
			Anope::string modes = c->ci->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "");
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(c->creation_time) << " " << c->ci->name << " " << modes;
		}
	}

	EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (cm && ci->c && (cm->Type == MODE_REGULAR || cm->Type == MODE_PARAM) && Capab.count("MLOCK") > 0)
		{
			Anope::string modes = ci->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "") + cm->ModeChar;
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->name << " " << modes;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnUnMLock(ChannelInfo *ci, ModeLock *lock) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(lock->name);
		if (cm && ci->c && (cm->Type == MODE_REGULAR || cm->Type == MODE_PARAM) && Capab.count("MLOCK") > 0)
		{
			Anope::string modes = ci->GetMLockAsString(false).replace_all_cs("+", "").replace_all_cs("-", "").replace_all_cs(cm->ModeChar, "");
			UplinkSocket::Message(Me) << "MLOCK " << static_cast<long>(ci->c->creation_time) << " " << ci->name << " " << modes;
		}

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(ProtoUnreal)
