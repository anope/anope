/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
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

class NSIdentifyRequestListener : public NickServ::IdentifyRequestListener
{
	CommandSource source;
	Command *cmd;

 public:
	NSIdentifyRequestListener(CommandSource &s, Command *c) : source(s), cmd(c) { }

	void OnSuccess(NickServ::IdentifyRequest *req) override
	{
		if (!source.GetUser())
			return;

		User *u = source.GetUser();
		NickServ::Nick *na = NickServ::FindNick(req->GetAccount());

		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), req->GetAccount());
			return;
		}

		if (u->IsIdentified())
			cmd->logger.Command(LogType::COMMAND, source, _("{source} used {command} to log out of account {0}"), u->Account()->GetDisplay());

		cmd->logger.Command(LogType::COMMAND, source, _("{source} used {command} and identified for account {0}"), na->GetAccount()->GetDisplay());

		source.Reply(_("Password accepted - you are now recognized as \002{0}\002."), na->GetAccount()->GetDisplay());
		u->Identify(na);
	}

	void OnFail(NickServ::IdentifyRequest *req) override
	{
		if (!source.GetUser())
			return;

		bool accountexists = NickServ::FindNick(req->GetAccount()) != NULL;
		if (!accountexists)
			cmd->logger.Command(LogType::COMMAND, source, _("{source} used {command} and failed to identify to nonexistent account {0}"), req->GetAccount());
		else
			cmd->logger.Command(LogType::COMMAND, source, _("{source} used {command} and failed to identify to account {0}"), req->GetAccount());

		if (accountexists)
		{
			source.Reply(_("Password incorrect."));
			source.GetUser()->BadPassword();
		}
		else
		{
			source.Reply("\002{0}\002 isn't registered.", req->GetAccount());
		}
	}
};

class CommandNSIdentify : public Command
{
 public:
	CommandNSIdentify(Module *creator) : Command(creator, "nickserv/identify", 1, 2)
	{
		this->SetDesc(_("Identify yourself with your password"));
		this->SetSyntax(_("[\037account\037] \037password\037"));
		this->AllowUnregistered(true);
		this->RequireUser(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		User *u = source.GetUser();

		const Anope::string &nick = params.size() == 2 ? params[0] : u->nick;
		Anope::string pass = params[params.size() - 1];

		NickServ::Nick *na = NickServ::FindNick(nick);
		if (na && na->GetAccount()->HasFieldS("NS_SUSPENDED"))
		{
			source.Reply(_("\002{0}\002 is suspended."), na->GetNick());
			return;
		}

		if (u->Account() && na && u->Account() == na->GetAccount())
		{
			source.Reply(_("You are already identified."));
			return;
		}

		unsigned int maxlogins = Config->GetModule(this->GetOwner())->Get<unsigned int>("maxlogins");
		if (na && maxlogins && na->GetAccount()->users.size() >= maxlogins)
		{
			source.Reply(_("Account \002{0}\002 has already reached the maximum number of simultaneous logins ({1})."), na->GetAccount()->GetDisplay(), maxlogins);
			return;
		}

		NickServ::IdentifyRequest *req = NickServ::service->CreateIdentifyRequest(new NSIdentifyRequestListener(source, this), this->GetOwner(), na ? na->GetAccount()->GetDisplay() : nick, pass);
		EventManager::Get()->Dispatch(&Event::CheckAuthentication::OnCheckAuthentication, u, req);

		req->Dispatch();
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Logs you in to account \037account\037. If no \037account\037 is given, your current nickname is used."
		               " Many commands require you to authenticate with this command before you use them. \037password\037 should be the same one you registered with."));
		return true;
	}
};

class NSIdentify : public Module
{
	CommandNSIdentify commandnsidentify;

 public:
	NSIdentify(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsidentify(this)
	{

	}
};

MODULE_INIT(NSIdentify)
