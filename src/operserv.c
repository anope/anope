/* OperServ functions.
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
#include "pseudo.h"

/*************************************************************************/

/* AKILL, SGLINE, SQLINE and SZLINE lists */
SList akills, sglines, sqlines, szlines;

/*************************************************************************/

static int is_akill_entry_equal(SList * slist, void *item1, void *item2);
static void free_akill_entry(SList * slist, void *item);
static int is_sgline_entry_equal(SList * slist, void *item1, void *item2);
static void free_sgline_entry(SList * slist, void *item);
static int is_sqline_entry_equal(SList * slist, void *item1, void *item2);
static void free_sqline_entry(SList * slist, void *item);
static int is_szline_entry_equal(SList * slist, void *item1, void *item2);
static void free_szline_entry(SList * slist, void *item);

/* News items */
std::vector<NewsItem *> News;

std::vector<std::bitset<32> > DefCon;
int DefConModesSet = 0;
/* Defcon modes mlocked on */
Flags<ChannelModeName> DefConModesOn;
/* Defcon modes mlocked off */
Flags<ChannelModeName> DefConModesOff;
/* Map of Modesa and Params for DefCon */
std::map<ChannelModeName, std::string> DefConModesOnParams;

bool SetDefConParam(ChannelModeName Name, std::string &buf)
{
	return DefConModesOnParams.insert(std::make_pair(Name, buf)).second;
}

bool GetDefConParam(ChannelModeName Name, std::string *buf)
{
	std::map<ChannelModeName, std::string>::iterator it = DefConModesOnParams.find(Name);

	buf->clear();

	if (it != DefConModesOnParams.end())
	{
		*buf = it->second;
		return true;
	}

	return false;
}

void UnsetDefConParam(ChannelModeName Name)
{
	std::map<ChannelModeName, std::string>::iterator it = DefConModesOnParams.find(Name);

	if (it != DefConModesOnParams.end())
	{
		DefConModesOnParams.erase(it);
	}
}

/*************************************************************************/

void moduleAddOperServCmds();
/*************************************************************************/

/* Options for the lists */
SListOpts akopts = { 0, NULL, &is_akill_entry_equal, &free_akill_entry };

SListOpts sgopts = { 0, NULL, &is_sgline_entry_equal, &free_sgline_entry };
SListOpts sqopts =
	{ SLISTF_SORT, NULL, &is_sqline_entry_equal, &free_sqline_entry };
SListOpts szopts = { 0, NULL, &is_szline_entry_equal, &free_szline_entry };

/*************************************************************************/
/* *INDENT-OFF* */
void moduleAddOperServCmds() {
	ModuleManager::LoadModuleList(Config.OperServCoreModules);
}

/* *INDENT-ON* */
/*************************************************************************/
/*************************************************************************/

/* OperServ initialization. */

void os_init()
{
	moduleAddOperServCmds();

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

	alog("%s: %s: %s", Config.s_OperServ, u->nick.c_str(), buf);

	cmd = strtok(buf, " ");
	if (!cmd) {
		return;
	} else if (stricmp(cmd, "\1PING") == 0) {
		if (!(s = strtok(NULL, ""))) {
			s = "";
		}
		ircdproto->SendCTCP(findbot(Config.s_OperServ), u->nick.c_str(), "PING %s", s);
	} else {
		mod_run_cmd(Config.s_OperServ, u, OPERSERV, cmd);
	}
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
		if (!s->HasFlag(SERVER_ISME) && !s->HasFlag(SERVER_JUPED))
			notice_server(Config.s_GlobalNoticer, s, "%s", msg);

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
	if ((nick) && (!Config.AnonymousGlobal)) {
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
						notice_lang(Config.s_OperServ, u, OPER_AKILL_EXISTS,
									mask);
					return -1;
				} else {
					entry->expires = expires;
					if (u)
						notice_lang(Config.s_OperServ, u, OPER_AKILL_CHANGED,
									amask);
					return -2;
				}
			}

			if (Anope::Match(mask, amask, false)
				&& (entry->expires >= expires || entry->expires == 0)) {
				if (u)
					notice_lang(Config.s_OperServ, u, OPER_AKILL_ALREADY_COVERED,
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
			notice_lang(Config.s_OperServ, u, OPER_AKILL_REACHED_LIMIT,
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

	if (Config.AkillOnAdd)
		ircdproto->SendAkill(entry);

	delete [] mask2;

	return deleted;
}

/* Does the user match any AKILLs? */

int check_akill(const char *nick, const char *username, const char *host,
				const char *vhost, const char *ip)
{
	int i;
	Akill *ak;

	if (akills.count == 0)
		return 0;

	for (i = 0; i < akills.count; i++) {
		ak = static_cast<Akill *>(akills.list[i]);
		if (!ak)
			continue;
		if (Anope::Match(username, ak->user, false)
			&& Anope::Match(host, ak->host, false)) {
			ircdproto->SendAkill(ak);
			return 1;
		}
		if (ircd->vhost) {
			if (vhost) {
				if (Anope::Match(username, ak->user, false)
					&& Anope::Match(vhost, ak->host, false)) {
					ircdproto->SendAkill(ak);
					return 1;
				}
			}
		}
		if (ircd->nickip) {
			if (ip) {
				if (Anope::Match(username, ak->user, false)
					&& Anope::Match(ip, ak->host, false)) {
					ircdproto->SendAkill(ak);
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

		if (Config.WallAkillExpire)
			ircdproto->SendGlobops(findbot(Config.s_OperServ), "AKILL on %s@%s has expired",
							 ak->user, ak->host);
		slist_delete(&akills, i);
	}
}

static void free_akill_entry(SList * slist, void *item)
{
	Akill *ak = static_cast<Akill *>(item);

	/* Remove the AKILLs from all the servers */
	ircdproto->SendAkillDel(ak);

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
						notice_lang(Config.s_OperServ, u, OPER_SGLINE_EXISTS,
									mask);
					return -1;
				} else {
					entry->expires = expires;
					if (u)
						notice_lang(Config.s_OperServ, u, OPER_SGLINE_CHANGED,
									entry->mask);
					return -2;
				}
			}

			if (Anope::Match(mask, entry->mask, false )
				&& (entry->expires >= expires || entry->expires == 0)) {
				if (u)
					notice_lang(Config.s_OperServ, u, OPER_SGLINE_ALREADY_COVERED,
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
			notice_lang(Config.s_OperServ, u, OPER_SGLINE_REACHED_LIMIT,
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

	ircdproto->SendSGLine(entry);

	if (Config.KillonSGline && !ircd->sglineenforce) {
		snprintf(buf, (BUFSIZE - 1), "G-Lined: %s", entry->reason);
		u2 = firstuser();
		while (u2) {
			next = nextuser();
			if (!is_oper(u2)) {
				if (Anope::Match(u2->realname, entry->mask, false)) {
					kill_user(Config.ServerName, u2->nick, buf);
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
			ircdproto->SendSGLine(sx);
			/* We kill nick since s_sgline can't */
			ircdproto->SendSVSKill(NULL, finduser(nick), "G-Lined: %s", sx->reason);
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

		if (Config.WallSGLineExpire)
			ircdproto->SendGlobops(findbot(Config.s_OperServ), "SGLINE on \2%s\2 has expired",
							 sx->mask);
		slist_delete(&sglines, i);
	}
}

static void free_sgline_entry(SList * slist, void *item)
{
	SXLine *sx = static_cast<SXLine *>(item);

	/* Remove the SGLINE from all the servers */
	ircdproto->SendSGLineDel(sx);

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
						notice_lang(Config.s_OperServ, u, OPER_SQLINE_EXISTS,
									mask);
					return -1;
				} else {
					entry->expires = expires;
					if (u)
						notice_lang(Config.s_OperServ, u, OPER_SQLINE_CHANGED,
									entry->mask);
					return -2;
				}
			}

			if (Anope::Match(mask, entry->mask, false)
				&& (entry->expires >= expires || entry->expires == 0)) {
				if (u)
					notice_lang(Config.s_OperServ, u, OPER_SQLINE_ALREADY_COVERED,
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
			notice_lang(Config.s_OperServ, u, OPER_SQLINE_REACHED_LIMIT,
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

	if (Config.KillonSQline) {
		snprintf(buf, (BUFSIZE - 1), "Q-Lined: %s", entry->reason);
		u2 = firstuser();
		while (u2) {
			next = nextuser();
			if (!is_oper(u2)) {
				if (Anope::Match(u2->nick, entry->mask, false)) {
					kill_user(Config.ServerName, u2->nick, buf);
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
			kill_user(Config.s_OperServ, nick, reason);
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

		if (Config.WallSQLineExpire)
			ircdproto->SendGlobops(findbot(Config.s_OperServ), "SQLINE on \2%s\2 has expired",
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
						notice_lang(Config.s_OperServ, u, OPER_SZLINE_EXISTS,
									mask);
					return -1;
				} else {
					entry->expires = expires;
					if (u)
						notice_lang(Config.s_OperServ, u, OPER_SZLINE_EXISTS,
									mask);
					return -2;
				}
			}

			if (Anope::Match(mask, entry->mask, false)) {
				if (u)
					notice_lang(Config.s_OperServ, u, OPER_SZLINE_ALREADY_COVERED,
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
			notice_lang(Config.s_OperServ, u, OPER_SZLINE_REACHED_LIMIT,
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
	ircdproto->SendSZLine(entry);

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
			ircdproto->SendSZLine(sx);
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

		if (Config.WallSZLineExpire)
			ircdproto->SendGlobops(findbot(Config.s_OperServ), "SZLINE on \2%s\2 has expired",
							 sx->mask);
		slist_delete(&szlines, i);
	}
}

static void free_szline_entry(SList * slist, void *item)
{
	SXLine *sx = static_cast<SXLine *>(item);

	/* Remove the SZLINE from all the servers */
	ircdproto->SendSZLineDel(sx);

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

/** Check if a certain defcon option is currently in affect
 * @param Level The level
 * @return true and false
 */
bool CheckDefCon(DefconLevel Level)
{
	if (Config.DefConLevel)
		return DefCon[Config.DefConLevel][Level];
	return false;
}

/** Check if a certain defcon option is in a certain defcon level
 * @param level The defcon level
 * @param Level The defcon level name
 * @return true or false
 */
bool CheckDefCon(int level, DefconLevel Level)
{
	return DefCon[level][Level];
}

/** Add a defcon level option to a defcon level
 * @param level The defcon level
 * @param Level The defcon level option
 */
void AddDefCon(int level, DefconLevel Level)
{
	DefCon[level][Level] = true;
}

/** Remove a defcon level option from a defcon level
 * @param level The defcon level
 * @param Level The defcon level option
 */
void DelDefCon(int level, DefconLevel Level)
{
	DefCon[level][Level] = false;
}

