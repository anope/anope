/* NickServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandNSSuspend : public Command
{
 public:
	CommandNSSuspend(Module *creator) : Command(creator, "nickserv/suspend", 2, 2, "nickserv/suspend")
	{
		this->SetDesc(_("Suspend a given nick"));
		this->SetSyntax(_("\037nickname\037 \037reason\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &nick = params[0];
		const Anope::string &reason = params[1];

		if (readonly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		NickAlias *na = findnick(nick);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
		}

		if (Config->NSSecureAdmins && na->nc->IsServicesOper())
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		na->nc->SetFlag(NI_SUSPENDED);
		na->nc->SetFlag(NI_SECURE);
		na->nc->UnsetFlag(NI_KILLPROTECT);
		na->nc->UnsetFlag(NI_KILL_QUICK);
		na->nc->UnsetFlag(NI_KILL_IMMED);

		for (std::list<NickAlias *>::iterator it = na->nc->aliases.begin(), it_end = na->nc->aliases.end(); it != it_end; ++it)
		{
			NickAlias *na2 = *it;

			if (na2->nc == na->nc)
			{
				na2->last_quit = reason;

				User *u2 = finduser(na2->nick);
				if (u2)
				{
					u2->Logout();
					u2->Collide(na2);
				}
			}
		}

		Log(LOG_ADMIN, u, this) << "for " << nick << " (" << (!reason.empty() ? reason : "No reason") << ")";
		source.Reply(_("Nick %s is now suspended."), nick.c_str());

		FOREACH_MOD(I_OnNickSuspended, OnNickSuspend(na));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Suspends a registered nickname, which prevents from being used\n"
				"while keeping all the data for that nick."));
		return true;
	}
};

class CommandNSUnSuspend : public Command
{
 public:
	CommandNSUnSuspend(Module *creator) : Command(creator, "nickserv/unsuspend", 1, 1, "nickserv/suspend")
	{
		this->SetDesc(_("Unsuspend a given nick"));
		this->SetSyntax(_("\037nickname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &nick = params[0];

		if (readonly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		NickAlias *na = findnick(nick);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
		}

		if (Config->NSSecureAdmins && na->nc->IsServicesOper())
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		na->nc->UnsetFlag(NI_SUSPENDED);

		Log(LOG_ADMIN, u, this) << "for " << na->nick;
		source.Reply(_("Nick %s is now released."), nick.c_str());

		FOREACH_MOD(I_OnNickUnsuspended, OnNickUnsuspended(na));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Unsuspends a nickname which allows it to be used again."));
		return true;
	}
};

class NSSuspend : public Module
{
	CommandNSSuspend commandnssuspend;
	CommandNSUnSuspend commandnsunsuspend;

 public:
	NSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssuspend(this), commandnsunsuspend(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandnssuspend);
		ModuleManager::RegisterService(&commandnsunsuspend);
	}
};

MODULE_INIT(NSSuspend)
