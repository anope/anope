/* Routines to maintain a list of online users.
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */


#include "services.h"
#include "modules.h"
#include "users.h"
#include "account.h"
#include "protocol.h"
#include "servers.h"
#include "channels.h"
#include "bots.h"
#include "config.h"
#include "opertype.h"
#include "language.h"

user_map UserListByNick, UserListByUID;

int OperCount = 0;
unsigned MaxUserCount = 0;
time_t MaxUserTime = 0;

std::list<User *> User::quitting_users;

User::User(const Anope::string &snick, const Anope::string &sident, const Anope::string &shost, const Anope::string &svhost, const Anope::string &sip, Server *sserver, const Anope::string &srealname, time_t ssignon, const Anope::string &smodes, const Anope::string &suid)
{
	if (snick.empty() || sident.empty() || shost.empty())
		throw CoreException("Bad args passed to User::User");

	/* we used to do this by calloc, no more. */
	quit = false;
	server = NULL;
	invalid_pw_count = invalid_pw_time = lastmemosend = lastnickreg = lastmail = 0;
	on_access = false;

	this->nick = snick;
	this->ident = sident;
	this->host = shost;
	this->vhost = svhost;
	this->chost = svhost;
	this->ip = sip;
	this->server = sserver;
	this->realname = srealname;
	this->timestamp = this->signon = ssignon;
	this->SetModesInternal("%s", smodes.c_str());
	this->uid = suid;
	this->super_admin = false;

	size_t old = UserListByNick.size();
	UserListByNick[snick] = this;
	if (!suid.empty())
		UserListByUID[suid] = this;
	if (old == UserListByNick.size())
		Log(LOG_DEBUG) << "Duplicate user " << snick << " in user table?";

	this->nc = NULL;
	this->UpdateHost();

	if (sserver && sserver->IsSynced()) // Our bots are introduced on startup with no server
	{
		++sserver->users;
		Log(this, "connect") << (!svhost.empty() ? Anope::string("(") + svhost + ") " : "") << "(" << srealname << ") " << sip << " connected to the network (" << sserver->GetName() << ")";
	}

	if (UserListByNick.size() > MaxUserCount)
	{
		MaxUserCount = UserListByNick.size();
		MaxUserTime = Anope::CurTime;
		if (sserver && sserver->IsSynced())
			Log(this, "maxusers") << "connected - new maximum user count: " << UserListByNick.size();
	}

	bool exempt = false;
	if (server && server->IsULined())
		exempt = true;
	FOREACH_MOD(I_OnUserConnect, OnUserConnect(this, exempt));
}

void User::ChangeNick(const Anope::string &newnick, time_t ts)
{
	/* Sanity check to make sure we don't segfault */
	if (newnick.empty())
		throw CoreException("User::ChangeNick() got a bad argument");
	
	this->super_admin = false;
	Log(this, "nick") << "(" << this->realname << ") changed nick to " << newnick;

	Anope::string old = this->nick;
	this->timestamp = ts;

	if (this->nick.equals_ci(newnick))
		this->nick = newnick;
	else
	{
		NickAlias *old_na = NickAlias::Find(this->nick);
		if (old_na && (this->IsIdentified(true) || this->IsRecognized()))
			old_na->last_seen = Anope::CurTime;
		
		UserListByNick.erase(this->nick);
		this->nick = newnick;
		UserListByNick[this->nick] = this;

		on_access = false;
		NickAlias *na = NickAlias::Find(this->nick);
		if (na)
			on_access = na->nc->IsOnAccess(this);

		if (na && na->nc == this->Account())
		{
			na->last_seen = Anope::CurTime;
			this->UpdateHost();
		}
	}

	FOREACH_MOD(I_OnUserNickChange, OnUserNickChange(this, old));
}

void User::SetDisplayedHost(const Anope::string &shost)
{
	if (shost.empty())
		throw CoreException("empty host? in MY services? it seems it's more likely than I thought.");

	this->vhost = shost;

	Log(this, "host") << "changed vhost to " << shost;

	this->UpdateHost();
}

const Anope::string &User::GetDisplayedHost() const
{
	if (!this->vhost.empty())
		return this->vhost;
	else if (this->HasMode("CLOAK") && !this->GetCloakedHost().empty())
		return this->GetCloakedHost();
	else
		return this->host;
}

void User::SetCloakedHost(const Anope::string &newhost)
{
	if (newhost.empty())
		throw "empty host in User::SetCloakedHost";

	chost = newhost;

	Log(this, "host") << "changed cloaked host to " << newhost;

	this->UpdateHost();
}

const Anope::string &User::GetCloakedHost() const
{
	return chost;
}

const Anope::string &User::GetUID() const
{
	if (!this->uid.empty() && IRCD->RequiresID)
		return this->uid;
	else
		return this->nick;
}

void User::SetVIdent(const Anope::string &sident)
{
	this->vident = sident;

	Log(this, "ident") << "changed vident to " << sident;

	this->UpdateHost();
}

const Anope::string &User::GetVIdent() const
{
	if (!this->vident.empty())
		return this->vident;
	else
		return this->ident;
}

void User::SetIdent(const Anope::string &sident)
{
	this->ident = sident;

	Log(this, "ident") << "changed real ident to " << sident;

	this->UpdateHost();
}

const Anope::string &User::GetIdent() const
{
	return this->ident;
}

Anope::string User::GetMask() const
{
	return this->nick + "!" + this->ident + "@" + this->host;
}

Anope::string User::GetDisplayedMask() const
{
	return this->nick + "!" + this->GetVIdent() + "@" + this->GetDisplayedHost();
}

void User::SetRealname(const Anope::string &srealname)
{
	if (srealname.empty())
		throw CoreException("realname empty in SetRealname");

	this->realname = srealname;
	NickAlias *na = NickAlias::Find(this->nick);

	if (na && (this->IsIdentified(true) || this->IsRecognized()))
		na->last_realname = srealname;

	Log(this, "realname") << "changed realname to " << srealname;
}

User::~User()
{
	if (this->server != NULL)
	{
		if (this->server->IsSynced())
			Log(this, "disconnect") << "(" << this->realname << ") disconnected from the network (" << this->server->GetName() << ")";
		--this->server->users;
	}

	FOREACH_MOD(I_OnPreUserLogoff, OnPreUserLogoff(this));

	ModeManager::StackerDel(this);
	this->Logout();

	if (this->HasMode("OPER"))
		--OperCount;

	while (!this->chans.empty())
		this->chans.begin()->second->chan->DeleteUser(this);

	UserListByNick.erase(this->nick);
	if (!this->uid.empty())
		UserListByUID.erase(this->uid);

	FOREACH_MOD(I_OnPostUserLogoff, OnPostUserLogoff(this));
}

void User::SendMessage(const BotInfo *source, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";

	const char *translated_message = Language::Translate(this, fmt);

	va_start(args, fmt);
	vsnprintf(buf, BUFSIZE - 1, translated_message, args);

	this->SendMessage(source, Anope::string(buf));

	va_end(args);
}

void User::SendMessage(const BotInfo *source, const Anope::string &msg)
{
	const char *translated_message = Language::Translate(this, msg.c_str());

	/* Send privmsg instead of notice if:
	* - UsePrivmsg is enabled
	* - The user is not registered and NSDefMsg is enabled
	* - The user is registered and has set /ns set msg on
	*/
	sepstream sep(translated_message, '\n', true);
	for (Anope::string tok; sep.GetToken(tok);)
	{
		if (Config->UsePrivmsg && ((!this->nc && Config->DefPrivmsg) || (this->nc && this->nc->HasExt("MSG"))))
			IRCD->SendPrivmsg(source, this->GetUID(), "%s", tok.c_str());
		else
			IRCD->SendNotice(source, this->GetUID(), "%s", tok.c_str());
	}
}

/** Collides a nick.
 *
 * First, it marks the nick as COLLIDED, this is checked in NickAlias::OnCancel.
 *
 * Then it does one of two things.
 *
 * 1. This will force change the users nick to the guest nick. This gets processed by the IRCd and comes
 *    back as a nick change, which calls NickAlias::OnCancel
 *    with the users old nick's nickalias (if there is one).
 *
 * 2. Calls User::Kill, which kills the user and deletes the user at the end of the I/O loop.
 *     Users destructor then calls NickAlias::OnCancel
 *
 * NickAlias::OnCancel checks for NS_COLLIDED, it then does one of two things.
 *
 * 1. If supported, we send a SVSHold for the user. We are done here, the IRCds expires this at the time we give it.
 *
 * 2. We create a new client with SendClientIntroduction(). Note that is it important that this is called either after the
 *    user has been removed from our internal list of user or after the users nick has been updated completely internally.
 *    We then create a release timer for this new client that waits and later on sends a QUIT for the client. Release timers
 *    are never used for SVSHolds. Ever.
 *
 *
 *  Note that now for the timers we only store the users name, not the NickAlias* pointer. We never remove timers when
 *  a user changes nick or a nick is deleted, the timers must assume that either of these may have happend.
 *
 *  Adam
 */
void User::Collide(NickAlias *na)
{
	if (na)
		na->Extend("COLLIDED");

	if (IRCD->CanSVSNick)
	{
		const Anope::string &guestprefix = Config->GetBlock("options")->Get<const Anope::string &>("guestnickprefix");

		Anope::string guestnick;

		int i = 0;
		do
		{
			guestnick = guestprefix + stringify(static_cast<uint16_t>(rand()));
		} while (User::Find(guestnick) && i++ < 10);

		if (i == 11)
			this->Kill(NickServ ? NickServ->nick : "", "Services nickname-enforcer kill");
		else
		{
			if (NickServ)
				this->SendMessage(NickServ, _("Your nickname is now being changed to \002%s\002"), guestnick.c_str());
			IRCD->SendForceNickChange(this, guestnick, Anope::CurTime);
		}
	}
	else
		this->Kill(NickServ ? NickServ->nick : "", "Services nickname-enforcer kill");
}

void User::Identify(NickAlias *na)
{
	if (!na)
	{
		Log() << "User::Identify() called with NULL pointer";
		return;
	}

	if (this->nick.equals_ci(na->nick))
	{
		Anope::string last_usermask = this->GetIdent() + "@" + this->GetDisplayedHost();
		Anope::string last_realhost = this->GetIdent() + "@" + this->host;
		na->last_usermask = last_usermask;
		na->last_realhost = last_realhost;
		na->last_realname = this->realname;
		na->last_seen = Anope::CurTime;
	}

	this->Login(na->nc);
	IRCD->SendLogin(this);

	const NickAlias *this_na = NickAlias::Find(this->nick);
	if (!Config->GetBlock("options")->Get<bool>("nonicknameownership") && this_na && this_na->nc == *na->nc && na->nc->HasExt("UNCONFIRMED") == false)
		this->SetMode(NickServ, "REGISTERED");

	FOREACH_MOD(I_OnNickIdentify, OnNickIdentify(this));

	if (this->IsServicesOper())
	{
		if (!this->nc->o->ot->modes.empty())
		{
			this->SetModes(OperServ, "%s", this->nc->o->ot->modes.c_str());
			if (OperServ)
				this->SendMessage(OperServ, "Changing your usermodes to \002%s\002", this->nc->o->ot->modes.c_str());
			UserMode *um = ModeManager::FindUserModeByName("OPER");
			if (um && !this->HasMode("OPER") && this->nc->o->ot->modes.find(um->mchar) != Anope::string::npos)
				IRCD->SendOper(this);
		}
		if (IRCD->CanSetVHost && !this->nc->o->vhost.empty())
		{
			if (OperServ)
				this->SendMessage(OperServ, "Changing your vhost to \002%s\002", this->nc->o->vhost.c_str());
 			this->SetDisplayedHost(this->nc->o->vhost);
			IRCD->SendVhost(this, "", this->nc->o->vhost);
		}
	}
}


void User::Login(NickCore *core)
{
	if (core == this->nc)
		return;

	this->Logout();
	this->nc = core;
	core->users.push_back(this);

	this->UpdateHost();

	if (this->server->IsSynced())
		Log(this, "account") << "is now identified as " << this->nc->display;
}

void User::Logout()
{
	if (!this->nc)
		return;
	
	Log(this, "account") << "is no longer identified as " << this->nc->display;

	std::list<User *>::iterator it = std::find(this->nc->users.begin(), this->nc->users.end(), this);
	if (it != this->nc->users.end())
		this->nc->users.erase(it);

	this->nc = NULL;
}

NickCore *User::Account() const
{
	return this->nc;
}

bool User::IsIdentified(bool check_nick) const
{
	if (check_nick && this->nc)
	{
		NickAlias *na = NickAlias::Find(this->nc->display);
		return na && *na->nc == *this->nc;
	}

	return this->nc ? true : false;
}

bool User::IsRecognized(bool check_secure) const
{
	if (check_secure && on_access)
	{
		const NickAlias *na = NickAlias::Find(this->nick);

		if (!na || na->nc->HasExt("SECURE"))
			return false;
	}

	return on_access;
}

bool User::IsServicesOper()
{
	if (!this->nc || !this->nc->IsServicesOper())
		// No opertype.
		return false;
	else if (this->nc->o->require_oper && !this->HasMode("OPER"))
		return false;
	else if (!this->nc->o->certfp.empty() && this->fingerprint != this->nc->o->certfp)
		// Certfp mismatch
		return false;
	else if (!this->nc->o->hosts.empty())
	{
		bool match = false;
		Anope::string match_host = this->GetIdent() + "@" + this->host;
		for (unsigned i = 0; i < this->nc->o->hosts.size(); ++i)
			if (Anope::Match(match_host, this->nc->o->hosts[i]))
				match = true;
		if (match == false)
			return false;
	}

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_IsServicesOper, IsServicesOper(this));
	if (MOD_RESULT == EVENT_STOP)
		return false;

	return true;
}

bool User::HasCommand(const Anope::string &command)
{
	if (this->IsServicesOper())
		return this->nc->o->ot->HasCommand(command);
	return false;
}

bool User::HasPriv(const Anope::string &priv)
{
	if (this->IsServicesOper())
		return this->nc->o->ot->HasPriv(priv);
	return false;
}

void User::UpdateHost()
{
	if (this->host.empty())
		return;

	NickAlias *na = NickAlias::Find(this->nick);
	on_access = false;
	if (na)
		on_access = na->nc->IsOnAccess(this);

	if (na && (this->IsIdentified(true) || this->IsRecognized()))
	{
		Anope::string last_usermask = this->GetIdent() + "@" + this->GetDisplayedHost();
		Anope::string last_realhost = this->GetIdent() + "@" + this->host;
		na->last_usermask = last_usermask;
		na->last_realhost = last_realhost;
	}
}

bool User::HasMode(const Anope::string &mname) const
{
	return this->modes.count(mname);
}

void User::SetModeInternal(UserMode *um, const Anope::string &param)
{
	if (!um)
		return;

	this->modes[um->name] = param;

	FOREACH_MOD(I_OnUserModeSet, OnUserModeSet(this, um->name));
}

void User::RemoveModeInternal(UserMode *um)
{
	if (!um)
		return;

	this->modes.erase(um->name);

	FOREACH_MOD(I_OnUserModeUnset, OnUserModeUnset(this, um->name));
}

void User::SetMode(const BotInfo *bi, UserMode *um, const Anope::string &Param)
{
	if (!um || HasMode(um->name))
		return;

	ModeManager::StackerAdd(bi, this, um, true, Param);
	SetModeInternal(um, Param);
}

void User::SetMode(const BotInfo *bi, const Anope::string &uname, const Anope::string &Param)
{
	SetMode(bi, ModeManager::FindUserModeByName(uname), Param);
}

void User::RemoveMode(const BotInfo *bi, UserMode *um)
{
	if (!um || !HasMode(um->name))
		return;

	ModeManager::StackerAdd(bi, this, um, false);
	RemoveModeInternal(um);
}

void User::RemoveMode(const BotInfo *bi, const Anope::string &name)
{
	RemoveMode(bi, ModeManager::FindUserModeByName(name));
}

void User::SetModes(const BotInfo *bi, const char *umodes, ...)
{
	char buf[BUFSIZE] = "";
	va_list args;
	Anope::string modebuf, sbuf;
	int add = -1;
	va_start(args, umodes);
	vsnprintf(buf, BUFSIZE - 1, umodes, args);
	va_end(args);

	spacesepstream sep(buf);
	sep.GetToken(modebuf);
	for (unsigned i = 0, end = modebuf.length(); i < end; ++i)
	{
		UserMode *um;

		switch (modebuf[i])
		{
			case '+':
				add = 1;
				continue;
			case '-':
				add = 0;
				continue;
			default:
				if (add == -1)
					continue;
				um = ModeManager::FindUserModeByChar(modebuf[i]);
				if (!um)
					continue;
		}

		if (add)
		{
			if (um->type == MODE_PARAM && sep.GetToken(sbuf))
				this->SetMode(bi, um, sbuf);
			else
				this->SetMode(bi, um);
		}
		else
			this->RemoveMode(bi, um);
	}
}

void User::SetModesInternal(const char *umodes, ...)
{
	char buf[BUFSIZE] = "";
	va_list args;
	Anope::string modebuf, sbuf;
	int add = -1;
	va_start(args, umodes);
	vsnprintf(buf, BUFSIZE - 1, umodes, args);
	va_end(args);

	if (this->server && this->server->IsSynced())
		Log(this, "mode") << "changes modes to " << buf;

	spacesepstream sep(buf);
	sep.GetToken(modebuf);
	for (unsigned i = 0, end = modebuf.length(); i < end; ++i)
	{
		UserMode *um;

		switch (modebuf[i])
		{
			case '+':
				add = 1;
				continue;
			case '-':
				add = 0;
				continue;
			default:
				if (add == -1)
					continue;
				um = ModeManager::FindUserModeByChar(modebuf[i]);
				if (!um)
					continue;
		}

		if (add)
		{
			if (um->type == MODE_PARAM && sep.GetToken(sbuf))
				this->SetModeInternal(um, sbuf);
			else
				this->SetModeInternal(um);
		}
		else
			this->RemoveModeInternal(um);

		if (um->name == "OPER")
		{
			if (add)
				++OperCount;
			else
				--OperCount;
		}
		else if (um->name == "CLOAK" || um->name == "VHOST")
		{
			if (!add && !this->vhost.empty())
				this->vhost.clear();
			this->UpdateHost();
		}
	}
}

Anope::string User::GetModes() const
{
	Anope::string m, params;

	typedef std::map<Anope::string, Anope::string> mode_map;
	for (mode_map::const_iterator it = this->modes.begin(), it_end = this->modes.end(); it != it_end; ++it)
	{
		UserMode *um = ModeManager::FindUserModeByName(it->first);
		if (um == NULL)
			continue;

		m += um->mchar;

		if (!it->second.empty())
			params += " " + it->second;
	}

	return m + params;
}

ChanUserContainer *User::FindChannel(Channel *c) const
{
	User::ChanUserList::const_iterator it = this->chans.find(c);
	if (it != this->chans.end())
		return it->second;
	return NULL;
}

bool User::IsProtected() const
{
	if (this->HasMode("PROTECTED") || this->HasMode("GOD"))
		return true;

	return false;
}

void User::Kill(const Anope::string &source, const Anope::string &reason)
{
	Anope::string real_reason = (source.empty() ? Me->GetName() : source) + " (" + reason + ")";

	IRCD->SendSVSKill(BotInfo::Find(source), this, "%s", real_reason.c_str());
}

void User::KillInternal(const Anope::string &source, const Anope::string &reason)
{
	if (this->quit)
	{
		Log(LOG_DEBUG) << "Duplicate quit for " << this->nick;
		return;
	}

	Log(this, "killed") << "was killed by " << source << " (Reason: " << reason << ")";

	this->Quit(reason);

	this->quit = true;
	quitting_users.push_back(this);
}

void User::Quit(const Anope::string &reason)
{
	if (this->quit)
	{
		Log(LOG_DEBUG) << "Duplicate quit for " << this->nick;
		return;
	}

	FOREACH_MOD(I_OnUserQuit, OnUserQuit(this, reason));

	this->quit = true;
	quitting_users.push_back(this);
}

bool User::Quitting() const
{
	return this->quit;
}

Anope::string User::Mask() const
{
	Anope::string mask;
	Anope::string mident = this->GetIdent();
	Anope::string mhost = this->GetDisplayedHost();

	if (mident[0] == '~')
		mask = "*" + mident + "@";
	else
		mask = mident + "@";

	size_t dot;
	/* To make sure this is an IP, make sure the host contains only numbers and dots, and check to make sure it only contains 3 dots */
	if (mhost.find_first_not_of("0123456789.") == Anope::string::npos && (dot = mhost.find('.')) != Anope::string::npos && (dot = mhost.find('.', dot + 1)) != Anope::string::npos && (dot = mhost.find('.', dot + 1)) != Anope::string::npos && mhost.find('.', dot + 1) == Anope::string::npos)
	{ /* IP addr */
		dot = mhost.find('.');
		mask += mhost.substr(0, dot) + ".*";
	}
	else
	{
		if ((dot = mhost.find('.')) != Anope::string::npos && mhost.find('.', dot + 1) != Anope::string::npos)
			mask += "*" + mhost.substr(dot);
		else
			mask += mhost;
	}

	return mask;
}

bool User::BadPassword()
{
	if (!Config->GetBlock("options")->Get<int>("badpasslimit"))
		return false;

	if (Config->GetBlock("options")->Get<time_t>("badpasstimeout") > 0 && this->invalid_pw_time > 0 && this->invalid_pw_time < Anope::CurTime - Config->GetBlock("options")->Get<time_t>("badpasstimeout"))
		this->invalid_pw_count = 0;
	++this->invalid_pw_count;
	this->invalid_pw_time = Anope::CurTime;
	if (this->invalid_pw_count >= Config->GetBlock("options")->Get<int>("badpasslimit"))
	{
		this->Kill(Me->GetName(), "Too many invalid passwords");
		return true;
	}

	return false;
}

User* User::Find(const Anope::string &name, bool nick_only)
{
	if (!nick_only && isdigit(name[0]) && IRCD->RequiresID)
	{
		user_map::iterator it = UserListByUID.find(name);
		if (it != UserListByUID.end())
			return it->second;
	}
	else
	{
		user_map::iterator it = UserListByNick.find(name);
		if (it != UserListByNick.end())
			return it->second;
	}

	return NULL;
}

void User::QuitUsers()
{
	for (std::list<User *>::iterator it = quitting_users.begin(), it_end = quitting_users.end(); it != it_end; ++it)
		delete *it;
	quitting_users.clear();
}

