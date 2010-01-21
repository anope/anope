/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
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

/** do_util: not a command, but does the job of others
 * @param u The user doing the command
 * @param cm A channel mode class
 * @param chan The channel its being set on
 * @param nick The nick the modes being set on
 * @param set Is the mode being set or removed
 * @param level The acecss level required to set this mode on someone else
 * @param levelself The access level required to set this mode on yourself
 * @param name The name, eg "OP" or "HALFOP"
 * @param notice Flag required on a channel to send a notice
 */
static CommandReturn do_util(User *u, ChannelMode *cm, const char *chan, const char *nick, bool set, int level, int levelself, const std::string &name, ChannelInfoFlag notice)
{
	Channel *c = findchan(chan);
	ChannelInfo *ci;
	User *u2;

	int is_same;

	if (!nick)
		nick = u->nick.c_str();

	is_same = (nick == u->nick) ? 1 : (stricmp(nick, u->nick.c_str()) == 0);

	if (c)
		ci = c->ci;

	if (!c)
		notice_lang(Config.s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
	else if (is_same ? !(u2 = u) : !(u2 = finduser(nick)))
		notice_lang(Config.s_ChanServ, u, NICK_X_NOT_IN_USE, nick);
	else if (!c->FindUser(u2))
		notice_lang(Config.s_ChanServ, u, NICK_X_NOT_ON_CHAN, u2->nick.c_str(), c->name.c_str());
	else if (is_same ? !check_access(u, ci, levelself) : !check_access(u, ci, level))
		notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
	else if (!set && !is_same && (ci->HasFlag(CI_PEACE)) && (get_access(u2, ci) >= get_access(u, ci)))
		notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
	else if (!set && is_protected(u2) && !is_same)
		notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
	else
	{
		if (set)
			c->SetMode(NULL, cm, u2->nick);
		else
			c->RemoveMode(NULL, cm, u2->nick);

		if (notice && ci->HasFlag(notice))
			ircdproto->SendMessage(whosends(ci), c->name.c_str(), "%s command used for %s by %s",
				   name.c_str(), u2->nick.c_str(), u->nick.c_str());
	}

	return MOD_CONT;
}

class CommandCSOp : public Command
{
 public:
	CommandCSOp() : Command("OP", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OP);

		return do_util(u, cm, (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL), true, CA_OPDEOP, CA_OPDEOPME, "OP", CI_OPNOTICE);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_OP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "OP", CHAN_OP_SYNTAX);
	}
};


class CommandCSDeOp : public Command
{
 public:
	CommandCSDeOp() : Command("DEOP", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OP);

		return do_util(u, cm, (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL), false, CA_OPDEOP, CA_OPDEOPME, "DEOP", CI_OPNOTICE);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_DEOP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "DEOP", CHAN_DEOP_SYNTAX);
	}
};


class CommandCSVoice : public Command
{
 public:
	CommandCSVoice() : Command("VOICE", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_VOICE);

		return do_util(u, cm, (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL), true, CA_VOICE, CA_VOICEME, "VOICE", CI_BEGIN);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_VOICE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "VOICE", CHAN_VOICE_SYNTAX);
	}
};


class CommandCSDeVoice : public Command
{
 public:
	CommandCSDeVoice() : Command("DEVOICE", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_VOICE);

		return do_util(u, cm, (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL), false, CA_VOICE, CA_VOICEME, "DEVOICE", CI_BEGIN);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_DEVOICE);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "DEVOICE", CHAN_DEVOICE_SYNTAX);
	}
};


class CommandCSHalfOp : public Command
{
 public:
	CommandCSHalfOp() : Command("HALFOP", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_HALFOP);
		
		if (!cm)
		{
			return MOD_CONT;
		}

		return do_util(u, cm, (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL), true, CA_HALFOP, CA_HALFOPME, "HALFOP", CI_BEGIN);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_HALFOP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "HALFOP", CHAN_HALFOP_SYNTAX);
	}
};


class CommandCSDeHalfOp : public Command
{
 public:
	CommandCSDeHalfOp() : Command("DEHALFOP", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_HALFOP);

		if (!cm)
		{
			return MOD_CONT;

		}

		return do_util(u, cm, (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL), false, CA_HALFOP, CA_HALFOPME, "DEHALFOP", CI_BEGIN);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_DEHALFOP);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "DEHALFOP", CHAN_DEHALFOP_SYNTAX);
	}
};


class CommandCSProtect : public Command
{
 public:
	CommandCSProtect() : Command("PROTECT", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PROTECT);

		if (!cm)
		{
			return MOD_CONT;
		}

		return do_util(u, cm, (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL), true, CA_PROTECT, CA_PROTECTME, "PROTECT", CI_BEGIN);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_PROTECT);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "PROTECT", CHAN_PROTECT_SYNTAX);
	}
};

class CommandCSDeProtect : public Command
{
 public:
	CommandCSDeProtect() : Command("DEPROTECT", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PROTECT);

		if (!cm)
		{
			return MOD_CONT;
		}

		return do_util(u, cm, (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL), false, CA_PROTECT, CA_PROTECTME, "DEPROTECT", CI_BEGIN);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_DEPROTECT);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "DEPROTECT", CHAN_DEPROTECT_SYNTAX);
	}
};

class CommandCSOwner : public Command
{
 public:
	CommandCSOwner() : Command("OWNER", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OWNER);

		if (!cm)
		{
			return MOD_CONT;
		}

		return do_util(u, cm, (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL), true, CA_OWNER, CA_OWNERME, "OWNER", CI_BEGIN);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_OWNER);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "OWNER", CHAN_OWNER_SYNTAX);
	}
};

class CommandCSDeOwner : public Command
{
 public:
	CommandCSDeOwner() : Command("DEOWNER", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OWNER);

		if (!cm)
		{
			return MOD_CONT;
		}

		return do_util(u, cm, (params.size() > 0 ? params[0].c_str() : NULL), (params.size() > 1 ? params[1].c_str() : NULL), false, CA_OWNER, CA_OWNERME, "DEOWNER", CI_BEGIN);
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_DEOWNER);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "DEOWNER", CHAN_DEOWNER_SYNTAX);
	}
};


class CSModes : public Module
{
 public:
	CSModes(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(CHANSERV, new CommandCSOp());
		this->AddCommand(CHANSERV, new CommandCSDeOp());
		this->AddCommand(CHANSERV, new CommandCSVoice());
		this->AddCommand(CHANSERV, new CommandCSDeVoice());

		if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
		{
			this->AddCommand(CHANSERV, new CommandCSHalfOp());
			this->AddCommand(CHANSERV, new CommandCSDeHalfOp());
		}

		if (ModeManager::FindChannelModeByName(CMODE_PROTECT))
		{
			this->AddCommand(CHANSERV, new CommandCSProtect());
			this->AddCommand(CHANSERV, new CommandCSDeProtect());
		}

		if (ModeManager::FindChannelModeByName(CMODE_OWNER))
		{
			this->AddCommand(CHANSERV, new CommandCSOwner());
			this->AddCommand(CHANSERV, new CommandCSDeOwner());
		}

		ModuleManager::Attach(I_OnChanServHelp, this);
	}
	void OnChanServHelp(User *u)
	{
		if (ModeManager::FindChannelModeByName(CMODE_OWNER))
		{
			notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_OWNER);
			notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_DEOWNER);
		}

		if (ModeManager::FindChannelModeByName(CMODE_PROTECT))
		{
			notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_PROTECT);
			notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_DEPROTECT);
		}

		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_OP);
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_DEOP);

		if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
		{
			notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_HALFOP);
			notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_DEHALFOP);
		}

		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_VOICE);
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_DEVOICE);
	}
};


MODULE_INIT(CSModes)
