/* Routines to maintain a list of online users.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */

#include "services.h"
#include "modules.h"
#include "language.h"

#define HASH(nick)	(((nick)[0]&31)<<5 | ((nick)[1]&31))
User *userlist[1024];

#define HASH2(nick)	(((nick)[0]&31)<<5 | ((nick)[1]&31))

int32 opcnt = 0;
uint32 usercnt = 0, maxusercnt = 0;
time_t maxusertime;

/*************************************************************************/
/*************************************************************************/

User::User(const std::string &snick, const std::string &suid)
{
	User **list;

	if (snick.empty())
		throw "what the craq, empty nick passed to constructor";

	// XXX: we should also duplicate-check here.

	/* we used to do this by calloc, no more. */
	this->next = NULL;
	this->prev = NULL;
	host = hostip = vhost = realname = NULL;
	server = NULL;
	nc = NULL;
	invalid_pw_count = timestamp = my_signon = invalid_pw_time = lastmemosend = lastnickreg = lastmail = 0;
	OnAccess = false;

	this->nick = snick;
	this->uid = suid;
	list = &userlist[HASH(this->nick)];
	this->next = *list;

	if (*list)
		(*list)->prev = this;

	*list = this;

	this->nc = NULL;

	usercnt++;

	if (usercnt > maxusercnt)
	{
		maxusercnt = usercnt;
		maxusertime = time(NULL);
		if (Config.LogMaxUsers)
			Alog() << "user: New maximum user count: "<< maxusercnt;
	}

	this->isSuperAdmin = 0;	 /* always set SuperAdmin to 0 for new users */
}

void User::SetNewNick(const std::string &newnick)
{
	User **list;

	/* Sanity check to make sure we don't segfault */
	if (newnick.empty())
	{
		throw "User::SetNewNick() got a bad argument";
	}

	Alog(LOG_DEBUG) << this->nick << " changed nick to " << newnick;

	if (this->prev)
		this->prev->next = this->next;
	else
		userlist[HASH(this->nick)] = this->next;

	if (this->next)
		this->next->prev = this->prev;

	this->nick = newnick;
	list = &userlist[HASH(this->nick)];
	this->next = *list;
	this->prev = NULL;

	if (*list)
		(*list)->prev = this;
	*list = this;

	OnAccess = false;
	NickAlias *na = findnick(this->nick);
	if (na)
		OnAccess = is_on_access(this, na->nc);
}

void User::SetDisplayedHost(const std::string &shost)
{
	if (shost.empty())
		throw "empty host? in MY services? it seems it's more likely than I thought.";

	if (this->vhost)
		delete [] this->vhost;
	this->vhost = sstrdup(shost.c_str());

	Alog(LOG_DEBUG) << this->nick << " changed vhost to " << shost;

	this->UpdateHost();
}

/** Get the displayed vhost of a user record.
 * @return The displayed vhost of the user, where ircd-supported, or the user's real host.
 */
const std::string User::GetDisplayedHost() const
{
	if (ircd->vhost && this->vhost)
		return this->vhost;
	else if (this->HasMode(UMODE_CLOAK) && !this->GetCloakedHost().empty())
		return this->GetCloakedHost();
	else
		return this->host;
}

/** Update the cloaked host of a user
 * @param host The cloaked host
 */
void User::SetCloakedHost(const std::string &newhost)
{
	if (newhost.empty())
		throw "empty host in User::SetCloakedHost";

	chost = newhost;

	Alog(LOG_DEBUG) << this->nick << " changed cloaked host to " << newhost;

	this->UpdateHost();
}

/** Get the cloaked host of a user
 * @return The cloaked host
 */
const std::string &User::GetCloakedHost() const
{
	return chost;
}

const std::string &User::GetUID() const
{
 return this->uid;
}


void User::SetVIdent(const std::string &sident)
{
	this->vident = sident;

	Alog(LOG_DEBUG) << this->nick << " changed ident to " << sident;

	this->UpdateHost();
}

const std::string &User::GetVIdent() const
{
	if (this->HasMode(UMODE_CLOAK))
		return this->vident;
	else if (ircd->vident && !this->vident.empty())
		return this->vident;
	else
		return this->ident;
}

void User::SetIdent(const std::string &sident)
{
	this->ident = sident;

	Alog(LOG_DEBUG) << this->nick << " changed real ident to " << sident;

	this->UpdateHost();
}

const std::string &User::GetIdent() const
{
	return this->ident;
}

const std::string User::GetMask()
{
	std::stringstream buf;
	buf << this->nick << "!" << this->ident << "@" << this->host;
	return buf.str();
}

void User::SetRealname(const std::string &srealname)
{
	if (srealname.empty())
		throw "realname empty in SetRealname";

	if (this->realname)
		delete [] this->realname;
	this->realname = sstrdup(srealname.c_str());
	NickAlias *na = findnick(this->nick);

	if (na && (this->IsIdentified(true) || (!na->nc->HasFlag(NI_SECURE) && this->IsRecognized())))
	{
		if (na->last_realname)
			delete [] na->last_realname;
		na->last_realname = sstrdup(srealname.c_str());
	}

	Alog(LOG_DEBUG) << this->nick << " changed realname to " << srealname;
}

User::~User()
{
	Alog(LOG_DEBUG_2) << "User::~User() called";

	if (Config.LogUsers)
	{
		const char *srealname = normalizeBuffer(this->realname);

		Alog() << "LOGUSERS: " << this->GetMask() << (ircd->vhost ? " => " : " ")
			<< (ircd->vhost ? this->GetDisplayedHost() : "")
			<< " (" << srealname << ") left the network (" << this->server->name << ").";

		delete [] srealname;
	}

	FOREACH_MOD(I_OnUserLogoff, OnUserLogoff(this));

	usercnt--;

	if (is_oper(this))
		opcnt--;
	
	while (!this->chans.empty())
	{
		this->chans.front()->chan->DeleteUser(this);
	}
	
	if (this->prev)
		this->prev->next = this->next;
	else
		userlist[HASH(this->nick)] = this->next;
	if (this->next)
		this->next->prev = this->prev;

	NickAlias *na = findnick(this->nick);
	if (na)
		na->OnCancel(this);

	Alog(LOG_DEBUG_2) << "User::~User(): free user data";

	delete [] this->host;

	if (this->vhost)
		delete [] this->vhost;

	if (this->realname)
		delete [] this->realname;
	if (this->hostip)
		delete [] this->hostip;

	Alog(LOG_DEBUG_2) << "User::~User() done";
}

void User::SendMessage(const std::string &source, const char *fmt, ...)
{
	va_list args;
	char buf[BUFSIZE];
	*buf = '\0';

	if (fmt)
	{
		va_start(args, fmt);
		vsnprintf(buf, BUFSIZE - 1, fmt, args);

		this->SendMessage(source, std::string(buf));

		va_end(args);
	}
}

void User::SendMessage(const std::string &source, const std::string &msg)
{
	/* Send privmsg instead of notice if:
	* - UsePrivmsg is enabled
	* - The user is not registered and NSDefMsg is enabled
	* - The user is registered and has set /ns set msg on
	*/
	if (Config.UsePrivmsg &&
		((!this->nc && Config.NSDefFlags.HasFlag(NI_MSG)) || (this->nc && this->nc->HasFlag(NI_MSG))))
	{
		ircdproto->SendPrivmsg(findbot(source), this->nick.c_str(), "%s", msg.c_str());
	}
	else
	{
		ircdproto->SendNotice(findbot(source), this->nick.c_str(), "%s", msg.c_str());
	}
}

/** Collides a nick.
 *
 * First, it marks the nick (if the user is on a registered nick, we don't use it without but it could be)
 * as COLLIDED, this is checked in NickAlias::OnCancel.
 *
 * Then it does one of two things.
 *
 * 1. This will force change the users nick to the guest nick. This gets processed by the IRCd and comes
 *    back to call do_nick. do_nick changes the nick of the use to the new one, then calls NickAlias::OnCancel
 *    with the users old nick's nickalias (if there is one).
 *
 * 2. Calls kill_user, which will either delete the user immediatly or kill them, wait for the QUIT,
 *    then delete the user then. Users destructor then calls NickAlias::OnCancel
 *
 * NickAlias::OnCancel checks for NS_COLLIDED, it then does one of two things.
 *
 * 1. If supported, we send a SVSHold for the user. We are done here, the IRCds expires this at the time we give it.
 *
 * 2. We create a new client with SendClientIntroduction(). Note that is it important that this is called either after the
 *    user has been removed from our internal list of user or after the users nick has been updated completely internally.
 *    This is beacuse SendClientIntroduction will destroy any users we think are currently on the nickname (which causes a
 *    lot of problems, eg, deleting the user which recalls OnCancel), whether they really are or not. We then create a
 *    release timer for this new client that waits and later on sends a QUIT for the client. Release timers are never used
 *    for SVSHolds. Ever.
 *
 *
 *  Note that now for the timers we only store the users name, not the NickAlias* pointer. We never remove timers when
 *  a user changes nick or a nick is deleted, the timers must assume that either of these may have happend.
 *
 *  Storing NickAlias* pointers caused quite a problem, some of which are:
 *
 *  Having a valid timer alive that calls User::Collide would either:
 *
 *  1. Kill the user, causing users destructor to cancel all timers for the nick (as it should, it has no way of knowing
 *     if we are in a timer or not) which would delete the currently active timer while it was running, causing TimerManager
 *     to explode.
 *
 *  2. Force a user off of their nick, this would call NickAlias::Cancel before updating the user internally (to cancel the
 *     current nicks timers, granted we could have easially saved this and called it after) which could possibly try to
 *     introduce an enforcer nick. We would then check to see if the nick is already in use (it is, internally) and send
 *     a kill for that nick. That may in turn delete the user immediatly, calling users destructor, which would attempt to
 *     delete the timer, causing TimerManager to explode.
 *
 *     Additionally, if we marked the timer as "in use" so that calling the ClearTimer function wouldn't delete them, users
 *     destructor would then call NickAlias::OnCancel, which would (at this point, it was unsetting GUESTED after introducing
 *     the new client) introduce the same new client again, without actually deleting the originial user, causing an infinite
 *     loop.
 *
 *     This is why we remove NS_GUESTED first in NickAlias::OnCancel before introducing a new client, although this should
 *     not happen anymore. If I must emphasize this again, users need to be GONE from the internal list before calling
 *     NickAlias::OnCancel. NickAlias::OnCancel intentionally reffers to this->nick, not the user passed to it. They *can*
 *     (but not always) be different, depending if the user changed nicks or disconnected.
 *
 *
 *  Adam
 */
void User::Collide(NickAlias *na)
{
	if (na)
		na->SetFlag(NS_COLLIDED);

	if (ircd->svsnick)
	{
		std::string guestnick;

		do
		{
			char randbuf[17];
			snprintf(randbuf, sizeof(randbuf), "%d", getrandom16());
			guestnick = std::string(Config.NSGuestNickPrefix) + std::string(randbuf);
		}
		while (finduser(guestnick.c_str()));

		notice_lang(Config.s_NickServ, this, FORCENICKCHANGE_CHANGING, guestnick.c_str());
		ircdproto->SendForceNickChange(this, guestnick.c_str(), time(NULL));
	}
	else
	{
		kill_user(Config.s_NickServ, this->nick, "Services nickname-enforcer kill");
	}
}

/** Check if the user should become identified because
 * their svid matches the one stored in their nickcore
 * @param svid Services id
 */
void User::CheckAuthenticationToken(const char *svid)
{
	char *c;
	NickAlias *na;

	if ((na = findnick(this->nick)))
	{
		if (na->nc && na->nc->GetExtArray("authenticationtoken", c))
		{
			if (svid && c && !strcmp(svid, c))
			{
				/* Users authentication token matches so they should become identified */
				check_memos(this);
				this->nc = na->nc;
			}
		}
	}
}

/** Auto identify the user to the given accountname.
 * @param account Display nick of account
 */
void User::AutoID(const std::string &account)
{
	NickCore *core = findcore(account.c_str());

	if (core)
	{
		this->Login(core);

		NickAlias *na = findnick(this->nick);
		if (na && na->nc == core)
		{
			if (na->last_realname)
				delete [] na->last_realname;
			na->last_realname = sstrdup(this->realname);
			na->last_seen = time(NULL);
			this->SetMode(findbot(Config.s_NickServ), UMODE_REGISTERED);
			this->UpdateHost();
			check_memos(this);

			FOREACH_MOD(I_OnNickIdentify, OnNickIdentify(this));
		}
	}
}


/** Login the user to a NickCore
 * @param core The account the user is useing
 */
void User::Login(NickCore *core)
{
	nc = core;
	core->Users.push_back(this);
}

/** Logout the user
 */
void User::Logout()
{
	if (!this->nc)
		return;
		
	std::list<User *>::iterator it = std::find(this->nc->Users.begin(), this->nc->Users.end(), this);
	if (it != this->nc->Users.end())
	{
		this->nc->Users.erase(it);
	}

	nc = NULL;
}

/** Get the account the user is logged in using
 * @reurn The account or NULL
 */
NickCore *User::Account() const
{
	return nc;
}

/** Check if the user is identified for their nick
 * @param CheckNick True to check if the user is identified to the nickname they are on too
 * @return true or false
 */
const bool User::IsIdentified(bool CheckNick) const
{
	if (CheckNick && this->nc)
	{
		NickAlias *na = findnick(this->nc->display);

		if (na && na->nc == this->nc)
		{
			return this->nc;
		}
		
		return NULL;
	}

	return this->nc;
}

/** Check if the user is recognized for their nick (on the nicks access list)
 * @return true or false
 */
const bool User::IsRecognized() const
{
	return OnAccess;
}

/** Update the last usermask stored for a user, and check to see if they are recognized
 */
void User::UpdateHost()
{
	NickAlias *na = findnick(this->nick);

	OnAccess = false;
	if (na)
		OnAccess = is_on_access(this, na->nc);

	if (na && (this->IsIdentified(true) || (!na->nc->HasFlag(NI_SECURE) && this->IsRecognized())))
	{
		if (na->last_usermask)
			delete [] na->last_usermask;

		std::string last_usermask = this->GetIdent() + "@" + this->GetDisplayedHost();
		na->last_usermask = sstrdup(last_usermask.c_str());
	}
}

/*************************************************************************/

/* Return statistics.  Pointers are assumed to be valid. */

void get_user_stats(long *nusers, long *memuse)
{
	long count = 0, mem = 0;
	int i;
	User *user;

	for (i = 0; i < 1024; i++) {
		for (user = userlist[i]; user; user = user->next) {
			count++;
			mem += sizeof(*user);
			if (user->host)
				mem += strlen(user->host) + 1;
			if (ircd->vhost) {
				if (user->vhost)
					mem += strlen(user->vhost) + 1;
			}
			if (user->realname)
				mem += strlen(user->realname) + 1;
			if (user->server->name)
				mem += strlen(user->server->name) + 1;
			mem += (sizeof(ChannelContainer) * user->chans.size());
		}
	}
	*nusers = count;
	*memuse = mem;
}

/*************************************************************************/

/* Find a user by nick.  Return NULL if user could not be found. */

User *finduser(const std::string &nick)
{
	User *user;

	Alog(LOG_DEBUG_3) << "finduser("<<  nick << ")";

	if (isdigit(nick[0]) && ircd->ts6)
		return find_byuid(nick);

	ci::string ci_nick(nick.c_str());

	user = userlist[HASH(nick)];
	while (user && ci_nick != user->nick)
		user = user->next;
	Alog(LOG_DEBUG_3) << "finduser(" << nick << ") -> " << static_cast<void *>(user);
	return user;
}

/** Check if the user has a mode
 * @param Name Mode name
 * @return true or false
 */
const bool User::HasMode(UserModeName Name) const
{
	return modes[Name];
}

/** Set a mode internally on the user, the IRCd is not informed
 * @param um The user mode
 * @param Param The param, if there is one
 */
void User::SetModeInternal(UserMode *um, const std::string &Param)
{
	if (!um)
		return;

	modes[um->Name] = true;
	if (!Param.empty())
	{
		Params.insert(std::make_pair(um->Name, Param));
	}

	FOREACH_MOD(I_OnUserModeSet, OnUserModeSet(this, um->Name));
}

/** Remove a mode internally on the user, the IRCd is not informed
 * @param um The user mode
 */
void User::RemoveModeInternal(UserMode *um)
{
	if (!um)
		return;

	modes[um->Name] = false;
	std::map<UserModeName, std::string>::iterator it = Params.find(um->Name);
	if (it != Params.end())
	{
		Params.erase(it);
	}

	FOREACH_MOD(I_OnUserModeUnset, OnUserModeUnset(this, um->Name));
}

/** Set a mode on the user
 * @param bi The client setting the mode
 * @param um The user mode
 * @param Param Optional param for the mode
 */
void User::SetMode(BotInfo *bi, UserMode *um, const std::string &Param)
{
	if (!um || HasMode(um->Name))
		return;

	ModeManager::StackerAdd(bi, this, um, true, Param);
	SetModeInternal(um, Param);
}

/** Set a mode on the user
 * @param bi The client setting the mode
 * @param Name The mode name
 * @param param Optional param for the mode
 */
void User::SetMode(BotInfo *bi, UserModeName Name, const std::string &Param)
{
	SetMode(bi, ModeManager::FindUserModeByName(Name), Param);
}

/* Set a mode on the user
 * @param bi The client setting the mode
 * @param ModeChar The mode char
 * @param param Optional param for the mode
 */
void User::SetMode(BotInfo *bi, char ModeChar, const std::string &Param)
{
	SetMode(bi, ModeManager::FindUserModeByChar(ModeChar), Param);
}

/** Remove a mode on the user
 * @param bi The client setting the mode
 * @param um The user mode
 */
void User::RemoveMode(BotInfo *bi, UserMode *um)
{
	if (!um || !HasMode(um->Name))
		return;

	ModeManager::StackerAdd(bi, this, um, false);
	RemoveModeInternal(um);
}

/** Remove a mode from the user
 * @param bi The client setting the mode
 * @param Name The mode name
 */
void User::RemoveMode(BotInfo *bi, UserModeName Name)
{
	RemoveMode(bi, ModeManager::FindUserModeByName(Name));
}

/** Remove a mode from the user
 * @param bi The client setting the mode
 * @param ModeChar The mode char
 */
void User::RemoveMode(BotInfo *bi, char ModeChar)
{
	RemoveMode(bi, ModeManager::FindUserModeByChar(ModeChar));
}

/** Set a string of modes on a user
 * @param bi The client setting the mode
 * @param modes The modes
 */
void User::SetModes(BotInfo *bi, const char *modes, ...)
{
	char buf[BUFSIZE] = "";
	va_list args;
	std::string modebuf, sbuf;
	int add = -1;
	va_start(args, modes);
	vsnprintf(buf, BUFSIZE - 1, modes, args);
	va_end(args);

	spacesepstream sep(buf);
	sep.GetToken(modebuf);
	for (unsigned i = 0; i < modebuf.size(); ++i)
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
			if (um->Type == MODE_PARAM && sep.GetToken(sbuf))
				this->SetMode(bi, um, sbuf);
			else
				this->SetMode(bi, um);
		}
		else
		{
			this->RemoveMode(bi, um);
		}
	}
}

/** Find the channel container for Channel c that the user is on
 * This is preferred over using FindUser in Channel, as there are usually more users in a channel
 * than channels a user is in
 * @param c The channel
 * @return The channel container, or NULL
 */
ChannelContainer *User::FindChannel(Channel *c)
{
	for (UChannelList::iterator it = this->chans.begin(); it != this->chans.end(); ++it)
		if ((*it)->chan == c)
			return *it;
	return NULL;
}

/*************************************************************************/

/* Iterate over all users in the user list.  Return NULL at end of list. */

static User *current;
static int next_index;

User *firstuser()
{
	next_index = 0;
	current = NULL;
	while (next_index < 1024 && current == NULL)
		current = userlist[next_index++];
	Alog(LOG_DEBUG) << "firstuser() returning " << (current ? current->nick : "NULL (end of list)");
	return current;
}

User *nextuser()
{
	if (current)
		current = current->next;
	if (!current && next_index < 1024) {
		while (next_index < 1024 && current == NULL)
			current = userlist[next_index++];
	}
	Alog(LOG_DEBUG) << "nextuser() returning " << (current ? current->nick : "NULL (end of list)");
	return current;
}

User *find_byuid(const std::string &uid)
{
	User *u, *next;

	u = first_uid();
	while (u) {
		next = next_uid();
		if (u->GetUID() == uid) {
			return u;
		}
		u = next;
	}
	return NULL;
}

static User *current_uid;
static int next_index_uid;

User *first_uid()
{
	next_index_uid = 0;
	current_uid = NULL;
	while (next_index_uid < 1024 && current_uid == NULL) {
		current_uid = userlist[next_index_uid++];
	}
	Alog(LOG_DEBUG_2) << "first_uid() returning "
			<< (current_uid ? current_uid->nick : "NULL (end of list)") << " "
			<< (current_uid ? current_uid->GetUID() : "");
	return current_uid;
}

User *next_uid()
{
	if (current_uid)
		current_uid = current_uid->next;
	if (!current_uid && next_index_uid < 1024) {
		while (next_index_uid < 1024 && current_uid == NULL)
			current_uid = userlist[next_index_uid++];
	}
	Alog(LOG_DEBUG_2) << "next_uid() returning "
			<< (current_uid ? current_uid->nick : "NULL (end of list)") << " "
			<< (current_uid ? current_uid->GetUID() : "");
	return current_uid;
}

/*************************************************************************/
/*************************************************************************/

/* Handle a server NICK command. */

User *do_nick(const char *source, const char *nick, const char *username, const char *host,
			  const char *server, const char *realname, time_t ts,
			  uint32 ip, const char *vhost, const char *uid)
{
	User *user = NULL;

	if (!*source) {
		char ipbuf[16];
		struct in_addr addr;

		if (ircd->nickvhost) {
			if (vhost) {
				if (!strcmp(vhost, "*")) {
					vhost = NULL;
					Alog(LOG_DEBUG) << "new user with no vhost in NICK command: " << nick;
				}
			}
		}

		/* This is a new user; create a User structure for it. */
		Alog(LOG_DEBUG) << "new user: " << nick;

		if (ircd->nickip) {
			addr.s_addr = htonl(ip);
			ntoa(addr, ipbuf, sizeof(ipbuf));
		}


		if (Config.LogUsers)
		{
			/**
			 * Ugly swap routine for Flop's bug :)   XXX
			 **/
			if (realname)
			{
				char *tmp = const_cast<char *>(strchr(realname, '%'));
				while (tmp)
				{
					*tmp = '-';
					tmp = const_cast<char *>(strchr(realname, '%'));
				}
			}
			const char *logrealname = normalizeBuffer(realname);

			/**
			 * End of ugly swap
			 **/
			Alog() << "LOGUSERS: " << nick << " (" << username << "@" << host
				<< (ircd->nickvhost && vhost ? " => " : "")
				<< (ircd->nickvhost && vhost ? vhost : "") << ") (" << logrealname << ") "
				<< (ircd->nickip ? "[" : "") << (ircd->nickip ? ipbuf : "") << (ircd->nickip ? "]" : "")
				<< " connected to the network (" << server << ").";
			delete [] logrealname;
		}

		/* Allocate User structure and fill it in. */
		user = new User(nick, uid ? uid : "");
		user->SetIdent(username);
		user->host = sstrdup(host);
		user->server = findserver(servlist, server);
		user->realname = sstrdup(realname);
		user->timestamp = ts;
		user->my_signon = time(NULL);
		if (vhost)
			user->SetCloakedHost(vhost);
		user->SetVIdent(username);
		/* We now store the user's ip in the user_ struct,
		 * because we will use it in serveral places -- DrStein */
		if (ircd->nickip) {
			user->hostip = sstrdup(ipbuf);
		} else {
			user->hostip = NULL;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnPreUserConnect, OnPreUserConnect(user));
		if (MOD_RESULT == EVENT_STOP)
			return finduser(nick);

		check_akill(nick, username, host, vhost, ipbuf);

		if (ircd->sgline)
			check_sgline(nick, realname);

		if (ircd->sqline)
			check_sqline(nick, 0);

		if (ircd->szline && ircd->nickip)
			check_szline(nick, ipbuf);

		if (Config.LimitSessions && !is_ulined(server))
			add_session(nick, host, ipbuf);

		/* User is no longer connected, return */
		if (!finduser(nick))
			return NULL;

		FOREACH_MOD(I_OnUserConnect, OnUserConnect(user));
	}
	else
	{
		/* An old user changing nicks. */
		if (ircd->ts6)
			user = find_byuid(source);

		if (!user)
			user = finduser(source);

		if (!user) {
			Alog() << "user: NICK from nonexistent nick " << source;
			return NULL;
		}
		user->isSuperAdmin = 0; /* Dont let people nick change and stay SuperAdmins */
		Alog(LOG_DEBUG) << source << " changes nick to " <<  nick;

		if (Config.LogUsers)
		{
			const char *logrealname = normalizeBuffer(user->realname);
			Alog() << "LOGUSERS: " << user->nick << " (" << user->GetIdent() << "@" << user->host
				<< (ircd->vhost ? " => " : "") << (ircd->vhost ? user->GetDisplayedHost() : "") << ") (" 
				<< logrealname << ") " << "changed nick to " << nick << " (" << user->server->name << ").";
			if (logrealname)
				delete [] logrealname;
		}

		user->timestamp = ts;

		if (stricmp(nick, user->nick.c_str()) == 0)
		{
			/* No need to redo things */
			user->SetNewNick(nick);
		}
		else
		{
			/* Update this only if nicks aren't the same */
			user->my_signon = time(NULL);

			NickAlias *old_na = findnick(user->nick);
			if (old_na && (old_na->nc == user->Account() || user->IsRecognized()))
				old_na->last_seen = time(NULL);

			std::string oldnick = user->nick;
			user->SetNewNick(nick);
			FOREACH_MOD(I_OnUserNickChange, OnUserNickChange(user, oldnick.c_str()));

			if (old_na)
				old_na->OnCancel(user);

			NickAlias *na = findnick(user->nick);
			/* If the new nick isnt registerd or its registerd and not yours */
			if (!na || (old_na && na->nc != old_na->nc))
			{
				ircdproto->SendUnregisteredNick(user);
			}
		}

		if (ircd->sqline)
		{
			if (!is_oper(user) && check_sqline(user->nick.c_str(), 1))
				return NULL;
		}

	}						   /* if (!*source) */

	/* Do not attempt to validate the user if their server is syncing and this
	 * ircd has delayed auth - Adam
	 */
	if (!(ircd->b_delay_auth && user->server->sync == SSYNC_IN_PROGRESS))
	{
		NickAlias *na = findnick(user->nick);

		if (!na || na->nc != user->Account())
		{
			if (validate_user(user))
				check_memos(user);
		}
		else
		{
			na->last_seen = time(NULL);
			user->UpdateHost();
			do_on_id(user);
			ircdproto->SetAutoIdentificationToken(user);
			Alog() << Config.s_NickServ << ": " << user->GetMask() << " automatically identified for group " << user->Account()->display;
		}

		/* Bahamut sets -r on every nick changes */
		if (ircd->check_nick_id)
		{
			if (user->IsIdentified())
			{
				ircdproto->SetAutoIdentificationToken(user);
			}
		}
	}

	return user;
}

/*************************************************************************/

/* Handle a MODE command for a user.
 *	av[0] = nick to change mode for
 *	av[1] = modes
 */

void do_umode(const char *source, int ac, const char **av)
{
	User *user;

	user = finduser(av[0]);
	if (!user) {
		Alog() << "user: MODE "<< av[1] << " for nonexistent nick "<< av[0] << ":" << merge_args(ac, av);
		return;
	}

	UserSetInternalModes(user, ac - 1, &av[1]);
}

/*************************************************************************/

/* Handle a QUIT command.
 *	av[0] = reason
 */

void do_quit(const char *source, int ac, const char **av)
{
	User *user;
	NickAlias *na;

	user = finduser(source);
	if (!user) {
		Alog() << "user: QUIT from nonexistent user " << source << ":" << merge_args(ac, av);
		return;
	}
	Alog(LOG_DEBUG) << source << " quits";
	if ((na = findnick(user->nick)) && !na->HasFlag(NS_FORBIDDEN)
		&& !na->nc->HasFlag(NI_SUSPENDED) && (user->IsRecognized() || user->IsIdentified(true))) {
		na->last_seen = time(NULL);
		if (na->last_quit)
			delete [] na->last_quit;
		na->last_quit = *av[0] ? sstrdup(av[0]) : NULL;
	}
	if (Config.LimitSessions && !is_ulined(user->server->name)) {
		del_session(user->host);
	}
	FOREACH_MOD(I_OnUserQuit, OnUserQuit(user, *av[0] ? av[0] : ""));
	delete user;
}

/*************************************************************************/

/* Handle a KILL command.
 *	av[0] = nick being killed
 *	av[1] = reason
 */

void do_kill(const std::string &nick, const std::string &msg)
{
	User *user;
	NickAlias *na;

	user = finduser(nick);
	if (!user)
	{
		Alog(LOG_DEBUG) << "KILL of nonexistent nick: " <<  nick;
		return;
	}
	Alog(LOG_DEBUG) << nick << " killed";
	if ((na = findnick(user->nick)) && !na->HasFlag(NS_FORBIDDEN) && !na->nc->HasFlag(NI_SUSPENDED) && (user->IsRecognized() || user->IsIdentified(true))) 
	{
		na->last_seen = time(NULL);
		if (na->last_quit)
			delete [] na->last_quit;
		na->last_quit = !msg.empty() ? sstrdup(msg.c_str()) : NULL;
	}
	if (Config.LimitSessions && !is_ulined(user->server->name)) {
		del_session(user->host);
	}
	delete user;
}

/*************************************************************************/
/*************************************************************************/

/* Is the given user protected from kicks and negative mode changes? */

int is_protected(User * user)
{
	if (user && user->HasMode(UMODE_PROTECTED))
		return 1;

	return 0;
}

/*************************************************************************/

/* Is the given nick an oper? */

int is_oper(User * user)
{
	if (user && user->HasMode(UMODE_OPER))
		return 1;

	return 0;
}

/*************************************************************************/
/*************************************************************************/

/* Is the given user ban-excepted? */
int is_excepted(ChannelInfo * ci, User * user)
{
	if (!ci->c || !ModeManager::FindChannelModeByName(CMODE_EXCEPT))
		return 0;

	if (elist_match_user(ci->c->excepts, user))
		return 1;

	return 0;
}

/*************************************************************************/

/* Is the given MASK ban-excepted? */
int is_excepted_mask(ChannelInfo * ci, const char *mask)
{
	if (!ci->c || !ModeManager::FindChannelModeByName(CMODE_EXCEPT))
		return 0;

	if (elist_match_mask(ci->c->excepts, mask, 0))
		return 1;

	return 0;
}


/*************************************************************************/

/* Does the user's usermask match the given mask (either nick!user@host or
 * just user@host)?
 */

int match_usermask(const char *mask, User * user)
{
	char *mask2;
	char *nick, *username, *host;
	int result;

	if (!mask || !*mask) {
		return 0;
	}

	mask2 = sstrdup(mask);

	if (strchr(mask2, '!')) {
		nick = strtok(mask2, "!");
		username = strtok(NULL, "@");
	} else {
		nick = NULL;
		username = strtok(mask2, "@");
	}
	host = strtok(NULL, "");
	if (!username || !host) {
		delete [] mask2;
		return 0;
	}

	if (nick) {
		result = Anope::Match(user->nick, nick, false)
			&& Anope::Match(user->GetIdent().c_str(), username, false)
			&& (Anope::Match(user->host, host, false)
				|| Anope::Match(user->GetDisplayedHost().c_str(), host, false));
	} else {
		result = Anope::Match(user->GetIdent().c_str(), username, false)
			&& (Anope::Match(user->host, host, false)
				|| Anope::Match(user->GetDisplayedHost().c_str(), host, false));
	}

	delete [] mask2;
	return result;
}

/*************************************************************************/

/* Given a user, return a mask that will most likely match any address the
 * user will have from that location.  For IP addresses, wildcards the
 * appropriate subnet mask (e.g. 35.1.1.1 -> 35.*; 128.2.1.1 -> 128.2.*);
 * for named addresses, wildcards the leftmost part of the name unless the
 * name only contains two parts.  If the username begins with a ~, delete
 * it.  The returned character string is malloc'd and should be free'd
 * when done with.
 */

char *create_mask(User * u)
{
	char *mask, *s, *end;
	std::string mident = u->GetIdent();
	std::string mhost = u->GetDisplayedHost();
	int ulen = mident.length();

	/* Get us a buffer the size of the username plus hostname.  The result
	 * will never be longer than this (and will often be shorter), thus we
	 * can use strcpy() and sprintf() safely.
	 */
	end = mask = new char[ulen + mhost.length() + 3];
	if (mident[0] == '~')
		end += sprintf(end, "*%s@", mident.c_str());
	else
		end += sprintf(end, "%s@", mident.c_str());

	// XXX: someone needs to rewrite this godawful kitten murdering pile of crap.
	if (strspn(mhost.c_str(), "0123456789.") == mhost.length()
		&& (s = strchr(const_cast<char *>(mhost.c_str()), '.')) // XXX - Potentially unsafe cast
		&& (s = strchr(s + 1, '.'))
		&& (s = strchr(s + 1, '.'))
		&& (!strchr(s + 1, '.')))
	{	 /* IP addr */
		s = sstrdup(mhost.c_str());
		*strrchr(s, '.') = 0;

		sprintf(end, "%s.*", s);
		delete [] s;
	}
	else
	{
		if ((s = strchr(const_cast<char *>(mhost.c_str()), '.')) && strchr(s + 1, '.')) {
			s = sstrdup(strchr(mhost.c_str(), '.') - 1);
			*s = '*';
			strcpy(end, s);
			delete [] s;
		} else {
			strcpy(end, mhost.c_str());
		}
	}
	return mask;
}

/*************************************************************************/

/** Set modes internally on a user
 * @param user The user
 * @param ac Number of args
 * @param av Args
 */
void UserSetInternalModes(User *user, int ac, const char **av)
{
	int add = -1, j = 0;
	const char *modes = av[0];
	if (!user || !modes)
		return;

	Alog(LOG_DEBUG) << "Changing user modes for " << user->nick << " to " << merge_args(ac, av);

	for (; *modes; *modes++)
	{
		UserMode *um;

		switch (*modes)
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
				um = ModeManager::FindUserModeByChar(*modes);
				if (!um)
					continue;
		}

		if (um->Type == MODE_REGULAR)
		{
			if (add)
				user->SetModeInternal(um);
			else
				user->RemoveModeInternal(um);
		}
		else if (++j < ac)
		{
			if (add)
				user->SetModeInternal(um, av[j]);
			else
				user->RemoveModeInternal(um);
		}

		switch (um->Name)
		{
			case UMODE_OPER:
				if (add)
				{
					++opcnt;
					if (Config.WallOper)
						ircdproto->SendGlobops(findbot(Config.s_OperServ), "\2%s\2 is now an IRC operator.", user->nick.c_str());
				}
				else
					--opcnt;
				break;
			case UMODE_REGISTERED:
				if (add && !user->IsIdentified())
					user->RemoveMode(findbot(Config.s_NickServ), UMODE_REGISTERED);
				break;
			case UMODE_CLOAK:
			case UMODE_VHOST:
				if (add && user->vhost)
				{
					user->SetCloakedHost(user->vhost);
					delete [] user->vhost;
					user->vhost = NULL;
				}
				else if (user->vhost)
				{
					delete [] user->vhost;
					user->vhost = NULL;
				}
				user->UpdateHost();
			default:
				break;
		}
	}
}

