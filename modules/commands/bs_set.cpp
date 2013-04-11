/* BotServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandBSSet : public Command
{
 public:
	CommandBSSet(Module *creator) : Command(creator, "botserv/set", 3, 3)
	{
		this->SetDesc(_("Configures bot options"));
		this->SetSyntax(_("\037option\037 \037(channel | bot)\037 \037settings\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Configures bot options.\n"
			" \n"
			"Available options:"));
		Anope::string this_name = source.command;
		for (CommandInfo::map::const_iterator it = source.service->commands.begin(), it_end = source.service->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;
			if (c_name.find_ci(this_name + " ") == 0)
			{
				ServiceReference<Command> command("Command", info.name);
				if (command)
				{
					source.command = it->first;
					command->OnServHelp(source);
				}
			}
		}
		source.Reply(_("Type \002%s%s HELP %s \037option\037\002 for more information on a\n"
				"particular option."), Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str(), this_name.c_str());

		return true;
	}
};

class CommandBSSetBanExpire : public Command
{
 public:
 	class UnbanTimer : public Timer
	{
		Anope::string chname;
		Anope::string mask;

	 public:
		UnbanTimer(Module *creator, const Anope::string &ch, const Anope::string &bmask, time_t t) : Timer(creator, t), chname(ch), mask(bmask) { }

		void Tick(time_t) anope_override
		{
			Channel *c = Channel::Find(chname);
			if (c)
				c->RemoveMode(NULL, "BAN", mask);
		}
	};

	CommandBSSetBanExpire(Module *creator, const Anope::string &sname = "botserv/set/banexpire") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Configures the time bot bans expire in"));
		this->SetSyntax(_("\037channel\037 \037time\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &chan = params[0];
		const Anope::string &arg = params[1];

		ChannelInfo *ci = ChannelInfo::Find(chan);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
			return;
		}

		AccessGroup access = source.AccessFor(ci);
		if (!source.HasPriv("botserv/administration") && !access.HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bot option setting is temporarily disabled."));
			return;
		}

		ci->banexpire = Anope::DoTime(arg);

		bool override = !access.HasPriv("SET");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to change banexpire to " << ci->banexpire;

		if (!ci->banexpire)
			source.Reply(_("Bot bans will no longer automatically expire."));
		else
			source.Reply(_("Bot bans will automatically expire after %s."), Anope::Duration(ci->banexpire, source.GetAccount()).c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
				"Sets the time bot bans expire in. If enabled, any bans placed by\n"
				"bots, such as flood kicker, badwords kicker, etc. will automatically\n"
				"be removed after the given time. Set to 0 to disable bans from\n"
				"automatically expiring."));
		return true;
	}
};

class CommandBSSetDontKickOps : public Command
{
 public:
	CommandBSSetDontKickOps(Module *creator, const Anope::string &sname = "botserv/set/dontkickops") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("To protect ops against bot kicks"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		AccessGroup access = source.AccessFor(ci);
		if (!source.HasPriv("botserv/administration") && !access.HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bot option setting is temporarily disabled."));
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			bool override = !access.HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enable dontkickops"; 

			ci->ExtendMetadata("BS_DONTKICKOPS");
			source.Reply(_("Bot \002won't kick ops\002 on channel %s."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			bool override = !access.HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable dontkickops"; 

			ci->Shrink("BS_DONTKICKOPS");
			source.Reply(_("Bot \002will kick ops\002 on channel %s."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
				"Enables or disables \002ops protection\002 mode on a channel.\n"
				"When it is enabled, ops won't be kicked by the bot\n"
				"even if they don't match the NOKICK level."));
		return true;
	}
};

class CommandBSSetDontKickVoices : public Command
{
 public:
	CommandBSSetDontKickVoices(Module *creator, const Anope::string &sname = "botserv/set/dontkickvoices") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("To protect voices against bot kicks"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		AccessGroup access = source.AccessFor(ci);
		if (!source.HasPriv("botserv/administration") && !access.HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bot option setting is temporarily disabled."));
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			bool override = !access.HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enable dontkickvoices"; 

			ci->ExtendMetadata("BS_DONTKICKVOICES");
			source.Reply(_("Bot \002won't kick voices\002 on channel %s."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			bool override = !access.HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable dontkickvoices"; 

			ci->Shrink("BS_DONTKICKVOICES");
			source.Reply(_("Bot \002will kick voices\002 on channel %s."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
				"Enables or disables \002voices protection\002 mode on a channel.\n"
				"When it is enabled, voices won't be kicked by the bot\n"
				"even if they don't match the NOKICK level."));
		return true;
	}
};

class CommandBSSetFantasy : public Command
{
 public:
	CommandBSSetFantasy(Module *creator, const Anope::string &sname = "botserv/set/fantasy") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Enable fantaisist commands"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		const Anope::string &value = params[1];

		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!source.HasPriv("botserv/administration") && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bot option setting is temporarily disabled."));
			return;
		}

		if (value.equals_ci("ON"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enable fantasy"; 

			ci->ExtendMetadata("BS_FANTASY");
			source.Reply(_("Fantasy mode is now \002on\002 on channel %s."), ci->name.c_str());
		}
		else if (value.equals_ci("OFF"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable fantasy"; 

			ci->Shrink("BS_FANTASY");
			source.Reply(_("Fantasy mode is now \002off\002 on channel %s."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
				"Enables or disables \002fantasy\002 mode on a channel.\n"
				"When it is enabled, users will be able to use\n"
				"%s commands on a channel when prefixed\n"
				"with one of the following fantasy characters: \002%s\002\n"
				" \n"
				"Note that users wanting to use fantaisist\n"
				"commands MUST have enough level for both\n"
				"the FANTASIA and another level depending\n"
				"of the command if required (for example, to use\n"
				"!op, user must have enough access for the OPDEOP\n"
				"level)."), Config->ChanServ.c_str(), Config->BSFantasyCharacter.c_str());
		return true;
	}
};

class CommandBSSetGreet : public Command
{
 public:
	CommandBSSetGreet(Module *creator, const Anope::string &sname = "botserv/set/greet") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Enable greet messages"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		const Anope::string &value = params[1];

		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!source.HasPriv("botserv/administration") && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bot option setting is temporarily disabled."));
			return;
		}

		if (value.equals_ci("ON"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enable greets"; 

			ci->ExtendMetadata("BS_GREET");
			source.Reply(_("Greet mode is now \002on\002 on channel %s."), ci->name.c_str());
		}
		else if (value.equals_ci("OFF"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable greets"; 

			ci->Shrink("BS_GREET");
			source.Reply(_("Greet mode is now \002off\002 on channel %s."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
			"Enables or disables \002greet\002 mode on a channel.\n"
			"When it is enabled, the bot will display greet\n"
			"messages of users joining the channel, provided\n"
			"they have enough access to the channel."));
		return true;
	}
};

class CommandBSSetNoBot : public Command
{
 public:
	CommandBSSetNoBot(Module *creator, const Anope::string &sname = "botserv/set/nobot") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Prevent a bot from being assigned to a channel"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		const Anope::string &value = params[1];

		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (value.equals_ci("ON"))
		{
			Log(LOG_ADMIN, source, this, ci) << "to enable nobot"; 

			ci->ExtendMetadata("BS_NOBOT");
			if (ci->bi)
				ci->bi->UnAssign(source.GetUser(), ci);
			source.Reply(_("No-bot mode is now \002on\002 on channel %s."), ci->name.c_str());
		}
		else if (value.equals_ci("OFF"))
		{
			Log(LOG_ADMIN, source, this, ci) << "to disable nobot"; 

			ci->Shrink("BS_NOBOT");
			source.Reply(_("No-bot mode is now \002off\002 on channel %s."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
				"This option makes a channel be unassignable. If a bot\n"
				"is already assigned to the channel, it is unassigned\n"
				"automatically when you enable the option."));
		return true;
	}
};

class CommandBSSetPrivate : public Command
{
 public:
	CommandBSSetPrivate(Module *creator, const Anope::string &sname = "botserv/set/private") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Prevent a bot from being assigned by non IRC operators"));
		this->SetSyntax(_("\037botname\037 {\037ON|OFF\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		BotInfo *bi = BotInfo::Find(params[0], true);
		const Anope::string &value = params[1];

		if (bi == NULL)
		{
			source.Reply(BOT_DOES_NOT_EXIST, params[0].c_str());
			return;
		}

		if (value.equals_ci("ON"))
		{
			bi->oper_only = true;
			source.Reply(_("Private mode of bot %s is now \002on\002."), bi->nick.c_str());
		}
		else if (value.equals_ci("OFF"))
		{
			bi->oper_only = false;
			source.Reply(_("Private mode of bot %s is now \002off\002."), bi->nick.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
			"This option prevents a bot from being assigned to a\n"
			"channel by users that aren't IRC Operators."));
		return true;
	}
};

class BSSet : public Module
{
	CommandBSSet commandbsset;
	CommandBSSetBanExpire commandbssetbanexpire;
	CommandBSSetDontKickOps commandbssetdontkickops;
	CommandBSSetDontKickVoices commandbssetdontkickvoices;
	CommandBSSetFantasy commandbssetfantasy;
	CommandBSSetGreet commandbssetgreet;
	CommandBSSetNoBot commandbssetnobot;
	CommandBSSetPrivate commandbssetprivate;

 public:
	BSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandbsset(this), commandbssetbanexpire(this), commandbssetdontkickops(this), commandbssetdontkickvoices(this),
		commandbssetfantasy(this), commandbssetgreet(this), commandbssetnobot(this), commandbssetprivate(this)
	{

		ModuleManager::Attach(I_OnBotBan, this);
	}

	void OnBotBan(User *u, ChannelInfo *ci, const Anope::string &mask) anope_override
	{
		if (!ci->banexpire)
			return;

		new CommandBSSetBanExpire::UnbanTimer(this, ci->name, mask, ci->banexpire);
	}
};

MODULE_INIT(BSSet)
