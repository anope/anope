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
#include "nickserv.h"

class NickServCollide;
class NickServRelease;

typedef std::map<Anope::string, NickServCollide *> nickservcollides_map;
typedef std::map<Anope::string, NickServRelease *> nickservreleases_map;

static nickservcollides_map NickServCollides;
static nickservreleases_map NickServReleases;

/** Timer for colliding nicks to force people off of nicknames
 */
class NickServCollide : public Timer
{
	dynamic_reference<User> u;
	Anope::string nick;

 public:
	/** Default constructor
	 * @param nick The nick we're colliding
	 * @param delay How long to delay before kicking the user off the nick
	 */
	NickServCollide(User *user, time_t delay) : Timer(delay), u(user), nick(u->nick)
	{
		/* Erase the current collide and use the new one */
		nickservcollides_map::iterator nit = NickServCollides.find(user->nick);
		if (nit != NickServCollides.end())
			delete nit->second;

		NickServCollides.insert(std::make_pair(nick, this));
	}

	/** Default destructor
	 */
	virtual ~NickServCollide()
	{
		NickServCollides.erase(this->nick);
	}

	/** Called when the delay is up
	 * @param t The current time
	 */
	void Tick(time_t t) anope_override
	{
		if (!u)
			return;
		/* If they identified or don't exist anymore, don't kill them. */
		NickAlias *na = findnick(u->nick);
		if (!na || u->Account() == na->nc || u->my_signon > this->GetSetTime())
			return;

		u->Collide(na);
	}
};

class MyNickServService : public NickServService
{
 public:
	MyNickServService(Module *m) : NickServService(m) { }

	void Validate(User *u) anope_override
	{
		NickAlias *na = findnick(u->nick);
		if (!na)
			return;
		const BotInfo *NickServ = findbot(Config->NickServ);

		if (na->nc->HasFlag(NI_SUSPENDED))
		{
			u->SendMessage(NickServ, NICK_X_SUSPENDED, u->nick.c_str());
			u->Collide(na);
			return;
		}

		if (!(u->Account() == na->nc) && !u->fingerprint.empty() && na->nc->FindCert(u->fingerprint))
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

		if (Config->NoNicknameOwnership)
			return;

		if (u->IsRecognized(false) || !na->nc->HasFlag(NI_KILL_IMMED))
		{
			if (na->nc->HasFlag(NI_SECURE))
				u->SendMessage(NickServ, NICK_IS_SECURE, Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str());
			else
				u->SendMessage(NickServ, NICK_IS_REGISTERED, Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str());
		}

		if (na->nc->HasFlag(NI_KILLPROTECT) && !u->IsRecognized(false))
		{
			if (na->nc->HasFlag(NI_KILL_IMMED))
			{
				u->SendMessage(NickServ, FORCENICKCHANGE_NOW);
				u->Collide(na);
			}
			else if (na->nc->HasFlag(NI_KILL_QUICK))
			{
				u->SendMessage(NickServ, _("If you do not change within %s, I will change your nick."), duration(Config->NSKillQuick, u->Account()).c_str());
				new NickServCollide(u, Config->NSKillQuick);
			}
			else
			{
				u->SendMessage(NickServ, _("If you do not change within %s, I will change your nick."), duration(Config->NSKill, u->Account()).c_str());
				new NickServCollide(u, Config->NSKill);
			}
		}

	}
};

class ExpireCallback : public CallBack
{
 public:
	ExpireCallback(Module *owner) : CallBack(owner, Config->ExpireTimeout, Anope::CurTime, true) { }

	void Tick(time_t) anope_override
	{
		if (noexpire || readonly)
			return;

		for (nickalias_map::const_iterator it = NickAliasList->begin(), it_end = NickAliasList->end(); it != it_end; )
		{
			NickAlias *na = it->second;
			++it;

			User *u = finduser(na->nick);
			if (u && (na->nc->HasFlag(NI_SECURE) ? u->IsIdentified(true) : u->IsRecognized()))
				na->last_seen = Anope::CurTime;

			bool expire = false;

			if (na->nc->HasFlag(NI_UNCONFIRMED))
				if (Config->NSUnconfirmedExpire && Anope::CurTime - na->time_registered >= Config->NSUnconfirmedExpire)
					expire = true;
			if (Config->NSExpire && Anope::CurTime - na->last_seen >= Config->NSExpire)
				expire = true;
			if (na->HasFlag(NS_NO_EXPIRE))
				expire = false;

			FOREACH_MOD(I_OnPreNickExpire, OnPreNickExpire(na, expire));
		
			if (expire)
			{
				Anope::string extra;
				if (na->nc->HasFlag(NI_SUSPENDED))
					extra = "suspended ";
				Log(LOG_NORMAL, "expire") << "Expiring " << extra << "nickname " << na->nick << " (group: " << na->nc->display << ") (e-mail: " << (na->nc->email.empty() ? "none" : na->nc->email) << ")";
				FOREACH_MOD(I_OnNickExpire, OnNickExpire(na));
				na->destroy();
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

		if (!findbot(Config->NickServ))
			throw ModuleException("No bot named " + Config->NickServ);

		Implementation i[] = { I_OnDelNick, I_OnDelCore, I_OnChangeCoreDisplay, I_OnNickIdentify, I_OnNickGroup,
		I_OnNickUpdate, I_OnUserNickChange, I_OnPreHelp, I_OnPostHelp, I_OnUserConnect };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
	}

	void OnDelNick(NickAlias *na) anope_override
	{
		User *u = finduser(na->nick);
		if (u && u->Account() == na->nc)
		{
			ircdproto->SendLogout(u);
			u->RemoveMode(findbot(Config->NickServ), UMODE_REGISTERED);
			u->Logout();
		}
	}

	void OnDelCore(NickCore *nc) anope_override
	{
		Log(findbot(Config->NickServ), "nick") << "deleting nickname group " << nc->display;

		/* Clean up this nick core from any users online using it
		 * (ones that /nick but remain unidentified)
		 */
		for (std::list<User *>::iterator it = nc->Users.begin(); it != nc->Users.end();)
		{
			User *user = *it++;
			ircdproto->SendLogout(user);
			user->RemoveMode(findbot(Config->NickServ), UMODE_REGISTERED);
			user->Logout();
			FOREACH_MOD(I_OnNickLogout, OnNickLogout(user));
		}
		nc->Users.clear();
	}

	void OnChangeCoreDisplay(NickCore *nc, const Anope::string &newdisplay) anope_override
	{
		Log(LOG_NORMAL, "nick", findbot(Config->NickServ)) << "Changing " << nc->display << " nickname group display to " << newdisplay;
	}

	void OnNickIdentify(User *u) anope_override
	{
		const BotInfo *NickServ = findbot(Config->NickServ);

		if (!Config->NoNicknameOwnership)
		{
			const NickAlias *this_na = findnick(u->nick);
			if (this_na && this_na->nc == u->Account() && u->Account()->HasFlag(NI_UNCONFIRMED) == false)
				u->SetMode(NickServ, UMODE_REGISTERED);
		}

		if (Config->NSModeOnID)
			for (UChannelList::iterator it = u->chans.begin(), it_end = u->chans.end(); it != it_end; ++it)
			{
				ChannelContainer *cc = *it;
				Channel *c = cc->chan;
				if (c)
					chan_set_correct_modes(u, c, 1, true);
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
			const NickAlias *this_na = findnick(u->Account()->display);
			time_t time_registered = Anope::CurTime - this_na->time_registered;
			if (Config->NSUnconfirmedExpire > time_registered)
				u->SendMessage(NickServ, _("Your account will expire, if not confirmed, in %s"), duration(Config->NSUnconfirmedExpire - time_registered).c_str());
		}
	}

	void OnNickGroup(User *u, NickAlias *target) anope_override
	{
		if (target->nc->HasFlag(NI_UNCONFIRMED) == false)
			u->SetMode(findbot(Config->NickServ), UMODE_REGISTERED);
	}

	void OnNickUpdate(User *u) anope_override
	{
		for (UChannelList::iterator it = u->chans.begin(), it_end = u->chans.end(); it != it_end; ++it)
		{
			ChannelContainer *cc = *it;
			Channel *c = cc->chan;
			if (c)
				chan_set_correct_modes(u, c, 1, false);
		}
	}

	void OnUserNickChange(User *u, const Anope::string &oldnick) anope_override
	{
		const NickAlias *na = findnick(u->nick);
		const BotInfo *NickServ = findbot(Config->NickServ);
		/* If the new nick isnt registerd or its registerd and not yours */
		if (!na || na->nc != u->Account())
		{
			ircdproto->SendLogout(u);
			u->RemoveMode(NickServ, UMODE_REGISTERED);

			this->mynickserv.Validate(u);
		}
		else
		{
			ircdproto->SendLogin(u);
			if (!Config->NoNicknameOwnership && na->nc == u->Account() && na->nc->HasFlag(NI_UNCONFIRMED) == false)
				u->SetMode(NickServ, UMODE_REGISTERED);
			Log(NickServ) << u->GetMask() << " automatically identified for group " << u->Account()->display;
		}
	}

	void OnUserModeSet(User *u, UserModeName Name) anope_override
	{
		if (Name == UMODE_REGISTERED && !u->IsIdentified())
			u->RemoveMode(findbot(Config->NickServ), Name);
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!params.empty() || source.owner->nick != Config->NickServ)
			return EVENT_CONTINUE;
		source.Reply(_("\002%s\002 allows you to \"register\" a nickname and\n"
			"prevent others from using it. The following\n"
			"commands allow for registration and maintenance of\n"
			"nicknames; to use them, type \002%s%s \037command\037\002.\n"
			"For more information on a specific command, type\n"
			"\002%s%s %s \037command\037\002.\n "), Config->NickServ.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->NickServ.c_str(), source.command.c_str());
		return EVENT_CONTINUE;
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!params.empty() || source.owner->nick != Config->NickServ)
			return;
		if (source.IsServicesOper())
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

	void OnUserConnect(dynamic_reference<User> &u, bool &exempt) anope_override
	{
		if (!Config->NoNicknameOwnership && !Config->NSUnregisteredNotice.empty() && u && !findnick(u->nick))
			u->SendMessage(findbot(Config->NickServ), Config->NSUnregisteredNotice);
	}
};

MODULE_INIT(NickServCore)

