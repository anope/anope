
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

static std::map<NickAlias *, NickServCollide *> NickServCollides;
static std::map<NickAlias *, NickServRelease *> NickServReleases;

NickServCollide::NickServCollide(NickAlias *nickalias, time_t delay) : Timer(delay), na(nickalias)
{
	/* Erase the current collide and use the new one */
	std::map<NickAlias *, NickServCollide *>::iterator nit = NickServCollides.find(nickalias);
	if (nit != NickServCollides.end())
	{
		TimerManager::DelTimer(nit->second);
	}

	it = NickServCollides.insert(std::make_pair(nickalias, this));
}

NickServCollide::~NickServCollide()
{
	if (it.second)
	{
		NickServCollides.erase(it.first);
	}
}

void NickServCollide::Tick(time_t ctime)
{
	/* If they identified or don't exist anymore, don't kill them. */
	User *u = finduser(na->nick);
	if (!u || u->nc == na->nc || u->my_signon > this->GetSetTime())
		return;

	/* The RELEASE timeout will always add to the beginning of the
	 * list, so we won't see it.  Which is fine because it can't be
	 * triggered yet anyway. */
	collide(na, 1);
}

void NickServCollide::ClearTimers(NickAlias *na)
{
	std::map<NickAlias *, NickServCollide *>::iterator i = NickServCollides.find(na);
	NickServCollide *t;

	if (i != NickServCollides.end())
	{
		t = i->second;

		TimerManager::DelTimer(t);
	}
}

NickServRelease::NickServRelease(NickAlias *nickalias, time_t delay) : Timer(delay), na(nickalias)
{
	/* Erase the current release timer and use the new one */
	std::map<NickAlias *, NickServRelease *>::iterator nit = NickServReleases.find(nickalias);
	if (nit != NickServReleases.end())
	{
		TimerManager::DelTimer(nit->second);
	}

	it = NickServReleases.insert(std::make_pair(nickalias, this));
}

NickServRelease::~NickServRelease()
{
	if (it.second)
	{
		NickServReleases.erase(it.first);
	}
}

void NickServRelease::Tick(time_t ctime)
{
	if (ircd->svshold)
	{
		ircdproto->SendSVSHoldDel(na->nick);
	}
	else
	{
		if (ircd->ts6 && !uid.empty())
			ircdproto->SendQuit(uid.c_str(), NULL);
		else
			ircdproto->SendQuit(na->nick, NULL);
	}
	na->UnsetFlag(NS_KILL_HELD);
}

void NickServRelease::ClearTimers(NickAlias *na, bool dorelease)
{
	std::map<NickAlias *, NickServRelease *>::iterator i = NickServReleases.find(na);
	NickServRelease *t;

	if (i != NickServReleases.end())
	{
		t = i->second;

		if (dorelease)
			release(na, 1);

		TimerManager::DelTimer(t);
	}
}

/*************************************************************************/
/* *INDENT-OFF* */
void moduleAddNickServCmds()
{
	ModuleManager::LoadModuleList(Config.NickServCoreModules);
}
/* *INDENT-ON* */
/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_aliases_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;
	int i;
	NickAlias *na;

	for (i = 0; i < 1024; i++)
	{
		for (na = nalists[i]; na; na = na->next)
		{
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
	unsigned i, j;
	NickCore *nc;

	for (i = 0; i < 1024; i++)
	{
		for (nc = nclists[i]; nc; nc = nc->next)
		{
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

			mem += sizeof(std::string) * nc->access.size();
			for (j = 0; j < nc->access.size(); ++j)
				mem += nc->GetAccess(j).length() + 1;

			mem += nc->memos.memos.size() * sizeof(Memo);
			for (j = 0; j < nc->memos.memos.size(); j++)
			{
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

	if (!cmd)
	{
		return;
	}
	else if (stricmp(cmd, "\1PING") == 0)
	{
		if (!(s = strtok(NULL, "")))
		{
			s = "";
		}
		ircdproto->SendCTCP(findbot(Config.s_NickServ), u->nick, "PING %s", s);
	}
	else
	{
		mod_run_cmd(Config.s_NickServ, u, NICKSERV, cmd);
	}

}

/*************************************************************************/

/* Load/save data files. */


#define SAFE(x) do {					\
	if ((x) < 0) {					\
	if (!forceload)					\
		fatal("Read error on %s", Config.NickDBName);	\
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

	if (!(f = open_db(Config.s_NickServ, Config.PreNickDBName, "r", PRE_NICK_VERSION)))
		return;
	ver = get_file_version(f);
	for (i = 0; i < 1024 && !failed; i++)
	{
		while ((c = getc_db(f)) == 1)
		{
			if (c != 1)
				fatal("Invalid format in %s", Config.PreNickDBName);

			char *s;
			SAFE(read_string(&s, f));
			nr = new NickRequest(s);
			delete [] s;

			SAFE(read_string(&nr->passcode, f));
			if (ver < 2)
			{
				SAFE(read_string(&pass, f));
				len = strlen(pass);
				enc_encrypt(pass, len, nr->password, PASSMAX);
				memset(pass, 0, len);
				delete [] pass;
			}
			else
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
	int ver, c;
	unsigned i, j;
	NickAlias *na, **nalast, *naprev;
	NickCore *nc, **nclast, *ncprev;
	int failed = 0;
	uint16 tmp16;
	uint32 tmp32;
	char *s;

	if (!(f = open_db(Config.s_NickServ, Config.NickDBName, "r", NICK_VERSION)))
		return;

	ver = get_file_version(f);

	if (ver != 14)
	{
		close_db(f);
		fatal("DB %s is too old", Config.NickDBName);
		return;
	}

	/* First we load nick cores */
	for (i = 0; i < 1024 && !failed; i++)
	{
		nclast = &nclists[i];
		ncprev = NULL;

		while ((c = getc_db(f)) == 1)
		{
			if (c != 1)
				fatal("Invalid format in %s", Config.NickDBName);

			SAFE(read_string(&s, f));
			nc = new NickCore(s);
			delete [] s;
			*nclast = nc;
			nclast = &nc->next;
			nc->prev = ncprev;
			ncprev = nc;

			slist_init(&nc->aliases);

			SAFE(read_buffer(nc->pass, f));

			SAFE(read_string(&nc->email, f));
			SAFE(read_string(&nc->greet, f));
			SAFE(read_int32(&nc->icq, f));
			SAFE(read_string(&nc->url, f));

			//SAFE(read_int32(&nc->flags, f));
			SAFE(read_int32(&tmp32, f));
			if (!Config.NSAllowKillImmed)
				nc->UnsetFlag(NI_KILL_IMMED);
			SAFE(read_int16(&nc->language, f));

			uint16 accesscount;
			SAFE(read_int16(&accesscount, f));
			if (accesscount)
			{
				for (j = 0; j < accesscount; ++j)
				{
					char *access;
					SAFE(read_string(&access, f));
					if (access)
					{
						nc->AddAccess(access);
						delete [] access;
					}
				}
			}

			SAFE(read_int16(&tmp16, f));
			if (tmp16) nc->memos.memos.resize(tmp16);
			SAFE(read_int16(&tmp16, f));
			nc->memos.memomax = static_cast<int16>(tmp16);
			if (!nc->memos.memos.empty())
			{
				for (j = 0; j < nc->memos.memos.size(); j++)
				{
					nc->memos.memos[j] = new Memo;
					Memo *memo = nc->memos.memos[j];
					SAFE(read_int32(&memo->number, f));
					//SAFE(read_int16(&memo->flags, f));
					SAFE(read_int16(&tmp16, f));
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

	for (i = 0; i < 1024 && !failed; i++)
	{
		nalast = &nalists[i];
		naprev = NULL;
		while ((c = getc_db(f)) == 1)
		{
			if (c != 1)
				fatal("Invalid format in %s", Config.NickDBName);

			SAFE(read_string(&s, f));
			na = new NickAlias(s, nclists[0]); // XXXXXXXX
			delete [] s;

			SAFE(read_string(&na->last_usermask, f));
			SAFE(read_string(&na->last_realname, f));
			SAFE(read_string(&na->last_quit, f));

			SAFE(read_int32(&tmp32, f));
			na->time_registered = tmp32;
			SAFE(read_int32(&tmp32, f));
			na->last_seen = tmp32;
			//SAFE(read_int16(&na->status, f));
			SAFE(read_int16(&tmp16, f));
			//na->UnsetFlag(NS_TEMPORARY);

			SAFE(read_string(&s, f));
			na->nc = findcore(s);
			delete [] s;

			slist_add(&na->nc->aliases, na);

			if (!na->HasFlag(NS_FORBIDDEN))
			{
				if (!na->last_usermask)
					na->last_usermask = sstrdup("");
				if (!na->last_realname)
					na->last_realname = sstrdup("");
			}

			*nalast = na;
			nalast = &na->next;
			na->prev = naprev;
			naprev = na;

		}					   /* while (getc_db(f) != 0) */

		*nalast = NULL;
	}						   /* for (i) */

	close_db(f);

	for (i = 0; i < 1024; i++)
	{
		NickAlias *next;

		for (na = nalists[i]; na; na = next)
		{
			next = na->next;
			/* We check for coreless nicks (although it should never happen) */
			if (!na->nc)
			{
				alog("%s: while loading database: %s has no core! We delete it.", Config.s_NickServ, na->nick);
				delete na;
				continue;
			}

		}
	}
}

#undef SAFE

/*************************************************************************/

#define SAFE(x) do {						\
	if ((x) < 0) {						\
	restore_db(f);						\
	log_perror("Write error on %s", Config.NickDBName);		\
	if (time(NULL) - lastwarn > Config.WarningTimeout) {		\
		ircdproto->SendGlobops(NULL, "Write error on %s: %s", Config.NickDBName,	\
			strerror(errno));			\
		lastwarn = time(NULL);				\
	}							\
	return;							\
	}								\
} while (0)



void save_ns_dbase()
{
	dbFILE *f;
	unsigned i, j;
	NickAlias *na;
	NickCore *nc;
	static time_t lastwarn = 0;

	if (!(f = open_db(Config.s_NickServ, Config.NickDBName, "w", NICK_VERSION)))
		return;

	for (i = 0; i < 1024; i++)
	{
		for (nc = nclists[i]; nc; nc = nc->next)
		{
			SAFE(write_int8(1, f));

			SAFE(write_string(nc->display, f));
			SAFE(write_buffer(nc->pass, f));

			SAFE(write_string(nc->email, f));
			SAFE(write_string(nc->greet, f));
			SAFE(write_int32(nc->icq, f));
			SAFE(write_string(nc->url, f));

			//SAFE(write_int32(nc->flags, f));
			SAFE(write_int32(0, f));
			SAFE(write_int16(nc->language, f));

			SAFE(write_int16(nc->access.size(), f));
			for (j = 0; j < nc->access.size(); ++j)
			{
				std::string access = nc->GetAccess(j);
				SAFE(write_string(access.c_str(), f));
			}

			SAFE(write_int16(nc->memos.memos.size(), f));
			SAFE(write_int16(nc->memos.memomax, f));
			for (j = 0; j < nc->memos.memos.size(); j++)
			{
				Memo *memo = nc->memos.memos[j];
				SAFE(write_int32(memo->number, f));
				//SAFE(write_int16(memo->flags, f));
				SAFE(write_int16(0, f));
				SAFE(write_int32(memo->time, f));
				SAFE(write_buffer(memo->sender, f));
				SAFE(write_string(memo->text, f));
			}

			SAFE(write_int16(nc->channelcount, f));
			SAFE(write_int16(nc->channelcount, f)); // write this twice to avoid having to revbump the NickServ DB from anope 1.7, hack alert XXX

		}					   /* for (nc) */

		SAFE(write_int8(0, f));

	}						   /* for (i) */

	for (i = 0; i < 1024; i++)
	{
		for (na = nalists[i]; na; na = na->next)
		{
			SAFE(write_int8(1, f));

			SAFE(write_string(na->nick, f));

			SAFE(write_string(na->last_usermask, f));
			SAFE(write_string(na->last_realname, f));
			SAFE(write_string(na->last_quit, f));

			SAFE(write_int32(na->time_registered, f));
			SAFE(write_int32(na->last_seen, f));

			//SAFE(write_int16(na->status, f));
			SAFE(write_int16(0, f));

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

	if (!(f = open_db(Config.s_NickServ, Config.PreNickDBName, "w", PRE_NICK_VERSION)))
		return;

	for (i = 0; i < 1024; i++)
	{
		for (nr = nrlists[i]; nr; nr = nr->next)
		{
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
	NickServCollide *t;

	if ((nr = findrequestnick(u->nick)))
	{
		notice_lang(Config.s_NickServ, u, NICK_IS_PREREG);
	}

	if (!(na = findnick(u->nick)))
		return 0;

	if (na->HasFlag(NS_FORBIDDEN))
	{
		notice_lang(Config.s_NickServ, u, NICK_MAY_NOT_BE_USED);
		collide(na, 0);
		return 0;
	}

	if (na->nc->HasFlag(NI_SUSPENDED))
	{
		notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, u->nick);
		collide(na, 0);
		return 0;
	}

	if (!na->nc->HasFlag(NI_SECURE) && u->IsRecognized())
	{
		na->last_seen = time(NULL);
		if (na->last_usermask)
			delete [] na->last_usermask;
		std::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
		na->last_usermask = sstrdup(last_usermask.c_str());
		if (na->last_realname)
			delete [] na->last_realname;
		na->last_realname = sstrdup(u->realname);
		return 1;
	}

	if (u->IsRecognized() || !na->nc->HasFlag(NI_KILL_IMMED))
	{
		if (na->nc->HasFlag(NI_SECURE))
			notice_lang(Config.s_NickServ, u, NICK_IS_SECURE, Config.s_NickServ);
		else
			notice_lang(Config.s_NickServ, u, NICK_IS_REGISTERED, Config.s_NickServ);
	}

	if (na->nc->HasFlag(NI_KILLPROTECT) && !u->IsRecognized())
	{
		if (na->nc->HasFlag(NI_KILL_IMMED))
		{
			notice_lang(Config.s_NickServ, u, FORCENICKCHANGE_NOW);
			collide(na, 0);
		}
		else if (na->nc->HasFlag(NI_KILL_QUICK))
		{
			notice_lang(Config.s_NickServ, u, FORCENICKCHANGE_IN_20_SECONDS);
			t = new NickServCollide(na, 20);
		}
		else
		{
			notice_lang(Config.s_NickServ, u, FORCENICKCHANGE_IN_1_MINUTE);
			t = new NickServCollide(na, 60);
		}
	}

	return 0;
}

/*************************************************************************/

/* Cancel validation flags for a nick (i.e. when the user with that nick
 * signs off or changes nicks).  Also cancels any impending collide. */

void cancel_user(User * u)
{
	NickAlias *na = findnick(u->nick);
	NickServRelease *t;
	std::string uid;

	if (na)
	{
		if (na->HasFlag(NS_GUESTED))
		{
			if (ircd->svshold)
			{
				ircdproto->SendSVSHold(na->nick);
			}
			else if (ircd->svsnick)
			{
				uid = ts6_uid_retrieve();
				ircdproto->SendClientIntroduction(u->nick, Config.NSEnforcerUser, Config.NSEnforcerHost, "Services Enforcer", "+", uid.c_str());
				t = new NickServRelease(na, Config.NSReleaseTimeout);
				t->uid = uid;
			}
			else
			{
				ircdproto->SendSVSKill(Config.s_NickServ, u->nick, "Please do not use a registered nickname without identifying");
			}
			na->SetFlag(NS_KILL_HELD);
		}

		na->UnsetFlag(NS_KILL_HELD);
		na->UnsetFlag(NS_GUESTED);

		NickServCollide::ClearTimers(na);
	}
}

/*************************************************************************/

/* Return whether a user has identified for their nickname. */

int nick_identified(User * u)
{
	if (u->nc)
	{
		return 1;
	}

	return 0;
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

	for (i = 0; i < 1024; i++)
	{
		for (na = nalists[i]; na; na = next)
		{
			next = na->next;

			User *u = finduser(na->nick);
			if (u && (na->nc->HasFlag(NI_SECURE) ? nick_identified(u) : u->IsRecognized()))
			{
				if (debug >= 2)
					alog("debug: NickServ: updating last seen time for %s",
					     na->nick);
				na->last_seen = now;
				continue;
			}

			if (Config.NSExpire && now - na->last_seen >= Config.NSExpire
			        && !na->HasFlag(NS_FORBIDDEN) && !na->HasFlag(NS_NO_EXPIRE)
			        && !na->nc->HasFlag(NI_SUSPENDED))
			{
				EventReturn MOD_RESULT;
				FOREACH_RESULT(I_OnPreNickExpire, OnPreNickExpire(na));
				if (MOD_RESULT == EVENT_STOP)
					continue;

				alog("Expiring nickname %s (group: %s) (e-mail: %s)",
				     na->nick, na->nc->display,
				     (na->nc->email ? na->nc->email : "none"));
				tmpnick = sstrdup(na->nick);
				delete na;
				FOREACH_MOD(I_OnNickExpire, OnNickExpire(tmpnick));
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
	for (i = 0; i < 1024; i++)
	{
		for (nr = nrlists[i]; nr; nr = next)
		{
			next = nr->next;
			if (Config.NSRExpire && now - nr->requested >= Config.NSRExpire)
			{
				alog("Request for nick %s expiring", nr->nick);
				delete nr;
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

	if (!*nick || !nick)
	{
		if (debug)
		{
			alog("debug: findrequestnick() called with NULL values");
		}
		return NULL;
	}

	FOREACH_MOD(I_OnFindRequestNick, OnFindRequestNick(nick));

	for (nr = nrlists[HASH(nick)]; nr; nr = nr->next)
	{
		if (stricmp(nr->nick, nick) == 0)
		{
			return nr;
		}
	}
	return NULL;
}

/* Return the NickAlias structure for the given nick, or NULL if the nick
 * isn't registered. */

NickAlias *findnick(const char *nick)
{
	NickAlias *na;

	if (!nick || !*nick)
	{
		if (debug)
		{
			alog("debug: findnick() called with NULL values");
		}
		return NULL;
	}

	FOREACH_MOD(I_OnFindNick, OnFindNick(nick));

	for (na = nalists[HASH(nick)]; na; na = na->next)
	{
		if (stricmp(na->nick, nick) == 0)
		{
			return na;
		}
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

	if (!nick || !*nick)
	{
		if (debug)
		{
			alog("debug: findcore() called with NULL values");
		}
		return NULL;
	}

	FOREACH_MOD(I_OnFindCore, OnFindCore(nick));

	for (nc = nclists[HASH(nick)]; nc; nc = nc->next)
	{
		if (stricmp(nc->display, nick) == 0)
		{
			return nc;
		}
	}

	return NULL;
}

/*************************************************************************/
/*********************** NickServ private routines ***********************/
/*************************************************************************/

/** Is the user's address on the nickcores access list?
 * @param u The user
 * @param nc The nickcore
 * @return true or false
 */
bool is_on_access(User *u, NickCore *nc)
{
	unsigned i;
	char *buf;
	char *buf2 = NULL;
	char *buf3 = NULL;
	std::string tmp_buf;

	if (!u || !nc || nc->access.empty())
		return false;

	tmp_buf = u->GetIdent() + "@" + u->host;
	buf = sstrdup(tmp_buf.c_str());
	if (ircd->vhost)
	{
		if (u->vhost)
		{
			tmp_buf = u->GetIdent() + "@" + u->vhost;
			buf2 = sstrdup(tmp_buf.c_str());
		}
		if (!u->GetCloakedHost().empty())
		{
			tmp_buf = u->GetIdent() + "@" + u->GetCloakedHost();
			buf3 = sstrdup(tmp_buf.c_str());
		}
	}

	for (i = 0; i < nc->access.size(); i++)
	{
		std::string access = nc->GetAccess(i);
		if (Anope::Match(buf, access, false) || (buf2 && Anope::Match(buf2, access, false)) || (buf3 && Anope::Match(buf3, access, false)))
		{
			delete [] buf;
			if (ircd->vhost)
			{
				if (u->vhost)
				{
					delete [] buf2;
				}
				if (!u->GetCloakedHost().empty())
				{
					delete [] buf3;
				}
			}
			return true;
		}
	}
	delete [] buf;
	if (ircd->vhost)
	{
		if (buf2)
			delete [] buf2;
		if (buf3)
			delete [] buf3;
	}
	return false;
}

/*************************************************************************/

/* Insert a nick alias alphabetically into the database. */

void alpha_insert_alias(NickAlias * na)
{
	NickAlias *ptr, *prev;
	char *nick;
	int index;

	if (!na)
	{
		if (debug)
		{
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

	if (!nc)
	{
		if (debug)
		{
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
	if (!nr)
	{
		if (debug)
		{
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


void change_core_display(NickCore * nc, const char *newdisplay)
{
	/*
	       if (!newdisplay) {
			NickAlias *na;

			if (nc->aliases.count <= 0)
				return;

			na = static_cast<NickAlias *>(nc->aliases.list[0]);
			newdisplay = na->nick;
		}
	*/
	/* Log ... */
	FOREACH_MOD(I_OnChangeCoreDisplay, OnChangeCoreDisplay(nc, newdisplay));
	alog("%s: changing %s nickname group display to %s", Config.s_NickServ,
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

void change_core_display(NickCore * nc)
{
	NickAlias *na;
	if (nc->aliases.count <= 0)
		return;
	na = static_cast<NickAlias *>(nc->aliases.list[0]);
	change_core_display(nc,na->nick);
}


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
		NickServCollide::ClearTimers(na);

	/* Old system was unsure since there can be more than one collide
	 * per second. So let use another safer method.
	 *		  --lara
	 */
	/* So you should check the length of Config.NSGUestNickPrefix, eh Lara?
	 *		  --Certus
	 */

	if (ircd->svsnick)
	{
		/* We need to make sure the guestnick is free -- heinz */
		do
		{
			snprintf(guestnick, sizeof(guestnick), "%s%d",
			         Config.NSGuestNickPrefix, getrandom16());
		}
		while (finduser(guestnick));
		notice_lang(Config.s_NickServ, finduser(na->nick), FORCENICKCHANGE_CHANGING, guestnick);
		ircdproto->SendForceNickChange(na->nick, guestnick, time(NULL));
		na->SetFlag(NS_GUESTED);
	}
	else
	{
		kill_user(Config.s_NickServ, na->nick, "Services nickname-enforcer kill");
	}
}

/*************************************************************************/

/* Release hold on a nick. */

void release(NickAlias * na, int from_timeout)
{
	if (!from_timeout)
		NickServRelease::ClearTimers(na);

	if (ircd->svshold)
	{
		ircdproto->SendSVSHoldDel(na->nick);
	}
	else
	{
		ircdproto->SendQuit(na->nick, NULL);
	}
	na->UnsetFlag(NS_KILL_HELD);
}

/*************************************************************************/

void del_ns_timeout(NickAlias * na, int type)
{
	if (type == TO_COLLIDE)
		NickServCollide::ClearTimers(na);
	else if (type == TO_RELEASE)
		NickServRelease::ClearTimers(na);
}

/** Set the correct oper type for a nickcore
 * @param nc The nick core
 */
void SetOperType(NickCore *nc)
{
	for (std::list<std::pair<std::string, std::string> >::iterator it = Config.Opers.begin(); it != Config.Opers.end(); ++it)
	{
		std::string nick = it->first;
		std::string type = it->second;

		NickAlias *na = findnick(nick);

		if (!na)
		{
			/* Nonexistant nick */
			continue;
		}

		if (na->nc == nc)
		{
			 for (std::list<OperType *>::iterator tit = Config.MyOperTypes.begin(); tit != Config.MyOperTypes.end(); ++tit)
			 {
				 OperType *ot = *tit;

				 if (ot->GetName() == type)
				 {
					 nc->ot = ot;
					 alog("%s: Tied oper %s to type %s", Config.s_OperServ, nc->display, type.c_str());
				 }
			 }
		}
	}
}

/*************************************************************************/
/*********************** NickServ command routines ***********************/
/*************************************************************************/


/* We don't use this function but we keep it for module coders -certus */
int should_mode_change(int16 status, int16 mode)
{
	switch (mode)
	{
		case CUS_OP:
			if (status & CUS_OP)
			{
				return 0;
			}
			break;
		case CUS_VOICE:
			if (status & CUS_OP)
			{
				return 0;
			}
			if (status & CUS_HALFOP)
			{
				return 0;
			}
			if (status & CUS_VOICE)
			{
				return 0;
			}
			return 1;
			break;
		case CUS_HALFOP:
			if (status & CUS_OP)
			{
				return 0;
			}
			if (status & CUS_HALFOP)
			{
				return 0;
			}
			return 1;
			break;
		case CUS_OWNER:
			if (ModeManager::FindChannelModeByName(CMODE_OWNER))
			{
				if (status & CUS_OWNER)
				{
					return 0;
				}
			}
			break;
		case CUS_PROTECT:
			if (ModeManager::FindChannelModeByName(CMODE_PROTECT))
			{
				if (status & CUS_PROTECT)
				{
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
	for (uc = u->chans; uc; uc = uc->next)
	{
		if ((c = uc->chan))
			chan_set_correct_modes(u, c, 1);
	}
	return MOD_CONT;
}

