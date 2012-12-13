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

struct NickSuspend : ExtensibleItem, Serializable
{
	Anope::string nick;
	time_t when;

	NickSuspend() : Serializable("NickSuspend")
	{
	}

	void Serialize(Serialize::Data &sd) const anope_override
	{
		sd["nick"] << this->nick;
		sd["when"] << this->when;
	}

	static Serializable* Unserialize(Serializable *obj, Serialize::Data &sd)
	{
		Anope::string snick;

		sd["nick"] >> snick;

		const NickAlias *na = NickAlias::Find(snick);
		if (na == NULL)
			return NULL;

		NickSuspend *ns;
		if (obj)
			ns = anope_dynamic_static_cast<NickSuspend *>(obj);
		else
			ns = new NickSuspend();
		
		sd["nick"] >> ns->nick;
		sd["when"] >> ns->when;

		if (!obj)
			na->nc->Extend("ns_suspend_expire", ns);

		return ns;
	}
};

class CommandNSSuspend : public Command
{
 public:
	CommandNSSuspend(Module *creator) : Command(creator, "nickserv/suspend", 2, 3)
	{
		this->SetDesc(_("Suspend a given nick"));
		this->SetSyntax(_("\037nickname\037 [+\037expiry\037] \037reason\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{

		const Anope::string &nick = params[0];
		Anope::string expiry = params[1];
		Anope::string reason = params.size() > 2 ? params[2] : "";
		time_t expiry_secs = Config->NSSuspendExpire;

		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		if (expiry[0] != '+')
		{
			reason = expiry + " " + reason;
			reason.trim();
			expiry.clear();
		}
		else
			expiry_secs = Anope::DoTime(expiry);

		NickAlias *na = NickAlias::Find(nick);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
		}

		if (Config->NSSecureAdmins && na->nc->IsServicesOper())
		{
			source.Reply(_("You may not suspend other services operators nicknames."));
			return;
		}

		NickCore *nc = na->nc;

		nc->SetFlag(NI_SUSPENDED);
		nc->SetFlag(NI_SECURE);
		nc->UnsetFlag(NI_KILLPROTECT);
		nc->UnsetFlag(NI_KILL_QUICK);
		nc->UnsetFlag(NI_KILL_IMMED);

		for (std::list<Serialize::Reference<NickAlias> >::iterator it = nc->aliases.begin(), it_end = nc->aliases.end(); it != it_end;)
		{
			NickAlias *na2 = *it++;

			if (na2 && *na2->nc == *na->nc)
			{
				na2->last_quit = reason;

				User *u2 = User::Find(na2->nick);
				if (u2)
				{
					u2->Logout();
					u2->Collide(na2);
				}
			}
		}

		if (expiry_secs > 0)
		{
			NickSuspend *ns = new NickSuspend();
			ns->nick = na->nick;
			ns->when = Anope::CurTime + expiry_secs;

			nc->Extend("ns_suspend_expire", ns);
		}

		Log(LOG_ADMIN, source, this) << "for " << nick << " (" << (!reason.empty() ? reason : "No reason") << "), expires in " << (expiry_secs ? Anope::strftime(Anope::CurTime + expiry_secs) : "never");
		source.Reply(_("Nick %s is now suspended."), nick.c_str());

		FOREACH_MOD(I_OnNickSuspended, OnNickSuspend(na));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Suspends a registered nickname, which prevents from being used\n"
				"while keeping all the data for that nick. If an expiry is given\n"
				"the nick will be unsuspended after that period of time, else the\n"
				"default expiry from the configuration is used."));
		return true;
	}
};

class CommandNSUnSuspend : public Command
{
 public:
	CommandNSUnSuspend(Module *creator) : Command(creator, "nickserv/unsuspend", 1, 1)
	{
		this->SetDesc(_("Unsuspend a given nick"));
		this->SetSyntax(_("\037nickname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &nick = params[0];

		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		NickAlias *na = NickAlias::Find(nick);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
		}

		if (!na->nc->HasFlag(NI_SUSPENDED))
		{
			source.Reply(_("Nick %s is not suspended."), na->nick.c_str());
			return;
		}

		na->nc->UnsetFlag(NI_SUSPENDED);
		na->nc->Shrink("ns_suspend_expire");

		Log(LOG_ADMIN, source, this) << "for " << na->nick;
		source.Reply(_("Nick %s is now released."), nick.c_str());

		FOREACH_MOD(I_OnNickUnsuspended, OnNickUnsuspended(na));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Unsuspends a nickname which allows it to be used again."));
		return true;
	}
};

class NSSuspend : public Module
{
	Serialize::Type nicksuspend_type;
	CommandNSSuspend commandnssuspend;
	CommandNSUnSuspend commandnsunsuspend;

 public:
	NSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		nicksuspend_type("NickSuspend", NickSuspend::Unserialize), commandnssuspend(this), commandnsunsuspend(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnPreNickExpire };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	~NSSuspend()
	{
		for (nickcore_map::const_iterator it = NickCoreList->begin(), it_end = NickCoreList->end(); it != it_end; ++it)
			it->second->Shrink("ns_suspend_expire");
	}

	void OnPreNickExpire(NickAlias *na, bool &expire) anope_override
	{
		if (!na->nc->HasFlag(NI_SUSPENDED))
			return;

		expire = false;

		NickSuspend *ns = na->nc->GetExt<NickSuspend *>("ns_suspend_expire");
		if (ns != NULL && ns->when < Anope::CurTime)
		{
			na->last_seen = Anope::CurTime;
			na->nc->UnsetFlag(NI_SUSPENDED);
			na->nc->Shrink("ns_suspend_expire");

			Log(LOG_NORMAL, "expire", NickServ) << "Expiring suspend for " << na->nick;
		}
	}
};

MODULE_INIT(NSSuspend)
