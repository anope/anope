
/* NickServ functions.
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

/*************************************************************************/

#define HASH(nick)	((tolower((nick)[0])&31)<<5 | (tolower((nick)[1])&31))

NickAlias *nalists[1024];
NickCore *nclists[1024];
NickRequest *nrlists[1024];

unsigned int guestnum;          /* Current guest number */

#define TO_COLLIDE   0          /* Collide the user with this nick */
#define TO_RELEASE   1          /* Release a collided nick */

/*************************************************************************/

extern char *getvHost(char *nick);

void alpha_insert_alias(NickAlias * na);
void insert_core(NickCore * nc);
void insert_requestnick(NickRequest * nr);
static NickAlias *makenick(const char *nick);
static NickRequest *makerequest(const char *nick);
static NickAlias *makealias(const char *nick, NickCore * nc);
static void change_core_display(NickCore * nc, char *newdisplay);

static void collide(NickAlias * na, int from_timeout);
static void release(NickAlias * na, int from_timeout);
static void add_ns_timeout(NickAlias * na, int type, time_t delay);
static void del_ns_timeout(NickAlias * na, int type);
int delnickrequest(NickRequest * nr);
NickRequest *findrequestnick(const char *nick);
static int do_sendregmail(User * u, NickRequest * nr);
static int do_setmodes(User * u);

static int do_help(User * u);
static int do_register(User * u);
static int do_confirm(User * u);
static int do_group(User * u);
static int do_nickupdate(User * u);
static int do_identify(User * u);
static int do_logout(User * u);
static int do_drop(User * u);
static int do_set(User * u);
static int do_set_display(User * u, NickCore * nc, char *param);
static int do_set_password(User * u, NickCore * nc, char *param);
static int do_set_language(User * u, NickCore * nc, char *param);
static int do_set_url(User * u, NickCore * nc, char *param);
static int do_set_email(User * u, NickCore * nc, char *param);
static int do_set_greet(User * u, NickCore * nc, char *param);
static int do_set_icq(User * u, NickCore * nc, char *param);
static int do_set_kill(User * u, NickCore * nc, char *param);
static int do_set_secure(User * u, NickCore * nc, char *param);
static int do_set_private(User * u, NickCore * nc, char *param);
static int do_set_msg(User * u, NickCore * nc, char *param);
static int do_set_hide(User * u, NickCore * nc, char *param);
static int do_set_noexpire(User * u, NickAlias * nc, char *param);
static int do_access(User * u);
static int do_info(User * u);
static int do_list(User * u);
static int do_alist(User * u);
static int do_glist(User * u);
static int do_recover(User * u);
static int do_release(User * u);
static int do_ghost(User * u);
static int do_status(User * u);
static int do_getpass(User * u);
static int do_getemail(User * u);
static int do_sendpass(User * u);
static int do_forbid(User * u);
static int do_resend(User * u);

/* Obsolete commands */
static int do_link(User * u);
static int do_unlink(User * u);
static int do_listlinks(User * u);


/*************************************************************************/
/* *INDENT-OFF* */
void moduleAddNickServCmds(void) {
    Command *c;
    c = createCommand("HELP",     do_help,     NULL,  -1,                     -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("REGISTER", do_register, NULL,  NICK_HELP_REGISTER,     -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("RESEND", do_resend,     NULL,  -1,                     -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("CONFIRM",  do_confirm,  NULL,  -1,		      -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("GROUP",    do_group,    NULL,  NICK_HELP_GROUP,        -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("UPDATE",   do_nickupdate,    NULL,  NICK_HELP_UPDATE,        -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("ID",       do_identify, NULL,  NICK_HELP_IDENTIFY,     -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("IDENTIFY", do_identify, NULL,  NICK_HELP_IDENTIFY,     -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SIDENTIFY",do_identify, NULL,  -1,                     -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("LOGOUT",   do_logout,   NULL,  -1,NICK_HELP_LOGOUT, NICK_SERVADMIN_HELP_LOGOUT,NICK_SERVADMIN_HELP_LOGOUT, NICK_SERVADMIN_HELP_LOGOUT); addCoreCommand(NICKSERV,c);
    c = createCommand("DROP",     do_drop,     NULL,  -1,NICK_HELP_DROP, NICK_SERVADMIN_HELP_DROP,NICK_SERVADMIN_HELP_DROP, NICK_SERVADMIN_HELP_DROP); addCoreCommand(NICKSERV,c);
    c = createCommand("LINK",     do_link,     NULL,  -1,                     -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("UNLINK",   do_unlink,   NULL,  -1,                     -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("LISTLINKS",do_listlinks,NULL,  -1,                     -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("ACCESS",   do_access,   NULL,  NICK_HELP_ACCESS,       -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SET",      do_set,      NULL,  NICK_HELP_SET,-1, NICK_SERVADMIN_HELP_SET,NICK_SERVADMIN_HELP_SET, NICK_SERVADMIN_HELP_SET); addCoreCommand(NICKSERV,c);
    c = createCommand("SET DISPLAY",  NULL,    NULL,  NICK_HELP_SET_DISPLAY,  -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SET PASSWORD", NULL,    NULL,  NICK_HELP_SET_PASSWORD, -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SET URL",      NULL,    NULL,  NICK_HELP_SET_URL,      -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SET EMAIL",    NULL,    NULL,  NICK_HELP_SET_EMAIL,    -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SET ICQ",      NULL,    NULL,  NICK_HELP_SET_ICQ,      -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SET GREET",    NULL,    NULL,  NICK_HELP_SET_GREET,    -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SET KILL",     NULL,    NULL,  NICK_HELP_SET_KILL,     -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SET SECURE",   NULL,    NULL,  NICK_HELP_SET_SECURE,   -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SET PRIVATE",  NULL,    NULL,  NICK_HELP_SET_PRIVATE,  -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SET MSG",      NULL,    NULL,  NICK_HELP_SET_MSG,      -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SET HIDE",     NULL,    NULL,  NICK_HELP_SET_HIDE,     -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SET NOEXPIRE", NULL,    NULL,  -1, -1,NICK_SERVADMIN_HELP_SET_NOEXPIRE,NICK_SERVADMIN_HELP_SET_NOEXPIRE,NICK_SERVADMIN_HELP_SET_NOEXPIRE); addCoreCommand(NICKSERV,c);
    c = createCommand("RECOVER",  do_recover,  NULL,  NICK_HELP_RECOVER,      -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("RELEASE",  do_release,  NULL,  NICK_HELP_RELEASE,      -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("GHOST",    do_ghost,    NULL,  NICK_HELP_GHOST,        -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("INFO",     do_info,     NULL,  NICK_HELP_INFO,-1, NICK_HELP_INFO, NICK_SERVADMIN_HELP_INFO,NICK_SERVADMIN_HELP_INFO); addCoreCommand(NICKSERV,c);
    c = createCommand("LIST",     do_list,     NULL,  -1,NICK_HELP_LIST, NICK_SERVADMIN_HELP_LIST,NICK_SERVADMIN_HELP_LIST, NICK_SERVADMIN_HELP_LIST); addCoreCommand(NICKSERV,c);
    c = createCommand("ALIST",    do_alist,    NULL,  -1,NICK_HELP_ALIST, NICK_SERVADMIN_HELP_ALIST,NICK_SERVADMIN_HELP_ALIST, NICK_SERVADMIN_HELP_ALIST); addCoreCommand(NICKSERV,c);
    c = createCommand("GLIST",    do_glist,    NULL,  -1,NICK_HELP_GLIST, NICK_SERVADMIN_HELP_GLIST,NICK_SERVADMIN_HELP_GLIST, NICK_SERVADMIN_HELP_GLIST); addCoreCommand(NICKSERV,c);
    c = createCommand("STATUS",   do_status,   NULL,  NICK_HELP_STATUS,       -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("SENDPASS", do_sendpass, NULL,  NICK_HELP_SENDPASS,     -1,-1,-1,-1); addCoreCommand(NICKSERV,c);
    c = createCommand("GETPASS",  do_getpass,  is_services_admin,  -1,-1, NICK_SERVADMIN_HELP_GETPASS,NICK_SERVADMIN_HELP_GETPASS, NICK_SERVADMIN_HELP_GETPASS); addCoreCommand(NICKSERV,c);
    c = createCommand("GETEMAIL", do_getemail, is_services_admin,  -1,-1, NICK_SERVADMIN_HELP_GETEMAIL,NICK_SERVADMIN_HELP_GETEMAIL, NICK_SERVADMIN_HELP_GETEMAIL); addCoreCommand(NICKSERV,c);
    c = createCommand("FORBID",   do_forbid,   is_services_admin,  -1,-1, NICK_SERVADMIN_HELP_FORBID,NICK_SERVADMIN_HELP_FORBID, NICK_SERVADMIN_HELP_FORBID); addCoreCommand(NICKSERV,c);
}
/* *INDENT-ON* */
/*************************************************************************/

/* Display total number of registered nicks and info about each; or, if
 * a specific nick is given, display information about that nick (like
 * /msg NickServ INFO <nick>).  If count_only != 0, then only display the
 * number of registered nicks (the nick parameter is ignored).
 */

void listnicks(int count_only, const char *nick)
{
    int count = 0;
    NickAlias *na;
    int i;
    char *end;

    if (count_only) {

        for (i = 0; i < 1024; i++) {
            for (na = nalists[i]; na; na = na->next)
                count++;
        }
        printf("%d nicknames registered.\n", count);

    } else if (nick) {

        struct tm *tm;
        char buf[512];
        static const char commastr[] = ", ";
        int need_comma = 0;

        if (!(na = findnick(nick))) {
            printf("%s not registered.\n", nick);
            return;
        } else if (na->status & NS_VERBOTEN) {
            printf("%s is FORBIDden.\n", nick);
            return;
        }
        printf("%s is %s\n", nick, na->last_realname);
        printf("Last seen address: %s\n", na->last_usermask);
        tm = localtime(&na->time_registered);
        strftime(buf, sizeof(buf),
                 getstring(NULL, STRFTIME_DATE_TIME_FORMAT), tm);
        printf("  Time registered: %s\n", buf);
        tm = localtime(&na->last_seen);
        strftime(buf, sizeof(buf),
                 getstring(NULL, STRFTIME_DATE_TIME_FORMAT), tm);
        printf("   Last seen time: %s\n", buf);
        if (na->nc->url)
            printf("              URL: %s\n", na->nc->url);
        if (na->nc->email)
            printf("   E-mail address: %s\n", na->nc->email);
        if (na->nc->icq)
            printf("            ICQ #: %d\n", na->nc->icq);
        if (na->nc->greet)
            printf("            Greet: %s\n", na->nc->greet);
        *buf = 0;
        end = buf;
        if (na->nc->flags & NI_KILLPROTECT) {
            end +=
                snprintf(end, sizeof(buf) - (end - buf),
                         "Kill protection");
            need_comma = 1;
        }
        if (na->nc->flags & NI_SECURE) {
            end += snprintf(buf, sizeof(buf) - (end - buf), "%sSecurity",
                            need_comma ? commastr : "");
            need_comma = 1;
        }
        if (na->nc->flags & NI_PRIVATE) {
            end += snprintf(buf, sizeof(buf) - (end - buf), "%sPrivate",
                            need_comma ? commastr : "");
            need_comma = 1;
        }
        if (na->status & NS_NO_EXPIRE) {
            end += snprintf(buf, sizeof(buf) - (end - buf), "%sNo Expire",
                            need_comma ? commastr : "");
            need_comma = 1;
        }
        printf("          Options: %s\n", *buf ? buf : "None");

    } else {

        for (i = 0; i < 1024; i++) {
            for (na = nalists[i]; na; na = na->next) {
                printf("    %s %-20s  %s\n",
                       na->status & NS_NO_EXPIRE ? "!" : " ",
                       na->nick, na->status & NS_VERBOTEN ?
                       "Disallowed (FORBID)" : na->last_usermask);
                count++;
            }
        }
        printf("%d nicknames registered.\n", count);

    }
}

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

            mem += nc->memos.memocount * sizeof(Memo);
            for (j = 0; j < nc->memos.memocount; j++) {
                if (nc->memos.memos[j].text)
                    mem += strlen(nc->memos.memos[j].text) + 1;
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

void ns_init(void)
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
    char *cmd, *s;

    cmd = strtok(buf, " ");

    if (!cmd) {
        return;
    } else if (stricmp(cmd, "\1PING") == 0) {
        if (!(s = strtok(NULL, "")))
            s = "\1";
        notice(s_NickServ, u->nick, "\1PING %s", s);
    } else if (skeleton) {
        notice_lang(s_NickServ, u, SERVICE_OFFLINE, s_NickServ);
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

/* Loads NickServ database versions 5 to 11 (<= 4 is not supported) */

void load_old_ns_dbase(void)
{
    dbFILE *f;
    int ver, i, j, c;
    NickAlias *na, *na2, *next;
    NickCore *nc;
    int failed = 0;

    int16 tmp16;
    int32 tmp32;

    char bufn[NICKMAX], bufp[PASSMAX];
    char *email, *greet, *url, *forbidby, *forbidreason;
    uint32 icq;

    if (!(f = open_db(s_NickServ, NickDBName, "r", NICK_VERSION)))
        return;

    ver = get_file_version(f);
    if (ver <= 4) {
        fatal("Unsupported version number (%d) on %s", ver, NickDBName);
        close_db(f);
        return;
    }

    for (i = 0; i < 256 && !failed; i++) {
        while ((c = getc_db(f)) == 1) {
            if (c != 1)
                fatal("Invalid format in %s", NickDBName);

            na = scalloc(sizeof(NickAlias), 1);

            SAFE(read_buffer(bufn, f));
            na->nick = sstrdup(bufn);
            SAFE(read_buffer(bufp, f)); /* Will be used later if needed */

            SAFE(read_string(&url, f));
            SAFE(read_string(&email, f));
            if (ver >= 10)
                SAFE(read_int32(&icq, f));
            else
                icq = 0;
            if (ver >= 9)
                SAFE(read_string(&greet, f));
            else
                greet = NULL;

            SAFE(read_string(&na->last_usermask, f));
            SAFE(read_string(&na->last_realname, f));
            SAFE(read_string(&na->last_quit, f));

            SAFE(read_int32(&tmp32, f));
            na->time_registered = tmp32;
            SAFE(read_int32(&tmp32, f));
            na->last_seen = tmp32;

            SAFE(read_int16(&na->status, f));
            na->status &= ~NS_TEMPORARY;
#ifdef USE_ENCRYPTION
            if (!(na->status & (NS_OLD_ENCRYPTEDPW | NS_VERBOTEN))) {
                if (debug)
                    alog("debug: %s: encrypting password for `%s' on load",
                         s_NickServ, na->nick);
                if (encrypt_in_place(bufp, PASSMAX) < 0)
                    fatal("%s: Can't encrypt `%s' nickname password!",
                          s_NickServ, na->nick);
                na->status |= NS_OLD_ENCRYPTEDPW;
            }
#else
            if (na->status & NS_OLD_ENCRYPTEDPW) {
                /* Bail: it makes no sense to continue with encrypted
                 * passwords, since we won't be able to verify them */
                fatal
                    ("%s: load database: password for %s encrypted but encryption disabled, aborting",
                     s_NickServ, na->nick);
            }
#endif
            if (ver >= 9) {
                SAFE(read_string(&forbidby, f));
                SAFE(read_string(&forbidreason, f));
                /* Cleanup */
                if (forbidby && *forbidby == '@') {
                    free(forbidby);
                    forbidby = NULL;
                }
                if (forbidreason && *forbidreason == 0) {
                    free(forbidreason);
                    forbidreason = NULL;
                }
            } else {
                forbidby = NULL;
                forbidreason = NULL;
            }

            if (na->status & NS_VERBOTEN) {
                if (na->last_usermask)
                    free(na->last_usermask);
                if (na->last_realname)
                    free(na->last_realname);

                na->last_usermask = forbidby;
                na->last_realname = forbidreason;
            } else {
                if (!na->last_usermask)
                    na->last_usermask = sstrdup("");
                if (!na->last_realname)
                    na->last_realname = sstrdup("");
            }

            /* Store the reference for later resolving */
            SAFE(read_string((char **) &na->nc, f));
            SAFE(read_int16(&tmp16, f));        /* Was linkcount */

            if (na->nc) {
                SAFE(read_int16(&tmp16, f));    /* Was channelcount */
            } else {
                /* This nick was a master nick, so it also has all the
                 * core info! =)
                 */
                nc = scalloc(1, sizeof(NickCore));
                slist_init(&nc->aliases);

                /* The initial display is what used to be the master nick */
                nc->display = sstrdup(na->nick);

                /* We grabbed info before; fill the appropriate fields now */
                if (*bufp)
                    nc->pass = sstrdup(bufp);
                else
                    nc->pass = NULL;    /* Which may be the case for forbidden nicks .. */

                nc->email = email;
                nc->greet = greet;
                nc->icq = icq;
                nc->url = url;

                /* We check whether the e-mail is valid because it was not tested
                 * in older versions.
                 */
                if (ver <= 10 && nc->email && !MailValidate(nc->email)) {
                    free(nc->email);
                    nc->email = NULL;
                }

                SAFE(read_int32(&nc->flags, f));
                if (!NSAllowKillImmed)
                    nc->flags &= ~NI_KILL_IMMED;

                /* Status flags cleanup */
                if (na->status & NS_OLD_ENCRYPTEDPW) {
                    nc->flags |= NI_ENCRYPTEDPW;
                    na->status &= ~NS_OLD_ENCRYPTEDPW;
                }

                /* Add services opers and admins to the appropriate list, but
                   only if the database version is equal to or more than 10. */
                if (ver >= 10) {
                    if (nc->flags & NI_SERVICES_ADMIN)
                        slist_add(&servadmins, nc);
                    if (nc->flags & NI_SERVICES_OPER)
                        slist_add(&servopers, nc);
                }

                /* Add the Services root flag if needed. */
                if (nc)
                    for (j = 0; j < RootNumber; j++)
                        if (!stricmp(ServicesRoots[j], na->nick))
                            nc->flags |= NI_SERVICES_ROOT;

                SAFE(read_int16(&nc->accesscount, f));
                if (nc->accesscount) {
                    char **access;
                    access = scalloc(sizeof(char *) * nc->accesscount, 1);
                    nc->access = access;
                    for (j = 0; j < nc->accesscount; j++, access++)
                        SAFE(read_string(access, f));
                }

                SAFE(read_int16(&nc->memos.memocount, f));
                SAFE(read_int16(&nc->memos.memomax, f));
                if (nc->memos.memocount) {
                    Memo *memos;
                    memos = scalloc(sizeof(Memo) * nc->memos.memocount, 1);
                    nc->memos.memos = memos;

                    for (j = 0; j < nc->memos.memocount; j++, memos++) {
                        SAFE(read_int32(&memos->number, f));
                        SAFE(read_int16(&memos->flags, f));
                        SAFE(read_int32(&tmp32, f));
                        memos->time = tmp32;
                        SAFE(read_buffer(memos->sender, f));
                        SAFE(read_string(&memos->text, f));
                        memos->moduleData = NULL;
                    }
                }

                /* We read the channel count, but don't take care of it.
                   load_cs_dbase will regenerate it correctly. */
                SAFE(read_int16(&tmp16, f));
                SAFE(read_int16(&nc->channelmax, f));
                if (ver == 5)
                    nc->channelmax = CSMaxReg;

                SAFE(read_int16(&nc->language, f));

                if (ver >= 11 && ver < 13) {
                    char *s;

                    SAFE(read_int16(&tmp16, f));
                    SAFE(read_int32(&tmp32, f));
                    SAFE(read_int16(&tmp16, f));
                    SAFE(read_string(&s, f));
                }

                /* Set us as being a master nick; fill the nc field also.
                   The NS_MASTER flag will not be cleared in this function. */
                na->status |= NS_MASTER;
                na->nc = nc;
                slist_add(&nc->aliases, na);

                /* Insert our new core in the core list */
                insert_core(nc);
            }

            alpha_insert_alias(na);

        }                       /* while (getc_db(f) != 0) */
    }                           /* for (i) */

    /* Now resolve what were called links */
    for (i = 0; i < 1024; i++) {
        for (na = nalists[i]; na; na = next) {
            next = na->next;

            /* Master nicks are already resolved */
            if (na->status & NS_MASTER)
                continue;

            na2 = na;
            /* While the reference resolves and it's not a master nick */
            while ((na2 = findnick((char *) na2->nc))
                   && !(na2->status & NS_MASTER));

            /* It didn't resolve. This is problematic since there is no core. :/
               We delete the nick. */
            if (!na2) {
                alog("%s: while loading database: %s was linked to inexistant %s", s_NickServ, na->nick, (char *) na->nc);
                delnick(na);
                continue;
            }

            /* OK we have information on the core. We mark the current alias
               as a master nick because it now contains a valid core. */
            na->nc = na2->nc;
            na->status |= NS_MASTER;
            slist_add(&na->nc->aliases, na);
        }
    }

    close_db(f);
}

void load_ns_req_db(void)
{
    dbFILE *f;
    int i, c, ver;
    NickRequest *nr;
    int32 tmp32;
    int failed = 0;

    if (!(f = open_db(s_NickServ, PreNickDBName, "r", PRE_NICK_VERSION)))
        return;
    ver = get_file_version(f);
    for (i = 0; i < 1024 && !failed; i++) {
        while ((c = getc_db(f)) == 1) {
            if (c != 1)
                fatal("Invalid format in %s", PreNickDBName);
            nr = scalloc(1, sizeof(NickRequest));
            SAFE(read_string(&nr->nick, f));
            SAFE(read_string(&nr->passcode, f));
            SAFE(read_string(&nr->password, f));
            SAFE(read_string(&nr->email, f));
            SAFE(read_int32(&tmp32, f));
            nr->requested = tmp32;
            insert_requestnick(nr);
        }
    }
    close_db(f);
}

void load_ns_dbase(void)
{
    dbFILE *f;
    int ver, i, j, c;
    NickAlias *na, **nalast, *naprev;
    NickCore *nc, **nclast, *ncprev;
    int failed = 0;
    int16 tmp16;
    int32 tmp32;
    char *s;

    if (!(f = open_db(s_NickServ, NickDBName, "r", NICK_VERSION)))
        return;

    ver = get_file_version(f);

    if (ver <= 11) {
        close_db(f);
        load_old_ns_dbase();
        return;
    }

    /* First we load nick cores */
    for (i = 0; i < 1024 && !failed; i++) {
        nclast = &nclists[i];
        ncprev = NULL;

        while ((c = getc_db(f)) == 1) {
            if (c != 1)
                fatal("Invalid format in %s", NickDBName);

            nc = scalloc(1, sizeof(NickCore));
            *nclast = nc;
            nclast = &nc->next;
            nc->prev = ncprev;
            ncprev = nc;

            slist_init(&nc->aliases);

            SAFE(read_string(&nc->display, f));
            SAFE(read_string(&nc->pass, f));
            SAFE(read_string(&nc->email, f));
            SAFE(read_string(&nc->greet, f));
            SAFE(read_int32(&nc->icq, f));
            SAFE(read_string(&nc->url, f));

            SAFE(read_int32(&nc->flags, f));
            if (!NSAllowKillImmed)
                nc->flags &= ~NI_KILL_IMMED;
#ifdef USE_ENCRYPTION
            if (nc->pass && !(nc->flags & NI_ENCRYPTEDPW)) {
                if (debug)
                    alog("debug: %s: encrypting password for `%s' on load",
                         s_NickServ, nc->display);
                if (encrypt_in_place(nc->pass, PASSMAX) < 0)
                    fatal("%s: Can't encrypt `%s' nickname password!",
                          s_NickServ, nc->display);
                nc->flags |= NI_ENCRYPTEDPW;
            }
#else
            if (nc->flags & NI_ENCRYPTEDPW) {
                /* Bail: it makes no sense to continue with encrypted
                 * passwords, since we won't be able to verify them */
                fatal
                    ("%s: load database: password for %s encrypted but encryption disabled, aborting",
                     s_NickServ, nc->display);
            }
#endif
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
                access = scalloc(sizeof(char *) * nc->accesscount, 1);
                nc->access = access;
                for (j = 0; j < nc->accesscount; j++, access++)
                    SAFE(read_string(access, f));
            }

            SAFE(read_int16(&nc->memos.memocount, f));
            SAFE(read_int16(&nc->memos.memomax, f));
            if (nc->memos.memocount) {
                Memo *memos;
                memos = scalloc(sizeof(Memo) * nc->memos.memocount, 1);
                nc->memos.memos = memos;
                for (j = 0; j < nc->memos.memocount; j++, memos++) {
                    SAFE(read_int32(&memos->number, f));
                    SAFE(read_int16(&memos->flags, f));
                    SAFE(read_int32(&tmp32, f));
                    memos->time = tmp32;
                    SAFE(read_buffer(memos->sender, f));
                    SAFE(read_string(&memos->text, f));
                    memos->moduleData = NULL;
                }
            }

            SAFE(read_int16(&nc->channelcount, f));
            SAFE(read_int16(&tmp16, f));
            nc->channelmax = CSMaxReg;

            if (ver < 13) {
                /* Used to be dead authentication system */
                SAFE(read_int16(&tmp16, f));
                SAFE(read_int32(&tmp32, f));
                SAFE(read_int16(&tmp16, f));
                SAFE(read_string(&s, f));
            }

        }                       /* while (getc_db(f) != 0) */
        *nclast = NULL;
    }                           /* for (i) */

    for (i = 0; i < 1024 && !failed; i++) {
        nalast = &nalists[i];
        naprev = NULL;
        while ((c = getc_db(f)) == 1) {
            if (c != 1)
                fatal("Invalid format in %s", NickDBName);

            na = scalloc(1, sizeof(NickAlias));

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
            free(s);

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

        }                       /* while (getc_db(f) != 0) */

        *nalast = NULL;
    }                           /* for (i) */

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
	    anope_cmd_global(NULL, "Write error on %s: %s", NickDBName,	\
			strerror(errno));			\
	    lastwarn = time(NULL);				\
	}							\
	return;							\
    }								\
} while (0)



void save_ns_dbase(void)
{
    dbFILE *f;
    int i, j;
    NickAlias *na;
    NickCore *nc;
    char **access;
    Memo *memos;
    static time_t lastwarn = 0;

    if (!(f = open_db(s_NickServ, NickDBName, "w", NICK_VERSION)))
        return;

    for (i = 0; i < 1024; i++) {
        for (nc = nclists[i]; nc; nc = nc->next) {
            SAFE(write_int8(1, f));

            SAFE(write_string(nc->display, f));
            SAFE(write_string(nc->pass, f));

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

            SAFE(write_int16(nc->memos.memocount, f));
            SAFE(write_int16(nc->memos.memomax, f));
            memos = nc->memos.memos;
            for (j = 0; j < nc->memos.memocount; j++, memos++) {
                SAFE(write_int32(memos->number, f));
                SAFE(write_int16(memos->flags, f));
                SAFE(write_int32(memos->time, f));
                SAFE(write_buffer(memos->sender, f));
                SAFE(write_string(memos->text, f));
            }

            SAFE(write_int16(nc->channelcount, f));
            SAFE(write_int16(nc->channelmax, f));

        }                       /* for (nc) */

        SAFE(write_int8(0, f));

    }                           /* for (i) */

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

        }                       /* for (na) */
        SAFE(write_int8(0, f));
    }                           /* for (i) */

    close_db(f);

}

void save_ns_req_dbase(void)
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
            SAFE(write_string(nr->password, f));
            SAFE(write_string(nr->email, f));
            SAFE(write_int32(nr->requested, f));
            SAFE(write_int8(0, f));
        }
    }
    close_db(f);

}

#undef SAFE

void save_ns_rdb_dbase(void)
{
#ifdef USE_RDB
    int i;
    NickAlias *na;
    NickCore *nc;

    if (!rdb_open())
        return;

    rdb_tag_table("anope_ns_core");
    rdb_tag_table("anope_ns_alias");
    rdb_scrub_table("anope_ms_info", "serv='NICK'");
    rdb_clear_table("anope_ns_access");

    for (i = 0; i < 1024; i++) {
        for (nc = nclists[i]; nc; nc = nc->next) {
            rdb_save_ns_core(nc);

        }                       /* for (nc) */
    }                           /* for (i) */

    for (i = 0; i < 1024; i++) {
        for (na = nalists[i]; na; na = na->next) {
            rdb_save_ns_alias(na);

        }                       /* for (na) */
    }                           /* for (i) */

    rdb_scrub_table("anope_ns_core", "active='0'");
    rdb_scrub_table("anope_ns_alias", "active='0'");
    rdb_close();
#endif
}

void save_ns_req_rdb_dbase(void)
{
#ifdef USE_RDB
    int i;
    NickRequest *nr;

    if (!rdb_open())
        return;

    rdb_tag_table("anope_ns_request");

    for (i = 0; i < 1024; i++) {
        for (nr = nrlists[i]; nr; nr = nr->next) {
            rdb_save_ns_req(nr);
        }
    }

    rdb_scrub_table("anope_ns_request", "active='0'");
    rdb_close();
#endif

}

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

    on_access = is_on_access(u, na->nc);
    if (on_access)
        na->status |= NS_ON_ACCESS;

    if (!(na->nc->flags & NI_SECURE) && on_access) {
        na->status |= NS_RECOGNIZED;
        na->last_seen = time(NULL);
        if (na->last_usermask)
            free(na->last_usermask);
        na->last_usermask =
            scalloc(strlen(common_get_vident(u)) +
                    strlen(common_get_vhost(u)) + 2, 1);
        sprintf(na->last_usermask, "%s@%s", common_get_vident(u),
                common_get_vhost(u));
        if (na->last_realname)
            free(na->last_realname);
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

    if (na) {
        if (na->status & NS_GUESTED) {
            if (ircd->svshold) {
                if (UseSVSHOLD) {
                    anope_cmd_svshold(na->nick);
                } else {
                    if (ircd->svsnick) {
                        anope_cmd_guest_nick(u->nick, NSEnforcerUser,
                                             NSEnforcerHost,
                                             "Services Enforcer", "+");
                        add_ns_timeout(na, TO_RELEASE, NSReleaseTimeout);
                    } else {
                        anope_cmd_svskill(s_NickServ, u->nick,
                                          "Killing to enforce nick");
                    }
                }
            } else {
                if (ircd->svsnick) {
                    anope_cmd_guest_nick(u->nick, NSEnforcerUser,
                                         NSEnforcerHost,
                                         "Services Enforcer", "+");
                    add_ns_timeout(na, TO_RELEASE, NSReleaseTimeout);
                } else {
                    anope_cmd_svskill(s_NickServ, u->nick,
                                      "Killing to enforce nick");
                }
            }
            na->status &= ~NS_TEMPORARY;
            na->status |= NS_KILL_HELD;
        } else {
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
                && !(na->status & (NS_VERBOTEN | NS_NO_EXPIRE))) {
                alog("Expiring nickname %s (group: %s) (e-mail: %s)",
                     na->nick, na->nc->display,
                     (na->nc->email ? na->nc->email : "none"));
                delnick(na);
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
            alog("Error: findrequestnick() called with NULL values");
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
            alog("Error: findnick() called with NULL values");
        }
        return NULL;
    }

    for (na = nalists[HASH(nick)]; na; na = na->next) {
        if (stricmp(na->nick, nick) == 0)
            return na;
    }

    return NULL;
}

/*************************************************************************/

/* Return the NickCore structure for the given nick, or NULL if the core
 * doesn't exist. */

NickCore *findcore(const char *nick)
{
    NickCore *nc;

    if (!nick || !*nick) {
        if (debug) {
            alog("Error: findcore() called with NULL values");
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

    buf = scalloc(strlen(u->username) + strlen(u->host) + 2, 1);
    sprintf(buf, "%s@%s", u->username, u->host);
    if (ircd->vhost) {
        if (u->vhost) {
            buf2 = scalloc(strlen(u->username) + strlen(u->vhost) + 2, 1);
            sprintf(buf2, "%s@%s", u->username, u->vhost);
        }
    }

    for (i = 0; i < nc->accesscount; i++) {
        if (match_wild_nocase(nc->access[i], buf)
            || (ircd->vhost ? match_wild_nocase(nc->access[i], buf2) : 0)) {
            free(buf);
            if (ircd->vhost) {
                if (u->vhost) {
                    free(buf2);
                }
            }
            return 1;
        }
    }
    free(buf);
    if (ircd->vhost) {
        free(buf2);
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
/* Creates a new Nick Request */
static NickRequest *makerequest(const char *nick)
{
    NickRequest *nr;

    nr = scalloc(1, sizeof(NickRequest));
    nr->nick = sstrdup(nick);
    insert_requestnick(nr);
    alog("%s: Nick %s has been requested", s_NickServ, nr->nick);
    return nr;
}



/* Creates a full new nick (alias + core) in NickServ database. */

static NickAlias *makenick(const char *nick)
{
    NickAlias *na;
    NickCore *nc;

    /* First make the core */
    nc = scalloc(1, sizeof(NickCore));
    nc->display = sstrdup(nick);
    slist_init(&nc->aliases);
    insert_core(nc);
    alog("%s: group %s has been created", s_NickServ, nc->display);

    /* Then make the alias */
    na = scalloc(1, sizeof(NickAlias));
    na->nick = sstrdup(nick);
    na->nc = nc;
    slist_add(&nc->aliases, na);
    alpha_insert_alias(na);
    return na;
}

/*************************************************************************/

/* Creates a new alias in NickServ database. */

static NickAlias *makealias(const char *nick, NickCore * nc)
{
    NickAlias *na;

    /* Just need to make the alias */
    na = scalloc(1, sizeof(NickAlias));
    na->nick = sstrdup(nick);
    na->nc = nc;
    slist_add(&nc->aliases, na);
    alpha_insert_alias(na);
    return na;
}

/*************************************************************************/

/* Sets nc->display to newdisplay. If newdisplay is NULL, it will change
 * it to the first alias in the list.
 */

static void change_core_display(NickCore * nc, char *newdisplay)
{
    if (!newdisplay) {
        NickAlias *na;

        if (nc->aliases.count <= 0)
            return;

        na = nc->aliases.list[0];
        newdisplay = na->nick;
    }

    /* Log ... */
    alog("%s: changing %s nickname group display to %s", s_NickServ,
         nc->display, newdisplay);

#ifdef USE_RDB
    /* Reflect this change in the database right away. This
     * ensures that we know how to deal with this "new" nick
     * on the next /OS UPDATE might need it on /NS DROP too...
     */
    if (rdb_open()) {
        rdb_ns_set_display(newdisplay, nc->display);
        rdb_close();
    }
#endif

    /* Remove the core from the list */
    if (nc->next)
        nc->next->prev = nc->prev;
    if (nc->prev)
        nc->prev->next = nc->next;
    else
        nclists[HASH(nc->display)] = nc->next;

    free(nc->display);
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
#ifdef USE_RDB
    static char clause[128];
#endif
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

#ifdef USE_RDB
    /* Reflect this change in the database right away. */
    if (rdb_open()) {

        snprintf(clause, sizeof(clause), "display='%s'", nc->display);
        rdb_scrub_table("anope_ns_access", clause);
        rdb_scrub_table("anope_ns_core", clause);
        rdb_scrub_table("anope_cs_access", clause);
        /* I'm unsure how to clean up the OS ADMIN/OPER list on the db */
        /* I wish the "display" primary key would be the same on all tables */
        snprintf(clause, sizeof(clause), "receiver='%s' AND serv='NICK'",
                 nc->display);
        rdb_scrub_table("anope_ms_info", clause);
        rdb_close();
    }
#endif

    /* Now we can safely free it. */
    free(nc->display);
    if (nc->pass)
        free(nc->pass);

    if (nc->email)
        free(nc->email);
    if (nc->greet)
        free(nc->greet);
    if (nc->url)
        free(nc->url);

    if (nc->access) {
        for (i = 0; i < nc->accesscount; i++) {
            if (nc->access[i])
                free(nc->access[i]);
        }
        free(nc->access);
    }

    if (nc->memos.memos) {
        for (i = 0; i < nc->memos.memocount; i++) {
            if (nc->memos.memos[i].text)
                moduleCleanStruct(&nc->memos.memos[i].moduleData);
            free(nc->memos.memos[i].text);
        }
        free(nc->memos.memos);
    }

    moduleCleanStruct(&nc->moduleData);

    free(nc);

    return 1;
}


/*************************************************************************/
int delnickrequest(NickRequest * nr)
{
    if (nr) {
        nrlists[HASH(nr->nick)] = nr->next;
        if (nr->nick)
            free(nr->nick);
        if (nr->passcode)
            free(nr->passcode);
        if (nr->password)
            free(nr->password);
        if (nr->email)
            free(nr->email);
        free(nr);
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
#ifdef USE_RDB
    static char clause[128];
#endif
    /* First thing to do: remove any timeout belonging to the nick we're deleting */
    clean_ns_timeouts(na);

    /* Second thing to do: look for an user using the alias
     * being deleted, and make appropriate changes */

    if (na->u) {
        na->u->na = NULL;

        common_svsmode(na->u, ircd->modeonunreg, "1");

    }

    delHostCore(na->nick);      /* delete any vHost's for this nick */

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

#ifdef USE_RDB
    /* Reflect this change in the database right away. */
    if (rdb_open()) {

        snprintf(clause, sizeof(clause), "nick='%s'", na->nick);
        rdb_scrub_table("anope_ns_alias", clause);
        rdb_close();
    }
#endif

    free(na->nick);
    if (na->last_usermask)
        free(na->last_usermask);
    if (na->last_realname)
        free(na->last_realname);
    if (na->last_quit)
        free(na->last_quit);

    moduleCleanStruct(&na->moduleData);

    free(na);


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

static void collide(NickAlias * na, int from_timeout)
{
    char guestnick[NICKMAX];

    if (!from_timeout)
        del_ns_timeout(na, TO_COLLIDE);

    /* Old system was unsure since there can be more than one collide
     * per second. So let use another safer method.
     *          --lara
     */
    /* So you should check the length of NSGuestNickPrefix, eh Lara?
     *          --Certus
     */

    if (ircd->svsnick) {
        snprintf(guestnick, sizeof(guestnick), "%s%d", NSGuestNickPrefix,
                 guestnum++);
        notice_lang(s_NickServ, na->u, FORCENICKCHANGE_CHANGING,
                    guestnick);
        anope_cmd_svsnick(na->nick, guestnick, time(NULL));
        na->status |= NS_GUESTED;
    } else {
        kill_user(s_NickServ, na->nick, "Services nickname-enforcer kill");
    }
}

/*************************************************************************/

/* Release hold on a nick. */

static void release(NickAlias * na, int from_timeout)
{
    if (!from_timeout)
        del_ns_timeout(na, TO_RELEASE);
    if (ircd->svshold) {
        if (UseSVSHOLD) {
            anope_cmd_release_svshold(na->nick);
        } else {
            anope_cmd_quit(na->nick, NULL);
        }
    } else {
        anope_cmd_quit(na->nick, NULL);
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
            free(t);
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
    NickAlias *na = t->data;

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
    NickAlias *na = t->data;

    rem_ns_timeout(na, TO_RELEASE);
    release(na, 1);
}

/*************************************************************************/

/* Add a collide/release timeout. */

void add_ns_timeout(NickAlias * na, int type, time_t delay)
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
             type, (void *) na, na->nick, (long int) delay);
        return;
    }

    to = add_timeout(delay, timeout_routine, 0);
    to->data = na;

    t = scalloc(sizeof(struct my_timeout), 1);
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

static void del_ns_timeout(NickAlias * na, int type)
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
            free(t);
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
                alog("%s: deleting timeout type %d from %s", s_NickServ,
                     t->type, t->na->nick);
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
            free(t);
        }
    }
}

/*************************************************************************/
/*********************** NickServ command routines ***********************/
/*************************************************************************/

/* Return a help message. */

static int do_help(User * u)
{
    char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_NickServ, u, NICK_HELP);
        if (NSExpire >= 86400)
            notice_help(s_NickServ, u, NICK_HELP_EXPIRES,
                        NSExpire / 86400);
        if (is_services_oper(u))
            notice_help(s_NickServ, u, NICK_SERVADMIN_HELP);
        moduleDisplayHelp(1, u);
    } else if (stricmp(cmd, "SET LANGUAGE") == 0) {
        int i;
        notice_help(s_NickServ, u, NICK_HELP_SET_LANGUAGE);
        for (i = 0; i < NUM_LANGS && langlist[i] >= 0; i++)
            notice_user(s_NickServ, u, "    %2d) %s", i + 1,
                        langnames[langlist[i]]);
    } else {
        mod_help_cmd(s_NickServ, u, NICKSERV, cmd);
    }
    return MOD_CONT;
}

/*************************************************************************/

/* Register a nick. */

static int do_register(User * u)
{
    NickRequest *nr = NULL, *anr = NULL;
    NickCore *nc = NULL;
    int prefixlen = strlen(NSGuestNickPrefix);
    int nicklen = strlen(u->nick);
    char *pass = strtok(NULL, " ");
    char *email = strtok(NULL, " ");
    char passcode[11];
    int idx, min = 1, max = 62, i = 0;
    int chars[] =
        { ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
        'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
        'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
        'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
    };

    if (readonly) {
        notice_lang(s_NickServ, u, NICK_REGISTRATION_DISABLED);
        return MOD_CONT;
    }

    if (checkDefCon(DEFCON_NO_NEW_NICKS)) {
        notice_lang(s_NickServ, u, OPER_DEFCON_DENIED);
        return MOD_CONT;
    }

    if (!is_oper(u) && NickRegDelay
        && ((time(NULL) - u->my_signon) < NickRegDelay)) {
        notice_lang(s_NickServ, u, NICK_REG_DELAY, NickRegDelay);
        return MOD_CONT;
    }

    if ((anr = findrequestnick(u->nick))) {
        notice_lang(s_NickServ, u, NICK_REQUESTED);
        return MOD_CONT;
    }
    /* Prevent "Guest" nicks from being registered. -TheShadow */

    /* Guest nick can now have a series of between 1 and 7 digits.
     *      --lara
     */
    if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1 &&
        stristr(u->nick, NSGuestNickPrefix) == u->nick &&
        strspn(u->nick + prefixlen, "1234567890") == nicklen - prefixlen) {
        notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick);
        return MOD_CONT;
    }

    if (RestrictOperNicks) {
        for (i = 0; i < RootNumber; i++) {
            if (stristr(u->nick, ServicesRoots[i]) && !is_oper(u)) {
                notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED,
                            u->nick);
                return MOD_CONT;
            }
        }
        for (i = 0; i < servadmins.count && (nc = servadmins.list[i]); i++) {
            if (stristr(u->nick, nc->display) && !is_oper(u)) {
                notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED,
                            u->nick);
                return MOD_CONT;
            }
        }
        for (i = 0; i < servopers.count && (nc = servopers.list[i]); i++) {
            if (stristr(u->nick, nc->display) && !is_oper(u)) {
                notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED,
                            u->nick);
                return MOD_CONT;
            }
        }
    }

    if (!pass) {
        if (NSForceEmail && !email) {
            syntax_error(s_NickServ, u, "REGISTER",
                         NICK_REGISTER_SYNTAX_EMAIL);
        } else {
            syntax_error(s_NickServ, u, "REGISTER", NICK_REGISTER_SYNTAX);
        }
    } else if (time(NULL) < u->lastnickreg + NSRegDelay) {
        notice_lang(s_NickServ, u, NICK_REG_PLEASE_WAIT, NSRegDelay);
    } else if (u->na) {         /* i.e. there's already such a nick regged */
        if (u->na->status & NS_VERBOTEN) {
            alog("%s: %s@%s tried to register FORBIDden nick %s",
                 s_NickServ, u->username, common_get_vhost(u), u->nick);
            notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED, u->nick);
        } else {
            notice_lang(s_NickServ, u, NICK_ALREADY_REGISTERED, u->nick);
        }
    } else if (stricmp(u->nick, pass) == 0
               || (StrictPasswords && strlen(pass) < 5)) {
        notice_lang(s_NickServ, u, MORE_OBSCURE_PASSWORD);
    } else if (email && !MailValidate(email)) {
        notice_lang(s_NickServ, u, MAIL_X_INVALID, email);
    } else {
#ifdef USE_ENCRYPTION
        if (strlen(pass) > PASSMAX) {
            pass[PASSMAX] = 0;
            notice_lang(s_NickServ, u, PASSWORD_TRUNCATED, PASSMAX);
        }
#else
        if (strlen(pass) > PASSMAX - 1) {       /* -1 for null byte */
            pass[PASSMAX] = 0;
            notice_lang(s_NickServ, u, PASSWORD_TRUNCATED, PASSMAX - 1);
        }
#endif
        for (idx = 0; idx < 9; idx++) {
            passcode[idx] =
                chars[(1 +
                       (int) (((float) (max - min)) * getrandom16() /
                              (65535 + 1.0)) + min)];
        } passcode[idx] = '\0';
        nr = makerequest(u->nick);
        nr->passcode = sstrdup(passcode);
        nr->password = sstrdup(pass);
        if (email) {
            nr->email = sstrdup(email);
        }
        nr->requested = time(NULL);
        if (NSEmailReg) {
            if (do_sendregmail(u, nr) == 0) {
                notice_lang(s_NickServ, u, NICK_ENTER_REG_CODE, email,
                            s_NickServ);
                alog("%s: sent registration verification code to %s",
                     s_NickServ, nr->email);
            } else {
                alog("%s: Unable to send registration verification mail",
                     s_NickServ);
                notice_lang(s_NickServ, u, NICK_REG_UNABLE);
                delnickrequest(nr);     /* Delete the NickRequest if we couldnt send the mail */
                return MOD_CONT;
            }
        } else {
            do_confirm(u);
        }

    }
    return MOD_CONT;
}


static int do_resend(User * u)
{
    NickRequest *nr = NULL;
    if (NSEmailReg) {
        if ((nr = findrequestnick(u->nick))) {
            if (do_sendregmail(u, nr) == 0) {
                notice_lang(s_NickServ, u, NICK_REG_RESENT, nr->email);
                alog("%s: re-sent registration verification code for %s to %s", s_NickServ, nr->nick, nr->email);
            } else {
                alog("%s: Unable to re-send registration verification mail for %s", s_NickServ, nr->nick);
                return MOD_CONT;
            }
        }
    }
    return MOD_CONT;
}

static int do_sendregmail(User * u, NickRequest * nr)
{
    MailInfo *mail = NULL;
    char buf[BUFSIZE];

    if (!(nr || u)) {
        return -1;
    }
    snprintf(buf, sizeof(buf), getstring2(NULL, NICK_REG_MAIL_SUBJECT),
             nr->nick);
    mail = MailRegBegin(u, nr, buf, s_NickServ);
    if (!mail) {
        return -1;
    }
    fprintf(mail->pipe, getstring2(NULL, NICK_REG_MAIL_HEAD));
    fprintf(mail->pipe, "\n\n");
    fprintf(mail->pipe, getstring2(NULL, NICK_REG_MAIL_LINE_1), nr->nick);
    fprintf(mail->pipe, "\n\n");
    fprintf(mail->pipe, getstring2(NULL, NICK_REG_MAIL_LINE_2), s_NickServ,
            nr->passcode);
    fprintf(mail->pipe, "\n\n");
    fprintf(mail->pipe, getstring2(NULL, NICK_REG_MAIL_LINE_3));
    fprintf(mail->pipe, "\n\n");
    fprintf(mail->pipe, getstring2(NULL, NICK_REG_MAIL_LINE_4));
    fprintf(mail->pipe, "\n\n");
    fprintf(mail->pipe, getstring2(NULL, NICK_REG_MAIL_LINE_5),
            NetworkName);
    fprintf(mail->pipe, "\n.\n");
    MailEnd(mail);
    return 0;
}

static int do_confirm(User * u)
{

    NickRequest *nr = NULL;
    NickAlias *na = NULL;
    char *passcode = strtok(NULL, " ");
    char *pass = NULL;
    char *email = NULL;
    int forced = 0;
    User *utmp = NULL;
#ifdef USE_ENCRYPTION
    int len;
#endif
    nr = findrequestnick(u->nick);

    if (NSEmailReg) {
        if (!passcode) {
            notice_lang(s_NickServ, u, NICK_CONFIRM_INVALID);
            return MOD_CONT;
        }

        if (!nr) {
            if (is_services_admin(u)) {
/* If an admin, thier nick is obviously already regged, so look at the passcode to get the nick
   of the user they are trying to validate, and push that user through regardless of passcode */
                nr = findrequestnick(passcode);
                if (nr) {
                    utmp = finduser(passcode);
                    if (utmp) {
                        sprintf(passcode,
                                "FORCE_ACTIVATION_DUE_TO_OPER_CONFIRM %s",
                                nr->passcode);
                        passcode = strtok(passcode, " ");
                        notice_lang(s_NickServ, u, NICK_FORCE_REG,
                                    nr->nick);
                        do_confirm(utmp);
                        return MOD_CONT;
                    } else {
                        passcode = sstrdup(nr->passcode);
                        forced = 1;
                    }
                } else {
                    notice_lang(s_NickServ, u, NICK_CONFIRM_NOT_FOUND,
                                s_NickServ);
                    return MOD_CONT;
                }
            } else {
                notice_lang(s_NickServ, u, NICK_CONFIRM_NOT_FOUND,
                            s_NickServ);
                return MOD_CONT;
            }
        }

        if (stricmp(nr->passcode, passcode) != 0) {
            notice_lang(s_NickServ, u, NICK_CONFIRM_INVALID);
            return MOD_CONT;
        }
    }

    if (!nr) {
        notice_lang(s_NickServ, u, NICK_REGISTRATION_FAILED);
        return MOD_CONT;
    }
    pass = sstrdup(nr->password);

    if (nr->email) {
        email = sstrdup(nr->email);
    }
    na = makenick(nr->nick);

    if (na) {
        int i;
        char tsbuf[16];

#ifdef USE_ENCRYPTION
        len = strlen(pass);
        na->nc->pass = smalloc(PASSMAX);
        if (encrypt(pass, len, na->nc->pass, PASSMAX) < 0) {
            memset(pass, 0, strlen(pass));
            alog("%s: Failed to encrypt password for %s (register)",
                 s_NickServ, nr->nick);
            notice_lang(s_NickServ, u, NICK_REGISTRATION_FAILED);
            return MOD_CONT;
        }
        memset(pass, 0, strlen(pass));
        na->status = (int16) (NS_IDENTIFIED | NS_RECOGNIZED);
        na->nc->flags |= NI_ENCRYPTEDPW;
#else
        na->nc->pass = sstrdup(pass);
        na->status = (int16) (NS_IDENTIFIED | NS_RECOGNIZED);
#endif
        na->nc->flags |= NSDefFlags;
        for (i = 0; i < RootNumber; i++) {
            if (!stricmp(ServicesRoots[i], nr->nick)) {
                na->nc->flags |= NI_SERVICES_ROOT;
                break;
            }
        }
        na->nc->memos.memomax = MSMaxMemos;
        na->nc->channelmax = CSMaxReg;
        if (forced == 1) {
            na->last_usermask = sstrdup("*@*");
            na->last_realname = sstrdup("unknown");
        } else {
            na->last_usermask =
                scalloc(strlen(common_get_vident(u)) +
                        strlen(common_get_vhost(u)) + 2, 1);
            sprintf(na->last_usermask, "%s@%s", common_get_vident(u),
                    common_get_vhost(u));
            na->last_realname = sstrdup(u->realname);
        }
        na->time_registered = na->last_seen = time(NULL);
        na->nc->accesscount = 1;
        na->nc->access = scalloc(sizeof(char *), 1);
        na->nc->access[0] = create_mask(u);
        na->nc->language = NSDefLanguage;
        if (email)
            na->nc->email = sstrdup(email);
        if (forced != 1) {
            u->na = na;
            na->u = u;
            alog("%s: '%s' registered by %s@%s (e-mail: %s)", s_NickServ,
                 u->nick, u->username, common_get_vhost(u),
                 (email ? email : "none"));
            notice_lang(s_NickServ, u, NICK_REGISTERED, u->nick,
                        na->nc->access[0]);
#ifndef USE_ENCRYPTION
            notice_lang(s_NickServ, u, NICK_PASSWORD_IS, na->nc->pass);
#endif
            u->lastnickreg = time(NULL);
            if (ircd->modeonreg) {
                if (ircd->tsonmode) {
                    snprintf(tsbuf, sizeof(tsbuf), "%lu",
                             (unsigned long int) u->timestamp);
                    common_svsmode(u, ircd->modeonreg, tsbuf);
                } else {
                    common_svsmode(u, ircd->modeonreg, NULL);
                }
            }

        } else {
            notice_lang(s_NickServ, u, NICK_FORCE_REG, nr->nick);
        }
        delnickrequest(nr);     /* remove the nick request */
    } else {
        alog("%s: makenick(%s) failed", s_NickServ, u->nick);
        notice_lang(s_NickServ, u, NICK_REGISTRATION_FAILED);
    }

    /* Enable nick tracking if enabled */
    if (NSNickTracking)
        nsStartNickTracking(u);

    return MOD_CONT;
}

/*************************************************************************/

/* Register a nick in a specified group. */

static int do_group(User * u)
{
    NickAlias *na, *target;
    NickCore *nc;
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    int i;
    char tsbuf[16];

    if (NSEmailReg && (findrequestnick(u->nick))) {
        notice_lang(s_NickServ, u, NICK_REQUESTED);
        return MOD_CONT;
    }

    if (readonly) {
        notice_lang(s_NickServ, u, NICK_GROUP_DISABLED);
        return MOD_CONT;
    }
    if (checkDefCon(DEFCON_NO_NEW_NICKS)) {
        notice_lang(s_NickServ, u, OPER_DEFCON_DENIED);
        return MOD_CONT;
    }

    if (RestrictOperNicks) {
        for (i = 0; i < RootNumber; i++) {
            if (stristr(u->nick, ServicesRoots[i]) && !is_oper(u)) {
                notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED,
                            u->nick);
                return MOD_CONT;
            }
        }
        for (i = 0; i < servadmins.count && (nc = servadmins.list[i]); i++) {
            if (stristr(u->nick, nc->display) && !is_oper(u)) {
                notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED,
                            u->nick);
                return MOD_CONT;
            }
        }
        for (i = 0; i < servopers.count && (nc = servopers.list[i]); i++) {
            if (stristr(u->nick, nc->display) && !is_oper(u)) {
                notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED,
                            u->nick);
                return MOD_CONT;
            }
        }
    }

    if (!nick || !pass) {
        syntax_error(s_NickServ, u, "GROUP", NICK_GROUP_SYNTAX);
    } else if (time(NULL) < u->lastnickreg + NSRegDelay) {
        notice_lang(s_NickServ, u, NICK_GROUP_PLEASE_WAIT, NSRegDelay);
    } else if (u->na && (u->na->status & NS_VERBOTEN)) {
        alog("%s: %s@%s tried to use GROUP from FORBIDden nick %s",
             s_NickServ, u->username, common_get_vhost(u), u->nick);
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, u->nick);
    } else if (u->na && NSNoGroupChange) {
        notice_lang(s_NickServ, u, NICK_GROUP_CHANGE_DISABLED, s_NickServ);
    } else if (u->na && !nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (!(target = findnick(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (target->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
    } else if (u->na && target->nc == u->na->nc) {
        notice_lang(s_NickServ, u, NICK_GROUP_SAME, target->nick);
    } else if (NSMaxAliases && (target->nc->aliases.count >= NSMaxAliases)
               && !nick_is_services_admin(target->nc)) {
        notice_lang(s_NickServ, u, NICK_GROUP_TOO_MANY, target->nick,
                    s_NickServ, s_NickServ);
    } else if (check_password(pass, target->nc->pass) != 1) {
        alog("%s: Failed GROUP for %s!%s@%s (invalid password)",
             s_NickServ, u->nick, u->username, common_get_vhost(u));
        notice_lang(s_NickServ, u, PASSWORD_INCORRECT);
        bad_password(u);
    } else {
        /* If the nick is already registered, drop it.
         * If not, check that it is valid.
         */
        if (u->na) {
            delnick(u->na);
        } else {
            int prefixlen = strlen(NSGuestNickPrefix);
            int nicklen = strlen(u->nick);

            if (nicklen <= prefixlen + 7 && nicklen >= prefixlen + 1
                && stristr(u->nick, NSGuestNickPrefix) == u->nick
                && strspn(u->nick + prefixlen,
                          "1234567890") == nicklen - prefixlen) {
                notice_lang(s_NickServ, u, NICK_CANNOT_BE_REGISTERED,
                            u->nick);
                return MOD_CONT;
            }
        }
        na = makealias(u->nick, target->nc);

        if (na) {
            na->last_usermask =
                scalloc(strlen(common_get_vident(u)) +
                        strlen(common_get_vhost(u)) + 2, 1);
            sprintf(na->last_usermask, "%s@%s", common_get_vident(u),
                    common_get_vhost(u));
            na->last_realname = sstrdup(u->realname);
            na->time_registered = na->last_seen = time(NULL);
            na->status = (int16) (NS_IDENTIFIED | NS_RECOGNIZED);

            if (!(na->nc->flags & NI_SERVICES_ROOT)) {
                for (i = 0; i < RootNumber; i++) {
                    if (!stricmp(ServicesRoots[i], u->nick)) {
                        na->nc->flags |= NI_SERVICES_ROOT;
                        break;
                    }
                }
            }

            u->na = na;
            na->u = u;

#ifdef USE_RDB
            /* Is this really needed? Since this is a new alias it will get
             * its unique id on the next update, since it was previously
             * deleted by delnick. Must observe...
             */
            if (rdb_open()) {
                rdb_save_ns_alias(na);
                rdb_close();
            }
#endif
            alog("%s: %s!%s@%s makes %s join group of %s (%s) (e-mail: %s)", s_NickServ, u->nick, u->username, common_get_vhost(u), u->nick, target->nick, target->nc->display, (target->nc->email ? target->nc->email : "none"));
            notice_lang(s_NickServ, u, NICK_GROUP_JOINED, target->nick);

            u->lastnickreg = time(NULL);
            snprintf(tsbuf, sizeof(tsbuf), "%lu",
                     (unsigned long int) u->timestamp);
            if (ircd->modeonreg) {
                if (ircd->tsonmode) {
                    common_svsmode(u, ircd->modeonreg, tsbuf);
                } else {
                    common_svsmode(u, ircd->modeonreg, NULL);
                }
            }

            check_memos(u);
        } else {
            alog("%s: makealias(%s) failed", s_NickServ, u->nick);
            notice_lang(s_NickServ, u, NICK_GROUP_FAILED);
        }
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_nickupdate(User * u)
{
    NickAlias *na;

    if (!nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else {
        na = u->na;
        if (NSModeOnID)
            do_setmodes(u);
        check_memos(u);
        if (na->last_realname)
            free(na->last_realname);
        na->last_realname = sstrdup(u->realname);
        na->status |= NS_IDENTIFIED;
        na->last_seen = time(NULL);
        if (ircd->vhost) {
            do_on_id(u);
        }
        notice_lang(s_NickServ, u, NICK_UPDATE_SUCCESS, s_NickServ);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_identify(User * u)
{
    char *pass = strtok(NULL, " ");
    NickAlias *na;
    NickRequest *nr;
    int res;
    char tsbuf[16];

    if (!pass) {
        syntax_error(s_NickServ, u, "IDENTIFY", NICK_IDENTIFY_SYNTAX);
    } else if (!(na = u->na)) {
        if ((nr = findrequestnick(u->nick))) {
            notice_lang(s_NickServ, u, NICK_IS_PREREG);
        } else {
            notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
        }
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else if (nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_ALREADY_IDENTIFIED);
    } else if (!(res = check_password(pass, na->nc->pass))) {
        alog("%s: Failed IDENTIFY for %s!%s@%s", s_NickServ, u->nick,
             u->username, common_get_vhost(u));
        notice_lang(s_NickServ, u, PASSWORD_INCORRECT);
        bad_password(u);
    } else if (res == -1) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_FAILED);
    } else {
        if (!(na->status & NS_IDENTIFIED) && !(na->status & NS_RECOGNIZED)) {
            if (na->last_usermask)
                free(na->last_usermask);
            na->last_usermask =
                scalloc(strlen(common_get_vident(u)) +
                        strlen(common_get_vhost(u)) + 2, 1);
            sprintf(na->last_usermask, "%s@%s", common_get_vident(u),
                    common_get_vhost(u));
            if (na->last_realname)
                free(na->last_realname);
            na->last_realname = sstrdup(u->realname);
        }

        na->status |= NS_IDENTIFIED;
        na->last_seen = time(NULL);
        snprintf(tsbuf, sizeof(tsbuf), "%lu",
                 (unsigned long int) u->timestamp);

        if (ircd->modeonreg) {
            if (ircd->tsonmode) {
                common_svsmode(u, ircd->modeonreg, tsbuf);
            } else {
                common_svsmode(u, ircd->modeonreg, "");
            }
        }

        alog("%s: %s!%s@%s identified for nick %s", s_NickServ, u->nick,
             u->username, common_get_vhost(u), u->nick);
        notice_lang(s_NickServ, u, NICK_IDENTIFY_SUCCEEDED);
        if (ircd->vhost) {
            do_on_id(u);
        }
        if (NSModeOnID) {
            do_setmodes(u);
        }

        if (NSForceEmail && u->na && !u->na->nc->email) {
            notice_lang(s_NickServ, u, NICK_IDENTIFY_EMAIL_REQUIRED);
            notice_help(s_NickServ, u, NICK_IDENTIFY_EMAIL_HOWTO);
        }

        if (!(na->status & NS_RECOGNIZED))
            check_memos(u);

        /* Enable nick tracking if enabled */
        if (NSNickTracking)
            nsStartNickTracking(u);

        /* Clear any timers */
        if (na->nc->flags & NI_KILLPROTECT) {
            del_ns_timeout(na, TO_COLLIDE);
        }

    }
    return MOD_CONT;
}

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

static int do_setmodes(User * u)
{
    struct u_chanlist *uc;
    Channel *c;
    char *chan;

    /* Walk users current channels */
    for (uc = u->chans; uc; uc = uc->next) {
        if ((c = uc->chan)) {
            chan = c->name;
            if (ircd->owner && should_mode_change(uc->status, CUS_OWNER)
                && check_should_owner(u, chan)) {
                chan_set_user_status(c, u, CUS_OWNER);
            } else if (ircd->protect
                       && should_mode_change(uc->status, CUS_PROTECT)
                       && check_should_protect(u, chan)) {
                chan_set_user_status(c, u, CUS_PROTECT);
            } else if (should_mode_change(uc->status, CUS_OP)
                       && check_should_op(u, chan)) {
                chan_set_user_status(c, u, CUS_OP);
            } else if (ircd->halfop
                       && should_mode_change(uc->status, CUS_HALFOP)
                       && check_should_halfop(u, chan)) {
                chan_set_user_status(c, u, CUS_HALFOP);
            } else if (should_mode_change(uc->status, CUS_VOICE)
                       && check_should_voice(u, chan)) {
                chan_set_user_status(c, u, CUS_VOICE);
            }
        }
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_logout(User * u)
{
    char *nick = strtok(NULL, " ");
    char *param = strtok(NULL, " ");
    User *u2;

    if (!is_services_admin(u) && nick) {
        syntax_error(s_NickServ, u, "LOGOUT", NICK_LOGOUT_SYNTAX);
    } else if (!(u2 = (nick ? finduser(nick) : u))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (!u2->na) {
        if (nick)
            notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
        else
            notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
    } else if (u2->na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, u2->na->nick);
    } else if (!nick && !nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (nick && is_services_admin(u2)) {
        notice_lang(s_NickServ, u, NICK_LOGOUT_SERVICESADMIN, nick);
    } else {
        if (nick && param && !stricmp(param, "REVALIDATE")) {
            cancel_user(u2);
            validate_user(u2);
        } else {
            u2->na->status &= ~(NS_IDENTIFIED | NS_RECOGNIZED);
        }

        if (ircd->modeonreg) {
            common_svsmode(u2, ircd->modeonunreg, "1");
        }

        u->isSuperAdmin = 0;    /* Dont let people logout and remain a SuperAdmin */
        alog("%s: %s!%s@%s logged out nickname %s", s_NickServ, u->nick,
             u->username, common_get_vhost(u), u2->nick);

        if (nick)
            notice_lang(s_NickServ, u, NICK_LOGOUT_X_SUCCEEDED, nick);
        else
            notice_lang(s_NickServ, u, NICK_LOGOUT_SUCCEEDED);

        /* Stop nick tracking if enabled */
        if (NSNickTracking)
            nsStopNickTracking(u);

        /* Clear any timers again */
        if (u->na->nc->flags & NI_KILLPROTECT) {
            del_ns_timeout(u->na, TO_COLLIDE);
        }

    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_drop(User * u)
{
    char *nick = strtok(NULL, " ");
    NickAlias *na;
    NickRequest *nr = NULL;
    int is_servadmin = is_services_admin(u);
    int is_mine;                /* Does the nick being dropped belong to the user that is dropping? */

    if (readonly && !is_servadmin) {
        notice_lang(s_NickServ, u, NICK_DROP_DISABLED);
        return MOD_CONT;
    }

    if (!(na = (nick ? findnick(nick) : u->na))) {
        if (nick) {
            if ((nr = findrequestnick(nick)) && is_servadmin) {
                if (readonly)
                    notice_lang(s_NickServ, u, READ_ONLY_MODE);
                if (WallDrop)
                    anope_cmd_global(s_NickServ,
                                     "\2%s\2 used DROP on \2%s\2", u->nick,
                                     nick);
                alog("%s: %s!%s@%s dropped nickname %s (e-mail: %s)",
                     s_NickServ, u->nick, u->username, common_get_vhost(u),
                     nr->nick, nr->email);
                delnickrequest(nr);
                notice_lang(s_NickServ, u, NICK_X_DROPPED, nick);
            } else {
                notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
            }
        } else
            notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
        return MOD_CONT;
    }

    is_mine = (u->na && (u->na->nc == na->nc));

    if (is_mine && !nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (!is_mine && !is_servadmin) {
        notice_lang(s_NickServ, u, ACCESS_DENIED);
    } else if (NSSecureAdmins && !is_mine && nick_is_services_admin(na->nc)
               && !is_services_root(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
    } else {
        if (readonly)
            notice_lang(s_NickServ, u, READ_ONLY_MODE);

        alog("%s: %s!%s@%s dropped nickname %s (group %s) (e-mail: %s)",
             s_NickServ, u->nick, u->username, common_get_vhost(u),
             na->nick, na->nc->display,
             (na->nc->email ? na->nc->email : "none"));
        delnick(na);

        if (!is_mine) {
            if (WallDrop)
                anope_cmd_global(s_NickServ, "\2%s\2 used DROP on \2%s\2",
                                 u->nick, nick);
            notice_lang(s_NickServ, u, NICK_X_DROPPED, nick);
        } else {
            if (nick)
                notice_lang(s_NickServ, u, NICK_X_DROPPED, nick);
            else
                notice_lang(s_NickServ, u, NICK_DROPPED);
        }
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set(User * u)
{
    char *cmd = strtok(NULL, " ");
    char *param = strtok(NULL, " ");

    NickAlias *na;
    int is_servadmin = is_services_admin(u);
    int set_nick = 0;

    if (readonly) {
        notice_lang(s_NickServ, u, NICK_SET_DISABLED);
        return MOD_CONT;
    }

    if (is_servadmin && cmd && (na = findnick(cmd))) {
        cmd = param;
        param = strtok(NULL, " ");
        set_nick = 1;
    } else {
        na = u->na;
    }

    if (!param
        && (!cmd
            || (stricmp(cmd, "URL") != 0 && stricmp(cmd, "EMAIL") != 0
                && stricmp(cmd, "GREET") != 0
                && stricmp(cmd, "ICQ") != 0))) {
        if (is_servadmin) {
            syntax_error(s_NickServ, u, "SET", NICK_SET_SERVADMIN_SYNTAX);
        } else {
            syntax_error(s_NickServ, u, "SET", NICK_SET_SYNTAX);
        }
    } else if (!na) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else if (!is_servadmin && !nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (stricmp(cmd, "DISPLAY") == 0) {
        do_set_display(u, na->nc, param);
    } else if (stricmp(cmd, "PASSWORD") == 0) {
        do_set_password(u, na->nc, param);
    } else if (stricmp(cmd, "LANGUAGE") == 0) {
        do_set_language(u, na->nc, param);
    } else if (stricmp(cmd, "URL") == 0) {
        do_set_url(u, na->nc, param);
    } else if (stricmp(cmd, "EMAIL") == 0) {
        do_set_email(u, na->nc, param);
    } else if (stricmp(cmd, "ICQ") == 0) {
        do_set_icq(u, na->nc, param);
    } else if (stricmp(cmd, "GREET") == 0) {
        do_set_greet(u, na->nc, param);
    } else if (stricmp(cmd, "KILL") == 0) {
        do_set_kill(u, na->nc, param);
    } else if (stricmp(cmd, "SECURE") == 0) {
        do_set_secure(u, na->nc, param);
    } else if (stricmp(cmd, "PRIVATE") == 0) {
        do_set_private(u, na->nc, param);
    } else if (stricmp(cmd, "MSG") == 0) {
        do_set_msg(u, na->nc, param);
    } else if (stricmp(cmd, "HIDE") == 0) {
        do_set_hide(u, na->nc, param);
    } else if (stricmp(cmd, "NOEXPIRE") == 0) {
        do_set_noexpire(u, na, param);
    } else {
        if (is_servadmin)
            notice_lang(s_NickServ, u, NICK_SET_UNKNOWN_OPTION_OR_BAD_NICK,
                        cmd);
        else
            notice_lang(s_NickServ, u, NICK_SET_UNKNOWN_OPTION, cmd);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_display(User * u, NickCore * nc, char *param)
{
    int i;
    NickAlias *na;

    /* First check whether param is a valid nick of the group */
    for (i = 0; i < nc->aliases.count; i++) {
        na = nc->aliases.list[i];
        if (!stricmp(na->nick, param)) {
            param = na->nick;   /* Because case may differ */
            break;
        }
    }

    if (i == nc->aliases.count) {
        notice_lang(s_NickServ, u, NICK_SET_DISPLAY_INVALID);
        return MOD_CONT;
    }

    change_core_display(nc, param);
    notice_lang(s_NickServ, u, NICK_SET_DISPLAY_CHANGED, nc->display);

    /* Enable nick tracking if enabled */
    if (NSNickTracking)
        nsStartNickTracking(u);

    return MOD_CONT;
}

/*************************************************************************/

static int do_set_password(User * u, NickCore * nc, char *param)
{
    int len = strlen(param);

    if (NSSecureAdmins && u->na->nc != nc && nick_is_services_admin(nc)
        && !is_services_root(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
        return MOD_CONT;
    } else if (stricmp(nc->display, param) == 0
               || (StrictPasswords && len < 5)) {
        notice_lang(s_NickServ, u, MORE_OBSCURE_PASSWORD);
        return MOD_CONT;
    }

    if (nc->pass)
        free(nc->pass);

#ifdef USE_ENCRYPTION
    nc->pass = smalloc(PASSMAX);

    if (encrypt(param, len, nc->pass, PASSMAX) < 0) {
        memset(param, 0, len);
        alog("%s: Failed to encrypt password for %s (set)", s_NickServ,
             nc->display);
        notice_lang(s_NickServ, u, NICK_SET_PASSWORD_FAILED);
        return MOD_CONT;
    }

    memset(param, 0, len);
    notice_lang(s_NickServ, u, NICK_SET_PASSWORD_CHANGED);
#else
    nc->pass = sstrdup(param);
    notice_lang(s_NickServ, u, NICK_SET_PASSWORD_CHANGED_TO, nc->pass);
#endif

    if (u->na && u->na->nc != nc && is_services_admin(u)) {
        alog("%s: %s!%s@%s used SET PASSWORD as Services admin on %s (e-mail: %s)", s_NickServ, u->nick, u->username, common_get_vhost(u), nc->display, (nc->email ? nc->email : "none"));
        if (WallSetpass)
            anope_cmd_global(s_NickServ,
                             "\2%s\2 used SET PASSWORD as Services admin on \2%s\2",
                             u->nick, nc->display);
    } else {
        alog("%s: %s!%s@%s (e-mail: %s) changed its password.", s_NickServ,
             u->nick, u->username, common_get_vhost(u),
             (nc->email ? nc->email : "none"));
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_language(User * u, NickCore * nc, char *param)
{
    int langnum;

    if (param[strspn(param, "0123456789")] != 0) {      /* i.e. not a number */
        syntax_error(s_NickServ, u, "SET LANGUAGE",
                     NICK_SET_LANGUAGE_SYNTAX);
        return MOD_CONT;
    }
    langnum = atoi(param) - 1;
    if (langnum < 0 || langnum >= NUM_LANGS || langlist[langnum] < 0) {
        notice_lang(s_NickServ, u, NICK_SET_LANGUAGE_UNKNOWN, langnum + 1,
                    s_NickServ);
        return MOD_CONT;
    }
    nc->language = langlist[langnum];
    notice_lang(s_NickServ, u, NICK_SET_LANGUAGE_CHANGED);
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_url(User * u, NickCore * nc, char *param)
{
    if (nc->url)
        free(nc->url);

    if (param) {
        nc->url = sstrdup(param);
        notice_lang(s_NickServ, u, NICK_SET_URL_CHANGED, param);
    } else {
        nc->url = NULL;
        notice_lang(s_NickServ, u, NICK_SET_URL_UNSET);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_email(User * u, NickCore * nc, char *param)
{
    if (!param && NSForceEmail) {
        notice_lang(s_NickServ, u, NICK_SET_EMAIL_UNSET_IMPOSSIBLE);
        return MOD_CONT;
    } else if (param && !MailValidate(param)) {
        notice_lang(s_NickServ, u, MAIL_X_INVALID, param);
        return MOD_CONT;
    }

    alog("%s: %s!%s@%s (e-mail: %s) changed its e-mail to %s.", s_NickServ,
         u->nick, u->username, common_get_vhost(u),
         (nc->email ? nc->email : "none"), (param ? param : "none"));

    if (nc->email)
        free(nc->email);

    if (param) {
        nc->email = sstrdup(param);
        notice_lang(s_NickServ, u, NICK_SET_EMAIL_CHANGED, param);
    } else {
        nc->email = NULL;
        notice_lang(s_NickServ, u, NICK_SET_EMAIL_UNSET);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_icq(User * u, NickCore * nc, char *param)
{
    if (param) {
        int32 tmp = atol(param);
        if (!tmp) {
            notice_lang(s_NickServ, u, NICK_SET_ICQ_INVALID, param);
        } else {
            nc->icq = tmp;
            notice_lang(s_NickServ, u, NICK_SET_ICQ_CHANGED, param);
        }
    } else {
        nc->icq = 0;
        notice_lang(s_NickServ, u, NICK_SET_ICQ_UNSET);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_greet(User * u, NickCore * nc, char *param)
{
    if (nc->greet)
        free(nc->greet);

    if (param) {
        char buf[BUFSIZE];
        char *end = strtok(NULL, "");

        snprintf(buf, sizeof(buf), "%s%s%s", param, (end ? " " : ""),
                 (end ? end : ""));

        nc->greet = sstrdup(buf);
        notice_lang(s_NickServ, u, NICK_SET_GREET_CHANGED, buf);
    } else {
        nc->greet = NULL;
        notice_lang(s_NickServ, u, NICK_SET_GREET_UNSET);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_kill(User * u, NickCore * nc, char *param)
{
    if (stricmp(param, "ON") == 0) {
        nc->flags |= NI_KILLPROTECT;
        nc->flags &= ~(NI_KILL_QUICK | NI_KILL_IMMED);
        notice_lang(s_NickServ, u, NICK_SET_KILL_ON);
    } else if (stricmp(param, "QUICK") == 0) {
        nc->flags |= NI_KILLPROTECT | NI_KILL_QUICK;
        nc->flags &= ~NI_KILL_IMMED;
        notice_lang(s_NickServ, u, NICK_SET_KILL_QUICK);
    } else if (stricmp(param, "IMMED") == 0) {
        if (NSAllowKillImmed) {
            nc->flags |= NI_KILLPROTECT | NI_KILL_IMMED;
            nc->flags &= ~NI_KILL_QUICK;
            notice_lang(s_NickServ, u, NICK_SET_KILL_IMMED);
        } else {
            notice_lang(s_NickServ, u, NICK_SET_KILL_IMMED_DISABLED);
        }
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~(NI_KILLPROTECT | NI_KILL_QUICK | NI_KILL_IMMED);
        notice_lang(s_NickServ, u, NICK_SET_KILL_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET KILL",
                     NSAllowKillImmed ? NICK_SET_KILL_IMMED_SYNTAX :
                     NICK_SET_KILL_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_secure(User * u, NickCore * nc, char *param)
{
    if (stricmp(param, "ON") == 0) {
        nc->flags |= NI_SECURE;
        notice_lang(s_NickServ, u, NICK_SET_SECURE_ON);
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~NI_SECURE;
        notice_lang(s_NickServ, u, NICK_SET_SECURE_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET SECURE", NICK_SET_SECURE_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_private(User * u, NickCore * nc, char *param)
{
    if (stricmp(param, "ON") == 0) {
        nc->flags |= NI_PRIVATE;
        notice_lang(s_NickServ, u, NICK_SET_PRIVATE_ON);
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~NI_PRIVATE;
        notice_lang(s_NickServ, u, NICK_SET_PRIVATE_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET PRIVATE",
                     NICK_SET_PRIVATE_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_msg(User * u, NickCore * nc, char *param)
{
    if (!UsePrivmsg) {
        notice_lang(s_NickServ, u, NICK_SET_OPTION_DISABLED, "MSG");
        return MOD_CONT;
    }

    if (stricmp(param, "ON") == 0) {
        nc->flags |= NI_MSG;
        notice_lang(s_NickServ, u, NICK_SET_MSG_ON);
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~NI_MSG;
        notice_lang(s_NickServ, u, NICK_SET_MSG_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET MSG", NICK_SET_MSG_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_hide(User * u, NickCore * nc, char *param)
{
    int flag, onmsg, offmsg;

    if (stricmp(param, "EMAIL") == 0) {
        flag = NI_HIDE_EMAIL;
        onmsg = NICK_SET_HIDE_EMAIL_ON;
        offmsg = NICK_SET_HIDE_EMAIL_OFF;
    } else if (stricmp(param, "USERMASK") == 0) {
        flag = NI_HIDE_MASK;
        onmsg = NICK_SET_HIDE_MASK_ON;
        offmsg = NICK_SET_HIDE_MASK_OFF;
    } else if (stricmp(param, "STATUS") == 0) {
        flag = NI_HIDE_STATUS;
        onmsg = NICK_SET_HIDE_STATUS_ON;
        offmsg = NICK_SET_HIDE_STATUS_OFF;
    } else if (stricmp(param, "QUIT") == 0) {
        flag = NI_HIDE_QUIT;
        onmsg = NICK_SET_HIDE_QUIT_ON;
        offmsg = NICK_SET_HIDE_QUIT_OFF;
    } else {
        syntax_error(s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
        return MOD_CONT;
    }

    param = strtok(NULL, " ");
    if (!param) {
        syntax_error(s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
    } else if (stricmp(param, "ON") == 0) {
        nc->flags |= flag;
        notice_lang(s_NickServ, u, onmsg, s_NickServ);
    } else if (stricmp(param, "OFF") == 0) {
        nc->flags &= ~flag;
        notice_lang(s_NickServ, u, offmsg, s_NickServ);
    } else {
        syntax_error(s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_noexpire(User * u, NickAlias * na, char *param)
{
    if (!is_services_admin(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
        return MOD_CONT;
    }
    if (!param) {
        syntax_error(s_NickServ, u, "SET NOEXPIRE",
                     NICK_SET_NOEXPIRE_SYNTAX);
        return MOD_CONT;
    }
    if (stricmp(param, "ON") == 0) {
        na->status |= NS_NO_EXPIRE;
        notice_lang(s_NickServ, u, NICK_SET_NOEXPIRE_ON, na->nick);
    } else if (stricmp(param, "OFF") == 0) {
        na->status &= ~NS_NO_EXPIRE;
        notice_lang(s_NickServ, u, NICK_SET_NOEXPIRE_OFF, na->nick);
    } else {
        syntax_error(s_NickServ, u, "SET NOEXPIRE",
                     NICK_SET_NOEXPIRE_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_link(User * u)
{
    notice_lang(s_NickServ, u, OBSOLETE_COMMAND, "GROUP");
    return MOD_CONT;
}

/*************************************************************************/

static int do_unlink(User * u)
{
    notice_lang(s_NickServ, u, OBSOLETE_COMMAND, "DROP");
    return MOD_CONT;
}

/*************************************************************************/

static int do_listlinks(User * u)
{
    notice_lang(s_NickServ, u, OBSOLETE_COMMAND, "GLIST");
    return MOD_CONT;
}

/*************************************************************************/

static int do_access(User * u)
{
    char *cmd = strtok(NULL, " ");
    char *mask = strtok(NULL, " ");
    NickAlias *na;
    int i;
    char **access;

    if (cmd && stricmp(cmd, "LIST") == 0 && mask && is_services_admin(u)
        && (na = findnick(mask))) {

        if (na->nc->accesscount == 0) {
            notice_lang(s_NickServ, u, NICK_ACCESS_LIST_X_EMPTY, na->nick);
            return MOD_CONT;
        }

        if (na->status & NS_VERBOTEN) {
            notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
            return MOD_CONT;
        }

        notice_lang(s_NickServ, u, NICK_ACCESS_LIST_X, mask);
        mask = strtok(NULL, " ");
        for (access = na->nc->access, i = 0; i < na->nc->accesscount;
             access++, i++) {
            if (mask && !match_wild(mask, *access))
                continue;
            notice_user(s_NickServ, u, "    %s", *access);
        }

    } else if (!cmd || ((stricmp(cmd, "LIST") == 0) ? !!mask : !mask)) {
        syntax_error(s_NickServ, u, "ACCESS", NICK_ACCESS_SYNTAX);

    } else if (mask && !strchr(mask, '@')) {
        notice_lang(s_NickServ, u, BAD_USERHOST_MASK);
        notice_lang(s_NickServ, u, MORE_INFO, s_NickServ, "ACCESS");

    } else if (!(na = u->na)) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);

    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);

    } else if (!nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);

    } else if (stricmp(cmd, "ADD") == 0) {
        if (na->nc->accesscount >= NSAccessMax) {
            notice_lang(s_NickServ, u, NICK_ACCESS_REACHED_LIMIT,
                        NSAccessMax);
            return MOD_CONT;
        }

        for (access = na->nc->access, i = 0; i < na->nc->accesscount;
             access++, i++) {
            if (strcmp(*access, mask) == 0) {
                notice_lang(s_NickServ, u, NICK_ACCESS_ALREADY_PRESENT,
                            *access);
                return MOD_CONT;
            }
        }

        na->nc->accesscount++;
        na->nc->access =
            srealloc(na->nc->access, sizeof(char *) * na->nc->accesscount);
        na->nc->access[na->nc->accesscount - 1] = sstrdup(mask);
        notice_lang(s_NickServ, u, NICK_ACCESS_ADDED, mask);

    } else if (stricmp(cmd, "DEL") == 0) {

        for (access = na->nc->access, i = 0; i < na->nc->accesscount;
             access++, i++) {
            if (stricmp(*access, mask) == 0)
                break;
        }
        if (i == na->nc->accesscount) {
            notice_lang(s_NickServ, u, NICK_ACCESS_NOT_FOUND, mask);
            return MOD_CONT;
        }

        notice_lang(s_NickServ, u, NICK_ACCESS_DELETED, *access);
        free(*access);
        na->nc->accesscount--;
        if (i < na->nc->accesscount)    /* if it wasn't the last entry... */
            memmove(access, access + 1,
                    (na->nc->accesscount - i) * sizeof(char *));
        if (na->nc->accesscount)        /* if there are any entries left... */
            na->nc->access =
                srealloc(na->nc->access,
                         na->nc->accesscount * sizeof(char *));
        else {
            free(na->nc->access);
            na->nc->access = NULL;
        }
    } else if (stricmp(cmd, "LIST") == 0) {
        if (na->nc->accesscount == 0) {
            notice_lang(s_NickServ, u, NICK_ACCESS_LIST_EMPTY, u->nick);
            return MOD_CONT;
        }

        notice_lang(s_NickServ, u, NICK_ACCESS_LIST);
        for (access = na->nc->access, i = 0; i < na->nc->accesscount;
             access++, i++) {
            if (mask && !match_wild(mask, *access))
                continue;
            notice_user(s_NickServ, u, "    %s", *access);
        }
    } else {
        syntax_error(s_NickServ, u, "ACCESS", NICK_ACCESS_SYNTAX);

    }
    return MOD_CONT;
}

/*************************************************************************/

/* Show hidden info to nick owners and sadmins when the "ALL" parameter is
 * supplied. If a nick is online, the "Last seen address" changes to "Is
 * online from".
 * Syntax: INFO <nick> {ALL}
 * -TheShadow (13 Mar 1999)
 */

static int do_info(User * u)
{
    char *nick = strtok(NULL, " ");
    char *param = strtok(NULL, " ");

    NickAlias *na;
    NickRequest *nr = NULL;
    int is_servadmin = is_services_admin(u);

    if (!nick) {
        syntax_error(s_NickServ, u, "INFO", NICK_INFO_SYNTAX);
    } else if (!(na = findnick(nick))) {
        if ((nr = findrequestnick(nick))) {
            notice_lang(s_NickServ, u, NICK_IS_PREREG);
            if (param && stricmp(param, "ALL") == 0 && is_servadmin) {
                notice_lang(s_NickServ, u, NICK_INFO_EMAIL, nr->email);
            } else {
                if (is_servadmin) {
                    notice_lang(s_NickServ, u, NICK_INFO_FOR_MORE,
                                s_NickServ, nr->nick);
                }
            }
        } else if (nickIsServices(nick, 1)) {
            notice_lang(s_NickServ, u, NICK_X_IS_SERVICES, nick);
        } else {
            notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
        }
    } else if (na->status & NS_VERBOTEN) {
        if (is_oper(u) && na->last_usermask)
            notice_lang(s_NickServ, u, NICK_X_FORBIDDEN_OPER, nick,
                        na->last_usermask,
                        (na->last_realname ? na->
                         last_realname : getstring(u->na, NO_REASON)));
        else
            notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
    } else {
        struct tm *tm;
        char buf[BUFSIZE], *end;
        const char *commastr = getstring(u->na, COMMA_SPACE);
        int need_comma = 0;
        int nick_online = 0;
        int show_hidden = 0;
        time_t expt;

        /* Is the real owner of the nick we're looking up online? -TheShadow */
        if (na->status & (NS_RECOGNIZED | NS_IDENTIFIED))
            nick_online = 1;

        /* Only show hidden fields to owner and sadmins and only when the ALL
         * parameter is used. -TheShadow */
        if (param && stricmp(param, "ALL") == 0 && u->na
            && ((nick_identified(u) && (na->nc == u->na->nc))
                || is_servadmin))
            show_hidden = 1;

        notice_lang(s_NickServ, u, NICK_INFO_REALNAME, na->nick,
                    na->last_realname);

        if ((nick_identified(u) && (na->nc == u->na->nc)) || is_servadmin) {

            if (nick_is_services_root(na->nc))
                notice_lang(s_NickServ, u, NICK_INFO_SERVICES_ROOT,
                            na->nick);
            else if (nick_is_services_admin(na->nc))
                notice_lang(s_NickServ, u, NICK_INFO_SERVICES_ADMIN,
                            na->nick);
            else if (nick_is_services_oper(na->nc))
                notice_lang(s_NickServ, u, NICK_INFO_SERVICES_OPER,
                            na->nick);

        } else {

            if (nick_is_services_root(na->nc)
                && !(na->nc->flags & NI_HIDE_STATUS))
                notice_lang(s_NickServ, u, NICK_INFO_SERVICES_ROOT,
                            na->nick);
            else if (nick_is_services_admin(na->nc)
                     && !(na->nc->flags & NI_HIDE_STATUS))
                notice_lang(s_NickServ, u, NICK_INFO_SERVICES_ADMIN,
                            na->nick);
            else if (nick_is_services_oper(na->nc)
                     && !(na->nc->flags & NI_HIDE_STATUS))
                notice_lang(s_NickServ, u, NICK_INFO_SERVICES_OPER,
                            na->nick);

        }


        if (nick_online) {
            if (show_hidden || !(na->nc->flags & NI_HIDE_MASK))
                notice_lang(s_NickServ, u, NICK_INFO_ADDRESS_ONLINE,
                            na->last_usermask);
            else
                notice_lang(s_NickServ, u, NICK_INFO_ADDRESS_ONLINE_NOHOST,
                            na->nick);
        } else {
            if (show_hidden || !(na->nc->flags & NI_HIDE_MASK))
                notice_lang(s_NickServ, u, NICK_INFO_ADDRESS,
                            na->last_usermask);
        }

        tm = localtime(&na->time_registered);
        strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
        notice_lang(s_NickServ, u, NICK_INFO_TIME_REGGED, buf);

        if (!nick_online) {
            tm = localtime(&na->last_seen);
            strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT,
                          tm);
            notice_lang(s_NickServ, u, NICK_INFO_LAST_SEEN, buf);
        }

        if (na->last_quit
            && (show_hidden || !(na->nc->flags & NI_HIDE_QUIT)))
            notice_lang(s_NickServ, u, NICK_INFO_LAST_QUIT, na->last_quit);

        if (na->nc->url)
            notice_lang(s_NickServ, u, NICK_INFO_URL, na->nc->url);
        if (na->nc->email
            && (show_hidden || !(na->nc->flags & NI_HIDE_EMAIL)))
            notice_lang(s_NickServ, u, NICK_INFO_EMAIL, na->nc->email);
        if (na->nc->icq)
            notice_lang(s_NickServ, u, NICK_INFO_ICQ, na->nc->icq);

        if (show_hidden) {
            if (s_HostServ && ircd->vhost) {
                if (getvHost(na->nick) != NULL) {
                    if (ircd->vident && getvIdent(na->nick) != NULL) {
                        notice_lang(s_NickServ, u, NICK_INFO_VHOST2,
                                    getvIdent(na->nick),
                                    getvHost(na->nick));
                    } else {
                        notice_lang(s_NickServ, u, NICK_INFO_VHOST,
                                    getvHost(na->nick));
                    }
                }
            }
            if (na->nc->greet)
                notice_lang(s_NickServ, u, NICK_INFO_GREET, na->nc->greet);

            *buf = 0;
            end = buf;

            if (na->nc->flags & NI_KILLPROTECT) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s",
                                getstring(u->na, NICK_INFO_OPT_KILL));
                need_comma = 1;
            }
            if (na->nc->flags & NI_SECURE) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, NICK_INFO_OPT_SECURE));
                need_comma = 1;
            }
            if (na->nc->flags & NI_PRIVATE) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, NICK_INFO_OPT_PRIVATE));
                need_comma = 1;
            }
            if (na->nc->flags & NI_MSG) {
                end += snprintf(end, sizeof(buf) - (end - buf), "%s%s",
                                need_comma ? commastr : "",
                                getstring(u->na, NICK_INFO_OPT_MSG));
                need_comma = 1;
            }

            notice_lang(s_NickServ, u, NICK_INFO_OPTIONS,
                        *buf ? buf : getstring(u->na, NICK_INFO_OPT_NONE));

            if (na->status & NS_NO_EXPIRE) {
                notice_lang(s_NickServ, u, NICK_INFO_NO_EXPIRE);
            } else {
                if (is_services_admin(u)) {
                    expt = na->last_seen + NSExpire;
                    tm = localtime(&expt);
                    strftime_lang(buf, sizeof(buf), na->u,
                                  STRFTIME_DATE_TIME_FORMAT, tm);
                    notice_lang(s_NickServ, u, NICK_INFO_EXPIRE, buf);
                }
            }
        }

        if (!show_hidden
            && ((u->na && (na->nc == u->na->nc) && nick_identified(u))
                || is_servadmin))
            notice_lang(s_NickServ, u, NICK_INFO_FOR_MORE, s_NickServ,
                        na->nick);
    }
    return MOD_CONT;
}

/*************************************************************************/

/* SADMINS can search for nicks based on their NS_VERBOTEN and NS_NO_EXPIRE
 * status. The keywords FORBIDDEN and NOEXPIRE represent these two states
 * respectively. These keywords should be included after the search pattern.
 * Multiple keywords are accepted and should be separated by spaces. Only one
 * of the keywords needs to match a nick's state for the nick to be displayed.
 * Forbidden nicks can be identified by "[Forbidden]" appearing in the last
 * seen address field. Nicks with NOEXPIRE set are preceeded by a "!". Only
 * SADMINS will be shown forbidden nicks and the "!" indicator.
 * Syntax for sadmins: LIST pattern [FORBIDDEN] [NOEXPIRE]
 * -TheShadow
 */

static int do_list(User * u)
{
    char *pattern = strtok(NULL, " ");
    char *keyword;
    NickAlias *na;
    NickCore *mync;
    int nnicks, i;
    char buf[BUFSIZE];
    int is_servadmin = is_services_admin(u);
    int16 matchflags = 0;
    NickRequest *nr = NULL;
    int nronly = 0;
    char noexpire_char = ' ';
    int count = 0, from = 0, to = 0;
    char *tmp = NULL;
    char *s = NULL;

    if (NSListOpersOnly && !(is_oper(u))) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
        return MOD_CONT;
    }

    if (!pattern) {
        syntax_error(s_NickServ, u, "LIST",
                     is_servadmin ? NICK_LIST_SERVADMIN_SYNTAX :
                     NICK_LIST_SYNTAX);
    } else {

        if (pattern) {
            if (pattern[0] == '#') {
                tmp = myStrGetOnlyToken((pattern + 1), '-', 0); /* Read FROM out */
                if (!tmp) {
                    return MOD_CONT;
                }
                for (s = tmp; *s; s++) {
                    if (!isdigit(*s)) {
                        return MOD_CONT;
                    }
                }
                from = atoi(tmp);
                tmp = myStrGetTokenRemainder(pattern, '-', 1);  /* Read TO out */
                if (!tmp) {
                    return MOD_CONT;
                }
                for (s = tmp; *s; s++) {
                    if (!isdigit(*s)) {
                        return MOD_CONT;
                    }
                }
                to = atoi(tmp);
                pattern = sstrdup("*");
            }
        }

        nnicks = 0;

        while (is_servadmin && (keyword = strtok(NULL, " "))) {
            if (stricmp(keyword, "FORBIDDEN") == 0)
                matchflags |= NS_VERBOTEN;
            if (stricmp(keyword, "NOEXPIRE") == 0)
                matchflags |= NS_NO_EXPIRE;
            if (stricmp(keyword, "UNCONFIRMED") == 0)
                nronly = 1;
        }

        mync = (nick_identified(u) ? u->na->nc : NULL);

        notice_lang(s_NickServ, u, NICK_LIST_HEADER, pattern);
        if (nronly != 1) {
            for (i = 0; i < 1024; i++) {
                for (na = nalists[i]; na; na = na->next) {
                    /* Don't show private and forbidden nicks to non-services admins. */
                    if ((na->status & NS_VERBOTEN) && !is_servadmin)
                        continue;
                    if ((na->nc->flags & NI_PRIVATE) && !is_servadmin
                        && na->nc != mync)
                        continue;
                    if ((matchflags != 0) && !(na->status & matchflags))
                        continue;

                    /* We no longer compare the pattern against the output buffer.
                     * Instead we build a nice nick!user@host buffer to compare.
                     * The output is then generated separately. -TheShadow */
                    snprintf(buf, sizeof(buf), "%s!%s", na->nick,
                             (na->last_usermask
                              && !(na->status & NS_VERBOTEN)) ? na->
                             last_usermask : "*@*");
                    if (stricmp(pattern, na->nick) == 0
                        || match_wild_nocase(pattern, buf)) {

                        if ((((count + 1 >= from) && (count + 1 <= to))
                             || ((from == 0) && (to == 0)))
                            && (++nnicks <= NSListMax)) {
                            if (is_servadmin
                                && (na->status & NS_NO_EXPIRE))
                                noexpire_char = '!';
                            else {
                                noexpire_char = ' ';
                            }
                            if ((na->nc->flags & NI_HIDE_MASK)
                                && !is_servadmin && na->nc != mync) {
                                snprintf(buf, sizeof(buf),
                                         "%-20s  [Hostname Hidden]",
                                         na->nick);
                            } else if (na->status & NS_VERBOTEN) {
                                snprintf(buf, sizeof(buf),
                                         "%-20s  [Forbidden]", na->nick);
                            } else {
                                snprintf(buf, sizeof(buf), "%-20s  %s",
                                         na->nick, na->last_usermask);
                            }
                            notice_user(s_NickServ, u, "   %c%s",
                                        noexpire_char, buf);
                        }
                        count++;
                    }
                }
            }
        }

        if (nronly == 1 || (is_servadmin && matchflags == 0)) {
            noexpire_char = ' ';
            for (i = 0; i < 1024; i++) {
                for (nr = nrlists[i]; nr; nr = nr->next) {
                    snprintf(buf, sizeof(buf), "%s!*@*", nr->nick);
                    if (stricmp(pattern, nr->nick) == 0
                        || match_wild_nocase(pattern, buf)) {
                        if (++nnicks <= NSListMax) {
                            snprintf(buf, sizeof(buf),
                                     "%-20s  [UNCONFIRMED]", nr->nick);
                            notice_user(s_NickServ, u, "   %c%s",
                                        noexpire_char, buf);
                        }
                    }
                }
            }
        }
        notice_lang(s_NickServ, u, NICK_LIST_RESULTS,
                    nnicks > NSListMax ? NSListMax : nnicks, nnicks);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_glist(User * u)
{
    char *nick = strtok(NULL, " ");

    NickAlias *na, *na2;
    int i;

    if ((nick ? (stricmp(nick, u->nick) ? !is_services_admin(u)
                 : !nick_identified(u))
         : !nick_identified(u))) {
        notice_lang(s_NickServ, u, ACCESS_DENIED);
    } else if ((!nick ? !(na = u->na) : !(na = findnick(nick)))) {
        notice_lang(s_NickServ, u,
                    (!nick ? NICK_NOT_REGISTERED : NICK_X_NOT_REGISTERED),
                    nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else {
        time_t expt;
        struct tm *tm;
        char buf[BUFSIZE];
        int wont_expire;

        notice_lang(s_NickServ, u,
                    nick ? NICK_GLIST_HEADER_X : NICK_GLIST_HEADER,
                    na->nc->display);
        for (i = 0; i < na->nc->aliases.count; i++) {
            na2 = na->nc->aliases.list[i];
            if (na2->nc == na->nc) {
                if (!(wont_expire = na2->status & NS_NO_EXPIRE)) {
                    expt = na2->last_seen + NSExpire;
                    tm = localtime(&expt);
                    strftime_lang(buf, sizeof(buf), na2->u,
                                  STRFTIME_DATE_TIME_FORMAT, tm);
                }
                notice_lang(s_NickServ, u,
                            ((is_services_admin(u) && !wont_expire)
                             ? NICK_GLIST_REPLY_ADMIN : NICK_GLIST_REPLY),
                            (wont_expire ? '!' : ' '), na2->nick, buf);
            }
        }
        notice_lang(s_NickServ, u, NICK_GLIST_FOOTER,
                    na->nc->aliases.count);
    }
    return MOD_CONT;
}

/*************************************************************************/

/**
 * List the channels that the given nickname has access on
 *
 * /ns ALIST [level]
 * /ns ALIST [nickname] [level]
 *
 * -jester
 */
static int do_alist(User * u)
{
    char *nick = NULL;
    char *lev = NULL;

    NickAlias *na;

    int min_level = 0;
    int is_servadmin = is_services_admin(u);

    if (!is_servadmin) {
        /* Non service admins can only see their own levels */
        na = u->na;
    } else {
        /* Services admins can request ALIST on nicks.
         * The first argument for service admins must
         * always be a nickname.
         */
        nick = strtok(NULL, " ");

        /* If an argument was passed, use it as the nick to see levels
         * for, else check levels for the user calling the command */
        if (nick) {
            na = findnick(nick);
        } else {
            na = u->na;
        }
    }

    /* If available, get level from arguments */
    lev = strtok(NULL, " ");

    /* if a level was given, make sure it's an int for later */
    if (lev) {
        if (stricmp(lev, "FOUNDER") == 0) {
            min_level = ACCESS_FOUNDER;
        } else if (stricmp(lev, "SOP") == 0) {
            min_level = ACCESS_SOP;
        } else if (stricmp(lev, "AOP") == 0) {
            min_level = ACCESS_AOP;
        } else if (stricmp(lev, "HOP") == 0) {
            min_level = ACCESS_HOP;
        } else if (stricmp(lev, "VOP") == 0) {
            min_level = ACCESS_VOP;
        } else {
            min_level = atoi(lev);
        }
    }

    if (!nick_identified(u)) {
        notice_lang(s_NickServ, u, ACCESS_DENIED);
    } else if (is_servadmin && nick && !na) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else {
        int i, level;
        int chan_count = 0;
        int match_count = 0;
        ChannelInfo *ci;

        notice_lang(s_NickServ, u, (is_servadmin ? NICK_ALIST_HEADER_X :
                                    NICK_ALIST_HEADER), na->nick);

        for (i = 0; i < 256; i++) {
            for ((ci = chanlists[i]); ci; (ci = ci->next)) {

                if ((level = get_access_level(ci, na))) {
                    chan_count++;

                    if (min_level > level) {
                        continue;
                    }

                    match_count++;

                    if ((ci->flags & CI_XOP) || (level == ACCESS_FOUNDER)) {
                        char *xop;

                        xop = get_xop_level(level);

                        notice_lang(s_NickServ, u, NICK_ALIST_XOP_FORMAT,
                                    match_count,
                                    ((ci->
                                      flags & CI_NO_EXPIRE) ? '!' : ' '),
                                    ci->name, xop,
                                    (ci->desc ? ci->desc : ""));
                    } else {
                        notice_lang(s_NickServ, u,
                                    NICK_ALIST_ACCESS_FORMAT, match_count,
                                    ((ci->
                                      flags & CI_NO_EXPIRE) ? '!' : ' '),
                                    ci->name, level,
                                    (ci->desc ? ci->desc : ""));

                    }
                }
            }
        }

        notice_lang(s_NickServ, u, NICK_ALIST_FOOTER, match_count,
                    chan_count);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_recover(User * u)
{
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    NickAlias *na;
    User *u2;

    if (!nick) {
        syntax_error(s_NickServ, u, "RECOVER", NICK_RECOVER_SYNTAX);
    } else if (!(u2 = finduser(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (!(na = u2->na)) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else if (stricmp(nick, u->nick) == 0) {
        notice_lang(s_NickServ, u, NICK_NO_RECOVER_SELF);
    } else if (pass) {
        int res = check_password(pass, na->nc->pass);

        if (res == 1) {
            notice_lang(s_NickServ, u2, FORCENICKCHANGE_NOW);
            collide(na, 0);
            notice_lang(s_NickServ, u, NICK_RECOVERED, s_NickServ, nick);
        } else {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
            if (res == 0) {
                alog("%s: RECOVER: invalid password for %s by %s!%s@%s",
                     s_NickServ, nick, u->nick, u->username,
                     common_get_vhost(u));
                bad_password(u);
            }
        }
    } else {
        if (group_identified(u, na->nc)
            || (!(na->nc->flags & NI_SECURE) && is_on_access(u, na->nc))) {
            notice_lang(s_NickServ, u2, FORCENICKCHANGE_NOW);
            collide(na, 0);
            notice_lang(s_NickServ, u, NICK_RECOVERED, s_NickServ, nick);
        } else {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
        }
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_release(User * u)
{
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    NickAlias *na;

    if (!nick) {
        syntax_error(s_NickServ, u, "RELEASE", NICK_RELEASE_SYNTAX);
    } else if (!(na = findnick(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else if (!(na->status & NS_KILL_HELD)) {
        notice_lang(s_NickServ, u, NICK_RELEASE_NOT_HELD, nick);
    } else if (pass) {
        int res = check_password(pass, na->nc->pass);
        if (res == 1) {
            release(na, 0);
            notice_lang(s_NickServ, u, NICK_RELEASED);
        } else {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
            if (res == 0) {
                alog("%s: RELEASE: invalid password for %s by %s!%s@%s",
                     s_NickServ, nick, u->nick, u->username,
                     common_get_vhost(u));
                bad_password(u);
            }
        }
    } else {
        if (group_identified(u, na->nc)
            || (!(na->nc->flags & NI_SECURE) && is_on_access(u, na->nc))) {
            release(na, 0);
            notice_lang(s_NickServ, u, NICK_RELEASED);
        } else {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
        }
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_ghost(User * u)
{
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    NickAlias *na;
    User *u2;

    if (!nick) {
        syntax_error(s_NickServ, u, "GHOST", NICK_GHOST_SYNTAX);
    } else if (!(u2 = finduser(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (!(na = u2->na)) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else if (stricmp(nick, u->nick) == 0) {
        notice_lang(s_NickServ, u, NICK_NO_GHOST_SELF);
    } else if (pass) {
        int res = check_password(pass, na->nc->pass);
        if (res == 1) {
            char buf[NICKMAX + 32];
            snprintf(buf, sizeof(buf), "GHOST command used by %s",
                     u->nick);
            kill_user(s_NickServ, nick, buf);
            notice_lang(s_NickServ, u, NICK_GHOST_KILLED, nick);
        } else {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
            if (res == 0) {
                alog("%s: GHOST: invalid password for %s by %s!%s@%s",
                     s_NickServ, nick, u->nick, u->username,
                     common_get_vhost(u));
                bad_password(u);
            }
        }
    } else {
        if (group_identified(u, na->nc)
            || (!(na->nc->flags & NI_SECURE) && is_on_access(u, na->nc))) {
            char buf[NICKMAX + 32];
            snprintf(buf, sizeof(buf), "GHOST command used by %s",
                     u->nick);
            kill_user(s_NickServ, nick, buf);
            notice_lang(s_NickServ, u, NICK_GHOST_KILLED, nick);
        } else {
            notice_lang(s_NickServ, u, ACCESS_DENIED);
        }
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_status(User * u)
{
    char *nick;
    User *u2;
    int i = 0;

    while ((nick = strtok(NULL, " ")) && (i++ < 16)) {
        if (!(u2 = finduser(nick)))
            notice_lang(s_NickServ, u, NICK_STATUS_0, nick);
        else if (nick_identified(u2))
            notice_lang(s_NickServ, u, NICK_STATUS_3, nick);
        else if (nick_recognized(u2))
            notice_lang(s_NickServ, u, NICK_STATUS_2, nick);
        else
            notice_lang(s_NickServ, u, NICK_STATUS_1, nick);
    }
    return MOD_CONT;
}

/*************************************************************************/
/* A simple call to check for all emails that a user may have registered */
/* with. It returns the nicks that match the email you provide. Wild     */
/* Cards are not excepted. Must use user@email-host.                     */
/*************************************************************************/
static int do_getemail(User * u)
{
    char *email = strtok(NULL, " ");
    int i, j = 0;
    NickCore *nc;

    if (!email) {
        syntax_error(s_NickServ, u, "GETMAIL", NICK_GETEMAIL_SYNTAX);
        return MOD_CONT;
    }
    alog("%s: %s!%s@%s used GETEMAIL on %s", s_NickServ, u->nick,
         u->username, common_get_vhost(u), email);
    for (i = 0; i < 1024; i++) {
        for (nc = nclists[i]; nc; nc = nc->next) {
            if (nc->email) {
                if (stricmp(nc->email, email) == 0) {
                    j++;
                    notice_lang(s_NickServ, u, NICK_GETEMAIL_EMAILS_ARE,
                                nc->display, email);
                }
            }
        }
    }
    if (j <= 0) {
        notice_lang(s_NickServ, u, NICK_GETEMAIL_NOT_USED, email);
        return MOD_CONT;
    }
    return MOD_CONT;
}

/**************************************************************************/

static int do_getpass(User * u)
{
#ifndef USE_ENCRYPTION
    char *nick = strtok(NULL, " ");
    NickAlias *na;
    NickRequest *nr = NULL;
#endif

    /* Assumes that permission checking has already been done. */
#ifdef USE_ENCRYPTION
    notice_lang(s_NickServ, u, NICK_GETPASS_UNAVAILABLE);
#else
    if (!nick) {
        syntax_error(s_NickServ, u, "GETPASS", NICK_GETPASS_SYNTAX);
    } else if (!(na = findnick(nick))) {
        if ((nr = findrequestnick(nick))) {
            alog("%s: %s!%s@%s used GETPASS on %s", s_NickServ, u->nick,
                 u->username, common_get_vhost(u), nick);
            if (WallGetpass)
                anope_cmd_global(s_NickServ,
                                 "\2%s\2 used GETPASS on \2%s\2", u->nick,
                                 nick);
            notice_lang(s_NickServ, u, NICK_GETPASS_PASSCODE_IS, nick,
                        nr->passcode);
        } else {
            notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
        }
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else if (NSSecureAdmins && nick_is_services_admin(na->nc)
               && !is_services_root(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
    } else if (NSRestrictGetPass && !is_services_root(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
    } else {
        alog("%s: %s!%s@%s used GETPASS on %s", s_NickServ, u->nick,
             u->username, common_get_vhost(u), nick);
        if (WallGetpass)
            anope_cmd_global(s_NickServ, "\2%s\2 used GETPASS on \2%s\2",
                             u->nick, nick);
        notice_lang(s_NickServ, u, NICK_GETPASS_PASSWORD_IS, nick,
                    na->nc->pass);
    }
#endif
    return MOD_CONT;
}

/*************************************************************************/

static int do_sendpass(User * u)
{
#ifndef USE_ENCRYPTION
    char *nick = strtok(NULL, " ");
    NickAlias *na;
#endif

#ifdef USE_ENCRYPTION
    notice_lang(s_NickServ, u, NICK_SENDPASS_UNAVAILABLE);
#else
    if (!nick) {
        syntax_error(s_NickServ, u, "SENDPASS", NICK_SENDPASS_SYNTAX);
    } else if (RestrictMail && !is_oper(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
    } else if (!(na = findnick(nick))) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else {
        char buf[BUFSIZE];
        MailInfo *mail;

        snprintf(buf, sizeof(buf), getstring(na, NICK_SENDPASS_SUBJECT),
                 na->nick);
        mail = MailBegin(u, na->nc, buf, s_NickServ);
        if (!mail)
            return MOD_CONT;

        fprintf(mail->pipe, getstring(na, NICK_SENDPASS_HEAD));
        fprintf(mail->pipe, "\n\n");
        fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_1), na->nick);
        fprintf(mail->pipe, "\n\n");
        fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_2),
                na->nc->pass);
        fprintf(mail->pipe, "\n\n");
        fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_3));
        fprintf(mail->pipe, "\n\n");
        fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_4));
        fprintf(mail->pipe, "\n\n");
        fprintf(mail->pipe, getstring(na, NICK_SENDPASS_LINE_5),
                NetworkName);
        fprintf(mail->pipe, "\n.\n");

        MailEnd(mail);

        alog("%s: %s!%s@%s used SENDPASS on %s", s_NickServ, u->nick,
             u->username, common_get_vhost(u), nick);
        notice_lang(s_NickServ, u, NICK_SENDPASS_OK, nick);
    }
#endif
    return MOD_CONT;
}

/*************************************************************************/

static int do_forbid(User * u)
{
    NickAlias *na;
    char *nick = strtok(NULL, " ");
    char *reason = strtok(NULL, "");

    /* Assumes that permission checking has already been done. */
    if (!nick || (ForceForbidReason && !reason)) {
        syntax_error(s_NickServ, u, "FORBID",
                     (ForceForbidReason ? NICK_FORBID_SYNTAX_REASON :
                      NICK_FORBID_SYNTAX));
        return MOD_CONT;
    }

    if (readonly)
        notice_lang(s_NickServ, u, READ_ONLY_MODE);
    if ((na = findnick(nick)) != NULL) {
        if (NSSecureAdmins && nick_is_services_admin(na->nc)
            && !is_services_root(u)) {
            notice_lang(s_NickServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }
        delnick(na);
    }
    na = makenick(nick);
    if (na) {
        na->status |= NS_VERBOTEN;
        na->last_usermask = sstrdup(u->nick);
        if (reason)
            na->last_realname = sstrdup(reason);

        na->u = finduser(na->nick);
        if (na->u)
            na->u->na = na;

        if (na->u) {
            notice_lang(s_NickServ, na->u, FORCENICKCHANGE_NOW);
            collide(na, 0);
        }

        if (WallForbid)
            anope_cmd_global(s_NickServ, "\2%s\2 used FORBID on \2%s\2",
                             u->nick, nick);

        alog("%s: %s set FORBID for nick %s", s_NickServ, u->nick, nick);
        notice_lang(s_NickServ, u, NICK_FORBID_SUCCEEDED, nick);
    } else {
        alog("%s: Valid FORBID for %s by %s failed", s_NickServ, nick,
             u->nick);
        notice_lang(s_NickServ, u, NICK_FORBID_FAILED, nick);
    }
    return MOD_CONT;
}

/*************************************************************************/

int ns_do_register(User * u)
{
    return do_register(u);
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
            free(u->nickTrack);

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
        free(u->nickTrack);
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
    if ((!(na = u->na)) || (!(nick = na->nick)))
        return 0;

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
