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
			source.Reply(_("You may not suspend other Services Operators' nicknames."));
			return;
		}

		NickCore *nc = na->nc;

		nc->ExtendMetadata("SUSPENDED");
		nc->ExtendMetadata("SECURE");
		nc->Shrink("KILLPROTECT");
		nc->Shrink("KILL_QUICK");
		nc->Shrink("KILL_IMMED");

		nc->ExtendMetadata("suspend:by", source.GetNick());
		if (!reason.empty())
			nc->ExtendMetadata("suspend:reason", reason);
		if (expiry_secs > 0)
			nc->ExtendMetadata("suspend:expire", stringify(Anope::CurTime + expiry_secs));


		for (unsigned i = 0; i < nc->aliases->size(); ++i)
		{
			NickAlias *na2 = nc->aliases->at(i);

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

		Log(LOG_ADMIN, source, this) << "for " << nick << " (" << (!reason.empty() ? reason : "No reason") << "), expires in " << (expiry_secs ? Anope::strftime(Anope::CurTime + expiry_secs) : "never");
		source.Reply(_("Nick %s is now suspended."), nick.c_str());

		FOREACH_MOD(I_OnNickSuspended, OnNickSuspend(na));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Suspends a registered nickname, which prevents it from being used\n"
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

		if (!na->nc->HasExt("SUSPENDED"))
		{
			source.Reply(_("Nick %s is not suspended."), na->nick.c_str());
			return;
		}

		na->nc->Shrink("SUSPENDED");
		na->nc->Shrink("suspend:expire");
		na->nc->Shrink("suspend:by");
		na->nc->Shrink("suspend:reason");

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
	CommandNSSuspend commandnssuspend;
	CommandNSUnSuspend commandnsunsuspend;

 public:
	NSSuspend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnssuspend(this), commandnsunsuspend(this)
	{

		Implementation i[] = { I_OnPreNickExpire };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnPreNickExpire(NickAlias *na, bool &expire) anope_override
	{
		if (!na->nc->HasExt("SUSPENDED"))
			return;

		expire = false;

		Anope::string *str = na->nc->GetExt<ExtensibleItemClass<Anope::string> *>("suspend:expire");
		if (str == NULL)
			return;

		try
		{
			time_t when = convertTo<time_t>(*str);
			if (when < Anope::CurTime)
			{
				na->last_seen = Anope::CurTime;
				na->nc->Shrink("SUSPENDED");
				na->nc->Shrink("suspend:expire");
				na->nc->Shrink("suspend:by");
				na->nc->Shrink("suspend:reason");

				Log(LOG_NORMAL, "expire", NickServ) << "Expiring suspend for " << na->nick;
			}
		}
		catch (const ConvertException &) { }
	}
};

MODULE_INIT(NSSuspend)
