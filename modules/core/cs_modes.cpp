/* ChanServ core functions
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


class CommandModeBase : public Command
{
 protected:
	/** do_util: not a command, but does the job of others
	 * @param source The source of the command
	 * @param com The command calling this function
	 * @param cm A channel mode class
	 * @param chan The channel its being set on
	 * @param nick The nick the modes being set on
	 * @param set Is the mode being set or removed
	 * @param level The acecss level required to set this mode on someone else
	 * @param levelself The access level required to set this mode on yourself
	 * @param notice Flag required on a channel to send a notice
	 */
	CommandReturn do_util(CommandSource &source, Command *com, ChannelMode *cm, const Anope::string &chan, const Anope::string &nick, bool set, int level, int levelself, ChannelInfoFlag notice)
	{
		User *u = source.u;

		if (chan.empty())
			for (UChannelList::iterator it = u->chans.begin(); it != u->chans.end(); ++it)
				do_mode(source, com, cm, (*it)->chan->name, u->nick, set, level, levelself, notice);
		else
			do_mode(source, com, cm, chan, !nick.empty() ? nick : u->nick, set, level, levelself, notice);

		return MOD_CONT;
	}

	void do_mode(CommandSource &source, Command *com, ChannelMode *cm, const Anope::string &chan, const Anope::string &nick, bool set, int level, int levelself, ChannelInfoFlag notice)
	{
		User *u = source.u;
		User *u2 = finduser(nick);
		Channel *c = findchan(chan);
		ChannelInfo *ci = c ? c->ci : NULL;

		bool is_same = u == u2;

		ChanAccess *u_access = ci ? ci->GetAccess(u) : NULL, *u2_access = ci && u2 ? ci->GetAccess(u2) : NULL;
		uint16 u_level = u_access ? u_access->level : 0, u2_level = u2_access ? u2_access->level : 0;

		if (!c)
			source.Reply(_(CHAN_X_NOT_IN_USE), chan.c_str());
		else if (!ci)
			source.Reply(_(CHAN_X_NOT_REGISTERED), chan.c_str());
		else if (!u2)
			source.Reply(_(NICK_X_NOT_IN_USE), nick.c_str());
		else if (is_same ? !check_access(u, ci, levelself) : !check_access(u, ci, level))
			source.Reply(_(ACCESS_DENIED));
		else if (!set && !is_same && ci->HasFlag(CI_PEACE) && u2_level >= u_level)
			source.Reply(_(ACCESS_DENIED));
		else if (!set && u2->IsProtected() && !is_same)
			source.Reply(_(ACCESS_DENIED));
		else if (!c->FindUser(u2))
			source.Reply(_(NICK_X_NOT_ON_CHAN), u2->nick.c_str(), c->name.c_str());
		else
		{
			if (set)
				c->SetMode(NULL, cm, u2->nick);
			else
				c->RemoveMode(NULL, cm, u2->nick);

			Log(LOG_COMMAND, u, com, ci) << "for " << u2->nick;
			if (notice && ci->HasFlag(notice))
				ircdproto->SendMessage(whosends(ci), c->name, "%s command used for %s by %s", com->name.c_str(), u2->nick.c_str(), u->nick.c_str());
		}
	}


 public:
	CommandModeBase(const Anope::string &cname) : Command(cname, 0, 2) { }
};

class CommandCSOp : public CommandModeBase
{
 public:
	CommandCSOp() : CommandModeBase("OP")
	{
		this->SetDesc(_("Gives Op status to a selected nick on a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OP);

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", true, CA_OPDEOP, CA_OPDEOPME, CI_OPNOTICE);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002OP [\037#channel\037] [\037nick\037]\002\n"
				" \n"
				"Ops a selected nick on a channel. If nick is not given,\n"
				"it will op you. If channel is not given, it will op you\n"
				"on every channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access \n"
				"and above on the channel."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "OP", _("OP [\037#channel\037] [\037nick\037]\002"));
	}
};

class CommandCSDeOp : public CommandModeBase
{
 public:
	CommandCSDeOp() : CommandModeBase("DEOP")
	{
		this->SetDesc(_("Deops a selected nick on a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OP);

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", false, CA_OPDEOP, CA_OPDEOPME, CI_OPNOTICE);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002DEOP [\037#channel\037] [\037nick\037]\002\n"
				" \n"
				"Deops a selected nick on a channel. If nick is not given,\n"
				"it will deop you. If channel is not given, it will deop\n"
				"you on every channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access \n"
				"and above on the channel."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "DEOP", _("DEOP [\037#channel\037] [\037nick\037]\002"));
	}
};

class CommandCSVoice : public CommandModeBase
{
 public:
	CommandCSVoice() : CommandModeBase("VOICE")
	{
		this->SetDesc(_("Voices a selected nick on a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_VOICE);

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", true, CA_VOICE, CA_VOICEME, CI_BEGIN);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002VOICE [\037#channel\037] [\037nick\037]\002\n"
				" \n"
				"Voices a selected nick on a channel. If nick is not given,\n"
				"it will voice you. If channel is not given, it will voice you\n"
				"on every channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access \n"
				"and above on the channel, or to VOPs or those with level 3 \n"
				"and above for self voicing."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "VOICE", _("VOICE [\037#channel\037] [\037nick\037]\002"));
	}
};

class CommandCSDeVoice : public CommandModeBase
{
 public:
	CommandCSDeVoice() : CommandModeBase("DEVOICE")
	{
		this->SetDesc(_("Devoices a selected nick on a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_VOICE);

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", false, CA_VOICE, CA_VOICEME, CI_BEGIN);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002DEVOICE [\037#channel\037] [\037nick\037]\002\n"
				" \n"
				"Devoices a selected nick on a channel. If nick is not given,\n"
				"it will devoice you. If channel is not given, it will devoice\n"
				"you on every channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access \n"
				"and above on the channel, or to VOPs or those with level 3 \n"
				"and above for self devoicing."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "DEVOICE", _("DEVOICE [\037#channel\037] [\037nick\037]\002"));
	}
};

class CommandCSHalfOp : public CommandModeBase
{
 public:
	CommandCSHalfOp() : CommandModeBase("HALFOP")
	{
		this->SetDesc(_("Halfops a selected nick on a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_HALFOP);

		if (!cm)
			return MOD_CONT;

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", true, CA_HALFOP, CA_HALFOPME, CI_BEGIN);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002HALFOP [\037#channel\037] [\037nick\037]\002\n"
				" \n"
				"Halfops a selected nick on a channel. If nick is not given,\n"
				"it will halfop you. If channel is not given, it will halfop\n"
				"you on every channel.\n"
				" \n"
				"By default, limited to AOPs and those with level 5 access \n"
				"and above on the channel, or to HOPs or those with level 4 \n"));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "HALFOP", _("HALFOP [\037#channel\037] [\037nick\037]\002"));
	}
};

class CommandCSDeHalfOp : public CommandModeBase
{
 public:
	CommandCSDeHalfOp() : CommandModeBase("DEHALFOP")
	{
		this->SetDesc(_("Dehalfops a selected nick on a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_HALFOP);

		if (!cm)
			return MOD_CONT;

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", false, CA_HALFOP, CA_HALFOPME, CI_BEGIN);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002DEHALFOP [\037#channel\037] [\037nick\037]\002\n"
				" \n"
				"Dehalfops a selected nick on a channel. If nick is not given,\n"
				"it will dehalfop you. If channel is not given, it will dehalfop\n"
				"you on every channel.\n"
				" \n"
				"By default, limited to AOPs and those with level 5 access \n"
				"and above on the channel, or to HOPs or those with level 4 \n"
				"and above for self dehalfopping."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "DEHALFOP", _("DEHALFOP [\037#channel\037] [\037nick\037]\002"));
	}
};

class CommandCSProtect : public CommandModeBase
{
 public:
	CommandCSProtect() : CommandModeBase("PROTECT")
	{
		this->SetDesc(_("Protects a selected nick on a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PROTECT);

		if (!cm)
			return MOD_CONT;

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", true, CA_PROTECT, CA_PROTECTME, CI_BEGIN);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002PROTECT [\037#channel\037] [\037nick\037]\002\n"
				" \n"
				"Protects a selected nick on a channel. If nick is not given,\n"
				"it will protect you. If channel is not given, it will protect\n"
				"you on every channel.\n"
				" \n"
				"By default, limited to the founder, or to SOPs or those with \n"
				"level 10 and above on the channel for self protecting."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "PROTECT", _("PROTECT [\037#channel\037] [\037nick\037]\002"));
	}
};

class CommandCSDeProtect : public CommandModeBase
{
 public:
	CommandCSDeProtect() : CommandModeBase("DEPROTECT")
	{
		this->SetDesc(_("Deprotects a selected nick on a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PROTECT);

		if (!cm)
			return MOD_CONT;

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", false, CA_PROTECT, CA_PROTECTME, CI_BEGIN);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002DEPROTECT [\037#channel\037] [\037nick\037]\002\n"
				" \n"
				"Deprotects a selected nick on a channel. If nick is not given,\n"
				"it will deprotect you. If channel is not given, it will deprotect\n"
				"you on every channel.\n"
				" \n"
				"By default, limited to the founder, or to SOPs or those with \n"));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "DEPROTECT", _("DEROTECT [\037#channel\037] [\037nick\037]\002"));
	}
};

class CommandCSOwner : public CommandModeBase
{
 public:
	CommandCSOwner() : CommandModeBase("OWNER")
	{
		this->SetDesc(_("Gives you owner status on channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OWNER);

		if (!cm)
			return MOD_CONT;

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", true, CA_OWNER, CA_OWNERME, CI_BEGIN);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002OWNER [\037#channel\037] [\037nick\037]\002\n"
				" \n"
				"Gives the selected nick owner status on \002channel\002. If nick is not\n"
				"given, it will give you owner. If channel is not given, it will\n"
				"give you owner on every channel.\n"
				" \n"
				"Limited to those with founder access on the channel."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "OWNER", _("OWNER [\037#channel\037] [\037nick\037]\002"));
	}
};

class CommandCSDeOwner : public CommandModeBase
{
 public:
	CommandCSDeOwner() : CommandModeBase("DEOWNER")
	{
		this->SetDesc(_("Removes your owner status on a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OWNER);

		if (!cm)
			return MOD_CONT;

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", false, CA_OWNER, CA_OWNERME, CI_BEGIN);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002DEOWNER [\037#channel\037] [\037nick\037]\002\n"
				" \n"
				"Removes owner status from the selected nick on \002channel\002. If nick\n"
				"is not given, it will deowner you. If channel is not given, it will\n"
				"deowner you on every channel.\n"
				" \n"
				"Limited to those with founder access on the channel."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "DEOWNER", _("DEOWNER [\037#channel\037] [\037nick\037]\002"));
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
