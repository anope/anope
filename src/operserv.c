/* OperServ functions.
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

/* List of Services administrators */
SList servadmins;
/* List of Services operators */
SList servopers;
/* AKILL, SGLINE, SQLINE and SZLINE lists */
SList akills, sglines, sqlines, szlines;

/*************************************************************************/

static int compare_adminlist_entries(SList * slist, void *item1,
									 void *item2);
static int compare_operlist_entries(SList * slist, void *item1,
									void *item2);
static void free_adminlist_entry(SList * slist, void *item);
static void free_operlist_entry(SList * slist, void *item);

static int is_akill_entry_equal(SList * slist, void *item1, void *item2);
static void free_akill_entry(SList * slist, void *item);
static int is_sgline_entry_equal(SList * slist, void *item1, void *item2);
static void free_sgline_entry(SList * slist, void *item);
static int is_sqline_entry_equal(SList * slist, void *item1, void *item2);
static void free_sqline_entry(SList * slist, void *item);
static int is_szline_entry_equal(SList * slist, void *item1, void *item2);
static void free_szline_entry(SList * slist, void *item);

time_t DefContimer;
int DefConModesSet = 0;
char *defconReverseModes(const char *modes);

uint32 DefConModesOn;		   /* Modes to be enabled during DefCon */
uint32 DefConModesOff;		  /* Modes to be disabled during DefCon */
ChannelInfo DefConModesCI;	  /* ChannelInfo containg params for locked modes
								 * during DefCon; I would've done this nicer if i
								 * could, but all damn mode functions require a
								 * ChannelInfo struct! --gdex
								 */


void moduleAddOperServCmds();
/*************************************************************************/

/* Options for the lists */
SListOpts akopts = { 0, NULL, &is_akill_entry_equal, &free_akill_entry };
SListOpts saopts = { SLISTF_SORT, &compare_adminlist_entries, NULL,
	&free_adminlist_entry
};

SListOpts sgopts = { 0, NULL, &is_sgline_entry_equal, &free_sgline_entry };
SListOpts soopts =
	{ SLISTF_SORT, &compare_operlist_entries, NULL, &free_operlist_entry };
SListOpts sqopts =
	{ SLISTF_SORT, NULL, &is_sqline_entry_equal, &free_sqline_entry };
SListOpts szopts = { 0, NULL, &is_szline_entry_equal, &free_szline_entry };

/*************************************************************************/
/* *INDENT-OFF* */
void moduleAddOperServCmds() {
	ModuleManager::LoadModuleList(OperServCoreNumber, OperServCoreModules);
}

/* *INDENT-ON* */
/*************************************************************************/
/*************************************************************************/

/* OperServ initialization. */

void os_init()
{
	moduleAddOperServCmds();

	/* Initialization of the lists */
	slist_init(&servadmins);
	servadmins.opts = &saopts;
	slist_init(&servopers);
	servopers.opts = &soopts;

	slist_init(&akills);
	akills.opts = &akopts;

	if (ircd->sgline) {
		slist_init(&sglines);
		sglines.opts = &sgopts;
	}
	if (ircd->sqline) {
		slist_init(&sqlines);
		sqlines.opts = &sqopts;
	}
	if (ircd->szline) {
		slist_init(&szlines);
		szlines.opts = &szopts;
	}
}

/*************************************************************************/

/* Main OperServ routine. */

void operserv(User * u, char *buf)
{
	const char *cmd;
	const char *s;

	alog("%s: %s: %s", s_OperServ, u->nick, buf);

	cmd = strtok(buf, " ");
	if (!cmd) {
		return;
	} else if (stricmp(cmd, "\1PING") == 0) {
		if (!(s = strtok(NULL, ""))) {
			s = "";
		}
		ircdproto->SendCTCP(findbot(s_OperServ), u->nick, "PING %s", s);
	} else {
		mod_run_cmd(s_OperServ, u, OPERSERV, cmd);
	}
}

/*************************************************************************/
/**************************** Privilege checks ***************************/
/*************************************************************************/

/* Load OperServ data. */

#define SAFE(x) do {					\
	if ((x) < 0) {					\
	if (!forceload)					\
		fatal("Read error on %s", OperDBName);	\
	failed = 1;					\
	break;						\
	}							\
} while (0)

void load_os_dbase()
{
	dbFILE *f;
	int16 i, ver;
	uint16 tmp16;
	uint32 tmp32;
	int failed = 0;

	if (!(f = open_db(s_OperServ, OperDBName, "r", OPER_VERSION)))
		return;

	ver = get_file_version(f);
	if (ver != 13)
	{
		close_db(f);
		fatal("Read error on %s", ChanDBName);
		return;
	}

	SAFE(read_int32(&maxusercnt, f));
	SAFE(read_int32(&tmp32, f));
	maxusertime = tmp32;

	Akill *ak;

	read_int16(&tmp16, f);
	slist_setcapacity(&akills, tmp16);

	for (i = 0; i < akills.capacity; i++) {
		ak = new Akill;

		SAFE(read_string(&ak->user, f));
		SAFE(read_string(&ak->host, f));
		SAFE(read_string(&ak->by, f));
		SAFE(read_string(&ak->reason, f));
		SAFE(read_int32(&tmp32, f));
		ak->seton = tmp32;
		SAFE(read_int32(&tmp32, f));
		ak->expires = tmp32;

		slist_add(&akills, ak);
	}

	SXLine *sx;

	read_int16(&tmp16, f);
	slist_setcapacity(&sglines, tmp16);

	for (i = 0; i < sglines.capacity; i++) {
		sx = new SXLine;

		SAFE(read_string(&sx->mask, f));
		SAFE(read_string(&sx->by, f));
		SAFE(read_string(&sx->reason, f));
		SAFE(read_int32(&tmp32, f));
		sx->seton = tmp32;
		SAFE(read_int32(&tmp32, f));
		sx->expires = tmp32;

		slist_add(&sglines, sx);
	}

	read_int16(&tmp16, f);
	slist_setcapacity(&sqlines, tmp16);

	for (i = 0; i < sqlines.capacity; i++) {
		sx = new SXLine;

		SAFE(read_string(&sx->mask, f));
		SAFE(read_string(&sx->by, f));
		SAFE(read_string(&sx->reason, f));
		SAFE(read_int32(&tmp32, f));
		sx->seton = tmp32;
		SAFE(read_int32(&tmp32, f));
		sx->expires = tmp32;

		slist_add(&sqlines, sx);
	}

	read_int16(&tmp16, f);
	slist_setcapacity(&szlines, tmp16);

	for (i = 0; i < szlines.capacity; i++) {
		sx = new SXLine;

		SAFE(read_string(&sx->mask, f));
		SAFE(read_string(&sx->by, f));
		SAFE(read_string(&sx->reason, f));
		SAFE(read_int32(&tmp32, f));
		sx->seton = tmp32;
		SAFE(read_int32(&tmp32, f));
		sx->expires = tmp32;

		slist_add(&szlines, sx);
	}

	close_db(f);

}

#undef SAFE

/*************************************************************************/

/* Save OperServ data. */

#define SAFE(x) do {						\
	if ((x) < 0) {						\
	restore_db(f);						\
	log_perror("Write error on %s", OperDBName);		\
	if (time(NULL) - lastwarn > WarningTimeout) {		\
		ircdproto->SendGlobops(NULL, "Write error on %s: %s", OperDBName,	\
			strerror(errno));			\
		lastwarn = time(NULL);				\
	}							\
	return;							\
	}								\
} while (0)

void save_os_dbase()
{
	int i;
	dbFILE *f;
	static time_t lastwarn = 0;
	Akill *ak;
	SXLine *sx;

	if (!(f = open_db(s_OperServ, OperDBName, "w", OPER_VERSION)))
		return;
	SAFE(write_int32(maxusercnt, f));
	SAFE(write_int32(maxusertime, f));

	SAFE(write_int16(akills.count, f));
	for (i = 0; i < akills.count; i++) {
		ak = static_cast<Akill *>(akills.list[i]);

		SAFE(write_string(ak->user, f));
		SAFE(write_string(ak->host, f));
		SAFE(write_string(ak->by, f));
		SAFE(write_string(ak->reason, f));
		SAFE(write_int32(ak->seton, f));
		SAFE(write_int32(ak->expires, f));
	}

	SAFE(write_int16(sglines.count, f));
	for (i = 0; i < sglines.count; i++) {
		sx = static_cast<SXLine *>(sglines.list[i]);

		SAFE(write_string(sx->mask, f));
		SAFE(write_string(sx->by, f));
		SAFE(write_string(sx->reason, f));
		SAFE(write_int32(sx->seton, f));
		SAFE(write_int32(sx->expires, f));
	}

	SAFE(write_int16(sqlines.count, f));
	for (i = 0; i < sqlines.count; i++) {
		sx = static_cast<SXLine *>(sqlines.list[i]);

		SAFE(write_string(sx->mask, f));
		SAFE(write_string(sx->by, f));
		SAFE(write_string(sx->reason, f));
		SAFE(write_int32(sx->seton, f));
		SAFE(write_int32(sx->expires, f));
	}

	SAFE(write_int16(szlines.count, f));
	for (i = 0; i < szlines.count; i++) {
		sx = static_cast<SXLine *>(szlines.list[i]);

		SAFE(write_string(sx->mask, f));
		SAFE(write_string(sx->by, f));
		SAFE(write_string(sx->reason, f));
		SAFE(write_int32(sx->seton, f));
		SAFE(write_int32(sx->expires, f));
	}

	close_db(f);

}

#undef SAFE

/*************************************************************************/

/* Removes the nick structure from OperServ lists. */

void os_remove_nick(NickCore * nc)
{
	slist_remove(&servadmins, nc);
	slist_remove(&servopers, nc);
}

/*************************************************************************/

/* Does the given user have Services root privileges?
   Now enhanced. */

int is_services_root(User * u)
{
	if ((NSStrictPrivileges && !is_oper(u))
		|| (!nick_identified(u)))
		return 0;
	if ((u->nc->flags & NI_SERVICES_ROOT))
		return 1;
	return 0;
}

/*************************************************************************/

/* Does the given user have Services admin privileges? */

int is_services_admin(User * u)
{
	if ((NSStrictPrivileges && !is_oper(u))
		|| (!nick_identified(u)))
		return 0;
	if ((u->nc->flags & (NI_SERVICES_ADMIN | NI_SERVICES_ROOT)))
		return 1;
	return 0;
}

/*************************************************************************/

/* Does the given user have Services oper privileges? */

int is_services_oper(User * u)
{
	if ((NSStrictPrivileges && !is_oper(u))
		|| (!nick_identified(u)))
		return 0;
	if ((u->nc->
			flags & (NI_SERVICES_OPER | NI_SERVICES_ADMIN |
					 NI_SERVICES_ROOT)))
		return 1;
	return 0;
}

/*************************************************************************/

/* Is the given nick a Services root nick? */

int nick_is_services_root(NickCore * nc)
{
	if (nc) {
		if (nc->flags & (NI_SERVICES_ROOT))
			return 1;
	}
	return 0;
}

/*************************************************************************/

/* Is the given nick a Services admin/root nick? */

int nick_is_services_admin(NickCore * nc)
{
	if (nc) {
		if (nc->flags & (NI_SERVICES_ADMIN | NI_SERVICES_ROOT))
			return 1;
	}
	return 0;
}

/*************************************************************************/

/* Is the given nick a Services oper/admin/root nick? */

int nick_is_services_oper(NickCore * nc)
{
	if (nc) {
		if (nc->
			flags & (NI_SERVICES_OPER | NI_SERVICES_ADMIN |
					 NI_SERVICES_ROOT))
			return 1;
	}
	return 0;
}


/*************************************************************************/
/*********************** OperServ command functions **********************/
/*************************************************************************/

/*************************************************************************/


Server *server_global(Server * s, char *msg)
{
	Server *sl;

	while (s) {
		/* Do not send the notice to ourselves our juped servers */
		if (!(s->flags & (SERVER_ISME | SERVER_JUPED)))
			notice_server(s_GlobalNoticer, s, "%s", msg);

		if (s->links) {
			sl = server_global(s->links, msg);
			if (sl)
				s = sl;
			else
				s = s->next;
		} else {
			s = s->next;
		}
	}
	return s;

}

void oper_global(char *nick, const char *fmt, ...)
{
	va_list args;
	char msg[2048];			 /* largest valid message is 512, this should cover any global */
	char dmsg[2048];			/* largest valid message is 512, this should cover any global */

	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);

	/* I don't like the way this is coded... */
	if ((nick) && (!AnonymousGlobal)) {
		snprintf(dmsg, sizeof(dmsg), "[%s] %s", nick, msg);
		server_global(servlist, dmsg);
	} else {
		server_global(servlist, msg);
	}

}

/**************************************************************************/


/************************************************************************/
/*************************************************************************/

/* Adds an AKILL to the list. Returns >= 0 on success, -1 if it fails, -2
 * if only the expiry time was changed.
 * The success result is the number of AKILLs that were deleted to successfully add one.
 */

int add_akill(User * u, const char *mask, const char *by, const time_t expires,
			  const char *reason)
{
	int deleted = 0, i;
	char *user, *mask2, *host;
	Akill *entry;

	if (!mask) {
		return -1;
	}

	/* Checks whether there is an AKILL that already covers
	 * the one we want to add, and whether there are AKILLs
	 * that would be covered by this one. The masks AND the
	 * expiry times are used to determine this, because some
	 * AKILLs may become useful when another one expires.
	 * If so, warn the user in the first case and cleanup
	 * the useless AKILLs in the second.
	 */

	if (akills.count > 0) {

		for (i = akills.count - 1; i >= 0; i--) {
			char amask[BUFSIZE];

			entry = static_cast<Akill *>(akills.list[i]);

			if (!entry)
				continue;

			snprintf(amask, sizeof(amask), "%s@%s", entry->user,
					 entry->host);

			if (!stricmp(amask, mask)) {
				/* We change the AKILL expiry time if its current one is less than the new.
				 * This is preferable to be sure we don't change an important AKILL
				 * accidentely.
				 */
				if (entry->expires >= expires || entry->expires == 0) {
					if (u)
						notice_lang(s_OperServ, u, OPER_AKILL_EXISTS,
									mask);
					return -1;
				} else {
					entry->expires = expires;
					if (u)
						notice_lang(s_OperServ, u, OPER_AKILL_CHANGED,
									amask);
					return -2;
				}
			}

			if (Anope::Match(mask, amask, false)
				&& (entry->expires >= expires || entry->expires == 0)) {
				if (u)
					notice_lang(s_OperServ, u, OPER_AKILL_ALREADY_COVERED,
								mask, amask);
				return -1;
			}

			if (Anope::Match(amask, mask, false)
				&& (entry->expires <= expires || expires == 0)) {
				slist_delete(&akills, i);
				deleted++;
			}
		}

	}

	/* We can now check whether the list is full or not. */
	if (slist_full(&akills)) {
		if (u)
			notice_lang(s_OperServ, u, OPER_AKILL_REACHED_LIMIT,
						akills.limit);
		return -1;
	}

	/* We can now (really) add the AKILL. */
	mask2 = sstrdup(mask);
	host = strchr(mask2, '@');

	if (!host) {
		delete [] mask2;
		return -1;
	}

	user = mask2;
	*host = 0;
	host++;

	entry = new Akill;

	if (!entry) {
		delete [] mask2;
		return -1;
	}

	entry->user = sstrdup(user);
	entry->host = sstrdup(host);
	entry->by = sstrdup(by);
	entry->reason = sstrdup(reason);
	entry->seton = time(NULL);
	entry->expires = expires;

	slist_add(&akills, entry);

	if (AkillOnAdd)
		ircdproto->SendAkill(entry->user, entry->host, entry->by, entry->seton,
						entry->expires, entry->reason);

	delete [] mask2;

	return deleted;
}

/* Does the user match any AKILLs? */

int check_akill(const char *nick, const char *username, const char *host,
				const char *vhost, const char *ip)
{
	int i;
	Akill *ak;

	/**
	 * If DefCon is set to NO new users - kill the user ;).
	 **/
	if (checkDefCon(DEFCON_NO_NEW_CLIENTS)) {
		kill_user(s_OperServ, nick, DefConAkillReason);
		return 1;
	}

	if (akills.count == 0)
		return 0;

	for (i = 0; i < akills.count; i++) {
		ak = static_cast<Akill *>(akills.list[i]);
		if (!ak)
			continue;
		if (Anope::Match(username, ak->user, false)
			&& Anope::Match(host, ak->host, false)) {
			ircdproto->SendAkill(ak->user, ak->host, ak->by, ak->seton,
							ak->expires, ak->reason);
			return 1;
		}
		if (ircd->vhost) {
			if (vhost) {
				if (Anope::Match(username, ak->user, false)
					&& Anope::Match(vhost, ak->host, false)) {
					ircdproto->SendAkill(ak->user, ak->host, ak->by, ak->seton,
									ak->expires, ak->reason);
					return 1;
				}
			}
		}
		if (ircd->nickip) {
			if (ip) {
				if (Anope::Match(username, ak->user, false)
					&& Anope::Match(ip, ak->host, false)) {
					ircdproto->SendAkill(ak->user, ak->host, ak->by, ak->seton,
									ak->expires, ak->reason);
					return 1;
				}
			}
		}

	}

	return 0;
}

/* Delete any expired autokills. */

void expire_akills()
{
	int i;
	time_t now = time(NULL);
	Akill *ak;

	for (i = akills.count - 1; i >= 0; i--) {
		ak = static_cast<Akill *>(akills.list[i]);

		if (!ak->expires || ak->expires > now)
			continue;

		if (WallAkillExpire)
			ircdproto->SendGlobops(s_OperServ, "AKILL on %s@%s has expired",
							 ak->user, ak->host);
		slist_delete(&akills, i);
	}
}

static void free_akill_entry(SList * slist, void *item)
{
	Akill *ak = static_cast<Akill *>(item);

	/* Remove the AKILLs from all the servers */
	ircdproto->SendAkillDel(ak->user, ak->host);

	/* Free the structure */
	delete [] ak->user;
	delete [] ak->host;
	delete [] ak->by;
	delete [] ak->reason;
	delete ak;
}

/* item1 is not an Akill pointer, but a char
 */

static int is_akill_entry_equal(SList * slist, void *item1, void *item2)
{
	char *ak1 = static_cast<char *>(item1), buf[BUFSIZE];
	Akill *ak2 = static_cast<Akill *>(item2);

	if (!ak1 || !ak2)
		return 0;

	snprintf(buf, sizeof(buf), "%s@%s", ak2->user, ak2->host);

	if (!stricmp(ak1, buf))
		return 1;
	else
		return 0;
}


/*************************************************************************/

/* Adds an SGLINE to the list. Returns >= 0 on success, -1 if it failed, -2 if
 * only the expiry time changed.
 * The success result is the number of SGLINEs that were deleted to successfully add one.
 */

int add_sgline(User * u, const char *mask, const char *by, time_t expires,
			   const char *reason)
{
	int deleted = 0, i;
	SXLine *entry;
	User *u2, *next;
	char buf[BUFSIZE];
	*buf = '\0';

	/* Checks whether there is an SGLINE that already covers
	 * the one we want to add, and whether there are SGLINEs
	 * that would be covered by this one.
	 * If so, warn the user in the first case and cleanup
	 * the useless SGLINEs in the second.
	 */

	if (!mask) {
		return -1;
	}

	if (sglines.count > 0) {

		for (i = sglines.count - 1; i >= 0; i--) {
			entry = static_cast<SXLine *>(sglines.list[i]);

			if (!entry)
				continue;

			if (!stricmp(entry->mask, mask)) {
				if (entry->expires >= expires || entry->expires == 0) {
					if (u)
						notice_lang(s_OperServ, u, OPER_SGLINE_EXISTS,
									mask);
					return -1;
				} else {
					entry->expires = expires;
					if (u)
						notice_lang(s_OperServ, u, OPER_SGLINE_CHANGED,
									entry->mask);
					return -2;
				}
			}

			if (Anope::Match(mask, entry->mask, false )
				&& (entry->expires >= expires || entry->expires == 0)) {
				if (u)
					notice_lang(s_OperServ, u, OPER_SGLINE_ALREADY_COVERED,
								mask, entry->mask);
				return -1;
			}

			if (Anope::Match(entry->mask, mask, false)
				&& (entry->expires <= expires || expires == 0)) {
				slist_delete(&sglines, i);
				deleted++;
			}
		}

	}

	/* We can now check whether the list is full or not. */
	if (slist_full(&sglines)) {
		if (u)
			notice_lang(s_OperServ, u, OPER_SGLINE_REACHED_LIMIT,
						sglines.limit);
		return -1;
	}

	/* We can now (really) add the SGLINE. */
	entry = new SXLine;
	if (!entry)
		return -1;

	entry->mask = sstrdup(mask);
	entry->by = sstrdup(by);
	entry->reason = sstrdup(reason);
	entry->seton = time(NULL);
	entry->expires = expires;

	slist_add(&sglines, entry);

	ircdproto->SendSGLine(entry->mask, entry->reason);

	if (KillonSGline && !ircd->sglineenforce) {
		snprintf(buf, (BUFSIZE - 1), "G-Lined: %s", entry->reason);
		u2 = firstuser();
		while (u2) {
			next = nextuser();
			if (!is_oper(u2)) {
				if (Anope::Match(u2->realname, entry->mask, false)) {
					kill_user(ServerName, u2->nick, buf);
				}
			}
			u2 = next;
		}
	}
	return deleted;
}

/* Does the user match any SGLINEs? */

int check_sgline(const char *nick, const char *realname)
{
	int i;
	SXLine *sx;

	if (sglines.count == 0)
		return 0;

	for (i = 0; i < sglines.count; i++) {
		sx = static_cast<SXLine *>(sglines.list[i]);
		if (!sx)
			continue;

		if (Anope::Match(realname, sx->mask, false)) {
			ircdproto->SendSGLine(sx->mask, sx->reason);
			/* We kill nick since s_sgline can't */
			ircdproto->SendSVSKill(ServerName, nick, "G-Lined: %s", sx->reason);
			return 1;
		}
	}

	return 0;
}

/* Delete any expired SGLINEs. */

void expire_sglines()
{
	int i;
	time_t now = time(NULL);
	SXLine *sx;

	for (i = sglines.count - 1; i >= 0; i--) {
		sx = static_cast<SXLine *>(sglines.list[i]);

		if (!sx->expires || sx->expires > now)
			continue;

		if (WallSGLineExpire)
			ircdproto->SendGlobops(s_OperServ, "SGLINE on \2%s\2 has expired",
							 sx->mask);
		slist_delete(&sglines, i);
	}
}

static void free_sgline_entry(SList * slist, void *item)
{
	SXLine *sx = static_cast<SXLine *>(item);

	/* Remove the SGLINE from all the servers */
	ircdproto->SendSGLineDel(sx->mask);

	/* Free the structure */
	delete [] sx->mask;
	delete [] sx->by;
	delete [] sx->reason;
	delete sx;
}

/* item1 is not an SXLine pointer, but a char */

static int is_sgline_entry_equal(SList * slist, void *item1, void *item2)
{
	char *sx1 = static_cast<char *>(item1);
	SXLine *sx2 = static_cast<SXLine *>(item2);

	if (!sx1 || !sx2)
		return 0;

	if (!stricmp(sx1, sx2->mask))
		return 1;
	else
		return 0;
}

/*************************************************************************/

/* Adds an SQLINE to the list. Returns >= 0 on success, -1 if it failed, -2 if
 * only the expiry time changed.
 * The success result is the number of SQLINEs that were deleted to successfully add one.
 */

int add_sqline(User * u, const char *mask, const char *by, time_t expires,
			   const char *reason)
{
	int deleted = 0, i;
	User *u2, *next;
	SXLine *entry;
	char buf[BUFSIZE];
	*buf = '\0';

	/* Checks whether there is an SQLINE that already covers
	 * the one we want to add, and whether there are SQLINEs
	 * that would be covered by this one.
	 * If so, warn the user in the first case and cleanup
	 * the useless SQLINEs in the second.
	 */

	if (!mask) {
		return -1;
	}

	if (sqlines.count > 0) {

		for (i = sqlines.count - 1; i >= 0; i--) {
			entry = static_cast<SXLine *>(sqlines.list[i]);

			if (!entry)
				continue;

			if ((*mask == '#' && *entry->mask != '#') ||
				(*mask != '#' && *entry->mask == '#'))
				continue;

			if (!stricmp(entry->mask, mask)) {
				if (entry->expires >= expires || entry->expires == 0) {
					if (u)
						notice_lang(s_OperServ, u, OPER_SQLINE_EXISTS,
									mask);
					return -1;
				} else {
					entry->expires = expires;
					if (u)
						notice_lang(s_OperServ, u, OPER_SQLINE_CHANGED,
									entry->mask);
					return -2;
				}
			}

			if (Anope::Match(mask, entry->mask, false)
				&& (entry->expires >= expires || entry->expires == 0)) {
				if (u)
					notice_lang(s_OperServ, u, OPER_SQLINE_ALREADY_COVERED,
								mask, entry->mask);
				return -1;
			}

			if (Anope::Match(entry->mask, mask, false)
				&& (entry->expires <= expires || expires == 0)) {
				slist_delete(&sqlines, i);
				deleted++;
			}
		}

	}

	/* We can now check whether the list is full or not. */
	if (slist_full(&sqlines)) {
		if (u)
			notice_lang(s_OperServ, u, OPER_SQLINE_REACHED_LIMIT,
						sqlines.limit);
		return -1;
	}

	/* We can now (really) add the SQLINE. */
	entry = new SXLine;
	if (!entry)
		return -1;

	entry->mask = sstrdup(mask);
	entry->by = sstrdup(by);
	entry->reason = sstrdup(reason);
	entry->seton = time(NULL);
	entry->expires = expires;

	slist_add(&sqlines, entry);

	sqline(entry->mask, entry->reason);

	if (KillonSQline) {
		snprintf(buf, (BUFSIZE - 1), "Q-Lined: %s", entry->reason);
		u2 = firstuser();
		while (u2) {
			next = nextuser();
			if (!is_oper(u2)) {
				if (Anope::Match(u2->nick, entry->mask, false)) {
					kill_user(ServerName, u2->nick, buf);
				}
			}
			u2 = next;
		}
	}

	return deleted;
}

/* Does the user match any SQLINEs? */

int check_sqline(const char *nick, int nick_change)
{
	int i;
	SXLine *sx;
	char reason[300];

	if (sqlines.count == 0)
		return 0;

	for (i = 0; i < sqlines.count; i++) {
		sx = static_cast<SXLine *>(sqlines.list[i]);
		if (!sx)
			continue;

		if (ircd->chansqline) {
			if (*sx->mask == '#')
				continue;
		}

		if (Anope::Match(nick, sx->mask, false)) {
			sqline(sx->mask, sx->reason);
			/* We kill nick since s_sqline can't */
			snprintf(reason, sizeof(reason), "Q-Lined: %s", sx->reason);
			kill_user(s_OperServ, nick, reason);
			return 1;
		}
	}

	return 0;
}

int check_chan_sqline(const char *chan)
{
	int i;
	SXLine *sx;

	if (sqlines.count == 0)
		return 0;

	for (i = 0; i < sqlines.count; i++) {
		sx = static_cast<SXLine *>(sqlines.list[i]);
		if (!sx)
			continue;

		if (*sx->mask != '#')
			continue;

		if (Anope::Match(chan, sx->mask, false)) {
			sqline(sx->mask, sx->reason);
			return 1;
		}
	}

	return 0;
}

/* Delete any expired SQLINEs. */

void expire_sqlines()
{
	int i;
	time_t now = time(NULL);
	SXLine *sx;

	for (i = sqlines.count - 1; i >= 0; i--) {
		sx = static_cast<SXLine *>(sqlines.list[i]);

		if (!sx->expires || sx->expires > now)
			continue;

		if (WallSQLineExpire)
			ircdproto->SendGlobops(s_OperServ, "SQLINE on \2%s\2 has expired",
							 sx->mask);

		slist_delete(&sqlines, i);
	}
}

static void free_sqline_entry(SList * slist, void *item)
{
	SXLine *sx = static_cast<SXLine *>(item);

	/* Remove the SQLINE from all the servers */
	ircdproto->SendSQLineDel(sx->mask);

	/* Free the structure */
	delete [] sx->mask;
	delete [] sx->by;
	delete [] sx->reason;
	delete sx;
}

/* item1 is not an SXLine pointer, but a char */

static int is_sqline_entry_equal(SList * slist, void *item1, void *item2)
{
	char *sx1 = static_cast<char *>(item1);
	SXLine *sx2 = static_cast<SXLine *>(item2);

	if (!sx1 || !sx2)
		return 0;

	if (!stricmp(sx1, sx2->mask))
		return 1;
	else
		return 0;
}

/*************************************************************************/

/* Adds an SZLINE to the list. Returns >= 0 on success, -1 on error, -2 if
 * only the expiry time changed.
 * The success result is the number of SZLINEs that were deleted to successfully add one.
 */

int add_szline(User * u, const char *mask, const char *by, time_t expires,
			   const char *reason)
{
	int deleted = 0, i;
	SXLine *entry;

	if (!mask) {
		return -1;
	}

	/* Checks whether there is an SZLINE that already covers
	 * the one we want to add, and whether there are SZLINEs
	 * that would be covered by this one.
	 * If so, warn the user in the first case and cleanup
	 * the useless SZLINEs in the second.
	 */

	if (szlines.count > 0) {

		for (i = szlines.count - 1; i >= 0; i--) {
			entry = static_cast<SXLine *>(szlines.list[i]);

			if (!entry)
				continue;

			if (!stricmp(entry->mask, mask)) {
				if (entry->expires >= expires || entry->expires == 0) {
					if (u)
						notice_lang(s_OperServ, u, OPER_SZLINE_EXISTS,
									mask);
					return -1;
				} else {
					entry->expires = expires;
					if (u)
						notice_lang(s_OperServ, u, OPER_SZLINE_EXISTS,
									mask);
					return -2;
				}
			}

			if (Anope::Match(mask, entry->mask, false)) {
				if (u)
					notice_lang(s_OperServ, u, OPER_SZLINE_ALREADY_COVERED,
								mask, entry->mask);
				return -1;
			}

			if (Anope::Match(entry->mask, mask, false)) {
				slist_delete(&szlines, i);
				deleted++;
			}
		}

	}

	/* We can now check whether the list is full or not. */
	if (slist_full(&szlines)) {
		if (u)
			notice_lang(s_OperServ, u, OPER_SZLINE_REACHED_LIMIT,
						szlines.limit);
		return -1;
	}

	/* We can now (really) add the SZLINE. */
	entry = new SXLine;
	if (!entry)
		return -1;

	entry->mask = sstrdup(mask);
	entry->by = sstrdup(by);
	entry->reason = sstrdup(reason);
	entry->seton = time(NULL);
	entry->expires = expires;

	slist_add(&szlines, entry);
	ircdproto->SendSZLine(entry->mask, entry->reason, entry->by);

	return deleted;
}

/* Check and enforce any Zlines that we have */
int check_szline(const char *nick, char *ip)
{
	int i;
	SXLine *sx;

	if (szlines.count == 0) {
		return 0;
	}

	if (!ip) {
		return 0;
	}

	for (i = 0; i < szlines.count; i++) {
		sx = static_cast<SXLine *>(szlines.list[i]);
		if (!sx) {
			continue;
		}

		if (Anope::Match(ip, sx->mask, false)) {
			ircdproto->SendSZLine(sx->mask, sx->reason, sx->by);
			return 1;
		}
	}

	return 0;
}


/* Delete any expired SZLINEs. */

void expire_szlines()
{
	int i;
	time_t now = time(NULL);
	SXLine *sx;

	for (i = szlines.count - 1; i >= 0; i--) {
		sx = static_cast<SXLine *>(szlines.list[i]);

		if (!sx->expires || sx->expires > now)
			continue;

		if (WallSZLineExpire)
			ircdproto->SendGlobops(s_OperServ, "SZLINE on \2%s\2 has expired",
							 sx->mask);
		slist_delete(&szlines, i);
	}
}

static void free_szline_entry(SList * slist, void *item)
{
	SXLine *sx = static_cast<SXLine *>(item);

	/* Remove the SZLINE from all the servers */
	ircdproto->SendSZLineDel(sx->mask);

	/* Free the structure */
	delete [] sx->mask;
	delete [] sx->by;
	delete [] sx->reason;
	delete sx;
}

/* item1 is not an SXLine pointer, but a char
 */

static int is_szline_entry_equal(SList * slist, void *item1, void *item2)
{
	char *sx1 = static_cast<char *>(item1);
	SXLine *sx2 = static_cast<SXLine *>(item2);

	if (!sx1 || !sx2)
		return 0;

	if (!stricmp(sx1, sx2->mask))
		return 1;
	else
		return 0;
}

/*************************************************************************/

/* Callback function used to sort the admin list */

static int compare_adminlist_entries(SList * slist, void *item1,
									 void *item2)
{
	NickCore *nc1 = static_cast<NickCore *>(item1), *nc2 = static_cast<NickCore *>(item2);
	if (!nc1 || !nc2)
		return -1;			  /* To tell to continue */
	return stricmp(nc1->display, nc2->display);
}

/* Callback function used when an admin list entry is deleted */

static void free_adminlist_entry(SList * slist, void *item)
{
	NickCore *nc = static_cast<NickCore *>(item);
	nc->flags &= ~NI_SERVICES_ADMIN;
}

/*************************************************************************/

/* Callback function used to sort the oper list */

static int compare_operlist_entries(SList * slist, void *item1,
									void *item2)
{
	NickCore *nc1 = static_cast<NickCore *>(item1), *nc2 = static_cast<NickCore *>(item2);
	if (!nc1 || !nc2)
		return -1;			  /* To tell to continue */
	return stricmp(nc1->display, nc2->display);
}

/* Callback function used when an oper list entry is deleted */

static void free_operlist_entry(SList * slist, void *item)
{
	NickCore *nc = static_cast<NickCore *>(item);
	nc->flags &= ~NI_SERVICES_OPER;
}

/*************************************************************************/

/**
 * Returns 1 if the passed level is part of the CURRENT defcon, else 0 is returned
 **/
int checkDefCon(int level)
{
	return DefCon[DefConLevel] & level;
}

/**
 * Automaticaly re-set the DefCon level if the time limit has expired.
 **/
void resetDefCon(int level)
{
	char strLevel[5];
	snprintf(strLevel, 4, "%d", level);
	if (DefConLevel != level) {
		if ((DefContimer)
			&& (time(NULL) - DefContimer >= DefConTimeOut)) {
			DefConLevel = level;
			send_event(EVENT_DEFCON_LEVEL, 1, strLevel);
			alog("Defcon level timeout, returning to lvl %d", level);
			ircdproto->SendGlobops(s_OperServ,
							 getstring(OPER_DEFCON_WALL),
							 s_OperServ, level);
			if (GlobalOnDefcon) {
				if (DefConOffMessage) {
					oper_global(NULL, "%s", DefConOffMessage);
				} else {
					oper_global(NULL, getstring(DEFCON_GLOBAL),
								DefConLevel);
				}
			}
			if (GlobalOnDefconMore && !DefConOffMessage) {
				oper_global(NULL, "%s", DefconMessage);
			}
			runDefCon();
		}
	}
}

/**
 * Run DefCon level specific Functions.
 **/
void runDefCon()
{
	char *newmodes;
	if (checkDefCon(DEFCON_FORCE_CHAN_MODES)) {
		if (DefConChanModes && !DefConModesSet) {
			if (DefConChanModes[0] == '+' || DefConChanModes[0] == '-') {
				alog("DEFCON: setting %s on all chan's", DefConChanModes);
				do_mass_mode(DefConChanModes);
				DefConModesSet = 1;
			}
		}
	} else {
		if (DefConChanModes && (DefConModesSet != 0)) {
			if (DefConChanModes[0] == '+' || DefConChanModes[0] == '-') {
				DefConModesSet = 0;
				if ((newmodes = defconReverseModes(DefConChanModes))) {
					alog("DEFCON: setting %s on all chan's", newmodes);
					do_mass_mode(newmodes);
					delete [] newmodes;
				}
			}
		}
	}
}

/**
 * Reverse the mode string, used for remove DEFCON chan modes.
 **/
char *defconReverseModes(const char *modes)
{
	char *newmodes = NULL;
	unsigned i = 0;
	if (!modes) {
		return NULL;
	}
	if (!(newmodes = new char[strlen(modes) + 1])) {
		return NULL;
	}
	for (i = 0; i < strlen(modes); i++) {
		if (modes[i] == '+')
			newmodes[i] = '-';
		else if (modes[i] == '-')
			newmodes[i] = '+';
		else
			newmodes[i] = modes[i];
	}
	newmodes[i] = '\0';
	return newmodes;
}

/* Parse the defcon mlock mode string and set the correct global vars.
 *
 * @param str mode string to parse
 * @return 1 if accepted, 0 if failed
 */
int defconParseModeString(const char *str)
{
	int add = -1;			   /* 1 if adding, 0 if deleting, -1 if neither */
	unsigned char mode;
	CBMode *cbm;
	char *str_copy = sstrdup(str);	  /* We need this copy as str is const -GD */
	char *param;				/* Store parameters during mode parsing */

	/* Reinitialize everything */
	DefConModesOn = 0;
	DefConModesOff = 0;
	DefConModesCI.mlock_limit = 0;
	DefConModesCI.mlock_key = NULL;
	DefConModesCI.mlock_flood = NULL;
	DefConModesCI.mlock_redirect = NULL;

	/* Initialize strtok() internal buffer */
	strtok(str_copy, " ");

	/* Loop while there are modes to set */
	while ((mode = *str++) && (mode != ' ')) {
		switch (mode) {
		case '+':
			add = 1;
			continue;
		case '-':
			add = 0;
			continue;
		default:
			if (add < 0)
				continue;
		}

		if (static_cast<int>(mode) < 128 && (cbm = &cbmodes[static_cast<int>(mode)])->flag != 0) {
			if (cbm->flags & CBM_NO_MLOCK) {
				alog("DefConChanModes mode character '%c' cannot be locked", mode);
				delete [] str_copy;
				return 0;
			} else if (add) {
				DefConModesOn |= cbm->flag;
				DefConModesOff &= ~cbm->flag;
				if (cbm->cssetvalue) {
					if (!(param = strtok(NULL, " "))) {
						alog("DefConChanModes mode character '%c' has no parameter while one is expected", mode);
						delete [] str_copy;
						return 0;
					}
					cbm->cssetvalue(&DefConModesCI, param);
				}
			} else {
				DefConModesOff |= cbm->flag;
				if (DefConModesOn & cbm->flag) {
					DefConModesOn &= ~cbm->flag;
					if (cbm->cssetvalue) {
						cbm->cssetvalue(&DefConModesCI, NULL);
					}
				}
			}
		} else {
			alog("DefConChanModes unknown mode character '%c'", mode);
			delete [] str_copy;
			return 0;
		}
	}						   /* while (*param) */

	delete [] str_copy;

	if (ircd->Lmode) {
		/* We can't mlock +L if +l is not mlocked as well. */
		if ((DefConModesOn & ircd->chan_lmode)
			&& !(DefConModesOn & anope_get_limit_mode())) {
			DefConModesOn &= ~ircd->chan_lmode;
			delete [] DefConModesCI.mlock_redirect;
			DefConModesCI.mlock_redirect = NULL;
			alog("DefConChanModes must lock mode +l as well to lock mode +L");
			return 0;
		}
	}

	/* Some ircd we can't set NOKNOCK without INVITE */
	/* So check if we need there is a NOKNOCK MODE and that we need INVITEONLY */
	if (ircd->noknock && ircd->knock_needs_i) {
		if ((DefConModesOn & ircd->noknock)
			&& !(DefConModesOn & anope_get_invite_mode())) {
			DefConModesOn &= ~ircd->noknock;
			alog("DefConChanModes must lock mode +i as well to lock mode +K");
			return 0;
		}
	}

	/* Everything is set fine, return 1 */
	return 1;
}

/*************************************************************************/
