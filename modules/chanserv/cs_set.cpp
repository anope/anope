/* ChanServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/chanserv/mode.h"

class CommandCSSet final
	: public Command
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
		source.Reply(_(
			"Allows the channel founder to set various channel options "
			"and other information."
			"\n\n"
			"Available options:"));
		Anope::string this_name = source.command;
		bool hide_privileged_commands = Config->GetBlock("options").Get<bool>("hideprivilegedcommands"),
		     hide_registered_commands = Config->GetBlock("options").Get<bool>("hideregisteredcommands");

		HelpWrapper help;
		for (const auto &[c_name, info] : source.service->commands)
		{
			if (c_name.find_ci(this_name + " ") == 0)
			{
				if (info.hide)
					continue;

				ServiceReference<Command> c("Command", info.name);

				// XXX dup
				if (!c)
					continue;
				else if (hide_registered_commands && !c->AllowUnregistered() && !source.GetAccount())
					continue;
				else if (hide_privileged_commands && !info.permission.empty() && !source.HasCommand(info.permission))
					continue;

				source.command = c_name;
				c->OnServHelp(source, help);
			}
		}
		help.SendTo(source);

		source.Reply(_("Type \002%s\032\037option\037\002 for more information on a particular option."),
			source.service->GetQueryCommand("generic/help", this_name).c_str());
		return true;
	}
};

class CommandCSSetAutoOp final
	: public Command
{
public:
	CommandCSSetAutoOp(Module *creator, const Anope::string &cname = "chanserv/set/autoop") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Should services automatically give status to users"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable autoop";
			ci->Shrink<bool>("NOAUTOOP");
			source.Reply(_("Services will now automatically give modes to users in \002%s\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable autoop";
			ci->Extend<bool>("NOAUTOOP");
			source.Reply(_("Services will no longer automatically give modes to users in \002%s\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "AUTOOP");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Enables or disables %s's autoop feature for a "
				"channel. When disabled, users who join the channel will "
				"not automatically gain any status from %s."
			),
			source.service->nick.c_str(),
			source.service->nick.c_str());
		return true;
	}
};

class CommandCSSetBanType final
	: public Command
{
public:
	CommandCSSetBanType(Module *creator, const Anope::string &cname = "chanserv/set/bantype") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Set how services make bans on the channel"));
		this->SetSyntax(_("\037channel\037 \037bantype\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		auto new_type = Anope::Convert<int16_t>(params[1], -1);
		if (new_type < 0 || new_type > 3)
		{
			source.Reply(_("\002%s\002 is not a valid ban type."), params[1].c_str());
			return;
		}

		Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change the ban type to " << new_type;
		ci->bantype = new_type;
		source.Reply(_("Ban type for channel %s is now #%d."), ci->name.c_str(), ci->bantype);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Sets the ban type that will be used by services whenever "
			"they need to ban someone from your channel."
			"\n\n"
			"Bantype is a number between 0 and 3 that means:"
			"\n\n"
			"0: ban in the form *!user@host\n"
			"1: ban in the form *!*user@host\n"
			"2: ban in the form *!*@host\n"
			"3: ban in the form *!*user@*.domain"
		));
		return true;
	}
};

class CommandCSSetDescription final
	: public Command
{
public:
	CommandCSSetDescription(Module *creator, const Anope::string &cname = "chanserv/set/description") : Command(creator, cname, 1, 2)
	{
		this->SetDesc(_("Set the channel description"));
		this->SetSyntax(_("\037channel\037 [\037description\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		const Anope::string &param = params.size() > 1 ? params[1] : "";
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (!param.empty())
		{
			ci->desc = param;
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change the description to " << ci->desc;
			source.Reply(_("Description of %s changed to \002%s\002."), ci->name.c_str(), ci->desc.c_str());
		}
		else
		{
			ci->desc.clear();
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to unset the description";
			source.Reply(_("Description of %s unset."), ci->name.c_str());
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Sets the description for the channel, which shows up with "
			"the \002LIST\002 and \002INFO\002 commands."
		));
		return true;
	}
};

class CommandCSSetFounder final
	: public Command
{
public:
	CommandCSSetFounder(Module *creator, const Anope::string &cname = "chanserv/set/founder") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Set the founder of a channel"));
		this->SetSyntax(_("\037channel\037 \037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && (ci->HasExt("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")) && source.permission.empty() && !source.HasPriv("chanserv/administration"))
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
		else if (na->nc->HasExt("NEVEROP"))
		{
			source.Reply(_("\002%s\002 does not wish to be added to channel access lists."),
				na->nc->display.c_str());
			return;
		}

		NickCore *nc = na->nc;
		unsigned max_reg = Config->GetModule("chanserv").Get<unsigned>("maxregistered");
		if (max_reg && nc->channelcount >= max_reg && !source.HasPriv("chanserv/no-register-limit"))
		{
			source.Reply(_("\002%s\002 has too many channels registered."), na->nick.c_str());
			return;
		}

		Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change the founder from " << (ci->GetFounder() ? ci->GetFounder()->display : "(none)") << " to " << nc->display;

		ci->SetFounder(nc);

		source.Reply(_("Founder of \002%s\002 changed to \002%s\002."), ci->name.c_str(), na->nick.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Changes the founder of a channel. The new nickname must "
			"be a registered one."
		));
		return true;
	}
};

class CommandCSSetKeepModes final
	: public Command
{
public:
	CommandCSSetKeepModes(Module *creator, const Anope::string &cname = "chanserv/set/keepmodes") :  Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Retain modes when channel is not in use"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable keep modes";
			ci->Extend<bool>("CS_KEEP_MODES");
			source.Reply(_("Keep modes for %s is now \002on\002."), ci->name.c_str());
			if (ci->c)
				ci->last_modes = ci->c->GetModes();
		}
		else if (params[1].equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable keep modes";
			ci->Shrink<bool>("CS_KEEP_MODES");
			source.Reply(_("Keep modes for %s is now \002off\002."), ci->name.c_str());
			ci->last_modes.clear();
		}
		else
			this->OnSyntaxError(source, "KEEPMODES");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Enables or disables keepmodes for the given channel. If keep "
			"modes is enabled, services will remember modes set on the channel "
			"and attempt to re-set them the next time the channel is created."
		));
		return true;
	}
};

class CommandCSSetPeace final
	: public Command
{
public:
	CommandCSSetPeace(Module *creator, const Anope::string &cname = "chanserv/set/peace") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Regulate the use of critical commands"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable peace";
			ci->Extend<bool>("PEACE");
			source.Reply(_("Peace option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable peace";
			ci->Shrink<bool>("PEACE");
			source.Reply(_("Peace option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PEACE");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Enables or disables the \002peace\002 option for a channel. "
				"When \002peace\002 is set, a user won't be able to kick, "
				"ban or remove a channel status of a user that has "
				"a level superior or equal to theirs via %s commands."
			),
			source.service->nick.c_str());
		return true;
	}
};

inline static Anope::string BotModes()
{
	return Config->GetModule("botserv").Get<Anope::string>("botmodes",
		Config->GetModule("chanserv").Get<Anope::string>("botmodes", "o")
	);
}

class CommandCSSetPersist final
	: public Command
{
public:
	CommandCSSetPersist(Module *creator, const Anope::string &cname = "chanserv/set/persist") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Set the channel as permanent"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		ChannelMode *cm = ModeManager::FindChannelModeByName("PERM");

		if (params[1].equals_ci("ON"))
		{
			if (!ci->HasExt("PERSIST"))
			{
				ci->Extend<bool>("PERSIST");

				/* Set the perm mode */
				if (cm)
				{
					/* Add it to the channels mlock */
					ModeLocks *ml = ci->Require<ModeLocks>("modelocks");
					if (ml)
						ml->SetMLock(cm, true, "", source.GetNick());

					if (ci->c && !ci->c->HasMode("PERM"))
						ci->c->SetMode(NULL, cm);
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
					if (ci->c && !ci->c->FindUser(ChanServ))
					{
						ChannelStatus status(BotModes());
						ChanServ->Join(ci->c, &status);
					}
				}
			}

			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable persist";
			source.Reply(_("Channel \002%s\002 is now persistent."), ci->name.c_str());
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
					/* Remove from mlock */
					ModeLocks *ml = ci->GetExt<ModeLocks>("modelocks");
					if (ml)
						ml->RemoveMLock(cm, true);

					if (ci->c && ci->c->HasMode("PERM"))
						ci->c->RemoveMode(NULL, cm);
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
			source.Reply(_("Channel \002%s\002 is no longer persistent."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PERSIST");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		BotInfo *BotServ = Config->GetClient("BotServ");
		BotInfo *ChanServ = Config->GetClient("ChanServ");
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
				"Enables or disables the persistent channel setting. "
				"When persistent is set, the service bot will remain "
				"in the channel when it has emptied of users. "
				"\n\n"
				"If your IRCd does not have a permanent (persistent) channel "
				"mode you must have a service bot in your channel to "
				"set persist on, and it can not be unassigned while persist "
				"is on."
				"\n\n"
				"If this network does not have %s enabled and does "
				"not have a permanent channel mode, %s will "
				"join your channel when you set persist on (and leave when "
				"it has been set off)."
				"\n\n"
				"If your IRCd has a permanent (persistent) channel mode "
				"and it is set or unset (for any reason, including MODE LOCK), "
				"persist is automatically set and unset for the channel as well. "
				"Additionally, services will set or unset this mode when you "
				"set persist on or off."
			),
			BotServ ? BotServ->nick.c_str() : "BotServ",
			ChanServ ? ChanServ->nick.c_str() : "ChanServ"
		);
		return true;
	}
};

class CommandCSSetRestricted final
	: public Command
{
public:
	CommandCSSetRestricted(Module *creator, const Anope::string &cname = "chanserv/set/restricted") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Restrict access to the channel"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable restricted";
			ci->Extend<bool>("RESTRICTED");
			source.Reply(_("Restricted access option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable restricted";
			ci->Shrink<bool>("RESTRICTED");
			source.Reply(_("Restricted access option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "RESTRICTED");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Enables or disables the \002restricted access\002 option for a "
			"channel. When \002restricted access\002 is set, users not on the access list will "
			"instead be kicked and banned from the channel."
		));
		return true;
	}
};

class CommandCSSetSecureFounder final
	: public Command
{
public:
	CommandCSSetSecureFounder(Module *creator, const Anope::string &cname = "chanserv/set/securefounder") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Stricter control of channel founder status"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && (ci->HasExt("SECUREFOUNDER") ? !source.IsFounder(ci) : !source.AccessFor(ci).HasPriv("FOUNDER")) && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable secure founder";
			ci->Extend<bool>("SECUREFOUNDER");
			source.Reply(_("Secure founder option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable secure founder";
			ci->Shrink<bool>("SECUREFOUNDER");
			source.Reply(_("Secure founder option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SECUREFOUNDER");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Enables or disables the \002secure founder\002 option for a channel. "
			"When \002secure founder\002 is set, only the real founder will be "
			"able to drop the channel, change its founder and its successor, "
			"and not those who have founder level access through "
			"the access/qop command."
		));
		return true;
	}
};

class CommandCSSetSecureOps final
	: public Command
{
public:
	CommandCSSetSecureOps(Module *creator, const Anope::string &cname = "chanserv/set/secureops") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Stricter control of chanop status"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable secure ops";
			ci->Extend<bool>("SECUREOPS");
			source.Reply(_("Secure ops option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable secure ops";
			ci->Shrink<bool>("SECUREOPS");
			source.Reply(_("Secure ops option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "SECUREOPS");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Enables or disables the \002secure ops\002 option for a channel. "
			"When \002secure ops\002 is set, users who are not on the access list "
			"will not be allowed channel operator status."
		));
		return true;
	}
};

class CommandCSSetSignKick final
	: public Command
{
public:
	CommandCSSetSignKick(Module *creator, const Anope::string &cname = "chanserv/set/signkick") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Sign kicks that are done with the KICK command"));
		this->SetSyntax(_("\037channel\037 {ON | LEVEL | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && !source.AccessFor(ci).HasPriv("SET") && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->Extend<bool>("SIGNKICK");
			ci->Shrink<bool>("SIGNKICK_LEVEL");
			source.Reply(_("Signed kick option for %s is now \002on\002."), ci->name.c_str());
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable sign kick";
		}
		else if (params[1].equals_ci("LEVEL"))
		{
			ci->Extend<bool>("SIGNKICK_LEVEL");
			ci->Shrink<bool>("SIGNKICK");
			source.Reply(_("Signed kick option for %s is now \002on\002, but depends of the level of the user that is using the command."),
				ci->name.c_str());
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to enable sign kick level";
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->Shrink<bool>("SIGNKICK");
			ci->Shrink<bool>("SIGNKICK_LEVEL");
			source.Reply(_("Signed kick option for %s is now \002off\002."), ci->name.c_str());
			Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to disable sign kick";
		}
		else
			this->OnSyntaxError(source, "SIGNKICK");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Enables or disables signed kicks for a "
			"channel. When \002SIGNKICK\002 is set, kicks issued with "
			"the \002KICK\002 command will have the nick that used the "
			"command in their reason."
			"\n\n"
			"If you use \002LEVEL\002, those who have a level that is superior "
			"or equal to the SIGNKICK level on the channel won't have their "
			"kicks signed."
		));
		return true;
	}
};

class CommandCSSetSuccessor final
	: public Command
{
public:
	CommandCSSetSuccessor(Module *creator, const Anope::string &cname = "chanserv/set/successor") : Command(creator, cname, 1, 2)
	{
		this->SetDesc(_("Set the successor for a channel"));
		this->SetSyntax(_("\037channel\037 [\037nick\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		const Anope::string &param = params.size() > 1 ? params[1] : "";
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetChannelOption, MOD_RESULT, (source, this, ci, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		auto can_set_successor = ci->HasExt("SECUREFOUNDER")
			? source.IsFounder(ci)
			: source.AccessFor(ci).HasPriv("FOUNDER");

		// Special case: users can remove themselves as successor with no other privs.
		if (param.empty() && source.GetAccount() && source.GetAccount() == ci->GetSuccessor())
			can_set_successor = true;

		if (MOD_RESULT != EVENT_ALLOW && !can_set_successor && source.permission.empty() && !source.HasPriv("chanserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		NickCore *nc;

		if (!param.empty())
		{
			const NickAlias *na = NickAlias::Find(param);

			if (!na)
			{
				source.Reply(NICK_X_NOT_REGISTERED, param.c_str());
				return;
			}
			else if (na->nc->HasExt("NEVEROP"))
			{
				source.Reply(_("\002%s\002 does not wish to be added to channel access lists."),
					na->nc->display.c_str());
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

		Log(source.AccessFor(ci).HasPriv("SET") ? LOG_COMMAND : LOG_OVERRIDE, source, this, ci) << "to change the successor from " << (ci->GetSuccessor() ? ci->GetSuccessor()->display : "(none)") << " to " << (nc ? nc->display : "(none)");

		ci->SetSuccessor(nc);

		if (nc)
			source.Reply(_("Successor for \002%s\002 changed to \002%s\002."), ci->name.c_str(), nc->display.c_str());
		else
			source.Reply(_("Successor for \002%s\002 unset."), ci->name.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Changes the successor of a channel. If the founder's "
			"nickname expires or is dropped while the channel is still "
			"registered, the successor will become the new founder of the "
			"channel. The successor's nickname must be a registered one. "
			"If there's no successor set, then the first nickname on the "
			"access list (with the highest access, if applicable) will "
			"become the new founder, but if the access list is empty, the "
			"channel will be dropped."
		));

		unsigned max_reg = Config->GetModule("chanserv").Get<unsigned>("maxregistered");
		if (max_reg)
		{
			source.Reply(" ");
			source.Reply(_(
					"Note, however, if the successor already has too many "
					"channels registered (%u), they will not be able to "
					"become the new founder and it will be as if the "
					"channel had no successor set."
				),
				max_reg);
		}
		return true;
	}
};

class CommandCSSetNoexpire final
	: public Command
{
public:
	CommandCSSetNoexpire(Module *creator) : Command(creator, "chanserv/saset/noexpire", 2, 2)
	{
		this->SetDesc(_("Prevent the channel from expiring"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

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
			Log(LOG_ADMIN, source, this, ci) << "to enable noexpire";
			ci->Extend<bool>("CS_NO_EXPIRE");
			source.Reply(_("Channel %s \002will not\002 expire."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			Log(LOG_ADMIN, source, this, ci) << "to disable noexpire";
			ci->Shrink<bool>("CS_NO_EXPIRE");
			source.Reply(_("Channel %s \002will\002 expire."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "NOEXPIRE");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Sets whether the given channel will expire. Setting this "
			"to ON prevents the channel from expiring."
		));
		return true;
	}
};

class CSSet final
	: public Module
{
	SerializableExtensibleItem<bool> noautoop, peace, securefounder,
		restricted, secureops, signkick, signkick_level, noexpire,
		persist;

	struct KeepModes final
		: SerializableExtensibleItem<bool>
	{
		KeepModes(Module *m, const Anope::string &n) : SerializableExtensibleItem<bool>(m, n) { }

		void ExtensibleSerialize(const Extensible *e, const Serializable *s, Serialize::Data &data) const override
		{
			SerializableExtensibleItem<bool>::ExtensibleSerialize(e, s, data);

			if (s->GetSerializableType()->GetName() != CHANNELINFO_TYPE)
				return;

			const ChannelInfo *ci = anope_dynamic_static_cast<const ChannelInfo *>(s);
			Anope::string modes;
			for (const auto &[last_mode, last_data] : ci->last_modes)
			{
				if (!modes.empty())
					modes += " ";

				modes += '+';
				modes += last_mode;
				if (!last_data.value.empty())
				{
					modes += "," + Anope::ToString(last_data.set_at);
					modes += "," + last_data.set_by;
					modes += "," + last_data.value;
				}
			}
			data.Store("last_modes", modes);
		}

		void ExtensibleUnserialize(Extensible *e, Serializable *s, Serialize::Data &data) override
		{
			SerializableExtensibleItem<bool>::ExtensibleUnserialize(e, s, data);

			if (s->GetSerializableType()->GetName() != CHANNELINFO_TYPE)
				return;

			ChannelInfo *ci = anope_dynamic_static_cast<ChannelInfo *>(s);
			Anope::string modes;
			data["last_modes"] >> modes;
			ci->last_modes.clear();
			for (spacesepstream sep(modes); sep.GetToken(modes);)
			{
				if (modes[0] == '+')
				{
					commasepstream mode(modes, true);
					mode.GetToken(modes);
					modes.erase(0, 1);

					ModeData info;
					Anope::string set_at;
					mode.GetToken(set_at);
					info.set_at = Anope::Convert(set_at, 0);
					mode.GetToken(info.set_by);
					info.value = mode.GetRemaining();

					ci->last_modes.emplace(modes, info);
					continue;
				}
				else
				{
					// Begin 2.0 compatibility.
					size_t c = modes.find(',');
					if (c == Anope::string::npos)
						ci->last_modes.emplace(modes, ModeData());
					else
						ci->last_modes.emplace(modes.substr(0, c), ModeData(modes.substr(c + 1)));
					// End 2.0 compatibility.
				}
			}
		}
	} keep_modes;

	CommandCSSet commandcsset;
	CommandCSSetAutoOp commandcssetautoop;
	CommandCSSetBanType commandcssetbantype;
	CommandCSSetDescription commandcssetdescription;
	CommandCSSetFounder commandcssetfounder;
	CommandCSSetKeepModes commandcssetkeepmodes;
	CommandCSSetPeace commandcssetpeace;
	CommandCSSetPersist commandcssetpersist;
	CommandCSSetRestricted commandcssetrestricted;
	CommandCSSetSecureFounder commandcssetsecurefounder;
	CommandCSSetSecureOps commandcssetsecureops;
	CommandCSSetSignKick commandcssetsignkick;
	CommandCSSetSuccessor commandcssetsuccessor;
	CommandCSSetNoexpire commandcssetnoexpire;

	ExtensibleRef<bool> inhabit;

	bool persist_lower_ts;

public:
	CSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		noautoop(this, "NOAUTOOP"), peace(this, "PEACE"),
		securefounder(this, "SECUREFOUNDER"), restricted(this, "RESTRICTED"),
		secureops(this, "SECUREOPS"), signkick(this, "SIGNKICK"),
		signkick_level(this, "SIGNKICK_LEVEL"), noexpire(this, "CS_NO_EXPIRE"),
		persist(this, "PERSIST"),
		keep_modes(this, "CS_KEEP_MODES"),

		commandcsset(this), commandcssetautoop(this), commandcssetbantype(this),
		commandcssetdescription(this), commandcssetfounder(this), commandcssetkeepmodes(this),
		commandcssetpeace(this), commandcssetpersist(this), commandcssetrestricted(this),
		commandcssetsecurefounder(this), commandcssetsecureops(this), commandcssetsignkick(this),
		commandcssetsuccessor(this), commandcssetnoexpire(this),

		inhabit("inhabit")
	{
	}

	void OnReload(Configuration::Conf &conf) override
	{
		persist_lower_ts = conf.GetModule(this).Get<bool>("persist_lower_ts");
	}

	void OnCreateChan(ChannelInfo *ci) override
	{
		ci->bantype = Config->GetModule(this).Get<int>("defbantype", "2");
	}

	void OnChannelSync(Channel *c) override
	{
		if (c->ci && keep_modes.HasExt(c->ci))
		{
			Channel::ModeList ml = c->ci->last_modes;
			for (const auto &[last_mode, last_data] : ml)
				c->SetMode(c->ci->WhoSends(), last_mode, last_data);
		}
	}

	EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) override
	{
		if (!c->ci || !restricted.HasExt(c->ci) || c->MatchesList(u, "EXCEPT"))
			return EVENT_CONTINUE;

		if (c->ci->AccessFor(u).empty() && (!c->ci->GetFounder() || u->Account() != c->ci->GetFounder()))
			return EVENT_STOP;

		return EVENT_CONTINUE;
	}

	void OnDelChan(ChannelInfo *ci) override
	{
		if (ci->c && persist.HasExt(ci))
			ci->c->RemoveMode(ci->WhoSends(), "PERM", "", false);
		persist.Unset(ci);
	}

	EventReturn OnChannelModeSet(Channel *c, MessageSource &setter, ChannelMode *mode, const ModeData &data) override
	{
		if (c->ci)
		{
			/* Channel mode +P or so was set, mark this channel as persistent */
			if (mode->name == "PERM")
				persist.Set(c->ci, true);

			if (mode->type != MODE_STATUS && !c->syncing && Me->IsSynced() && (!inhabit || !inhabit->HasExt(c)))
				c->ci->last_modes = c->GetModes();
		}

		return EVENT_CONTINUE;
	}

	EventReturn OnChannelModeUnset(Channel *c, MessageSource &setter, ChannelMode *mode, const Anope::string &param) override
	{
		if (mode->name == "PERM")
		{
			if (c->ci)
				persist.Unset(c->ci);
		}

		if (c->ci && mode->type != MODE_STATUS && !c->syncing && Me->IsSynced() && (!inhabit || !inhabit->HasExt(c)))
			c->ci->last_modes = c->GetModes();

		return EVENT_CONTINUE;
	}

	void OnJoinChannel(User *u, Channel *c) override
	{
		if (u->server != Me && persist_lower_ts && c->ci && persist.HasExt(c->ci) && c->created > c->ci->registered)
		{
			Log(LOG_DEBUG) << "Changing TS of " << c->name << " from " << c->created << " to " << c->ci->registered;
			c->created = c->ci->registered;
			IRCD->SendChannel(c);
			c->Reset();
		}
	}

	void OnSetCorrectModes(User *user, Channel *chan, AccessGroup &access, bool &give_modes, bool &take_modes) override
	{
		if (chan->ci)
		{
			if (noautoop.HasExt(chan->ci))
				give_modes = false;
			if (secureops.HasExt(chan->ci) && !user->HasPriv("chanserv/administration"))
				// This overrides what chanserv does because it is loaded after chanserv
				take_modes = true;
		}
	}

	void OnPreChanExpire(ChannelInfo *ci, bool &expire) override
	{
		if (noexpire.HasExt(ci))
			expire = false;
	}

	void OnChanInfo(CommandSource &source, ChannelInfo *ci, InfoFormatter &info, bool show_all) override
	{
		if (!show_all)
			return;

		if (peace.HasExt(ci))
			info.AddOption(_("Peace"));
		if (restricted.HasExt(ci))
			info.AddOption(_("Restricted access"));
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
