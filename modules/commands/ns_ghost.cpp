/* NickServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

struct NSGhostExtensibleInfo : ExtensibleItem, std::map<Anope::string, ChannelStatus> { };

class NSGhostRequest : public IdentifyRequest
{
	CommandSource source;
	Command *cmd;
	Reference<User> u;
 
 public:
	NSGhostRequest(Module *o, CommandSource &src, Command *c, User *user, const Anope::string &pass) : IdentifyRequest(o, user->nick, pass), source(src), cmd(c), u(user) { }

	void OnSuccess() anope_override
	{
		if (!source.GetUser() || !source.service || !u)
			return;

		if (Config->NSRestoreOnGhost)
		{
			if (u->Account() != NULL)
				source.GetUser()->Login(u->Account());

			if (!u->chans.empty())
			{
				NSGhostExtensibleInfo *ei = new NSGhostExtensibleInfo;
				for (UChannelList::iterator it = u->chans.begin(), it_end = u->chans.end(); it != it_end; ++it)
					(*ei)[(*it)->chan->name] = *(*it)->status;

				source.GetUser()->Extend("ns_ghost_info", ei);
			}
		}

		Log(LOG_COMMAND, source, cmd) << "for " << GetAccount();
		Anope::string buf = "GHOST command used by " + source.GetNick();
		u->Kill(source.service->nick, buf);
		source.Reply(_("Ghost with your nick has been killed."));

		if (Config->NSRestoreOnGhost)
			IRCD->SendForceNickChange(source.GetUser(), GetAccount(), Anope::CurTime);
	}

	void OnFail() anope_override
	{
		if (!u)
			;
		else if (!NickAlias::Find(GetAccount()))
			source.Reply(NICK_X_NOT_REGISTERED, GetAccount().c_str());
		else
		{
			source.Reply(ACCESS_DENIED);
			if (!GetPassword().empty())
			{
				Log(LOG_COMMAND, source, cmd) << "with an invalid password for " << GetAccount();
				if (source.GetUser())
					source.GetUser()->BadPassword();
			}
		}
	}
};

class CommandNSGhost : public Command
{
 public:
	CommandNSGhost(Module *creator) : Command(creator, "nickserv/ghost", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Disconnects a \"ghost\" IRC session using your nick"));
		this->SetSyntax("\037nickname\037 [\037password\037]");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &nick = params[0];
		const Anope::string &pass = params.size() > 1 ? params[1] : "";

		User *user = User::Find(nick, true);
		const NickAlias *na = NickAlias::Find(nick);

		if (!user)
			source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
		else if (user->server == Me)
			source.Reply(_("\2%s\2 is a services enforcer."), user->nick.c_str());
		else if (!user->IsIdentified())
			source.Reply(_("You may not ghost an unidentified user, use RECOVER instead."));
		else if (!na)
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->nc->HasFlag(NI_SUSPENDED))
			source.Reply(NICK_X_SUSPENDED, na->nick.c_str());
		else if (nick.equals_ci(source.GetNick()))
			source.Reply(_("You can't ghost yourself!"));
		else
		{
			bool ok = false;
			if (source.GetAccount() == na->nc)
				ok = true;
			else if (!na->nc->HasFlag(NI_SECURE) && source.GetUser() && na->nc->IsOnAccess(source.GetUser()))
				ok = true;
			else if (source.GetUser() && !source.GetUser()->fingerprint.empty() && na->nc->FindCert(source.GetUser()->fingerprint))
				ok = true;

			if (ok == false && !pass.empty())
			{
				NSGhostRequest *req = new NSGhostRequest(owner, source, this, user, pass);
				FOREACH_MOD(I_OnCheckAuthentication, OnCheckAuthentication(source.GetUser(), req));
				req->Dispatch();
			}
			else
			{
				NSGhostRequest req(owner, source, this, user, pass);

				if (ok)
					req.OnSuccess();
				else
					req.OnFail();
			}
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Terminates a \"ghost\" IRC session using your nick. A\n"
				"ghost\" session is one which is not actually connected,\n"
				"but which the IRC server believes is still online for one\n"
				"reason or another. Typically, this happens if your\n"
				"computer crashes or your Internet or modem connection\n"
				"goes down while you're on IRC.\n"
				" \n"
				"In order to use the \002GHOST\002 command for a nick, your\n"
				"current address as shown in /WHOIS must be on that nick's\n"
				"access list, you must be identified and in the group of\n"
				"that nick, or you must supply the correct password for\n"
				"the nickname."));
		return true;
	}
};

class NSGhost : public Module
{
	CommandNSGhost commandnsghost;

 public:
	NSGhost(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsghost(this)
	{
		this->SetAuthor("Anope");

		if (Config->NoNicknameOwnership)
			throw ModuleException(modname + " can not be used with options:nonicknameownership enabled");

		Implementation i[] = { I_OnUserNickChange, I_OnJoinChannel };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	~NSGhost()
	{
		for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			it->second->Shrink("ns_ghost_info");
	}

	void OnUserNickChange(User *u, const Anope::string &oldnick) anope_override
	{
		if (Config->NSRestoreOnGhost)
		{
			NSGhostExtensibleInfo *ei = u->GetExt<NSGhostExtensibleInfo *>("ns_ghost_info");

			if (ei != NULL)
				for (std::map<Anope::string, ChannelStatus>::iterator it = ei->begin(), it_end = ei->end(); it != it_end; ++it)
					IRCD->SendSVSJoin(NickServ, u->GetUID(), it->first, "");
		}
	}

	void OnJoinChannel(User *u, Channel *c) anope_override
	{
		if (Config->NSRestoreOnGhost)
		{
			NSGhostExtensibleInfo *ei = u->GetExt<NSGhostExtensibleInfo *>("ns_ghost_info");

			if (ei != NULL)
			{
				std::map<Anope::string, ChannelStatus>::iterator it = ei->find(c->name);
				if (it != ei->end())
				{
					for (size_t j = CMODE_BEGIN + 1; j < CMODE_END; ++j)
						if (it->second.HasFlag(static_cast<ChannelModeName>(j)))
							c->SetMode(c->ci->WhoSends(), ModeManager::FindChannelModeByName(static_cast<ChannelModeName>(j)), u->GetUID());

					ei->erase(it);
					if (ei->empty())
						u->Shrink("ns_ghost_info");
				}
			}
		}
	}
};

MODULE_INIT(NSGhost)
