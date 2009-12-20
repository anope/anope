/* Routines to maintain a list of online users.
 *
 * (C) 2003-2009 Anope Team
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
	chans = NULL;
	invalid_pw_count = timestamp = my_signon = invalid_pw_time = lastmemosend = lastnickreg = lastmail = 0;
	OnAccess = false;

	strscpy(this->nick, snick.c_str(), NICKMAX);
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
			alog("user: New maximum user count: %d", maxusercnt);
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

	if (debug)
		alog("debug: %s changed nick to %s", this->nick, newnick.c_str());

	if (this->prev)
		this->prev->next = this->next;
	else
		userlist[HASH(this->nick)] = this->next;

	if (this->next)
		this->next->prev = this->prev;

	strscpy(this->nick, newnick.c_str(), NICKMAX);
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

	if (debug)
		alog("debug: %s changed vhost to %s", this->nick, shost.c_str());

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

	if (debug)
		alog("debug: %s changed cloaked host to %s", this->nick, newhost.c_str());

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

	if (debug)
		alog("debug: %s changed ident to %s", this->nick, sident.c_str());

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

	if (debug)
		alog("debug: %s changed real ident to %s", this->nick, sident.c_str());

	this->UpdateHost();
}

const std::string &User::GetIdent() const
{
	return this->ident;
}

void User::SetRealname(const std::string &srealname)
{
	if (srealname.empty())
		throw "realname empty in SetRealname";

	if (this->realname)
		delete [] this->realname;
	this->realname = sstrdup(srealname.c_str());
	NickAlias *na = findnick(this->nick);

	if (na && (nick_identified(this) || !this->nc || !this->nc->HasFlag(NI_SECURE) && IsRecognized()))
	{
		if (na->last_realname)
			delete [] na->last_realname;
		na->last_realname = sstrdup(srealname.c_str());
	}

	if (debug)
		alog("debug: %s changed realname to %s", this->nick, srealname.c_str());
}

User::~User()
{
	struct u_chanlist *c, *c2;
	char *srealname;

	if (Config.LogUsers)
	{
		srealname = normalizeBuffer(this->realname);

		if (ircd->vhost)
		{
			alog("LOGUSERS: %s (%s@%s => %s) (%s) left the network (%s).",
				this->nick, this->GetIdent().c_str(), this->host,
				this->GetDisplayedHost().c_str(), srealname, this->server->name);
		}
		else
		{
			alog("LOGUSERS: %s (%s@%s) (%s) left the network (%s).",
				this->nick, this->GetIdent().c_str(), this->host,
				srealname, this->server->name);
		}

		delete [] srealname;
	}

	FOREACH_MOD(I_OnUserLogoff, OnUserLogoff(this));

	if (debug >= 2)
		alog("debug: User::~User() called");

	usercnt--;

	if (is_oper(this))
		opcnt--;

	if (debug >= 2)
		alog("debug: User::~User(): free user data");

	delete [] this->host;

	if (this->vhost)
		delete [] this->vhost;

	if (this->realname)
		delete [] this->realname;
	if (this->hostip)
		delete [] this->hostip;

	if (debug >= 2)
		alog("debug: User::~User(): remove from channels");

	c = this->chans;

	while (c)
	{
		c2 = c->next;
		chan_deluser(this, c->chan);
		delete c;
		c = c2;
	}

	/* Cancel pending nickname enforcers, etc */
	cancel_user(this);

	if (debug >= 2)
		alog("debug: User::~User(): delete from list");

	if (this->prev)
		this->prev->next = this->next;
	else
		userlist[HASH(this->nick)] = this->next;

	if (this->next)
		this->next->prev = this->prev;

	if (debug >= 2)
		alog("debug: User::~User() done");
}

void User::SendMessage(const char *source, const char *fmt, ...)
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

void User::SendMessage(const char *source, const std::string &msg)
{
	/* Send privmsg instead of notice if:
	* - UsePrivmsg is enabled
	* - The user is not registered and NSDefMsg is enabled
	* - The user is registered and has set /ns set msg on
	*/
	if (Config.UsePrivmsg &&
		((!this->nc && Config.NSDefFlags.HasFlag(NI_MSG)) || (this->nc && this->nc->HasFlag(NI_MSG))))
	{
		ircdproto->SendPrivmsg(findbot(source), this->nick, "%s", msg.c_str());
	}
	else
	{
		ircdproto->SendNotice(findbot(source), this->nick, "%s", msg.c_str());
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
void User::AutoID(const char *account)
{
	NickCore *tnc;
	NickAlias *na;

	if ((tnc = findcore(account)))
	{
		this->nc = tnc;
		if ((na = findnick(this->nick)) && na->nc == tnc)
		{
			check_memos(this);
		}
	}
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

	if (nick_identified(this) || (na && !na->nc->HasFlag(NI_SECURE) && IsRecognized()))
	{
		if (na->last_usermask)
			delete [] na->last_usermask;

		std::string last_usermask = this->GetIdent() + "@" + this->GetDisplayedHost();
		na->last_usermask = sstrdup(last_usermask.c_str());
	}

	OnAccess = false;
	if (na && this->host)
		OnAccess = is_on_access(this, na->nc);
}

/*************************************************************************/

/* Return statistics.  Pointers are assumed to be valid. */

void get_user_stats(long *nusers, long *memuse)
{
	long count = 0, mem = 0;
	int i;
	User *user;
	struct u_chanlist *uc;

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
			for (uc = user->chans; uc; uc = uc->next)
				mem += sizeof(*uc);
		}
	}
	*nusers = count;
	*memuse = mem;
}

/*************************************************************************/

/* Find a user by nick.  Return NULL if user could not be found. */

User *finduser(const char *nick)
{
	User *user;

	if (debug >= 3)
		alog("debug: finduser(%p)", nick);

	if (isdigit(*nick) && ircd->ts6)
		return find_byuid(nick);

	user = userlist[HASH(nick)];
	while (user && stricmp(user->nick, nick) != 0)
		user = user->next;
	if (debug >= 3)
		alog("debug: finduser(%s) -> 0x%p", nick, static_cast<void *>(user));
	FOREACH_MOD(I_OnFindUser, OnFindUser(user));
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
	if (!um)
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
	if (!um)
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
void User::SetModes(BotInfo *bi, const std::string &modes, ...)
{
	char buf[BUFSIZE] = "";
	va_list args;
	std::string modebuf, sbuf;
	int add = -1;
	va_start(args, modes.c_str());
	vsnprintf(buf, BUFSIZE - 1, modes.c_str(), args);
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
		else if (add == 0)
		{
			this->RemoveMode(bi, um);
		}
	}
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
	if (debug)
		alog("debug: firstuser() returning %s",
			 current ? current->nick : "NULL (end of list)");
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
	if (debug)
		alog("debug: nextuser() returning %s",
			 current ? current->nick : "NULL (end of list)");
	return current;
}

User *find_byuid(const char *uid)
{
	User *u, *next;

	u = first_uid();
	while (u) {
		next = next_uid();
		if (u->GetUID() == uid) {
			FOREACH_MOD(I_OnFindUser, OnFindUser(u));
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
	if (debug >= 2) {
		alog("debug: first_uid() returning %s %s",
			 current_uid ? current_uid->nick : "NULL (end of list)",
			 current_uid ? current_uid->GetUID().c_str() : "");
	}
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
	if (debug >= 2) {
		alog("debug: next_uid() returning %s %s",
			 current_uid ? current_uid->nick : "NULL (end of list)",
			 current_uid ? current_uid->GetUID().c_str() : "");
	}
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

	char *tmp = NULL;
	NickAlias *old_na;		  /* Old nick rec */
	int nc_changed = 1;		 /* Did nick core change? */
	char *logrealname;
	std::string oldnick;		/* stores the old nick of the user, so we can pass it to OnUserNickChange */

	if (!*source) {
		char ipbuf[16];
		struct in_addr addr;

		if (ircd->nickvhost) {
			if (vhost) {
				if (!strcmp(vhost, "*")) {
					vhost = NULL;
					if (debug)
						alog("debug: new user�with no vhost in NICK command: %s", nick);
				}
			}
		}

		/* This is a new user; create a User structure for it. */
		if (debug)
			alog("debug: new user: %s", nick);

		if (ircd->nickip) {
			addr.s_addr = htonl(ip);
			ntoa(addr, ipbuf, sizeof(ipbuf));
		}


		if (Config.LogUsers) {
		/**
	 * Ugly swap routine for Flop's bug :)
 	 **/
			if (realname) {
				tmp = const_cast<char *>(strchr(realname, '%'));
				while (tmp) {
					*tmp = '-';
					tmp = const_cast<char *>(strchr(realname, '%'));
				}
			}
			logrealname = normalizeBuffer(realname);

		/**
	 * End of ugly swap
	 **/

			if (ircd->nickvhost) {
				if (ircd->nickip) {
					alog("LOGUSERS: %s (%s@%s => %s) (%s) [%s] connected to the network (%s).", nick, username, host, (vhost ? vhost : "none"), logrealname, ipbuf, server);
				} else {
					alog("LOGUSERS: %s (%s@%s => %s) (%s) connected to the network (%s).", nick, username, host, (vhost ? vhost : "none"), logrealname, server);
				}
			} else {
				if (ircd->nickip) {
					alog("LOGUSERS: %s (%s@%s) (%s) [%s] connected to the network (%s).", nick, username, host, logrealname, ipbuf, server);
				} else {
					alog("LOGUSERS: %s (%s@%s) (%s) connected to the network (%s).", nick, username, host, logrealname, server);
				}
			}
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

		/* Only call I_OnUserConnect if the user still exists */
		if (finduser(nick))
		{
			FOREACH_MOD(I_OnUserConnect, OnUserConnect(user));
		}
	} else {
		/* An old user changing nicks. */
		if (ircd->ts6)
			user = find_byuid(source);

		if (!user)
			user = finduser(source);

		if (!user) {
			alog("user: NICK from nonexistent nick %s", source);
			return NULL;
		}
		user->isSuperAdmin = 0; /* Dont let people nick change and stay SuperAdmins */
		if (debug)
			alog("debug: %s changes nick to %s", source, nick);

		if (Config.LogUsers) {
			logrealname = normalizeBuffer(user->realname);
			if (ircd->vhost) {
				alog("LOGUSERS: %s (%s@%s => %s) (%s) changed nick to %s (%s).", user->nick, user->GetIdent().c_str(), user->host, user->GetDisplayedHost().c_str(), logrealname, nick, user->server->name);
			} else {
				alog("LOGUSERS: %s (%s@%s) (%s) changed nick to %s (%s).",
					 user->nick, user->GetIdent().c_str(), user->host, logrealname,
					 nick, user->server->name);
			}
			if (logrealname) {
				delete [] logrealname;
			}
		}

		user->timestamp = ts;

		if (stricmp(nick, user->nick) == 0) {
			/* No need to redo things */
			user->SetNewNick(nick);
			nc_changed = 0;
		} else {
			/* Update this only if nicks aren't the same */
			user->my_signon = time(NULL);

			old_na = findnick(user->nick);
			if (old_na) {
				if (user->IsRecognized())
					old_na->last_seen = time(NULL);
				cancel_user(user);
			}

			oldnick = user->nick;
			user->SetNewNick(nick);
			FOREACH_MOD(I_OnUserNickChange, OnUserNickChange(user, oldnick.c_str()));

			if ((old_na ? old_na->nc : NULL) == user->nc)
				nc_changed = 0;

			if (!nc_changed)
			{
				NickAlias *tmpcore = findnick(user->nick);

				/* If the new nick isnt registerd or its registerd and not yours */
				if (!tmpcore || (old_na && tmpcore->nc != old_na->nc))
				{
					ircdproto->SendUnregisteredNick(user);
				}
			}
			else
			{
				if (!nick_identified(user) || !user->IsRecognized())
				{
					ircdproto->SendUnregisteredNick(user);
				}
			}
		}

		if (ircd->sqline)
		{
			if (!is_oper(user) && check_sqline(user->nick, 1))
				return NULL;
		}

	}						   /* if (!*source) */

	/* Do not attempt to validate the user if their server is syncing and this
	 * ircd has delayed auth - Adam
	 */
	if (!(ircd->b_delay_auth && user->server->sync == SSYNC_IN_PROGRESS))
	{
		NickAlias *ntmp = findnick(user->nick);
		if (ntmp && user->nc == ntmp->nc)
		{
			nc_changed = 0;
		}

		if (!ntmp || ntmp->nc != user->nc || nc_changed)
		{
			if (validate_user(user))
				check_memos(user);
		}
		else
		{
			ntmp->last_seen = time(NULL);

			if (ntmp->last_usermask)
				delete [] ntmp->last_usermask;
			std::string last_usermask = user->GetIdent() + "@" + user->GetDisplayedHost();
			ntmp->last_usermask = sstrdup(last_usermask.c_str());
			ircdproto->SetAutoIdentificationToken(user);
			alog("%s: %s!%s@%s automatically identified for nick %s", Config.s_NickServ, user->nick, user->GetIdent().c_str(), user->host, user->nick);
		}

		/* Bahamut sets -r on every nick changes, so we must test it even if nc_changed == 0 */
		if (ircd->check_nick_id)
		{
			if (nick_identified(user))
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
		alog("user: MODE %s for nonexistent nick %s: %s", av[1], av[0],
			 merge_args(ac, av));
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
		alog("user: QUIT from nonexistent user %s: %s", source,
			 merge_args(ac, av));
		return;
	}
	if (debug) {
		alog("debug: %s quits", source);
	}
	if ((na = findnick(user->nick)) && !na->HasFlag(NS_FORBIDDEN)
		&& !na->nc->HasFlag(NI_SUSPENDED) && (user->IsRecognized() || nick_identified(user))) {
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

void do_kill(const char *nick, const char *msg)
{
	User *user;
	NickAlias *na;

	user = finduser(nick);
	if (!user) {
		if (debug) {
			alog("debug: KILL of nonexistent nick: %s", nick);
		}
		return;
	}
	if (debug) {
		alog("debug: %s killed", nick);
	}
	if ((na = findnick(user->nick)) && !na->HasFlag(NS_FORBIDDEN) && !na->nc->HasFlag(NI_SUSPENDED) && (user->IsRecognized() || nick_identified(user))) {
		na->last_seen = time(NULL);
		if (na->last_quit)
			delete [] na->last_quit;
		na->last_quit = *msg ? sstrdup(msg) : NULL;

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
	
	if (debug)
		alog("debug: Changing user modes for %s to %s", user->nick, merge_args(ac, av));
	
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
						ircdproto->SendGlobops(findbot(Config.s_OperServ), "\2%s\2 is now an IRC operator.", user->nick);
				}
				else
					--opcnt;
				break;
			case UMODE_REGISTERED:
				if (add && !nick_identified(user))
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

