/* HostServ functions
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

/*************************************************************************/
#include "services.h"
#include "pseudo.h"

#define HASH(nick)	((tolower((nick)[0])&31)<<5 | (tolower((nick)[1])&31))

void load_hs_dbase_v1(dbFILE * f);
void load_hs_dbase_v2(dbFILE * f);
void load_hs_dbase_v3(dbFILE * f);

HostCore *head = NULL;          /* head of the HostCore list */
HostCore *createHostCorelist(HostCore * next, char *nick, char *vIdent,
                             char *vHost, char *creator, int32 tmp_time);
HostCore *findHostCore(HostCore * head, char *nick, boolean * found);
HostCore *insertHostCore(HostCore * head, HostCore * prev, char *nick,
                         char *vIdent, char *vHost, char *creator,
                         int32 tmp_time);
HostCore *deleteHostCore(HostCore * head, HostCore * prev);
void delHostCore(char *nick);

int is_host_setter(User * u);
int is_host_remover(User * u);

static int do_help(User * u);
static int do_set(User * u);
static int do_on(User * u);
int do_on_id(User * u);
static void set_lastmask(User * u);
static int do_off(User * u);
static int do_del(User * u);
static int do_group(User * u);
static int listOut(User * u);
int do_hs_sync(NickCore * nc, char *vIdent, char *hostmask, char *creator,
               time_t time);
int do_setall(User * u);
int do_delall(User * u);
void moduleAddHostServCmds(void);
/*************************************************************************/
void moduleAddHostServCmds(void)
{
    Command *c;
    c = createCommand("HELP", do_help, NULL, -1, -1, -1, -1, -1);
    addCoreCommand(HOSTSERV, c);
    c = createCommand("SET", do_set, is_host_setter, HOST_HELP_SET, -1, -1,
                      -1, -1);
    addCoreCommand(HOSTSERV, c);
    c = createCommand("GROUP", do_group, NULL, HOST_HELP_GROUP, -1, -1, -1,
                      -1);
    addCoreCommand(HOSTSERV, c);
    c = createCommand("SETALL", do_setall, is_host_setter,
                      HOST_HELP_SETALL, -1, -1, -1, -1);
    addCoreCommand(HOSTSERV, c);
    c = createCommand("DELALL", do_delall, is_host_remover,
                      HOST_HELP_DELALL, -1, -1, -1, -1);
    addCoreCommand(HOSTSERV, c);
    c = createCommand("ON", do_on, NULL, HOST_HELP_ON, -1, -1, -1, -1);
    addCoreCommand(HOSTSERV, c);
    c = createCommand("OFF", do_off, NULL, HOST_HELP_OFF, -1, -1, -1, -1);
    addCoreCommand(HOSTSERV, c);
    c = createCommand("DEL", do_del, is_host_remover, HOST_HELP_DEL, -1,
                      -1, -1, -1);
    addCoreCommand(HOSTSERV, c);
    c = createCommand("LIST", listOut, is_services_oper, -1,
                      -1, HOST_HELP_LIST, -1, -1);
    addCoreCommand(HOSTSERV, c);
}

/*************************************************************************/

/**
 * HostServ initialization.
 * @return void
 */
void hostserv_init(void)
{
    moduleAddHostServCmds();
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
int listOut(User * u)
{
    char *key = strtok(NULL, "");
    struct tm *tm;
    char buf[BUFSIZE];
    int counter = 1;
    int from = 0, to = 0;
    char *tmp = NULL;
    char *s = NULL;
    int display_counter = 0;

    HostCore *current;

    current = head;
    if (current == NULL)
        notice_lang(s_HostServ, u, HOST_EMPTY);
    else {
                /**
		 * Do a check for a range here, then in the next loop
		 * we'll only display what has been requested..
		 **/
        if (key) {
            if (key[0] == '#') {
                tmp = myStrGetOnlyToken((key + 1), '-', 0);     /* Read FROM out */
                if (!tmp) {
                    return MOD_CONT;
                }
                for (s = tmp; *s; s++) {
                    if (!isdigit(*s)) {
                        return MOD_CONT;
                    }
                }
                from = atoi(tmp);
                tmp = myStrGetTokenRemainder(key, '-', 1);      /* Read TO out */
                if (!tmp) {
                    return MOD_CONT;
                }
                for (s = tmp; *s; s++) {
                    if (!isdigit(*s)) {
                        return MOD_CONT;
                    }
                }
                to = atoi(tmp);
                key = NULL;
            }
        }

        while (current != NULL) {
            if (key) {
                if (((match_wild_nocase(key, current->nick))
                     || (match_wild_nocase(key, current->vHost)))
                    && (display_counter < NSListMax)) {
                    display_counter++;
                    tm = localtime(&current->time);
                    strftime_lang(buf, sizeof(buf), u,
                                  STRFTIME_DATE_TIME_FORMAT, tm);
                    if (current->vIdent) {
                        notice_lang(s_HostServ, u, HOST_IDENT_ENTRY,
                                    counter, current->nick,
                                    current->vIdent, current->vHost,
                                    current->creator, buf);
                    } else {
                        notice_lang(s_HostServ, u, HOST_ENTRY, counter,
                                    current->nick, current->vHost,
                                    current->creator, buf);
                    }
                }
            } else {
                        /**
			 * List the host if its in the display range, and not more
			 * than NSListMax records have been displayed...
			 **/
                if ((((counter >= from) && (counter <= to))
                     || ((from == 0) && (to == 0)))
                    && (display_counter < NSListMax)) {
                    display_counter++;
                    tm = localtime(&current->time);
                    strftime_lang(buf, sizeof(buf), u,
                                  STRFTIME_DATE_TIME_FORMAT, tm);
                    if (current->vIdent) {
                        notice_lang(s_HostServ, u, HOST_IDENT_ENTRY,
                                    counter, current->nick,
                                    current->vIdent, current->vHost,
                                    current->creator, buf);
                    } else {
                        notice_lang(s_HostServ, u, HOST_ENTRY, counter,
                                    current->nick, current->vHost,
                                    current->creator, buf);
                    }
                }
            }
            counter++;
            current = current->next;
        }
        if (key) {
            notice_lang(s_HostServ, u, HOST_LIST_KEY_FOOTER, key,
                        display_counter);
        } else {
            if (from != 0) {
                notice_lang(s_HostServ, u, HOST_LIST_RANGE_FOOTER, from,
                            to);
            } else {
                notice_lang(s_HostServ, u, HOST_LIST_FOOTER,
                            display_counter);
            }
        }
    }
    return MOD_CONT;
}

/*************************************************************************/
void delHostCore(char *nick)
{
#ifdef USE_RDB
    static char clause[128];
#endif
    HostCore *tmp;
    boolean found = false;
    tmp = findHostCore(head, nick, &found);
    if (found) {
        head = deleteHostCore(head, tmp);

#ifdef USE_RDB
        /* Reflect this change in the database right away. */
        if (rdb_open()) {

            snprintf(clause, sizeof(clause), "nick='%s'", nick);
            rdb_scrub_table("anope_hs_core", clause);
            rdb_close();
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

    rdb_clear_table("anope_hs_core");

    current = head;
    while (current != NULL) {
        rdb_save_hs_core(current);
        current = current->next;
    }
    rdb_close();
#endif
}

/*************************************************************************/
/*	End of Load/Save Functions					 */
/*************************************************************************/
/*************************************************************************/
/*	Start of Generic Functions					 */
/*************************************************************************/
int do_setall(User * u)
{

    char *nick = strtok(NULL, " ");
    char *rawhostmask = strtok(NULL, " ");
    char *hostmask = smalloc(HOSTMAX);

    NickAlias *na;
    int32 tmp_time;
    char *s;

    char *vIdent = NULL;

    if (!nick || !rawhostmask) {
        notice_lang(s_HostServ, u, HOST_SETALL_SYNTAX, s_HostServ);
        return MOD_CONT;
    }

    vIdent = myStrGetOnlyToken(rawhostmask, '@', 0);    /* Get the first substring, @ as delimiter */
    if (vIdent) {
        rawhostmask = myStrGetTokenRemainder(rawhostmask, '@', 1);      /* get the remaining string */
        if (!rawhostmask) {
            notice_lang(s_HostServ, u, HOST_SETALL_SYNTAX, s_HostServ);
            return MOD_CONT;
        }
        if (strlen(vIdent) > USERMAX - 1) {
            notice_lang(s_HostServ, u, HOST_SET_IDENTTOOLONG, USERMAX);
            return MOD_CONT;
        } else {
            for (s = vIdent; *s; s++) {
                if (!isvalidchar(*s)) {
                    notice_lang(s_HostServ, u, HOST_SET_IDENT_ERROR);
                    return MOD_CONT;
                }
            }
        }
        if (!ircd->vident) {
            notice_lang(s_HostServ, u, HOST_NO_VIDENT);
            return MOD_CONT;
        }
    }

    if (strlen(rawhostmask) < HOSTMAX - 1)
        snprintf(hostmask, HOSTMAX - 1, "%s", rawhostmask);
    else {
        notice_lang(s_HostServ, u, HOST_SET_TOOLONG, HOSTMAX);
        return MOD_CONT;
    }

    if (!isValidHost(hostmask, 3)) {
        notice_lang(s_HostServ, u, HOST_SET_ERROR);
        free(hostmask);
        return MOD_CONT;
    }

    tmp_time = time(NULL);

    if ((na = findnick(nick))) {
        if (na->status & NS_VERBOTEN) {
            notice_lang(s_HostServ, u, NICK_X_FORBIDDEN, nick);
            free(hostmask);
            return MOD_CONT;
        }
        if (vIdent && ircd->vident) {
            alog("vHost for all nicks in group \002%s\002 set to \002%s@%s\002 by oper \002%s\002", nick, vIdent, hostmask, u->nick);
        } else {
            alog("vHost for all nicks in group \002%s\002 set to \002%s\002 by oper \002%s\002", nick, hostmask, u->nick);
        }
        do_hs_sync(na->nc, vIdent, hostmask, u->nick, tmp_time);
        if (vIdent) {
            notice_lang(s_HostServ, u, HOST_IDENT_SETALL, nick, vIdent,
                        hostmask);
        } else {
            notice_lang(s_HostServ, u, HOST_SETALL, nick, hostmask);
        }
    } else {
        notice_lang(s_HostServ, u, HOST_NOREG, nick);
    }

    free(hostmask);
    return MOD_CONT;
}

int do_delall(User * u)
{
    int i;
    char *nick = strtok(NULL, " ");
    NickAlias *na;
    NickCore *nc;
    if (!nick) {
        notice_lang(s_HostServ, u, HOST_DELALL_SYNTAX, s_HostServ);
        return MOD_CONT;
    }
    if ((na = findnick(nick))) {
        if (na->status & NS_VERBOTEN) {
            notice_lang(s_HostServ, u, NICK_X_FORBIDDEN, nick);
            return MOD_CONT;
        }
        nc = na->nc;
        for (i = 0; i < nc->aliases.count; i++) {
            na = nc->aliases.list[i];
            delHostCore(na->nick);
        }
        alog("vHosts for all nicks in group \002%s\002 deleted by oper \002%s\002", nc->display, u->nick);
        notice_lang(s_HostServ, u, HOST_DELALL, nc->display);
    } else {
        notice_lang(s_HostServ, u, HOST_NOREG, nick);
    }
    return MOD_CONT;
}

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

static int do_group(User * u)
{
    NickAlias *na;
    HostCore *tmp;
    char *vHost = NULL;
    char *vIdent = NULL;
    char *creator = NULL;
    time_t time;
    boolean found = false;

    if ((na = findnick(u->nick))) {
        if (na->status & NS_IDENTIFIED) {
            tmp = findHostCore(head, u->nick, &found);
            if (found) {
                if (tmp == NULL) {
                    tmp = head; /* incase first in list */
                } else if (tmp->next) { /* we dont want the previous entry were not inserting! */
                    tmp = tmp->next;    /* jump to the next */
                }

                vHost = sstrdup(tmp->vHost);
                if (tmp->vIdent)
                    vIdent = sstrdup(tmp->vIdent);
                creator = sstrdup(tmp->creator);
                time = tmp->time;

                do_hs_sync(na->nc, vIdent, vHost, creator, time);
                if (tmp->vIdent) {
                    notice_lang(s_HostServ, u, HOST_IDENT_GROUP,
                                na->nc->display, vIdent, vHost);
                } else {
                    notice_lang(s_HostServ, u, HOST_GROUP, na->nc->display,
                                vHost);
                }
                free(vHost);
                if (vIdent)
                    free(vIdent);
                free(creator);

            } else {
                notice_lang(s_HostServ, u, HOST_NOT_ASSIGNED);
            }
        } else {
            notice_lang(s_HostServ, u, HOST_ID);
        }
    } else {
        notice_lang(s_HostServ, u, HOST_NOT_REGED);
    }
    return MOD_CONT;
}

static int do_help(User * u)
{
    char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_HostServ, u, HOST_HELP, s_HostServ);
        if ((is_services_oper(u)) || (is_host_setter(u)))
            notice_help(s_HostServ, u, HOST_OPER_HELP);
        /* Removed as atm, there is no admin only help */
/*        if (is_services_admin(u))
            notice_help(s_HostServ, u, HOST_ADMIN_HELP);*/
        moduleDisplayHelp(6, u);
    } else {
        mod_help_cmd(s_HostServ, u, HOSTSERV, cmd);
    }
    return MOD_CONT;
}

/*************************************************************************/
int do_set(User * u)
{
    char *nick = strtok(NULL, " ");
    char *rawhostmask = strtok(NULL, " ");
    char *hostmask = smalloc(HOSTMAX);

    NickAlias *na;
    int32 tmp_time;
    char *s;

    char *vIdent = NULL;

    if (!nick || !rawhostmask) {
        notice_lang(s_HostServ, u, HOST_SET_SYNTAX, s_HostServ);
        return MOD_CONT;
    }

    vIdent = myStrGetOnlyToken(rawhostmask, '@', 0);    /* Get the first substring, @ as delimiter */
    if (vIdent) {
        rawhostmask = myStrGetTokenRemainder(rawhostmask, '@', 1);      /* get the remaining string */
        if (!rawhostmask) {
            notice_lang(s_HostServ, u, HOST_SET_SYNTAX, s_HostServ);
            return MOD_CONT;
        }
        if (strlen(vIdent) > USERMAX - 1) {
            notice_lang(s_HostServ, u, HOST_SET_IDENTTOOLONG, USERMAX);
            return MOD_CONT;
        } else {
            for (s = vIdent; *s; s++) {
                if (!isvalidchar(*s)) {
                    notice_lang(s_HostServ, u, HOST_SET_IDENT_ERROR);
                    return MOD_CONT;
                }
            }
        }
        if (!ircd->vident) {
            notice_lang(s_HostServ, u, HOST_NO_VIDENT);
            return MOD_CONT;
        }
    }
    if (strlen(rawhostmask) < HOSTMAX - 1)
        snprintf(hostmask, HOSTMAX - 1, "%s", rawhostmask);
    else {
        notice_lang(s_HostServ, u, HOST_SET_TOOLONG, HOSTMAX);
        return MOD_CONT;
    }

    if (!isValidHost(hostmask, 3)) {
        notice_lang(s_HostServ, u, HOST_SET_ERROR);
        return MOD_CONT;
    }


    tmp_time = time(NULL);

    if ((na = findnick(nick))) {
        if (na->status & NS_VERBOTEN) {
            notice_lang(s_HostServ, u, NICK_X_FORBIDDEN, nick);
            return MOD_CONT;
        }
        if (vIdent && ircd->vident) {
            alog("vHost for user \002%s\002 set to \002%s@%s\002 by oper \002%s\002", nick, vIdent, hostmask, u->nick);
        } else {
            alog("vHost for user \002%s\002 set to \002%s\002 by oper \002%s\002", nick, hostmask, u->nick);
        }
        addHostCore(nick, vIdent, hostmask, u->nick, tmp_time);
        if (vIdent) {
            notice_lang(s_HostServ, u, HOST_IDENT_SET, nick, vIdent,
                        hostmask);
        } else {
            notice_lang(s_HostServ, u, HOST_SET, nick, hostmask);
        }
    } else {
        notice_lang(s_HostServ, u, HOST_NOREG, nick);
    }
    free(hostmask);
    return MOD_CONT;
}

/*************************************************************************/
int do_on(User * u)
{
    NickAlias *na;
    char *vHost;
    char *vIdent = NULL;
    if ((na = findnick(u->nick))) {
        if (na->status & NS_IDENTIFIED) {
            vHost = getvHost(u->nick);
            vIdent = getvIdent(u->nick);
            if (vHost == NULL) {
                notice_lang(s_HostServ, u, HOST_NOT_ASSIGNED);
            } else {
                if (vIdent) {
                    notice_lang(s_HostServ, u, HOST_IDENT_ACTIVATED,
                                vIdent, vHost);
                } else {
                    notice_lang(s_HostServ, u, HOST_ACTIVATED, vHost);
                }
                anope_cmd_vhost_on(u->nick, vIdent, vHost);
                if (ircd->vhost) {
                    u->vhost = sstrdup(vHost);
                }
                if (ircd->vident) {
                    if (vIdent)
                        u->vident = sstrdup(vIdent);
                }
                set_lastmask(u);
            }
        } else {
            notice_lang(s_HostServ, u, HOST_ID);
        }
    } else {
        notice_lang(s_HostServ, u, HOST_NOT_REGED);
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
            u->vhost = sstrdup(vHost);
        }
        if (ircd->vident) {
            if (vIdent)
                u->vident = sstrdup(vIdent);
        }
        set_lastmask(u);
    }
    return MOD_CONT;
}

/*************************************************************************/
int do_del(User * u)
{
    NickAlias *na;
    char *nick = strtok(NULL, " ");
    if (nick) {
        if ((na = findnick(nick))) {
            if (na->status & NS_VERBOTEN) {
                notice_lang(s_HostServ, u, NICK_X_FORBIDDEN, nick);
                return MOD_CONT;
            }
            alog("vHost for user \002%s\002 deleted by oper \002%s\002",
                 nick, u->nick);
            delHostCore(nick);
            notice_lang(s_HostServ, u, HOST_DEL, nick);
        } else {
            notice_lang(s_HostServ, u, HOST_NOREG, nick);
        }
    } else {
        notice_lang(s_HostServ, u, HOST_DEL_SYNTAX, s_HostServ);
    }
    return MOD_CONT;
}

/*************************************************************************/
int do_off(User * u)
{
    /* put any generic code here... :) */
    anope_cmd_vhost_off(u);
    return MOD_CONT;
}

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

/*************************************************************************/
/*	End of Generic Functions					 */
/*************************************************************************/

/*************************************************************************/
/*	End of Server Functions						 */
/*************************************************************************/
