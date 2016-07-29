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
#include "modules/chanserv/mode.h"
#include "modules/chanserv.h"
#include "modules/chanserv/info.h"
#include "modules/chanserv/set.h"

class CommandCSSet : public Command
{
	ServiceReference<ModeLocks> mlocks;
	
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
		bool hide_privileged_commands = Config->GetBlock("options")->Get<bool>("hideprivilegedcommands"),
		     hide_registered_commands = Config->GetBlock("options")->Get<bool>("hideregisteredcommands");
		for (CommandInfo::map::const_iterator it = source.service->commands.begin(), it_end = source.service->commands.end(); it != it_end; ++it)
		{
			const Anope::string &c_name = it->first;
			const CommandInfo &info = it->second;
			if (c_name.find_ci(this_name + " ") == 0)
			{
				ServiceReference<Command> c(info.name);

				// XXX dup
				if (!c)
					continue;
				else if (hide_registered_commands && !c->AllowUnregistered() && !source.GetAccount())
					continue;
				else if (hide_privileged_commands && !info.permission.empty() && !source.HasCommand(info.permission))
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

		EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, params[1]);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable autoop";
			ci->UnsetS<bool>("NOAUTOOP");
			source.Reply(_("Services will now automatically give modes to users in \002{0}\002."), ci->GetName());
		}
		else if (params[1].equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable autoop";
			ci->SetS<bool>("NOAUTOOP", true);
			source.Reply(_("Services will no longer automatically give modes to users in \002{0}\002."), ci->GetName());
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, params[1]);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		try
		{
			int16_t new_type = convertTo<int16_t>(params[1]);
			if (new_type < 0 || new_type > 3)
				throw ConvertException("Invalid range");
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change the ban type to " << new_type;
			ci->SetBanType(new_type);
			source.Reply(_("Ban type for channel \002{0}\002 is now \002#{1}\002."), ci->GetName(), new_type);
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		ci->SetDesc(param);
		if (!param.empty())
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change the description to " << ci->GetDesc();
			source.Reply(_("Description of \002{0}\002 changed to \002{1}\002."), ci->GetName(), ci->GetDesc());
		}
		else
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to unset the description";
			source.Reply(_("Description of \002{0}\002 unset."), ci->GetName());
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && (ci->HasFieldS("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")) && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->GetName());
			return;
		}

		NickServ::Nick *na = NickServ::FindNick(param);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), param);
			return;
		}

		NickServ::Account *nc = na->GetAccount();
		unsigned max_reg = Config->GetModule("chanserv")->Get<unsigned>("maxregistered");
		if (max_reg && nc->GetChannelCount() >= max_reg && !source.HasPriv("chanserv/no-register-limit"))
		{
			source.Reply(_("\002{0}\002 has too many channels registered."), na->GetNick());
			return;
		}

		Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change the founder from " << (ci->GetFounder() ? ci->GetFounder()->GetDisplay() : "(none)") << " to " << nc->GetDisplay();

		ci->SetFounder(nc);

		source.Reply(_("Founder of \002{0}\002 changed to \002{1}\002."), ci->GetName(), na->GetNick());
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable keep modes";
			ci->SetS<bool>("CS_KEEP_MODES", true);
			source.Reply(_("Keep modes for \002{0}\002 is now \002on\002."), ci->GetName());
			if (ci->c)
				for (const std::pair<Anope::string, Anope::string> &p : ci->c->GetModes())
				{
					ChanServ::Mode *mode = Serialize::New<ChanServ::Mode *>();
					mode->SetChannel(ci);
					mode->SetMode(p.first);
					mode->SetParam(p.second);
				}
		}
		else if (param.equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable keep modes";
			ci->UnsetS<bool>("CS_KEEP_MODES");
			source.Reply(_("Keep modes for \002{0}\002 is now \002off\002."), ci->GetName());
			for (ChanServ::Mode *m : ci->GetRefs<ChanServ::Mode *>())
				m->Delete();
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable peace";
			ci->SetS<bool>("PEACE", true);
			source.Reply(_("Peace option for \002{0}\002 is now \002on\002."), ci->GetName());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable peace";
			ci->UnsetS<bool>("PEACE");
			source.Reply(_("Peace option for \002{0}\002 is now \002off\002."), ci->GetName());
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
	ServiceReference<ModeLocks> mlocks;
	
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		ChannelMode *cm = ModeManager::FindChannelModeByName("PERM");

		if (params[1].equals_ci("ON"))
		{
			if (!ci->HasFieldS("PERSIST"))
			{
				ci->SetS<bool>("PERSIST", true);

				/* Channel doesn't exist, create it */
				if (!ci->c)
				{
					bool created;
					Channel *c = Channel::FindOrCreate(ci->GetName(), created);
					if (ci->GetBot())
					{
						ChannelStatus status(BotModes());
						ci->GetBot()->Join(c, &status);
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
					if (mlocks)
						mlocks->SetMLock(ci, cm, true, "", source.GetNick());
				}
				/* No botserv bot, no channel mode, give them ChanServ.
				 * Yes, this works fine with no BotServ.
				 */
				else if (!ci->GetBot())
				{
					ServiceBot *ChanServ = Config->GetClient("ChanServ");
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
			source.Reply(_("Channel \002{0}\002 is now persistent."), ci->GetName());
		}
		else if (params[1].equals_ci("OFF"))
		{
			if (ci->HasFieldS("PERSIST"))
			{
				ci->UnsetS<bool>("PERSIST");

				ServiceBot *ChanServ = Config->GetClient("ChanServ"),
					*BotServ = Config->GetClient("BotServ");

				/* Unset perm mode */
				if (cm)
				{
					if (ci->c && ci->c->HasMode("PERM"))
						ci->c->RemoveMode(NULL, cm);
					/* Remove from mlock */
					if (mlocks)
						mlocks->RemoveMLock(ci, cm, true);
				}
				/* No channel mode, no BotServ, but using ChanServ as the botserv bot
				 * which was assigned when persist was set on
				 */
				else if (!cm && !BotServ && ci->GetBot())
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
			source.Reply(_("Channel \002{0}\002 is no longer persistent."), ci->GetName());
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable restricted";
			ci->SetS<bool>("RESTRICTED", true);
			source.Reply(_("Restricted access option for \002{0}\002 is now \002on\002."), ci->GetName());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable restricted";
			ci->UnsetS<bool>("RESTRICTED");
			source.Reply(_("Restricted access option for \002{0}\002 is now \002off\002."), ci->GetName());
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable secure";
			ci->SetS<bool>("CS_SECURE", true);
			source.Reply(_("Secure option for \002{0}\002 is now \002on\002."), ci->GetName());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable secure";
			ci->UnsetS<bool>("CS_SECURE");
			source.Reply(_("Secure option for \002{0}\002 is now \002off\002."), ci->GetName());
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && (ci->HasFieldS("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")) && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->GetName());
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable secure founder";
			ci->SetS<bool>("SECUREFOUNDER", true);
			source.Reply(_("Secure founder option for \002{0}\002 is now \002on\002."), ci->GetName());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable secure founder";
			ci->UnsetS<bool>("SECUREFOUNDER");
			source.Reply(_("Secure founder option for \002{0}\002 is now \002off\002."), ci->GetName());
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable secure ops";
			ci->SetS<bool>("SECUREOPS", true);
			source.Reply(_("Secure ops option for \002{0}\002 is now \002on\002."), ci->GetName());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable secure ops";
			ci->UnsetS<bool>("SECUREOPS");
			source.Reply(_("Secure ops option for \002{0}\002 is now \002off\002."), ci->GetName());
		}
		else
			this->OnSyntaxError(source, "SECUREOPS");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables the \002secure ops\002 option for \037channel\037."
		               " When \002secure ops\002 is set, users will not be allowed to have channel operator status if they do not have the privileges for it."));
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (param.equals_ci("ON"))
		{
			ci->SetS<bool>("SIGNKICK", true);
			ci->UnsetS<bool>("SIGNKICK_LEVEL");
			source.Reply(_("Signed kick option for \002{0}\002 is now \002on\002."), ci->GetName());
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable sign kick";
		}
		else if (param.equals_ci("LEVEL"))
		{
			ci->SetS<bool>("SIGNKICK_LEVEL", true);
			ci->UnsetS<bool>("SIGNKICK");
			source.Reply(_("Signed kick option for \002{0}\002 is now \002on\002, but depends of the privileges of the user that is using the command."), ci->GetName());
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable sign kick level";
		}
		else if (param.equals_ci("OFF"))
		{
			ci->UnsetS<bool>("SIGNKICK");
			ci->UnsetS<bool>("SIGNKICK_LEVEL");
			source.Reply(_("Signed kick option for \002{0}\002 is now \002off\002."), ci->GetName());
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
		MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetChannelOption::OnSetChannelOption, source, this, ci, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && (ci->HasFieldS("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")) && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "FOUNDER", ci->GetName());
			return;
		}

		NickServ::Account *nc;

		if (!param.empty())
		{
			NickServ::Nick *na = NickServ::FindNick(param);

			if (!na)
			{
				source.Reply(_("\002{0}\002 isn't registered."), param);
				return;
			}

			if (na->GetAccount() == ci->GetFounder())
			{
				source.Reply(_("\002{0}\002 cannot be the successor of channel \002{1}\002 as they are the founder."), na->GetNick(), ci->GetName());
				return;
			}

			nc = na->GetAccount();
		}
		else
			nc = NULL;

		Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change the successor from " << (ci->GetSuccessor() ? ci->GetSuccessor()->GetDisplay() : "(none)") << " to " << (nc ? nc->GetDisplay() : "(none)");

		ci->SetSuccessor(nc);

		if (nc)
			source.Reply(_("Successor for \002{0}\002 changed to \002{1}\002."), ci->GetName(), nc->GetDisplay());
		else
			source.Reply(_("Successor for \002{0}\002 unset."), ci->GetName());
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
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (param.equals_ci("ON"))
		{
			Log(LOG_ADMIN, source, this, ci) << "to enable noexpire";
			ci->SetS<bool>("CS_NO_EXPIRE", true);
			source.Reply(_("Channel \002{0} will not\002 expire."), ci->GetName());
		}
		else if (param.equals_ci("OFF"))
		{
			Log(LOG_ADMIN, source, this, ci) << "to disable noexpire";
			ci->UnsetS<bool>("CS_NO_EXPIRE");
			source.Reply(_("Channel \002{0} will\002 expire."), ci->GetName());
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
	, public EventHook<Event::ChanRegistered>
	, public EventHook<Event::ChannelSync>
	, public EventHook<Event::CheckKick>
	, public EventHook<Event::DelChan>
	, public EventHook<Event::ChannelModeSet>
	, public EventHook<Event::ChannelModeUnset>
	, public EventHook<Event::JoinChannel>
	, public EventHook<Event::SetCorrectModes>
	, public EventHook<ChanServ::Event::PreChanExpire>
	, public EventHook<Event::ChanInfo>
{
	Serialize::Field<ChanServ::Channel, bool> noautoop, peace, securefounder,
		restricted, secure, secureops, signkick, signkick_level, noexpire, keep_modes, persist;

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

	ExtensibleRef<bool> inhabit;

	bool persist_lower_ts;

 public:
	CSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ChanRegistered>(this)
		, EventHook<Event::ChannelSync>(this)
		, EventHook<Event::CheckKick>(this)
		, EventHook<Event::DelChan>(this)
		, EventHook<Event::ChannelModeSet>(this)
		, EventHook<Event::ChannelModeUnset>(this)
		, EventHook<Event::JoinChannel>(this)
		, EventHook<Event::SetCorrectModes>(this)
		, EventHook<ChanServ::Event::PreChanExpire>(this)
		, EventHook<Event::ChanInfo>(this)

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

		, inhabit("inhabit")
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		persist_lower_ts = conf->GetModule(this)->Get<bool>("persist_lower_ts");
	}

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		ci->SetBanType(Config->GetModule(this)->Get<int>("defbantype", "2"));
	}

	void OnChannelSync(Channel *c) override
	{
		if (c->ci && keep_modes.HasExt(c->ci))
			for (ChanServ::Mode *m : c->ci->GetRefs<ChanServ::Mode *>())
				c->SetMode(c->ci->WhoSends(), m->GetMode(), m->GetParam());
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

			if (mode->type != MODE_STATUS && !c->syncing && Me->IsSynced() && (!inhabit || !inhabit->HasExt(c)))
			{
				ChanServ::Mode *m = Serialize::New<ChanServ::Mode *>();
				if (m != nullptr)
				{
					m->SetChannel(c->ci);
					m->SetMode(mode->name);
					m->SetParam(param);
				}
			}
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

		if (c->ci && mode->type != MODE_STATUS && !c->syncing && Me->IsSynced() && (!inhabit || !inhabit->HasExt(c)))
			for (ChanServ::Mode *m : c->ci->GetRefs<ChanServ::Mode *>())
				if (m->GetMode() == mode->name && m->GetParam().equals_ci(param))
					m->Delete();

		return EVENT_CONTINUE;
	}

	void OnJoinChannel(User *u, Channel *c) override
	{
		if (persist_lower_ts && c->ci && persist.HasExt(c->ci) && c->creation_time > c->ci->GetTimeRegistered())
		{
			Log(LOG_DEBUG) << "Changing TS of " << c->name << " from " << c->creation_time << " to " << c->ci->GetTimeRegistered();
			c->creation_time = c->ci->GetTimeRegistered();
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
