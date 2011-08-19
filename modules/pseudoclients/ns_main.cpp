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
#include "nickserv.h"

static BotInfo *NickServ;

class MyNickServService : public NickServService
{
 public:
	MyNickServService(Module *m) : NickServService(m) { }

	void Validate(User *u)
	{
		NickAlias *na = findnick(u->nick);
		if (!na)
			return;

		if (na->nc->HasFlag(NI_SUSPENDED))
		{
			u->SendMessage(NickServ, NICK_X_SUSPENDED, u->nick.c_str());
			u->Collide(na);
			return;
		}

		if (!u->IsIdentified() && !u->fingerprint.empty() && na->nc->FindCert(u->fingerprint))
		{
			u->SendMessage(NickServ, _("SSL Fingerprint accepted, you are now identified."));
			Log(u) << "automatically identified for account " << na->nc->display << " using a valid SSL fingerprint.";
			u->Identify(na);
			return;
		}

		if (!na->nc->HasFlag(NI_SECURE) && u->IsRecognized())
		{
			na->last_seen = Anope::CurTime;
			Anope::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
			na->last_usermask = last_usermask;
			na->last_realname = u->realname;
			return;
		}

		if (u->IsRecognized() || !na->nc->HasFlag(NI_KILL_IMMED))
		{
			if (na->nc->HasFlag(NI_SECURE))
				u->SendMessage(NickServ, NICK_IS_SECURE, Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str());
			else
				u->SendMessage(NickServ, NICK_IS_REGISTERED, Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str());
		}

		if (na->nc->HasFlag(NI_KILLPROTECT) && !u->IsRecognized())
		{
			if (na->nc->HasFlag(NI_KILL_IMMED))
			{
				u->SendMessage(NickServ, FORCENICKCHANGE_NOW);
				u->Collide(na);
			}
			else if (na->nc->HasFlag(NI_KILL_QUICK))
			{
				u->SendMessage(NickServ, _("If you do not change within 20 seconds, I will change your nick."));
				new NickServCollide(u, 20);
			}
			else
			{
				u->SendMessage(NickServ, _("If you do not change within one minute, I will change your nick."));
				new NickServCollide(u, 60);
			}
		}

	}
};

class ExpireCallback : public CallBack
{
 public:
	ExpireCallback(Module *owner) : CallBack(owner, Config->ExpireTimeout, Anope::CurTime, true) { }

	void Tick(time_t)
	{
		if (noexpire || readonly)
			return;

		for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; )
		{
			NickAlias *na = it->second;
			++it;

			User *u = finduser(na->nick);
			if (u && (na->nc->HasFlag(NI_SECURE) ? u->IsIdentified(true) : u->IsRecognized(true)))
				na->last_seen = Anope::CurTime;

			bool expire = false;

			if (na->nc->HasFlag(NI_UNCONFIRMED))
				if (Config->NSUnconfirmedExpire && Anope::CurTime - na->time_registered >= Config->NSUnconfirmedExpire)
					expire = true;
			if (na->nc->HasFlag(NI_SUSPENDED))
			{
				if (Config->NSSuspendExpire && Anope::CurTime - na->last_seen >= Config->NSSuspendExpire)
					expire = true;
			}
			else if (Config->NSExpire && Anope::CurTime - na->last_seen >= Config->NSExpire)
				expire = true;
			if (na->HasFlag(NS_NO_EXPIRE))
				expire = false;
		
			if (expire)
			{
				EventReturn MOD_RESULT;
				FOREACH_RESULT(I_OnPreNickExpire, OnPreNickExpire(na));
				if (MOD_RESULT == EVENT_STOP)
					continue;
				Anope::string extra;
				if (na->nc->HasFlag(NI_SUSPENDED))
					extra = "suspended ";
				Log(LOG_NORMAL, "expire") << "Expiring " << extra << "nickname " << na->nick << " (group: " << na->nc->display << ") (e-mail: " << (na->nc->email.empty() ? "none" : na->nc->email) << ")";
				FOREACH_MOD(I_OnNickExpire, OnNickExpire(na));
				delete na;
			}
		}
	}
};

class NickServCore : public Module
{
	MyNickServService mynickserv;
	ExpireCallback expires;

 public:
	NickServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), mynickserv(this), expires(this)
	{
		this->SetAuthor("Anope");

		NickServ = findbot(Config->NickServ);
		if (NickServ == NULL)
			throw ModuleException("No bot named " + Config->NickServ);

		Implementation i[] = { I_OnDelNick, I_OnDelCore, I_OnChangeCoreDisplay, I_OnNickIdentify, I_OnNickGroup,
		I_OnNickUpdate, I_OnUserNickChange, I_OnPreHelp, I_OnPostHelp };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		ModuleManager::RegisterService(&this->mynickserv);
	}

	void OnDelNick(NickAlias *na)
	{
		User *u = finduser(na->nick);
		if (u && u->Account() == na->nc)
		{
			u->RemoveMode(NickServ, UMODE_REGISTERED);
			ircdproto->SendAccountLogout(u, u->Account());
			ircdproto->SendUnregisteredNick(u);
			u->Logout();
		}
	}

	void OnDelCore(NickCore *nc)
	{
		Log(NickServ, "nick") << "deleting nickname group " << nc->display;

		/* Clean up this nick core from any users online using it
		 * (ones that /nick but remain unidentified)
		 */
		for (std::list<User *>::iterator it = nc->Users.begin(); it != nc->Users.end();)
		{
			User *user = *it++;
			ircdproto->SendAccountLogout(user, user->Account());
			user->RemoveMode(NickServ, UMODE_REGISTERED);
			ircdproto->SendUnregisteredNick(user);
			user->Logout();
			FOREACH_MOD(I_OnNickLogout, OnNickLogout(user));
		}
		nc->Users.clear();
	}

	void OnChangeCoreDisplay(NickCore *nc, const Anope::string &newdisplay)
	{
		Log(LOG_NORMAL, "nick", NickServ) << "Changing " << nc->display << " nickname group display to " << newdisplay;
	}

	void OnNickIdentify(User *u)
	{
		NickAlias *this_na = findnick(u->nick);
		if (this_na && this_na->nc == u->Account() && u->Account()->HasFlag(NI_UNCONFIRMED) == false)
			u->SetMode(NickServ, UMODE_REGISTERED);

		if (Config->NSModeOnID)
			for (UChannelList::iterator it = u->chans.begin(), it_end = u->chans.end(); it != it_end; ++it)
			{
				ChannelContainer *cc = *it;
				Channel *c = cc->chan;
				if (c)
					chan_set_correct_modes(u, c, 1);
			}

		if (Config->NSForceEmail && u->Account()->email.empty())
		{
			u->SendMessage(NickServ, _("You must now supply an e-mail for your nick.\n"
							"This e-mail will allow you to retrieve your password in\n"
							"case you forget it."));
			u->SendMessage(NickServ, _("Type \002%s%s SET EMAIL \037e-mail\037\002 in order to set your e-mail.\n"
							"Your privacy is respected; this e-mail won't be given to\n"
							"any third-party person."), Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str());
		}

		if (u->Account()->HasFlag(NI_UNCONFIRMED))
		{
			u->SendMessage(NickServ, _("Your email address is not confirmed. To confirm it, follow the instructions that were emailed to you when you registered."));
			this_na = findnick(u->Account()->display);
			time_t time_registered = Anope::CurTime - this_na->time_registered;
			if (Config->NSUnconfirmedExpire > time_registered)
				u->SendMessage(NickServ, _("Your account will expire, if not confirmed, in %s"), duration(Config->NSUnconfirmedExpire - time_registered).c_str());
		}
	}

	void OnNickGroup(User *u, NickAlias *target)
	{
		if (target->nc->HasFlag(NI_UNCONFIRMED) == false)
			u->SetMode(NickServ, UMODE_REGISTERED);
	}

	void OnNickUpdate(User *u)
	{
		for (UChannelList::iterator it = u->chans.begin(), it_end = u->chans.end(); it != it_end; ++it)
		{
			ChannelContainer *cc = *it;
			Channel *c = cc->chan;
			if (c)
				chan_set_correct_modes(u, c, 1);
		}
	}

	void OnUserNickChange(User *u, const Anope::string &oldnick)
	{
		NickAlias *na = findnick(u->nick);
		/* If the new nick isnt registerd or its registerd and not yours */
		if (!na || na->nc != u->Account())
		{
			u->RemoveMode(NickServ, UMODE_REGISTERED);
			ircdproto->SendUnregisteredNick(u);

			this->mynickserv.Validate(u);
		}
		else
		{
			if (na->nc->HasFlag(NI_UNCONFIRMED) == false)
			{
				u->SetMode(NickServ, UMODE_REGISTERED);
				ircdproto->SetAutoIdentificationToken(u);
			}
			Log(NickServ) << u->GetMask() << " automatically identified for group " << u->Account()->display;
		}
	}

	void OnUserModeSet(User *u, UserModeName Name)
	{
		if (Name == UMODE_REGISTERED && !u->IsIdentified())
			u->RemoveMode(NickServ, Name);
	}

	void OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!params.empty() || source.owner->nick != Config->NickServ)
			return;
		source.Reply(_("\002%s\002 allows you to \"register\" a nickname and\n"
			"prevent others from using it. The following\n"
			"commands allow for registration and maintenance of\n"
			"nicknames; to use them, type \002%s%s \037command\037\002.\n"
			"For more information on a specific command, type\n"
			"\002%s%s %s \037command\037\002.\n "), Config->NickServ.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str(), source.command.c_str());
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params)
	{
		if (!params.empty() || source.owner->nick != Config->NickServ)
			return;
		if (source.u->IsServicesOper())
			source.Reply(_(" \n"
				"Services Operators can also drop any nickname without needing\n"
				"to identify for the nick, and may view the access list for\n"
				"any nickname."));
		if (Config->NSExpire >= 86400)
			source.Reply(_("Nicknames that are not used anymore are subject to \n"
				"the automatic expiration, i.e. they will be deleted\n"
				"after %d days if not used."), Config->NSExpire / 86400);
		source.Reply(_(" \n"
			"\002NOTICE:\002 This service is intended to provide a way for\n"
			"IRC users to ensure their identity is not compromised.\n"
			"It is \002NOT\002 intended to facilitate \"stealing\" of\n"
			"nicknames or other malicious actions. Abuse of %s\n"
			"will result in, at minimum, loss of the abused\n"
			"nickname(s)."), Config->NickServ.c_str());
	}
};

MODULE_INIT(NickServCore)

