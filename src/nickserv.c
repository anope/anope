
/* NickServ functions.
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
#include "pseudo.h"

/*************************************************************************/

#define HASH(nick)	((tolower((nick)[0])&31)<<5 | (tolower((nick)[1])&31))

NickAlias *nalists[1024];
NickCore *nclists[1024];
NickRequest *nrlists[1024];

unsigned int guestnum;		  /* Current guest number */

#define TO_COLLIDE   0		  /* Collide the user with this nick */
#define TO_RELEASE   1		  /* Release a collided nick */

/*************************************************************************/

static void add_ns_timeout(NickAlias * na, int type, time_t delay);

/*************************************************************************/
/* *INDENT-OFF* */
void moduleAddNickServCmds() {
	ModuleManager::LoadModuleList(NickServCoreNumber, NickServCoreModules);
}
/* *INDENT-ON* */
/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_aliases_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;
	int i;
	NickAlias *na;

	for (i = 0; i < 1024; i++) {
		for (na = nalists[i]; na; na = na->next) {
			count++;
			mem += sizeof(*na);
			if (na->nick)
				mem += strlen(na->nick) + 1;
			if (na->last_usermask)
				mem += strlen(na->last_usermask) + 1;
			if (na->last_realname)
				mem += strlen(na->last_realname) + 1;
			if (na->last_quit)
				mem += strlen(na->last_quit) + 1;
		}
	}
	*nrec = count;
	*memuse = mem;
}

/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_core_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;
	int i, j;
	NickCore *nc;
	char **accptr;

	for (i = 0; i < 1024; i++) {
		for (nc = nclists[i]; nc; nc = nc->next) {
			count++;
			mem += sizeof(*nc);

			if (nc->display)
				mem += strlen(nc->display) + 1;
			if (nc->pass)
				mem += strlen(nc->pass) + 1;

			if (nc->url)
				mem += strlen(nc->url) + 1;
			if (nc->email)
				mem += strlen(nc->email) + 1;
			if (nc->greet)
				mem += strlen(nc->greet) + 1;

			mem += sizeof(char *) * nc->accesscount;
			for (accptr = nc->access, j = 0; j < nc->accesscount;
				 accptr++, j++) {
				if (*accptr)
					mem += strlen(*accptr) + 1;
			}

			mem += nc->memos.memos.size() * sizeof(Memo);
			for (j = 0; j < nc->memos.memos.size(); j++) {
				if (nc->memos.memos[j]->text)
					mem += strlen(nc->memos.memos[j]->text) + 1;
			}

			mem += sizeof(void *) * nc->aliases.count;
		}
	}
	*nrec = count;
	*memuse = mem;
}

/*************************************************************************/
/*************************************************************************/

/* NickServ initialization. */

void ns_init()
{
	moduleAddNickServCmds();
	guestnum = time(NULL);
	while (guestnum > 9999999)
		guestnum -= 10000000;
}

/*************************************************************************/

/* Main NickServ routine. */

void nickserv(User * u, char *buf)
{
	const char *cmd, *s;

	cmd = strtok(buf, " ");

	if (!cmd) {
		return;
	} else if (stricmp(cmd, "\1PING") == 0) {
		if (!(s = strtok(NULL, ""))) {
			s = "";
		}
		ircdproto->SendCTCP(findbot(s_NickServ), u->nick, "PING %s", s);
	} else {
		mod_run_cmd(s_NickServ, u, NICKSERV, cmd);
	}

}

/*************************************************************************/

/* Load/save data files. */


#define SAFE(x) do {					\
	if ((x) < 0) {					\
	if (!forceload)					\
		fatal("Read error on %s", NickDBName);	\
	failed = 1;					\
	break;						\
	}							\
} while (0)

void load_ns_req_db()
{
	dbFILE *f;
	int i, c, ver;
	NickRequest *nr;
	uint32 tmp32;
	int failed = 0, len;
	char *pass;

	if (!(f = open_db(s_NickServ, PreNickDBName, "r", PRE_NICK_VERSION)))
		return;
	ver = get_file_version(f);
	for (i = 0; i < 1024 && !failed; i++) {
		while ((c = getc_db(f)) == 1) {
			if (c != 1)
				fatal("Invalid format in %s", PreNickDBName);
			nr = new NickRequest;
			SAFE(read_string(&nr->nick, f));
			SAFE(read_string(&nr->passcode, f));
			if (ver < 2) {
				SAFE(read_string(&pass, f));
				len = strlen(pass);
				enc_encrypt(pass, len, nr->password, PASSMAX);
				memset(pass, 0, len);
				delete [] pass;
			} else
				SAFE(read_buffer(nr->password, f));
			SAFE(read_string(&nr->email, f));
			SAFE(read_int32(&tmp32, f));
			nr->requested = tmp32;
			insert_requestnick(nr);
		}
	}
	close_db(f);
}

void load_ns_dbase()
{
	dbFILE *f;
	int ver, i, j, c;
	NickAlias *na, **nalast, *naprev;
	NickCore *nc, **nclast, *ncprev;
	int failed = 0;
	uint16 tmp16;
	uint32 tmp32;
	char *s, *pass;

	if (!(f = open_db(s_NickServ, NickDBName, "r", NICK_VERSION)))
		return;

	ver = get_file_version(f);

	if (ver != 14) {
		close_db(f);
		fatal("DB %s is too old", NickDBName);
		return;
	}

	/* First we load nick cores */
	for (i = 0; i < 1024 && !failed; i++) {
		nclast = &nclists[i];
		ncprev = NULL;

		while ((c = getc_db(f)) == 1) {
			if (c != 1)
				fatal("Invalid format in %s", NickDBName);

			nc = new NickCore();
			*nclast = nc;
			nclast = &nc->next;
			nc->prev = ncprev;
			ncprev = nc;

			slist_init(&nc->aliases);

			SAFE(read_string(&nc->display, f));
			SAFE(read_buffer(nc->pass, f));

			SAFE(read_string(&nc->email, f));
			SAFE(read_string(&nc->greet, f));
			SAFE(read_int32(&nc->icq, f));
			SAFE(read_string(&nc->url, f));

			SAFE(read_int32(&nc->flags, f));
			if (!NSAllowKillImmed)
				nc->flags &= ~NI_KILL_IMMED;
			SAFE(read_int16(&nc->language, f));

			/* Add services opers and admins to the appropriate list, but
			   only if the database version is more than 10. */
			if (nc->flags & NI_SERVICES_ADMIN)
				slist_add(&servadmins, nc);
			if (nc->flags & NI_SERVICES_OPER)
				slist_add(&servopers, nc);

			SAFE(read_int16(&nc->accesscount, f));
			if (nc->accesscount) {
				char **access;
				access = static_cast<char **>(scalloc(sizeof(char *) * nc->accesscount, 1));
				nc->access = access;
				for (j = 0; j < nc->accesscount; j++, access++)
					SAFE(read_string(access, f));
			}

			SAFE(read_int16(&tmp16, f));
			if (tmp16) nc->memos.memos.resize(tmp16);
			SAFE(read_int16(&tmp16, f));
			nc->memos.memomax = static_cast<int16>(tmp16);
			if (!nc->memos.memos.empty()) {
				for (j = 0; j < nc->memos.memos.size(); j++) {
					nc->memos.memos[j] = new Memo;
					Memo *memo = nc->memos.memos[j];
					SAFE(read_int32(&memo->number, f));
					SAFE(read_int16(&memo->flags, f));
					SAFE(read_int32(&tmp32, f));
					memo->time = tmp32;
					SAFE(read_buffer(memo->sender, f));
					SAFE(read_string(&memo->text, f));
				}
			}

			SAFE(read_int16(&nc->channelcount, f));
			SAFE(read_int16(&tmp16, f));
		}					   /* while (getc_db(f) != 0) */
		*nclast = NULL;
	}						   /* for (i) */

	for (i = 0; i < 1024 && !failed; i++) {
		nalast = &nalists[i];
		naprev = NULL;
		while ((c = getc_db(f)) == 1) {
			if (c != 1)
				fatal("Invalid format in %s", NickDBName);

			na = new NickAlias();

			SAFE(read_string(&na->nick, f));

			SAFE(read_string(&na->last_usermask, f));
			SAFE(read_string(&na->last_realname, f));
			SAFE(read_string(&na->last_quit, f));

			SAFE(read_int32(&tmp32, f));
			na->time_registered = tmp32;
			SAFE(read_int32(&tmp32, f));
			na->last_seen = tmp32;
			SAFE(read_int16(&na->status, f));
			na->status &= ~NS_TEMPORARY;

			SAFE(read_string(&s, f));
			na->nc = findcore(s);
			delete [] s;

			slist_add(&na->nc->aliases, na);

			if (!(na->status & NS_VERBOTEN)) {
				if (!na->last_usermask)
					na->last_usermask = sstrdup("");
				if (!na->last_realname)
					na->last_realname = sstrdup("");
			}

			na->nc->flags &= ~NI_SERVICES_ROOT;

			*nalast = na;
			nalast = &na->next;
			na->prev = naprev;
			naprev = na;

		}					   /* while (getc_db(f) != 0) */

		*nalast = NULL;
	}						   /* for (i) */

	close_db(f);

	for (i = 0; i < 1024; i++) {
		NickAlias *next;

		for (na = nalists[i]; na; na = next) {
			next = na->next;
			/* We check for coreless nicks (although it should never happen) */
			if (!na->nc) {
				alog("%s: while loading database: %s has no core! We delete it.", s_NickServ, na->nick);
				delnick(na);
				continue;
			}

			/* Add the Services root flag if needed. */
			for (j = 0; j < RootNumber; j++)
				if (!stricmp(ServicesRoots[j], na->nick))
					na->nc->flags |= NI_SERVICES_ROOT;
		}
	}
}

#undef SAFE

/*************************************************************************/

#define SAFE(x) do {						\
	if ((x) < 0) {						\
	restore_db(f);						\
	log_perror("Write error on %s", NickDBName);		\
	if (time(NULL) - lastwarn > WarningTimeout) {		\
		ircdproto->SendGlobops(NULL, "Write error on %s: %s", NickDBName,	\
			strerror(errno));			\
		lastwarn = time(NULL);				\
	}							\
	return;							\
	}								\
} while (0)



void save_ns_dbase()
{
	dbFILE *f;
	int i, j;
	NickAlias *na;
	NickCore *nc;
	char **access;
	static time_t lastwarn = 0;

	if (!(f = open_db(s_NickServ, NickDBName, "w", NICK_VERSION)))
		return;

	for (i = 0; i < 1024; i++) {
		for (nc = nclists[i]; nc; nc = nc->next) {
			SAFE(write_int8(1, f));

			SAFE(write_string(nc->display, f));
			SAFE(write_buffer(nc->pass, f));

			SAFE(write_string(nc->email, f));
			SAFE(write_string(nc->greet, f));
			SAFE(write_int32(nc->icq, f));
			SAFE(write_string(nc->url, f));

			SAFE(write_int32(nc->flags, f));
			SAFE(write_int16(nc->language, f));

			SAFE(write_int16(nc->accesscount, f));
			for (j = 0, access = nc->access; j < nc->accesscount;
				 j++, access++)
				SAFE(write_string(*access, f));

			SAFE(write_int16(nc->memos.memos.size(), f));
			SAFE(write_int16(nc->memos.memomax, f));
			for (j = 0; j < nc->memos.memos.size(); j++) {
				Memo *memo = nc->memos.memos[j];
				SAFE(write_int32(memo->number, f));
				SAFE(write_int16(memo->flags, f));
				SAFE(write_int32(memo->time, f));
				SAFE(write_buffer(memo->sender, f));
				SAFE(write_string(memo->text, f));
			}

			SAFE(write_int16(nc->channelcount, f));
			SAFE(write_int16(nc->channelcount, f)); // write this twice to avoid having to revbump the NickServ DB from anope 1.7, hack alert XXX

		}					   /* for (nc) */

		SAFE(write_int8(0, f));

	}						   /* for (i) */

	for (i = 0; i < 1024; i++) {
		for (na = nalists[i]; na; na = na->next) {
			SAFE(write_int8(1, f));

			SAFE(write_string(na->nick, f));

			SAFE(write_string(na->last_usermask, f));
			SAFE(write_string(na->last_realname, f));
			SAFE(write_string(na->last_quit, f));

			SAFE(write_int32(na->time_registered, f));
			SAFE(write_int32(na->last_seen, f));

			SAFE(write_int16(na->status, f));

			SAFE(write_string(na->nc->display, f));

		}					   /* for (na) */
		SAFE(write_int8(0, f));
	}						   /* for (i) */

	close_db(f);

}

void save_ns_req_dbase()
{
	dbFILE *f;
	int i;
	NickRequest *nr;
	static time_t lastwarn = 0;

	if (!(f = open_db(s_NickServ, PreNickDBName, "w", PRE_NICK_VERSION)))
		return;

	for (i = 0; i < 1024; i++) {
		for (nr = nrlists[i]; nr; nr = nr->next) {
			SAFE(write_int8(1, f));
			SAFE(write_string(nr->nick, f));
			SAFE(write_string(nr->passcode, f));
			SAFE(write_buffer(nr->password, f));
			SAFE(write_string(nr->email, f));
			SAFE(write_int32(nr->requested, f));
			SAFE(write_int8(0, f));
		}
	}
	close_db(f);

}

#undef SAFE

/*************************************************************************/

/* Check whether a user is on the access list of the nick they're using If
 * not, send warnings as appropriate.  If so (and not NI_SECURE), update
 * last seen info.
 * Return 1 if the user is valid and recognized, 0 otherwise (note
 * that this means an NI_SECURE nick will return 0 from here).
 * If the user's nick is not registered, 0 is returned.
 */

int validate_user(User * u)
{
	NickAlias *na;
	NickRequest *nr;

	int on_access;

	if ((nr = findrequestnick(u->nick))) {
		notice_lang(s_NickServ, u, NICK_IS_PREREG);
	}

	if (!(na = u->na))
		return 0;

	if (na->status & NS_VERBOTEN) {
		notice_lang(s_NickServ, u, NICK_MAY_NOT_BE_USED);
		collide(na, 0);
		return 0;
	}

	if (na->nc->flags & NI_SUSPENDED) {
		notice_lang(s_NickServ, u, NICK_X_SUSPENDED, u->nick);
		collide(na, 0);
		return 0;
	}

	on_access = is_on_access(u, na->nc);
	if (on_access)
		na->status |= NS_ON_ACCESS;

	if (!(na->nc->flags & NI_SECURE) && on_access) {
		na->status |= NS_RECOGNIZED;
		na->last_seen = time(NULL);
		if (na->last_usermask)
			delete [] na->last_usermask;
		na->last_usermask = new char[strlen(common_get_vident(u)) + strlen(common_get_vhost(u)) + 2];
		sprintf(na->last_usermask, "%s@%s", common_get_vident(u),
				common_get_vhost(u));
		if (na->last_realname)
			delete [] na->last_realname;
		na->last_realname = sstrdup(u->realname);
		return 1;
	}

	if (on_access || !(na->nc->flags & NI_KILL_IMMED)) {
		if (na->nc->flags & NI_SECURE)
			notice_lang(s_NickServ, u, NICK_IS_SECURE, s_NickServ);
		else
			notice_lang(s_NickServ, u, NICK_IS_REGISTERED, s_NickServ);
	}

	if ((na->nc->flags & NI_KILLPROTECT) && !on_access) {
		if (na->nc->flags & NI_KILL_IMMED) {
			notice_lang(s_NickServ, u, FORCENICKCHANGE_NOW);
			collide(na, 0);
		} else if (na->nc->flags & NI_KILL_QUICK) {
			notice_lang(s_NickServ, u, FORCENICKCHANGE_IN_20_SECONDS);
			add_ns_timeout(na, TO_COLLIDE, 20);
		} else {
			notice_lang(s_NickServ, u, FORCENICKCHANGE_IN_1_MINUTE);
			add_ns_timeout(na, TO_COLLIDE, 60);
		}
	}

	return 0;
}

/*************************************************************************/

/* Cancel validation flags for a nick (i.e. when the user with that nick
 * signs off or changes nicks).  Also cancels any impending collide. */

void cancel_user(User * u)
{
	NickAlias *na = u->na;

	if (na)
	{
		if (na->status & NS_GUESTED)
		{
			if (ircd->svshold)
			{
				ircdproto->SendSVSHold(na->nick);
			}
			else if (ircd->svsnick)
			{
				ircdproto->SendClientIntroduction(u->nick, NSEnforcerUser, NSEnforcerHost, "Services Enforcer", "+", ts6_uid_retrieve());
				add_ns_timeout(na, TO_RELEASE, NSReleaseTimeout);
			}
			else
			{
				ircdproto->SendSVSKill(s_NickServ, u->nick, "Please do not use a registered nickname without identifying");
			}
			na->status &= ~NS_TEMPORARY;
			na->status |= NS_KILL_HELD;
		}
		else
		{
			na->status &= ~NS_TEMPORARY;
		}

		del_ns_timeout(na, TO_COLLIDE);
	}
}

/*************************************************************************/

/* Return whether a user has identified for their nickname. */

int nick_identified(User * u)
{
	if (u) {
		if (u->na) {
			if (u->na->status) {
				return (u->na->status & NS_IDENTIFIED);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}
	return 0;
}

/*************************************************************************/

/* Return whether a user is recognized for their nickname. */

int nick_recognized(User * u)
{
	if (u) {
		if (u->na) {
			if (u->na->status) {
				return (u->na->status & (NS_IDENTIFIED | NS_RECOGNIZED));
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}
	return 0;
}

/*************************************************************************/

/* Returns whether a user is identified AND in the group nc */

int group_identified(User * u, NickCore * nc)
{
	return nick_identified(u) && u->na->nc == nc;
}

/*************************************************************************/

/* Remove all nicks which have expired.  Also update last-seen time for all
 * nicks.
 */

void expire_nicks()
{
	int i;
	NickAlias *na, *next;
	time_t now = time(NULL);
	char *tmpnick;

	for (i = 0; i < 1024; i++) {
		for (na = nalists[i]; na; na = next) {
			next = na->next;

			if (na->u
				&& ((na->nc->flags & NI_SECURE) ? nick_identified(na->u) :
					nick_recognized(na->u))) {
				if (debug >= 2)
					alog("debug: NickServ: updating last seen time for %s",
						 na->nick);
				na->last_seen = now;
				continue;
			}

			if (NSExpire && now - na->last_seen >= NSExpire
				&& !(na->status & (NS_VERBOTEN | NS_NO_EXPIRE))
				&& !(na->nc->flags & (NI_SUSPENDED))) {
				alog("Expiring nickname %s (group: %s) (e-mail: %s)",
					 na->nick, na->nc->display,
					 (na->nc->email ? na->nc->email : "none"));
				tmpnick = sstrdup(na->nick);
				delnick(na);
				send_event(EVENT_NICK_EXPIRE, 1, tmpnick);
				delete [] tmpnick;
			}
		}
	}
}

void expire_requests()
{
	int i;
	NickRequest *nr, *next;
	time_t now = time(NULL);
	for (i = 0; i < 1024; i++) {
		for (nr = nrlists[i]; nr; nr = next) {
			next = nr->next;
			if (NSRExpire && now - nr->requested >= NSRExpire) {
				alog("Request for nick %s expiring", nr->nick);
				delnickrequest(nr);
			}
		}
	}
}

/*************************************************************************/
/*************************************************************************/
/* Return the NickRequest structire for the given nick, or NULL */

NickRequest *findrequestnick(const char *nick)
{
	NickRequest *nr;

	if (!*nick || !nick) {
		if (debug) {
			alog("debug: findrequestnick() called with NULL values");
		}
		return NULL;
	}

	for (nr = nrlists[HASH(nick)]; nr; nr = nr->next) {
		if (stricmp(nr->nick, nick) == 0)
			return nr;
	}
	return NULL;
}

/* Return the NickAlias structure for the given nick, or NULL if the nick
 * isn't registered. */

NickAlias *findnick(const char *nick)
{
	NickAlias *na;

	if (!nick || !*nick) {
		if (debug) {
			alog("debug: findnick() called with NULL values");
		}
		return NULL;
	}

	for (na = nalists[HASH(nick)]; na; na = na->next) {
		if (stricmp(na->nick, nick) == 0)
			return na;
	}

	return NULL;
}

NickAlias *findnick(const std::string &nick)
{
	return findnick(nick.c_str());
}

/*************************************************************************/

/* Return the NickCore structure for the given nick, or NULL if the core
 * doesn't exist. */

NickCore *findcore(const char *nick)
{
	NickCore *nc;

	if (!nick || !*nick) {
		if (debug) {
			alog("debug: findcore() called with NULL values");
		}
		return NULL;
	}

	for (nc = nclists[HASH(nick)]; nc; nc = nc->next) {
		if (stricmp(nc->display, nick) == 0)
			return nc;
	}

	return NULL;
}

/*************************************************************************/
/*********************** NickServ private routines ***********************/
/*************************************************************************/

/* Is the given user's address on the given nick's access list?  Return 1
 * if so, 0 if not. */

int is_on_access(User * u, NickCore * nc)
{
	int i;
	char *buf;
	char *buf2 = NULL;

	if (nc->accesscount == 0)
		return 0;

	buf = new char[strlen(u->username) + strlen(u->host) + 2];
	sprintf(buf, "%s@%s", u->username, u->host);
	if (ircd->vhost) {
		if (u->vhost) {
			buf2 = new char[strlen(u->username) + strlen(u->vhost) + 2];
			sprintf(buf2, "%s@%s", u->username, u->vhost);
		}
	}

	for (i = 0; i < nc->accesscount; i++) {
		if (match_wild_nocase(nc->access[i], buf)
			|| (ircd->vhost ? match_wild_nocase(nc->access[i], buf2) : 0)) {
			delete [] buf;
			if (ircd->vhost) {
				if (u->vhost) {
					delete [] buf2;
				}
			}
			return 1;
		}
	}
	delete [] buf;
	if (ircd->vhost) {
		delete [] buf2;
	}
	return 0;
}

/*************************************************************************/

/* Insert a nick alias alphabetically into the database. */

void alpha_insert_alias(NickAlias * na)
{
	NickAlias *ptr, *prev;
	char *nick;
	int index;

	if (!na) {
		if (debug) {
			alog("debug: alpha_insert_alias called with NULL values");
		}
		return;
	}

	nick = na->nick;
	index = HASH(nick);

	for (prev = NULL, ptr = nalists[index];
		 ptr && stricmp(ptr->nick, nick) < 0; prev = ptr, ptr = ptr->next);
	na->prev = prev;
	na->next = ptr;
	if (!prev)
		nalists[index] = na;
	else
		prev->next = na;
	if (ptr)
		ptr->prev = na;
}

/*************************************************************************/

/* Insert a nick core into the database. */

void insert_core(NickCore * nc)
{
	int index;

	if (!nc) {
		if (debug) {
			alog("debug: insert_core called with NULL values");
		}
		return;
	}

	index = HASH(nc->display);

	nc->prev = NULL;
	nc->next = nclists[index];
	if (nc->next)
		nc->next->prev = nc;
	nclists[index] = nc;
}

/*************************************************************************/
void insert_requestnick(NickRequest * nr)
{
	int index = HASH(nr->nick);
	if (!nr) {
		if (debug) {
			alog("debug: insert_requestnick called with NULL values");
		}
		return;
	}


	nr->prev = NULL;
	nr->next = nrlists[index];
	if (nr->next)
		nr->next->prev = nr;
	nrlists[index] = nr;
}

/*************************************************************************/

/* Sets nc->display to newdisplay. If newdisplay is NULL, it will change
 * it to the first alias in the list.
 */

void change_core_display(NickCore * nc, char *newdisplay)
{
	if (!newdisplay) {
		NickAlias *na;

		if (nc->aliases.count <= 0)
			return;

		na = static_cast<NickAlias *>(nc->aliases.list[0]);
		newdisplay = na->nick;
	}

	/* Log ... */
	alog("%s: changing %s nickname group display to %s", s_NickServ,
		 nc->display, newdisplay);

	/* Remove the core from the list */
	if (nc->next)
		nc->next->prev = nc->prev;
	if (nc->prev)
		nc->prev->next = nc->next;
	else
		nclists[HASH(nc->display)] = nc->next;

	delete [] nc->display;
	nc->display = sstrdup(newdisplay);
	insert_core(nc);

}

/*************************************************************************/

/* Deletes the core. This must be called only when there is no more
 * aliases for it, because no cleanup is done.
 * This function removes all references to the core as well.
 */

static int delcore(NickCore * nc)
{
	int i;
	/* (Hopefully complete) cleanup */
	cs_remove_nick(nc);
	os_remove_nick(nc);

	/* Remove the core from the list */
	if (nc->next)
		nc->next->prev = nc->prev;
	if (nc->prev)
		nc->prev->next = nc->next;
	else
		nclists[HASH(nc->display)] = nc->next;

	/* Log .. */
	alog("%s: deleting nickname group %s", s_NickServ, nc->display);

	/* Now we can safely free it. */
	delete [] nc->display;

	if (nc->email)
		delete [] nc->email;
	if (nc->greet)
		delete [] nc->greet;
	if (nc->url)
		delete [] nc->url;

	if (nc->access) {
		for (i = 0; i < nc->accesscount; i++) {
			if (nc->access[i])
				delete [] nc->access[i];
		}
		free(nc->access);
	}

	if (!nc->memos.memos.empty()) {
		for (i = 0; i < nc->memos.memos.size(); ++i) {
			if (nc->memos.memos[i]->text)
				delete [] nc->memos.memos[i]->text;
			delete nc->memos.memos[i];
		}
		nc->memos.memos.clear();
	}

	delete nc;
	return 1;
}


/*************************************************************************/
int delnickrequest(NickRequest * nr)
{
	if (nr) {
		nrlists[HASH(nr->nick)] = nr->next;
		if (nr->nick)
			delete [] nr->nick;
		if (nr->passcode)
			delete [] nr->passcode;
		if (nr->email)
			delete [] nr->email;
		delete nr;
	}

	return 0;
}

/*************************************************************************/

/* Deletes an alias. The core will also be deleted if it has no more
 * nicks attached to it. Easy but powerful.
 * Well, we must also take care that the nick being deleted is not
 * the core display, and if so, change it to the next alias in the list,
 * otherwise weird things will happen.
 * Returns 1 on success, 0 otherwise.
 */

int delnick(NickAlias * na)
{
	/* First thing to do: remove any timeout belonging to the nick we're deleting */
	clean_ns_timeouts(na);

	/* Second thing to do: look for an user using the alias
	 * being deleted, and make appropriate changes */

	if (na->u) {
		na->u->na = NULL;

		if (ircd->modeonunreg)
			common_svsmode(na->u, ircd->modeonunreg, "1");

	}

	delHostCore(na->nick);	  /* delete any vHost's for this nick */

	/* Accept nicks that have no core, because of database load functions */
	if (na->nc) {
		/* Next: see if our core is still useful. */
		slist_remove(&na->nc->aliases, na);
		if (na->nc->aliases.count == 0) {
			if (!delcore(na->nc))
				return 0;
			na->nc = NULL;
		} else {
			/* Display updating stuff */
			if (!stricmp(na->nick, na->nc->display))
				change_core_display(na->nc, NULL);
		}
	}

	/* Remove us from the aliases list */
	if (na->next)
		na->next->prev = na->prev;
	if (na->prev)
		na->prev->next = na->next;
	else
		nalists[HASH(na->nick)] = na->next;

	delete [] na->nick;
	if (na->last_usermask)
		delete [] na->last_usermask;
	if (na->last_realname)
		delete [] na->last_realname;
	if (na->last_quit)
		delete [] na->last_quit;

	delete na;
	return 1;
}

/*************************************************************************/
/*************************************************************************/

/* Collide a nick.
 *
 * When connected to a network using DALnet servers, version 4.4.15 and above,
 * Services is now able to force a nick change instead of killing the user.
 * The new nick takes the form "Guest######". If a nick change is forced, we
 * do not introduce the enforcer nick until the user's nick actually changes.
 * This is watched for and done in cancel_user(). -TheShadow
 */

void collide(NickAlias * na, int from_timeout)
{
	char guestnick[NICKMAX];

	if (!from_timeout)
		del_ns_timeout(na, TO_COLLIDE);

	/* Old system was unsure since there can be more than one collide
	 * per second. So let use another safer method.
	 *		  --lara
	 */
	/* So you should check the length of NSGuestNickPrefix, eh Lara?
	 *		  --Certus
	 */

	if (ircd->svsnick) {
		/* We need to make sure the guestnick is free -- heinz */
		do {
			snprintf(guestnick, sizeof(guestnick), "%s%d",
					 NSGuestNickPrefix, getrandom16());
		} while (finduser(guestnick));
		notice_lang(s_NickServ, na->u, FORCENICKCHANGE_CHANGING,
					guestnick);
		ircdproto->SendForceNickChange(na->nick, guestnick, time(NULL));
		na->status |= NS_GUESTED;
	} else {
		kill_user(s_NickServ, na->nick, "Services nickname-enforcer kill");
	}
}

/*************************************************************************/

/* Release hold on a nick. */

void release(NickAlias * na, int from_timeout)
{
	if (!from_timeout)
		del_ns_timeout(na, TO_RELEASE);
	if (ircd->svshold) {
		ircdproto->SendSVSHoldDel(na->nick);
	} else {
		ircdproto->SendQuit(na->nick, NULL);
	}
	na->status &= ~NS_KILL_HELD;
}

/*************************************************************************/
/*************************************************************************/

static struct my_timeout {
	struct my_timeout *next, *prev;
	NickAlias *na;
	Timeout *to;
	int type;
} *my_timeouts;

/*************************************************************************/

/* Remove a collide/release timeout from our private list. */

static void rem_ns_timeout(NickAlias * na, int type)
{
	struct my_timeout *t, *t2;

	t = my_timeouts;
	while (t) {
		if (t->na == na && t->type == type) {
			t2 = t->next;
			if (t->next)
				t->next->prev = t->prev;
			if (t->prev)
				t->prev->next = t->next;
			else
				my_timeouts = t->next;
			delete t;
			t = t2;
		} else {
			t = t->next;
		}
	}
}

/*************************************************************************/

/* Collide a nick on timeout. */

static void timeout_collide(Timeout * t)
{
	NickAlias *na = static_cast<NickAlias *>(t->data);

	rem_ns_timeout(na, TO_COLLIDE);
	/* If they identified or don't exist anymore, don't kill them. */
	if ((na->status & NS_IDENTIFIED) || !na->u
		|| na->u->my_signon > t->settime)
		return;
	/* The RELEASE timeout will always add to the beginning of the
	 * list, so we won't see it.  Which is fine because it can't be
	 * triggered yet anyway. */
	collide(na, 1);
}

/*************************************************************************/

/* Release a nick on timeout. */

static void timeout_release(Timeout * t)
{
	NickAlias *na = static_cast<NickAlias *>(t->data);

	rem_ns_timeout(na, TO_RELEASE);
	release(na, 1);
}

/*************************************************************************/

/* Add a collide/release timeout. */

static void add_ns_timeout(NickAlias * na, int type, time_t delay)
{
	Timeout *to;
	struct my_timeout *t;
	void (*timeout_routine) (Timeout *);

	if (type == TO_COLLIDE)
		timeout_routine = timeout_collide;
	else if (type == TO_RELEASE)
		timeout_routine = timeout_release;
	else {
		alog("NickServ: unknown timeout type %d! na=0x%p (%s), delay=%ld",
			 type, static_cast<void *>(na), na->nick, static_cast<long>(delay));
		return;
	}

	to = add_timeout(delay, timeout_routine, 0);
	to->data = na;

	t = new my_timeout;
	t->na = na;
	t->to = to;
	t->type = type;

	t->prev = NULL;
	t->next = my_timeouts;
	my_timeouts = t;
	/* Andy Church should stop coding while being drunk.
	 * Here's the two lines he forgot that produced the timed_update evil bug
	 * and a *big* memory leak.
	 */
	if (t->next)
		t->next->prev = t;
}

/*************************************************************************/

/* Delete a collide/release timeout. */

void del_ns_timeout(NickAlias * na, int type)
{
	struct my_timeout *t, *t2;

	t = my_timeouts;
	while (t) {
		if (t->na == na && t->type == type) {
			t2 = t->next;
			if (t->next)
				t->next->prev = t->prev;
			if (t->prev)
				t->prev->next = t->next;
			else
				my_timeouts = t->next;
			del_timeout(t->to);
			delete t;
			t = t2;
		} else {
			t = t->next;
		}
	}
}

/*************************************************************************/

/* Deletes all timeouts belonging to a given nick.
 * This should only be called before nick deletion.
 */

void clean_ns_timeouts(NickAlias * na)
{
	struct my_timeout *t, *next;

	for (t = my_timeouts; t; t = next) {
		next = t->next;
		if (t->na == na) {
			if (debug)
				alog("debug: %s deleting timeout type %d from %s",
					 s_NickServ, t->type, t->na->nick);
			/* If the timeout has the TO_RELEASE type, we should release the user */
			if (t->type == TO_RELEASE)
				release(na, 1);
			if (t->next)
				t->next->prev = t->prev;
			if (t->prev)
				t->prev->next = t->next;
			else
				my_timeouts = t->next;
			del_timeout(t->to);
			delete t;
		}
	}
}

/*************************************************************************/
/*********************** NickServ command routines ***********************/
/*************************************************************************/


/* We don't use this function but we keep it for module coders -certus */
int should_mode_change(int16 status, int16 mode)
{
	switch (mode) {
	case CUS_OP:
		if (status & CUS_OP) {
			return 0;
		}
		break;
	case CUS_VOICE:
		if (status & CUS_OP) {
			return 0;
		}
		if (status & CUS_HALFOP) {
			return 0;
		}
		if (status & CUS_VOICE) {
			return 0;
		}
		return 1;
		break;
	case CUS_HALFOP:
		if (status & CUS_OP) {
			return 0;
		}
		if (status & CUS_HALFOP) {
			return 0;
		}
		return 1;
		break;
	case CUS_OWNER:
		if (ircd->owner) {
			if (status & CUS_OWNER) {
				return 0;
			}
		}
		break;
	case CUS_PROTECT:
		if (ircd->protect) {
			if (status & CUS_PROTECT) {
				return 0;
			}
		}
		break;
	}
	return 1;
}

/*************************************************************************/

int do_setmodes(User * u)
{
	struct u_chanlist *uc;
	Channel *c;

	/* Walk users current channels */
	for (uc = u->chans; uc; uc = uc->next) {
		if ((c = uc->chan))
			chan_set_correct_modes(u, c, 1);
	}
	return MOD_CONT;
}

/*************************************************************************/
/*
 * Nick tracking
 */

/**
 * Start Nick tracking and store the nick core display under the user struct.
 * @param u The user to track nicks for
 **/
void nsStartNickTracking(User * u)
{
	NickCore *nc;

	/* We only track identified users */
	if (nick_identified(u)) {
		nc = u->na->nc;

		/* Release memory if needed */
		if (u->nickTrack)
			delete [] u->nickTrack;

		/* Copy the nick core displayed nick to
		   the user structure for further checks */
		u->nickTrack = sstrdup(nc->display);
	}
}

/**
 * Stop Nick tracking and remove the nick core display under the user struct.
 * @param u The user to stop tracking for
 **/
void nsStopNickTracking(User * u)
{
	/* Simple enough. If its there, release it */
	if (u->nickTrack) {
		delete [] u->nickTrack;
		u->nickTrack = NULL;
	}
}

/**
 * Boolean function to check if the user requesting a nick has the tracking
 * signature of that core in its structure.
 * @param u The user whom to check tracking for
 **/
int nsCheckNickTracking(User * u)
{
	NickCore *nc;
	NickAlias *na;
	char *nick;

	/* No nick alias or nick return false by default */
	if ((!(na = u->na)) || (!(nick = na->nick))) {
		return 0;
	}

	/* nick is forbidden best return 0 */
	if (na->status & NS_VERBOTEN) {
		return 0;
	}

	/* Get the core for the requested nick */
	nc = na->nc;

	/* If the core and the tracking displayed nick are there,
	 * and they match, return true
	 */
	if (nc && u->nickTrack && (strcmp(nc->display, u->nickTrack) == 0))
		return 1;
	else
		return 0;
}
