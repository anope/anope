/* ChanServ core functions
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

#include "module.h"

/** do_util: not a command, but does the job of others
 * @param u The user doing the command
 * @param com The command calling this function
 * @param cm A channel mode class
 * @param chan The channel its being set on
 * @param nick The nick the modes being set on
 * @param set Is the mode being set or removed
 * @param level The acecss level required to set this mode on someone else
 * @param levelself The access level required to set this mode on yourself
 * @param name The name, eg "OP" or "HALFOP"
 * @param notice Flag required on a channel to send a notice
 */
static CommandReturn do_util(User *u, Command *com, ChannelMode *cm, const Anope::string &chan, const Anope::string &nick, bool set, int level, int levelself, const Anope::string &name, ChannelInfoFlag notice)
{
	Channel *c = findchan(chan);
	ChannelInfo *ci = c ? c->ci : NULL;
	User *u2;

	Anope::string realnick = (!nick.empty() ? nick : u->nick);
	bool is_same = u->nick.equals_ci(realnick);

	if (!c)
		u->SendMessage(ChanServ, CHAN_X_NOT_IN_USE, chan.c_str());
	else if (is_same ? !(u2 = u) : !(u2 = finduser(realnick)))
		u->SendMessage(ChanServ, NICK_X_NOT_IN_USE, realnick.c_str());
	else if (is_same ? !check_access(u, ci, levelself) : !check_access(u, ci, level))
		u->SendMessage(ChanServ, ACCESS_DENIED);
	else if (!set && !is_same && (ci->HasFlag(CI_PEACE)) && (get_access(u2, ci) >= get_access(u, ci)))
		u->SendMessage(ChanServ, ACCESS_DENIED);
	else if (!set && u2->IsProtected() && !is_same)
		u->SendMessage(ChanServ, ACCESS_DENIED);
	else if (!c->FindUser(u2))
		u->SendMessage(ChanServ, NICK_X_NOT_ON_CHAN, u2->nick.c_str(), c->name.c_str());
	else
	{
		if (set)
			c->SetMode(NULL, cm, u2->nick);
		else
			c->RemoveMode(NULL, cm, u2->nick);

		Log(LOG_COMMAND, u, com, ci) << "for " << u2->nick;
		if (notice && ci->HasFlag(notice))
			ircdproto->SendMessage(whosends(ci), c->name, "%s command used for %s by %s", name.c_str(), u2->nick.c_str(), u->nick.c_str());
	}

	return MOD_CONT;
}

class CommandCSOp : public Command
{
 public:
	CommandCSOp() : Command("OP", 1, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OP);

		return do_util(u, this, cm, params[0], params.size() > 1 ? params[1] : "", true, CA_OPDEOP, CA_OPDEOPME, "OP", CI_OPNOTICE);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_OP);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "OP", CHAN_OP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_OP);
	}
};

class CommandCSDeOp : public Command
{
 public:
	CommandCSDeOp() : Command("DEOP", 1, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OP);

		return do_util(u, this, cm, params[0], params.size() > 1 ? params[1] : "", false, CA_OPDEOP, CA_OPDEOPME, "DEOP", CI_OPNOTICE);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_DEOP);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "DEOP", CHAN_DEOP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_DEOP);
	}
};

class CommandCSVoice : public Command
{
 public:
	CommandCSVoice() : Command("VOICE", 1, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_VOICE);

		return do_util(u, this, cm, params[0], params.size() > 1 ? params[1] : "", true, CA_VOICE, CA_VOICEME, "VOICE", CI_BEGIN);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_VOICE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "VOICE", CHAN_VOICE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_VOICE);
	}
};

class CommandCSDeVoice : public Command
{
 public:
	CommandCSDeVoice() : Command("DEVOICE", 1, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_VOICE);

		return do_util(u, this, cm, params[0], params.size() > 1 ? params[1] : "", false, CA_VOICE, CA_VOICEME, "DEVOICE", CI_BEGIN);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_DEVOICE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "DEVOICE", CHAN_DEVOICE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_DEVOICE);
	}
};

class CommandCSHalfOp : public Command
{
 public:
	CommandCSHalfOp() : Command("HALFOP", 1, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_HALFOP);

		if (!cm)
			return MOD_CONT;

		return do_util(u, this, cm, params[0], params.size() > 1 ? params[1] : "", true, CA_HALFOP, CA_HALFOPME, "HALFOP", CI_BEGIN);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_HALFOP);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "HALFOP", CHAN_HALFOP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_HALFOP);
	}
};

class CommandCSDeHalfOp : public Command
{
 public:
	CommandCSDeHalfOp() : Command("DEHALFOP", 1, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_HALFOP);

		if (!cm)
			return MOD_CONT;

		return do_util(u, this, cm, params[0], params.size() > 1 ? params[1] : "", false, CA_HALFOP, CA_HALFOPME, "DEHALFOP", CI_BEGIN);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_DEHALFOP);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "DEHALFOP", CHAN_DEHALFOP_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_DEHALFOP);
	}
};

class CommandCSProtect : public Command
{
 public:
	CommandCSProtect() : Command("PROTECT", 1, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PROTECT);

		if (!cm)
			return MOD_CONT;

		return do_util(u, this, cm, params[0], params.size() > 1 ? params[1] : "", true, CA_PROTECT, CA_PROTECTME, "PROTECT", CI_BEGIN);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_PROTECT);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "PROTECT", CHAN_PROTECT_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_PROTECT);
	}
};

class CommandCSDeProtect : public Command
{
 public:
	CommandCSDeProtect() : Command("DEPROTECT", 1, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PROTECT);

		if (!cm)
			return MOD_CONT;

		return do_util(u, this, cm, params[0], params.size() > 1 ? params[1] : "", false, CA_PROTECT, CA_PROTECTME, "DEPROTECT", CI_BEGIN);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_DEPROTECT);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "DEPROTECT", CHAN_DEPROTECT_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_DEPROTECT);
	}
};

class CommandCSOwner : public Command
{
 public:
	CommandCSOwner() : Command("OWNER", 1, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OWNER);

		if (!cm)
			return MOD_CONT;

		return do_util(u, this, cm, params[0], params.size() > 1 ? params[1] : "", true, CA_OWNER, CA_OWNERME, "OWNER", CI_BEGIN);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_OWNER);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "OWNER", CHAN_OWNER_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_OWNER);
	}
};

class CommandCSDeOwner : public Command
{
 public:
	CommandCSDeOwner() : Command("DEOWNER", 1, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OWNER);

		if (!cm)
			return MOD_CONT;

		return do_util(u, this, cm, params[0], params.size() > 1 ? params[1] : "", false, CA_OWNER, CA_OWNERME, "DEOWNER", CI_BEGIN);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_DEOWNER);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "DEOWNER", CHAN_DEOWNER_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_DEOWNER);
	}
};

class CSModes : public Module
{
	CommandCSOwner commandcsowner;
	CommandCSDeOwner commandcsdeowner;
	CommandCSProtect commandcsprotect;
	CommandCSDeProtect commandcsdeprotect;
	CommandCSOp commandcsop;
	CommandCSDeOp commandcsdeop;
	CommandCSHalfOp commandcshalfop;
	CommandCSDeHalfOp commandcsdehalfop;
	CommandCSVoice commandcsvoice;
	CommandCSDeVoice commandcsdevoice;

 public:
	CSModes(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsop);
		this->AddCommand(ChanServ, &commandcsdeop);
		this->AddCommand(ChanServ, &commandcsvoice);
		this->AddCommand(ChanServ, &commandcsdevoice);

		if (Me && Me->IsSynced())
			OnUplinkSync(NULL);

		Implementation i[] = {I_OnUplinkSync, I_OnServerDisconnect};
		ModuleManager::Attach(i, this, 2);
	}

	void OnUplinkSync(Server *)
	{
		if (ModeManager::FindChannelModeByName(CMODE_OWNER))
		{
			this->AddCommand(ChanServ, &commandcsowner);
			this->AddCommand(ChanServ, &commandcsdeowner);
		}

		if (ModeManager::FindChannelModeByName(CMODE_PROTECT))
		{
			this->AddCommand(ChanServ, &commandcsprotect);
			this->AddCommand(ChanServ, &commandcsdeprotect);
		}

		if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
		{
			this->AddCommand(ChanServ, &commandcshalfop);
			this->AddCommand(ChanServ, &commandcsdehalfop);
		}
	}

	void OnServerDisconnect()
	{
		this->DelCommand(ChanServ, &commandcsowner);
		this->DelCommand(ChanServ, &commandcsdeowner);
		this->DelCommand(ChanServ, &commandcsprotect);
		this->DelCommand(ChanServ, &commandcsdeprotect);
		this->DelCommand(ChanServ, &commandcshalfop);
		this->DelCommand(ChanServ, &commandcsdehalfop);
	}
};

MODULE_INIT(CSModes)
