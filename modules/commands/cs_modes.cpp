/* ChanServ core functions
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

class CommandModeBase : public Command
{
	void do_mode(CommandSource &source, Command *com, ChannelMode *cm, const Anope::string &chan, const Anope::string &nick, bool set, const Anope::string &level, const Anope::string &levelself)
	{
		User *u = source.GetUser();
		User *u2 = finduser(nick);
		Channel *c = findchan(chan);

		if (!c)
		{
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
			return;

		}

		if (!u2)
		{
			source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
			return;
		}

		ChannelInfo *ci = c->ci;
		if (!ci)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, c->name.c_str());
			return;
		}

		bool is_same = u == u2;
		AccessGroup u_access = source.AccessFor(ci), u2_access = ci->AccessFor(u2);

		if (is_same ? !source.AccessFor(ci).HasPriv(levelself) : !source.AccessFor(ci).HasPriv(level))
			source.Reply(ACCESS_DENIED);
		else if (!set && !is_same && ci->HasFlag(CI_PEACE) && u2_access >= u_access)
			source.Reply(ACCESS_DENIED);
		else if (!set && u2->IsProtected() && !is_same)
			source.Reply(ACCESS_DENIED);
		else if (!c->FindUser(u2))
			source.Reply(NICK_X_NOT_ON_CHAN, u2->nick.c_str(), c->name.c_str());
		else
		{
			if (set)
				c->SetMode(NULL, cm, u2->nick);
			else
				c->RemoveMode(NULL, cm, u2->nick);

			Log(LOG_COMMAND, source, com, ci) << "for " << u2->nick;
		}
	}

 protected:
	/** do_util: not a command, but does the job of others
	 * @param source The source of the command
	 * @param com The command calling this function
	 * @param cm A channel mode class
	 * @param chan The channel its being set on
	 * @param nick The nick the modes being set on
	 * @param set Is the mode being set or removed
	 * @param level The access level required to set this mode on someone else
	 * @param levelself The access level required to set this mode on yourself
	 */
	void do_util(CommandSource &source, Command *com, ChannelMode *cm, const Anope::string &chan, const Anope::string &nick, bool set, const Anope::string &level, const Anope::string &levelself)
	{
		User *u = source.GetUser();

		if (chan.empty() && u)
			for (UChannelList::iterator it = u->chans.begin(); it != u->chans.end(); ++it)
				do_mode(source, com, cm, (*it)->chan->name, u->nick, set, level, levelself);
		else if (!chan.empty())
			do_mode(source, com, cm, chan, !nick.empty() ? nick : u->nick, set, level, levelself);
	}

 public:
	CommandModeBase(Module *creator, const Anope::string &cname) : Command(creator, cname, 0, 2)
	{
		this->SetSyntax(_("[\037channel\037] [\037nick\037]"));
	}
};

class CommandCSOp : public CommandModeBase
{
 public:
	CommandCSOp(Module *creator) : CommandModeBase(creator, "chanserv/op")
	{
		this->SetDesc(_("Gives Op status to a selected nick on a channel"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OP);

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", true, "OPDEOP", "OPDEOPME");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Ops a selected nick on a channel. If nick is not given,\n"
				"it will op you. If channel is not given, it will op you\n"
				"on every channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access\n"
				"and above on the channel."));
		return true;
	}
};

class CommandCSDeOp : public CommandModeBase
{
 public:
	CommandCSDeOp(Module *creator) : CommandModeBase(creator, "chanserv/deop")
	{
		this->SetDesc(_("Deops a selected nick on a channel"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OP);

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", false, "OPDEOP", "OPDEOPME");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply("Deops a selected nick on a channel. If nick is not given,\n"
				"it will deop you. If channel is not given, it will deop\n"
				"you on every channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access\n"
				"and above on the channel.");
		return true;
	}
};

class CommandCSVoice : public CommandModeBase
{
 public:
	CommandCSVoice(Module *creator) : CommandModeBase(creator, "chanserv/voice")
	{
		this->SetDesc(_("Voices a selected nick on a channel"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_VOICE);

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", true, "VOICE", "VOICEME");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Voices a selected nick on a channel. If nick is not given,\n"
				"it will voice you. If channel is not given, it will voice you\n"
				"on every channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access\n"
				"and above on the channel, or to VOPs or those with level 3\n"
				"and above for self voicing."));
		return true;
	}
};

class CommandCSDeVoice : public CommandModeBase
{
 public:
	CommandCSDeVoice(Module *creator) : CommandModeBase(creator, "chanserv/devoice")
	{
		this->SetDesc(_("Devoices a selected nick on a channel"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_VOICE);

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", false, "VOICE", "VOICEME");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Devoices a selected nick on a channel. If nick is not given,\n"
				"it will devoice you. If channel is not given, it will devoice\n"
				"you on every channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 access\n"
				"and above on the channel, or to VOPs or those with level 3\n"
				"and above for self devoicing."));
		return true;
	}
};

class CommandCSHalfOp : public CommandModeBase
{
 public:
	CommandCSHalfOp(Module *creator) : CommandModeBase(creator, "chanserv/halfop")
	{
		this->SetDesc(_("Halfops a selected nick on a channel"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_HALFOP);

		if (!cm)
			return;

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", true, "HALFOP", "HALFOPME");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Halfops a selected nick on a channel. If nick is not given,\n"
				"it will halfop you. If channel is not given, it will halfop\n"
				"you on every channel.\n"
				" \n"
				"By default, limited to AOPs and those with level 5 access\n"
				"and above on the channel, or to HOPs or those with level 4\n"
				"and above for self halfopping."));
		return true;
	}
};

class CommandCSDeHalfOp : public CommandModeBase
{
 public:
	CommandCSDeHalfOp(Module *creator) : CommandModeBase(creator, "chanserv/dehalfop")
	{
		this->SetDesc(_("Dehalfops a selected nick on a channel"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_HALFOP);

		if (!cm)
			return;

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", false, "HALFOP", "HALFOPME");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Dehalfops a selected nick on a channel. If nick is not given,\n"
				"it will dehalfop you. If channel is not given, it will dehalfop\n"
				"you on every channel.\n"
				" \n"
				"By default, limited to AOPs and those with level 5 access\n"
				"and above on the channel, or to HOPs or those with level 4\n"
				"and above for self dehalfopping."));
		return true;
	}
};

class CommandCSProtect : public CommandModeBase
{
 public:
	CommandCSProtect(Module *creator) : CommandModeBase(creator, "chanserv/protect")
	{
		this->SetDesc(_("Protects a selected nick on a channel"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PROTECT);

		if (!cm)
			return;

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", true, "PROTECT", "PROTECTME");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Protects a selected nick on a channel. If nick is not given,\n"
				"it will protect you. If channel is not given, it will protect\n"
				"you on every channel.\n"
				" \n"
				"By default, limited to the founder, or to SOPs or those with\n"
				"level 10 and above on the channel for self protecting."));
		return true;
	}
};

class CommandCSDeProtect : public CommandModeBase
{
 public:
	CommandCSDeProtect(Module *creator) : CommandModeBase(creator, "chanserv/deprotect")
	{
		this->SetDesc(_("Deprotects a selected nick on a channel"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PROTECT);

		if (!cm)
			return;

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", false, "PROTECT", "PROTECTME");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Deprotects a selected nick on a channel. If nick is not given,\n"
				"it will deprotect you. If channel is not given, it will deprotect\n"
				"you on every channel.\n"
				" \n"
				"By default, limited to the founder, or to SOPs or those with\n"
				"level 10 and above on the channel for self deprotecting."));
		return true;
	}
};

class CommandCSOwner : public CommandModeBase
{
 public:
	CommandCSOwner(Module *creator) : CommandModeBase(module, "chanserv/owner")
	{
		this->SetDesc(_("Gives you owner status on channel"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OWNER);

		if (!cm)
			return;

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", true, "OWNER", "OWNERME");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Gives the selected nick owner status on \002channel\002. If nick is not\n"
				"given, it will give you owner. If channel is not given, it will\n"
				"give you owner on every channel.\n"
				" \n"
				"Limited to those with founder access on the channel."));
		return true;
	}
};

class CommandCSDeOwner : public CommandModeBase
{
 public:
	CommandCSDeOwner(Module *creator) : CommandModeBase(creator, "chanserv/deowner")
	{
		this->SetDesc(_("Removes your owner status on a channel"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_OWNER);

		if (!cm)
			return;

		return do_util(source, this, cm, !params.empty() ? params[0] : "", params.size() > 1 ? params[1] : "", false, "OWNER", "OWNERME");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Removes owner status from the selected nick on \002channel\002. If nick\n"
				"is not given, it will deowner you. If channel is not given, it will\n"
				"deowner you on every channel.\n"
				" \n"
				"Limited to those with founder access on the channel."));
		return true;
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
	CSModes(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsowner(this), commandcsdeowner(this), commandcsprotect(this), commandcsdeprotect(this),
		commandcsop(this), commandcsdeop(this), commandcshalfop(this), commandcsdehalfop(this),
		commandcsvoice(this), commandcsdevoice(this)
	{
		this->SetAuthor("Anope");

		
	}
};

MODULE_INIT(CSModes)
