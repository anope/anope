/*
 * Anope IRC Services
 *
 * Copyright (C) 2013-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
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

		if (!source.AccessFor(ci).HasPriv("SET") && !source.HasOverridePriv("botserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SET", ci->GetName());
			return;
		}

		if (value.equals_ci("ON"))
		{
			logger.Command(source, ci, _("{source} used {command} on {channel} to enable greets"));

			ci->SetGreet(true);
			source.Reply(_("Greet mode for \002{0}\002 is now \002on\002."), ci->GetName());
		}
		else if (value.equals_ci("OFF"))
		{
			logger.Command(source, ci, _("{source} used {command} on {channel} to disable greets"));

			ci->SetGreet(false);
			source.Reply(_("Greet mode for \002{0}\002 is now \002off\002."), ci->GetName());
		}
		else
		{
			this->OnSyntaxError(source, source.GetCommand());
		}
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
			logger.Command(nc == source.GetAccount() ? LogType::COMMAND : LogType::ADMIN, source, _("{source} used {command} to change the greet of {0}"), nc->GetDisplay());

			nc->SetGreet(param);
			source.Reply(_("Greet message for \002{0}\002 changed to \002{1}\002."), nc->GetDisplay(), param);
		}
		else
		{
			logger.Command(nc == source.GetAccount() ? LogType::COMMAND : LogType::ADMIN, source, _("{source} used {command} to unset the greet of {0}"), nc->GetDisplay());

			nc->SetGreet("");
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
	CommandBSSetGreet commandbssetgreet;
	CommandNSSetGreet commandnssetgreet;
	CommandNSSASetGreet commandnssasetgreet;

 public:
	Greet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::JoinChannel>(this)
		, EventHook<Event::NickInfo>(this)
		, EventHook<Event::ServiceBotEvent>(this)
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

		Anope::string greet = user->Account()->GetGreet();
		if (c->ci->IsGreet() && !greet.empty() && c->FindUser(c->ci->GetBot()) && c->ci->AccessFor(user).HasPriv("GREET"))
		{
			IRCD->SendPrivmsg(c->ci->GetBot(), c->name, "[{0}] {1}", user->Account()->GetDisplay(), greet);
			c->ci->GetBot()->lastmsg = Anope::CurTime;
		}
	}

	void OnNickInfo(CommandSource &source, NickServ::Nick *na, InfoFormatter &info, bool show_hidden) override
	{
		Anope::string greet = na->GetAccount()->GetGreet();
		if (!greet.empty())
			info[_("Greet")] = greet;
	}

	void OnServiceBot(CommandSource &source, ServiceBot *bi, ChanServ::Channel *ci, InfoFormatter &info) override
	{
		if (ci->IsGreet())
			info.AddOption(_("Greet"));
	}
};

MODULE_INIT(Greet)
