/* OperServ functions.
 *
 * (C) 2003-2005 Anope Team
 * Contact us at info@anope.org
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



/* List of most recent users - statically initialized to zeros */
struct clone clonelist[CLONE_DETECT_SIZE];

/* Which hosts have we warned about, and when? This is used to keep us
 * from sending out notices over and over for clones from the same host. */
struct clone warnings[CLONE_DETECT_SIZE];

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

void resetDefCon(int level);

time_t DefContimer;
int DefConModesSet = 0;
void runDefCon(void);
char *defconReverseModes(const char *modes);
void oper_global(char *nick, char *fmt, ...);


#ifdef DEBUG_COMMANDS
static int send_clone_lists(User * u);
static int do_matchwild(User * u);
#endif

void moduleAddOperServCmds(void);
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
void moduleAddOperServCmds(void) {
    Command *c;
    
    modules_core_init(OperServCoreNumber, OperServCoreModules); 

#ifdef DEBUG_COMMANDS
    c = createCommand("LISTTIMERS", send_timeout_list,  is_services_root, -1,-1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("MATCHWILD",  do_matchwild,       is_services_root, -1,-1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("LISTCLONES", send_clone_lists,   is_services_root, -1,-1,-1,-1,-1); addCoreCommand(OPERSERV,c);
#endif
}

/* *INDENT-ON* */
/*************************************************************************/
/*************************************************************************/

/* OperServ initialization. */

void os_init(void)
{
    Command *cmd;
    moduleAddOperServCmds();
    cmd = findCommand(OPERSERV, "GLOBAL");
    if (cmd)
        cmd->help_param1 = s_GlobalNoticer;
    cmd = findCommand(OPERSERV, "ADMIN");
    if (cmd)
        cmd->help_param1 = s_NickServ;
    cmd = findCommand(OPERSERV, "OPER");
    if (cmd)
        cmd->help_param1 = s_NickServ;

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
    char *cmd;
    char *s;

    alog("%s: %s: %s", s_OperServ, u->nick, buf);

    cmd = strtok(buf, " ");
    if (!cmd) {
        return;
    } else if (stricmp(cmd, "\1PING") == 0) {
        if (!(s = strtok(NULL, ""))) {
            s = "";
        }
        anope_cmd_ctcp(s_OperServ, u->nick, "PING %s", s);
    } else {
        mod_run_cmd(s_OperServ, u, OPERSERV, cmd);
    }
}

/*************************************************************************/
/**************************** Privilege checks ***************************/
/*************************************************************************/

/* Load old AKILL data. */

#define SAFE(x) do {					\
    if ((x) < 0) {					\
	if (!forceload)					\
	    fatal("Read error on %s", AutokillDBName);	\
	break;						\
    }							\
} while (0)

static void load_old_akill(void)
{
    dbFILE *f;
    int i, j;
    uint16 tmp16;
    uint32 tmp32;
    char buf[NICKMAX], mask2[BUFSIZE], *mask, *s;
    Akill *ak, *entry;

    if (!
        (f =
         open_db("AKILL", AutokillDBName ? AutokillDBName : "akill.db",
                 "r", 9)))
        return;

    get_file_version(f);

    read_int16(&tmp16, f);
    slist_setcapacity(&akills, tmp16);

    for (j = 0; j < akills.capacity; j++) {
        ak = scalloc(sizeof(Akill), 1);

        SAFE(read_string(&mask, f));
        s = strchr(mask, '@');
        *s = 0;
        s++;
        ak->user = sstrdup(mask);
        ak->host = sstrdup(s);
        SAFE(read_string(&ak->reason, f));
        SAFE(read_buffer(buf, f));
        if (!*buf)
            ak->by = sstrdup("<unknown>");
        else
            ak->by = sstrdup(buf);
        SAFE(read_int32(&tmp32, f));
        ak->seton = tmp32 ? tmp32 : time(NULL);
        SAFE(read_int32(&tmp32, f));
        ak->expires = tmp32;

        /* Sanity checks *sigh* */

        /* No nicknames allowed! */
        if (strchr(ak->user, '!')) {
            anope_cmd_remove_akill(ak->user, ak->host);
            free(ak);
            continue;
        }

        snprintf(mask2, sizeof(mask2), "%s@%s", ak->user, ak->host);

        /* Is the mask already in the AKILL list? */
        if (slist_indexof(&akills, mask2) != -1) {
            free(ak);
            continue;
        }

        /* Checks whether there is an AKILL that already covers
         * the one we want to add, and whether there are AKILLs
         * that would be covered by this one. Expiry time
         * does *also* matter.
         */

        if (akills.count > 0) {

            for (i = akills.count - 1; i >= 0; i--) {

                char amask[BUFSIZE];

                entry = akills.list[i];

                if (!entry)
                    continue;

                snprintf(amask, sizeof(amask), "%s@%s", entry->user,
                         entry->host);

                if (match_wild_nocase(amask, mask2)
                    && (entry->expires >= ak->expires
                        || entry->expires == 0)) {
                    anope_cmd_remove_akill(ak->user, ak->host);
                    free(ak);
                    ak = NULL;
                    break;
                }

                if (match_wild_nocase(mask2, amask)
                    && (entry->expires <= ak->expires || ak->expires == 0))
                    slist_delete(&akills, i);
            }

        }

        if (ak)
            slist_add(&akills, ak);
    }

    close_db(f);
}

#undef SAFE

/* Load OperServ data. */

#define SAFE(x) do {					\
    if ((x) < 0) {					\
	if (!forceload)					\
	    fatal("Read error on %s", OperDBName);	\
	failed = 1;					\
	break;						\
    }							\
} while (0)

void load_os_dbase(void)
{
    dbFILE *f;
    int16 i, ver;
    uint16 tmp16, n;
    uint32 tmp32;
    char *s;
    int failed = 0;

    if (!(f = open_db(s_OperServ, OperDBName, "r", OPER_VERSION)))
        return;

    ver = get_file_version(f);

    if (ver <= 9) {
        NickAlias *na;

        SAFE(read_int16(&n, f));
        for (i = 0; i < n && !failed; i++) {
            SAFE(read_string(&s, f));
            if (s) {
                na = findnick(s);
                if (na) {
                    na->nc->flags |= NI_SERVICES_ADMIN;
                    if (slist_indexof(&servadmins, na) == -1)
                        slist_add(&servadmins, na);
                }
                free(s);
            }
        }
        if (!failed)
            SAFE(read_int16(&n, f));
        for (i = 0; i < n && !failed; i++) {
            SAFE(read_string(&s, f));
            if (s) {
                na = findnick(s);
                if (na) {
                    na->nc->flags |= NI_SERVICES_OPER;
                    if (slist_indexof(&servopers, na) == -1)
                        slist_add(&servopers, na);
                }
                free(s);
            }
        }
    }

    if (ver >= 7) {
        uint32 tmp32;
        SAFE(read_int32(&maxusercnt, f));
        SAFE(read_int32(&tmp32, f));
        maxusertime = tmp32;
    }

    if (ver <= 10)
        load_old_akill();
    else {
        Akill *ak;

        read_int16(&tmp16, f);
        slist_setcapacity(&akills, tmp16);

        for (i = 0; i < akills.capacity; i++) {
            ak = scalloc(sizeof(Akill), 1);

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
    }

    if (ver >= 11) {
        SXLine *sx;

        read_int16(&tmp16, f);
        slist_setcapacity(&sglines, tmp16);

        for (i = 0; i < sglines.capacity; i++) {
            sx = scalloc(sizeof(SXLine), 1);

            SAFE(read_string(&sx->mask, f));
            SAFE(read_string(&sx->by, f));
            SAFE(read_string(&sx->reason, f));
            SAFE(read_int32(&tmp32, f));
            sx->seton = tmp32;
            SAFE(read_int32(&tmp32, f));
            sx->expires = tmp32;

            slist_add(&sglines, sx);
        }

        if (ver >= 13) {
            read_int16(&tmp16, f);
            slist_setcapacity(&sqlines, tmp16);

            for (i = 0; i < sqlines.capacity; i++) {
                sx = scalloc(sizeof(SXLine), 1);

                SAFE(read_string(&sx->mask, f));
                SAFE(read_string(&sx->by, f));
                SAFE(read_string(&sx->reason, f));
                SAFE(read_int32(&tmp32, f));
                sx->seton = tmp32;
                SAFE(read_int32(&tmp32, f));
                sx->expires = tmp32;

                slist_add(&sqlines, sx);
            }
        }

        read_int16(&tmp16, f);
        slist_setcapacity(&szlines, tmp16);

        for (i = 0; i < szlines.capacity; i++) {
            sx = scalloc(sizeof(SXLine), 1);

            SAFE(read_string(&sx->mask, f));
            SAFE(read_string(&sx->by, f));
            SAFE(read_string(&sx->reason, f));
            SAFE(read_int32(&tmp32, f));
            sx->seton = tmp32;
            SAFE(read_int32(&tmp32, f));
            sx->expires = tmp32;

            slist_add(&szlines, sx);
        }
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
	    anope_cmd_global(NULL, "Write error on %s: %s", OperDBName,	\
			strerror(errno));			\
	    lastwarn = time(NULL);				\
	}							\
	return;							\
    }								\
} while (0)

void save_os_dbase(void)
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
        ak = akills.list[i];

        SAFE(write_string(ak->user, f));
        SAFE(write_string(ak->host, f));
        SAFE(write_string(ak->by, f));
        SAFE(write_string(ak->reason, f));
        SAFE(write_int32(ak->seton, f));
        SAFE(write_int32(ak->expires, f));
    }

    SAFE(write_int16(sglines.count, f));
    for (i = 0; i < sglines.count; i++) {
        sx = sglines.list[i];

        SAFE(write_string(sx->mask, f));
        SAFE(write_string(sx->by, f));
        SAFE(write_string(sx->reason, f));
        SAFE(write_int32(sx->seton, f));
        SAFE(write_int32(sx->expires, f));
    }

    SAFE(write_int16(sqlines.count, f));
    for (i = 0; i < sqlines.count; i++) {
        sx = sqlines.list[i];

        SAFE(write_string(sx->mask, f));
        SAFE(write_string(sx->by, f));
        SAFE(write_string(sx->reason, f));
        SAFE(write_int32(sx->seton, f));
        SAFE(write_int32(sx->expires, f));
    }

    SAFE(write_int16(szlines.count, f));
    for (i = 0; i < szlines.count; i++) {
        sx = szlines.list[i];

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

void save_os_rdb_dbase(void)
{
#ifdef USE_RDB
    if (!rdb_open())
        return;
    rdb_save_os_db(maxusercnt, maxusertime, &akills, &sglines, &sqlines,
                   &szlines);
    rdb_close();
#endif
}

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
        || (!skeleton && !nick_identified(u)))
        return 0;
    if (skeleton || (u->na->nc->flags & NI_SERVICES_ROOT))
        return 1;
    return 0;
}

/*************************************************************************/

/* Does the given user have Services admin privileges? */

int is_services_admin(User * u)
{
    if ((NSStrictPrivileges && !is_oper(u))
        || (!skeleton && !nick_identified(u)))
        return 0;
    if (skeleton
        || (u->na->nc->flags & (NI_SERVICES_ADMIN | NI_SERVICES_ROOT)))
        return 1;
    return 0;
}

/*************************************************************************/

/* Does the given user have Services oper privileges? */

int is_services_oper(User * u)
{
    if ((NSStrictPrivileges && !is_oper(u))
        || (!skeleton && !nick_identified(u)))
        return 0;
    if (skeleton
        || (u->na->nc->
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
/**************************** Clone detection ****************************/
/*************************************************************************/

/* We just got a new user; does it look like a clone?  If so, send out a
 * wallops.
 */

void check_clones(User * user)
{
    int i, clone_count;
    long last_time;

    if (!CheckClones)
        return;

    if (clonelist[0].host)
        free(clonelist[0].host);
    i = CLONE_DETECT_SIZE - 1;
    memmove(clonelist, clonelist + 1, sizeof(struct clone) * i);
    clonelist[i].host = sstrdup(common_get_vhost(user));
    last_time = clonelist[i].time = time(NULL);
    clone_count = 1;
    while (--i >= 0 && clonelist[i].host) {
        if (clonelist[i].time < last_time - CloneMaxDelay)
            break;
        if (stricmp(clonelist[i].host, common_get_vhost(user)) == 0) {
            ++clone_count;
            last_time = clonelist[i].time;
            if (clone_count >= CloneMinUsers)
                break;
        }
    }
    if (clone_count >= CloneMinUsers) {
        /* Okay, we have clones.  Check first to see if we already know
         * about them. */
        for (i = CLONE_DETECT_SIZE - 1; i >= 0 && warnings[i].host; --i) {
            if (stricmp(warnings[i].host, common_get_vhost(user)) == 0)
                break;
        }
        if (i < 0
            || warnings[i].time < user->my_signon - CloneWarningDelay) {
            /* Send out the warning, and note it. */
            anope_cmd_global(s_OperServ,
                             "\2WARNING\2 - possible clones detected from %s",
                             common_get_vhost(user));
            alog("%s: possible clones detected from %s", s_OperServ,
                 user->host);
            i = CLONE_DETECT_SIZE - 1;
            if (warnings[0].host)
                free(warnings[0].host);
            memmove(warnings, warnings + 1, sizeof(struct clone) * i);
            warnings[i].host = sstrdup(common_get_vhost(user));
            warnings[i].time = clonelist[i].time;
            if (KillClones)
                kill_user(s_OperServ, user->nick, "Clone kill");
        }
    }
}

/*************************************************************************/

#ifdef DEBUG_COMMANDS

/* Send clone arrays to given nick. */

static int send_clone_lists(User * u)
{
    int i;

    if (!CheckClones) {
        notice_user(s_OperServ, u, "CheckClones not enabled.");
        return MOD_CONT;
    }

    notice_user(s_OperServ, u, "clonelist[]");
    for (i = 0; i < CLONE_DETECT_SIZE; i++) {
        if (clonelist[i].host)
            notice_user(s_OperServ, u, "    %10ld  %s", clonelist[i].time,
                        clonelist[i].host ? clonelist[i].host : "(null)");
    }
    notice_user(s_OperServ, u, "warnings[]");
    for (i = 0; i < CLONE_DETECT_SIZE; i++) {
        if (clonelist[i].host)
            notice_user(s_OperServ, u, "    %10ld  %s", warnings[i].time,
                        warnings[i].host ? warnings[i].host : "(null)");
    }
    return MOD_CONT;
}

#endif                          /* DEBUG_COMMANDS */

/*************************************************************************/
/*********************** OperServ command functions **********************/
/*************************************************************************/

/*************************************************************************/


Server *server_global(Server * s, char *msg)
{
    Server *sl;

    while (s) {
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

void oper_global(char *nick, char *fmt, ...)
{
    va_list args;
    char msg[2048];             /* largest valid message is 512, this should cover any global */
    char dmsg[2048];            /* largest valid message is 512, this should cover any global */

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

int add_akill(User * u, char *mask, const char *by, const time_t expires,
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

            entry = akills.list[i];

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

            if (match_wild_nocase(amask, mask)
                && (entry->expires >= expires || entry->expires == 0)) {
                if (u)
                    notice_lang(s_OperServ, u, OPER_AKILL_ALREADY_COVERED,
                                mask, amask);
                return -1;
            }

            if (match_wild_nocase(mask, amask)
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
    if (!host)
        return -1;
    user = mask2;
    *host = 0;
    host++;

    entry = scalloc(sizeof(Akill), 1);
    if (!entry)
        return -1;

    entry->user = sstrdup(user);
    entry->host = sstrdup(host);
    entry->by = sstrdup(by);
    entry->reason = sstrdup(reason);
    entry->seton = time(NULL);
    entry->expires = expires;

    slist_add(&akills, entry);

    if (AkillOnAdd)
        anope_cmd_akill(entry->user, entry->host, entry->by, entry->seton,
                        entry->expires, entry->reason);

    free(mask2);

    return deleted;
}

/* Does the user match any AKILLs? */

int check_akill(char *nick, const char *username, const char *host,
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
        ak = akills.list[i];
        if (!ak)
            continue;
        if (match_wild_nocase(ak->user, username)
            && match_wild_nocase(ak->host, host)) {
            anope_cmd_akill(ak->user, ak->host, ak->by, ak->seton,
                            ak->expires, ak->reason);
            return 1;
        }
        if (ircd->vhost) {
            if (vhost) {
                if (match_wild_nocase(ak->user, username)
                    && match_wild_nocase(ak->host, vhost)) {
                    anope_cmd_akill(ak->user, ak->host, ak->by, ak->seton,
                                    ak->expires, ak->reason);
                    return 1;
                }
            }
        }
        if (ircd->nickip) {
            if (ip) {
                if (match_wild_nocase(ak->user, username)
                    && match_wild_nocase(ak->host, ip)) {
                    anope_cmd_akill(ak->user, ak->host, ak->by, ak->seton,
                                    ak->expires, ak->reason);
                    return 1;
                }
            }
        }

    }

    return 0;
}

/* Delete any expired autokills. */

void expire_akills(void)
{
    int i;
    time_t now = time(NULL);
    Akill *ak;

    for (i = akills.count - 1; i >= 0; i--) {
        ak = akills.list[i];

        if (!ak->expires || ak->expires > now)
            continue;

        if (WallAkillExpire)
            anope_cmd_global(s_OperServ, "AKILL on %s@%s has expired",
                             ak->user, ak->host);
        slist_delete(&akills, i);
    }
}

static void free_akill_entry(SList * slist, void *item)
{
    Akill *ak = item;

    /* Remove the AKILLs from all the servers */
    anope_cmd_remove_akill(ak->user, ak->host);

    /* Free the structure */
    free(ak->user);
    free(ak->host);
    free(ak->by);
    free(ak->reason);
    free(ak);
}

/* item1 is not an Akill pointer, but a char
 */

static int is_akill_entry_equal(SList * slist, void *item1, void *item2)
{
    char *ak1 = item1, buf[BUFSIZE];
    Akill *ak2 = item2;

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

int add_sgline(User * u, char *mask, const char *by, const time_t expires,
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
            entry = sglines.list[i];

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

            if (match_wild_nocase(entry->mask, mask)
                && (entry->expires >= expires || entry->expires == 0)) {
                if (u)
                    notice_lang(s_OperServ, u, OPER_SGLINE_ALREADY_COVERED,
                                mask, entry->mask);
                return -1;
            }

            if (match_wild_nocase(mask, entry->mask)
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
    entry = scalloc(sizeof(SXLine), 1);
    if (!entry)
        return -1;

    entry->mask = sstrdup(mask);
    entry->by = sstrdup(by);
    entry->reason = sstrdup(reason);
    entry->seton = time(NULL);
    entry->expires = expires;

    slist_add(&sglines, entry);

    anope_cmd_sgline(entry->mask, entry->reason);

    if (KillonSGline && !ircd->sglineenforce) {
        snprintf(buf, (BUFSIZE - 1), "G-Lined: %s", entry->reason);
        u2 = firstuser();
        while (u2) {
            next = nextuser();
            if (!is_oper(u2)) {
                if (match_wild_nocase(entry->mask, u2->realname)) {
                    kill_user(ServerName, u2->nick, buf);
                }
            }
            u2 = next;
        }
    }
    return deleted;
}

/* Does the user match any SGLINEs? */

int check_sgline(char *nick, const char *realname)
{
    int i;
    SXLine *sx;

    if (sglines.count == 0)
        return 0;

    for (i = 0; i < sglines.count; i++) {
        sx = sglines.list[i];
        if (!sx)
            continue;

        if (match_wild_nocase(sx->mask, realname)) {
            anope_cmd_sgline(sx->mask, sx->reason);
            /* We kill nick since s_sgline can't */
            anope_cmd_svskill(ServerName, nick, "G-Lined: %s", sx->reason);
            return 1;
        }
    }

    return 0;
}

/* Delete any expired SGLINEs. */

void expire_sglines(void)
{
    int i;
    time_t now = time(NULL);
    SXLine *sx;

    for (i = sglines.count - 1; i >= 0; i--) {
        sx = sglines.list[i];

        if (!sx->expires || sx->expires > now)
            continue;

        if (WallSGLineExpire)
            anope_cmd_global(s_OperServ, "SGLINE on \2%s\2 has expired",
                             sx->mask);
        slist_delete(&sglines, i);
    }
}

static void free_sgline_entry(SList * slist, void *item)
{
    SXLine *sx = item;

    /* Remove the SGLINE from all the servers */
    anope_cmd_unsgline(sx->mask);

    /* Free the structure */
    free(sx->mask);
    free(sx->by);
    free(sx->reason);
    free(sx);
}

/* item1 is not an SXLine pointer, but a char */

static int is_sgline_entry_equal(SList * slist, void *item1, void *item2)
{
    char *sx1 = item1;
    SXLine *sx2 = item2;

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

int add_sqline(User * u, char *mask, const char *by, const time_t expires,
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
            entry = sqlines.list[i];

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

            if (match_wild_nocase(entry->mask, mask)
                && (entry->expires >= expires || entry->expires == 0)) {
                if (u)
                    notice_lang(s_OperServ, u, OPER_SQLINE_ALREADY_COVERED,
                                mask, entry->mask);
                return -1;
            }

            if (match_wild_nocase(mask, entry->mask)
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
    entry = scalloc(sizeof(SXLine), 1);
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
                if (match_wild_nocase(entry->mask, u2->nick)) {
                    kill_user(ServerName, u2->nick, buf);
                }
            }
            u2 = next;
        }
    }

    return deleted;
}

/* Does the user match any SQLINEs? */

int check_sqline(char *nick, int nick_change)
{
    int i;
    SXLine *sx;
    char reason[300];

    if (sqlines.count == 0)
        return 0;

    for (i = 0; i < sqlines.count; i++) {
        sx = sqlines.list[i];
        if (!sx)
            continue;

        if (ircd->chansqline) {
            if (*sx->mask == '#')
                continue;
        }

        if (match_wild_nocase(sx->mask, nick)) {
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
        sx = sqlines.list[i];
        if (!sx)
            continue;

        if (*sx->mask != '#')
            continue;

        if (match_wild_nocase(sx->mask, chan)) {
            sqline(sx->mask, sx->reason);
            return 1;
        }
    }

    return 0;
}

/* Delete any expired SQLINEs. */

void expire_sqlines(void)
{
    int i;
    time_t now = time(NULL);
    SXLine *sx;

    for (i = sqlines.count - 1; i >= 0; i--) {
        sx = sqlines.list[i];

        if (!sx->expires || sx->expires > now)
            continue;

        if (WallSQLineExpire)
            anope_cmd_global(s_OperServ, "SQLINE on \2%s\2 has expired",
                             sx->mask);

        slist_delete(&sqlines, i);
    }
}

static void free_sqline_entry(SList * slist, void *item)
{
    SXLine *sx = item;

    /* Remove the SQLINE from all the servers */
    anope_cmd_unsqline(sx->mask);

    /* Free the structure */
    free(sx->mask);
    free(sx->by);
    free(sx->reason);
    free(sx);
}

/* item1 is not an SXLine pointer, but a char */

static int is_sqline_entry_equal(SList * slist, void *item1, void *item2)
{
    char *sx1 = item1;
    SXLine *sx2 = item2;

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

int add_szline(User * u, char *mask, const char *by, const time_t expires,
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
            entry = szlines.list[i];

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

            if (match_wild_nocase(entry->mask, mask)) {
                if (u)
                    notice_lang(s_OperServ, u, OPER_SZLINE_ALREADY_COVERED,
                                mask, entry->mask);
                return -1;
            }

            if (match_wild_nocase(mask, entry->mask)) {
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
    entry = scalloc(sizeof(SXLine), 1);
    if (!entry)
        return -1;

    entry->mask = sstrdup(mask);
    entry->by = sstrdup(by);
    entry->reason = sstrdup(reason);
    entry->seton = time(NULL);
    entry->expires = expires;

    slist_add(&szlines, entry);
    anope_cmd_szline(entry->mask, entry->reason, entry->by);

    return deleted;
}

/* Check and enforce any Zlines that we have */
int check_szline(char *nick, char *ip)
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
        sx = szlines.list[i];
        if (!sx) {
            continue;
        }

        if (match_wild_nocase(sx->mask, ip)) {
            anope_cmd_szline(sx->mask, sx->reason, sx->by);
            return 1;
        }
    }

    return 0;
}


/* Delete any expired SZLINEs. */

void expire_szlines(void)
{
    int i;
    time_t now = time(NULL);
    SXLine *sx;

    for (i = szlines.count - 1; i >= 0; i--) {
        sx = szlines.list[i];

        if (!sx->expires || sx->expires > now)
            continue;

        if (WallSZLineExpire)
            anope_cmd_global(s_OperServ, "SZLINE on \2%s\2 has expired",
                             sx->mask);
        slist_delete(&szlines, i);
    }
}

static void free_szline_entry(SList * slist, void *item)
{
    SXLine *sx = item;

    /* Remove the SZLINE from all the servers */
    anope_cmd_unszline(sx->mask);

    /* Free the structure */
    free(sx->mask);
    free(sx->by);
    free(sx->reason);
    free(sx);
}

/* item1 is not an SXLine pointer, but a char
 */

static int is_szline_entry_equal(SList * slist, void *item1, void *item2)
{
    char *sx1 = item1;
    SXLine *sx2 = item2;

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
    NickCore *nc1 = item1, *nc2 = item2;
    if (!nc1 || !nc2)
        return -1;              /* To tell to continue */
    return stricmp(nc1->display, nc2->display);
}

/* Callback function used when an admin list entry is deleted */

static void free_adminlist_entry(SList * slist, void *item)
{
    NickCore *nc = item;
    nc->flags &= ~NI_SERVICES_ADMIN;
}

/*************************************************************************/

/* Callback function used to sort the oper list */

static int compare_operlist_entries(SList * slist, void *item1,
                                    void *item2)
{
    NickCore *nc1 = item1, *nc2 = item2;
    if (!nc1 || !nc2)
        return -1;              /* To tell to continue */
    return stricmp(nc1->display, nc2->display);
}

/* Callback function used when an oper list entry is deleted */

static void free_operlist_entry(SList * slist, void *item)
{
    NickCore *nc = item;
    nc->flags &= ~NI_SERVICES_OPER;
}

/*************************************************************************/

#ifdef DEBUG_COMMANDS

static int do_matchwild(User * u)
{
    char *pat = strtok(NULL, " ");
    char *str = strtok(NULL, " ");
    if (pat && str)
        notice_user(s_OperServ, u, "%d", match_wild(pat, str));
    else
        notice_user(s_OperServ, u, "Syntax error.");
    return MOD_CONT;
}

#endif                          /* DEBUG_COMMANDS */

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
            && (time(NULL) - DefContimer >= dotime(DefConTimeOut))) {
            DefConLevel = level;
            send_event(EVENT_DEFCON_LEVEL, 1, strLevel);
            alog("Defcon level timeout, returning to lvl %d", level);
            anope_cmd_global(s_OperServ,
                             getstring2(NULL, OPER_DEFCON_WALL),
                             s_OperServ, level);
            if (GlobalOnDefcon) {
                if (DefConOffMessage) {
                    oper_global(NULL, "%s", DefConOffMessage);
                } else {
                    oper_global(NULL, getstring(NULL, DEFCON_GLOBAL),
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
void runDefCon(void)
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
                if ((newmodes = defconReverseModes(DefConChanModes))) {
                    alog("DEFCON: setting %s on all chan's", newmodes);
                    do_mass_mode(newmodes);
                }
                DefConModesSet = 0;
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
    int i = 0;
    if (!modes) {
        return NULL;
    }
    if (!(newmodes = malloc(sizeof(char) * strlen(modes) + 1))) {
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


/*************************************************************************/
