/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
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
#include "modules/nickserv/cert.h"
#include "modules/nickserv.h"

typedef std::map<Anope::string, ChannelStatus> NSRecoverInfo;

class NSRecoverSvsnick
{
 public:
	Reference<User> from;
	Anope::string to;
};

class NSRecoverRequestListener : public NickServ::IdentifyRequestListener
{
	CommandSource source;
	Command *cmd;
	Anope::string user;
	Anope::string pass;

 public:
	NSRecoverRequestListener(CommandSource &src, Command *c, const Anope::string &nick, const Anope::string &p) : source(src), cmd(c), user(nick), pass(p) { }

	void OnSuccess(NickServ::IdentifyRequest *) override
	{
		User *u = User::Find(user, true);
		if (!source.GetUser() || !source.service)
			return;

		NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
			return;

		Log(LOG_COMMAND, source, cmd) << "for " << na->GetNick();

		/* Nick is being held by us, release it */
		if (na->HasFieldS("HELD"))
		{
			NickServ::service->Release(na);
			source.Reply(_("Service's hold on \002{0}\002 has been released."), na->GetNick());
		}
		else if (!u)
		{
			source.Reply(_("No one is using your nick, and services are not holding it."));
		}
		// If the user being recovered is identified for the account of the nick then the user is the
		// same person that is executing the command, so kill them off (old GHOST command).
		else if (u->Account() == na->GetAccount())
		{
			if (!source.GetAccount() && na->GetAccount()->HasFieldS("NS_SECURE"))
			{
				source.GetUser()->Login(u->Account());
				Log(LOG_COMMAND, source, cmd) << "and was automatically identified to " << u->Account()->GetDisplay();
			}

			if (Config->GetModule("ns_recover")->Get<bool>("restoreonrecover"))
			{
				if (!u->chans.empty())
				{
					NSRecoverInfo i;
					for (User::ChanUserList::iterator it = u->chans.begin(), it_end = u->chans.end(); it != it_end; ++it)
						i[it->first->name] = it->second->status;
					source.GetUser()->Extend<NSRecoverInfo>("recover", i);
				}
			}

			u->SendMessage(*source.service, _("This nickname has been recovered by \002{0}\002. If you did not do this, then \002{0}\002 may have your password, and you should change it."),
							source.GetNick());

			Anope::string buf = source.command.upper() + " command used by " + source.GetNick();
			u->Kill(*source.service, buf);

			source.Reply(_("Ghost with your nick has been killed."));

			if (IRCD->CanSVSNick)
				IRCD->SendForceNickChange(source.GetUser(), user, Anope::CurTime);
		}
		/* User is not identified or not identified to the same account as the person using this command */
		else
		{
			if (!source.GetAccount() && na->GetAccount()->HasFieldS("NS_SECURE"))
			{
				source.GetUser()->Login(na->GetAccount()); // Identify the user using the command if they arent identified
				Log(LOG_COMMAND, source, cmd) << "and was automatically identified to " << na->GetNick() << " (" << na->GetAccount()->GetDisplay() << ")";
				source.Reply(_("You have been logged in as \002{0}\002."), na->GetAccount()->GetDisplay());
			}

			u->SendMessage(*source.service, _("This nickname has been recovered by \002{0}\002."), source.GetNick());

			if (IRCD->CanSVSNick)
			{
				NSRecoverSvsnick svs;

				svs.from = source.GetUser();
				svs.to = u->nick;

				u->Extend<NSRecoverSvsnick>("svsnick", svs);
			}

			if (NickServ::service)
				NickServ::service->Collide(u, na);

			if (IRCD->CanSVSNick)
			{
				/* If we can svsnick then release our hold and svsnick the user using the command */
				if (NickServ::service)
					NickServ::service->Release(na);

				source.Reply(_("You have regained control of \002{0}\002."), u->nick);
			}
			else
				source.Reply(_("The user with your nick has been removed. Use this command again to release services's hold on your nick."));
		}
	}

	void OnFail(NickServ::IdentifyRequest *) override
	{
		if (NickServ::FindNick(user) != NULL)
		{
			source.Reply(_("Access denied."));
			if (!pass.empty())
			{
				Log(LOG_COMMAND, source, cmd) << "with an invalid password for " << user;
				if (source.GetUser())
					source.GetUser()->BadPassword();
			}
		}
		else
			source.Reply(_("\002{0}\002 isn't registered."), user);
	}
};

class CommandNSRecover : public Command
{
	ServiceReference<CertService> certservice;
	
 public:
	CommandNSRecover(Module *creator) : Command(creator, "nickserv/recover", 1, 2)
	{
		this->SetDesc(_("Regains control of your nick"));
		this->SetSyntax(_("\037nickname\037 [\037password\037]"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[0];
		const Anope::string &pass = params.size() > 1 ? params[1] : "";

		User *user = User::Find(nick, true);

		if (user && source.GetUser() == user)
		{
			source.Reply(_("You can't %s yourself!"), source.command.lower().c_str());
			return;
		}

		NickServ::Nick *na = NickServ::FindNick(nick);

		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		if (na->GetAccount()->HasFieldS("NS_SUSPENDED"))
		{
			source.Reply(_("\002{0}\002 is suspended."), na->GetNick());
			return;
		}

		bool ok = false;
		if (source.GetAccount() == na->GetAccount())
			ok = true;
		else if (!na->GetAccount()->HasFieldS("NS_SECURE") && source.GetUser() && na->GetAccount()->IsOnAccess(source.GetUser()))
			ok = true;

		if (certservice && source.GetUser() && certservice->Matches(source.GetUser(), na->GetAccount()))
			ok = true;

		if (ok == false && !pass.empty())
		{
			NickServ::IdentifyRequest *req = NickServ::service->CreateIdentifyRequest(new NSRecoverRequestListener(source, this, na->GetNick(), pass), GetOwner(), na->GetNick(), pass);
			EventManager::Get()->Dispatch(&Event::CheckAuthentication::OnCheckAuthentication, source.GetUser(), req);
			req->Dispatch();
		}
		else
		{
			NSRecoverRequestListener req(source, this, na->GetNick(), pass);

			if (ok)
				req.OnSuccess(nullptr);
			else
				req.OnFail(nullptr);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Recovers your nickname from another user or from services."
		               "If services are currently holding your nickname, the hold will be released."
		               " If another user is holding your nickname and is identified they will be killed."
		               " If they are not identified they will be forced off of the nickname."));
		return true;
	}
};

class NSRecover : public Module
	, public EventHook<Event::UserNickChange>
	, public EventHook<Event::JoinChannel>
{
	CommandNSRecover commandnsrecover;
	ExtensibleItem<NSRecoverInfo> recover;
	ExtensibleItem<NSRecoverSvsnick> svsnick;

 public:
	NSRecover(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::UserNickChange>(this)
		, EventHook<Event::JoinChannel>(this)
		, commandnsrecover(this)
		, recover(this, "recover")
		, svsnick(this, "svsnick")
	{

		if (Config->GetModule("nickserv/main")->Get<bool>("nonicknameownership"))
			throw ModuleException(modname + " can not be used with options:nonicknameownership enabled");

	}

	void OnUserNickChange(User *u, const Anope::string &oldnick) override
	{
		if (Config->GetModule(this)->Get<bool>("restoreonrecover"))
		{
			NSRecoverInfo *ei = recover.Get(u);
			ServiceBot *NickServ = Config->GetClient("NickServ");

			if (ei != NULL && NickServ != NULL)
				for (NSRecoverInfo::iterator it = ei->begin(), it_end = ei->end(); it != it_end;)
				{
					Channel *c = Channel::Find(it->first);
					const Anope::string &cname = it->first;
					++it;

					/* User might already be on the channel */
					if (u->FindChannel(c))
						this->OnJoinChannel(u, c);
					else if (IRCD->CanSVSJoin)
						IRCD->SendSVSJoin(NickServ, u, cname, "");
				}
		}

		NSRecoverSvsnick *svs = svsnick.Get(u);
		if (svs)
		{
			if (svs->from)
			{
				// svsnick from to to
				IRCD->SendForceNickChange(svs->from, svs->to, Anope::CurTime);
			}

			svsnick.Unset(u);
		}
	}

	void OnJoinChannel(User *u, Channel *c) override
	{
		if (Config->GetModule(this)->Get<bool>("restoreonrecover"))
		{
			NSRecoverInfo *ei = recover.Get(u);

			if (ei != NULL)
			{
				NSRecoverInfo::iterator it = ei->find(c->name);
				if (it != ei->end())
				{
					for (size_t i = 0; i < it->second.Modes().length(); ++i)
						c->SetMode(c->ci->WhoSends(), ModeManager::FindChannelModeByChar(it->second.Modes()[i]), u->GetUID());

					ei->erase(it);
					if (ei->empty())
						recover.Unset(u);
				}
			}
		}
	}
};

MODULE_INIT(NSRecover)
