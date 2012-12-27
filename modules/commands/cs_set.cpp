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

#include "module.h"

class CommandCSSet : public Command
{
 public:
	CommandCSSet(Module *creator) : Command(creator, "chanserv/set", 2, 3)
	{
		this->SetDesc(_("Set channel options and information"));
		this->SetSyntax(_("\037option\037 \037channel\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->OnSyntaxError(source, "");
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows the channel founder to set various channel options\n"
			"and other information.\n"
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
		source.Reply(_("Type \002%s%s HELP SET \037option\037\002 for more information on a\n"
				"particular option."), Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());
		return true;
	}
};

class CommandCSSASet : public Command
{
 public:
	CommandCSSASet(Module *creator) : Command(creator, "chanserv/saset", 2, 3)
	{
		this->SetDesc(_("Forcefully set channel options and information"));
		this->SetSyntax(_("\037option\037 \037channel\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->OnSyntaxError(source, "");
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows Services Operators to forcefully change settings\n"
				"on channels.\n"
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
		source.Reply(_("Type \002%s%s HELP SASET \037option\037\002 for more information on a\n"
			"particular option."), Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());
		return true;
	}
};

class CommandCSSetAutoOp : public Command
{
 public:
	CommandCSSetAutoOp(Module *creator, const Anope::string &cname = "chanserv/set/autoop") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Should services automatically give status to users"));
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

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->UnsetFlag(CI_NOAUTOOP);
			source.Reply(_("Services will now automatically give modes to users in \2%s\2"), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->SetFlag(CI_NOAUTOOP);
			source.Reply(_("Services will no longer automatically give modes to users in \2%s\2"), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "AUTOOP");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables %s's autoop feature for a\n"
			"channel. When disabled, users who join the channel will\n"
			"not automatically gain any status from %s"), Config->ChanServ.c_str(),
			Config->ChanServ.c_str(), this->name.c_str());
		return true;
	}
};

class CommandCSSetBanType : public Command
{
 public:
	CommandCSSetBanType(Module *creator, const Anope::string &cname = "chanserv/set/bantype") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Set how Services make bans on the channel"));
		this->SetSyntax(_("\037channel\037 \037bantype\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		try
		{
			int16_t new_type = convertTo<int16_t>(params[1]);
			if (new_type < 0 || new_type > 3)
				throw ConvertException("Invalid range");
			ci->bantype = new_type;
			source.Reply(_("Ban type for channel %s is now #%d."), ci->name.c_str(), ci->bantype);
		}
		catch (const ConvertException &)
		{
			source.Reply(_("\002%s\002 is not a valid ban type."), params[1].c_str());
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets the ban type that will be used by services whenever\n"
				"they need to ban someone from your channel.\n"
				" \n"
				"bantype is a number between 0 and 3 that means:\n"
				" \n"
				"0: ban in the form *!user@host\n"
				"1: ban in the form *!*user@host\n"
				"2: ban in the form *!*@host\n"
				"3: ban in the form *!*user@*.domain"), this->name.c_str());
		return true;
	}
};

class CommandCSSetChanstats : public Command
{
 public:
	CommandCSSetChanstats(Module *creator) : Command(creator, "chanserv/set/chanstats", 2, 2)
	{
		this->SetDesc(_("Turn chanstat statistics on or off"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (!ci)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_STATS);
			source.Reply(_("Chanstats statistics are now enabled for this channel"));
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_STATS);
			source.Reply(_("Chanstats statistics are now disabled for this channel"));
		}
		else
			this->OnSyntaxError(source, "");
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply("Turn Chanstats channel statistics ON or OFF");
		return true;
	}
};

class CommandCSSetDescription : public Command
{
 public:
	CommandCSSetDescription(Module *creator, const Anope::string &cname = "chanserv/set/description") : Command(creator, cname, 1, 2)
	{
		this->SetDesc(_("Set the channel description"));
		this->SetSyntax(_("\037channel\037 [\037description\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params.size() > 1)
		{
			ci->desc = params[1];
			source.Reply(_("Description of %s changed to \002%s\002."), ci->name.c_str(), ci->desc.c_str());
		}
		else
		{
			ci->desc.clear();
			source.Reply(_("Description of %s unset."), ci->name.c_str());
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets the description for the channel, which shows up with\n"
				"the \002LIST\002 and \002INFO\002 commands."), this->name.c_str());
		return true;
	}
};

class CommandCSSetFounder : public Command
{
 public:
	CommandCSSetFounder(Module *creator, const Anope::string &cname = "chanserv/set/founder") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Set the founder of a channel"));
		this->SetSyntax(_("\037channel\037 \037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (source.permission.empty() && (ci->HasFlag(CI_SECUREFOUNDER) ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		const NickAlias *na = NickAlias::Find(params[1]);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, params[1].c_str());
			return;
		}

		NickCore *nc = na->nc;
		if (Config->CSMaxReg && nc->channelcount >= Config->CSMaxReg && !source.HasPriv("chanserv/no-register-limit"))
		{
			source.Reply(_("\002%s\002 has too many channels registered."), na->nick.c_str());
			return;
		}

		Log(!source.permission.empty() ? LOG_ADMIN : LOG_COMMAND, source, this, ci) << "to change the founder from " << (ci->GetFounder() ? ci->GetFounder()->display : "(none)") << " to " << nc->display;

		ci->SetFounder(nc);

		source.Reply(_("Founder of \002%s\002 changed to \002%s\002."), ci->name.c_str(), na->nick.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the founder of a channel. The new nickname must\n"
				"be a registered one."), this->name.c_str());
		return true;
	}
};

class CommandCSSetKeepTopic : public Command
{
 public:
	CommandCSSetKeepTopic(Module *creator, const Anope::string &cname = "chanserv/set/keeptopic") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Retain topic when channel is not in use"));
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

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_KEEPTOPIC);
			source.Reply(_("Topic retention option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_KEEPTOPIC);
			source.Reply(_("Topic retention option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "KEEPTOPIC");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002topic retention\002 option for a\n"
				"channel. When \002%s\002 is set, the topic for the\n"
				"channel will be remembered by %s even after the\n"
				"last user leaves the channel, and will be restored the\n"
				"next time the channel is created."), this->name.c_str(), source.service->nick.c_str());
		return true;
	}
};

class CommandCSSetPeace : public Command
{
 public:
	CommandCSSetPeace(Module *creator, const Anope::string &cname = "chanserv/set/peace") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Regulate the use of critical commands"));
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
		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_PEACE);
			source.Reply(_("Peace option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_PEACE);
			source.Reply(_("Peace option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PEACE");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002peace\002 option for a channel.\n"
				"When \002peace\002 is set, a user won't be able to kick,\n"
				"ban or remove a channel status of a user that has\n"
				"a level superior or equal to his via %s commands."), source.service->nick.c_str());
		return true;
	}
};

class CommandCSSetPersist : public Command
{
 public:
	CommandCSSetPersist(Module *creator, const Anope::string &cname = "chanserv/set/persist") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Set the channel as permanent"));
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

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PERM);

		if (params[1].equals_ci("ON"))
		{
			if (!ci->HasFlag(CI_PERSIST))
			{
				ci->SetFlag(CI_PERSIST);
				if (ci->c)
					ci->c->SetFlag(CH_PERSIST);

				/* Channel doesn't exist, create it */
				if (!ci->c)
				{
					Channel *c = new Channel(ci->name);
					if (ci->bi)
						ci->bi->Join(c);
				}

				/* No botserv bot, no channel mode, give them ChanServ.
				 * Yes, this works fine with no Config->BotServ.
				 */
				if (!ci->bi && !cm)
				{
					if (!ChanServ)
					{
						source.Reply(_("ChanServ is required to enable persist on this network."));
						return;
					}
					ChanServ->Assign(NULL, ci);
					if (!ci->c->FindUser(ChanServ))
						ChanServ->Join(ci->c);
				}

				/* Set the perm mode */
				if (cm)
				{
					if (ci->c && !ci->c->HasMode(CMODE_PERM))
						ci->c->SetMode(NULL, cm);
					/* Add it to the channels mlock */
					ci->SetMLock(cm, true);
				}
			}

			source.Reply(_("Channel \002%s\002 is now persistent."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			if (ci->HasFlag(CI_PERSIST))
			{
				ci->UnsetFlag(CI_PERSIST);
				if (ci->c)
					ci->c->UnsetFlag(CH_PERSIST);

				/* Unset perm mode */
				if (cm)
				{
					if (ci->c && ci->c->HasMode(CMODE_PERM))
						ci->c->RemoveMode(NULL, cm);
					/* Remove from mlock */
					ci->RemoveMLock(cm, true);
				}

				/* No channel mode, no BotServ, but using ChanServ as the botserv bot
				 * which was assigned when persist was set on
				 */
				if (!cm && Config->BotServ.empty() && ci->bi)
				{
					if (!ChanServ)
					{
						source.Reply(_("ChanServ is required to enable persist on this network."));
						return;
					}
					/* Unassign bot */
					ChanServ->UnAssign(NULL, ci);
				}
			}

			source.Reply(_("Channel \002%s\002 is no longer persistent."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PERSIST");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the persistent channel setting.\n"
				"When persistent is set, the service bot will remain\n"
				"in the channel when it has emptied of users.\n"
				" \n"
				"If your IRCd does not have a permanent (persistent) channel\n"
				"mode you must have a service bot in your channel to\n"
				"set persist on, and it can not be unassigned while persist\n"
				"is on.\n"
				" \n"
				"If this network does not have BotServ enabled and does\n"
				"not have a permanent channel mode, ChanServ will\n"
				"join your channel when you set persist on (and leave when\n"
				"it has been set off).\n"
				" \n"
				"If your IRCd has a permanent (persistent) channel mode\n"
				"and it is set or unset (for any reason, including MODE LOCK),\n"
				"persist is automatically set and unset for the channel aswell.\n"
				"Additionally, services will set or unset this mode when you\n"
				"set persist on or off."));
		return true;
	}
};

class CommandCSSetPrivate : public Command
{
 public:
	CommandCSSetPrivate(Module *creator, const Anope::string &cname = "chanserv/set/private") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Hide channel from LIST command"));
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

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_PRIVATE);
			source.Reply(_("Private option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_PRIVATE);
			source.Reply(_("Private option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PRIVATE");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002private\002 option for a channel.\n"
				"When \002private\002 is set, a \002%s%s LIST\002 will not\n"
				"include the channel in any lists."),
				Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str());
		return true;
	}
};

class CommandCSSetRestricted : public Command
{
 public:
	CommandCSSetRestricted(Module *creator, const Anope::string &cname = "chanserv/set/restricted") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Restrict access to the channel"));
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

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_RESTRICTED);
			source.Reply(_("Restricted access option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_RESTRICTED);
			source.Reply(_("Restricted access option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "RESTRICTED");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002restricted access\002 option for a\n"
				"channel. When \002restricted access\002 is set, users not on the access list will\n"
				"instead be kicked and banned from the channel."));
		return true;
	}
};

class CommandCSSetSecure : public Command
{
 public:
	CommandCSSetSecure(Module *creator, const Anope::string &cname = "chanserv/set/secure") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Activate security features"));
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

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SECURE);
			source.Reply(_("Secure option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SECURE);
			source.Reply(_("Secure option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SECURE");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables security features for a\n"
			"channel. When \002%s\002 is set, only users who have\n"
			"registered their nicknames and IDENTIFY'd\n"
			"with their password will be given access to the channel\n"
			"as controlled by the access list."), this->name.c_str());
		return true;
	}
};

class CommandCSSetSecureFounder : public Command
{
 public:
	CommandCSSetSecureFounder(Module *creator, const Anope::string &cname = "chanserv/set/securefounder") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Stricter control of channel founder status"));
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

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SECUREFOUNDER);
			source.Reply(_("Secure founder option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SECUREFOUNDER);
			source.Reply(_("Secure founder option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SECUREFOUNDER");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002secure founder\002 option for a channel.\n"
			"When \002secure founder\002 is set, only the real founder will be\n"
			"able to drop the channel, change its founder and its successor,\n"
			"and not those who have founder level access through\n"
			"the access/qop command."));
		return true;
	}
};

class CommandCSSetSecureOps : public Command
{
 public:
	CommandCSSetSecureOps(Module *creator, const Anope::string &cname = "chanserv/set/secureops") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Stricter control of chanop status"));
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

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SECUREOPS);
			source.Reply(_("Secure ops option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SECUREOPS);
			source.Reply(_("Secure ops option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SECUREOPS");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002secure ops\002 option for a channel.\n"
				"When \002secure ops\002 is set, users who are not on the userlist\n"
				"will not be allowed chanop status."));
		return true;
	}
};

class CommandCSSetSignKick : public Command
{
 public:
	CommandCSSetSignKick(Module *creator, const Anope::string &cname = "chanserv/set/signkick") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Sign kicks that are done with KICK command"));
		this->SetSyntax(_("\037channel\037 SIGNKICK {ON | LEVEL | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_SIGNKICK);
			ci->UnsetFlag(CI_SIGNKICK_LEVEL);
			source.Reply(_("Signed kick option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("LEVEL"))
		{
			ci->SetFlag(CI_SIGNKICK_LEVEL);
			ci->UnsetFlag(CI_SIGNKICK);
			source.Reply(_("Signed kick option for %s is now \002ON\002, but depends of the\n"
				"level of the user that is using the command."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_SIGNKICK);
			ci->UnsetFlag(CI_SIGNKICK_LEVEL);
			source.Reply(_("Signed kick option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SIGNKICK");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables signed kicks for a\n"
				"channel.  When \002SIGNKICK\002 is set, kicks issued with\n"
				"the \002KICK\002 command will have the nick that used the\n"
				"command in their reason.\n"
				" \n"
				"If you use \002LEVEL\002, those who have a level that is superior\n"
				"or equal to the SIGNKICK level on the channel won't have their\n"
				"kicks signed."));
		return true;
	}
};

class CommandCSSetSuccessor : public Command
{
 public:
	CommandCSSetSuccessor(Module *creator, const Anope::string &cname = "chanserv/set/successor") : Command(creator, cname, 1, 2)
	{
		this->SetDesc(_("Set the successor for a channel"));
		this->SetSyntax(_("\037channel\037 \037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty())
		{
			if (!source.AccessFor(ci).HasPriv("SET"))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}

			if (ci->HasFlag(CI_SECUREFOUNDER) ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER"))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
		}

		NickCore *nc;

		if (params.size() > 1)
		{
			const NickAlias *na = NickAlias::Find(params[1]);

			if (!na)
			{
				source.Reply(NICK_X_NOT_REGISTERED, params[1].c_str());
				return;
			}
			if (na->nc == ci->GetFounder())
			{
				source.Reply(_("%s cannot be the successor on channel %s as they are the founder."), na->nick.c_str(), ci->name.c_str());
				return;
			}
			nc = na->nc;
		}
		else
			nc = NULL;

		Log(!source.permission.empty() ? LOG_ADMIN : LOG_COMMAND, source, this, ci) << "to change the successor from " << (ci->successor ? ci->successor->display : "(none)") << " to " << (nc ? nc->display : "(none)");

		ci->successor = nc;

		if (nc)
			source.Reply(_("Successor for \002%s\002 changed to \002%s\002."), ci->name.c_str(), nc->display.c_str());
		else
			source.Reply(_("Successor for \002%s\002 unset."), ci->name.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Changes the successor of a channel. If the founder's\n"
				"nickname expires or is dropped while the channel is still\n"
				"registered, the successor will become the new founder of the\n"
				"channel.  However, if the successor already has too many\n"
				"channels registered (%d), the channel will be dropped\n"
				"instead, just as if no successor had been set.  The new\n"
				"nickname must be a registered one."), Config->CSMaxReg);
		return true;
	}
};

class CommandCSSASetNoexpire : public Command
{
 public:
	CommandCSSASetNoexpire(Module *creator) : Command(creator, "chanserv/saset/noexpire", 2, 2)
	{
		this->SetDesc(_("Prevent the channel from expiring"));
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

		if (source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_NO_EXPIRE);
			source.Reply(_("Channel %s \002will not\002 expire."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_NO_EXPIRE);
			source.Reply(_("Channel %s \002will\002 expire."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "NOEXPIRE");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets whether the given channel will expire.  Setting this\n"
				"to ON prevents the channel from expiring."));
		return true;
	}
};

class CSSet : public Module
{
	CommandCSSet commandcsset;
	CommandCSSASet commandcssaset;
	CommandCSSetAutoOp commandcssetautoop;
	CommandCSSetBanType commandcssetbantype;
	CommandCSSetChanstats commandcssetchanstats;
	bool CSDefChanstats;
	CommandCSSetDescription commandcssetdescription;
	CommandCSSetFounder commandcssetfounder;
	CommandCSSetKeepTopic commandcssetkeeptopic;
	CommandCSSetPeace commandcssetpeace;
	CommandCSSetPersist commandcssetpersist;
	CommandCSSetPrivate commandcssetprivate;
	CommandCSSetRestricted commandcssetrestricted;
	CommandCSSetSecure commandcssetsecure;
	CommandCSSetSecureFounder commandcssetsecurefounder;
	CommandCSSetSecureOps commandcssetsecureops;
	CommandCSSetSignKick commandcssetsignkick;
	CommandCSSetSuccessor commandcssetsuccessor;
	CommandCSSASetNoexpire commandcssasetnoexpire;

 public:
	CSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsset(this), commandcssaset(this), commandcssetautoop(this), commandcssetbantype(this), commandcssetchanstats(this),
		CSDefChanstats(false), commandcssetdescription(this), commandcssetfounder(this), commandcssetkeeptopic(this),
		commandcssetpeace(this), commandcssetpersist(this), commandcssetprivate(this), commandcssetrestricted(this),
		commandcssetsecure(this), commandcssetsecurefounder(this), commandcssetsecureops(this), commandcssetsignkick(this),
		commandcssetsuccessor(this), commandcssasetnoexpire(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnChanRegistered };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		CSDefChanstats = config.ReadFlag("chanstats", "CSDefChanstats", "0", 0);
	}

	void OnChanRegistered(ChannelInfo *ci) anope_override
	{
		if (CSDefChanstats)
			ci->SetFlag(CI_STATS);
	}
};

MODULE_INIT(CSSet)
