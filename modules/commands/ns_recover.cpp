/* NickServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

struct NSRecoverExtensibleInfo : ExtensibleItem, std::map<Anope::string, ChannelStatus> { };

class NSRecoverRequest : public IdentifyRequest
{
	CommandSource source;
	Command *cmd;
	Anope::string user;
 
 public:
	NSRecoverRequest(Module *o, CommandSource &src, Command *c, const Anope::string &nick, const Anope::string &pass) : IdentifyRequest(o, nick, pass), source(src), cmd(c), user(nick) { }

	void OnSuccess() anope_override
	{
		User *u = User::Find(user, true);
		if (!source.GetUser() || !source.service)
			return;

		NickAlias *na = NickAlias::Find(user);
		if (!na)
			return;

		Log(LOG_COMMAND, source, cmd) << "for " << na->nick;

		/* Nick is being held by us, release it */
		if (na->HasExt("HELD"))
		{
			na->Release();
			source.Reply(_("Service's hold on \002%s\002 has been released."), na->nick.c_str());
		}
		else if (!u)
		{
			source.Reply(_("No one is using your nick, and services are not holding it."));
		}
		// If the user being recovered is identified for the account of the nick then the user is the
		// same person that is executing the command, so kill them off (old GHOST command).
		else if (u->Account() == na->nc)
		{
			if (!source.GetAccount() && na->nc->HasExt("SECURE"))
			{
				source.GetUser()->Login(u->Account());
				Log(LOG_COMMAND, source, cmd) << "and was automatically identified to " << u->Account()->display;
			}

			if (Config->GetModule("ns_recover")->Get<bool>("restoreonrecover"))
			{
				if (!u->chans.empty())
				{
					NSRecoverExtensibleInfo *ei = new NSRecoverExtensibleInfo;
					for (User::ChanUserList::iterator it = u->chans.begin(), it_end = u->chans.end(); it != it_end; ++it)
						(*ei)[it->first->name] = it->second->status;

					source.GetUser()->Extend("ns_recover_info", ei);
				}
			}

			u->SendMessage(NickServ, _("This nickname has been recovered by %s. If you did not do\n"
							"this then %s may have your password, and you should change it.\n"),
							source.GetNick().c_str(), source.GetNick().c_str());

			Anope::string buf = source.command.upper() + " command used by " + source.GetNick();
			u->Kill(source.service->nick, buf);

			source.Reply(_("Ghost with your nick has been killed."));

			if (IRCD->CanSVSNick)
				IRCD->SendForceNickChange(source.GetUser(), GetAccount(), Anope::CurTime);
		}
		/* User is not identified or not identified to the same account as the person using this command */
		else
		{
			if (!source.GetAccount() && na->nc->HasExt("SECURE"))
			{
				source.GetUser()->Login(na->nc); // Identify the user using the command if they arent identified
				Log(LOG_COMMAND, source, cmd) << "and was automatically identified to " << na->nick << " (" << na->nc->display << ")";
			}

			u->SendMessage(NickServ, _("This nickname has been recovered by %s."), source.GetNick().c_str());
			u->Collide(na);

			if (IRCD->CanSVSNick)
			{
				/* If we can svsnick then release our hold and svsnick the user using the command */
				na->Release();
				IRCD->SendForceNickChange(source.GetUser(), GetAccount(), Anope::CurTime);
			}
			else
				source.Reply(_("The user with your nick has been removed. Use this command again\n"
						"to release services's hold on your nick."));
		}
	}

	void OnFail() anope_override
	{
		if (NickAlias::Find(GetAccount()) != NULL)
		{
			source.Reply(ACCESS_DENIED);
			if (!GetPassword().empty())
			{
				Log(LOG_COMMAND, source, cmd) << "with an invalid password for " << GetAccount();
				if (source.GetUser())
					source.GetUser()->BadPassword();
			}
		}
		else
			source.Reply(NICK_X_NOT_REGISTERED, GetAccount().c_str());
	}
};

class CommandNSRecover : public Command
{
 public:
	CommandNSRecover(Module *creator) : Command(creator, "nickserv/recover", 1, 2)
	{
		this->SetDesc(_("Regains control of your nick"));
		this->SetSyntax("\037nickname\037 [\037password\037]");
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &nick = params[0];
		const Anope::string &pass = params.size() > 1 ? params[1] : "";

		User *user = User::Find(nick, true);

		if (user && source.GetUser() == user)
		{
			source.Reply(_("You can't %s yourself!"), source.command.lower().c_str());
			return;
		}

		const NickAlias *na = NickAlias::Find(nick);

		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
		}
		else if (na->nc->HasExt("SUSPENDED"))
		{
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
			return;
		}

		bool ok = false;
		if (source.GetAccount() == na->nc)
			ok = true;
		else if (!na->nc->HasExt("SECURE") && source.GetUser() && na->nc->IsOnAccess(source.GetUser()))
			ok = true;
		else if (source.GetUser() && !source.GetUser()->fingerprint.empty() && na->nc->FindCert(source.GetUser()->fingerprint))
			ok = true;

		if (ok == false && !pass.empty())
		{
			NSRecoverRequest *req = new NSRecoverRequest(owner, source, this, na->nick, pass);
			FOREACH_MOD(I_OnCheckAuthentication, OnCheckAuthentication(source.GetUser(), req));
			req->Dispatch();
		}
		else
		{
			NSRecoverRequest req(owner, source, this, na->nick, pass);

			if (ok)
				req.OnSuccess();
			else
				req.OnFail();
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Recovers your nick from another user or from services.\n"
				"If services are currently holding your nick, the hold\n"
				"will be released. If another user is holding your nick\n"
				"and is identified they will be killed (similar to the old\n"
				"GHOST command). If they are not identified they will be\n"
				"forced off of the nick."));
		return true;
	}
};

class NSRecover : public Module
{
	CommandNSRecover commandnsrecover;

 public:
	NSRecover(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsrecover(this)
	{

		if (Config->GetBlock("options")->Get<bool>("nonicknameownership"))
			throw ModuleException(modname + " can not be used with options:nonicknameownership enabled");

		Implementation i[] = { I_OnUserNickChange, I_OnJoinChannel, I_OnShutdown, I_OnRestart };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	~NSRecover()
	{
		for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			it->second->Shrink("ns_recover_info");

		OnShutdown();
	}

	void OnShutdown() anope_override
	{
		/* On shutdown, restart, or mod unload, remove all of our holds for nicks (svshold or qlines)
		 * because some IRCds do not allow us to have these automatically expire
		 */
		for (nickalias_map::const_iterator it = NickAliasList->begin(); it != NickAliasList->end(); ++it)
			it->second->Release();
	}

	void OnRestart() anope_override { OnShutdown(); }

	void OnUserNickChange(User *u, const Anope::string &oldnick) anope_override
	{
		if (Config->GetModule(this)->Get<bool>("restoreonrecover"))
		{
			NSRecoverExtensibleInfo *ei = u->GetExt<NSRecoverExtensibleInfo *>("ns_recover_info");

			if (ei != NULL)
				for (std::map<Anope::string, ChannelStatus>::iterator it = ei->begin(), it_end = ei->end(); it != it_end;)
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
	}

	void OnJoinChannel(User *u, Channel *c) anope_override
	{
		if (Config->GetModule(this)->Get<bool>("restoreonrecover"))
		{
			NSRecoverExtensibleInfo *ei = u->GetExt<NSRecoverExtensibleInfo *>("ns_recover_info");

			if (ei != NULL)
			{
				std::map<Anope::string, ChannelStatus>::iterator it = ei->find(c->name);
				if (it != ei->end())
				{
					for (size_t i = 0; i < it->second.Modes().length(); ++i)
						c->SetMode(c->ci->WhoSends(), ModeManager::FindChannelModeByChar(it->second.Modes()[i]), u->GetUID());

					ei->erase(it);
					if (ei->empty())
						u->Shrink("ns_recover_info");
				}
			}
		}
	}
};

MODULE_INIT(NSRecover)
