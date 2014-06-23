/* ChanServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/cs_mode.h"
#include "modules/chanserv.h"
#include "modules/cs_info.h"
#include "modules/cs_set.h"

class CommandCSSet : public Command
{
 public:
	CommandCSSet(Module *creator) : Command(creator, "chanserv/set", 2, 3)
	{
		this->SetDesc(_("Set channel options and information"));
		this->SetSyntax(_("\037option\037 \037channel\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows the channel founder to set various channel options and other information.\n"
		               "\n"
		               "Available options:"));
		Anope::string this_name = source.command;
		bool hide_privileged_commands = Config->GetBlock("options")->Get<bool>("hideprivilegedcommands");
		for (CommandInfo::map::const_iterator it = source.service->commands.begin(), it_end = source.service->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;
			if (c_name.find_ci(this_name + " ") == 0)
			{
				ServiceReference<Command> c("Command", info.name);

				if (!c)
					continue;
				else if (!hide_privileged_commands)
					; // Always show with hide_privileged_commands disabled
				else if (!c->AllowUnregistered() && !source.GetAccount())
					continue;
				else if (!info.permission.empty() && !source.HasCommand(info.permission))
					continue;

				source.command = it->first;
				c->OnServHelp(source);
			}
		}

		CommandInfo *help = source.service->FindCommand("generic/help");
		if (help)
			source.Reply(_("Type \002{0}{1} {2} {3} \037option\037\002 for more information on a particular option."),
			               Config->StrictPrivmsg, source.service->nick, help->cname, this_name);

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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, params[1]);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable autoop";
			ci->Shrink<bool>("NOAUTOOP");
			source.Reply(_("Services will now automatically give modes to users in \002{0}\002."), ci->name);
		}
		else if (params[1].equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable autoop";
			ci->Extend<bool>("NOAUTOOP");
			source.Reply(_("Services will no longer automatically give modes to users in \002{0}\002."), ci->name);
		}
		else
			this->OnSyntaxError(source, "AUTOOP");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables AUTOOP for a \037channel\037. When disabled, users who join \037channel\037 will not automatically gain any status from {0}."), source.service->nick);
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, params[1]);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
			return;
		}

		try
		{
			int16_t new_type = convertTo<int16_t>(params[1]);
			if (new_type < 0 || new_type > 3)
				throw ConvertException("Invalid range");
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change the ban type to " << new_type;
			ci->bantype = new_type;
			source.Reply(_("Ban type for channel \002{0}\002 is now \002#{1}\002."), ci->name, ci->bantype);
		}
		catch (const ConvertException &)
		{
			source.Reply(_("\002{0}\002 is not a valid ban type."), params[1]);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Sets the ban type that will be used by services whenever they need to ban someone from your channel.\n"
		               "\n"
		               "Bantype is a number between 0 and 3 that means:\n"
		               "\n"
		               "0: ban in the form *!user@host\n"
		               "1: ban in the form *!*user@host\n"
		               "2: ban in the form *!*@host\n"
		               "3: ban in the form *!*user@*.domain"));
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params.size() > 1 ? params[1] : "";

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
			return;
		}

		if (!param.empty())
		{
			ci->desc = param;
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change the description to " << ci->desc;
			source.Reply(_("Description of \002{0}\002 changed to \002{1}\002."), ci->name, ci->desc);
		}
		else
		{
			ci->desc.clear();
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to unset the description";
			source.Reply(_("Description of \002{0}\002 unset."), ci->name);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Sets the description for the channel, which shows up with the \002LIST\002 and \002INFO\002 commands."));
		return true;
	}
};

class CommandCSSetFounder : public Command
{
 public:
	CommandCSSetFounder(Module *creator, const Anope::string &cname = "chanserv/set/founder") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Set the founder of a channel"));
		this->SetSyntax(_("\037channel\037 \037user\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && (ci->HasExt("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")) && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->name);
			return;
		}

		const NickServ::Nick *na = NickServ::FindNick(param);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), param);
			return;
		}

		NickServ::Account *nc = na->nc;
		unsigned max_reg = Config->GetModule("chanserv")->Get<unsigned>("maxregistered");
		if (max_reg && nc->channelcount >= max_reg && !source.HasPriv("chanserv/no-register-limit"))
		{
			source.Reply(_("\002{0}\002 has too many channels registered."), na->nick);
			return;
		}

		Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change the founder from " << (ci->GetFounder() ? ci->GetFounder()->display : "(none)") << " to " << nc->display;

		ci->SetFounder(nc);

		source.Reply(_("Founder of \002{0}\002 changed to \002{1}\002."), ci->name, na->nick);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Changes the founder of \037channel\037 to \037user\037. Using this command will cause you to lose your founder access to \037channel\037, and cannot be undone."
		               "\n"
		               "Use of this command requires being the founder or \037channel\037 or having the \002{0}\002 privilege, if secure founder is enabled or not, respectively."));
		return true;
	}
};

class CommandCSSetKeepModes : public Command
{
 public:
	CommandCSSetKeepModes(Module *creator, const Anope::string &cname = "chanserv/set/keepmodes") :  Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Retain modes when channel is not in use"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, params[1]);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable keep modes";
			ci->Extend<bool>("CS_KEEP_MODES");
			source.Reply(_("Keep modes for \002{0}\002 is now \002on\002."), ci->name);
			if (ci->c)
				ci->last_modes = ci->c->GetModes();
		}
		else if (params[1].equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable keep modes";
			ci->Shrink<bool>("CS_KEEP_MODES");
			source.Reply(_("Keep modes for \002{0}\002 is now \002off\002."), ci->name);
			ci->last_modes.clear();
		}
		else
			this->OnSyntaxError(source, "KEEPMODES");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables keepmodes for \037channel\037. If keepmodes is enabled, services will remember modes set on the channel and re-set them the next time the channel is created."));
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable peace";
			ci->Extend<bool>("PEACE");
			source.Reply(_("Peace option for \002{0}\002 is now \002on\002."), ci->name);
		}
		else if (param.equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable peace";
			ci->Shrink<bool>("PEACE");
			source.Reply(_("Peace option for \002{0}\002 is now \002off\002."), ci->name);
		}
		else
			this->OnSyntaxError(source, "PEACE");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables the \002peace\002 option for \037channel\037."
		               " When \002peace\002 is set, a user won't be able to kick, ban or remove a channel status of a user that has a level superior or equal to his via services."),
		               source.service->nick);
		return true;
	}
};

inline static Anope::string BotModes()
{
	return Config->GetModule("botserv")->Get<Anope::string>("botmodes",
		Config->GetModule("chanserv")->Get<Anope::string>("botmodes", "o")
	);
}

class CommandCSSetPersist : public Command
{
 public:
	CommandCSSetPersist(Module *creator, const Anope::string &cname = "chanserv/set/persist") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Set the channel as permanent"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
			return;
		}

		ChannelMode *cm = ModeManager::FindChannelModeByName("PERM");

		if (params[1].equals_ci("ON"))
		{
			if (!ci->HasExt("PERSIST"))
			{
				ci->Extend<bool>("PERSIST");

				/* Channel doesn't exist, create it */
				if (!ci->c)
				{
					bool created;
					Channel *c = Channel::FindOrCreate(ci->name, created);
					if (ci->bi)
					{
						ChannelStatus status(BotModes());
						ci->bi->Join(c, &status);
					}
					if (created)
						c->Sync();
				}

				/* Set the perm mode */
				if (cm)
				{
					if (ci->c && !ci->c->HasMode("PERM"))
						ci->c->SetMode(NULL, cm);
					/* Add it to the channels mlock */
					ModeLocks *ml = ci->Require<ModeLocks>("modelocks");
					if (ml)
						ml->SetMLock(cm, true, "", source.GetNick());
				}
				/* No botserv bot, no channel mode, give them ChanServ.
				 * Yes, this works fine with no BotServ.
				 */
				else if (!ci->bi)
				{
					BotInfo *ChanServ = Config->GetClient("ChanServ");
					if (!ChanServ)
					{
						source.Reply(_("ChanServ is required to enable persist on this network."));
						return;
					}

					ChanServ->Assign(NULL, ci);
					if (!ci->c->FindUser(ChanServ))
					{
						ChannelStatus status(BotModes());
						ChanServ->Join(ci->c, &status);
					}
				}
			}

			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable persist";
			source.Reply(_("Channel \002{0}\002 is now persistent."), ci->name);
		}
		else if (params[1].equals_ci("OFF"))
		{
			if (ci->HasExt("PERSIST"))
			{
				ci->Shrink<bool>("PERSIST");

				BotInfo *ChanServ = Config->GetClient("ChanServ"),
					*BotServ = Config->GetClient("BotServ");

				/* Unset perm mode */
				if (cm)
				{
					if (ci->c && ci->c->HasMode("PERM"))
						ci->c->RemoveMode(NULL, cm);
					/* Remove from mlock */
					ModeLocks *ml = ci->GetExt<ModeLocks>("modelocks");
					if (ml)
						ml->RemoveMLock(cm, true);
				}
				/* No channel mode, no BotServ, but using ChanServ as the botserv bot
				 * which was assigned when persist was set on
				 */
				else if (!cm && !BotServ && ci->bi)
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

			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable persist";
			source.Reply(_("Channel \002{0}\002 is no longer persistent."), ci->name);
		}
		else
			this->OnSyntaxError(source, "PERSIST");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables the persistent channel setting."
		               " When persistent is set, the service bot will remain in the channel when it has emptied of users."));
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable restricted";
			ci->Extend<bool>("RESTRICTED");
			source.Reply(_("Restricted access option for \002{0}\002 is now \002on\002."), ci->name);
		}
		else if (param.equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable restricted";
			ci->Shrink<bool>("RESTRICTED");
			source.Reply(_("Restricted access option for \002{0}\002 is now \002off\002."), ci->name);
		}
		else
			this->OnSyntaxError(source, "RESTRICTED");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables the \002restricted access\002 option for \037channel\037. When \002restricted access\002 is set, users not on the access list will not be permitted to join the channel."));
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable secure";
			ci->Extend<bool>("CS_SECURE");
			source.Reply(_("Secure option for \002{0}\002 is now \002on\002."), ci->name);
		}
		else if (param.equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable secure";
			ci->Shrink<bool>("CS_SECURE");
			source.Reply(_("Secure option for \002{0}\002 is now \002off\002."), ci->name);
		}
		else
			this->OnSyntaxError(source, "SECURE");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables security features for a channel."
		               " When \002secure\002 is set, only users who have logged in (eg. not recognized based on their hostmask) will be given access to the channel as controlled by the access list."));
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && (ci->HasExt("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")) && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->name);
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable secure founder";
			ci->Extend<bool>("SECUREFOUNDER");
			source.Reply(_("Secure founder option for \002{0}\002 is now \002on\002."), ci->name);
		}
		else if (param.equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable secure founder";
			ci->Shrink<bool>("SECUREFOUNDER");
			source.Reply(_("Secure founder option for \002{0}\002 is now \002off\002."), ci->name);
		}
		else
			this->OnSyntaxError(source, "SECUREFOUNDER");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002secure founder\002 option for a channel."
		               " When \002secure founder\002 is set, only the real founder will be able to drop the channel, change its founder, and change its successor."
		               " Otherwise, anyone with the \002{0}\002 privilege will be able to use these commands."),
				"FOUNDER");
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable secure ops";
			ci->Extend<bool>("SECUREOPS");
			source.Reply(_("Secure ops option for \002{0}\002 is now \002on\002."), ci->name);
		}
		else if (param.equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable secure ops";
			ci->Shrink<bool>("SECUREOPS");
			source.Reply(_("Secure ops option for \002{0}\002 is now \002off\002."), ci->name);
		}
		else
			this->OnSyntaxError(source, "SECUREOPS");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables the \002secure ops\002 option for \037channel\037."
		               " When \002secure ops\002 is set, users will not be allowed to have channel operator status if they do not have the privileges to have it."));
		return true;
	}
};

class CommandCSSetSignKick : public Command
{
 public:
	CommandCSSetSignKick(Module *creator, const Anope::string &cname = "chanserv/set/signkick") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Sign kicks that are done with the KICK command"));
		this->SetSyntax(_("\037channel\037 {ON | LEVEL | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
			return;
		}

		if (param.equals_ci("ON"))
		{
			ci->Extend<bool>("SIGNKICK");
			ci->Shrink<bool>("SIGNKICK_LEVEL");
			source.Reply(_("Signed kick option for \002{0}\002 is now \002on\002."), ci->name);
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable sign kick";
		}
		else if (param.equals_ci("LEVEL"))
		{
			ci->Extend<bool>("SIGNKICK_LEVEL");
			ci->Shrink<bool>("SIGNKICK");
			source.Reply(_("Signed kick option for \002{0}\002 is now \002on\002, but depends of the privileges of the user that is using the command."), ci->name);
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable sign kick level";
		}
		else if (param.equals_ci("OFF"))
		{
			ci->Shrink<bool>("SIGNKICK");
			ci->Shrink<bool>("SIGNKICK_LEVEL");
			source.Reply(_("Signed kick option for \002{0}\002 is now \002off\002."), ci->name);
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable sign kick";
		}
		else
			this->OnSyntaxError(source, "SIGNKICK");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables \002signed kicks\002 for \037channel\037."
		               " When \002signed kicks\002 is enabled, kicks issued through services will have the nickname of the user who performed the kick included in the kick reason."
		               " If you use \002LEVEL\002 setting, then only users who do not have the \002{0}\002 privilege will have their kicks signed."),
		               "SIGNKICK");
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		EventReturn MOD_RESULT;
		MOD_RESULT = Event::OnSetChannelOption(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && (ci->HasExt("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")) && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->name);
			return;
		}

		NickServ::Account *nc;

		if (!param.empty())
		{
			const NickServ::Nick *na = NickServ::FindNick(param);

			if (!na)
			{
				source.Reply(_("\002{0}\002 isn't registered."), param);
				return;
			}

			if (na->nc == ci->GetFounder())
			{
				source.Reply(_("\002{0}\002 cannot be the successor of channel \002{1}\002 as they are the founder."), na->nick, ci->name);
				return;
			}

			nc = na->nc;
		}
		else
			nc = NULL;

		Log(!source.permission.empty() ? LOG_ADMIN : LOG_COMMAND, source, this, ci) << "to change the successor from " << (ci->GetSuccessor() ? ci->GetSuccessor()->display : "(none)") << " to " << (nc ? nc->display : "(none)");

		ci->SetSuccessor(nc);

		if (nc)
			source.Reply(_("Successor for \002{0}\002 changed to \002{1}\002."), ci->name, nc->display);
		else
			source.Reply(_("Successor for \002{0}\002 unset."), ci->name);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Changes the successor of \037channel\037."
		               " The successor of a channel is automatically given ownership of the channel if the founder's account drops of expires."
		               " If the success has too many registered channels or there is no successor, the channel may instead be given to one of the users on the channel access list with the most privileges."));
		return true;
	}
};

class CommandCSSetNoexpire : public Command
{
 public:
	CommandCSSetNoexpire(Module *creator) : Command(creator, "chanserv/saset/noexpire", 2, 2)
	{
		this->SetDesc(_("Prevent the channel from expiring"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &param = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->name);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			Log(LOG_ADMIN, source, this, ci) << "to enable noexpire";
			ci->Extend<bool>("CS_NO_EXPIRE");
			source.Reply(_("Channel \002{0} will not\002 expire."), ci->name);
		}
		else if (params[1].equals_ci("OFF"))
		{
			Log(LOG_ADMIN, source, this, ci) << "to disable noexpire";
			ci->Shrink<bool>("CS_NO_EXPIRE");
			source.Reply(_("Channel \002{0} will\002 expire."), ci->name);
		}
		else
			this->OnSyntaxError(source, "NOEXPIRE");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Sets whether the given \037channel\037 will expire. Setting \002 noexpire\002 to \002on\002 prevents the channel from expiring."));
		return true;
	}
};

class CSSet : public Module
	, public EventHook<Event::CreateChan>
	, public EventHook<Event::ChannelCreate>
	, public EventHook<Event::ChannelSync>
	, public EventHook<Event::CheckKick>
	, public EventHook<Event::DelChan>
	, public EventHook<Event::ChannelModeSet>
	, public EventHook<Event::ChannelModeUnset>
	, public EventHook<Event::CheckDelete>
	, public EventHook<Event::JoinChannel>
	, public EventHook<Event::SetCorrectModes>
	, public EventHook<ChanServ::Event::PreChanExpire>
	, public EventHook<Event::ChanInfo>
{
	SerializableExtensibleItem<bool> noautoop, peace, securefounder,
		restricted, secure, secureops, signkick, signkick_level, noexpire;

	struct KeepModes : SerializableExtensibleItem<bool>
	{
		KeepModes(Module *m, const Anope::string &n) : SerializableExtensibleItem<bool>(m, n) { }

		void ExtensibleSerialize(const Extensible *e, const Serializable *s, Serialize::Data &data) const override
		{
			SerializableExtensibleItem<bool>::ExtensibleSerialize(e, s, data);

			if (s->GetSerializableType()->GetName() != "ChannelInfo")
				return;

			const ChanServ::Channel *ci = anope_dynamic_static_cast<const ChanServ::Channel *>(s);
			Anope::string modes;
			for (Channel::ModeList::const_iterator it = ci->last_modes.begin(); it != ci->last_modes.end(); ++it)
			{
				if (!modes.empty())
					modes += " ";
				modes += it->first;
				if (!it->second.empty())
					modes += "," + it->second;
			}
			data["last_modes"] << modes;
		}

		void ExtensibleUnserialize(Extensible *e, Serializable *s, Serialize::Data &data) override
		{
			SerializableExtensibleItem<bool>::ExtensibleUnserialize(e, s, data);

			if (s->GetSerializableType()->GetName() != "ChannelInfo")
				return;

			ChanServ::Channel *ci = anope_dynamic_static_cast<ChanServ::Channel *>(s);
			Anope::string modes;
			data["last_modes"] >> modes;
			for (spacesepstream sep(modes); sep.GetToken(modes);)
			{
				size_t c = modes.find(',');
				if (c == Anope::string::npos)
					ci->last_modes.insert(std::make_pair(modes, ""));
				else
					ci->last_modes.insert(std::make_pair(modes.substr(0, c), modes.substr(c + 1)));
			}
		}
	} keep_modes;

	struct Persist : SerializableExtensibleItem<bool>
	{
		Persist(Module *m, const Anope::string &n) : SerializableExtensibleItem<bool>(m, n) { }

		void ExtensibleUnserialize(Extensible *e, Serializable *s, Serialize::Data &data) override
		{
			SerializableExtensibleItem<bool>::ExtensibleUnserialize(e, s, data);

			if (s->GetSerializableType()->GetName() != "ChanServ::Channel" || !this->HasExt(e))
				return;

			ChanServ::Channel *ci = anope_dynamic_static_cast<ChanServ::Channel *>(s);
			if (ci->c)
				return;

			bool created;
			Channel *c = Channel::FindOrCreate(ci->name, created);

			ChannelMode *cm = ModeManager::FindChannelModeByName("PERM");
			if (cm)
			{
				c->SetMode(NULL, cm);
			}
			else
			{
				if (!ci->bi)
				{
					BotInfo *ChanServ = Config->GetClient("ChanServ");
					if (ChanServ)
						ChanServ->Assign(NULL, ci);
				}

				if (ci->bi && !c->FindUser(ci->bi))
				{
					ChannelStatus status(BotModes());
					ci->bi->Join(c, &status);
				}
			}

			if (created)
				c->Sync();
		}
	} persist;

	CommandCSSet commandcsset;
	CommandCSSetAutoOp commandcssetautoop;
	CommandCSSetBanType commandcssetbantype;
	CommandCSSetDescription commandcssetdescription;
	CommandCSSetFounder commandcssetfounder;
	CommandCSSetKeepModes commandcssetkeepmodes;
	CommandCSSetPeace commandcssetpeace;
	CommandCSSetPersist commandcssetpersist;
	CommandCSSetRestricted commandcssetrestricted;
	CommandCSSetSecure commandcssetsecure;
	CommandCSSetSecureFounder commandcssetsecurefounder;
	CommandCSSetSecureOps commandcssetsecureops;
	CommandCSSetSignKick commandcssetsignkick;
	CommandCSSetSuccessor commandcssetsuccessor;
	CommandCSSetNoexpire commandcssetnoexpire;

	bool persist_lower_ts;

 public:
	CSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::CreateChan>("OnCreateChan")
		, EventHook<Event::ChannelCreate>("OnChannelCreate")
		, EventHook<Event::ChannelSync>("OnChannelSync")
		, EventHook<Event::CheckKick>("OnCheckKick")
		, EventHook<Event::DelChan>("OnDelChan")
		, EventHook<Event::ChannelModeSet>("OnChannelModeSet")
		, EventHook<Event::ChannelModeUnset>("OnChannelModeUnset")
		, EventHook<Event::CheckDelete>("OnCheckDelete")
		, EventHook<Event::JoinChannel>("OnJoinChannel")
		, EventHook<Event::SetCorrectModes>("OnSetCorrectModes")
		, EventHook<ChanServ::Event::PreChanExpire>("OnPreChanExpire")
		, EventHook<Event::ChanInfo>("OnChanInfo")
		, noautoop(this, "NOAUTOOP")
		, peace(this, "PEACE")
		, securefounder(this, "SECUREFOUNDER")
		, restricted(this, "RESTRICTED")
		, secure(this, "CS_SECURE")
		, secureops(this, "SECUREOPS")
		, signkick(this, "SIGNKICK")
		, signkick_level(this, "SIGNKICK_LEVEL")
		, noexpire(this, "CS_NO_EXPIRE")
		, keep_modes(this, "CS_KEEP_MODES")
		, persist(this, "PERSIST")

		, commandcsset(this)
		, commandcssetautoop(this)
		, commandcssetbantype(this)
		, commandcssetdescription(this)
		, commandcssetfounder(this)
		, commandcssetkeepmodes(this)
		, commandcssetpeace(this)
		, commandcssetpersist(this)
		, commandcssetrestricted(this)
		, commandcssetsecure(this)
		, commandcssetsecurefounder(this)
		, commandcssetsecureops(this)
		, commandcssetsignkick(this)
		, commandcssetsuccessor(this)
		, commandcssetnoexpire(this)
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		persist_lower_ts = conf->GetModule(this)->Get<bool>("persist_lower_ts");
	}

	void OnCreateChan(ChanServ::Channel *ci) override
	{
		ci->bantype = Config->GetModule(this)->Get<int>("defbantype", "2");
	}

	void OnChannelCreate(Channel *c) override
	{
		if (c->ci && keep_modes.HasExt(c->ci))
		{
			Channel::ModeList ml = c->ci->last_modes;
			for (Channel::ModeList::iterator it = ml.begin(); it != ml.end(); ++it)
				c->SetMode(c->ci->WhoSends(), it->first, it->second);
		}
	}

	void OnChannelSync(Channel *c) override
	{
		OnChannelCreate(c);
	}

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) override
	{
		if (!c->ci || !restricted.HasExt(c->ci) || c->MatchesList(u, "EXCEPT"))
			return EVENT_CONTINUE;

		if (c->ci->AccessFor(u).empty() && (!c->ci->GetFounder() || u->Account() != c->ci->GetFounder()))
			return EVENT_STOP;

		return EVENT_CONTINUE;
	}

	void OnDelChan(ChanServ::Channel *ci) override
	{
		if (ci->c && persist.HasExt(ci))
			ci->c->RemoveMode(ci->WhoSends(), "PERM", "", false);
		persist.Unset(ci);
	}

	EventReturn OnChannelModeSet(Channel *c, const MessageSource &setter, ChannelMode *mode, const Anope::string &param) override
	{
		if (c->ci)
		{
			/* Channel mode +P or so was set, mark this channel as persistent */
			if (mode->name == "PERM")
				persist.Set(c->ci, true);

			if (mode->type != MODE_STATUS && !c->syncing && Me->IsSynced())
				c->ci->last_modes = c->GetModes();
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnChannelModeUnset(Channel *c, const MessageSource &setter, ChannelMode *mode, const Anope::string &param) override
	{
		if (mode->name == "PERM")
		{
			if (c->ci)
				persist.Unset(c->ci);
		}

		if (c->ci && mode->type != MODE_STATUS && !c->syncing && Me->IsSynced())
			c->ci->last_modes = c->GetModes();

		return EVENT_CONTINUE;
	}

	EventReturn OnCheckDelete(Channel *c) override
	{
		if (c->ci && persist.HasExt(c->ci))
			return EVENT_STOP;
		return EVENT_CONTINUE;
	}

	void OnJoinChannel(User *u, Channel *c) override
	{
		if (persist_lower_ts && c->ci && persist.HasExt(c->ci) && c->creation_time > c->ci->time_registered)
		{
			Log(LOG_DEBUG) << "Changing TS of " << c->name << " from " << c->creation_time << " to " << c->ci->time_registered;
			c->creation_time = c->ci->time_registered;
			IRCD->SendChannel(c);
			c->Reset();
		}
	}

	void OnSetCorrectModes(User *user, Channel *chan, ChanServ::AccessGroup &access, bool &give_modes, bool &take_modes) override
	{
		if (chan->ci)
		{
			if (noautoop.HasExt(chan->ci))
				give_modes = false;
			if (secureops.HasExt(chan->ci))
				// This overrides what chanserv does because it is loaded after chanserv
				take_modes = true;
		}
	}

	void OnPreChanExpire(ChanServ::Channel *ci, bool &expire) override
	{
		if (noexpire.HasExt(ci))
			expire = false;
	}

	void OnChanInfo(CommandSource &source, ChanServ::Channel *ci, InfoFormatter &info, bool show_all) override
	{
		if (!show_all)
			return;

		if (peace.HasExt(ci))
			info.AddOption(_("Peace"));
		if (restricted.HasExt(ci))
			info.AddOption(_("Restricted access"));
		if (secure.HasExt(ci))
			info.AddOption(_("Security"));
		if (securefounder.HasExt(ci))
			info.AddOption(_("Secure founder"));
		if (secureops.HasExt(ci))
			info.AddOption(_("Secure ops"));
		if (signkick.HasExt(ci) || signkick_level.HasExt(ci))
			info.AddOption(_("Signed kicks"));
		if (persist.HasExt(ci))
			info.AddOption(_("Persistent"));
		if (noexpire.HasExt(ci))
			info.AddOption(_("No expire"));
		if (keep_modes.HasExt(ci))
			info.AddOption(_("Keep modes"));
		if (noautoop.HasExt(ci))
			info.AddOption(_("No auto-op"));
	}
};

MODULE_INIT(CSSet)
