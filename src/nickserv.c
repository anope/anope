
/* NickServ functions.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
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

static void add_ns_timeout(NickAlias * na, int type, time_t delay);

/*************************************************************************/
/* *INDENT-OFF* */
void moduleAddNickServCmds(void) {
    modules_core_init(NickServCoreNumber, NickServCoreModules);
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
            end += snprintf(end, sizeof(buf) - (end - buf), "%sSecurity",
                            need_comma ? commastr : "");
            need_comma = 1;
        }
        if (na->nc->flags & NI_PRIVATE) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%sPrivate",
                            need_comma ? commastr : "");
            need_comma = 1;
        }
        if (na->status & NS_NO_EXPIRE) {
            end += snprintf(end, sizeof(buf) - (end - buf), "%sNo Expire",
                            need_comma ? commastr : "");
            need_comma = 1;
        }
        printf("          Options: %s\n", *buf ? buf : "None");

        if (na->nc->flags & NI_SUSPENDED) {
            if (na->last_quit) {
                printf
                    ("This nickname is currently suspended, reason: %s\n",
                     na->last_quit);
            } else {
                printf("This nickname is currently suspended.\n");
            }
        }


    } else {

        for (i = 0; i < 1024; i++) {
            for (na = nalists[i]; na; na = na->next) {
                printf("    %s %-20s  %s\n",
                       na->status & NS_NO_EXPIRE ? "!" : " ",
                       na->nick, na->status & NS_VERBOTEN ?
                       "Disallowed (FORBID)" : (na->nc->
                                                flags & NI_SUSPENDED ?
                                                "Disallowed (SUSPENDED)" :
                                                na->last_usermask));
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
        if (!(s = strtok(NULL, ""))) {
            s = "";
        }
        anope_cmd_ctcp(s_NickServ, u->nick, "PING %s", s);
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

    uint16 tmp16;
    uint32 tmp32;

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
                    memcpy(nc->pass, bufp, PASSMAX);
                else
                    memset(nc->pass, 0, PASSMAX);       /* Which may be the case for forbidden nicks .. */

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

                SAFE(read_int16(&tmp16, f));
                nc->memos.memocount = (int16) tmp16;
                SAFE(read_int16(&tmp16, f));
                nc->memos.memomax = (int16) tmp16;
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
            nr = scalloc(1, sizeof(NickRequest));
            SAFE(read_string(&nr->nick, f));
            SAFE(read_string(&nr->passcode, f));
            if (ver < 2) {
                SAFE(read_string(&pass, f));
                len = strlen(pass);
                enc_encrypt(pass, len, nr->password, PASSMAX);
                memset(pass, 0, len);
                free(pass);
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

void load_ns_dbase(void)
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
            if (ver < 14) {
                SAFE(read_string(&pass, f));
                if (pass) {
                    memset(nc->pass, 0, PASSMAX);
                    memcpy(nc->pass, pass, strlen(pass));
                } else
                    memset(nc->pass, 0, PASSMAX);
            } else
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
                access = scalloc(sizeof(char *) * nc->accesscount, 1);
                nc->access = access;
                for (j = 0; j < nc->accesscount; j++, access++)
                    SAFE(read_string(access, f));
            }

            SAFE(read_int16(&tmp16, f));
            nc->memos.memocount = (int16) tmp16;
            SAFE(read_int16(&tmp16, f));
            nc->memos.memomax = (int16) tmp16;
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
            SAFE(write_buffer(nr->password, f));
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

    if (rdb_tag_table("anope_ns_core") == 0) {
        alog("Unable to tag 'anope_ns_core' - NickServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_tag_table("anope_ns_alias") == 0) {
        alog("Unable to tag 'anope_ns_alias' - NickServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_tag_table("anope_ns_access") == 0) {
        alog("Unable to tag 'anope_ns_access' - NickServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_tag_table_where("anope_ms_info", "serv='NICK'") == 0) {
        alog("Unable to tag 'anope_ms_info' - NickServ RDB save failed.");
        rdb_close();
        return;
    }

    for (i = 0; i < 1024; i++) {
        for (nc = nclists[i]; nc; nc = nc->next) {
            if (rdb_save_ns_core(nc) == 0) {
                alog("Unable to save NickCore for '%s' - NickServ RDB save failed.", nc->display);
                rdb_close();
                return;
            }
        }                       /* for (nc) */
    }                           /* for (i) */

    for (i = 0; i < 1024; i++) {
        for (na = nalists[i]; na; na = na->next) {
            if (rdb_save_ns_alias(na) == 0) {
                alog("Unable to save NickAlias for '%s' - NickServ RDB save failed.", na->nick);
                rdb_close();
                return;
            }
        }                       /* for (na) */
    }                           /* for (i) */

    if (rdb_clean_table("anope_ns_core") == 0) {
        alog("Unable to clean table 'anope_ns_core' - NickServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_clean_table("anope_ns_alias") == 0) {
        alog("Unable to clean table 'anope_ns_alias' - NickServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_clean_table("anope_ns_access") == 0) {
        alog("Unable to clean table 'anope_ns_access' - NickServ RDB save failed.");
        rdb_close();
        return;
    }
    if (rdb_clean_table_where("anope_ms_info", "serv='NICK'") == 0)
        alog("Unable to clean table 'anope_ms_info' - NickServ RDB save failed.");

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

    if (rdb_tag_table("anope_ns_request") == 0) {
        alog("Unable to tag table 'anope_ns_request' - NickServ Request RDB save failed.");
        rdb_close();
        return;
    }

    for (i = 0; i < 1024; i++) {
        for (nr = nrlists[i]; nr; nr = nr->next) {
            if (rdb_save_ns_req(nr) == 0) {
                /* Something went wrong - abort saving */
                alog("Unable to save NickRequest (nick '%s') - NickServ Request RDB save failed.", nr->nick);
                rdb_close();
                return;
            }
        }
    }

    if (rdb_clean_table("anope_ns_request") == 0)
        alog("Unable to clean table 'anope_ns_request' - NickServ Request RDB save failed.");

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

    if (na->nc->flags & NI_SUSPENDED) {
        notice_lang(s_NickServ, u, NICK_X_SUSPENDED, u->nick);
        collide(na, 0);
        return 0;
    }

    if (na->status & NS_IDENTIFIED)
        return 1;

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
                free(tmpnick);
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
    char *buf, *buf2 = NULL, *buf3 = NULL;

    if (nc->accesscount == 0)
        return 0;

    buf = scalloc(strlen(u->username) + strlen(u->host) + 2, 1);
    sprintf(buf, "%s@%s", u->username, u->host);
    if (ircd->vhost) {
        if (u->vhost) {
            buf2 = scalloc(strlen(u->username) + strlen(u->vhost) + 2, 1);
            sprintf(buf2, "%s@%s", u->username, u->vhost);
        }
        if (u->chost)
        {
            buf3 = scalloc(strlen(u->username) + strlen(u->chost) + 2, 1);
            sprintf(buf3, "%s@%s", u->username, u->chost);
        }
    }

    for (i = 0; i < nc->accesscount; i++) {
          if (match_wild_nocase(nc->access[i], buf) || (buf2 && match_wild_nocase(nc->access[i], buf2)) || (buf3 && match_wild_nocase(nc->access[i], buf3)))
          {
            free(buf);
            if (ircd->vhost) {
                if (u->vhost) {
                    free(buf2);
                }
                if (u->chost)
                  free(buf3);
            }
            return 1;
        }
    }
    free(buf);
    if (buf2)
    	free(buf2);
    if (buf3)
        free(buf3);
    
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
    int index;
    if (!nr) {
        if (debug) {
            alog("debug: insert_requestnick called with NULL values");
        }
        return;
    }

    index = HASH(nr->nick);

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
        if (rdb_ns_set_display(newdisplay, nc->display) == 0) {
            alog("Unable to update display for %s - Nick Display RDB update failed.", nc->display);
        }
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
    char *q_display;
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
        q_display = rdb_quote(nc->display);
        snprintf(clause, sizeof(clause), "display='%s'", q_display);
        if (rdb_scrub_table("anope_ns_access", clause) == 0)
            alog("Unable to scrub table 'anope_ns_access' - RDB update failed.");
        else if (rdb_scrub_table("anope_ns_core", clause) == 0)
            alog("Unable to scrub table 'anope_ns_core' - RDB update failed.");
        else if (rdb_scrub_table("anope_cs_access", clause) == 0)
            alog("Unable to scrub table 'anope_cs_access' - RDB update failed.");
        else {
            /* I'm unsure how to clean up the OS ADMIN/OPER list on the db */
            /* I wish the "display" primary key would be the same on all tables */
            snprintf(clause, sizeof(clause),
                     "receiver='%s' AND serv='NICK'", q_display);
            if (rdb_scrub_table("anope_ms_info", clause) == 0)
                alog("Unable to scrub table 'anope_ms_info' - RDB update failed.");
        }
        rdb_close();
        free(q_display);
    }
#endif

    /* Now we can safely free it. */
    free(nc->display);

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
                free(nc->memos.memos[i].text);
            moduleCleanStruct(&nc->memos.memos[i].moduleData);
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
        if (nr->next)
	    nr->next->prev = nr->prev;
        if (nr->prev)
            nr->prev->next = nr->next;
        else
            nrlists[HASH(nr->nick)] = nr->next;

        if (nr->nick)
            free(nr->nick);
        if (nr->passcode)
            free(nr->passcode);
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
    char *q_nick;
#endif
    /* First thing to do: remove any timeout belonging to the nick we're deleting */
    clean_ns_timeouts(na);

    /* Second thing to do: look for an user using the alias
     * being deleted, and make appropriate changes */

    if (na->u) {
        na->u->na = NULL;

        if (ircd->modeonunreg)
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
        q_nick = rdb_quote(na->nick);
        snprintf(clause, sizeof(clause), "nick='%s'", q_nick);
        if (rdb_scrub_table("anope_ns_alias", clause) == 0)
            alog("Unable to scrub table 'anope_ns_alias' - RDB update failed");
        rdb_close();
        free(q_nick);
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

void collide(NickAlias * na, int from_timeout)
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
        /* We need to make sure the guestnick is free -- heinz */
        do {
            snprintf(guestnick, sizeof(guestnick), "%s%d",
                     NSGuestNickPrefix, getrandom16());
        } while (finduser(guestnick));
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

void release(NickAlias * na, int from_timeout)
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
            free(t);
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
