/*
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
#include "modules/nickserv/info.h"
#include "modules/botserv/info.h"
#include "modules/nickserv/set.h"

class CommandBSSetGreet : public Command
{
 public:
	CommandBSSetGreet(Module *creator, const Anope::string &sname = "botserv/set/greet") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Enable greet messages"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &value = params[1];

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

		if (!source.HasPriv("botserv/administration") && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (value.equals_ci("ON"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enable greets";

			ci->SetS<bool>("BS_GREET", true);
			source.Reply(_("Greet mode for \002{0}\002 is now \002on\002."), ci->GetName());
		}
		else if (value.equals_ci("OFF"))
		{
			bool override = !source.AccessFor(ci).HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable greets";

			ci->UnsetS<bool>("BS_GREET");
			source.Reply(_("Greet mode for \002{0}\002 is now \002off\002."), ci->GetName());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Enables or disables \002greet\002 mode on \037channel\037."
		               " When \002greet\002 mode is enabled, the assigned service bot will display the greet messages of users joining the channel, if they have the \002{0}\002 privilege."),
		               "GREET");
		return true;
	}
};

class CommandNSSetGreet : public Command
{
 public:
	CommandNSSetGreet(Module *creator, const Anope::string &sname = "nickserv/set/greet", size_t min = 0) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Associate a greet message with your nickname"));
		this->SetSyntax(_("\037message\037"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->GetAccount();

		EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (!param.empty())
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change the greet of " << nc->GetDisplay();
			nc->SetS<Anope::string>("greet", param);
			source.Reply(_("Greet message for \002{0}\002 changed to \002{1}\002."), nc->GetDisplay(), param);
		}
		else
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to unset the greet of " << nc->GetDisplay();
			nc->UnsetS<Anope::string>("greet");
			source.Reply(_("Greet message for \002{0}\002 unset."), nc->GetDisplay());
		}
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->GetDisplay(), params.size() > 0 ? params[0] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Sets your greet to \037message\037. Your greet is displayed when you enter channels that have the greet option enabled if you have the \002{0}\002 privilege on the channel."),
		               "GREET");
		return true;
	}
};

class CommandNSSASetGreet : public CommandNSSetGreet
{
 public:
	CommandNSSASetGreet(Module *creator) : CommandNSSetGreet(creator, "nickserv/saset/greet", 1)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037user\037 \037message\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, params[0], params.size() > 1 ? params[1] : "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Sets the greet for \037user\037 to \037message\037. Greet messages are displayed when users enter channels that have the greet option enabiled if they have the \002{0}\002 privilege on the channel."),
		               "GREET");
		return true;
	}
};

class Greet : public Module
	, public EventHook<Event::JoinChannel>
	, public EventHook<Event::NickInfo>
	, public EventHook<Event::ServiceBotEvent>
{
	/* channel setting for whether or not greet should be shown */
	Serialize::Field<ChanServ::Channel, bool> bs_greet;
	/* user greets */
	Serialize::Field<NickServ::Account, Anope::string> ns_greet;

	CommandBSSetGreet commandbssetgreet;
	CommandNSSetGreet commandnssetgreet;
	CommandNSSASetGreet commandnssasetgreet;

 public:
	Greet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::JoinChannel>(this)
		, EventHook<Event::NickInfo>(this)
		, EventHook<Event::ServiceBotEvent>(this)
		, bs_greet(this, "BS_GREET")
		, ns_greet(this, "greet")
		, commandbssetgreet(this)
		, commandnssetgreet(this)
		, commandnssasetgreet(this)
	{
	}

	void OnJoinChannel(User *user, Channel *c) override
	{
		/* Only display the greet if the main uplink we're connected
		 * to has synced, or we'll get greet-floods when the net
		 * recovers from a netsplit. -GD
		 */
		if (!c->ci || !c->ci->GetBot() || !user->server->IsSynced() || !user->Account())
			return;

		Anope::string greet = ns_greet.Get(user->Account());
		if (bs_greet.HasExt(c->ci) && !greet.empty() && c->FindUser(c->ci->GetBot()) && c->ci->AccessFor(user).HasPriv("GREET"))
		{
			IRCD->SendPrivmsg(c->ci->GetBot(), c->name, "[%s] %s", user->Account()->GetDisplay().c_str(), greet.c_str());
			c->ci->GetBot()->lastmsg = Anope::CurTime;
		}
	}

	void OnNickInfo(CommandSource &source, NickServ::Nick *na, InfoFormatter &info, bool show_hidden) override
	{
		Anope::string greet = ns_greet.Get(na->GetAccount());
		if (!greet.empty())
			info[_("Greet")] = greet;
	}

	void OnServiceBot(CommandSource &source, ServiceBot *bi, ChanServ::Channel *ci, InfoFormatter &info) override
	{
		if (bs_greet.HasExt(ci))
			info.AddOption(_("Greet"));
	}
};

MODULE_INIT(Greet)
