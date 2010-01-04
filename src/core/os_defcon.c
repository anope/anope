/* OperServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

void defcon_sendlvls(User *u);
void runDefCon();
void defconParseModeString(const char *str);
void resetDefCon(int level);
static char *defconReverseModes(const char *modes);
class DefConTimeout;
static DefConTimeout *timeout;

class DefConTimeout : public Timer
{
  int level;

  public:
	DefConTimeout(int newlevel) : Timer(Config.DefConTimeOut), level(newlevel) { }

	void Tick(time_t)
	{
		if (Config.DefConLevel != level)
		{
			Config.DefConLevel = level;
			FOREACH_MOD(I_OnDefconLevel, OnDefconLevel(level));
			alog("Defcon level timeout, returning to lvl %d", level);
			ircdproto->SendGlobops(findbot(Config.s_OperServ), getstring(OPER_DEFCON_WALL), Config.s_OperServ, level);

			if (Config.GlobalOnDefcon)
			{
				if (Config.DefConOffMessage)
					oper_global(NULL, "%s", Config.DefConOffMessage);
				else
					oper_global(NULL, getstring(DEFCON_GLOBAL), Config.DefConLevel);

				if (Config.GlobalOnDefconMore && !Config.DefConOffMessage)
						oper_global(NULL, "%s", Config.DefconMessage);
			}

			runDefCon();
		}
	}
};

class CommandOSDEFCON : public Command
{
 public:
	CommandOSDEFCON() : Command("DEFCON", 1, 1, "operserv/defcon")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *lvl = params[0].c_str();
		int newLevel = 0;
		const char *langglobal = getstring(DEFCON_GLOBAL);

		if (!lvl)
		{
			notice_lang(Config.s_OperServ, u, OPER_DEFCON_CHANGED, Config.DefConLevel);
			defcon_sendlvls(u);
			return MOD_CONT;
		}
		newLevel = atoi(lvl);
		if (newLevel < 1 || newLevel > 5)
		{
			this->OnSyntaxError(u, "");
			return MOD_CONT;
		}
		Config.DefConLevel = newLevel;

		FOREACH_MOD(I_OnDefconLevel, OnDefconLevel(newLevel));

		if (timeout)
		{
			TimerManager::DelTimer(timeout);
			timeout = NULL;
		}

		if (Config.DefConTimeOut)
			timeout = new DefConTimeout(5);

		notice_lang(Config.s_OperServ, u, OPER_DEFCON_CHANGED, Config.DefConLevel);
		defcon_sendlvls(u);
		alog("Defcon level changed to %d by Oper %s", newLevel, u->nick.c_str());
		ircdproto->SendGlobops(findbot(Config.s_OperServ), getstring(OPER_DEFCON_WALL), u->nick.c_str(), newLevel);
		/* Global notice the user what is happening. Also any Message that
		   the Admin would like to add. Set in config file. */
		if (Config.GlobalOnDefcon)
		{
			if (Config.DefConLevel == 5 && Config.DefConOffMessage)
				oper_global(NULL, "%s", Config.DefConOffMessage);
			else
				oper_global(NULL, langglobal, Config.DefConLevel);
		}
		if (Config.GlobalOnDefconMore)
		{
			if (!Config.DefConOffMessage || Config.DefConLevel != 5)
				oper_global(NULL, "%s", Config.DefconMessage);
		}
		/* Run any defcon functions, e.g. FORCE CHAN MODE */
		runDefCon();
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_DEFCON);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "DEFCON", OPER_DEFCON_SYNTAX);
	}
};

class OSDEFCON : public Module
{
 public:
	OSDEFCON(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		if (!Config.DefConLevel)
		{
			throw ModuleException("Invalid configuration settings");
		}

		Implementation i[] = { I_OnOperServHelp, I_OnPreUserConnect, I_OnChannelModeSet, I_OnChannelModeUnset, I_OnPreCommandRun, I_OnPreCommand, I_OnUserConnect, I_OnChannelModeAdd, I_OnChannelCreate };
		ModuleManager::Attach(i, this, 9);

		this->AddCommand(OPERSERV, new CommandOSDEFCON());

		defconParseModeString(Config.DefConChanModes);
	}

	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_DEFCON);
	}

	EventReturn OnPreUserConnect(User *u)
	{
		std::string mask;

		if (u && is_sync(u->server) && CheckDefCon(DEFCON_AKILL_NEW_CLIENTS) && !is_ulined(u->server->name))
		{
			if (CheckDefCon(DEFCON_AKILL_NEW_CLIENTS))
			{
				mask = "*@" + std::string(u->host);
				alog("DEFCON: adding akill for %s", mask.c_str());
				add_akill(NULL, mask.c_str(), Config.s_OperServ,
					time(NULL) + Config.DefConAKILL,
					Config.DefConAkillReason ? Config.DefConAkillReason :
					"DEFCON AKILL");
			}

			if (CheckDefCon(DEFCON_NO_NEW_CLIENTS) || CheckDefCon(DEFCON_AKILL_NEW_CLIENTS))
				kill_user(Config.s_OperServ, u->nick, Config.DefConAkillReason);

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnChannelModeSet(Channel *c, ChannelModeName Name)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(Name);

		if (CheckDefCon(DEFCON_FORCE_CHAN_MODES) && cm && DefConModesOff.HasFlag(Name))
		{
			c->RemoveMode(findbot(Config.s_OperServ), Name);

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnChannelModeUnset(Channel *c, ChannelModeName Name)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(Name);

		if (CheckDefCon(DEFCON_FORCE_CHAN_MODES) && cm && DefConModesOn.HasFlag(Name))
		{
			std::string param;

			if (GetDefConParam(Name, &param))
			{
				c->SetMode(findbot(Config.s_OperServ), Name, param);
			}
			else
				c->SetMode(findbot(Config.s_OperServ), Name);

			return EVENT_STOP;

		}

		return EVENT_CONTINUE;
	}

	EventReturn OnPreCommandRun(const std::string &service, User *u, const char *cmd, Command *c)
	{
		if (!c)
		{
			if (CheckDefCon(DEFCON_SILENT_OPER_ONLY) && !is_oper(u))
				return EVENT_STOP;
		}
		if ((CheckDefCon(DEFCON_OPER_ONLY) || CheckDefCon(DEFCON_SILENT_OPER_ONLY)) && !is_oper(u))
		{
			if (!CheckDefCon(DEFCON_SILENT_OPER_ONLY))
			{
				notice_lang(service, u, OPER_DEFCON_DENIED);
			}

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnPreCommand(User *u, const std::string &service, const ci::string &command, const std::vector<ci::string> &params)
	{
		if (service == Config.s_NickServ)
		{
			if (command == "SET")
			{
				if (!params.empty() && params[0] == "MLOCK")
				{
					if (CheckDefCon(DEFCON_NO_MLOCK_CHANGE))
					{
						notice_lang(Config.s_ChanServ, u, OPER_DEFCON_DENIED);
						return EVENT_STOP;
					}
				}
			}
			else if (command == "REGISTER" || command == "GROUP")
			{
				if (CheckDefCon(DEFCON_NO_NEW_NICKS))
				{
					notice_lang(Config.s_NickServ, u, OPER_DEFCON_DENIED);
					return EVENT_STOP;
				}
			}
		}
		else if (service == Config.s_ChanServ)
		{
			if (command == "REGISTER")
			{
				if (CheckDefCon(DEFCON_NO_NEW_CHANNELS))
				{
					notice_lang(Config.s_ChanServ, u, OPER_DEFCON_DENIED);
					return EVENT_STOP;
				}
			}
		}
		else if (service == Config.s_MemoServ)
		{
			if (command == "SEND" || command == "SENDALL")
			{
				if (CheckDefCon(DEFCON_NO_NEW_MEMOS))
				{
					notice_lang(Config.s_MemoServ, u, OPER_DEFCON_DENIED);
					return EVENT_STOP;
				}
			}
		}

		return EVENT_CONTINUE;
	}

	void OnUserConnect(User *u)
	{
		Session *session = findsession(u->host);
		Exception *exception = find_hostip_exception(u->host, u->hostip);

		if (CheckDefCon(DEFCON_REDUCE_SESSION) && !exception)
		{
			if (session && session->count > Config.DefConSessionLimit)
			{
				if (Config.SessionLimitExceeded)
					ircdproto->SendMessage(findbot(Config.s_OperServ), u->nick.c_str(), Config.SessionLimitExceeded, u->host);
				if (Config.SessionLimitDetailsLoc)
					ircdproto->SendMessage(findbot(Config.s_OperServ), u->nick.c_str(), "%s", Config.SessionLimitDetailsLoc);

				kill_user(Config.s_OperServ, u->nick, "Session limit exceeded");
				session->hits++;
				if (Config.MaxSessionKill && session->hits >= Config.MaxSessionKill)
				{
					char akillmask[BUFSIZE];
					snprintf(akillmask, sizeof(akillmask), "*@%s", u->host);
					add_akill(NULL, akillmask, Config.s_OperServ, time(NULL) + Config.SessionAutoKillExpiry, "Session limit exceeded");
					ircdproto->SendGlobops(findbot(Config.s_OperServ), "Added a temporary AKILL for \2%s\2 due to excessive connections", akillmask);
				}
			}
		}
	}

	void OnChannelModeAdd(ChannelMode *cm)
	{
		if (Config.DefConChanModes)
		{
			std::string modes = Config.DefConChanModes;

			if (modes.find(cm->ModeChar) != std::string::npos)
			{
				/* New mode has been added to Anope, check to see if defcon
				 * requires it
				 */
				defconParseModeString(Config.DefConChanModes);
			}
		}
	}

	void OnChannelCreate(Channel *c)
	{
		if (CheckDefCon(DEFCON_FORCE_CHAN_MODES))
		{
			c->SetModes(findbot(Config.s_OperServ), false, Config.DefConChanModes);
		}
	}
};

/**
 * Send a message to the oper about which precautions are "active" for this level
 **/
void defcon_sendlvls(User *u)
{
	if (CheckDefCon(DEFCON_NO_NEW_CHANNELS))
		notice_lang(Config.s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_CHANNELS);
	if (CheckDefCon(DEFCON_NO_NEW_NICKS))
		notice_lang(Config.s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_NICKS);
	if (CheckDefCon(DEFCON_NO_MLOCK_CHANGE))
		notice_lang(Config.s_OperServ, u, OPER_HELP_DEFCON_NO_MLOCK_CHANGE);
	if (CheckDefCon(DEFCON_FORCE_CHAN_MODES) && Config.DefConChanModes)
		notice_lang(Config.s_OperServ, u, OPER_HELP_DEFCON_FORCE_CHAN_MODES, Config.DefConChanModes);
	if (CheckDefCon(DEFCON_REDUCE_SESSION))
		notice_lang(Config.s_OperServ, u, OPER_HELP_DEFCON_REDUCE_SESSION, Config.DefConSessionLimit);
	if (CheckDefCon(DEFCON_NO_NEW_CLIENTS))
		notice_lang(Config.s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_CLIENTS);
	if (CheckDefCon(DEFCON_OPER_ONLY))
		notice_lang(Config.s_OperServ, u, OPER_HELP_DEFCON_OPER_ONLY);
	if (CheckDefCon(DEFCON_SILENT_OPER_ONLY))
		notice_lang(Config.s_OperServ, u, OPER_HELP_DEFCON_SILENT_OPER_ONLY);
	if (CheckDefCon(DEFCON_AKILL_NEW_CLIENTS))
		notice_lang(Config.s_OperServ, u, OPER_HELP_DEFCON_AKILL_NEW_CLIENTS);
	if (CheckDefCon(DEFCON_NO_NEW_MEMOS))
		notice_lang(Config.s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_MEMOS);
}

void runDefCon()
{
	char *newmodes;

	if (CheckDefCon(DEFCON_FORCE_CHAN_MODES))
	{
		if (Config.DefConChanModes && !DefConModesSet)
		{
			if (Config.DefConChanModes[0] == '+' || Config.DefConChanModes[0] == '-')
			{
				alog("DEFCON: setting %s on all chan's", Config.DefConChanModes);
				DefConModesSet = 1;
				MassChannelModes(findbot(Config.s_OperServ), Config.DefConChanModes);
			}
		}
	}
	else
	{
		if (Config.DefConChanModes && (DefConModesSet != 0))
		{
			if (Config.DefConChanModes[0] == '+' || Config.DefConChanModes[0] == '-')
			{
				DefConModesSet = 0;
				if ((newmodes = defconReverseModes(Config.DefConChanModes)))
				{
					alog("DEFCON: setting %s on all chan's", newmodes);
					MassChannelModes(findbot(Config.s_OperServ), newmodes);
					delete [] newmodes;
				}
			}
		}
	}
}

/* Parse the defcon mlock mode string and set the correct global vars.
 *
 * @param str mode string to parse
 * @return 1 if accepted, 0 if failed
 */
void defconParseModeString(const char *str)
{
	int add = -1;		      /* 1 if adding, 0 if deleting, -1 if neither */
	unsigned char mode;
	ChannelMode *cm;
	ChannelModeParam *cmp;
	std::string modes, param;

	if (!str)
		return;

	spacesepstream ss(str);

	DefConModesOn.ClearFlags();
	DefConModesOff.ClearFlags();
	ss.GetToken(modes);

	/* Loop while there are modes to set */
	for (unsigned i = 0; i < modes.size(); ++i)
	{
		mode = modes[i];

		switch (mode)
		{
			case '+':
				add = 1;
				continue;
			case '-':
				add = 0;
				continue;
			default:
				if (add < 0)
					continue;
		}

		if ((cm = ModeManager::FindChannelModeByChar(mode)))
		{
			if (cm->Type == MODE_STATUS || cm->Type == MODE_LIST || !cm->CanSet(NULL))
			{
				alog("DefConChanModes mode character '%c' cannot be locked", mode);
				continue;
			}
			else if (add)
			{
				DefConModesOn.SetFlag(cm->Name);
				DefConModesOff.UnsetFlag(cm->Name);

				if (cm->Type == MODE_PARAM)
				{
					cmp = dynamic_cast<ChannelModeParam *>(cm);

					if (!ss.GetToken(param))
					{
						alog("DefConChanModes mode character '%c' has no parameter while one is expected", mode);
						continue;
					}

					if (!cmp->IsValid(param.c_str()))
						continue;

					SetDefConParam(cmp->Name, param);
				}
			}
			else
			{
				if (DefConModesOn.HasFlag(cm->Name))
				{
					DefConModesOn.UnsetFlag(cm->Name);

					if (cm->Type == MODE_PARAM)
					{
						UnsetDefConParam(cm->Name);
					}
				}
			}
		}
	}

	if ((cm = ModeManager::FindChannelModeByName(CMODE_REDIRECT)))
	{
		/* We can't mlock +L if +l is not mlocked as well. */
		if (DefConModesOn.HasFlag(cm->Name) && !DefConModesOn.HasFlag(CMODE_LIMIT))
		{
			DefConModesOn.UnsetFlag(CMODE_REDIRECT);

			alog("DefConChanModes must lock mode +l as well to lock mode +L");
		}
	}

	/* Some ircd we can't set NOKNOCK without INVITE */
	/* So check if we need there is a NOKNOCK MODE and that we need INVITEONLY */
	if (ircd->knock_needs_i && (cm = ModeManager::FindChannelModeByName(CMODE_NOKNOCK)))
	{
		if (DefConModesOn.HasFlag(cm->Name) && !DefConModesOn.HasFlag(CMODE_INVITE))
		{
			DefConModesOn.UnsetFlag(CMODE_NOKNOCK);
			alog("DefConChanModes must lock mode +i as well to lock mode +K");
		}
	}
}

static char *defconReverseModes(const char *modes)
{
        char *newmodes = NULL;
        unsigned i = 0;
        if (!modes) {
                return NULL;
        }
        if (!(newmodes = new char[strlen(modes) + 1])) {
                return NULL;
        }
        for (i = 0; i < strlen(modes); i++) {
                if (modes[i] == '+')
                        newmodes[i] = '-';
                else if (modes[i] == '-')
                        newmodes[i] = '+';
                else
                        newmodes[i] = modes[i];
        }
        newmodes[i] = '\0';
        return newmodes;
}

MODULE_INIT(OSDEFCON)
