/* OperServ functions.
 *
 * (C) 2003 Anope Team
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

extern Module *mod_current_module;
extern int mod_current_op;
extern User *mod_current_user;
extern ModuleHash *MODULE_HASH[MAX_CMD_HASH];
/*************************************************************************/

struct clone {
    char *host;
    long time;
};

/* List of most recent users - statically initialized to zeros */
static struct clone clonelist[CLONE_DETECT_SIZE];

/* Which hosts have we warned about, and when? This is used to keep us
 * from sending out notices over and over for clones from the same host. */
static struct clone warnings[CLONE_DETECT_SIZE];

/* List of Services administrators */
SList servadmins;
/* List of Services operators */
SList servopers;
/* AKILL, SGLINE, SQLINE and SZLINE lists */
SList akills, sglines, sqlines, szlines;

/*************************************************************************/

static void get_operserv_stats(long *nrec, long *memuse);

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

static int do_help(User * u);
static int do_global(User * u);
static int do_stats(User * u);
static int do_admin(User * u);
static int do_oper(User * u);
static int do_os_mode(User * u);
static int do_clearmodes(User * u);
static int do_os_kick(User * u);
static int do_akill(User * u);
static int do_sgline(User * u);
static int do_sqline(User * u);
static int do_szline(User * u);
static int do_set(User * u);
static int do_noop(User * u);
static int do_jupe(User * u);
static int do_raw(User * u);
static int do_update(User * u);
static int do_reload(User * u);
static int do_os_quit(User * u);
static int do_shutdown(User * u);
static int do_restart(User * u);
static int do_ignorelist(User * u);
static int do_clearignore(User * u);
static int do_killclones(User * u);
static int do_chanlist(User * u);
static int do_userlist(User * u);
static int do_ignoreuser(User * u);
static int do_staff(User * u);
static int do_defcon(User * u);
static int do_chankill(User * u);
static void defcon_sendlvls(User * u);
char *defconReverseModes(const char *modes);
int DefConModesSet = 0;
time_t DefContimer;
void runDefCon(void);
void resetDefCon(int level);
void oper_global(char *nick, char *fmt, ...);

#ifdef USE_MODULES
int do_modload(User * u);
int do_modunload(User * u);
int do_modlist(User * u);
int do_modinfo(User * u);
static int showModuleCmdLoaded(CommandHash * cmdList, char *mod_name,
                               User * u);
static int showModuleMsgLoaded(MessageHash * msgList, char *mod_name,
                               User * u);
#endif

static int do_operumodes(User * u);
static int do_svsnick(User * u);
static int do_operoline(User * u);

#ifdef DEBUG_COMMANDS
static int send_clone_lists(User * u);
static int do_matchwild(User * u);
#endif

/* OperServ restart needs access to this if were gonna avoid sending ourself a signal */
extern int do_restart_services(void);
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
    c = createCommand("HELP",       do_help,       NULL,  -1,                   -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("GLOBAL",     do_global,     NULL,  OPER_HELP_GLOBAL,     -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("STATS",      do_stats,      NULL,  OPER_HELP_STATS,      -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("UPTIME",     do_stats,      NULL,  OPER_HELP_STATS,      -1,-1,-1,-1); addCoreCommand(OPERSERV,c);

    /* Anyone can use the LIST option to the ADMIN and OPER commands; those
     * routines check privileges to ensure that only authorized users
     * modify the list. */
    c = createCommand("ADMIN",      do_admin,      NULL,  OPER_HELP_ADMIN,      -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("OPER",       do_oper,       NULL,  OPER_HELP_OPER,       -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("STAFF",      do_staff,      NULL,  OPER_HELP_STAFF,      -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    /* Similarly, anyone can use *NEWS LIST, but *NEWS {ADD,DEL} are
     * reserved for Services admins. */
    c = createCommand("LOGONNEWS",  do_logonnews,  NULL,  NEWS_HELP_LOGON,      -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("OPERNEWS",   do_opernews,   NULL,  NEWS_HELP_OPER,       -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("RANDOMNEWS", do_randomnews, NULL,  NEWS_HELP_RANDOM,     -1,-1,-1,-1); addCoreCommand(OPERSERV,c);

    /* Commands for Services opers: */
    c = createCommand("MODE",       do_os_mode,    is_services_oper,OPER_HELP_MODE, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("CLEARMODES", do_clearmodes, is_services_oper,OPER_HELP_CLEARMODES, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("KICK",       do_os_kick,    is_services_oper,OPER_HELP_KICK, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("KILLCLONES", do_killclones, is_services_oper,OPER_HELP_KILLCLONES, -1,-1,-1, -1); addCoreCommand(OPERSERV,c);
    c = createCommand("AKILL",      do_akill,      is_services_oper,OPER_HELP_AKILL, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("SGLINE",     do_sgline,     is_services_oper,OPER_HELP_SGLINE, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("SQLINE",     do_sqline,     is_services_oper,OPER_HELP_SQLINE, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("SZLINE",     do_szline,     is_services_oper,OPER_HELP_SZLINE, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);

    /* Commands for Services admins: */
    c = createCommand("SET",        do_set,        is_services_admin,OPER_HELP_SET, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("SET READONLY", NULL,         NULL,OPER_HELP_SET_READONLY, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("SET LOGCHAN",NULL,         NULL,OPER_HELP_SET_LOGCHAN, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("SET DEBUG",  NULL,          NULL,OPER_HELP_SET_DEBUG, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("SET NOEXPIRE",NULL,         NULL,OPER_HELP_SET_NOEXPIRE, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("SET SUPERADMIN",NULL,         NULL,OPER_HELP_SET_SUPERADMIN, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("SVSNICK",       do_svsnick,    is_services_admin,OPER_HELP_SVSNICK, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("UMODE",     do_operumodes,     is_services_admin,OPER_HELP_UMODE, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("NOOP",       do_noop,       is_services_admin,OPER_HELP_NOOP, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("JUPE",       do_jupe,       is_services_admin,OPER_HELP_JUPE, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("RAW",        do_raw,        is_services_admin,OPER_HELP_RAW, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("IGNORE",     do_ignoreuser,     is_services_admin,OPER_HELP_IGNORE, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("OLINE",     do_operoline,     is_services_admin,OPER_HELP_OLINE, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("UPDATE",     do_update,     is_services_admin,OPER_HELP_UPDATE, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("RELOAD",     do_reload,     is_services_admin,OPER_HELP_RELOAD, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("QUIT",       do_os_quit,    is_services_admin,OPER_HELP_QUIT, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("SHUTDOWN",   do_shutdown,   is_services_admin,OPER_HELP_SHUTDOWN, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("RESTART",    do_restart,    is_services_admin,OPER_HELP_RESTART, -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
#ifndef STREAMLINED
    c = createCommand("SESSION",    do_session,    is_services_admin,OPER_HELP_SESSION, -1,-1,-1, -1); addCoreCommand(OPERSERV,c);
    c = createCommand("EXCEPTION",  do_exception,  is_services_admin,OPER_HELP_EXCEPTION, -1,-1,-1, -1); addCoreCommand(OPERSERV,c);
#endif
    c = createCommand("CHANLIST",   do_chanlist,   is_services_admin,OPER_HELP_CHANLIST,   -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("USERLIST",   do_userlist,   is_services_admin,OPER_HELP_USERLIST,   -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("CACHE",      do_cache,      is_services_admin,OPER_HELP_CACHE,      -1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("DEFCON", do_defcon,   is_services_admin, OPER_HELP_DEFCON,-1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    c = createCommand("CHANKILL", do_chankill,   is_services_admin, OPER_HELP_CHANKILL,-1,-1,-1,-1); addCoreCommand(OPERSERV,c);
    /* Commands for Services root: */
#ifdef USE_MODULES
    c = createCommand("MODLOAD", do_modload,   is_services_root, -1,-1,-1,-1,OPER_HELP_MODLOAD); addCoreCommand(OPERSERV,c);
    c = createCommand("MODUNLOAD", do_modunload,   is_services_root, -1,-1,-1,-1,OPER_HELP_MODUNLOAD); addCoreCommand(OPERSERV,c);
    c = createCommand("MODLIST", do_modlist,   is_services_root, -1,-1,-1,-1,OPER_HELP_MODLIST); addCoreCommand(OPERSERV,c);
    c = createCommand("MODINFO", do_modinfo,   is_services_root, -1,-1,-1,-1,OPER_HELP_MODINFO); addCoreCommand(OPERSERV,c);
#endif
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

static void get_operserv_stats(long *nrec, long *memuse)
{
    int i;
    long mem = 0, count = 0, mem2 = 0, count2 = 0;
    Akill *ak;
    SXLine *sx;

    if (CheckClones) {
        mem = sizeof(struct clone) * CLONE_DETECT_SIZE * 2;
        for (i = 0; i < CLONE_DETECT_SIZE; i++) {
            if (clonelist[i].host) {
                count++;
                mem += strlen(clonelist[i].host) + 1;
            }
            if (warnings[i].host) {
                count++;
                mem += strlen(warnings[i].host) + 1;
            }
        }
    }

    count += akills.count;
    mem += akills.capacity;
    mem += akills.count * sizeof(Akill);

    for (i = 0; i < akills.count; i++) {
        ak = akills.list[i];
        mem += strlen(ak->user) + 1;
        mem += strlen(ak->host) + 1;
        mem += strlen(ak->by) + 1;
        mem += strlen(ak->reason) + 1;
    }

    if (ircd->sgline) {
        count += sglines.count;
        mem += sglines.capacity;
        mem += sglines.count * sizeof(SXLine);

        for (i = 0; i < sglines.count; i++) {
            sx = sglines.list[i];
            mem += strlen(sx->mask) + 1;
            mem += strlen(sx->by) + 1;
            mem += strlen(sx->reason) + 1;
        }
    }
    if (ircd->sqline) {
        count += sqlines.count;
        mem += sqlines.capacity;
        mem += sqlines.count * sizeof(SXLine);

        for (i = 0; i < sqlines.count; i++) {
            sx = sqlines.list[i];
            mem += strlen(sx->mask) + 1;
            mem += strlen(sx->by) + 1;
            mem += strlen(sx->reason) + 1;
        }
    }
    if (ircd->szline) {
        count += szlines.count;
        mem += szlines.capacity;
        mem += szlines.count * sizeof(SXLine);

        for (i = 0; i < szlines.count; i++) {
            sx = szlines.list[i];
            mem += strlen(sx->mask) + 1;
            mem += strlen(sx->by) + 1;
            mem += strlen(sx->reason) + 1;
        }
    }


    get_news_stats(&count2, &mem2);
    count += count2;
    mem += mem2;
    get_exception_stats(&count2, &mem2);
    count += count2;
    mem += mem2;

    *nrec = count;
    *memuse = mem;
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
    int16 tmp16;
    int32 tmp32;
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
    int16 i, n, ver, c;
    HostCache *hc, **hclast, *hcprev;
    int16 tmp16;
    int32 tmp32;
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
        int32 tmp32;
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

    if (ver >= 12) {
        for (i = 0; i < 1024 && !failed; i++) {
            hclast = &hcache[i];
            hcprev = NULL;

            while ((c = getc_db(f)) != 0) {
                if (c != 1)
                    fatal("Invalid format in %s", OperDBName);

                hc = scalloc(1, sizeof(HostCache));

                SAFE(read_string(&hc->host, f));
                SAFE(read_int16(&tmp16, f));
                hc->status = tmp16;
                SAFE(read_int32(&tmp32, f));
                hc->used = tmp32;

                *hclast = hc;
                hclast = &hc->next;
                hc->prev = hcprev;
                hcprev = hc;
            }                   /* while (getc_db(f) != 0) */

            *hclast = NULL;
        }                       /* for (i) */
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
    HostCache *hc;

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

    for (i = 0; i < 1024; i++) {
        for (hc = hcache[i]; hc; hc = hc->next) {
            /* Don't save in-progress scans */
            if (hc->status < HC_NORMAL)
                continue;

            SAFE(write_int8(1, f));

            SAFE(write_string(hc->host, f));
            SAFE(write_int16(hc->status, f));
            SAFE(write_int32(hc->used, f));

        }                       /* for (hc) */
        SAFE(write_int8(0, f));
    }                           /* for (i) */

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
                   &szlines, hcache[0]);
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
#ifndef STREAMLINED
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
                 common_get_vhost(user));
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
#endif                          /* !STREAMLINED */
}

/*************************************************************************/

#ifdef DEBUG_COMMANDS

/* Send clone arrays to given nick. */

static int send_clone_lists(User * u)
{
    int i;

    if (!CheckClones) {
        notice(s_OperServ, u->nick, "CheckClones not enabled.");
        return MOD_CONT;
    }

    notice(s_OperServ, u->nick, "clonelist[]");
    for (i = 0; i < CLONE_DETECT_SIZE; i++) {
        if (clonelist[i].host)
            notice(s_OperServ, u->nick, "    %10ld  %s", clonelist[i].time,
                   clonelist[i].host ? clonelist[i].host : "(null)");
    }
    notice(s_OperServ, u->nick, "warnings[]");
    for (i = 0; i < CLONE_DETECT_SIZE; i++) {
        if (clonelist[i].host)
            notice(s_OperServ, u->nick, "    %10ld  %s", warnings[i].time,
                   warnings[i].host ? warnings[i].host : "(null)");
    }
    return MOD_CONT;
}

#endif                          /* DEBUG_COMMANDS */

/*************************************************************************/
/*********************** OperServ command functions **********************/
/*************************************************************************/

/* HELP command. */

static int do_help(User * u)
{
    const char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_OperServ, u, OPER_HELP);
        if (is_services_oper(u))
            notice_help(s_OperServ, u, OPER_HELP_OPER_CMD);
        if (is_services_admin(u))
            notice_help(s_OperServ, u, OPER_HELP_ADMIN_CMD);
#ifdef USE_MODULES
        if (is_services_root(u))
            notice_help(s_OperServ, u, OPER_HELP_ROOT_CMD);
#endif
        moduleDisplayHelp(5, u);
        notice_help(s_OperServ, u, OPER_HELP_LOGGED);
    } else {
        mod_help_cmd(s_OperServ, u, OPERSERV, cmd);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_global(User * u)
{
    char *msg = strtok(NULL, "");

    if (!msg) {
        syntax_error(s_OperServ, u, "GLOBAL", OPER_GLOBAL_SYNTAX);
        return MOD_CONT;
    }
    if (WallOSGlobal)
        anope_cmd_global(s_OperServ, "\2%s\2 just used GLOBAL command.",
                         u->nick);
    oper_global(u->nick, "%s", msg);
    return MOD_CONT;
}

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

/*************************************************************************/

/* STATS command. */

static int do_stats(User * u)
{
    time_t uptime = time(NULL) - start_time;
    char *extra = strtok(NULL, "");
    int days = uptime / 86400, hours = (uptime / 3600) % 24,
        mins = (uptime / 60) % 60, secs = uptime % 60;
    struct tm *tm;
    char timebuf[64];

    if (extra && stricmp(extra, "ALL") != 0) {
        if (stricmp(extra, "AKILL") == 0) {
            int timeout;
            /* AKILLs */
            notice_lang(s_OperServ, u, OPER_STATS_AKILL_COUNT,
                        akills.count);
            timeout = AutokillExpiry + 59;
            if (timeout >= 172800)
                notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_DAYS,
                            timeout / 86400);
            else if (timeout >= 86400)
                notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_DAY);
            else if (timeout >= 7200)
                notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_HOURS,
                            timeout / 3600);
            else if (timeout >= 3600)
                notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_HOUR);
            else if (timeout >= 120)
                notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_MINS,
                            timeout / 60);
            else if (timeout >= 60)
                notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_MIN);
            else
                notice_lang(s_OperServ, u, OPER_STATS_AKILL_EXPIRE_NONE);
            if (ircd->sgline) {
                /* SGLINEs */
                notice_lang(s_OperServ, u, OPER_STATS_SGLINE_COUNT,
                            sglines.count);
                timeout = SGLineExpiry + 59;
                if (timeout >= 172800)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SGLINE_EXPIRE_DAYS,
                                timeout / 86400);
                else if (timeout >= 86400)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SGLINE_EXPIRE_DAY);
                else if (timeout >= 7200)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SGLINE_EXPIRE_HOURS,
                                timeout / 3600);
                else if (timeout >= 3600)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SGLINE_EXPIRE_HOUR);
                else if (timeout >= 120)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SGLINE_EXPIRE_MINS,
                                timeout / 60);
                else if (timeout >= 60)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SGLINE_EXPIRE_MIN);
                else
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SGLINE_EXPIRE_NONE);
            }
            if (ircd->sqline) {
                /* SQLINEs */
                notice_lang(s_OperServ, u, OPER_STATS_SQLINE_COUNT,
                            sqlines.count);
                timeout = SQLineExpiry + 59;
                if (timeout >= 172800)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SQLINE_EXPIRE_DAYS,
                                timeout / 86400);
                else if (timeout >= 86400)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SQLINE_EXPIRE_DAY);
                else if (timeout >= 7200)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SQLINE_EXPIRE_HOURS,
                                timeout / 3600);
                else if (timeout >= 3600)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SQLINE_EXPIRE_HOUR);
                else if (timeout >= 120)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SQLINE_EXPIRE_MINS,
                                timeout / 60);
                else if (timeout >= 60)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SQLINE_EXPIRE_MIN);
                else
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SQLINE_EXPIRE_NONE);
            }
            if (ircd->szline) {
                /* SZLINEs */
                notice_lang(s_OperServ, u, OPER_STATS_SZLINE_COUNT,
                            szlines.count);
                timeout = SZLineExpiry + 59;
                if (timeout >= 172800)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SZLINE_EXPIRE_DAYS,
                                timeout / 86400);
                else if (timeout >= 86400)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SZLINE_EXPIRE_DAY);
                else if (timeout >= 7200)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SZLINE_EXPIRE_HOURS,
                                timeout / 3600);
                else if (timeout >= 3600)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SZLINE_EXPIRE_HOUR);
                else if (timeout >= 120)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SZLINE_EXPIRE_MINS,
                                timeout / 60);
                else if (timeout >= 60)
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SZLINE_EXPIRE_MIN);
                else
                    notice_lang(s_OperServ, u,
                                OPER_STATS_SZLINE_EXPIRE_NONE);
            }
            return MOD_CONT;
        } else if (!stricmp(extra, "RESET")) {
            if (is_services_admin(u)) {
                maxusercnt = usercnt;
                notice_lang(s_OperServ, u, OPER_STATS_RESET);
            } else {
                notice_lang(s_OperServ, u, PERMISSION_DENIED);
            }
            return MOD_CONT;
        } else {
            notice_lang(s_OperServ, u, OPER_STATS_UNKNOWN_OPTION, extra);
        }
    }

    notice_lang(s_OperServ, u, OPER_STATS_CURRENT_USERS, usercnt, opcnt);
    tm = localtime(&maxusertime);
    strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT,
                  tm);
    notice_lang(s_OperServ, u, OPER_STATS_MAX_USERS, maxusercnt, timebuf);
    if (days > 1) {
        notice_lang(s_OperServ, u, OPER_STATS_UPTIME_DHMS,
                    days, hours, mins, secs);
    } else if (days == 1) {
        notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1DHMS,
                    days, hours, mins, secs);
    } else {
        if (hours > 1) {
            if (mins != 1) {
                if (secs != 1) {
                    notice_lang(s_OperServ, u, OPER_STATS_UPTIME_HMS,
                                hours, mins, secs);
                } else {
                    notice_lang(s_OperServ, u, OPER_STATS_UPTIME_HM1S,
                                hours, mins, secs);
                }
            } else {
                if (secs != 1) {
                    notice_lang(s_OperServ, u, OPER_STATS_UPTIME_H1MS,
                                hours, mins, secs);
                } else {
                    notice_lang(s_OperServ, u, OPER_STATS_UPTIME_H1M1S,
                                hours, mins, secs);
                }
            }
        } else if (hours == 1) {
            if (mins != 1) {
                if (secs != 1) {
                    notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1HMS,
                                hours, mins, secs);
                } else {
                    notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1HM1S,
                                hours, mins, secs);
                }
            } else {
                if (secs != 1) {
                    notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1H1MS,
                                hours, mins, secs);
                } else {
                    notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1H1M1S,
                                hours, mins, secs);
                }
            }
        } else {
            if (mins != 1) {
                if (secs != 1) {
                    notice_lang(s_OperServ, u, OPER_STATS_UPTIME_MS,
                                mins, secs);
                } else {
                    notice_lang(s_OperServ, u, OPER_STATS_UPTIME_M1S,
                                mins, secs);
                }
            } else {
                if (secs != 1) {
                    notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1MS,
                                mins, secs);
                } else {
                    notice_lang(s_OperServ, u, OPER_STATS_UPTIME_1M1S,
                                mins, secs);
                }
            }
        }
    }

    if (extra && stricmp(extra, "ALL") == 0 && is_services_admin(u)) {
        long count, mem;

        notice_lang(s_OperServ, u, OPER_STATS_BYTES_READ,
                    total_read / 1024);
        notice_lang(s_OperServ, u, OPER_STATS_BYTES_WRITTEN,
                    total_written / 1024);

        get_user_stats(&count, &mem);
        notice_lang(s_OperServ, u, OPER_STATS_USER_MEM, count,
                    (mem + 512) / 1024);
        get_channel_stats(&count, &mem);
        notice_lang(s_OperServ, u, OPER_STATS_CHANNEL_MEM, count,
                    (mem + 512) / 1024);
        get_core_stats(&count, &mem);
        notice_lang(s_OperServ, u, OPER_STATS_GROUPS_MEM, count,
                    (mem + 512) / 1024);
        get_aliases_stats(&count, &mem);
        notice_lang(s_OperServ, u, OPER_STATS_ALIASES_MEM, count,
                    (mem + 512) / 1024);
        get_chanserv_stats(&count, &mem);
        notice_lang(s_OperServ, u, OPER_STATS_CHANSERV_MEM, count,
                    (mem + 512) / 1024);
        get_botserv_stats(&count, &mem);
        notice_lang(s_OperServ, u, OPER_STATS_BOTSERV_MEM, count,
                    (mem + 512) / 1024);
        get_operserv_stats(&count, &mem);
        notice_lang(s_OperServ, u, OPER_STATS_OPERSERV_MEM, count,
                    (mem + 512) / 1024);
        get_session_stats(&count, &mem);
        notice_lang(s_OperServ, u, OPER_STATS_SESSIONS_MEM, count,
                    (mem + 512) / 1024);
#ifdef USE_THREADS
        if (ProxyDetect) {
            get_proxy_stats(&count, &mem);
            notice_lang(s_OperServ, u, OPER_STATS_PROXY_MEM, count,
                        (mem + 512) / 1024);
        }
#endif
    }
    return MOD_CONT;
}

/*************************************************************************/

/* make Services ignore users for a certain time */

static int do_ignoreuser(User * u)
{
    char *cmd = strtok(NULL, " ");
    int t;

    if (!cmd) {
        notice_lang(s_OperServ, u, OPER_IGNORE_SYNTAX);
        return MOD_CONT;
    }

    if (!stricmp(cmd, "ADD")) {

        char *time = strtok(NULL, " ");
        char *nick = strtok(NULL, " ");
        char *rest = strtok(NULL, "");

        if (!nick) {
            notice_lang(s_OperServ, u, OPER_IGNORE_SYNTAX);
            return MOD_CONT;
        } else if (!time) {
            notice_lang(s_OperServ, u, OPER_IGNORE_SYNTAX);
            return MOD_CONT;
        } else {
            t = dotime(time);
            rest = NULL;

            if (t <= -1) {
                notice_lang(s_OperServ, u, OPER_IGNORE_VALID_TIME);
                return MOD_CONT;
            } else if (t == 0) {
                t = 157248000;  /* if 0 is given, we set time to 157248000 seconds == 5 years (let's hope the next restart will  be before that time ;-)) */
                add_ignore(nick, t);
                notice_lang(s_OperServ, u, OPER_IGNORE_PERM_DONE, nick);
            } else {
                add_ignore(nick, t);
                notice_lang(s_OperServ, u, OPER_IGNORE_TIME_DONE, nick,
                            time);
            }
        }
    } else if (!stricmp(cmd, "LIST")) {
        do_ignorelist(u);
    }

    else if (!stricmp(cmd, "DEL")) {
        char *nick = strtok(NULL, " ");
        if (!nick) {
            notice_lang(s_OperServ, u, OPER_IGNORE_SYNTAX);
        } else {
            if (get_ignore(nick) == 0) {
                notice_lang(s_OperServ, u, OPER_IGNORE_LIST_NOMATCH, nick);
                return MOD_CONT;
            } else {
                delete_ignore(nick);
                notice_lang(s_OperServ, u, OPER_IGNORE_DEL_DONE, nick);
            }
        }
    } else if (!stricmp(cmd, "CLEAR")) {
        do_clearignore(u);

    } else
        notice_lang(s_OperServ, u, OPER_IGNORE_SYNTAX);
    return MOD_CONT;
}

/*************************************************************************/

/* deletes a nick from the ignore list  */

void delete_ignore(const char *nick)
{
    IgnoreData *ign, *prev;
    IgnoreData **whichlist;

    if (!nick || !*nick) {
        return;
    }

    whichlist = &ignore[tolower(nick[0])];

    for (ign = *whichlist, prev = NULL; ign; prev = ign, ign = ign->next) {
        if (stricmp(ign->who, nick) == 0)
            break;
    }
    if (prev)
        prev->next = ign->next;
    else
        *whichlist = ign->next;
    free(ign);
    ign = NULL;
}

/*************************************************************************/

/* shows the Services ignore list */

static int do_ignorelist(User * u)
{
    int sent_header = 0;
    IgnoreData *id;
    int i;

    for (i = 0; i < 256; i++) {
        for (id = ignore[i]; id; id = id->next) {
            if (!sent_header) {
                notice_lang(s_OperServ, u, OPER_IGNORE_LIST);
                sent_header = 1;
            }
            notice(s_OperServ, u->nick, "%s", id->who);
        }
    }
    if (!sent_header)
        notice_lang(s_OperServ, u, OPER_IGNORE_LIST_EMPTY);
    return MOD_CONT;
}

/**************************************************************************/
/* Cleares the Services ignore list */

static int do_clearignore(User * u)
{
    IgnoreData *id = NULL, *next = NULL;
    int i;
    for (i = 0; i < 256; i++) {
        for (id = ignore[i]; id; id = next) {
            next = id->next;
            free(id);
            if (!next) {
                ignore[i] = NULL;
            }
        }
    }
    notice_lang(s_OperServ, u, OPER_IGNORE_LIST_CLEARED);
    return MOD_CONT;
}

/**************************************************************************/

/* Channel mode changing (MODE command). */

static int do_os_mode(User * u)
{
    int ac;
    char **av;
    char *chan = strtok(NULL, " "), *modes = strtok(NULL, "");
    Channel *c;

    if (!chan || !modes) {
        syntax_error(s_OperServ, u, "MODE", OPER_MODE_SYNTAX);
        return MOD_CONT;
    }

    if (!(c = findchan(chan))) {
        notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
        return MOD_CONT;
    } else if ((ircd->adminmode) && (!is_services_admin(u))
               && (c->mode & ircd->adminmode)) {
        notice_lang(s_OperServ, u, PERMISSION_DENIED);
        return MOD_CONT;
    } else {
        anope_cmd_mode(s_OperServ, chan, "%s", modes);

        ac = split_buf(modes, &av, 1);
        chan_set_modes(s_OperServ, c, ac, av, 0);

        if (WallOSMode)
            anope_cmd_global(s_OperServ, "%s used MODE %s on %s", u->nick,
                             modes, chan);
    }
    return MOD_CONT;
}

/**************************************************************************/

/**
 * Change any user's UMODES
 *
 * modified to be part of the SuperAdmin directive -jester
 * check user flag for SuperAdmin -rob
 */
static int do_operumodes(User * u)
{
    char *nick = strtok(NULL, " ");
    char *modes = strtok(NULL, "");

    User *u2;

    if (!ircd->umode) {
        notice_lang(s_OperServ, u, OPER_UMODE_UNSUPPORTED);
        return MOD_CONT;
    }

    /* Only allow this if SuperAdmin is enabled */
    if (!u->isSuperAdmin) {
        notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ONLY);
        return MOD_CONT;
    }

    if (!nick || !modes) {
        syntax_error(s_OperServ, u, "UMODE", OPER_UMODE_SYNTAX);
        return MOD_CONT;
    }

    /**
     * Only accept a +/- mode string
     *-rob
     **/
    if ((modes[0] != '+') && (modes[0] != '-')) {
        syntax_error(s_OperServ, u, "UMODE", OPER_UMODE_SYNTAX);
        return MOD_CONT;
    }
    if (!(u2 = finduser(nick))) {
        notice_lang(s_OperServ, u, NICK_X_NOT_IN_USE, nick);
    } else {
        anope_cmd_mode(s_OperServ, nick, "%s", modes);

        common_svsmode(u2, modes, NULL);

        notice_lang(s_OperServ, u, OPER_UMODE_SUCCESS, nick);
        notice_lang(s_OperServ, u2, OPER_UMODE_CHANGED, u->nick);

        if (WallOSMode)
            anope_cmd_global(s_OperServ, "\2%s\2 used UMODE on %s",
                             u->nick, nick);
    }
    return MOD_CONT;
}

/**************************************************************************/

/**
 * give Operflags to any user
 *
 * modified to be part of the SuperAdmin directive -jester
 * check u-> for SuperAdmin -rob
 */

static int do_operoline(User * u)
{
    char *nick = strtok(NULL, " ");
    char *flags = strtok(NULL, "");
    User *u2 = NULL;

    if (!ircd->omode) {
        notice_lang(s_OperServ, u, OPER_SVSO_UNSUPPORTED);
        return MOD_CONT;
    }

    /* Only allow this if SuperAdmin is enabled */
    if (!u->isSuperAdmin) {
        notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ONLY);
        return MOD_CONT;
    }

    if (!nick || !flags) {
        syntax_error(s_OperServ, u, "OLINE", OPER_OLINE_SYNTAX);
        return MOD_CONT;
    } else {
        u2 = finduser(nick);

/* let's check whether the user is online */

        if (!finduser(nick)) {
            notice_lang(s_OperServ, u, NICK_X_NOT_IN_USE, nick);
        } else if (u2 && flags[0] == '+') {
            anope_cmd_svso(s_OperServ, nick, flags);
            anope_cmd_mode(s_OperServ, nick, "+o");
            common_svsmode(u2, "+o", NULL);
            notice_lang(s_OperServ, u2, OPER_OLINE_IRCOP);
            notice_lang(s_OperServ, u, OPER_OLINE_SUCCESS, flags, nick);
            anope_cmd_global(s_OperServ, "\2%s\2 used OLINE for %s",
                             u->nick, nick);
        } else if (u2 && flags[0] == '-') {
            anope_cmd_svso(s_OperServ, nick, flags);
            notice_lang(s_OperServ, u, OPER_OLINE_SUCCESS, flags, nick);
            anope_cmd_global(s_OperServ, "\2%s\2 used OLINE for %s",
                             u->nick, nick);
        } else
            syntax_error(s_OperServ, u, "OLINE", OPER_OLINE_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

/* Clear all modes from a channel. */

static int do_clearmodes(User * u)
{
    char *s;
    int i;
    char *argv[2];
    char *chan = strtok(NULL, " ");
    Channel *c;
    int all = 0;
    int count;                  /* For saving ban info */
    char **bans;                /* For saving ban info */
    int exceptcount;            /* For saving except info */
    char **excepts;             /* For saving except info */
    int invitecount;            /* For saving invite info */
    char **invites;             /* For saving invite info */
    struct c_userlist *cu, *next;

    if (!chan) {
        syntax_error(s_OperServ, u, "CLEARMODES", OPER_CLEARMODES_SYNTAX);
        return MOD_CONT;
    } else if (!(c = findchan(chan))) {
        notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
        return MOD_CONT;
    } else if (c->bouncy_modes) {
        notice_lang(s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
        return MOD_CONT;
    } else {
        s = strtok(NULL, " ");
        if (s) {
            if (stricmp(s, "ALL") == 0) {
                all = 1;
            } else {
                syntax_error(s_OperServ, u, "CLEARMODES",
                             OPER_CLEARMODES_SYNTAX);
                return MOD_CONT;
            }
        }

        if (WallOSClearmodes)
            anope_cmd_global(s_OperServ, "%s used CLEARMODES%s on %s",
                             u->nick, all ? " ALL" : "", chan);

        if (all) {
            /* Clear mode +o */
            for (cu = c->users; cu; cu = next) {
                next = cu->next;

                if (!chan_has_user_status(c, cu->user, CUS_OP))
                    continue;

                argv[0] = sstrdup("-o");
                argv[1] = cu->user->nick;

                anope_cmd_mode(s_OperServ, c->name, "-o %s",
                               cu->user->nick);
                chan_set_modes(s_OperServ, c, 2, argv, 0);

                free(argv[0]);
            }

            /* Clear mode +v */
            for (cu = c->users; cu; cu = next) {
                next = cu->next;

                if (!chan_has_user_status(c, cu->user, CUS_VOICE))
                    continue;

                argv[0] = sstrdup("-v");
                argv[1] = sstrdup(cu->user->nick);

                anope_cmd_mode(s_OperServ, c->name, "-v %s",
                               cu->user->nick);
                chan_set_modes(s_OperServ, c, 2, argv, 0);

                free(argv[0]);
            }
            /* Clear mode +h */
            if (ircd->halfop) {
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;

                    if (!chan_has_user_status(c, cu->user, CUS_HALFOP))
                        continue;

                    argv[0] = sstrdup("-h");
                    argv[1] = sstrdup(cu->user->nick);

                    anope_cmd_mode(s_OperServ, c->name, "-h %s",
                                   cu->user->nick);
                    chan_set_modes(s_OperServ, c, 2, argv, 0);

                    free(argv[0]);
                }
            }

            /* Clear mode Owners */
            if (ircd->owner) {
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;

                    if (!chan_has_user_status(c, cu->user, CUS_OWNER))
                        continue;

                    argv[0] = sstrdup(ircd->ownerunset);
                    argv[1] = sstrdup(cu->user->nick);

                    anope_cmd_mode(s_OperServ, c->name, "%s %s",
                                   ircd->ownerunset, cu->user->nick);
                    chan_set_modes(s_OperServ, c, 2, argv, 0);

                    free(argv[0]);
                }
            }

            /* Clear mode protected or admins */
            if (ircd->protect || ircd->admin) {
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;

                    if (!chan_has_user_status(c, cu->user, CUS_PROTECT))
                        continue;

                    argv[0] = sstrdup("-a");
                    argv[1] = sstrdup(cu->user->nick);

                    anope_cmd_mode(s_OperServ, c->name, "-a %s",
                                   cu->user->nick);
                    chan_set_modes(s_OperServ, c, 2, argv, 0);

                    free(argv[0]);
                }
            }


        }

        if (c->mode) {
            /* Clear modes the bulk of the modes */
            anope_cmd_mode(s_OperServ, c->name, "%s", ircd->modestoremove);
            argv[0] = sstrdup(ircd->modestoremove);
            chan_set_modes(s_OperServ, c, 1, argv, 0);
            free(argv[0]);

            /* to prevent the internals from complaining send -k, -L, -f by themselves if we need
               to send them - TSL */
            if (c->key) {
                anope_cmd_mode(s_OperServ, c->name, "-k %s", c->key);
                argv[0] = sstrdup("-k");
                argv[1] = c->key;
                chan_set_modes(s_OperServ, c, 2, argv, 0);
                free(argv[0]);
            }
            if (ircd->Lmode && c->redirect) {
                anope_cmd_mode(s_OperServ, c->name, "-L %s", c->redirect);
                argv[0] = sstrdup("-L");
                argv[1] = c->redirect;
                chan_set_modes(s_OperServ, c, 2, argv, 0);
                free(argv[0]);
            }
            if (ircd->fmode && c->flood) {
                if (flood_mode_char_remove) {
                    anope_cmd_mode(s_OperServ, c->name, "%s %s",
                                   flood_mode_char_remove, c->flood);
                    argv[0] = sstrdup(flood_mode_char_remove);
                    argv[1] = c->flood;
                    chan_set_modes(s_OperServ, c, 2, argv, 0);
                    free(argv[0]);
                } else {
                    if (debug) {
                        alog("debug: flood_mode_char_remove was not set unable to remove flood/throttle modes");
                    }
                }
            }
        }

        /* Clear bans */
        count = c->bancount;
        bans = scalloc(sizeof(char *) * count, 1);

        for (i = 0; i < count; i++)
            bans[i] = sstrdup(c->bans[i]);

        for (i = 0; i < count; i++) {
            argv[0] = sstrdup("-b");
            argv[1] = bans[i];
            anope_cmd_mode(s_OperServ, c->name, "-b %s", argv[1]);
            chan_set_modes(s_OperServ, c, 2, argv, 0);
            free(argv[1]);
            free(argv[0]);
        }

        free(bans);

        excepts = NULL;

        if (ircd->except) {
            /* Clear excepts */
            exceptcount = c->exceptcount;
            excepts = scalloc(sizeof(char *) * exceptcount, 1);

            for (i = 0; i < exceptcount; i++)
                excepts[i] = sstrdup(c->excepts[i]);

            for (i = 0; i < exceptcount; i++) {
                argv[0] = sstrdup("-e");
                argv[1] = excepts[i];
                anope_cmd_mode(s_OperServ, c->name, "-e %s", argv[1]);
                chan_set_modes(s_OperServ, c, 2, argv, 0);
                free(argv[1]);
                free(argv[0]);
            }
            if (excepts) {
                free(excepts);
            }
        }

        if (ircd->invitemode) {
            /* Clear invites */
            invitecount = c->invitecount;
            invites = scalloc(sizeof(char *) * invitecount, 1);

            for (i = 0; i < invitecount; i++)
                invites[i] = sstrdup(c->invite[i]);

            for (i = 0; i < invitecount; i++) {
                argv[0] = sstrdup("-I");
                argv[1] = excepts[i];
                anope_cmd_mode(s_OperServ, c->name, "-I %s", argv[1]);
                chan_set_modes(s_OperServ, c, 2, argv, 0);
                free(argv[1]);
                free(argv[0]);
            }

            free(invites);
        }

    }

    if (all) {
        notice_lang(s_OperServ, u, OPER_CLEARMODES_ALL_DONE, chan);
    } else {
        notice_lang(s_OperServ, u, OPER_CLEARMODES_DONE, chan);
    }
    return MOD_CONT;
}

/*************************************************************************/

/* Kick a user from a channel (KICK command). */

static int do_os_kick(User * u)
{
    char *argv[3];
    char *chan, *nick, *s;
    Channel *c;

    chan = strtok(NULL, " ");
    nick = strtok(NULL, " ");
    s = strtok(NULL, "");
    if (!chan || !nick || !s) {
        syntax_error(s_OperServ, u, "KICK", OPER_KICK_SYNTAX);
        return MOD_CONT;
    }
    if (!(c = findchan(chan))) {
        notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (c->bouncy_modes) {
        notice_lang(s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
        return MOD_CONT;
    }
    anope_cmd_kick(s_OperServ, chan, nick, "%s (%s)", u->nick, s);
    if (WallOSKick)
        anope_cmd_global(s_OperServ, "%s used KICK on %s/%s", u->nick,
                         nick, chan);
    argv[0] = sstrdup(chan);
    argv[1] = sstrdup(nick);
    argv[2] = sstrdup(s);
    do_kick(s_OperServ, 3, argv);
    free(argv[2]);
    free(argv[1]);
    free(argv[0]);
    return MOD_CONT;
}

/*************************************************************************/

/* Forcefully change a user's nickname */

static int do_svsnick(User * u)
{
    char *nick = strtok(NULL, " ");
    char *newnick = strtok(NULL, " ");

    NickAlias *na;
    char *c;

    if (!ircd->svsnick) {
        notice_lang(s_OperServ, u, OPER_SVSNICK_UNSUPPORTED);
        return MOD_CONT;
    }

    /* Only allow this if SuperAdmin is enabled */
    if (!u->isSuperAdmin) {
        notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ONLY);
        return MOD_CONT;
    }

    if (!nick || !newnick) {
        syntax_error(s_OperServ, u, "SVSNICK", OPER_SVSNICK_SYNTAX);
        return MOD_CONT;
    }

    /* Truncate long nicknames to NICKMAX-2 characters */
    if (strlen(newnick) > (NICKMAX - 2)) {
        notice_lang(s_NickServ, u, NICK_X_TRUNCATED,
                    newnick, NICKMAX - 2, newnick);
        newnick[NICKMAX - 2] = '\0';
    }

    /* Check for valid characters */
    if (*newnick == '-' || isdigit(*newnick)) {
        notice_lang(s_OperServ, u, NICK_X_ILLEGAL, newnick);
        return MOD_CONT;
    }
#define isvalid(c) (((c) >= 'A' && (c) <= '~') || isdigit(c) || (c) == '-')
    for (c = newnick; *c && (c - newnick) < NICKMAX; c++) {
        if (!isvalid(*c) || isspace(*c)) {
            notice_lang(s_OperServ, u, NICK_X_ILLEGAL, nick);
            return MOD_CONT;
        }
    }

    /* Check for a nick in use or a forbidden/suspended nick */
    if (!finduser(nick)) {
        notice_lang(s_OperServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (finduser(newnick)) {
        notice_lang(s_NickServ, u, NICK_X_IN_USE, newnick);
    } else if ((na = findnick(newnick)) && (na->status & NS_VERBOTEN)) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, newnick);
    } else {
        notice_lang(s_OperServ, u, OPER_SVSNICK_NEWNICK, nick, newnick);
        anope_cmd_global(s_OperServ, "%s used SVSNICK to change %s to %s",
                         u->nick, nick, newnick);
        anope_cmd_svsnick(nick, newnick, time(NULL));
    }
    return MOD_CONT;
}

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
            && (match_wild_nocase(ak->host, host)
                || (vhost && match_wild_nocase(ak->host, vhost)))) {
            anope_cmd_akill(ak->user, ak->host, ak->by, ak->seton,
                            ak->expires, ak->reason);
            return 1;
        }
        if (ircd->nickip) {
            if (ip)
                if (match_wild_nocase(ak->user, username)
                    && match_wild_nocase(ak->host, ip)) {
                    anope_cmd_akill(ak->user, ak->host, ak->by, ak->seton,
                                    ak->expires, ak->reason);
                    return 1;
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

/* Lists an AKILL entry, prefixing it with the header if needed */

static int akill_list(int number, Akill * ak, User * u, int *sent_header)
{
    char mask[BUFSIZE];

    if (!ak)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_AKILL_LIST_HEADER);
        *sent_header = 1;
    }

    snprintf(mask, sizeof(mask), "%s@%s", ak->user, ak->host);
    notice_lang(s_OperServ, u, OPER_AKILL_LIST_FORMAT, number, mask,
                ak->reason);

    return 1;
}

/* Callback for enumeration purposes */

static int akill_list_callback(SList * slist, int number, void *item,
                               va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return akill_list(number, item, u, sent_header);
}

/* Lists an AKILL entry, prefixing it with the header if needed */

static int akill_view(int number, Akill * ak, User * u, int *sent_header)
{
    char mask[BUFSIZE];
    char timebuf[32], expirebuf[256];
    struct tm tm;

    if (!ak)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_AKILL_VIEW_HEADER);
        *sent_header = 1;
    }

    snprintf(mask, sizeof(mask), "%s@%s", ak->user, ak->host);
    tm = *localtime(&ak->seton);
    strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT,
                  &tm);
    expire_left(u->na, expirebuf, sizeof(expirebuf), ak->expires);
    notice_lang(s_OperServ, u, OPER_AKILL_VIEW_FORMAT, number, mask,
                ak->by, timebuf, expirebuf, ak->reason);

    return 1;
}

/* Callback for enumeration purposes */

static int akill_view_callback(SList * slist, int number, void *item,
                               va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return akill_view(number, item, u, sent_header);
}

/* Manage the AKILL list. */

static int do_akill(User * u)
{
    char *cmd = strtok(NULL, " ");
    char breason[BUFSIZE];

    if (!cmd)
        cmd = "";

    if (!stricmp(cmd, "ADD")) {
        int deleted = 0;
        char *expiry, *mask, *reason;
        time_t expires;

        mask = strtok(NULL, " ");
        if (mask && *mask == '+') {
            expiry = mask;
            mask = strtok(NULL, " ");
        } else {
            expiry = NULL;
        }

        expires = expiry ? dotime(expiry) : AutokillExpiry;
        /* If the expiry given does not contain a final letter, it's in days,
         * said the doc. Ah well.
         */
        if (expiry && isdigit(expiry[strlen(expiry) - 1]))
            expires *= 86400;
        /* Do not allow less than a minute expiry time */
        if (expires != 0 && expires < 60) {
            notice_lang(s_OperServ, u, BAD_EXPIRY_TIME);
            return MOD_CONT;
        } else if (expires > 0) {
            expires += time(NULL);
        }

        if (mask && (reason = strtok(NULL, ""))) {
            /* We first do some sanity check on the proposed mask. */
            if (strchr(mask, '!')) {
                notice_lang(s_OperServ, u, OPER_AKILL_NO_NICK);
                return MOD_CONT;
            }

            if (!strchr(mask, '@')) {
                notice_lang(s_OperServ, u, BAD_USERHOST_MASK);
                return MOD_CONT;
            }

            if (mask && strspn(mask, "~@.*?") == strlen(mask)) {
                notice_lang(s_OperServ, u, USERHOST_MASK_TOO_WIDE, mask);
                return MOD_CONT;
            }

            /**
             * Changed sprintf() to snprintf()and increased the size of
             * breason to match bufsize
             * -Rob
             **/
            if (AddAkiller) {
                snprintf(breason, sizeof(breason), "[%s] %s", u->nick,
                         reason);
                reason = sstrdup(breason);
            }

            deleted = add_akill(u, mask, u->nick, expires, reason);
            if (deleted < 0)
                return MOD_CONT;
            else if (deleted)
                notice_lang(s_OperServ, u, OPER_AKILL_DELETED_SEVERAL,
                            deleted);
            notice_lang(s_OperServ, u, OPER_AKILL_ADDED, mask);

            if (WallOSAkill) {
                char buf[128];

                if (!expires) {
                    strcpy(buf, "does not expire");
                } else {
                    int wall_expiry = expires - time(NULL);
                    char *s = NULL;

                    if (wall_expiry >= 86400) {
                        wall_expiry /= 86400;
                        s = "day";
                    } else if (wall_expiry >= 3600) {
                        wall_expiry /= 3600;
                        s = "hour";
                    } else if (wall_expiry >= 60) {
                        wall_expiry /= 60;
                        s = "minute";
                    }

                    snprintf(buf, sizeof(buf), "expires in %d %s%s",
                             wall_expiry, s,
                             (wall_expiry == 1) ? "" : "s");
                }

                anope_cmd_global(s_OperServ,
                                 "%s added an AKILL for %s (%s) (%s)",
                                 u->nick, mask, reason, buf);
            }

            if (readonly)
                notice_lang(s_OperServ, u, READ_ONLY_MODE);

        } else {
            syntax_error(s_OperServ, u, "AKILL", OPER_AKILL_SYNTAX);
        }

    } else if (!stricmp(cmd, "DEL")) {

        char *mask;
        int res = 0;

        mask = strtok(NULL, " ");

        if (!mask) {
            syntax_error(s_OperServ, u, "AKILL", OPER_AKILL_SYNTAX);
            return MOD_CONT;
        }

        if (akills.count == 0) {
            notice_lang(s_OperServ, u, OPER_AKILL_LIST_EMPTY);
            return MOD_CONT;
        }

        if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)) {
            /* Deleting a range */
            res = slist_delete_range(&akills, mask, NULL);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
                return MOD_CONT;
            } else if (res == 1) {
                notice_lang(s_OperServ, u, OPER_AKILL_DELETED_ONE);
            } else {
                notice_lang(s_OperServ, u, OPER_AKILL_DELETED_SEVERAL,
                            res);
            }
        } else {
            if ((res = slist_indexof(&akills, mask)) == -1) {
                notice_lang(s_OperServ, u, OPER_AKILL_NOT_FOUND, mask);
                return MOD_CONT;
            }

            slist_delete(&akills, res);
            notice_lang(s_OperServ, u, OPER_AKILL_DELETED, mask);
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);

    } else if (!stricmp(cmd, "LIST")) {
        char *mask;
        int res, sent_header = 0;

        if (akills.count == 0) {
            notice_lang(s_OperServ, u, OPER_AKILL_LIST_EMPTY);
            return MOD_CONT;
        }

        mask = strtok(NULL, " ");

        if (!mask || (isdigit(*mask)
                      && strspn(mask, "1234567890,-") == strlen(mask))) {
            res =
                slist_enum(&akills, mask, &akill_list_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
                return MOD_CONT;
            } else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Akill");
            }
        } else {
            int i;
            char amask[BUFSIZE];

            for (i = 0; i < akills.count; i++) {
                snprintf(amask, sizeof(amask), "%s@%s",
                         ((Akill *) akills.list[i])->user,
                         ((Akill *) akills.list[i])->host);
                if (!stricmp(mask, amask)
                    || match_wild_nocase(mask, amask))
                    akill_list(i + 1, akills.list[i], u, &sent_header);
            }

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
            else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Akill");
            }
        }
    } else if (!stricmp(cmd, "VIEW")) {
        char *mask;
        int res, sent_header = 0;

        if (akills.count == 0) {
            notice_lang(s_OperServ, u, OPER_AKILL_LIST_EMPTY);
            return MOD_CONT;
        }

        mask = strtok(NULL, " ");

        if (!mask || (isdigit(*mask)
                      && strspn(mask, "1234567890,-") == strlen(mask))) {
            res =
                slist_enum(&akills, mask, &akill_view_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
                return MOD_CONT;
            }
        } else {
            int i;
            char amask[BUFSIZE];

            for (i = 0; i < akills.count; i++) {
                snprintf(amask, sizeof(amask), "%s@%s",
                         ((Akill *) akills.list[i])->user,
                         ((Akill *) akills.list[i])->host);
                if (!stricmp(mask, amask)
                    || match_wild_nocase(mask, amask))
                    akill_view(i + 1, akills.list[i], u, &sent_header);
            }

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
        }
    } else if (!stricmp(cmd, "CLEAR")) {
        slist_clear(&akills, 1);
        notice_lang(s_OperServ, u, OPER_AKILL_CLEAR);
    } else {
        syntax_error(s_OperServ, u, "AKILL", OPER_AKILL_SYNTAX);
    }
    return MOD_CONT;
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

/* Lists an SGLINE entry, prefixing it with the header if needed */

static int sgline_list(int number, SXLine * sx, User * u, int *sent_header)
{
    if (!sx)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_SGLINE_LIST_HEADER);
        *sent_header = 1;
    }

    notice_lang(s_OperServ, u, OPER_SGLINE_LIST_FORMAT, number, sx->mask,
                sx->reason);

    return 1;
}

/* Callback for enumeration purposes */

static int sgline_list_callback(SList * slist, int number, void *item,
                                va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return sgline_list(number, item, u, sent_header);
}

/* Lists an SGLINE entry, prefixing it with the header if needed */

static int sgline_view(int number, SXLine * sx, User * u, int *sent_header)
{
    char timebuf[32], expirebuf[256];
    struct tm tm;

    if (!sx)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_SGLINE_VIEW_HEADER);
        *sent_header = 1;
    }

    tm = *localtime(&sx->seton);
    strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT,
                  &tm);
    expire_left(u->na, expirebuf, sizeof(expirebuf), sx->expires);
    notice_lang(s_OperServ, u, OPER_SGLINE_VIEW_FORMAT, number, sx->mask,
                sx->by, timebuf, expirebuf, sx->reason);

    return 1;
}

/* Callback for enumeration purposes */

static int sgline_view_callback(SList * slist, int number, void *item,
                                va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return sgline_view(number, item, u, sent_header);
}


/* Manage the SGLINE list. */

static int do_sgline(User * u)
{
    char *cmd = strtok(NULL, " ");

    if (!ircd->sgline) {
        notice_lang(s_OperServ, u, OPER_SGLINE_UNSUPPORTED);
        return MOD_CONT;
    }

    if (!cmd)
        cmd = "";

    if (!stricmp(cmd, "ADD")) {
        int deleted = 0;
        char *expiry, *mask, *reason;
        time_t expires;

        mask = strtok(NULL, ":");
        if (mask && *mask == '+') {
            expiry = mask;
            mask = strchr(expiry, ' ');
            if (mask) {
                *mask = 0;
                mask++;
            }
        } else {
            expiry = NULL;
        }

        expires = expiry ? dotime(expiry) : SGLineExpiry;
        /* If the expiry given does not contain a final letter, it's in days,
         * said the doc. Ah well.
         */
        if (expiry && isdigit(expiry[strlen(expiry) - 1]))
            expires *= 86400;
        /* Do not allow less than a minute expiry time */
        if (expires != 0 && expires < 60) {
            notice_lang(s_OperServ, u, BAD_EXPIRY_TIME);
            return MOD_CONT;
        } else if (expires > 0) {
            expires += time(NULL);
        }

        if (mask && (reason = strtok(NULL, ""))) {
            /* We first do some sanity check on the proposed mask. */

            if (mask && strspn(mask, "*?") == strlen(mask)) {
                notice_lang(s_OperServ, u, USERHOST_MASK_TOO_WIDE, mask);
                return MOD_CONT;
            }

            deleted = add_sgline(u, mask, u->nick, expires, reason);
            if (deleted < 0)
                return MOD_CONT;
            else if (deleted)
                notice_lang(s_OperServ, u, OPER_SGLINE_DELETED_SEVERAL,
                            deleted);
            notice_lang(s_OperServ, u, OPER_SGLINE_ADDED, mask);

            if (WallOSSGLine) {
                char buf[128];

                if (!expires) {
                    strcpy(buf, "does not expire");
                } else {
                    int wall_expiry = expires - time(NULL);
                    char *s = NULL;

                    if (wall_expiry >= 86400) {
                        wall_expiry /= 86400;
                        s = "day";
                    } else if (wall_expiry >= 3600) {
                        wall_expiry /= 3600;
                        s = "hour";
                    } else if (wall_expiry >= 60) {
                        wall_expiry /= 60;
                        s = "minute";
                    }

                    snprintf(buf, sizeof(buf), "expires in %d %s%s",
                             wall_expiry, s,
                             (wall_expiry == 1) ? "" : "s");
                }

                anope_cmd_global(s_OperServ,
                                 "%s added an SGLINE for %s (%s)", u->nick,
                                 mask, buf);
            }

            if (readonly)
                notice_lang(s_OperServ, u, READ_ONLY_MODE);

        } else {
            syntax_error(s_OperServ, u, "SGLINE", OPER_SGLINE_SYNTAX);
        }

    } else if (!stricmp(cmd, "DEL")) {

        char *mask;
        int res = 0;

        mask = strtok(NULL, "");

        if (!mask) {
            syntax_error(s_OperServ, u, "SGLINE", OPER_SGLINE_SYNTAX);
            return MOD_CONT;
        }

        if (sglines.count == 0) {
            notice_lang(s_OperServ, u, OPER_SGLINE_LIST_EMPTY);
            return MOD_CONT;
        }

        if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)) {
            /* Deleting a range */
            res = slist_delete_range(&sglines, mask, NULL);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_SGLINE_NO_MATCH);
                return MOD_CONT;
            } else if (res == 1) {
                notice_lang(s_OperServ, u, OPER_SGLINE_DELETED_ONE);
            } else {
                notice_lang(s_OperServ, u, OPER_SGLINE_DELETED_SEVERAL,
                            res);
            }
        } else {
            if ((res = slist_indexof(&sglines, mask)) == -1) {
                notice_lang(s_OperServ, u, OPER_SGLINE_NOT_FOUND, mask);
                return MOD_CONT;
            }

            slist_delete(&sglines, res);
            notice_lang(s_OperServ, u, OPER_SGLINE_DELETED, mask);
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);

    } else if (!stricmp(cmd, "LIST")) {
        char *mask;
        int res, sent_header = 0;

        if (sglines.count == 0) {
            notice_lang(s_OperServ, u, OPER_SGLINE_LIST_EMPTY);
            return MOD_CONT;
        }

        mask = strtok(NULL, "");

        if (!mask || (isdigit(*mask)
                      && strspn(mask, "1234567890,-") == strlen(mask))) {
            res =
                slist_enum(&sglines, mask, &sgline_list_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_SGLINE_NO_MATCH);
                return MOD_CONT;
            }
        } else {
            int i;
            char *amask;

            for (i = 0; i < sglines.count; i++) {
                amask = ((SXLine *) sglines.list[i])->mask;
                if (!stricmp(mask, amask)
                    || match_wild_nocase(mask, amask))
                    sgline_list(i + 1, sglines.list[i], u, &sent_header);
            }

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_SGLINE_NO_MATCH);
            else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "SGLine");
            }
        }
    } else if (!stricmp(cmd, "VIEW")) {
        char *mask;
        int res, sent_header = 0;

        if (sglines.count == 0) {
            notice_lang(s_OperServ, u, OPER_SGLINE_LIST_EMPTY);
            return MOD_CONT;
        }

        mask = strtok(NULL, "");

        if (!mask || (isdigit(*mask)
                      && strspn(mask, "1234567890,-") == strlen(mask))) {
            res =
                slist_enum(&sglines, mask, &sgline_view_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_SGLINE_NO_MATCH);
                return MOD_CONT;
            }
        } else {
            int i;
            char *amask;

            for (i = 0; i < sglines.count; i++) {
                amask = ((SXLine *) sglines.list[i])->mask;
                if (!stricmp(mask, amask)
                    || match_wild_nocase(mask, amask))
                    sgline_view(i + 1, sglines.list[i], u, &sent_header);
            }

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_SGLINE_NO_MATCH);
        }
    } else if (!stricmp(cmd, "CLEAR")) {
        slist_clear(&sglines, 1);
        notice_lang(s_OperServ, u, OPER_SGLINE_CLEAR);
    } else {
        syntax_error(s_OperServ, u, "SGLINE", OPER_SGLINE_SYNTAX);
    }
    return MOD_CONT;
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
    SXLine *entry;

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

/* Lists an SQLINE entry, prefixing it with the header if needed */

static int sqline_list(int number, SXLine * sx, User * u, int *sent_header)
{
    if (!sx)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_SQLINE_LIST_HEADER);
        *sent_header = 1;
    }

    notice_lang(s_OperServ, u, OPER_SQLINE_LIST_FORMAT, number, sx->mask,
                sx->reason);

    return 1;
}

/* Callback for enumeration purposes */

static int sqline_list_callback(SList * slist, int number, void *item,
                                va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return sqline_list(number, item, u, sent_header);
}

/* Lists an SQLINE entry, prefixing it with the header if needed */

static int sqline_view(int number, SXLine * sx, User * u, int *sent_header)
{
    char timebuf[32], expirebuf[256];
    struct tm tm;

    if (!sx)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_SQLINE_VIEW_HEADER);
        *sent_header = 1;
    }

    tm = *localtime(&sx->seton);
    strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT,
                  &tm);
    expire_left(u->na, expirebuf, sizeof(expirebuf), sx->expires);
    notice_lang(s_OperServ, u, OPER_SQLINE_VIEW_FORMAT, number, sx->mask,
                sx->by, timebuf, expirebuf, sx->reason);

    return 1;
}

/* Callback for enumeration purposes */

static int sqline_view_callback(SList * slist, int number, void *item,
                                va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return sqline_view(number, item, u, sent_header);
}

/* Manage the SQLINE list. */

static int do_sqline(User * u)
{
    char *cmd = strtok(NULL, " ");

    if (!ircd->sqline) {
        notice_lang(s_OperServ, u, OPER_SQLINE_UNSUPPORTED);
        return MOD_CONT;
    }

    if (!cmd)
        cmd = "";

    if (!stricmp(cmd, "ADD")) {
        int deleted = 0;
        char *expiry, *mask, *reason;
        time_t expires;

        mask = strtok(NULL, " ");
        if (mask && *mask == '+') {
            expiry = mask;
            mask = strtok(NULL, " ");
        } else {
            expiry = NULL;
        }

        expires = expiry ? dotime(expiry) : SQLineExpiry;
        /* If the expiry given does not contain a final letter, it's in days,
         * said the doc. Ah well.
         */
        if (expiry && isdigit(expiry[strlen(expiry) - 1]))
            expires *= 86400;
        /* Do not allow less than a minute expiry time */
        if (expires != 0 && expires < 60) {
            notice_lang(s_OperServ, u, BAD_EXPIRY_TIME);
            return MOD_CONT;
        } else if (expires > 0) {
            expires += time(NULL);
        }

        if (mask && (reason = strtok(NULL, ""))) {

            /* We first do some sanity check on the proposed mask. */
            if (strspn(mask, "*") == strlen(mask)) {
                notice_lang(s_OperServ, u, USERHOST_MASK_TOO_WIDE, mask);
                return MOD_CONT;
            }

            /* Channel SQLINEs are only supported on Bahamut servers */
            if (*mask == '#' && !ircd->chansqline) {
                notice_lang(s_OperServ, u,
                            OPER_SQLINE_CHANNELS_UNSUPPORTED);
                return MOD_CONT;
            }

            deleted = add_sqline(u, mask, u->nick, expires, reason);
            if (deleted < 0)
                return MOD_CONT;
            else if (deleted)
                notice_lang(s_OperServ, u, OPER_SQLINE_DELETED_SEVERAL,
                            deleted);
            notice_lang(s_OperServ, u, OPER_SQLINE_ADDED, mask);

            if (WallOSSQLine) {
                char buf[128];

                if (!expires) {
                    strcpy(buf, "does not expire");
                } else {
                    int wall_expiry = expires - time(NULL);
                    char *s = NULL;

                    if (wall_expiry >= 86400) {
                        wall_expiry /= 86400;
                        s = "day";
                    } else if (wall_expiry >= 3600) {
                        wall_expiry /= 3600;
                        s = "hour";
                    } else if (wall_expiry >= 60) {
                        wall_expiry /= 60;
                        s = "minute";
                    }

                    snprintf(buf, sizeof(buf), "expires in %d %s%s",
                             wall_expiry, s,
                             (wall_expiry == 1) ? "" : "s");
                }

                anope_cmd_global(s_OperServ,
                                 "%s added an SQLINE for %s (%s)", u->nick,
                                 mask, buf);
            }

            if (readonly)
                notice_lang(s_OperServ, u, READ_ONLY_MODE);

        } else {
            syntax_error(s_OperServ, u, "SQLINE", OPER_SQLINE_SYNTAX);
        }

    } else if (!stricmp(cmd, "DEL")) {

        char *mask;
        int res = 0;

        mask = strtok(NULL, "");

        if (!mask) {
            syntax_error(s_OperServ, u, "SQLINE", OPER_SQLINE_SYNTAX);
            return MOD_CONT;
        }

        if (sqlines.count == 0) {
            notice_lang(s_OperServ, u, OPER_SQLINE_LIST_EMPTY);
            return MOD_CONT;
        }

        if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)) {
            /* Deleting a range */
            res = slist_delete_range(&sqlines, mask, NULL);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_SQLINE_NO_MATCH);
                return MOD_CONT;
            } else if (res == 1) {
                notice_lang(s_OperServ, u, OPER_SQLINE_DELETED_ONE);
            } else {
                notice_lang(s_OperServ, u, OPER_SQLINE_DELETED_SEVERAL,
                            res);
            }
        } else {
            if ((res = slist_indexof(&sqlines, mask)) == -1) {
                notice_lang(s_OperServ, u, OPER_SQLINE_NOT_FOUND, mask);
                return MOD_CONT;
            }

            slist_delete(&sqlines, res);
            notice_lang(s_OperServ, u, OPER_SQLINE_DELETED, mask);
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);

    } else if (!stricmp(cmd, "LIST")) {
        char *mask;
        int res, sent_header = 0;

        if (sqlines.count == 0) {
            notice_lang(s_OperServ, u, OPER_SQLINE_LIST_EMPTY);
            return MOD_CONT;
        }

        mask = strtok(NULL, "");

        if (!mask || (isdigit(*mask)
                      && strspn(mask, "1234567890,-") == strlen(mask))) {
            res =
                slist_enum(&sqlines, mask, &sqline_list_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_SQLINE_NO_MATCH);
                return MOD_CONT;
            }
        } else {
            int i;
            char *amask;

            for (i = 0; i < sqlines.count; i++) {
                amask = ((SXLine *) sqlines.list[i])->mask;
                if (!stricmp(mask, amask)
                    || match_wild_nocase(mask, amask))
                    sqline_list(i + 1, sqlines.list[i], u, &sent_header);
            }

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_SQLINE_NO_MATCH);
            else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "SQLine");
            }
        }
    } else if (!stricmp(cmd, "VIEW")) {
        char *mask;
        int res, sent_header = 0;

        if (sqlines.count == 0) {
            notice_lang(s_OperServ, u, OPER_SQLINE_LIST_EMPTY);
            return MOD_CONT;
        }

        mask = strtok(NULL, "");

        if (!mask || (isdigit(*mask)
                      && strspn(mask, "1234567890,-") == strlen(mask))) {
            res =
                slist_enum(&sqlines, mask, &sqline_view_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_SQLINE_NO_MATCH);
                return MOD_CONT;
            }
        } else {
            int i;
            char *amask;

            for (i = 0; i < sqlines.count; i++) {
                amask = ((SXLine *) sqlines.list[i])->mask;
                if (!stricmp(mask, amask)
                    || match_wild_nocase(mask, amask))
                    sqline_view(i + 1, sqlines.list[i], u, &sent_header);
            }

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_SQLINE_NO_MATCH);
        }
    } else if (!stricmp(cmd, "CLEAR")) {
        slist_clear(&sqlines, 1);
        notice_lang(s_OperServ, u, OPER_SQLINE_CLEAR);
    } else {
        syntax_error(s_OperServ, u, "SQLINE", OPER_SQLINE_SYNTAX);
    }
    return MOD_CONT;
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

    if (szlines.count == 0)
        return 0;

    for (i = 0; i < szlines.count; i++) {
        sx = szlines.list[i];
        if (!sx)
            continue;

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

/* Lists an SZLINE entry, prefixing it with the header if needed */

static int szline_list(int number, SXLine * sx, User * u, int *sent_header)
{
    if (!sx)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_SZLINE_LIST_HEADER);
        *sent_header = 1;
    }

    notice_lang(s_OperServ, u, OPER_SZLINE_LIST_FORMAT, number, sx->mask,
                sx->reason);

    return 1;
}

/* Callback for enumeration purposes */

static int szline_list_callback(SList * slist, int number, void *item,
                                va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return szline_list(number, item, u, sent_header);
}

/* Lists an SZLINE entry, prefixing it with the header if needed */

static int szline_view(int number, SXLine * sx, User * u, int *sent_header)
{
    char timebuf[32], expirebuf[256];
    struct tm tm;

    if (!sx)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_SZLINE_VIEW_HEADER);
        *sent_header = 1;
    }

    tm = *localtime(&sx->seton);
    strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT,
                  &tm);
    expire_left(u->na, expirebuf, sizeof(expirebuf), sx->expires);
    notice_lang(s_OperServ, u, OPER_SZLINE_VIEW_FORMAT, number, sx->mask,
                sx->by, timebuf, expirebuf, sx->reason);

    return 1;
}

/* Callback for enumeration purposes */

static int szline_view_callback(SList * slist, int number, void *item,
                                va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return szline_view(number, item, u, sent_header);
}

/* Manage the SZLINE list. */

static int do_szline(User * u)
{
    char *cmd = strtok(NULL, " ");

    if (!ircd->szline) {
        notice_lang(s_OperServ, u, OPER_SZLINE_UNSUPPORTED);
        return MOD_CONT;
    }

    if (!cmd)
        cmd = "";

    if (!stricmp(cmd, "ADD")) {
        int deleted = 0;
        char *expiry, *mask, *reason;
        time_t expires;

        mask = strtok(NULL, " ");
        if (mask && *mask == '+') {
            expiry = mask;
            mask = strtok(NULL, " ");
        } else {
            expiry = NULL;
        }

        expires = expiry ? dotime(expiry) : SZLineExpiry;
        /* If the expiry given does not contain a final letter, it's in days,
         * said the doc. Ah well.
         */
        if (expiry && isdigit(expiry[strlen(expiry) - 1]))
            expires *= 86400;
        /* Do not allow less than a minute expiry time */
        if (expires != 0 && expires < 60) {
            notice_lang(s_OperServ, u, BAD_EXPIRY_TIME);
            return MOD_CONT;
        } else if (expires > 0) {
            expires += time(NULL);
        }

        if (mask && (reason = strtok(NULL, ""))) {
            /* We first do some sanity check on the proposed mask. */

            if (strchr(mask, '!') || strchr(mask, '@')) {
                notice_lang(s_OperServ, u, OPER_SZLINE_ONLY_IPS);
                return MOD_CONT;
            }

            if (strspn(mask, "*?") == strlen(mask)) {
                notice_lang(s_OperServ, u, USERHOST_MASK_TOO_WIDE, mask);
                return MOD_CONT;
            }

            deleted = add_szline(u, mask, u->nick, expires, reason);
            if (deleted < 0)
                return MOD_CONT;
            else if (deleted)
                notice_lang(s_OperServ, u, OPER_SZLINE_DELETED_SEVERAL,
                            deleted);
            notice_lang(s_OperServ, u, OPER_SZLINE_ADDED, mask);

            if (WallOSSZLine) {
                char buf[128];

                if (!expires) {
                    strcpy(buf, "does not expire");
                } else {
                    int wall_expiry = expires - time(NULL);
                    char *s = NULL;

                    if (wall_expiry >= 86400) {
                        wall_expiry /= 86400;
                        s = "day";
                    } else if (wall_expiry >= 3600) {
                        wall_expiry /= 3600;
                        s = "hour";
                    } else if (wall_expiry >= 60) {
                        wall_expiry /= 60;
                        s = "minute";
                    }

                    snprintf(buf, sizeof(buf), "expires in %d %s%s",
                             wall_expiry, s,
                             (wall_expiry == 1) ? "" : "s");
                }

                anope_cmd_global(s_OperServ,
                                 "%s added an SZLINE for %s (%s)", u->nick,
                                 mask, buf);
            }

            if (readonly)
                notice_lang(s_OperServ, u, READ_ONLY_MODE);

        } else {
            syntax_error(s_OperServ, u, "SZLINE", OPER_SZLINE_SYNTAX);
        }

    } else if (!stricmp(cmd, "DEL")) {

        char *mask;
        int res = 0;

        mask = strtok(NULL, " ");

        if (!mask) {
            syntax_error(s_OperServ, u, "SZLINE", OPER_SZLINE_SYNTAX);
            return MOD_CONT;
        }

        if (szlines.count == 0) {
            notice_lang(s_OperServ, u, OPER_SZLINE_LIST_EMPTY);
            return MOD_CONT;
        }

        if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)) {
            /* Deleting a range */
            res = slist_delete_range(&szlines, mask, NULL);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_SZLINE_NO_MATCH);
                return MOD_CONT;
            } else if (res == 1) {
                notice_lang(s_OperServ, u, OPER_SZLINE_DELETED_ONE);
            } else {
                notice_lang(s_OperServ, u, OPER_SZLINE_DELETED_SEVERAL,
                            res);
            }
        } else {
            if ((res = slist_indexof(&szlines, mask)) == -1) {
                notice_lang(s_OperServ, u, OPER_SZLINE_NOT_FOUND, mask);
                return MOD_CONT;
            }

            slist_delete(&szlines, res);
            notice_lang(s_OperServ, u, OPER_SZLINE_DELETED, mask);
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);

    } else if (!stricmp(cmd, "LIST")) {
        char *mask;
        int res, sent_header = 0;

        if (szlines.count == 0) {
            notice_lang(s_OperServ, u, OPER_SZLINE_LIST_EMPTY);
            return MOD_CONT;
        }

        mask = strtok(NULL, " ");

        if (!mask || (isdigit(*mask)
                      && strspn(mask, "1234567890,-") == strlen(mask))) {
            res =
                slist_enum(&szlines, mask, &szline_list_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_SZLINE_NO_MATCH);
                return MOD_CONT;
            }
        } else {
            int i;
            char *amask;

            for (i = 0; i < szlines.count; i++) {
                amask = ((SXLine *) szlines.list[i])->mask;
                if (!stricmp(mask, amask)
                    || match_wild_nocase(mask, amask))
                    szline_list(i + 1, szlines.list[i], u, &sent_header);
            }

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_SZLINE_NO_MATCH);
        }
    } else if (!stricmp(cmd, "VIEW")) {
        char *mask;
        int res, sent_header = 0;

        if (szlines.count == 0) {
            notice_lang(s_OperServ, u, OPER_SZLINE_LIST_EMPTY);
            return MOD_CONT;
        }

        mask = strtok(NULL, " ");

        if (!mask || (isdigit(*mask)
                      && strspn(mask, "1234567890,-") == strlen(mask))) {
            res =
                slist_enum(&szlines, mask, &szline_view_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_SZLINE_NO_MATCH);
                return MOD_CONT;
            }
        } else {
            int i;
            char *amask;

            for (i = 0; i < szlines.count; i++) {
                amask = ((SXLine *) szlines.list[i])->mask;
                if (!stricmp(mask, amask)
                    || match_wild_nocase(mask, amask))
                    szline_view(i + 1, szlines.list[i], u, &sent_header);
            }

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_SZLINE_NO_MATCH);
        }
    } else if (!stricmp(cmd, "CLEAR")) {
        slist_clear(&szlines, 1);
        notice_lang(s_OperServ, u, OPER_SZLINE_CLEAR);
    } else {
        syntax_error(s_OperServ, u, "SZLINE", OPER_SZLINE_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_chanlist(User * u)
{
    char *pattern = strtok(NULL, " ");
    char *opt = strtok(NULL, " ");

    int modes = 0;
    User *u2;

    if (opt && !stricmp(opt, "SECRET"))
        modes |= (CMODE_s | CMODE_p);

    if (pattern && (u2 = finduser(pattern))) {
        struct u_chanlist *uc;

        notice_lang(s_OperServ, u, OPER_CHANLIST_HEADER_USER, u2->nick);

        for (uc = u2->chans; uc; uc = uc->next) {
            if (modes && !(uc->chan->mode & modes))
                continue;
            notice_lang(s_OperServ, u, OPER_CHANLIST_RECORD,
                        uc->chan->name, uc->chan->usercount,
                        chan_get_modes(uc->chan, 1, 1),
                        (uc->chan->topic ? uc->chan->topic : ""));
        }
    } else {
        int i;
        Channel *c;

        notice_lang(s_OperServ, u, OPER_CHANLIST_HEADER);

        for (i = 0; i < 1024; i++) {
            for (c = chanlist[i]; c; c = c->next) {
                if (pattern && !match_wild_nocase(pattern, c->name))
                    continue;
                if (modes && !(c->mode & modes))
                    continue;
                notice_lang(s_OperServ, u, OPER_CHANLIST_RECORD, c->name,
                            c->usercount, chan_get_modes(c, 1, 1),
                            (c->topic ? c->topic : ""));
            }
        }
    }

    notice_lang(s_OperServ, u, OPER_CHANLIST_END);
    return MOD_CONT;
}

/*************************************************************************/

static int do_userlist(User * u)
{
    char *pattern = strtok(NULL, " ");
    char *opt = strtok(NULL, " ");

    Channel *c;
    int modes = 0;

    if (opt && !stricmp(opt, "INVISIBLE"))
        modes |= UMODE_i;

    if (pattern && (c = findchan(pattern))) {
        struct c_userlist *cu;

        notice_lang(s_OperServ, u, OPER_USERLIST_HEADER_CHAN, pattern);

        for (cu = c->users; cu; cu = cu->next) {
            if (modes && !(cu->user->mode & modes))
                continue;
            notice_lang(s_OperServ, u, OPER_USERLIST_RECORD,
                        cu->user->nick, common_get_vident(cu->user),
                        common_get_vhost(cu->user));
        }
    } else {
        char mask[BUFSIZE];
        int i;
        User *u2;

        notice_lang(s_OperServ, u, OPER_USERLIST_HEADER);

        for (i = 0; i < 1024; i++) {
            for (u2 = userlist[i]; u2; u2 = u2->next) {
                if (pattern) {
                    snprintf(mask, sizeof(mask), "%s!%s@%s", u2->nick,
                             common_get_vident(u2), common_get_vhost(u2));
                    if (!match_wild_nocase(pattern, mask))
                        continue;
                    if (modes && !(u2->mode & modes))
                        continue;
                }
                notice_lang(s_OperServ, u, OPER_USERLIST_RECORD, u2->nick,
                            common_get_vident(u2), common_get_vhost(u2));
            }
        }
    }

    notice_lang(s_OperServ, u, OPER_USERLIST_END);
    return MOD_CONT;
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

/* Lists an admin entry, prefixing it with the header if needed */

static int admin_list(int number, NickCore * nc, User * u,
                      int *sent_header)
{
    if (!nc)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_ADMIN_LIST_HEADER);
        *sent_header = 1;
    }

    notice_lang(s_OperServ, u, OPER_ADMIN_LIST_FORMAT, number,
                nc->display);
    return 1;
}

/* Callback for enumeration purposes */

static int admin_list_callback(SList * slist, int number, void *item,
                               va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return admin_list(number, item, u, sent_header);
}

/* Services admin list viewing/modification. */

static int do_admin(User * u)
{
    char *cmd = strtok(NULL, " ");
    char *nick = strtok(NULL, " ");
    NickAlias *na;
    int res = 0;

    if (skeleton) {
        notice_lang(s_OperServ, u, OPER_ADMIN_SKELETON);
        return MOD_CONT;
    }

    if (!cmd || (!nick && stricmp(cmd, "LIST") && stricmp(cmd, "CLEAR"))) {
        syntax_error(s_OperServ, u, "ADMIN", OPER_ADMIN_SYNTAX);
    } else if (!stricmp(cmd, "ADD")) {
        if (!is_services_root(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        if (!(na = findnick(nick))) {
            notice_lang(s_OperServ, u, NICK_X_NOT_REGISTERED, nick);
            return MOD_CONT;
        }

        if (na->status & NS_VERBOTEN) {
            notice_lang(s_OperServ, u, NICK_X_FORBIDDEN, nick);
            return MOD_CONT;
        }

        if (na->nc->flags & NI_SERVICES_ADMIN
            || slist_indexof(&servadmins, na->nc) != -1) {
            notice_lang(s_OperServ, u, OPER_ADMIN_EXISTS, nick);
            return MOD_CONT;
        }

        res = slist_add(&servadmins, na->nc);
        if (res == -2) {
            notice_lang(s_OperServ, u, OPER_ADMIN_REACHED_LIMIT, nick);
            return MOD_CONT;
        } else {
            na->nc->flags |= NI_SERVICES_ADMIN;
            notice_lang(s_OperServ, u, OPER_ADMIN_ADDED, nick);
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    } else if (!stricmp(cmd, "DEL")) {
        if (!is_services_root(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        if (servadmins.count == 0) {
            notice_lang(s_OperServ, u, OPER_ADMIN_LIST_EMPTY);
            return MOD_CONT;
        }

        if (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick)) {
            /* Deleting a range */
            res = slist_delete_range(&servadmins, nick, NULL);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_ADMIN_NO_MATCH);
                return MOD_CONT;
            } else if (res == 1) {
                notice_lang(s_OperServ, u, OPER_ADMIN_DELETED_ONE);
            } else {
                notice_lang(s_OperServ, u, OPER_ADMIN_DELETED_SEVERAL,
                            res);
            }
        } else {
            if (!(na = findnick(nick))) {
                notice_lang(s_OperServ, u, NICK_X_NOT_REGISTERED, nick);
                return MOD_CONT;
            }

            if (na->status & NS_VERBOTEN) {
                notice_lang(s_OperServ, u, NICK_X_FORBIDDEN, nick);
                return MOD_CONT;
            }

            if (!(na->nc->flags & NI_SERVICES_ADMIN)
                || (res = slist_indexof(&servadmins, na->nc)) == -1) {
                notice_lang(s_OperServ, u, OPER_ADMIN_NOT_FOUND, nick);
                return MOD_CONT;
            }

            slist_delete(&servadmins, res);
            notice_lang(s_OperServ, u, OPER_ADMIN_DELETED, nick);
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    } else if (!stricmp(cmd, "LIST")) {
        int sent_header = 0;

        if (servadmins.count == 0) {
            notice_lang(s_OperServ, u, OPER_ADMIN_LIST_EMPTY);
            return MOD_CONT;
        }

        if (!nick || (isdigit(*nick)
                      && strspn(nick, "1234567890,-") == strlen(nick))) {
            res =
                slist_enum(&servadmins, nick, &admin_list_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_ADMIN_NO_MATCH);
                return MOD_CONT;
            } else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Admin");
            }
        } else {
            int i;

            for (i = 0; i < servadmins.count; i++)
                if (!stricmp
                    (nick, ((NickCore *) servadmins.list[i])->display)
                    || match_wild_nocase(nick,
                                         ((NickCore *) servadmins.
                                          list[i])->display))
                    admin_list(i + 1, servadmins.list[i], u, &sent_header);

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_ADMIN_NO_MATCH);
            else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Admin");
            }
        }
    } else if (!stricmp(cmd, "CLEAR")) {
        if (!is_services_root(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        if (servadmins.count == 0) {
            notice_lang(s_OperServ, u, OPER_ADMIN_LIST_EMPTY);
            return MOD_CONT;
        }

        slist_clear(&servadmins, 1);
        notice_lang(s_OperServ, u, OPER_ADMIN_CLEAR);
    } else {
        syntax_error(s_OperServ, u, "ADMIN", OPER_ADMIN_SYNTAX);
    }
    return MOD_CONT;
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

/* Lists an oper entry, prefixing it with the header if needed */

static int oper_list(int number, NickCore * nc, User * u, int *sent_header)
{
    if (!nc)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_OPER_LIST_HEADER);
        *sent_header = 1;
    }

    notice_lang(s_OperServ, u, OPER_OPER_LIST_FORMAT, number, nc->display);
    return 1;
}

/* Callback for enumeration purposes */

static int oper_list_callback(SList * slist, int number, void *item,
                              va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return oper_list(number, item, u, sent_header);
}

/**
 * Display an Opers list Entry
 **/
static int opers_list(int number, NickCore * nc, User * u, char *level)
{
    User *au = NULL;
    NickAlias *na;
    int found;
    int i;

    if (!nc)
        return 0;

    found = 0;
    if ((au = finduser(nc->display))) { /* see if user is online */
        found = 1;
        notice_lang(s_OperServ, u, OPER_STAFF_FORMAT, '*', level,
                    nc->display);
    } else {
        for (i = 0; i < nc->aliases.count; i++) {       /* check all aliases */
            na = nc->aliases.list[i];
            if ((au = finduser(na->nick))) {    /* see if user is online */
                found = 1;
                notice_lang(s_OperServ, u, OPER_STAFF_AFORMAT, '*', level,
                            nc->display, na->nick);
            }
        }
    }

    if (!found)
        notice_lang(s_OperServ, u, OPER_STAFF_FORMAT, ' ', level,
                    nc->display);

    return 1;
}

/**
 * Function for the enumerator to call
 **/
static int opers_list_callback(SList * slist, int number, void *item,
                               va_list args)
{
    User *u = va_arg(args, User *);
    char *level = va_arg(args, char *);

    return opers_list(number, item, u, level);
}

/**
 * Display all Services Opers/Admins with Level + Online Status
 * /msg OperServ opers
 **/
static int do_staff(User * u)
{
    int idx = 0;
    User *au = NULL;
    NickCore *nc;
    NickAlias *na;
    int found;
    int i;

    notice_lang(s_OperServ, u, OPER_STAFF_LIST_HEADER);
    slist_enum(&servopers, NULL, &opers_list_callback, u, "OPER");
    slist_enum(&servadmins, NULL, &opers_list_callback, u, "ADMN");

    for (idx = 0; idx < RootNumber; idx++) {
        found = 0;
        if ((au = finduser(ServicesRoots[idx]))) {      /* see if user is online */
            found = 1;
            notice_lang(s_OperServ, u, OPER_STAFF_FORMAT, '*', "ROOT",
                        ServicesRoots[idx]);
        } else if ((nc = findcore(ServicesRoots[idx]))) {
            for (i = 0; i < nc->aliases.count; i++) {   /* check all aliases */
                na = nc->aliases.list[i];
                if ((au = finduser(na->nick))) {        /* see if user is online */
                    found = 1;
                    notice_lang(s_OperServ, u, OPER_STAFF_AFORMAT,
                                '*', "ROOT", ServicesRoots[idx], na->nick);
                }
            }
        }

        if (!found)
            notice_lang(s_OperServ, u, OPER_STAFF_FORMAT, ' ', "ROOT",
                        ServicesRoots[idx]);

    }
    notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Staff");
    return MOD_CONT;
}

/* Services operator list viewing/modification. */

static int do_oper(User * u)
{
    char *cmd = strtok(NULL, " ");
    char *nick = strtok(NULL, " ");
    NickAlias *na;
    int res = 0;

    if (skeleton) {
        notice_lang(s_OperServ, u, OPER_OPER_SKELETON);
        return MOD_CONT;
    }

    if (!cmd || (!nick && stricmp(cmd, "LIST") && stricmp(cmd, "CLEAR"))) {
        syntax_error(s_OperServ, u, "OPER", OPER_OPER_SYNTAX);
    } else if (!stricmp(cmd, "ADD")) {
        if (!is_services_admin(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        if (!(na = findnick(nick))) {
            notice_lang(s_OperServ, u, NICK_X_NOT_REGISTERED, nick);
            return MOD_CONT;
        }

        if (na->status & NS_VERBOTEN) {
            notice_lang(s_OperServ, u, NICK_X_FORBIDDEN, nick);
            return MOD_CONT;
        }

        if (na->nc->flags & NI_SERVICES_OPER
            || slist_indexof(&servopers, na->nc) != -1) {
            notice_lang(s_OperServ, u, OPER_OPER_EXISTS, nick);
            return MOD_CONT;
        }

        res = slist_add(&servopers, na->nc);
        if (res == -2) {
            notice_lang(s_OperServ, u, OPER_OPER_REACHED_LIMIT, nick);
            return MOD_CONT;
        } else {
            na->nc->flags |= NI_SERVICES_OPER;
            notice_lang(s_OperServ, u, OPER_OPER_ADDED, nick);
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    } else if (!stricmp(cmd, "DEL")) {
        if (!is_services_admin(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        if (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick)) {
            /* Deleting a range */
            res = slist_delete_range(&servopers, nick, NULL);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_OPER_NO_MATCH);
                return MOD_CONT;
            } else if (res == 1) {
                notice_lang(s_OperServ, u, OPER_OPER_DELETED_ONE);
            } else {
                notice_lang(s_OperServ, u, OPER_OPER_DELETED_SEVERAL, res);
            }
        } else {
            if (!(na = findnick(nick))) {
                notice_lang(s_OperServ, u, NICK_X_NOT_REGISTERED, nick);
                return MOD_CONT;
            }

            if (na->status & NS_VERBOTEN) {
                notice_lang(s_OperServ, u, NICK_X_FORBIDDEN, nick);
                return MOD_CONT;
            }

            if (!(na->nc->flags & NI_SERVICES_OPER)
                || (res = slist_indexof(&servopers, na->nc)) == -1) {
                notice_lang(s_OperServ, u, OPER_OPER_NOT_FOUND, nick);
                return MOD_CONT;
            }

            slist_delete(&servopers, res);
            notice_lang(s_OperServ, u, OPER_OPER_DELETED, nick);
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    } else if (!stricmp(cmd, "LIST")) {
        int sent_header = 0;

        if (servopers.count == 0) {
            notice_lang(s_OperServ, u, OPER_OPER_LIST_EMPTY);
            return MOD_CONT;
        }

        if (!nick || (isdigit(*nick)
                      && strspn(nick, "1234567890,-") == strlen(nick))) {
            res =
                slist_enum(&servopers, nick, &oper_list_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_OPER_NO_MATCH);
                return MOD_CONT;
            } else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Oper");
            }
        } else {
            int i;

            for (i = 0; i < servopers.count; i++)
                if (!stricmp
                    (nick, ((NickCore *) servopers.list[i])->display)
                    || match_wild_nocase(nick,
                                         ((NickCore *) servopers.list[i])->
                                         display))
                    oper_list(i + 1, servopers.list[i], u, &sent_header);

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_OPER_NO_MATCH);
            else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Oper");
            }
        }
    } else if (!stricmp(cmd, "CLEAR")) {
        if (!is_services_admin(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        if (servopers.count == 0) {
            notice_lang(s_OperServ, u, OPER_OPER_LIST_EMPTY);
            return MOD_CONT;
        }

        slist_clear(&servopers, 1);
        notice_lang(s_OperServ, u, OPER_OPER_CLEAR);
    } else {
        syntax_error(s_OperServ, u, "OPER", OPER_OPER_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

/* Set various Services runtime options. */

static int do_set(User * u)
{
    char *option = strtok(NULL, " ");
    char *setting = strtok(NULL, " ");

    if (!option || !setting) {
        syntax_error(s_OperServ, u, "SET", OPER_SET_SYNTAX);

    } else if (stricmp(option, "IGNORE") == 0) {
        if (stricmp(setting, "on") == 0) {
            allow_ignore = 1;
            notice_lang(s_OperServ, u, OPER_SET_IGNORE_ON);
        } else if (stricmp(setting, "off") == 0) {
            allow_ignore = 0;
            notice_lang(s_OperServ, u, OPER_SET_IGNORE_OFF);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_IGNORE_ERROR);
        }

    } else if (stricmp(option, "READONLY") == 0) {
        if (stricmp(setting, "on") == 0) {
            readonly = 1;
            alog("Read-only mode activated");
            close_log();
            notice_lang(s_OperServ, u, OPER_SET_READONLY_ON);
        } else if (stricmp(setting, "off") == 0) {
            readonly = 0;
            open_log();
            alog("Read-only mode deactivated");
            notice_lang(s_OperServ, u, OPER_SET_READONLY_OFF);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_READONLY_ERROR);
        }

    } else if (stricmp(option, "LOGCHAN") == 0) {
        /* Unlike the other SET commands where only stricmp is necessary,
         * we also have to ensure that LogChannel is defined or we can't
         * send to it.
         *
         * -jester
         */
        if (LogChannel && (stricmp(setting, "on") == 0)) {
            if (ircd->join2msg) {
                anope_cmd_join(s_GlobalNoticer, LogChannel, time(NULL));
            }
            logchan = 1;
            alog("Now sending log messages to %s", LogChannel);
            notice_lang(s_OperServ, u, OPER_SET_LOGCHAN_ON, LogChannel);
        } else if (LogChannel && (stricmp(setting, "off") == 0)) {
            if (ircd->join2msg) {
                anope_cmd_part(s_GlobalNoticer, LogChannel, NULL);
            }
            logchan = 0;
            alog("No longer sending log messages to a channel");
            notice_lang(s_OperServ, u, OPER_SET_LOGCHAN_OFF);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_LOGCHAN_ERROR);
        }
        /**
         * Allow the user to turn super admin on/off
	 *
	 * Rob
         **/
    } else if (stricmp(option, "SUPERADMIN") == 0) {
        if (SuperAdmin && (stricmp(setting, "on") == 0)) {
            u->isSuperAdmin = 1;
            notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ON);
            alog("%s: %s is a SuperAdmin ", s_OperServ, u->nick);
            anope_cmd_global(s_OperServ,
                             getstring2(NULL, OPER_SUPER_ADMIN_WALL_ON),
                             u->nick);
        } else if (SuperAdmin && (stricmp(setting, "off") == 0)) {
            u->isSuperAdmin = 0;
            notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_OFF);
            alog("%s: %s is no longer a SuperAdmin", s_OperServ, u->nick);
            anope_cmd_global(s_OperServ,
                             getstring2(NULL, OPER_SUPER_ADMIN_WALL_OFF),
                             u->nick);
        } else {
            notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_SYNTAX);
        }
    } else if (stricmp(option, "DEBUG") == 0) {
        if (stricmp(setting, "on") == 0) {
            debug = 1;
            alog("Debug mode activated");
            notice_lang(s_OperServ, u, OPER_SET_DEBUG_ON);
        } else if (stricmp(setting, "off") == 0 ||
                   (*setting == '0' && atoi(setting) == 0)) {
            alog("Debug mode deactivated");
            debug = 0;
            notice_lang(s_OperServ, u, OPER_SET_DEBUG_OFF);
        } else if (isdigit(*setting) && atoi(setting) > 0) {
            debug = atoi(setting);
            alog("Debug mode activated (level %d)", debug);
            notice_lang(s_OperServ, u, OPER_SET_DEBUG_LEVEL, debug);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_DEBUG_ERROR);
        }

    } else if (stricmp(option, "NOEXPIRE") == 0) {
        if (stricmp(setting, "ON") == 0) {
            noexpire = 1;
            alog("No expire mode activated");
            notice_lang(s_OperServ, u, OPER_SET_NOEXPIRE_ON);
        } else if (stricmp(setting, "OFF") == 0) {
            noexpire = 0;
            alog("No expire mode deactivated");
            notice_lang(s_OperServ, u, OPER_SET_NOEXPIRE_OFF);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_NOEXPIRE_ERROR);
        }

    } else {
        notice_lang(s_OperServ, u, OPER_SET_UNKNOWN_OPTION, option);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_noop(User * u)
{
    char *cmd = strtok(NULL, " ");
    char *server = strtok(NULL, " ");

    if (!cmd || !server) {
        syntax_error(s_OperServ, u, "NOOP", OPER_NOOP_SYNTAX);
    } else if (!stricmp(cmd, "SET")) {
        User *u2;
        User *u3 = NULL;
        char reason[NICKMAX + 32];

        /* Remove the O:lines */
        anope_cmd_svsnoop(server, 1);

        snprintf(reason, sizeof(reason), "NOOP command used by %s",
                 u->nick);
        if (WallOSNoOp)
            anope_cmd_global(s_OperServ, "\2%s\2 used NOOP on \2%s\2",
                             u->nick, server);
        notice_lang(s_OperServ, u, OPER_NOOP_SET, server);

        /* Kill all the IRCops of the server */
        for (u2 = firstuser(); u2; u2 = u3) {
            u3 = nextuser();
            if ((u2) && is_oper(u2) && (u2->server->name)
                && match_wild(server, u2->server->name)) {
                kill_user(s_OperServ, u2->nick, reason);
            }
        }
    } else if (!stricmp(cmd, "REVOKE")) {
        anope_cmd_svsnoop(server, 0);
        notice_lang(s_OperServ, u, OPER_NOOP_REVOKE, server);
    } else {
        syntax_error(s_OperServ, u, "NOOP", OPER_NOOP_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_jupe(User * u)
{
    char *jserver = strtok(NULL, " ");
    char *reason = strtok(NULL, "");

    if (!jserver) {
        syntax_error(s_OperServ, u, "JUPE", OPER_JUPE_SYNTAX);
    } else {
        if (!isValidHost(jserver, 3)) {
            notice_lang(s_OperServ, u, OPER_JUPE_HOST_ERROR);
        } else {
            anope_cmd_jupe(jserver, u->nick, reason);

            if (WallOSJupe)
                anope_cmd_global(s_OperServ, "\2%s\2 used JUPE on \2%s\2",
                                 u->nick, jserver);
        }
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_raw(User * u)
{

    if (!DisableRaw) {

        char *text = strtok(NULL, "");

        if (!text)
            syntax_error(s_OperServ, u, "RAW", OPER_RAW_SYNTAX);
        else {
            send_cmd(NULL, "%s", text);
            if (WallOSRaw) {
                char *kw = strtok(text, " ");
                while (kw && *kw == ':')
                    kw = strtok(NULL, " ");
                anope_cmd_global(s_OperServ,
                                 "\2%s\2 used RAW command for \2%s\2",
                                 u->nick,
                                 (kw ? kw :
                                  "\2non RFC compliant message\2"));
            }
            alog("%s used RAW command for %s", u->nick, text);
        }
    } else {
        notice_lang(s_OperServ, u, RAW_DISABLED);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_update(User * u)
{
    notice_lang(s_OperServ, u, OPER_UPDATING);
    save_data = 1;
    return MOD_CONT;
}

/*************************************************************************/

static int do_reload(User * u)
{
    if (!read_config(1)) {
        quitmsg = calloc(28 + strlen(u->nick), 1);
        if (!quitmsg)
            quitmsg =
                "Error during the reload of the configuration file, but out of memory!";
        else
            sprintf(quitmsg,
                    "Error during the reload of the configuration file!");
        quitting = 1;
    }

    notice_lang(s_OperServ, u, OPER_RELOAD);
    return MOD_CONT;
}

/*************************************************************************/

static int do_os_quit(User * u)
{
    quitmsg = calloc(28 + strlen(u->nick), 1);
    if (!quitmsg)
        quitmsg = "QUIT command received, but out of memory!";
    else
        sprintf(quitmsg, "QUIT command received from %s", u->nick);

    if (GlobalOnCycle) {
        oper_global(NULL, "%s", GlobalOnCycleMessage);
    }
    quitting = 1;
    return MOD_CONT;
}

/*************************************************************************/

static int do_shutdown(User * u)
{
    quitmsg = calloc(32 + strlen(u->nick), 1);
    if (!quitmsg)
        quitmsg = "SHUTDOWN command received, but out of memory!";
    else
        sprintf(quitmsg, "SHUTDOWN command received from %s", u->nick);

    if (GlobalOnCycle) {
        oper_global(NULL, "%s", GlobalOnCycleMessage);
    }
    save_data = 1;
    delayed_quit = 1;
    return MOD_CONT;
}

/*************************************************************************/

static int do_restart(User * u)
{
#ifdef SERVICES_BIN
    quitmsg = calloc(31 + strlen(u->nick), 1);
    if (!quitmsg)
        quitmsg = "RESTART command received, but out of memory!";
    else
        sprintf(quitmsg, "RESTART command received from %s", u->nick);

    if (GlobalOnCycle) {
        oper_global(NULL, "%s", GlobalOnCycleMessage);
    }
    /*    raise(SIGHUP); */
    do_restart_services();
#else
    notice_lang(s_OperServ, u, OPER_CANNOT_RESTART);
#endif
    return MOD_CONT;
}

/*************************************************************************/

#ifdef DEBUG_COMMANDS

static int do_matchwild(User * u)
{
    char *pat = strtok(NULL, " ");
    char *str = strtok(NULL, " ");
    if (pat && str)
        notice(s_OperServ, u->nick, "%d", match_wild(pat, str));
    else
        notice(s_OperServ, u->nick, "Syntax error.");
    return MOD_CONT;
}

#endif                          /* DEBUG_COMMANDS */

/*************************************************************************/

/* Kill all users matching a certain host. The host is obtained from the
 * supplied nick. The raw hostmsk is not supplied with the command in an effort
 * to prevent abuse and mistakes from being made - which might cause *.com to
 * be killed. It also makes it very quick and simple to use - which is usually
 * what you want when someone starts loading numerous clones. In addition to
 * killing the clones, we add a temporary AKILL to prevent them from
 * immediately reconnecting.
 * Syntax: KILLCLONES nick
 * -TheShadow (29 Mar 1999)
 */

static int do_killclones(User * u)
{
    char *clonenick = strtok(NULL, " ");
    int count = 0;
    User *cloneuser, *user, *tempuser;
    char *clonemask, *akillmask;
    char killreason[NICKMAX + 32];
    char akillreason[] = "Temporary KILLCLONES akill.";

    if (!clonenick) {
        notice_lang(s_OperServ, u, OPER_KILLCLONES_SYNTAX);

    } else if (!(cloneuser = finduser(clonenick))) {
        notice_lang(s_OperServ, u, OPER_KILLCLONES_UNKNOWN_NICK,
                    clonenick);

    } else {
        clonemask = smalloc(strlen(cloneuser->host) + 5);
        sprintf(clonemask, "*!*@%s", cloneuser->host);

        akillmask = smalloc(strlen(cloneuser->host) + 3);
        sprintf(akillmask, "*@%s", cloneuser->host);

        user = firstuser();
        while (user) {
            if (match_usermask(clonemask, user) != 0) {
                tempuser = nextuser();
                count++;
                snprintf(killreason, sizeof(killreason),
                         "Cloning [%d]", count);
                kill_user(NULL, user->nick, killreason);
                user = tempuser;
            } else {
                user = nextuser();
            }
        }

        add_akill(u, akillmask, u->nick,
                  time(NULL) + KillClonesAkillExpire, akillreason);

        anope_cmd_global(s_OperServ,
                         "\2%s\2 used KILLCLONES for \2%s\2 killing "
                         "\2%d\2 clones. A temporary AKILL has been added "
                         "for \2%s\2.", u->nick, clonemask, count,
                         akillmask);

        alog("%s: KILLCLONES: %d clone(s) matching %s killed.",
             s_OperServ, count, clonemask);

        free(akillmask);
        free(clonemask);
    }
    return MOD_CONT;
}

/**
 * Defcon - A method of impelemting various stages of securty, the hope is this will help serives
 * protect a network during an attack, allowing admins to choose the precautions taken at each
 * level.
 *
 * /msg OperServ DefCon [level]
 *
 **/

static int do_defcon(User * u)
{
    char *lvl = strtok(NULL, " ");
    int newLevel = 0;
    char *langglobal;
    langglobal = getstring(NULL, DEFCON_GLOBAL);

    if (!DefConLevel) {         /* If we dont have a .conf setting! */
        notice_lang(s_OperServ, u, OPER_DEFCON_NO_CONF);
        return MOD_CONT;
    }

    if (!lvl) {
        notice_lang(s_OperServ, u, OPER_DEFCON_CHANGED, DefConLevel);
        defcon_sendlvls(u);
        return MOD_CONT;
    }
    newLevel = atoi(lvl);
    if (newLevel < 1 || newLevel > 5) {
        notice_lang(s_OperServ, u, OPER_DEFCON_SYNTAX);
        return MOD_CONT;
    }

    DefConLevel = newLevel;
    DefContimer = time(NULL);
    notice_lang(s_OperServ, u, OPER_DEFCON_CHANGED, DefConLevel);
    defcon_sendlvls(u);
    alog("Defcon level changed to %d by Oper %s", newLevel, u->nick);
    anope_cmd_global(s_OperServ, getstring2(NULL, OPER_DEFCON_WALL),
                     u->nick, newLevel);
    /* Global notice the user what is happening. Also any Message that
       the Admin would like to add. Set in config file. */
    if (GlobalOnDefcon) {
        if ((DefConLevel == 5) && (DefConOffMessage)) {
            oper_global(NULL, "%s", DefConOffMessage);
        } else {
            oper_global(NULL, langglobal, DefConLevel);
        }
    }
    if (GlobalOnDefconMore) {
        if ((DefConOffMessage) && DefConLevel == 5) {
        } else {
            oper_global(NULL, "%s", DefconMessage);
        }
    }
    /* Run any defcon functions, e.g. FORCE CHAN MODE */
    runDefCon();
    return MOD_CONT;
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

/**
 * Returns 1 if the passed level is part of the CURRENT defcon, else 0 is returned
 **/
int checkDefCon(int level)
{
    return DefCon[DefConLevel] & level;
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
 * Automaticaly re-set the DefCon level if the time limit has expired.
 **/
void resetDefCon(int level)
{
    if (DefConLevel != level) {
        if ((DefContimer)
            && (time(NULL) - DefContimer >= dotime(DefConTimeOut))) {
            DefConLevel = level;
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
 * Send a message to the oper about which precautions are "active" for this level
 **/
static void defcon_sendlvls(User * u)
{
    if (checkDefCon(DEFCON_NO_NEW_CHANNELS)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_CHANNELS);
    }
    if (checkDefCon(DEFCON_NO_NEW_NICKS)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_NICKS);
    }
    if (checkDefCon(DEFCON_NO_MLOCK_CHANGE)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_MLOCK_CHANGE);
    }
    if (checkDefCon(DEFCON_FORCE_CHAN_MODES) && (DefConChanModes)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_FORCE_CHAN_MODES,
                    DefConChanModes);
    }
    if (checkDefCon(DEFCON_REDUCE_SESSION)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_REDUCE_SESSION,
                    DefConSessionLimit);
    }
    if (checkDefCon(DEFCON_NO_NEW_CLIENTS)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_CLIENTS);
    }
    if (checkDefCon(DEFCON_OPER_ONLY)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_OPER_ONLY);
    }
    if (checkDefCon(DEFCON_SILENT_OPER_ONLY)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_SILENT_OPER_ONLY);
    }
    if (checkDefCon(DEFCON_AKILL_NEW_CLIENTS)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_AKILL_NEW_CLIENTS);
    }
    if (checkDefCon(DEFCON_NO_NEW_MEMOS)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_MEMOS);
    }
}

/**
 * ChanKill - Akill an entire channel (got botnet?)
 *
 * /msg OperServ ChanKill +expire #channel reason
 *
 **/

static int do_chankill(User * u)
{
    char *expiry, *channel, *reason;
    time_t expires;
    char breason[BUFSIZE];
    char mask[USERMAX + HOSTMAX + 2];
    struct c_userlist *cu, *next;
    Channel *c;

    channel = strtok(NULL, " ");
    if (channel && *channel == '+') {
        expiry = channel;
        channel = strtok(NULL, " ");
    } else {
        expiry = NULL;
    }

    expires = expiry ? dotime(expiry) : ChankillExpiry;
    if (expiry && isdigit(expiry[strlen(expiry) - 1]))
        expires *= 86400;
    if (expires != 0 && expires < 60) {
        notice_lang(s_OperServ, u, BAD_EXPIRY_TIME);
        return MOD_CONT;
    } else if (expires > 0) {
        expires += time(NULL);
    }

    if (channel && (reason = strtok(NULL, ""))) {

        if (AddAkiller) {
            snprintf(breason, sizeof(breason), "[%s] %s", u->nick, reason);
            reason = sstrdup(breason);
        }

        if ((c = findchan(channel))) {
            for (cu = c->users; cu; cu = next) {
                next = cu->next;
                if (is_oper(cu->user)) {
                    continue;
                }
                strncpy(mask, "*@", 3); /* Use *@" for the akill's, */
                strncat(mask, cu->user->host, HOSTMAX);
                add_akill(NULL, mask, s_OperServ, expires, reason);
                check_akill(cu->user->nick, cu->user->username,
                            cu->user->host, NULL, NULL);
            }
            if (WallOSAkill) {
                anope_cmd_global(s_OperServ, "%s used CHANKILL on %s (%s)",
                                 u->nick, channel, reason);
            }
        } else {
            notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, channel);
        }
    } else {
        syntax_error(s_OperServ, u, "CHANKILL", OPER_CHANKILL_SYNTAX);
    }
    return MOD_CONT;
}

#ifdef USE_MODULES

int do_modload(User * u)
{
    char *name;
    Module *m;

    name = strtok(NULL, "");
    if (!name) {
        syntax_error(s_OperServ, u, "MODLOAD", OPER_MODULE_LOAD_SYNTAX);
        return MOD_CONT;
    }
    m = findModule(name);
    if (!m) {
        m = createModule(name);
        mod_current_module = m;
        mod_current_user = u;
        mod_current_op = 1;
    } else {
        notice_lang(s_OperServ, u, OPER_MODULE_LOAD_FAIL, name);
    }
    return MOD_CONT;
}

int do_modunload(User * u)
{
    char *name;
    Module *m;

    name = strtok(NULL, "");
    if (!name) {
        syntax_error(s_OperServ, u, "MODUNLOAD",
                     OPER_MODULE_UNLOAD_SYNTAX);
        return MOD_CONT;
    }
    m = findModule(name);
    if (m) {
        mod_current_user = u;
        mod_current_module = m;
        mod_current_op = 2;
    } else {
        notice_lang(s_OperServ, u, OPER_MODULE_REMOVE_FAIL, name);
    }
    return MOD_CONT;
}

int do_modlist(User * u)
{
    int idx;
    int count = 0;
    ModuleHash *current = NULL;

    notice_lang(s_OperServ, u, OPER_MODULE_LIST_HEADER);

    for (idx = 0; idx != MAX_CMD_HASH; idx++) {
        for (current = MODULE_HASH[idx]; current; current = current->next) {
            notice_lang(s_OperServ, u, OPER_MODULE_LIST, current->name,
                        current->m->version);
            count++;
        }
    }
    if (count == 0) {
        notice_lang(s_OperServ, u, OPER_MODULE_NO_LIST);
    } else {
        notice_lang(s_OperServ, u, OPER_MODULE_LIST_FOOTER, count);
    }

    return MOD_CONT;
}

int do_modinfo(User * u)
{
    char *file;
    struct tm tm;
    char timebuf[64];
    Module *m;
    int idx = 0;
    int display = 0;

    file = strtok(NULL, "");
    if (!file) {
        syntax_error(s_OperServ, u, "MODINFO", OPER_MODULE_INFO_SYNTAX);
        return MOD_CONT;
    }
    m = findModule(file);
    if (m) {
        tm = *localtime(&m->time);
        strftime_lang(timebuf, sizeof(timebuf), u,
                      STRFTIME_DATE_TIME_FORMAT, &tm);
        notice_lang(s_OperServ, u, OPER_MODULE_INFO_LIST, m->name,
                    m->version ? m->version : "?",
                    m->author ? m->author : "?", timebuf);
        for (idx = 0; idx < MAX_CMD_HASH; idx++) {
            display += showModuleCmdLoaded(HOSTSERV[idx], m->name, u);
            display += showModuleCmdLoaded(OPERSERV[idx], m->name, u);
            display += showModuleCmdLoaded(NICKSERV[idx], m->name, u);
            display += showModuleCmdLoaded(CHANSERV[idx], m->name, u);
            display += showModuleCmdLoaded(BOTSERV[idx], m->name, u);
            display += showModuleCmdLoaded(MEMOSERV[idx], m->name, u);
            display += showModuleCmdLoaded(HELPSERV[idx], m->name, u);
            display += showModuleMsgLoaded(IRCD[idx], m->name, u);

        }
    }
    if (display == 0) {
        notice_lang(s_OperServ, u, OPER_MODULE_NO_INFO, file);
    }
    return MOD_CONT;
}

static int showModuleCmdLoaded(CommandHash * cmdList, char *mod_name,
                               User * u)
{
    Command *c;
    CommandHash *current;
    int display = 0;

    for (current = cmdList; current; current = current->next) {
        for (c = current->c; c; c = c->next) {
            if ((c->mod_name) && (stricmp(c->mod_name, mod_name) == 0)) {
                notice_lang(s_OperServ, u, OPER_MODULE_CMD_LIST,
                            c->service, c->name);
                display++;
            }
        }
    }
    return display;
}

static int showModuleMsgLoaded(MessageHash * msgList, char *mod_name,
                               User * u)
{
    Message *msg;
    MessageHash *mcurrent;
    int display = 0;
    for (mcurrent = msgList; mcurrent; mcurrent = mcurrent->next) {
        for (msg = mcurrent->m; msg; msg = msg->next) {
            if ((msg->mod_name) && (stricmp(msg->mod_name, mod_name) == 0)) {
                notice_lang(s_OperServ, u, OPER_MODULE_MSG_LIST,
                            msg->name);
                display++;
            }
        }
    }
    return display;
}

#endif

/*************************************************************************/
