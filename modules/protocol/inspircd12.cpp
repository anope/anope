/* inspircd 1.2 functions
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

/* inspircd-ts6.h uses these */
static bool has_globopsmod = false;
static bool has_chghostmod = false;
static bool has_chgidentmod = false;
static bool has_rlinemod = false;
static bool has_svstopic_topiclock = false;
static unsigned int spanningtree_proto_ver = 1201;
#include "inspircd-ts6.h"

static bool has_servicesmod = false;
static bool has_hidechansmod = false;

class InspIRCd12Proto : public InspIRCdTS6Proto
{
 public:
	InspIRCd12Proto() : InspIRCdTS6Proto("InspIRCd 1.2") { }
};

class InspIRCdExtBan : public ChannelModeList
{
 public:
	InspIRCdExtBan(ChannelModeName mName, char modeChar) : ChannelModeList(mName, modeChar) { }

	bool Matches(const User *u, const Entry *e) anope_override
	{
		const Anope::string &mask = e->mask;

		if (mask.find("m:") == 0 || mask.find("N:") == 0)
		{
			Anope::string real_mask = mask.substr(2);

			Entry en(this->Name, real_mask);
			if (en.Matches(u))
				return true;
		}
		else if (mask.find("j:") == 0)
		{
			Anope::string real_mask = mask.substr(2);

			Channel *c = findchan(real_mask);
			if (c != NULL && c->FindUser(u) != NULL)
				return true;
		}
		else if (mask.find("M:") == 0 || mask.find("R:") == 0)
		{
			Anope::string real_mask = mask.substr(2);

			if (u->IsIdentified() && real_mask.equals_ci(u->Account()->display))
				return true;
		}
		else if (mask.find("r:") == 0)
		{
			Anope::string real_mask = mask.substr(2);

			if (Anope::Match(u->realname, real_mask))
				return true;
		}
		else if (mask.find("s:") == 0)
		{
			Anope::string real_mask = mask.substr(2);

			if (Anope::Match(u->server->GetName(), real_mask))
				return true;
		}

		return false;
	}
};

struct IRCDMessageCapab : IRCDMessage
{
	IRCDMessageCapab() : IRCDMessage("CAPAB", 1) { SetFlag(IRCDMESSAGE_SOFT_LIMIT); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (params[0].equals_cs("START"))
		{
			/* reset CAPAB */
			has_servicesmod = false;
			has_globopsmod = false;
			has_chghostmod = false;
			has_chgidentmod = false;
			has_hidechansmod = false;
			ircdproto->CanSVSHold = false;
		}
		else if (params[0].equals_cs("MODULES") && params.size() > 1)
		{
			if (params[1].find("m_globops.so") != Anope::string::npos)
				has_globopsmod = true;
			if (params[1].find("m_services_account.so") != Anope::string::npos)
				has_servicesmod = true;
			if (params[1].find("m_svshold.so") != Anope::string::npos)
				ircdproto->CanSVSHold = true;
			if (params[1].find("m_chghost.so") != Anope::string::npos)
				has_chghostmod = true;
			if (params[1].find("m_chgident.so") != Anope::string::npos)
				has_chgidentmod = true;
			if (params[1].find("m_hidechans.so") != Anope::string::npos)
				has_hidechansmod = true;
			if (params[1].find("m_servprotect.so") != Anope::string::npos)
				ircdproto->DefaultPseudoclientModes = "+Ik";
			if (params[1].find("m_rline.so") != Anope::string::npos)
				has_rlinemod = true;
		}
		else if (params[0].equals_cs("CAPABILITIES") && params.size() > 1)
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
						switch (modebuf[t])
						{
							case 'b':
								ModeManager::AddChannelMode(new InspIRCdExtBan(CMODE_BAN, 'b'));
								continue;
							case 'e':
								ModeManager::AddChannelMode(new InspIRCdExtBan(CMODE_EXCEPT, 'e'));
								continue;
							case 'I':
								ModeManager::AddChannelMode(new InspIRCdExtBan(CMODE_INVITEOVERRIDE, 'I'));
								continue;
							/* InspIRCd sends q and a here if they have no prefixes */
							case 'q':
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, 'q', '@'));
								continue;
							case 'a':
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT , 'a', '@'));
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
							default:
								ModeManager::AddChannelMode(new ChannelModeParam(CMODE_END, modebuf[t]));
						}
					}

					sep.GetToken(modebuf);
					for (size_t t = 0, end = modebuf.length(); t < end; ++t)
					{
						switch (modebuf[t])
						{
							case 'F':
								ModeManager::AddChannelMode(new ChannelModeParam(CMODE_NICKFLOOD, 'F', true));
								continue;
							case 'J':
								ModeManager::AddChannelMode(new ChannelModeParam(CMODE_NOREJOIN, 'J', true));
								continue;
							case 'L':
								ModeManager::AddChannelMode(new ChannelModeParam(CMODE_REDIRECT, 'L', true));
								continue;
							case 'f':
								ModeManager::AddChannelMode(new ChannelModeFlood('f', true));
								continue;
							case 'j':
								ModeManager::AddChannelMode(new ChannelModeParam(CMODE_JOINFLOOD, 'j', true));
								continue;
							case 'l':
								ModeManager::AddChannelMode(new ChannelModeParam(CMODE_LIMIT, 'l', true));
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
							case 'A':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_ALLINVITE, 'A'));
								continue;
							case 'B':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCAPS, 'B'));
								continue;
							case 'C':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_NOCTCP, 'C'));
								continue;
							case 'D':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_DELAYEDJOIN, 'D'));
								continue;
							case 'G':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_FILTER, 'G'));
								continue;
							case 'K':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKNOCK, 'K'));
								continue;
							case 'M':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_REGMODERATED, 'M'));
								continue;
							case 'N':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_NONICK, 'N'));
								continue;
							case 'O':
								ModeManager::AddChannelMode(new ChannelModeOper('O'));
								continue;
							case 'P':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_PERM, 'P'));
								continue;
							case 'Q':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_NOKICK, 'Q'));
								continue;
							case 'R':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_REGISTEREDONLY, 'R'));
								continue;
							case 'S':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_STRIPCOLOR, 'S'));
								continue;
							case 'T':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_NONOTICE, 'T'));
								continue;
							case 'c':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_BLOCKCOLOR, 'c'));
								continue;
							case 'i':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_INVITE, 'i'));
								continue;
							case 'm':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_MODERATED, 'm'));
								continue;
							case 'n':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_NOEXTERNAL, 'n'));
								continue;
							case 'p':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_PRIVATE, 'p'));
								continue;
							case 'r':
								ModeManager::AddChannelMode(new ChannelModeRegistered('r'));
								continue;
							case 's':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_SECRET, 's'));
								continue;
							case 't':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_TOPIC, 't'));
								continue;
							case 'u':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_AUDITORIUM, 'u'));
								continue;
							case 'z':
								ModeManager::AddChannelMode(new ChannelMode(CMODE_SSL, 'z'));
								continue;
							default:
								ModeManager::AddChannelMode(new ChannelMode(CMODE_END, modebuf[t]));
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
									ModeManager::AddUserMode(new UserMode(UMODE_HELPOP, 'h'));
									continue;
								case 'B':
									ModeManager::AddUserMode(new UserMode(UMODE_BOT, 'B'));
									continue;
								case 'G':
									ModeManager::AddUserMode(new UserMode(UMODE_FILTER, 'G'));
									continue;
								case 'H':
									ModeManager::AddUserMode(new UserMode(UMODE_HIDEOPER, 'H'));
									continue;
								case 'I':
									ModeManager::AddUserMode(new UserMode(UMODE_PRIV, 'I'));
									continue;
								case 'Q':
									ModeManager::AddUserMode(new UserMode(UMODE_HIDDEN, 'Q'));
									continue;
								case 'R':
									ModeManager::AddUserMode(new UserMode(UMODE_REGPRIV, 'R'));
									continue;
								case 'S':
									ModeManager::AddUserMode(new UserMode(UMODE_STRIPCOLOR, 'S'));
									continue;
								case 'W':
									ModeManager::AddUserMode(new UserMode(UMODE_WHOIS, 'W'));
									continue;
								case 'c':
									ModeManager::AddUserMode(new UserMode(UMODE_COMMONCHANS, 'c'));
									continue;
								case 'g':
									ModeManager::AddUserMode(new UserMode(UMODE_CALLERID, 'g'));
									continue;
								case 'i':
									ModeManager::AddUserMode(new UserMode(UMODE_INVIS, 'i'));
									continue;
								case 'k':
									ModeManager::AddUserMode(new UserMode(UMODE_PROTECTED, 'k'));
									continue;
								case 'o':
									ModeManager::AddUserMode(new UserMode(UMODE_OPER, 'o'));
									continue;
								case 'r':
									ModeManager::AddUserMode(new UserMode(UMODE_REGISTERED, 'r'));
									continue;
								case 'w':
									ModeManager::AddUserMode(new UserMode(UMODE_WALLOPS, 'w'));
									continue;
								case 'x':
									ModeManager::AddUserMode(new UserMode(UMODE_CLOAK, 'x'));
									continue;
								case 'd':
									ModeManager::AddUserMode(new UserMode(UMODE_DEAF, 'd'));
									continue;
								default:
									ModeManager::AddUserMode(new UserMode(UMODE_END, modebuf[t]));
							}
						}
					}
				}
				else if (capab.find("PREFIX=(") != Anope::string::npos)
				{
					Anope::string modes(capab.begin() + 8, capab.begin() + capab.find(')'));
					Anope::string chars(capab.begin() + capab.find(')') + 1, capab.end());
					unsigned short level = modes.length() - 1;

					for (size_t t = 0, end = modes.length(); t < end; ++t)
					{
						switch (modes[t])
						{
							case 'q':
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OWNER, 'q', chars[t], level--));
								continue;
							case 'a':
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_PROTECT, 'a', chars[t], level--));
								continue;
							case 'o':
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_OP, 'o', chars[t], level--));
								continue;
							case 'h':
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_HALFOP, 'h', chars[t], level--));
								continue;
							case 'v':
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_VOICE, 'v', chars[t], level--));
								continue;
							default:
								ModeManager::AddChannelMode(new ChannelModeStatus(CMODE_END, modes[t], chars[t], level--));
						}
					}
				}
				else if (capab.find("MAXMODES=") != Anope::string::npos)
				{
					Anope::string maxmodes(capab.begin() + 9, capab.end());
					ircdproto->MaxModes = maxmodes.is_pos_number_only() ? convertTo<unsigned>(maxmodes) : 3;
				}
			}
		}
		else if (params[0].equals_cs("END"))
		{
			if (!has_globopsmod)
			{
				UplinkSocket::Message() << "ERROR :m_globops is not loaded. This is required by Anope";
				quitmsg = "Remote server does not have the m_globops module loaded, and this is required.";
				quitting = true;
				return false;
			}
			if (!has_servicesmod)
			{
				UplinkSocket::Message() << "ERROR :m_services_account.so is not loaded. This is required by Anope";
				quitmsg = "ERROR: Remote server does not have the m_services_account module loaded, and this is required.";
				quitting = true;
				return false;
			}
			if (!has_hidechansmod)
			{
				UplinkSocket::Message() << "ERROR :m_hidechans.so is not loaded. This is required by Anope";
				quitmsg = "ERROR: Remote server does not have the m_hidechans module loaded, and this is required.";
				quitting = true;
				return false;
			}
			if (!ircdproto->CanSVSHold)
				Log() << "SVSHOLD missing, Usage disabled until module is loaded.";
			if (!has_chghostmod)
				Log() << "CHGHOST missing, Usage disabled until module is loaded.";
			if (!has_chgidentmod)
				Log() << "CHGIDENT missing, Usage disabled until module is loaded.";
		}

		return true;
	}
};

struct IRCDMessageChgIdent : IRCDMessage
{
	IRCDMessageChgIdent() : IRCDMessage("CHGIDENT", 2) { }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = finduser(params[0]);
		if (u)
			u->SetIdent(params[1]);
		return true;
	}
};

struct IRCDMessageChgName : IRCDMessage
{
	IRCDMessageChgName(const Anope::string &n) : IRCDMessage(n, 1) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->SetRealname(params[0]);
		return true;
	}
};

struct IRCDMessageSetIdent : IRCDMessage
{
	IRCDMessageSetIdent() : IRCDMessage("SETIDENT", 0) { SetFlag(IRCDMESSAGE_REQUIRE_USER); }

	bool Run(MessageSource &source, const std::vector<Anope::string> &params) anope_override
	{
		source.GetUser()->SetIdent(params[0]);
		return true;
	}
};

class ProtoInspIRCd : public Module
{
	InspIRCd12Proto ircd_proto;

	/* Core message handlers */
	CoreIRCDMessageAway core_message_away;
	CoreIRCDMessageCapab core_message_capab;
	CoreIRCDMessageError core_message_error;
	CoreIRCDMessageJoin core_message_join;
	CoreIRCDMessageKick core_message_kick;
	CoreIRCDMessageKill core_message_kill;
	CoreIRCDMessageMOTD core_message_motd;
	CoreIRCDMessagePart core_message_part;
	CoreIRCDMessagePing core_message_ping;
	CoreIRCDMessagePrivmsg core_message_privmsg;
	CoreIRCDMessageQuit core_message_quit;
	CoreIRCDMessageSQuit core_message_squit;
	CoreIRCDMessageStats core_message_stats;
	CoreIRCDMessageTopic core_message_topic;
	CoreIRCDMessageVersion core_message_version;

	/* inspircd-ts6.h message handlers */
	IRCDMessageEndburst message_endburst;
	IRCDMessageFHost message_fhost, message_sethost;
	IRCDMessageFJoin message_sjoin;
	IRCDMessageFMode message_fmode;
	IRCDMessageFTopic message_ftopic;
	IRCDMessageIdle message_idle;
	IRCDMessageMetadata message_metadata;
	IRCDMessageMode message_mode;
	IRCDMessageNick message_nick;
	IRCDMessageOperType message_opertype;
	IRCDMessageRSQuit message_rsquit;
	IRCDMessageServer message_server;
	IRCDMessageTime message_time;
	IRCDMessageUID message_uid;

	/* Our message handlers */
	IRCDMessageChgIdent message_chgident;
	IRCDMessageChgName message_setname, message_chgname;
	IRCDMessageCapab message_capab;
	IRCDMessageSetIdent message_setident;

 public:
	ProtoInspIRCd(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PROTOCOL),
		message_fhost("FHOST"), message_sethost("SETHOST"),
		message_setname("SETNAME"), message_chgname("FNAME")
	{
		this->SetAuthor("Anope");

		Capab.insert("NOQUIT");

		Implementation i[] = { I_OnUserNickChange, I_OnServerSync };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		if (Config->Numeric.empty())
		{
			Anope::string numeric = ts6_sid_retrieve();
			Me->SetSID(numeric);
			Config->Numeric = numeric;
		}

		for (botinfo_map::iterator it = BotListByNick->begin(), it_end = BotListByNick->end(); it != it_end; ++it)
			it->second->GenerateUID();
	}

	void OnUserNickChange(User *u, const Anope::string &) anope_override
	{
		/* InspIRCd 1.2 doesn't set -r on nick change, remove -r here. Note that if we have to set +r later
		 * this will cancel out this -r, resulting in no mode changes.
		 */
		u->RemoveMode(findbot(Config->NickServ), UMODE_REGISTERED);
	}

	void OnServerSync(Server *s) anope_override
	{
		if (nickserv)
			for (Anope::insensitive_map<User *>::iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			{
				User *u = it->second;
				if (u->server == s && !u->IsIdentified())
					nickserv->Validate(u);
			}
	}
};

MODULE_INIT(ProtoInspIRCd)
