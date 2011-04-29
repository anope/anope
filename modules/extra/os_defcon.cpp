/* OperServ core functions
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

#include "module.h"
#include "operserv.h"
#include "global.h"
#include "os_session.h"

enum DefconLevel
{
	DEFCON_NO_NEW_CHANNELS,
	DEFCON_NO_NEW_NICKS,
	DEFCON_NO_MLOCK_CHANGE,
	DEFCON_FORCE_CHAN_MODES,
	DEFCON_REDUCE_SESSION,
	DEFCON_NO_NEW_CLIENTS,
	DEFCON_OPER_ONLY,
	DEFCON_SILENT_OPER_ONLY,
	DEFCON_AKILL_NEW_CLIENTS,
	DEFCON_NO_NEW_MEMOS
};

static Module *me = NULL;
bool DefConModesSet = false;

struct DefconConfig
{
	std::vector<std::bitset<32> > DefCon;
	Flags<ChannelModeName, CMODE_END * 2> DefConModesOn;
	Flags<ChannelModeName, CMODE_END * 2> DefConModesOff;
	std::map<ChannelModeName, Anope::string> DefConModesOnParams;

	int defaultlevel, sessionlimit;
	Anope::string chanmodes, message, offmessage, akillreason;
	std::vector<Anope::string> defcons;
	time_t akillexpire, timeout;
	bool globalondefcon;

	DefconConfig()
	{
		this->DefCon.resize(5);
	}

	bool Check(DefconLevel level)
	{
		return this->Check(this->defaultlevel, level);
	}

	bool Check(int dlevel, DefconLevel level)
	{
		return this->DefCon[dlevel].test(level);
	}

	void Add(int dlevel, DefconLevel level)
	{
		this->DefCon[dlevel][level] = true;
	}

	void Del(int dlevel, DefconLevel level)
	{
		this->DefCon[dlevel][level] = false;
	}

	bool SetDefConParam(ChannelModeName Name, const Anope::string &buf)
	{
	       return DefConModesOnParams.insert(std::make_pair(Name, buf)).second;
	}

	void UnsetDefConParam(ChannelModeName Name)
	{
		DefConModesOnParams.erase(Name);
	}
	
	bool GetDefConParam(ChannelModeName Name, Anope::string &buf)
	{
	       std::map<ChannelModeName, Anope::string>::iterator it = DefConModesOnParams.find(Name);
	
	       buf.clear();
	
	       if (it != DefConModesOnParams.end())
	       {
	               buf = it->second;
	               return true;
	       }
	
	       return false;
	}
};

static DefconConfig DConfig;

/**************************************************************************/

void defcon_sendlvls(CommandSource &source);
void runDefCon();
static Anope::string defconReverseModes(const Anope::string &modes);

class DefConTimeout : public CallBack
{
	int level;

 public:
	DefConTimeout(Module *mod, int newlevel) : CallBack(mod, DConfig.timeout), level(newlevel) { }

	void Tick(time_t)
	{
		if (DConfig.defaultlevel != level)
		{
			DConfig.defaultlevel = level;
			FOREACH_MOD(I_OnDefconLevel, OnDefconLevel(level));
			Log(operserv->Bot(), "operserv/defcon") << "Defcon level timeout, returning to level " << level;
			ircdproto->SendGlobops(operserv->Bot(), GetString(NULL, _("\002%s\002 Changed the DEFCON level to \002%d\002")).c_str(), Config->s_OperServ.c_str(), level);

			if (DConfig.globalondefcon)
			{
				if (!DConfig.offmessage.empty())
					global->SendGlobal(global->Bot(), "", DConfig.offmessage);
				else
					global->SendGlobal(global->Bot(), "", Anope::printf(GetString(NULL, _("The Defcon Level is now at Level: \002%d\002")).c_str(), DConfig.defaultlevel));

				if (!DConfig.message.empty())
					global->SendGlobal(global->Bot(), "", DConfig.message);
			}

			runDefCon();
		}
	}
};
static DefConTimeout *timeout;

class CommandOSDefcon : public Command
{
	void SendLevels(CommandSource &source)
	{
		if (DConfig.Check(DEFCON_NO_NEW_CHANNELS))
			source.Reply(_("* No new channel registrations"));
		if (DConfig.Check(DEFCON_NO_NEW_NICKS))
			source.Reply(_("* No new nick registrations"));
		if (DConfig.Check(DEFCON_NO_MLOCK_CHANGE))
			source.Reply(_("* No MLOCK changes"));
		if (DConfig.Check(DEFCON_FORCE_CHAN_MODES) && !DConfig.chanmodes.empty())
			source.Reply(_("* Force Chan Modes (%s) to be set on all channels"), DConfig.chanmodes.c_str());
		if (DConfig.Check(DEFCON_REDUCE_SESSION))
			source.Reply(_("* Use the reduced session limit of %d"), DConfig.sessionlimit);
		if (DConfig.Check(DEFCON_NO_NEW_CLIENTS))
			source.Reply(_("* Kill any NEW clients connecting"));
		if (DConfig.Check(DEFCON_OPER_ONLY))
			source.Reply(_("* Ignore any non-opers with message"));
		if (DConfig.Check(DEFCON_SILENT_OPER_ONLY))
			source.Reply(_("* Silently ignore non-opers"));
		if (DConfig.Check(DEFCON_AKILL_NEW_CLIENTS))
			source.Reply(_("* AKILL any new clients connecting"));
		if (DConfig.Check(DEFCON_NO_NEW_MEMOS))
			source.Reply(_("* No new memos sent"));
	}

 public:
	CommandOSDefcon() : Command("DEFCON", 1, 1, "operserv/defcon")
	{
		this->SetDesc(_("Manipulate the DefCon system"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &lvl = params[0];

		if (lvl.empty())
		{
			source.Reply(_("Services are now at DEFCON \002%d\002"), DConfig.defaultlevel);
			this->SendLevels(source);
			return MOD_CONT;
		}

		int newLevel = 0;
		try
		{
			newLevel = convertTo<int>(lvl);
		}
		catch (const ConvertException &) { }

		if (newLevel < 1 || newLevel > 5)
		{
			this->OnSyntaxError(source, "");
			return MOD_CONT;
		}

		DConfig.defaultlevel = newLevel;

		FOREACH_MOD(I_OnDefconLevel, OnDefconLevel(newLevel));

		if (timeout)
		{
			delete timeout;
			timeout = NULL;
		}

		if (DConfig.timeout)
			timeout = new DefConTimeout(me, 5);

		source.Reply(_("Services are now at DEFCON \002%d\002"), DConfig.defaultlevel);
		this->SendLevels(source);
		Log(LOG_ADMIN, u, this) << "to change defcon level to " << newLevel;
		ircdproto->SendGlobops(operserv->Bot(), _("\002%s\002 Changed the DEFCON level to \002%d\002"), u->nick.c_str(), newLevel);
		/* Global notice the user what is happening. Also any Message that
		   the Admin would like to add. Set in config file. */
		if (DConfig.globalondefcon)
		{
			if (DConfig.defaultlevel == 5 && !DConfig.offmessage.empty())
				global->SendGlobal(global->Bot(), "", DConfig.offmessage);
			else if (DConfig.defaultlevel != 5)
			{
				global->SendGlobal(global->Bot(), "", Anope::printf(_("The Defcon level is now at: \002%d\002"), DConfig.defaultlevel));
				if (!DConfig.message.empty())
					global->SendGlobal(global->Bot(), "", DConfig.message);
			}
		}

		/* Run any defcon functions, e.g. FORCE CHAN MODE */
		runDefCon();
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002DEFCON\002 [\0021\002|\0022\002|\0023\002|\0024\002|\0025\002]\n"
				"The defcon system can be used to implement a pre-defined\n"
				"set of restrictions to services useful during an attempted\n"
				"attack on the network."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "DEFCON", _("DEFCON [\0021\002|\0022\002|\0023\002|\0024\002|\0025\002]"));
	}
};

class OSDefcon : public Module
{
	service_reference<SessionService> session_service;
	CommandOSDefcon commandosdefcon;

	void ParseModeString()
	{
		int add = -1; /* 1 if adding, 0 if deleting, -1 if neither */
		unsigned char mode;
		ChannelMode *cm;
		ChannelModeParam *cmp;
		Anope::string modes, param;

		spacesepstream ss(DConfig.chanmodes);

		DConfig.DefConModesOn.ClearFlags();
		DConfig.DefConModesOff.ClearFlags();
		ss.GetToken(modes);

		/* Loop while there are modes to set */
		for (unsigned i = 0, end = modes.length(); i < end; ++i)
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
					Log() << "DefConChanModes mode character '" << mode << "' cannot be locked";
					continue;
				}
				else if (add)
				{
					DConfig.DefConModesOn.SetFlag(cm->Name);
					DConfig.DefConModesOff.UnsetFlag(cm->Name);

					if (cm->Type == MODE_PARAM)
					{
						cmp = debug_cast<ChannelModeParam *>(cm);

						if (!ss.GetToken(param))
						{
							Log() << "DefConChanModes mode character '" << mode << "' has no parameter while one is expected";
							continue;
						}

						if (!cmp->IsValid(param))
							continue;

						DConfig.SetDefConParam(cmp->Name, param);
					}
				}
				else if (DConfig.DefConModesOn.HasFlag(cm->Name))
				{
					DConfig.DefConModesOn.UnsetFlag(cm->Name);

					if (cm->Type == MODE_PARAM)
						DConfig.UnsetDefConParam(cm->Name);
				}
			}
		}

		/* We can't mlock +L if +l is not mlocked as well. */
		if ((cm = ModeManager::FindChannelModeByName(CMODE_REDIRECT)) && DConfig.DefConModesOn.HasFlag(cm->Name) && !DConfig.DefConModesOn.HasFlag(CMODE_LIMIT))
		{
			DConfig.DefConModesOn.UnsetFlag(CMODE_REDIRECT);
	
			Log() << "DefConChanModes must lock mode +l as well to lock mode +L";
		}

		/* Some ircd we can't set NOKNOCK without INVITE */
		/* So check if we need there is a NOKNOCK MODE and that we need INVITEONLY */
		if (ircd->knock_needs_i && (cm = ModeManager::FindChannelModeByName(CMODE_NOKNOCK)) && DConfig.DefConModesOn.HasFlag(cm->Name) && !DConfig.DefConModesOn.HasFlag(CMODE_INVITE))
		{
			DConfig.DefConModesOn.UnsetFlag(CMODE_NOKNOCK);
			Log() << "DefConChanModes must lock mode +i as well to lock mode +K";
		}
	}

 public:
	OSDefcon(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED), session_service("session")
	{
		if (!DConfig.defaultlevel)
			throw ModuleException("Invalid configuration settings");

		me = this;

		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnChannelModeSet, I_OnChannelModeUnset, I_OnPreCommandRun, I_OnPreCommand, I_OnUserConnect, I_OnChannelModeAdd, I_OnChannelCreate };
		ModuleManager::Attach(i, this, 9);

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->AddCommand(operserv->Bot(), &commandosdefcon);

		this->OnReload();
	}

	void OnReload()
	{
		ConfigReader config;
		DefconConfig dconfig;

		dconfig.defaultlevel = config.ReadInteger("defcon", "defaultlevel", 0, 0);
		dconfig.defcons[4] = config.ReadValue("defcon", "level4", 0);
		dconfig.defcons[3] = config.ReadValue("defcon", "level3", 0);
		dconfig.defcons[2] = config.ReadValue("defcon", "level2", 0);
		dconfig.defcons[1] = config.ReadValue("defcon", "level1", 0);
		dconfig.sessionlimit = config.ReadInteger("defcon", "sessionlimit", 0, 0);
		dconfig.akillexpire = dotime(config.ReadValue("defcon", "akillexpire", 0));
		dconfig.chanmodes = config.ReadValue("defcon", "chanmodes", 0);
		dconfig.timeout = dotime(config.ReadValue("defcon", "timeout", 0));
		dconfig.globalondefcon = config.ReadFlag("defcon", "globalondefcon", 0);
		dconfig.message = config.ReadValue("defcon", "message", 0);
		dconfig.offmessage = config.ReadValue("defcon", "offmessage", 0);

		if (dconfig.defaultlevel < 1 || dconfig.defaultlevel > 5)
			throw ConfigException("The value for <os_defcon:defaultlevel> must be between 1 and 5");
		else if (dconfig.akillexpire <= 0)
			throw ConfigException("The value for <os_defcon:akillexpire> must be greater than zero!");

		for (unsigned level = 1; level < 5; ++level)
		{
			spacesepstream operations(dconfig.defcons[level]);
			Anope::string operation;
			while (operations.GetToken(operation))
			{
				if (operation.equals_ci("nonewchannels"))
					dconfig.Add(level, DEFCON_NO_NEW_CHANNELS);
				else if (operation.equals_ci("nonewnicks"))
					dconfig.Add(level, DEFCON_NO_NEW_NICKS);
				else if (operation.equals_ci("nomlockchanges"))
					dconfig.Add(level, DEFCON_NO_MLOCK_CHANGE);
				else if (operation.equals_ci("forcechanmodes"))
					dconfig.Add(level, DEFCON_FORCE_CHAN_MODES);
				else if (operation.equals_ci("reducedsessions"))
					dconfig.Add(level, DEFCON_REDUCE_SESSION);
				else if (operation.equals_ci("nonewclients"))
					dconfig.Add(level, DEFCON_NO_NEW_CLIENTS);
				else if (operation.equals_ci("operonly"))
					dconfig.Add(level, DEFCON_OPER_ONLY);
				else if (operation.equals_ci("silentoperonly"))
					dconfig.Add(level, DEFCON_SILENT_OPER_ONLY);
				else if (operation.equals_ci("akillnewclients"))
					dconfig.Add(level, DEFCON_AKILL_NEW_CLIENTS);
				else if (operation.equals_ci("nonewmemos"))
					dconfig.Add(level, DEFCON_NO_NEW_MEMOS);
			}

			if (dconfig.Check(level, DEFCON_REDUCE_SESSION) && dconfig.sessionlimit <= 0)
				throw ConfigException("The value for <os_defcon:sessionlimit> must be greater than zero!");
			else if (dconfig.Check(level, DEFCON_AKILL_NEW_CLIENTS) && dconfig.akillreason.empty())
				throw ConfigException("The value for <os_defcon:akillreason> must not be empty!");
			else if (dconfig.Check(level, DEFCON_FORCE_CHAN_MODES) && dconfig.chanmodes.empty())
				throw ConfigException("The value for <os_defcon:chanmodes> must not be empty!");
		}

		DConfig = dconfig;
		this->ParseModeString();
	}

	EventReturn OnUserConnect(User *u, bool &exempt)
	{
		if (!exempt && u->server->IsSynced() && DConfig.Check(DEFCON_AKILL_NEW_CLIENTS) && !u->server->IsULined())
		{
			if (DConfig.Check(DEFCON_AKILL_NEW_CLIENTS))
			{
				Log(operserv->Bot(), "operserv/defcon") << "DEFCON: adding akill for *@" << u->host;
				XLine *x = SGLine->Add("*@" + u->host, Config->s_OperServ, Anope::CurTime + DConfig.akillexpire, DConfig.akillreason);
				if (x)
					x->By = Config->s_OperServ;
			}

			if (DConfig.Check(DEFCON_NO_NEW_CLIENTS) || DConfig.Check(DEFCON_AKILL_NEW_CLIENTS))
				u->Kill(Config->s_OperServ, DConfig.akillreason);

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnChannelModeSet(Channel *c, ChannelModeName Name, const Anope::string &param)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(Name);

		if (DConfig.Check(DEFCON_FORCE_CHAN_MODES) && cm && DConfig.DefConModesOff.HasFlag(Name))
		{
			c->RemoveMode(operserv->Bot(), Name, param);

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnChannelModeUnset(Channel *c, ChannelModeName Name, const Anope::string &)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(Name);

		if (DConfig.Check(DEFCON_FORCE_CHAN_MODES) && cm && DConfig.DefConModesOn.HasFlag(Name))
		{
			Anope::string param;

			if (DConfig.GetDefConParam(Name, param))
				c->SetMode(operserv->Bot(), Name, param);
			else
				c->SetMode(operserv->Bot(), Name);

			return EVENT_STOP;

		}

		return EVENT_CONTINUE;
	}

	EventReturn OnPreCommandRun(User *&u, BotInfo *&bi, Anope::string &command, Anope::string &message, ChannelInfo *&ci)
	{
		if (!u->HasMode(UMODE_OPER) && (DConfig.Check(DEFCON_OPER_ONLY) || DConfig.Check(DEFCON_SILENT_OPER_ONLY)))
		{
			if (!DConfig.Check(DEFCON_SILENT_OPER_ONLY))
				u->SendMessage(bi, _("Services are in Defcon mode, Please try again later."));

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params)
	{
		BotInfo *service = command->service;
		if (service->nick == Config->s_NickServ)
		{
			if (command->name.equals_ci("REGISTER") || command->name.equals_ci("GROUP"))
			{
				if (DConfig.Check(DEFCON_NO_NEW_NICKS))
				{
					source.Reply(_("Services are in Defcon mode, Please try again later."));
					return EVENT_STOP;
				}
			}
		}
		else if (service->nick == Config->s_ChanServ)
		{
			if (command->name.equals_ci("SET"))
			{
				if (!params.empty() && params[0].equals_ci("MLOCK") && DConfig.Check(DEFCON_NO_MLOCK_CHANGE))
				{
					source.Reply(_("Services are in Defcon mode, Please try again later."));
					return EVENT_STOP;
				}
			}
			else if (command->name.equals_ci("REGISTER"))
			{
				if (DConfig.Check(DEFCON_NO_NEW_CHANNELS))
				{
					source.Reply(_("Services are in Defcon mode, Please try again later."));
					return EVENT_STOP;
				}
			}
		}
		else if (service->nick == Config->s_MemoServ)
		{
			if (command->name.equals_ci("SEND") || command->name.equals_ci("SENDALL"))
			{
				if (DConfig.Check(DEFCON_NO_NEW_MEMOS))
				{
					source.Reply(_("Services are in Defcon mode, Please try again later."));
					return EVENT_STOP;
				}
			}
		}

		return EVENT_CONTINUE;
	}

	void OnUserConnect(dynamic_reference<User> &u, bool &exempt)
	{
		if (exempt || !u || !u->server->IsSynced() || u->server->IsULined())
			return;

		if (DConfig.Check(DEFCON_AKILL_NEW_CLIENTS))
		{
			if (DConfig.Check(DEFCON_AKILL_NEW_CLIENTS))
			{
				Log(operserv->Bot(), "operserv/defcon") << "DEFCON: adding akill for *@" << u->host;
				XLine *x = SGLine->Add("*@" + u->host, Config->s_OperServ, Anope::CurTime + DConfig.akillexpire, DConfig.akillreason);
				if (x)
					x->By = Config->s_OperServ;
			}

			if (DConfig.Check(DEFCON_NO_NEW_CLIENTS) || DConfig.Check(DEFCON_AKILL_NEW_CLIENTS))
			{
				u->Kill(Config->s_OperServ, DConfig.akillreason);
				return;
			}
		}

		if (!DConfig.sessionlimit)
			return;

		if (DConfig.Check(DEFCON_AKILL_NEW_CLIENTS))
		{
			Log(operserv->Bot(), "operserv/defcon") << "DEFCON: adding akill for *@" << u->host;
			XLine *x = SGLine->Add("*@" + u->host, Config->s_OperServ, Anope::CurTime + DConfig.akillexpire, !DConfig.akillreason.empty() ? DConfig.akillreason : "DEFCON AKILL");
			if (x)
				x->By = Config->s_OperServ;
		}

		if (DConfig.Check(DEFCON_NO_NEW_CLIENTS) || DConfig.Check(DEFCON_AKILL_NEW_CLIENTS))
		{
			u->Kill(Config->s_OperServ, DConfig.akillreason);
			return;
		}

		Session *session = session_service->FindSession(u->host);
		Exception *exception = session_service->FindException(u);

		if (DConfig.Check(DEFCON_REDUCE_SESSION) && !exception)
		{
			if (session && session->count > DConfig.sessionlimit)
			{
				if (!Config->SessionLimitExceeded.empty())
					ircdproto->SendMessage(operserv->Bot(), u->nick, Config->SessionLimitExceeded.c_str(), u->host.c_str());
				if (!Config->SessionLimitDetailsLoc.empty())
					ircdproto->SendMessage(operserv->Bot(), u->nick, "%s", Config->SessionLimitDetailsLoc.c_str());

				u->Kill(Config->s_OperServ, "Defcon session limit exceeded");
				++session->hits;
				if (Config->MaxSessionKill && session->hits >= Config->MaxSessionKill)
				{
					SGLine->Add("*@" + u->host, Config->s_OperServ, Anope::CurTime + Config->SessionAutoKillExpiry, "Defcon session limit exceeded");
					ircdproto->SendGlobops(operserv->Bot(), "[DEFCON] Added a temporary AKILL for \2*@%s\2 due to excessive connections", u->host.c_str());
				}
			}
		}
	}

	void OnChannelModeAdd(ChannelMode *cm)
	{
		if (DConfig.chanmodes.find(cm->ModeChar) != Anope::string::npos)
			this->ParseModeString();
	}

	void OnChannelCreate(Channel *c)
	{
		if (DConfig.Check(DEFCON_FORCE_CHAN_MODES))
			c->SetModes(operserv->Bot(), false, "%s", DConfig.chanmodes.c_str());
	}
};

/**
 * Send a message to the oper about which precautions are "active" for this level
 **/
void defcon_sendlvls(CommandSource &source)
{
	if (DConfig.Check(DEFCON_NO_NEW_CHANNELS))
		source.Reply(_("* No new channel registrations"));
	if (DConfig.Check(DEFCON_NO_NEW_NICKS))
		source.Reply(_("* No new nick registrations"));
	if (DConfig.Check(DEFCON_NO_MLOCK_CHANGE))
		source.Reply(_("* No MLOCK changes"));
	if (DConfig.Check(DEFCON_FORCE_CHAN_MODES) && !DConfig.chanmodes.empty())
		source.Reply(_("* Force Chan Modes (%s) to be set on all channels"), DConfig.chanmodes.c_str());
	if (DConfig.Check(DEFCON_REDUCE_SESSION))
		source.Reply(_("* Use the reduced session limit of %d"), DConfig.sessionlimit);
	if (DConfig.Check(DEFCON_NO_NEW_CLIENTS))
		source.Reply(_("* Kill any NEW clients connecting"));
	if (DConfig.Check(DEFCON_OPER_ONLY))
		source.Reply(_("* Ignore any non-opers with message"));
	if (DConfig.Check(DEFCON_SILENT_OPER_ONLY))
		source.Reply(_("* Silently ignore non-opers"));
	if (DConfig.Check(DEFCON_AKILL_NEW_CLIENTS))
		source.Reply(_("* AKILL any new clients connecting"));
	if (DConfig.Check(DEFCON_NO_NEW_MEMOS))
		source.Reply(_("* No new memos sent"));
}

void runDefCon()
{
	if (DConfig.Check(DEFCON_FORCE_CHAN_MODES))
	{
		if (!DConfig.chanmodes.empty() && !DefConModesSet)
		{
			if (DConfig.chanmodes[0] == '+' || DConfig.chanmodes[0] == '-')
			{
				Log(operserv->Bot(), "operserv/defcon") << "DEFCON: setting " << DConfig.chanmodes << " on all channels";
				DefConModesSet = true;
				for (channel_map::const_iterator it = ChannelList.begin(), it_end = ChannelList.end(); it != it_end; ++it)
					it->second->SetModes(operserv->Bot(), false, "%s", DConfig.chanmodes.c_str());
			}
		}
	}
	else
	{
		if (!DConfig.chanmodes.empty() && DefConModesSet)
		{
			if (DConfig.chanmodes[0] == '+' || DConfig.chanmodes[0] == '-')
			{
				DefConModesSet = false;
				Anope::string newmodes = defconReverseModes(DConfig.chanmodes);
				if (!newmodes.empty())
				{
					Log(operserv->Bot(), "operserv/defcon") << "DEFCON: setting " << newmodes << " on all channels";
					for (channel_map::const_iterator it = ChannelList.begin(), it_end = ChannelList.end(); it != it_end; ++it)
						it->second->SetModes(operserv->Bot(), false, "%s", newmodes.c_str());
				}
			}
		}
	}
}

static Anope::string defconReverseModes(const Anope::string &modes)
{
	if (modes.empty())
		return "";
	Anope::string newmodes;
	for (unsigned i = 0, end = modes.length(); i < end; ++i)
	{
		if (modes[i] == '+')
			newmodes += '-';
		else if (modes[i] == '-')
			newmodes += '+';
		else
			newmodes += modes[i];
	}
	return newmodes;
}

MODULE_INIT(OSDefcon)
