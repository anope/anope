/* HostServ functions
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

/*************************************************************************/
#include "services.h"
#include "pseudo.h"

#define HASH(nick)	((tolower((nick)[0])&31)<<5 | (tolower((nick)[1])&31))

void load_hs_dbase_v1(dbFILE * f);
void load_hs_dbase_v2(dbFILE * f);
void load_hs_dbase_v3(dbFILE * f);

HostCore *head = NULL;          /* head of the HostCore list */

E int do_hs_sync(NickCore * nc, char *vIdent, char *hostmask,
                 char *creator, time_t time);

E void moduleAddHostServCmds(void);

/*************************************************************************/

void moduleAddHostServCmds(void)
{
    modules_core_init(HostServCoreNumber, HostServCoreModules);
}

/*************************************************************************/

/**
 * Return information on memory use.
 * Assumes pointers are valid. 
 **/

void get_hostserv_stats(long *nrec, long *memuse)
{
    long count = 0, mem = 0;
    HostCore *hc;

    for (hc = head; hc; hc = hc->next) {
        count++;
        mem += sizeof(*hc);
        if (hc->nick)
            mem += strlen(hc->nick) + 1;
        if (hc->vIdent)
            mem += strlen(hc->vIdent) + 1;
        if (hc->vHost)
            mem += strlen(hc->vHost) + 1;
        if (hc->creator)
            mem += strlen(hc->creator) + 1;
    }

    *nrec = count;
    *memuse = mem;
}

/*************************************************************************/

/**
 * HostServ initialization.
 * @return void
 */
void hostserv_init(void)
{
    if (s_HostServ) {
        moduleAddHostServCmds();
    }
}

/*************************************************************************/

/**
 * Main HostServ routine.
 * @param u User Struct
 * @param buf Buffer holding the message
 * @return void
 */
void hostserv(User * u, char *buf)
{
    char *cmd, *s;

    cmd = strtok(buf, " ");

    if (!cmd) {
        return;
    } else if (stricmp(cmd, "\1PING") == 0) {
        if (!(s = strtok(NULL, ""))) {
            s = "";
        }
        anope_cmd_ctcp(s_HostServ, u->nick, "PING %s", s);
    } else if (skeleton) {
        notice_lang(s_HostServ, u, SERVICE_OFFLINE, s_HostServ);
    } else {
        if (ircd->vhost) {
            mod_run_cmd(s_HostServ, u, HOSTSERV, cmd);
        } else {
            notice_lang(s_HostServ, u, SERVICE_OFFLINE, s_HostServ);
        }
    }
}

/*************************************************************************/
/*	Start of Linked List routines					                     */
/*************************************************************************/

HostCore *hostCoreListHead()
{
    return head;
}

/**
 * Create HostCore list member
 * @param next HostCore next slot
 * @param nick Nick to add
 * @param vIdent Virtual Ident
 * @param vHost  Virtual Host
 * @param creator Person whom set the vhost
 * @param time Time the vhost was Set
 * @return HostCore
 */
HostCore *createHostCorelist(HostCore * next, char *nick, char *vIdent,
                             char *vHost, char *creator, int32 tmp_time)
{

    next = malloc(sizeof(HostCore));
    if (next == NULL) {
        anope_cmd_global(s_HostServ,
                         "Unable to allocate memory to create the vHost LL, problems i sense..");
    } else {
        next->nick = malloc(sizeof(char) * strlen(nick) + 1);
        next->vHost = malloc(sizeof(char) * strlen(vHost) + 1);
        next->creator = malloc(sizeof(char) * strlen(creator) + 1);
        if (vIdent)
            next->vIdent = malloc(sizeof(char) * strlen(vIdent) + 1);
        if ((next->nick == NULL) || (next->vHost == NULL)
            || (next->creator == NULL)) {
            anope_cmd_global(s_HostServ,
                             "Unable to allocate memory to create the vHost LL, problems i sense..");
            return NULL;
        }
        strcpy(next->nick, nick);
        strcpy(next->vHost, vHost);
        strcpy(next->creator, creator);
        if (vIdent) {
            if ((next->vIdent == NULL)) {
                anope_cmd_global(s_HostServ,
                                 "Unable to allocate memory to create the vHost LL, problems i sense..");
                return NULL;
            }
            strcpy(next->vIdent, vIdent);
        } else {
            next->vIdent = NULL;
        }
        next->time = tmp_time;
        next->next = NULL;
        return next;
    }
    return NULL;
}

/*************************************************************************/
/**
 * Returns either NULL for the head, or the location of the *PREVIOUS*
 * record, this is where we need to insert etc..
 * @param head HostCore head
 * @param nick Nick to find
 * @param found If found
 * @return HostCore
 */
HostCore *findHostCore(HostCore * head, char *nick, boolean * found)
{
    HostCore *previous, *current;

    *found = false;
    current = head;
    previous = current;

    if (!nick) {
        return NULL;
    }

    while (current != NULL) {
        if (stricmp(nick, current->nick) == 0) {
            *found = true;
            break;
        } else if (stricmp(nick, current->nick) < 0) {
            /* we know were not gonna find it now.... */
            break;
        } else {
            previous = current;
            current = current->next;
        }
    }
    if (current == head) {
        return NULL;
    } else {
        return previous;
    }
}

/*************************************************************************/
HostCore *insertHostCore(HostCore * head, HostCore * prev, char *nick,
                         char *vIdent, char *vHost, char *creator,
                         int32 tmp_time)
{

    HostCore *newCore, *tmp;

    if (!nick || !vHost || !creator) {
        return NULL;
    }

    newCore = malloc(sizeof(HostCore));
    if (newCore == NULL) {
        anope_cmd_global(s_HostServ,
                         "Unable to allocate memory to insert into the vHost LL, problems i sense..");
        return NULL;
    } else {
        newCore->nick = malloc(sizeof(char) * strlen(nick) + 1);
        newCore->vHost = malloc(sizeof(char) * strlen(vHost) + 1);
        newCore->creator = malloc(sizeof(char) * strlen(creator) + 1);
        if (vIdent)
            newCore->vIdent = malloc(sizeof(char) * strlen(vIdent) + 1);
        if ((newCore->nick == NULL) || (newCore->vHost == NULL)
            || (newCore->creator == NULL)) {
            anope_cmd_global(s_HostServ,
                             "Unable to allocate memory to create the vHost LL, problems i sense..");
            return NULL;
        }
        strcpy(newCore->nick, nick);
        strcpy(newCore->vHost, vHost);
        strcpy(newCore->creator, creator);
        if (vIdent) {
            if ((newCore->vIdent == NULL)) {
                anope_cmd_global(s_HostServ,
                                 "Unable to allocate memory to create the vHost LL, problems i sense..");
                return NULL;
            }
            strcpy(newCore->vIdent, vIdent);
        } else {
            newCore->vIdent = NULL;
        }
        newCore->time = tmp_time;
        if (prev == NULL) {
            tmp = head;
            head = newCore;
            newCore->next = tmp;
        } else {
            tmp = prev->next;
            prev->next = newCore;
            newCore->next = tmp;
        }
    }
    return head;
}

/*************************************************************************/
HostCore *deleteHostCore(HostCore * head, HostCore * prev)
{

    HostCore *tmp;

    if (prev == NULL) {
        tmp = head;
        head = head->next;
    } else {
        tmp = prev->next;
        prev->next = tmp->next;
    }
    free(tmp->vHost);
    free(tmp->nick);
    free(tmp->creator);
    if (tmp->vIdent) {
        free(tmp->vIdent);
    }
    free(tmp);
    return head;
}

/*************************************************************************/
void addHostCore(char *nick, char *vIdent, char *vhost, char *creator,
                 int32 tmp_time)
{
    HostCore *tmp;
    boolean found = false;

    if (head == NULL) {
        head =
            createHostCorelist(head, nick, vIdent, vhost, creator,
                               tmp_time);
    } else {
        tmp = findHostCore(head, nick, &found);
        if (!found) {
            head =
                insertHostCore(head, tmp, nick, vIdent, vhost, creator,
                               tmp_time);
        } else {
            head = deleteHostCore(head, tmp);   /* delete the old entry */
            addHostCore(nick, vIdent, vhost, creator, tmp_time);        /* recursive call to add new entry */
        }
    }
}

/*************************************************************************/
char *getvHost(char *nick)
{
    HostCore *tmp;
    boolean found = false;
    tmp = findHostCore(head, nick, &found);
    if (found) {
        if (tmp == NULL)
            return head->vHost;
        else
            return tmp->next->vHost;
    }
    return NULL;
}

/*************************************************************************/
char *getvIdent(char *nick)
{
    HostCore *tmp;
    boolean found = false;
    tmp = findHostCore(head, nick, &found);
    if (found) {
        if (tmp == NULL)
            return head->vIdent;
        else
            return tmp->next->vIdent;
    }
    return NULL;
}

/*************************************************************************/
void delHostCore(char *nick)
{
#ifdef USE_RDB
    static char clause[128];
    char *q_nick;
#endif
    HostCore *tmp;
    boolean found = false;
    tmp = findHostCore(head, nick, &found);
    if (found) {
        head = deleteHostCore(head, tmp);

#ifdef USE_RDB
        /* Reflect this change in the database right away. */
        if (rdb_open()) {
            q_nick = rdb_quote(nick);
            snprintf(clause, sizeof(clause), "nick='%s'", q_nick);
            if (rdb_scrub_table("anope_hs_core", clause) == 0)
                alog("Unable to scrub table 'anope_hs_core' - HostServ RDB update failed.");
            rdb_close();
            free(q_nick);
        }
#endif

    }

}

/*************************************************************************/
/*	End of Linked List routines					 */
/*************************************************************************/
/*************************************************************************/
/*	Start of Load/Save routines					 */
/*************************************************************************/
#define SAFE(x) do {					\
    if ((x) < 0) {					\
	if (!forceload)					\
	    fatal("Read error on %s", HostDBName);	\
	failed = 1;					\
	break;						\
    }							\
} while (0)

void load_hs_dbase(void)
{
    dbFILE *f;
    int ver;

    if (!(f = open_db(s_HostServ, HostDBName, "r", HOST_VERSION))) {
        return;
    }
    ver = get_file_version(f);

    if (ver == 1) {
        load_hs_dbase_v1(f);
    } else if (ver == 2) {
        load_hs_dbase_v2(f);
    } else if (ver == 3) {
        load_hs_dbase_v3(f);
    }
    close_db(f);
}

void load_hs_dbase_v1(dbFILE * f)
{
    int c;
    int failed = 0;
    int32 tmp;

    char *nick;
    char *vHost;

    tmp = time(NULL);

    while (!failed && (c = getc_db(f)) == 1) {

        if (c == 1) {
            SAFE(read_string(&nick, f));
            SAFE(read_string(&vHost, f));
            addHostCore(nick, NULL, vHost, "Unknown", tmp);     /* could get a speed increase by not searching the list */
            free(nick);         /* as we know the db is in alphabetical order... */
            free(vHost);
        } else {
            fatal("Invalid format in %s %d", HostDBName, c);
        }
    }
}

void load_hs_dbase_v2(dbFILE * f)
{
    int c;
    int failed = 0;

    char *nick;
    char *vHost;
    char *creator;
    uint32 time;

    while (!failed && (c = getc_db(f)) == 1) {

        if (c == 1) {
            SAFE(read_string(&nick, f));
            SAFE(read_string(&vHost, f));
            SAFE(read_string(&creator, f));
            SAFE(read_int32(&time, f));
            addHostCore(nick, NULL, vHost, creator, time);      /* could get a speed increase by not searching the list */
            free(nick);         /* as we know the db is in alphabetical order... */
            free(vHost);
            free(creator);
        } else {
            fatal("Invalid format in %s %d", HostDBName, c);
        }
    }
}

void load_hs_dbase_v3(dbFILE * f)
{
    int c;
    int failed = 0;

    char *nick;
    char *vHost;
    char *creator;
    char *vIdent;
    uint32 time;

    while (!failed && (c = getc_db(f)) == 1) {
        if (c == 1) {
            SAFE(read_string(&nick, f));
            SAFE(read_string(&vIdent, f));
            SAFE(read_string(&vHost, f));
            SAFE(read_string(&creator, f));
            SAFE(read_int32(&time, f));
            addHostCore(nick, vIdent, vHost, creator, time);    /* could get a speed increase by not searching the list */
            free(nick);         /* as we know the db is in alphabetical order... */
            free(vHost);
            free(creator);
            free(vIdent);
        } else {
            fatal("Invalid format in %s %d", HostDBName, c);
        }
    }
}

#undef SAFE
/*************************************************************************/
#define SAFE(x) do {						\
    if ((x) < 0) {						\
	restore_db(f);						\
	log_perror("Write error on %s", HostDBName);		\
	if (time(NULL) - lastwarn > WarningTimeout) {		\
	    anope_cmd_global(NULL, "Write error on %s: %s", HostDBName,	\
			strerror(errno));			\
	    lastwarn = time(NULL);				\
	}							\
	return;							\
    }								\
} while (0)

void save_hs_dbase(void)
{
    dbFILE *f;
    static time_t lastwarn = 0;
    HostCore *current;

    if (!(f = open_db(s_HostServ, HostDBName, "w", HOST_VERSION)))
        return;

    current = head;
    while (current != NULL) {
        SAFE(write_int8(1, f));
        SAFE(write_string(current->nick, f));
        SAFE(write_string(current->vIdent, f));
        SAFE(write_string(current->vHost, f));
        SAFE(write_string(current->creator, f));
        SAFE(write_int32(current->time, f));
        current = current->next;
    }
    SAFE(write_int8(0, f));
    close_db(f);

}

#undef SAFE

void save_hs_rdb_dbase(void)
{
#ifdef USE_RDB
    HostCore *current;

    if (!rdb_open())
        return;

    if (rdb_tag_table("anope_hs_core") == 0) {
        alog("Unable to tag table 'anope_hs_core' - HostServ RDB save failed.");
        rdb_close();
        return;
    }

    current = head;
    while (current != NULL) {
        if (rdb_save_hs_core(current) == 0) {
            alog("Unable to save HostCore for %s - HostServ RDB save failed.", current->nick);
            rdb_close();
            return;
        }
        current = current->next;
    }

    if (rdb_clean_table("anope_hs_core") == 0)
        alog("Unable to clean table 'anope_hs_core' - HostServ RDB save failed.");

    rdb_close();
#endif
}

/*************************************************************************/
/*	End of Load/Save Functions					 */
/*************************************************************************/
/*************************************************************************/
/*	Start of Generic Functions					 */
/*************************************************************************/

int do_hs_sync(NickCore * nc, char *vIdent, char *hostmask, char *creator,
               time_t time)
{
    int i;
    NickAlias *na;

    for (i = 0; i < nc->aliases.count; i++) {
        na = nc->aliases.list[i];
        addHostCore(na->nick, vIdent, hostmask, creator, time);
    }
    return MOD_CONT;
}

/*************************************************************************/
int do_on_id(User * u)
{                               /* we've assumed that the user exists etc.. */
    char *vHost;                /* as were only gonna call this from nsid routine */
    char *vIdent;
    vHost = getvHost(u->nick);
    vIdent = getvIdent(u->nick);
    if (vHost != NULL) {
        if (vIdent) {
            notice_lang(s_HostServ, u, HOST_IDENT_ACTIVATED, vIdent,
                        vHost);
        } else {
            notice_lang(s_HostServ, u, HOST_ACTIVATED, vHost);
        }
        anope_cmd_vhost_on(u->nick, vIdent, vHost);
        if (ircd->vhost) {
            if (u->vhost)
                free(u->vhost);
            u->vhost = sstrdup(vHost);
        }
        if (ircd->vident) {
            if (vIdent) {
                if (u->vident)
                    free(u->vident);
                u->vident = sstrdup(vIdent);
            }
        }
        set_lastmask(u);
    }
    return MOD_CONT;
}

/*************************************************************************/
int is_host_setter(User * u)
{
    int i, j;
    NickAlias *na;

    if (is_services_oper(u)) {
        return 1;
    }
    if (!nick_identified(u)) {
        return 0;
    }

    /* Look through all user's aliases (0000412) */
    for (i = 0; i < u->na->nc->aliases.count; i++) {
        na = u->na->nc->aliases.list[i];
        for (j = 0; j < HostNumber; j++) {
            if (stricmp(HostSetters[j], na->nick) == 0) {
                return 1;
            }
        }
    }

    return 0;
}

int is_host_remover(User * u)
{
    return is_host_setter(u);   /* only here incase we want to split them up later */
}

/*
 * Sets the last_usermak properly. Using virtual ident and/or host 
 */
void set_lastmask(User * u)
{
    if (u->na->last_usermask)
        free(u->na->last_usermask);

    u->na->last_usermask =
        smalloc(strlen(common_get_vident(u)) +
                strlen(common_get_vhost(u)) + 2);
    sprintf(u->na->last_usermask, "%s@%s", common_get_vident(u),
            common_get_vhost(u));

}
