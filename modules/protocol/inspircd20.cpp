/* Inspircd 2.0 functions
 *
 * (C) 2003-2011 Anope Team
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

/* inspircd-ts6.h uses these */
static bool has_chghostmod = false;
static bool has_chgidentmod = false;
static bool has_globopsmod = true; // Not a typo
#include "inspircd-ts6.h"

IRCDVar myIrcd[] = {
	{"InspIRCd 2.0",	/* ircd name */
	 "+I",				/* Modes used by pseudoclients */
	 1,					/* SVSNICK */
	 1,					/* Vhost */
	 0,					/* Supports SNlines */
	 1,					/* Supports SQlines */
	 1,					/* Supports SZlines */
	 0,					/* Join 2 Message */
	 0,					/* Chan SQlines */
	 0,					/* Quit on Kill */
	 1,					/* vidents */
	 1,					/* svshold */
	 0,					/* time stamp on mode */
	 0,					/* O:LINE */
	 1,					/* UMODE */
	 1,					/* No Knock requires +i */
	 0,					/* Can remove User Channel Modes with SVSMODE */
	 0,					/* Sglines are not enforced until user reconnects */
	 1,					/* ts6 */
	 "$",				/* TLD Prefix for Global */
	 20,				/* Max number of modes we can send per line */
	 }
	,
	{NULL}
};

static bool has_servicesmod = false;
static bool has_svsholdmod = false;

/* CHGHOST */
void inspircd_cmd_chghost(const Anope::string &nick, const Anope::string &vhost)
{
	if (!has_chghostmod)
	{
		ircdproto->SendGlobops(OperServ, "CHGHOST not loaded!");
		return;
	}

	send_cmd(HostServ ? HostServ->GetUID() : Config->Numeric, "CHGHOST %s %s", nick.c_str(), vhost.c_str());
}

bool event_idle(const Anope::string &source, const std::vector<Anope::string> &params)
{
	BotInfo *bi = findbot(params[0]);

	send_cmd(bi ? bi->GetUID() : params[0], "IDLE %s %ld %ld", source.c_str(), static_cast<long>(start_time), bi ? (static_cast<long>(Anope::CurTime - bi->lastmsg)) : 0);
	return true;
}

class InspIRCdProto : public InspIRCdTS6Proto
{
	void SendConnect()
	{
		send_cmd("", "CAPAB START 1202");
		send_cmd("", "CAPAB CAPABILITIES :PROTOCOL=1202");
		send_cmd("", "CAPAB END");
		SendServer(Me);
		send_cmd(Config->Numeric, "BURST");
		send_cmd(Config->Numeric, "VERSION :Anope-%s %s :%s - (%s) -- %s", Anope::Version().c_str(), Config->ServerName.c_str(), ircd->name, Config->EncModuleList.begin()->c_str(), Anope::Build().c_str());
	}
};

bool event_ftopic(const Anope::string &source, const std::vector<Anope::string> &params)
{
	/* :source FTOPIC channel ts setby :topic */
	if (params.size() < 4)
		return true;

	Channel *c = findchan(params[0]);
	if (!c)
	{
		Log(LOG_DEBUG) << "TOPIC " << params[3] << " for nonexistent channel " << params[0];
		return true;
	}

	c->ChangeTopicInternal(params[2], params[3], Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : Anope::CurTime);

	return true;
}

bool event_opertype(const Anope::string &source, const std::vector<Anope::string> &params)
{
	/* opertype is equivalent to mode +o because servers
	   dont do this directly */
	User *u = finduser(source);
	if (u && !u->HasMode(UMODE_OPER))
	{
		std::vector<Anope::string> newparams;
		newparams.push_back(source);
		newparams.push_back("+o");
		return ircdmessage->OnMode(source, newparams);
	}
	return true;
}

bool event_fmode(const Anope::string &source, const std::vector<Anope::string> &params)
{
	/* :source FMODE #test 12345678 +nto foo */
	if (params.size() < 3)
		return true;

	Channel *c = findchan(params[0]);
	if (!c)
		return true;
	
	/* Checking the TS for validity to avoid desyncs */
	time_t ts = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : 0;
	if (c->creation_time > ts)
	{
		/* Our TS is bigger, we should lower it */
		c->creation_time = ts;
		c->Reset();
	}
	else if (c->creation_time < ts)
		/* The TS we got is bigger, we should ignore this message. */
		return true;

	/* TS's are equal now, so we can proceed with parsing */
	std::vector<Anope::string> newparams;
	newparams.push_back(params[0]);
	newparams.push_back(params[1]);
	Anope::string modes = params[2];
	for (unsigned n = 3; n < params.size(); ++n)
		modes += " " + params[n];
	newparams.push_back(modes);

	return ircdmessage->OnMode(source, newparams);
}

bool event_time(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() < 2)
		return true;

	send_cmd(Config->Numeric, "TIME %s %s %ld", source.c_str(), params[1].c_str(), static_cast<long>(Anope::CurTime));
	return true;
}

bool event_rsquit(const Anope::string &source, const std::vector<Anope::string> &params)
{
	/* On InspIRCd we must send a SQUIT when we recieve RSQUIT for a server we have juped */
	Server *s = Server::Find(params[0]);
	if (s && s->HasFlag(SERVER_JUPED))
		send_cmd(Config->Numeric, "SQUIT %s :%s", s->GetSID().c_str(), params.size() > 1 ? params[1].c_str() : "");

	ircdmessage->OnSQuit(source, params);

	return true;
}

bool event_setname(const Anope::string &source, const std::vector<Anope::string> &params)
{
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
	User *u = finduser(source);

	if (!u)
	{
		Log(LOG_DEBUG) << "FNAME for nonexistent user " << source;
		return true;
	}

	u->SetRealname(params[0]);
	return true;
}

bool event_setident(const Anope::string &source, const std::vector<Anope::string> &params)
{
	User *u = finduser(source);

	if (!u)
	{
		Log(LOG_DEBUG) << "SETIDENT for nonexistent user " << source;
		return true;
	}

	u->SetIdent(params[0]);
	return true;
}

bool event_chgident(const Anope::string &source, const std::vector<Anope::string> &params)
{
	User *u = finduser(source);

	if (!u)
	{
		Log(LOG_DEBUG) << "FIDENT for nonexistent user " << source;
		return true;
	}

	u->SetIdent(params[0]);
	return true;
}

bool event_sethost(const Anope::string &source, const std::vector<Anope::string> &params)
{
	User *u = finduser(source);

	if (!u)
	{
		Log(LOG_DEBUG) << "SETHOST for nonexistent user " << source;
		return true;
	}

	u->SetDisplayedHost(params[0]);
	return true;
}

bool event_chghost(const Anope::string &source, const std::vector<Anope::string> &params)
{
	User *u = finduser(source);

	if (!u)
	{
		Log(LOG_DEBUG) << "FHOST for nonexistent user " << source;
		return true;
	}

	u->SetDisplayedHost(params[0]);
	return true;
}


/*
 *   source     = numeric of the sending server
 *   params[0]  = uuid
 *   params[1]  = metadata name
 *   params[2]  = data
 */
bool event_metadata(const Anope::string &source, const std::vector<Anope::string> &params)
{
	if (params.size() < 3)
		return true;
	else if (params[1].equals_cs("accountname"))
	{
		User *u = finduser(params[0]);
		NickAlias *user_na = u ? findnick(u->nick) : NULL;
		NickCore *nc = findcore(params[2]);
		if (u && nc)
		{
			u->Login(nc);
			if (user_na && user_na->nc == nc)
				u->SetMode(NickServ, UMODE_REGISTERED);
		}
	}

	/*
	 *   possible incoming ssl_cert messages:
	 *   Received: :409 METADATA 409AAAAAA ssl_cert :vTrSe c38070ce96e41cc144ed6590a68d45a6 <...> <...>
	 *   Received: :409 METADATA 409AAAAAC ssl_cert :vTrSE Could not get peer certificate: error:00000000:lib(0):func(0):reason(0)
	 */
	else if (params[1].equals_cs("ssl_cert"))
	{
		User *u = finduser(params[0]);
		if (!u)
			return true;
	 	std::string data = params[2].c_str();
		size_t pos1 = data.find(' ') + 1;
		size_t pos2 = data.find(' ', pos1);
		if ((pos2 - pos1) == 32) // fingerprints should always be 32 bytes in size
		{
			u->fingerprint = data.substr(pos1, pos2 - pos1);
			FOREACH_MOD(I_OnFingerprint, OnFingerprint(u));
		}
	}
	return true;
}

bool event_endburst(const Anope::string &source, const std::vector<Anope::string> &params)
{
	Server *s = Server::Find(source);

	if (!s)
		throw CoreException("Got ENDBURST without a source");

	for (patricia_tree<User *, ci::ci_char_traits>::iterator it(UserListByNick); it.next();)
	{
		User *u = *it;
		if (u->server == s && !u->IsIdentified())
			validate_user(u);
	}

	Log(LOG_DEBUG) << "Processed ENDBURST for " << s->GetName();

	s->Sync(true);
	return true;
}

class Inspircd20IRCdMessage : public InspircdIRCdMessage
{
 public:
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
	bool OnUID(const Anope::string &source, const std::vector<Anope::string> &params)
	{
		time_t ts = Anope::string(params[1]).is_pos_number_only() ? convertTo<time_t>(params[1]) : 0;

		Anope::string modes = params[8];
		for (unsigned i = 9; i < params.size() - 1; ++i)
			modes += Anope::string(" ") + params[i];
		User *user = do_nick("", params[2], params[5], params[3], source, params[params.size() - 1], ts, params[6], params[4], params[0], modes);
		if (user && user->server->IsSynced())
			validate_user(user);

		return true;
	}

	bool OnCapab(const Anope::string &source, const std::vector<Anope::string> &params)
	{
		if (params[0].equals_cs("START"))
		{
			if (params.size() < 2 || (Anope::string(params[1]).is_pos_number_only() ? convertTo<unsigned>(params[1]) : 0) < 1202)
			{
				send_cmd("", "ERROR :Protocol mismatch, no or invalid protocol version given in CAPAB START");
				quitmsg = "Protocol mismatch, no or invalid protocol version given in CAPAB START";
				quitting = true;
				return MOD_STOP;
			}

			/* reset CAPAB */
			has_servicesmod = false;
			has_svsholdmod = false;
			has_chghostmod = false;
			has_chgidentmod = false;
		}
		else if (params[0].equals_cs("CHANMODES"))
		{
			spacesepstream ssep(params[1]);
			Anope::string capab;

			while (ssep.GetToken(capab))
			{
				Anope::string modename = capab.substr(0, capab.find('='));
				Anope::string modechar = capab.substr(capab.find('=') + 1);
				ChannelMode *cm = NULL;

				if (modename.equals_cs("admin"))
					cm = new ChannelModeStatus(CMODE_PROTECT, "CMODE_PROTECT", modechar[1], modechar[0]);
				else if (modename.equals_cs("allowinvite"))
					cm = new ChannelMode(CMODE_ALLINVITE, "CMODE_ALLINVITE", modechar[0]);
				else if (modename.equals_cs("auditorium"))
					cm = new ChannelMode(CMODE_AUDITORIUM, "CMODE_AUDITORIUM", modechar[0]);
				else if (modename.equals_cs("ban"))
					cm = new ChannelModeBan(modechar[0]);
				else if (modename.equals_cs("banexception"))
					cm = new ChannelModeExcept(modechar[0]);
				else if (modename.equals_cs("blockcaps"))
					cm = new ChannelMode(CMODE_BLOCKCAPS, "CMODE_BLOCKCAPS", modechar[0]);
				else if (modename.equals_cs("blockcolor"))
					cm = new ChannelMode(CMODE_BLOCKCOLOR, "CMODE_BLOCKCOLOR", modechar[0]);
				else if (modename.equals_cs("c_registered"))
					cm = new ChannelModeRegistered(modechar[0]);
				else if (modename.equals_cs("censor"))
					cm = new ChannelMode(CMODE_FILTER, "CMODE_FILTER", modechar[0]);
				else if (modename.equals_cs("delayjoin"))
					cm = new ChannelMode(CMODE_DELAYEDJOIN, "CMODE_DELAYEDJOIN", modechar[0]);
				else if (modename.equals_cs("flood"))
					cm = new ChannelModeFlood(modechar[0], true);
				else if (modename.equals_cs("founder"))
					cm = new ChannelModeStatus(CMODE_OWNER, "CMODE_OWNER", modechar[1], modechar[0]);
				else if (modename.equals_cs("halfop"))
					cm = new ChannelModeStatus(CMODE_HALFOP, "CMODE_HALFOP", modechar[1], modechar[0]);
				else if (modename.equals_cs("invex"))
					cm = new ChannelModeInvex(modechar[0]);
				else if (modename.equals_cs("inviteonly"))
					cm = new ChannelMode(CMODE_INVITE, "CMODE_INVITE", modechar[0]);
				else if (modename.equals_cs("joinflood"))
					cm = new ChannelModeParam(CMODE_JOINFLOOD, "CMODE_JOINFLOOD", modechar[0], true);
				else if (modename.equals_cs("key"))
					cm = new ChannelModeKey(modechar[0]);
				else if (modename.equals_cs("kicknorejoin"))
					cm = new ChannelModeParam(CMODE_NOREJOIN, "CMODE_NOREJOIN", modechar[0], true);
				else if (modename.equals_cs("limit"))
					cm = new ChannelModeParam(CMODE_LIMIT, "CMODE_LIMIT", modechar[0], true);
				else if (modename.equals_cs("moderated"))
					cm = new ChannelMode(CMODE_MODERATED, "CMODE_MODERATED", modechar[0]);
				else if (modename.equals_cs("nickflood"))
					cm = new ChannelModeParam(CMODE_NICKFLOOD, "CMODE_NICKFLOOD", modechar[0], true);
				else if (modename.equals_cs("noctcp"))
					cm = new ChannelMode(CMODE_NOCTCP, "CMODE_NOCTCP", modechar[0]);
				else if (modename.equals_cs("noextmsg"))
					cm = new ChannelMode(CMODE_NOEXTERNAL, "CMODE_NOEXTERNAL", modechar[0]);
				else if (modename.equals_cs("nokick"))
					cm = new ChannelMode(CMODE_NOKICK, "CMODE_NOKICK", modechar[0]);
				else if (modename.equals_cs("noknock"))
					cm = new ChannelMode(CMODE_NOKNOCK, "CMODE_NOKNOCK", modechar[0]);
				else if (modename.equals_cs("nonick"))
					cm = new ChannelMode(CMODE_NONICK, "CMODE_NONICK", modechar[0]);
				else if (modename.equals_cs("nonotice"))
					cm = new ChannelMode(CMODE_NONOTICE, "CMODE_NONOTICE", modechar[0]);
				else if (modename.equals_cs("op"))
					cm = new ChannelModeStatus(CMODE_OP, "CMODE_OP", modechar[1], modechar[0]);
				else if (modename.equals_cs("operonly"))
					cm = new ChannelModeOper(modechar[0]);
				else if (modename.equals_cs("permanent"))
					cm = new ChannelMode(CMODE_PERM, "CMODE_PERM", modechar[0]);
				else if (modename.equals_cs("private"))
					cm = new ChannelMode(CMODE_PRIVATE, "CMODE_PRIVATE", modechar[0]);
				else if (modename.equals_cs("redirect"))
					cm = new ChannelModeParam(CMODE_REDIRECT, "CMODE_REDIRECT", modechar[0], true);
				else if (modename.equals_cs("reginvite"))
					cm = new ChannelMode(CMODE_REGISTEREDONLY, "CMODE_REGISTEREDONLY", modechar[0]);
				else if (modename.equals_cs("regmoderated"))
					cm = new ChannelMode(CMODE_REGMODERATED, "CMODE_REGMODERATED", modechar[0]);
				else if (modename.equals_cs("secret"))
					cm = new ChannelMode(CMODE_SECRET, "CMODE_SECRET", modechar[0]);
				else if (modename.equals_cs("sslonly"))
					cm = new ChannelMode(CMODE_SSL, "CMODE_SSL", modechar[0]);
				else if (modename.equals_cs("stripcolor"))
					cm = new ChannelMode(CMODE_STRIPCOLOR, "CMODE_STRIPCOLOR", modechar[0]);
				else if (modename.equals_cs("topiclock"))
					cm = new ChannelMode(CMODE_TOPIC, "CMODE_TOPIC", modechar[0]);
				else if (modename.equals_cs("voice"))
					cm = new ChannelModeStatus(CMODE_VOICE, "CMODE_VOICE", modechar[1], modechar[0]);
				/* Unknown status mode, (customprefix) - add it */
				else if (modechar.length() == 2)
					cm = new ChannelModeStatus(CMODE_END, modechar[0], modechar[1], modechar[0]);
				/* else don't do anything here, we will get it in CAPAB CAPABILITIES */

				if (cm)
					ModeManager::AddChannelMode(cm);
				else
					Log() << "Unrecognized mode string in CAPAB CHANMODES: " << capab;
			}
		}
		if (params[0].equals_cs("USERMODES"))
		{
			spacesepstream ssep(params[1]);
			Anope::string capab;

			while (ssep.GetToken(capab))
			{
				Anope::string modename = capab.substr(0, capab.find('='));
				Anope::string modechar = capab.substr(capab.find('=') + 1);
				UserMode *um = NULL;

				if (modename.equals_cs("bot"))
					um = new UserMode(UMODE_BOT, "UMODE_BOT", modechar[0]);
				else if (modename.equals_cs("callerid"))
					um = new UserMode(UMODE_CALLERID, "UMODE_CALLERID", modechar[0]);
				else if (modename.equals_cs("cloak"))
					um = new UserMode(UMODE_CLOAK, "UMODE_CLOAK", modechar[0]);
				else if (modename.equals_cs("deaf"))
					um = new UserMode(UMODE_DEAF, "UMODE_DEAF", modechar[0]);
				else if (modename.equals_cs("deaf_commonchan"))
					um = new UserMode(UMODE_COMMONCHANS, "UMODE_COMMONCHANS", modechar[0]);
				else if (modename.equals_cs("helpop"))
					um = new UserMode(UMODE_HELPOP, "UMODE_HELPOP", modechar[0]);
				else if (modename.equals_cs("hidechans"))
					um = new UserMode(UMODE_PRIV, "UMODE_PRIV", modechar[0]);
				else if (modename.equals_cs("hideoper"))
					um = new UserMode(UMODE_HIDEOPER, "UMODE_HIDEOPER", modechar[0]);
				else if (modename.equals_cs("invisible"))
					um = new UserMode(UMODE_INVIS, "UMODE_INVIS", modechar[0]);
				else if (modename.equals_cs("invis-oper"))
					um = new UserMode(UMODE_INVISIBLE_OPER, "UMODE_INVISIBLE_OPER", modechar[0]);
				else if (modename.equals_cs("oper"))
					um = new UserMode(UMODE_OPER, "UMODE_OPER", modechar[0]);
				else if (modename.equals_cs("regdeaf"))
					um = new UserMode(UMODE_REGPRIV, "UMODE_REGPRIV", modechar[0]);
				else if (modename.equals_cs("servprotect"))
				{
					um = new UserMode(UMODE_PROTECTED, "UMODE_PROTECTED", modechar[0]);
					ircd->pseudoclient_mode = "+Ik";
				}
				else if (modename.equals_cs("showwhois"))
					um = new UserMode(UMODE_WHOIS, "UMODE_WHOIS", modechar[0]);
				else if (modename.equals_cs("snomask"))
					continue; // XXX
				else if (modename.equals_cs("u_censor"))
					um = new UserMode(UMODE_FILTER, "UMODE_FILTER", modechar[0]);
				else if (modename.equals_cs("u_registered"))
					um = new UserMode(UMODE_REGISTERED, "UMODE_REGISTERED", modechar[0]);
				else if (modename.equals_cs("u_stripcolor"))
					um = new UserMode(UMODE_STRIPCOLOR, "UMODE_STRIPCOLOR", modechar[0]);
				else if (modename.equals_cs("wallops"))
					um = new UserMode(UMODE_WALLOPS, "UMODE_WALLOPS", modechar[0]);

				if (um)
					ModeManager::AddUserMode(um);
				else
					Log() << "Unrecognized mode string in CAPAB USERMODES: " << capab;
			}
		}
		else if (params[0].equals_cs("MODULES"))
		{
			spacesepstream ssep(params[1]);
			Anope::string module;

			while (ssep.GetToken(module))
				if (module.equals_cs("m_svshold.so"))
					has_svsholdmod = true;
		}
		else if (params[0].equals_cs("MODSUPPORT"))
		{
			spacesepstream ssep(params[1]);
			Anope::string module;

			while (ssep.GetToken(module))
			{
				if (module.equals_cs("m_services_account.so"))
					has_servicesmod = true;
				else if (module.equals_cs("m_chghost.so"))
					has_chghostmod = true;
				else if (module.equals_cs("m_chgident.so"))
					has_chgidentmod = true;
			}
		}
		else if (params[0].equals_cs("CAPABILITIES"))
		{
			spacesepstream ssep(params[1]);
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
						if (ModeManager::FindChannelModeByChar(modebuf[t]))
							continue;
						ModeManager::AddChannelMode(new ChannelModeList(CMODE_END, modebuf[t], modebuf[t]));
					}

					sep.GetToken(modebuf);
					for (size_t t = 0, end = modebuf.length(); t < end; ++t)
					{
						if (ModeManager::FindChannelModeByChar(modebuf[t]))
							continue;
						ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, modebuf[t], modebuf[t]));
					}

					sep.GetToken(modebuf);
					for (size_t t = 0, end = modebuf.length(); t < end; ++t)
					{
						if (ModeManager::FindChannelModeByChar(modebuf[t]))
							continue;
						ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, modebuf[t], modebuf[t], true));
					}

					sep.GetToken(modebuf);
					for (size_t t = 0, end = modebuf.length(); t < end; ++t)
					{
						if (ModeManager::FindChannelModeByChar(modebuf[t]))
							continue;
						ModeManager::AddChannelMode(new ChannelMode(CMODE_END, modebuf[t], modebuf[t]));
					}
				}
				else if (capab.find("USERMODES") != Anope::string::npos)
				{
					Anope::string modes(capab.begin() + 10, capab.end());
					commasepstream sep(modes);
					Anope::string modebuf;

					sep.GetToken(modebuf);
					sep.GetToken(modebuf);

					if (sep.GetToken(modebuf))
						for (size_t t = 0, end = modebuf.length(); t < end; ++t)
							ModeManager::AddUserMode(new UserModeParam(UMODE_END, "", modebuf[t]));

					if (sep.GetToken(modebuf))
						for (size_t t = 0, end = modebuf.length(); t < end; ++t)
							ModeManager::AddUserMode(new UserMode(UMODE_END, "", modebuf[t]));
				}
				else if (capab.find("MAXMODES=") != Anope::string::npos)
				{
					Anope::string maxmodes(capab.begin() + 9, capab.end());
					ircd->maxmodes = maxmodes.is_pos_number_only() ? convertTo<unsigned>(maxmodes) : 3;
				}
			}
		}
		else if (params[0].equals_cs("END"))
		{
			if (!has_servicesmod)
			{
				send_cmd("", "ERROR :m_services_account.so is not loaded. This is required by Anope");
				quitmsg = "ERROR: Remote server does not have the m_services_account module loaded, and this is required.";
				quitting = true;
				return MOD_STOP;
			}
			if (!ModeManager::FindUserModeByName(UMODE_PRIV))
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

		IRCdMessage::OnCapab(source, params);

		return true;
	}
};

bool ChannelModeFlood::IsValid(const Anope::string &value) const
{
	try
	{
		Anope::string rest;
		if (!value.empty() && value[0] != ':' && convertTo<int>(value[0] == '*' ? value.substr(1) : value, rest, false) > 0 && rest[0] == ':' && rest.length() > 1 && convertTo<int>(rest.substr(1), rest, false) > 0 && rest.empty())
			return true;
	}
	catch (const ConvertException &) { }

	return false;
}

class ProtoInspIRCd : public Module
{
	Message message_endburst, message_time,
		message_rsquit, message_svsmode, message_fhost,
		message_chgident, message_fname, message_sethost, message_setident, message_setname, message_fjoin, message_fmode,
		message_ftopic, message_opertype, message_idle, message_metadata;
	
	InspIRCdProto ircd_proto;
	Inspircd20IRCdMessage ircd_message;
 public:
	ProtoInspIRCd(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator),
		message_endburst("ENDBURST", event_endburst),
		message_time("TIME", event_time), message_rsquit("RSQUIT", event_rsquit),
		message_svsmode("SVSMODE", OnMode), message_fhost("FHOST", event_chghost),
		message_chgident("FIDENT", event_chgident), message_fname("FNAME", event_chgname),
		message_sethost("SETHOST", event_sethost), message_setident("SETIDENT", event_setident),
		message_setname("SETNAME", event_setname), message_fjoin("FJOIN", OnSJoin),
		message_fmode("FMODE", event_fmode), message_ftopic("FTOPIC", event_ftopic),
		message_opertype("OPERTYPE", event_opertype), message_idle("IDLE", event_idle),
		message_metadata("METADATA", event_metadata)
	{
		this->SetAuthor("Anope");
		this->SetType(PROTOCOL);

		pmodule_ircd_var(myIrcd);
		pmodule_ircd_proto(&this->ircd_proto);
		pmodule_ircd_message(&this->ircd_message);
		
		Capab.SetFlag(CAPAB_NOQUIT);

		ModuleManager::Attach(I_OnUserNickChange, this);
	}

	void OnUserNickChange(User *u, const Anope::string &)
	{
		u->RemoveModeInternal(ModeManager::FindUserModeByName(UMODE_REGISTERED));
	}
};

MODULE_INIT(ProtoInspIRCd)
