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

static BotInfo *NickServ = NULL;

class MyNickServService : public NickServService
{
 public:
	MyNickServService(Module *m) : NickServService(m) { }

	BotInfo *Bot()
	{
		return NickServ;
	}
	
	void Validate(User *u)
	{
		NickAlias *na = findnick(u->nick);
		if (!na)
			return;

		if (na->nc->HasFlag(NI_SUSPENDED))
		{
			u->SendMessage(NickServ, _(NICK_X_SUSPENDED), u->nick.c_str());
			u->Collide(na);
			return;
		}

		if (!u->IsIdentified() && !u->fingerprint.empty() && na->nc->FindCert(u->fingerprint))
		{
			u->SendMessage(NickServ, _("SSL Fingerprint accepted, you are now identified"));
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
				u->SendMessage(NickServ, _(NICK_IS_SECURE), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str());
			else
				u->SendMessage(NickServ, _(NICK_IS_REGISTERED), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str());
		}

		if (na->nc->HasFlag(NI_KILLPROTECT) && !u->IsRecognized())
		{
			if (na->nc->HasFlag(NI_KILL_IMMED))
			{
				u->SendMessage(NickServ, _(FORCENICKCHANGE_NOW));
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

		Implementation i[] = { I_OnDelNick, I_OnDelCore, I_OnChangeCoreDisplay, I_OnNickIdentify, I_OnNickGroup, I_OnNickUpdate };
		ModuleManager::Attach(i, this, 5);

		ModuleManager::RegisterService(&this->mynickserv);
		
		NickServ = new BotInfo(Config->s_NickServ, Config->ServiceUser, Config->ServiceHost, Config->desc_NickServ);
		NickServ->SetFlag(BI_CORE);

		spacesepstream coreModules(Config->NickCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
			ModuleManager::LoadModule(module, NULL);
	}

	~NickServCore()
	{
		spacesepstream coreModules(Config->NickCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
		{
			Module *m = ModuleManager::FindModule(module);
			if (m != NULL)
				ModuleManager::UnloadModule(m, NULL);
		}

		delete NickServ;
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
							"any third-party person."), Config->UseStrictPrivMsgString.c_str(), Config->s_NickServ.c_str());
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
};

MODULE_INIT(NickServCore)

