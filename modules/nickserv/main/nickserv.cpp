/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
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
#include "modules/nickserv/group.h"
#include "modules/nickserv/update.h"
#include "modules/help.h"
#include "modules/nickserv.h"
#include "identifyrequest.h"
#include "nicktype.h"
#include "accounttype.h"
#include "modetype.h"

class NickServCollide;
static std::set<NickServCollide *> collides;

/** Timer for colliding nicks to force people off of nicknames
 */
class NickServCollide : public Timer
{
	NickServ::NickServService *service;
	Reference<User> u;
	time_t ts;
	Reference<NickServ::Nick> na;

 public:
	NickServCollide(Module *me, NickServ::NickServService *nss, User *user, NickServ::Nick *nick, time_t delay) : Timer(me, delay), service(nss), u(user), ts(user->timestamp), na(nick)
	{
		collides.insert(this);
	}

	~NickServCollide()
	{
		collides.erase(this);
	}

	NickServ::Nick *GetNick()
	{
		return na;
	}

	User *GetUser()
	{
		return u;
	}

	void Tick(time_t t) override
	{
		if (!u || !na)
			return;
		/* If they identified or don't exist anymore, don't kill them. */
		if (u->Account() == na->GetAccount() || u->timestamp > ts)
			return;

		service->Collide(u, na);
	}
};

/** Timer for removing HELD status from nicks.
 */
class NickServHeld : public Timer
{
	Reference<NickServ::Nick> na;
	Anope::string nick;
 public:
	NickServHeld(Module *me, NickServ::Nick *n, long l) : Timer(me, l), na(n), nick(na->GetNick())
	{
		n->SetS<bool>("HELD", true);
	}

	void Tick(time_t) override
	{
		if (na)
			na->UnsetS<bool>("HELD");
	}
};

class NickServRelease;
static Anope::map<NickServRelease *> NickServReleases;

/** Timer for releasing nicks to be available for use
 */
class NickServRelease : public User, public Timer
{
	Anope::string nick;

 public:
	NickServRelease(Module *me, NickServ::Nick *na, time_t delay) : User(na->GetNick(), Config->GetModule("nickserv/main")->Get<Anope::string>("enforceruser", "user"),
		Config->GetModule("nickserv/main")->Get<Anope::string>("enforcerhost", "services.localhost.net"), "", "", Me, "Services Enforcer", Anope::CurTime, "", IRCD->UID_Retrieve(), NULL), Timer(me, delay), nick(na->GetNick())
	{
		/* Erase the current release timer and use the new one */
		Anope::map<NickServRelease *>::iterator nit = NickServReleases.find(this->nick);
		if (nit != NickServReleases.end())
		{
			IRCD->SendQuit(nit->second, "");
			delete nit->second;
		}

		NickServReleases.insert(std::make_pair(this->nick, this));

		IRCD->Send<messages::NickIntroduction>(this);
	}

	~NickServRelease()
	{
		IRCD->SendQuit(this, "");
		NickServReleases.erase(this->nick);
	}

	void Tick(time_t t) override { }
};

class NickServCore : public Module, public NickServ::NickServService
	, public EventHook<Event::Shutdown>
	, public EventHook<Event::Restart>
	, public EventHook<Event::UserLogin>
	, public EventHook<Event::DelNick>
	, public EventHook<Event::DelCore>
	, public EventHook<Event::ChangeCoreDisplay>
	, public EventHook<Event::NickIdentify>
	, public EventHook<Event::NickGroup>
	, public EventHook<Event::NickUpdate>
	, public EventHook<Event::UserConnect>
	, public EventHook<Event::PostUserLogoff>
	, public EventHook<Event::ServerSync>
	, public EventHook<Event::UserNickChange>
	, public EventHook<Event::UserModeSet>
	, public EventHook<Event::Help>
	, public EventHook<Event::ExpireTick>
	, public EventHook<Event::NickInfo>
	, public EventHook<Event::ModuleUnload>
	, public EventHook<NickServ::Event::NickRegister>
	, public EventHook<Event::UserQuit>
{
	Reference<ServiceBot> NickServ;
	std::vector<Anope::string> defaults;
	ExtensibleItem<bool> held, collided;
	std::set<NickServ::IdentifyRequest *> identifyrequests;
	NickServ::nickalias_map NickList;
	NickServ::nickcore_map AccountList;
	NickType nick_type;
	AccountType account_type;
	NSModeType mode_type;

	void OnCancel(User *u, NickServ::Nick *na)
	{
		if (collided.HasExt(na))
		{
			collided.Unset(na);

			new NickServHeld(this, na, Config->GetModule("nickserv/main")->Get<time_t>("releasetimeout", "1m"));

			if (IRCD->CanSVSHold)
				IRCD->Send<messages::SVSHold>(na->GetNick(), Config->GetModule("nickserv/main")->Get<time_t>("releasetimeout", "1m"));
			else
				new NickServRelease(this, na, Config->GetModule("nickserv/main")->Get<time_t>("releasetimeout", "1m"));
		}
	}

 public:
	NickServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR)
		, NickServ::NickServService(this)

		, EventHook<Event::Shutdown>(this)
		, EventHook<Event::Restart>(this)
		, EventHook<Event::UserLogin>(this)
		, EventHook<Event::DelNick>(this)
		, EventHook<Event::DelCore>(this)
		, EventHook<Event::ChangeCoreDisplay>(this)
		, EventHook<Event::NickIdentify>(this)
		, EventHook<Event::NickGroup>(this)
		, EventHook<Event::NickUpdate>(this)
		, EventHook<Event::UserConnect>(this)
		, EventHook<Event::PostUserLogoff>(this)
		, EventHook<Event::ServerSync>(this)
		, EventHook<Event::UserNickChange>(this)
		, EventHook<Event::UserModeSet>(this)
		, EventHook<Event::Help>(this)
		, EventHook<Event::ExpireTick>(this)
		, EventHook<Event::NickInfo>(this)
		, EventHook<Event::ModuleUnload>(this)
		, EventHook<NickServ::Event::NickRegister>(this)
		, EventHook<Event::UserQuit>(this)

		, held(this, "HELD")
		, collided(this, "COLLIDED")
		, nick_type(this)
		, account_type(this)
		, mode_type(this)
	{
		NickServ::service = this;
	}

	~NickServCore()
	{
		OnShutdown();
		NickServ::service = nullptr;
	}

	void OnShutdown() override
	{
		/* On shutdown, restart, or mod unload, remove all of our holds for nicks (svshold or qlines)
		 * because some IRCds do not allow us to have these automatically expire
		 */
		for (NickServ::Nick *na : Serialize::GetObjects<NickServ::Nick *>())
			this->Release(na);
	}

	void OnRestart() override
	{
		OnShutdown();
	}

	void Validate(User *u) override
	{
		NickServ::Nick *na = NickServ::FindNick(u->nick);
		if (!na)
			return;

		EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&NickServ::Event::NickValidate::OnNickValidate, u, na);
		if (MOD_RESULT == EVENT_STOP)
		{
			this->Collide(u, na);
			return;
		}

		if (MOD_RESULT == EVENT_ALLOW)
			return;

		if (!na->GetAccount()->IsSecure() && u->IsRecognized())
		{
			na->SetLastSeen(Anope::CurTime);
			na->SetLastUsermask(u->GetIdent() + "@" + u->GetDisplayedHost());
			na->SetLastRealname(u->realname);
			return;
		}

		if (Config->GetModule("nickserv/main")->Get<bool>("nonicknameownership"))
			return;

		bool on_access = u->IsRecognized(false);

		if (on_access || !na->GetAccount()->IsKillImmed())
		{
			if (na->GetAccount()->IsSecure())
				u->SendMessage(*NickServ, _("This nickname is registered and protected. If this is your nickname, type \002{0}{1} IDENTIFY \037password\037\002. Otherwise, please choose a different nickname."), Config->StrictPrivmsg, NickServ->nick); // XXX
			else
				u->SendMessage(*NickServ, _("This nickname is owned by someone else. If this is your nickname, type \002{0}{1} IDENTIFY \037password\037\002. Otherwise, please choose a different nickname."), Config->StrictPrivmsg, NickServ->nick); // XXX
		}
		if (na->GetAccount()->IsKillProtect() && !on_access)
		{
			if (na->GetAccount()->IsKillImmed())
			{
				u->SendMessage(*NickServ, _("This nickname has been registered; you may not use it."));
				this->Collide(u, na);
			}
			else if (na->GetAccount()->IsKillQuick())
			{
				time_t killquick = Config->GetModule("nickserv/main")->Get<time_t>("killquick", "20s");
				u->SendMessage(*NickServ, _("If you do not change within %s, I will change your nick."), Anope::Duration(killquick, u->Account()).c_str());
				new NickServCollide(this, this, u, na, killquick);
			}
			else
			{
				time_t kill = Config->GetModule("nickserv/main")->Get<time_t>("kill", "60s");
				u->SendMessage(*NickServ, _("If you do not change within %s, I will change your nick."), Anope::Duration(kill, u->Account()).c_str());
				new NickServCollide(this, this, u, na, kill);
			}
		}

	}

	void OnUserLogin(User *u) override
	{
		NickServ::Nick *na = NickServ::FindNick(u->nick);
		if (na && na->GetAccount() == u->Account() && !Config->GetModule("nickserv/main")->Get<bool>("nonicknameownership") && !na->GetAccount()->IsUnconfirmed())
			u->SetMode(NickServ, "REGISTERED");

		const Anope::string &modesonid = Config->GetModule(this)->Get<Anope::string>("modesonid");
		if (!modesonid.empty())
			u->SetModes(NickServ, "%s", modesonid.c_str());
	}

	void Collide(User *u, NickServ::Nick *na) override
	{
		if (na)
			collided.Set(na, true);

		if (IRCD->CanSVSNick)
		{
			unsigned nicklen = Config->GetBlock("networkinfo")->Get<unsigned>("nicklen");
			const Anope::string &guestprefix = Config->GetModule("nickserv/main")->Get<Anope::string>("guestnickprefix", "Guest");

			Anope::string guestnick;

			int i = 0;
			do
			{
				guestnick = guestprefix + stringify(static_cast<uint16_t>(rand()));
				if (guestnick.length() > nicklen)
					guestnick = guestnick.substr(0, nicklen);
			}
			while (User::Find(guestnick, true) && i++ < 10);

			if (i == 11)
				u->Kill(*NickServ, "Services nickname-enforcer kill");
			else
			{
				u->SendMessage(*NickServ, _("Your nickname is now being changed to \002%s\002"), guestnick.c_str());
				IRCD->Send<messages::SVSNick>(u, guestnick, Anope::CurTime);
			}
		}
		else
		{
			u->Kill(*NickServ, "Services nickname-enforcer kill");
		}
	}

	void Release(NickServ::Nick *na) override
	{
		if (held.HasExt(na))
		{
			if (IRCD->CanSVSHold)
				IRCD->Send<messages::SVSHoldDel>(na->GetNick());
			else
			{
				User *u = User::Find(na->GetNick(), true);
				if (u && u->server == Me)
				{
					u->Quit();
				}
			}

			held.Unset(na);
		}
		collided.Unset(na); /* clear pending collide */
	}

	NickServ::IdentifyRequest *CreateIdentifyRequest(NickServ::IdentifyRequestListener *l, Module *o, const Anope::string &acc, const Anope::string &pass) override
	{
		return new IdentifyRequestImpl(l, o, acc, pass);
	}

	std::set<NickServ::IdentifyRequest *>& GetIdentifyRequests() override
	{
		return identifyrequests;
	}

	std::vector<NickServ::Nick *> GetNickList() override
	{
		return Serialize::GetObjects<NickServ::Nick *>();
	}

	NickServ::nickalias_map& GetNickMap() override
	{
		return NickList;
	}

	std::vector<NickServ::Account *> GetAccountList() override
	{
		return Serialize::GetObjects<NickServ::Account *>();
	}

	NickServ::nickcore_map& GetAccountMap() override
	{
		return AccountList;
	}

	NickServ::Nick *FindNick(const Anope::string &nick) override
	{
		return nick_type.FindNick(nick);
	}

	NickServ::Account *FindAccount(const Anope::string &acc) override
	{
		return account_type.FindAccount(acc);
	}

	void OnReload(Configuration::Conf *conf) override
	{
		const Anope::string &nsnick = conf->GetModule(this)->Get<Anope::string>("client");

		if (nsnick.empty())
			throw ConfigException(Module::name + ": <client> must be defined");

		ServiceBot *bi = ServiceBot::Find(nsnick, true);
		if (!bi)
			throw ConfigException(Module::name + ": no bot named " + nsnick);

		NickServ = bi;

		spacesepstream(conf->GetModule(this)->Get<Anope::string>("defaults", "secure memo_signon memo_receive")).GetTokens(defaults);
		if (defaults.empty())
			defaults = { "secure", "memo_signon", "memo_receive" };
		else if (defaults[0].equals_ci("none"))
			defaults.clear();
	}

	void OnDelNick(NickServ::Nick *na) override
	{
		User *u = User::Find(na->GetNick(), true);
		if (u && u->Account() == na->GetAccount())
		{
			IRCD->Send<messages::Logout>(u);
			u->RemoveMode(NickServ, "REGISTERED");
			u->Logout();
		}
	}

	void OnDelCore(NickServ::Account *nc) override
	{
		NickServ->logger.Category("nick").Log(_("Deleting nickname group {0}"), nc->GetDisplay());

		/* Clean up this nick core from any users online */
		for (unsigned int i = nc->users.size(); i > 0; --i)
		{
			User *user = nc->users[i - 1];
			IRCD->Send<messages::Logout>(user);
			user->RemoveMode(NickServ, "REGISTERED");
			user->Logout();
			EventManager::Get()->Dispatch(&Event::NickLogout::OnNickLogout, user);
		}
	}

	void OnChangeCoreDisplay(NickServ::Account *nc, const Anope::string &newdisplay) override
	{
		Anope::Logger.Bot(NickServ).Category("nick").Log(_("Changing {0} nickname group display to {1}"), nc->GetDisplay(), newdisplay);
	}

	void OnNickIdentify(User *u) override
	{
		Configuration::Block *block = Config->GetModule(this);

		if (block->Get<bool>("modeonid", "yes"))

			for (User::ChanUserList::iterator it = u->chans.begin(), it_end = u->chans.end(); it != it_end; ++it)
			{
				ChanUserContainer *cc = it->second;
				Channel *c = cc->chan;
				if (c)
					c->SetCorrectModes(u, true);
			}

		const Anope::string &modesonid = block->Get<Anope::string>("modesonid");
		if (!modesonid.empty())
			u->SetModes(NickServ, "%s", modesonid.c_str());

		if (block->Get<bool>("forceemail", "yes") && u->Account()->GetEmail().empty())
		{
			u->SendMessage(*NickServ, _("You must now supply an e-mail for your nick.\n"
							"This e-mail will allow you to retrieve your password in\n"
							"case you forget it."));
			u->SendMessage(*NickServ, _("Type \002%s%s SET EMAIL \037e-mail\037\002 in order to set your e-mail.\n"
							"Your privacy is respected; this e-mail won't be given to\n"
							"any third-party person."), Config->StrictPrivmsg.c_str(), NickServ->nick.c_str());
		}

		for (std::set<NickServCollide *>::iterator it = collides.begin(); it != collides.end(); ++it)
		{
			NickServCollide *c = *it;
			if (c->GetUser() == u && c->GetNick() && c->GetNick()->GetAccount() == u->Account())
			{
				delete c;
				break;
			}
		}
	}

	void OnNickGroup(User *u, NickServ::Nick *target) override
	{
		if (!target->GetAccount()->IsUnconfirmed())
			u->SetMode(NickServ, "REGISTERED");
	}

	void OnNickUpdate(User *u) override
	{
		for (User::ChanUserList::iterator it = u->chans.begin(), it_end = u->chans.end(); it != it_end; ++it)
		{
			ChanUserContainer *cc = it->second;
			Channel *c = cc->chan;
			if (c)
				c->SetCorrectModes(u, true);
		}
	}

	void OnUserConnect(User *u, bool &exempt) override
	{
		if (u->Quitting() || !u->server->IsSynced() || u->server->IsULined())
			return;

		const NickServ::Nick *na = NickServ::FindNick(u->nick);

		const Anope::string &unregistered_notice = Config->GetModule(this)->Get<Anope::string>("unregistered_notice");
		if (!Config->GetModule("nickserv/main")->Get<bool>("nonicknameownership") && !unregistered_notice.empty() && !na && !u->Account())
			u->SendMessage(*NickServ, unregistered_notice.replace_all_cs("%n", u->nick));
		else if (na && !u->IsIdentified(true))
			this->Validate(u);
	}

	void OnPostUserLogoff(User *u) override
	{
		NickServ::Nick *na = NickServ::FindNick(u->nick);
		if (na)
			OnCancel(u, na);
	}

	void OnServerSync(Server *s) override
	{
		for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
		{
			User *u = it->second;

			if (u->server == s)
			{
				if (u->HasMode("REGISTERED") && !u->IsIdentified(true))
					u->RemoveMode(NickServ, "REGISTERED");

				if (!u->IsIdentified())
					this->Validate(u);
			}
		}
	}

	void OnUserNickChange(User *u, const Anope::string &oldnick) override
	{
		NickServ::Nick *old_na = NickServ::FindNick(oldnick), *na = NickServ::FindNick(u->nick);
		/* If the new nick isn't registered or it's registered and not yours */
		if (!na || na->GetAccount() != u->Account())
		{
			/* Remove +r, but keep an account associated with the user */
			u->RemoveMode(NickServ, "REGISTERED");

			this->Validate(u);
		}
		else
		{
			/* Reset +r and re-send account (even though it really should be set at this point) */
			IRCD->Send<messages::Login>(u, na);
			if (!Config->GetModule("nickserv/main")->Get<bool>("nonicknameownership") && na->GetAccount() == u->Account() && !na->GetAccount()->IsUnconfirmed())
				u->SetMode(NickServ, "REGISTERED");
			u->logger.Bot(NickServ).Log(_("{0} automatically identified for account {1}"), u->GetMask(), u->Account()->GetDisplay());
		}

		if (!u->nick.equals_ci(oldnick) && old_na)
			OnCancel(u, old_na);
	}

	void OnUserModeSet(const MessageSource &setter, User *u, const Anope::string &mname) override
	{
		if (u->server->IsSynced() && mname == "REGISTERED" && !u->IsIdentified(true))
			u->RemoveMode(NickServ, mname);
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *NickServ)
			return EVENT_CONTINUE;
		if (!Config->GetModule("nickserv/main")->Get<bool>("nonicknameownership"))
			source.Reply(_("\002%s\002 allows you to register a nickname and\n"
				"prevent others from using it. The following\n"
				"commands allow for registration and maintenance of\n"
				"nicknames; to use them, type \002%s%s \037command\037\002.\n"
				"For more information on a specific command, type\n"
				"\002%s%s %s \037command\037\002.\n"), NickServ->nick.c_str(), Config->StrictPrivmsg.c_str(), NickServ->nick.c_str(), Config->StrictPrivmsg.c_str(), NickServ->nick.c_str(), source.command.c_str());
		else
			source.Reply(_("\002%s\002 allows you to register an account.\n"
				"The following commands allow for registration and maintenance of\n"
				"accounts; to use them, type \002%s%s \037command\037\002.\n"
				"For more information on a specific command, type\n"
				"\002%s%s %s \037command\037\002.\n"), NickServ->nick.c_str(), Config->StrictPrivmsg.c_str(), NickServ->nick.c_str(), Config->StrictPrivmsg.c_str(), NickServ->nick.c_str(), source.command.c_str());
		return EVENT_CONTINUE;
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *NickServ)
			return;
		if (source.IsServicesOper())
			source.Reply(_(" \n"
				"Services Operators can also drop any nickname without needing\n"
				"to identify for the nick, and may view the access list for\n"
				"any nickname."));
		time_t nickserv_expire = Config->GetModule(this)->Get<time_t>("expire", "21d");
		if (nickserv_expire >= 86400)
			source.Reply(_(" \n"
				"Accounts that are not used anymore are subject to\n"
				"the automatic expiration, i.e. they will be deleted\n"
				"after %d days if not used."), nickserv_expire / 86400);
		source.Reply(_(" \n"
			"\002NOTICE:\002 This service is intended to provide a way for\n"
			"IRC users to ensure their identity is not compromised.\n"
			"It is \002NOT\002 intended to facilitate \"stealing\" of\n"
			"nicknames or other malicious actions. Abuse of %s\n"
			"will result in, at minimum, loss of the abused\n"
			"nickname(s)."), NickServ->nick.c_str());
	}

	void OnNickRegister(User *, NickServ::Nick *na, const Anope::string &) override
	{
		/* Set default flags */
		for (unsigned i = 0; i < defaults.size(); ++i)
			na->GetAccount()->SetS<bool>(defaults[i], true);
	}

	void OnUserQuit(User *u, const Anope::string &msg) override
	{
		if (u->server && !u->server->GetQuitReason().empty() && Config->GetModule(this)->Get<bool>("hidenetsplitquit"))
			return;

		/* Update last quit and last seen for the user */
		NickServ::Nick *na = NickServ::FindNick(u->nick);
		if (na && !na->GetAccount()->HasFieldS("NS_SUSPENDED") && (u->IsRecognized() || u->IsIdentified(true)))
		{
			na->SetLastSeen(Anope::CurTime);
			na->SetLastQuit(msg);
		}
	}

	void OnExpireTick() override
	{
		if (Anope::NoExpire || Anope::ReadOnly)
			return;

		time_t nickserv_expire = Config->GetModule(this)->Get<time_t>("expire", "21d");

		for (NickServ::Nick *na : Serialize::GetObjects<NickServ::Nick *>())
		{
			User *u = User::Find(na->GetNick(), true);
			if (u && (u->IsIdentified(true) || u->IsRecognized()))
				na->SetLastSeen(Anope::CurTime);

			bool expire = false;

			if (nickserv_expire && Anope::CurTime - na->GetLastSeen() >= nickserv_expire)
				expire = true;

			EventManager::Get()->Dispatch(&NickServ::Event::PreNickExpire::OnPreNickExpire, na, expire);

			if (expire)
			{
				Anope::Logger.Bot(NickServ).Category("nickserv/expire").Log(_("Expiring nickname {0} (account: {1}) (e-mail: {2})"),
						na->GetNick(), na->GetAccount()->GetDisplay(), na->GetAccount()->GetEmail().empty() ? "none" : na->GetAccount()->GetEmail());
				EventManager::Get()->Dispatch(&NickServ::Event::NickExpire::OnNickExpire, na);
				na->Delete();
			}
		}
	}

	void OnNickInfo(CommandSource &source, NickServ::Nick *na, InfoFormatter &info, bool show_hidden) override
	{
		if (!na->GetAccount()->IsUnconfirmed())
		{
			time_t nickserv_expire = Config->GetModule(this)->Get<time_t>("expire", "21d");
			if (!na->IsNoExpire() && nickserv_expire && !Anope::NoExpire && (source.HasPriv("nickserv/auspex") || na->GetLastSeen() != Anope::CurTime))
				info[_("Expires")] = Anope::strftime(na->GetLastSeen() + nickserv_expire, source.GetAccount());
		}
		else
		{
			time_t unconfirmed_expire = Config->GetModule(this)->Get<time_t>("unconfirmedexpire", "1d");
			info[_("Expires")] = Anope::strftime(na->GetTimeRegistered() + unconfirmed_expire, source.GetAccount());
		}
	}

	void OnModuleUnload(User *u, Module *m) override
	{
		for (std::set<NickServ::IdentifyRequest *>::iterator it = identifyrequests.begin(), it_end = identifyrequests.end(); it != it_end;)
		{
			IdentifyRequestImpl *ir = anope_dynamic_static_cast<IdentifyRequestImpl *>(*it);
			++it;

			ir->Unload(m);
		}
	}
};

MODULE_INIT(NickServCore)

