/* Routines to maintain a list of connected servers
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

Server *servlist = NULL;
Server *me_server = NULL;       /* This are we        */
Server *serv_uplink = NULL;     /* This is our uplink */
uint32 uplink_capab;
char *uplink;
char *TS6UPLINK;
char *TS6SID;

/* For first_server / next_server */
static Server *server_cur;

CapabInfo capab_info[] = {
    {"NOQUIT", CAPAB_NOQUIT},
    {"TSMODE", CAPAB_TSMODE},
    {"UNCONNECT", CAPAB_UNCONNECT},
    {"NICKIP", CAPAB_NICKIP},
    {"SSJOIN", CAPAB_NSJOIN},
    {"ZIP", CAPAB_ZIP},
    {"BURST", CAPAB_BURST},
    {"TS5", CAPAB_TS5},
    {"TS3", CAPAB_TS3},
    {"DKEY", CAPAB_DKEY},
    {"PT4", CAPAB_PT4},
    {"SCS", CAPAB_SCS},
    {"QS", CAPAB_QS},
    {"UID", CAPAB_UID},
    {"KNOCK", CAPAB_KNOCK},
    {"CLIENT", CAPAB_CLIENT},
    {"IPV6", CAPAB_IPV6},
    {"SSJ5", CAPAB_SSJ5},
    {"SN2", CAPAB_SN2},
    {"TOK1", CAPAB_TOKEN},
    {"TOKEN", CAPAB_TOKEN},
    {"VHOST", CAPAB_VHOST},
    {"SSJ3", CAPAB_SSJ3},
    {"SJB64", CAPAB_SJB64},
    {"CHANMODES", CAPAB_CHANMODE},
    {"NICKCHARS", CAPAB_NICKCHARS},
    {NULL, 0}
};

/*************************************************************************/

/**
 * Return the first server in the server struct
 * @param flags Server Flags, see services.h
 * @return Server Struct
 */
Server *first_server(int flags)
{
    server_cur = servlist;
    if (flags > -1) {
        while (server_cur && (server_cur->flags != flags))
            server_cur = next_server(flags);
    }
    return server_cur;
}

/*************************************************************************/

/**
 * Return the next server in the server struct
 * @param flags Server Flags, see services.h
 * @return Server Struct
 */
Server *next_server(int flags)
{
    if (!server_cur)
        return NULL;

    do {
        if (server_cur->links) {
            server_cur = server_cur->links;
        } else if (server_cur->next) {
            server_cur = server_cur->next;
        } else {
            do {
                server_cur = server_cur->uplink;
                if (server_cur && server_cur->next) {
                    server_cur = server_cur->next;
                    break;
                }
            } while (server_cur);
        }
    } while (server_cur && ((flags > -1) && (server_cur->flags != flags)));

    return server_cur;
}

/*************************************************************************/

/**
 * This function makes a new Server structure and links it in the right
 * places in the linked list if a Server struct to it's uplink if provided.
 * It can also be NULL to indicate it's the uplink and should be first in
 * the server list.
 * @param uplink Server struct
 * @param name Server Name
 * @param desc Server Description
 * @param flags Server Flags, see services.h
 * @param suid Server Universal ID
 * @return Server Struct
 */
Server *new_server(Server * uplink, const char *name, const char *desc,
                   uint16 flags, char *suid)
{
    Server *serv;

    if (debug) 
        alog("debug: Creating %s(%s) uplinked to %s", name, suid ? suid : "", 
                uplink ? uplink->name : "No uplink");

    serv = scalloc(sizeof(Server), 1);
    if (!name)
        name = "";
    serv->name = sstrdup(name);
    serv->desc = sstrdup(desc);
    serv->flags = flags;
    serv->uplink = uplink;
    if (suid) {
        serv->suid = sstrdup(suid);
    } else {
        serv->suid = NULL;
    }
    if (ircd->sync)
        serv->sync = SSYNC_IN_PROGRESS;
    else
        serv->sync = SSYNC_UNKNOWN;
    serv->links = NULL;
    serv->prev = NULL;

    if (!uplink) {
        serv->hops = 0;
        serv->next = servlist;
        if (servlist)
            servlist->prev = serv;
        servlist = serv;
    } else {
        serv->hops = uplink->hops + 1;
        serv->next = uplink->links;
        if (uplink->links)
            uplink->links->prev = serv;
        uplink->links = serv;
    }
    /* Check if this is our uplink server */
    if ((uplink == me_server) && !(flags & SERVER_JUPED)) {
        serv_uplink = serv;
        serv->flags |= SERVER_ISUPLINK;
    }

    /* Write the StartGlobal (to non-juped servers) */
    if (GlobalOnCycle && GlobalOnCycleUP && !(flags & SERVER_JUPED))
        notice_server(s_GlobalNoticer, serv, "%s", GlobalOnCycleUP);

    return serv;
}

/*************************************************************************/

/**
 * Remove and free a Server structure. This function is the most complete
 * remove treatment a server can get, as it first quits all clients which
 * still pretend to be on this server, then it walks through all connected
 * servers and disconnects them too. If all mess is cleared, the server
 * itself will be too.
 * @param Server struct
 * @param reason the server quit
 * @return void
 */
static void delete_server(Server * serv, const char *quitreason)
{
    Server *s, *snext;
    User *u, *unext;
    NickAlias *na;

    if (!serv) {
        if (debug)
            alog("debug: delete_server() called with NULL arg!");
        return;
    }

    if (debug)
        alog("debug: Deleting %s(%s) uplinked to %s(%s)", serv->name, 
                serv->suid ? serv->suid : "", serv->uplink ? serv->uplink->name : "???",
                serv->uplink ? serv->uplink->suid : "");

    if (ircdcap->noquit || ircdcap->qs) {
        if ((uplink_capab & ircdcap->noquit)
            || (uplink_capab & ircdcap->qs)) {
            u = firstuser();
            while (u) {
                unext = nextuser();
                if (u->server == serv) {
                    if ((na = u->na) && !(na->status & NS_VERBOTEN)
                        && (!(na->nc->flags & NI_SUSPENDED))
                        && (na->status & (NS_IDENTIFIED | NS_RECOGNIZED))) {
                        na->last_seen = time(NULL);
                        if (na->last_quit)
                            free(na->last_quit);
                        na->last_quit =
                            (quitreason ? sstrdup(quitreason) : NULL);
                    }
                    if (LimitSessions && !is_ulined(u->server->name)) {
                        del_session(u->host);
                    }
                    delete_user(u);
                }
                u = unext;
            }
            if (debug)
                alog("debug: delete_server() cleared all users");
        }
    }

    s = serv->links;
    while (s) {
        snext = s->next;
        delete_server(s, quitreason);
        s = snext;
    }

    if (debug)
        alog("debug: delete_server() cleared all servers");

    free(serv->name);
    free(serv->desc);
    if (serv->prev)
        serv->prev->next = serv->next;
    if (serv->next)
        serv->next->prev = serv->prev;
    if (serv->uplink->links == serv)
        serv->uplink->links = serv->next;

    if (debug)
        alog("debug: delete_server() completed");
}

/*************************************************************************/

/**
 * Find a server by name, returns NULL if not found
 * @param s Server struct
 * @param name Server Name
 * @return Server struct
 */
Server *findserver(Server * s, const char *name)
{
    Server *sl;

    if (!name || !*name) {
        return NULL;
    }

    if (debug >= 3) {
        alog("debug: findserver(%p)", name);
    }
    while (s && (stricmp(s->name, name) != 0)) {
        if (s->links) {
            sl = findserver(s->links, name);
            if (sl) {
                s = sl;
            } else {
                s = s->next;
            }
        } else {
            s = s->next;
        }
    }
    if (debug >= 3) {
        alog("debug: findserver(%s) -> %p", name, (void *) s);
    }
    return s;
}

/*************************************************************************/

/**
 * Find a server by UID, returns NULL if not found
 * @param s Server struct
 * @param name Server Name
 * @return Server struct
 */
Server *findserver_uid(Server * s, const char *name)
{
    Server *sl;

    if (!name || !*name) {
        return NULL;
    }

    if (debug >= 3) {
        alog("debug: findserver_uid(%p)", name);
    }
    while (s && s->suid && (stricmp(s->suid, name) != 0)) {
        if (s->links) {
            sl = findserver_uid(s->links, name);
            if (sl) {
                s = sl;
            } else {
                s = s->next;
            }
        } else {
            s = s->next;
        }
    }
    if (debug >= 3) {
        alog("debug: findserver_uid(%s) -> %p", name, (void *) s);
    }
    return s;
}

/*************************************************************************/

/**
 * Find if the server is synced with the network
 * @param s Server struct
 * @param name Server Name
 * @return Not Synced returns -1, Synced returns 1, Error returns 0
 */
int anope_check_sync(const char *name)
{
    Server *s;
    s = findserver(servlist, name);

    if (!s)
        return 0;

    if (is_sync(s))
        return 1;
    else
        return -1;
}

/*************************************************************************/

/**
 * Handle adding the server to the Server struct
 * @param source Name of the uplink if any
 * @param servername Name of the server being linked
 * @param hops Number of hops to reach this server
 * @param descript Description of the server
 * @param numeric Server Numberic/SUID
 * @return void
 */
void do_server(const char *source, char *servername, char *hops,
               char *descript, char *numeric)
{
    Server *s;

    if (debug) {
        if (!*source) {
            alog("debug: Server introduced (%s)", servername);
        } else {
            alog("debug: Server introduced (%s) from %s", servername,
                 source);
        }
    }
    if (source[0] == '\0')
        s = me_server;
    else if (UseTS6 && ircd->ts6) {
        s = findserver_uid(servlist, source);
        if (!s)
            s = findserver(servlist, source);
    } else
        s = findserver(servlist, source);

    if (!s)
        fatal("FATAL ERROR: Received new server from nonexistant uplink.");

    new_server(s, servername, descript, 0, numeric);
    send_event(EVENT_SERVER_CONNECT, 1, servername);
}

/*************************************************************************/

/**
 * Handle removing the server from the Server struct
 * @param source Name of the server leaving
 * @param ac Number of arguments in av
 * @param av Agruments as part of the SQUIT
 * @return void
 */
void do_squit(const char *source, int ac, char **av)
{
    char buf[BUFSIZE];
    Server *s;

    if (UseTS6 && ircd->ts6) {
        s = findserver_uid(servlist, av[0]);
        if (!s) {
            s = findserver(servlist, av[0]);
        }
    } else {
        s = findserver(servlist, av[0]);
    }
    if (!s) {
        alog("SQUIT for nonexistent server (%s)!!", av[0]);
        return;
    }
    send_event(EVENT_SERVER_SQUIT, 1, s->name);

    /* If this is a juped server, send a nice global to inform the online
     * opers that we received it.
     */
    if (s->flags & SERVER_JUPED) {
        snprintf(buf, BUFSIZE, "Received SQUIT for juped server %s",
                 s->name);
        anope_cmd_global(s_OperServ, buf);
    }

    snprintf(buf, sizeof(buf), "%s %s", s->name,
             (s->uplink ? s->uplink->name : ""));

    if (ircdcap->unconnect) {
        if ((s->uplink == me_server)
            && (uplink_capab & ircdcap->unconnect)) {
            if (debug) {
                alog("debug: Sending UNCONNECT SQUIT for %s", s->name);
            }
            /* need to fix */
            anope_cmd_squit(s->name, buf);
        }
    }

    delete_server(s, buf);
}

/*************************************************************************/

/**
 * Handle parsing the CAPAB/PROTOCTL messages
 * @param ac Number of arguments in av
 * @param av Agruments 
 * @return void
 */
void capab_parse(int ac, char **av)
{
    int i;
    int j;
    char *s, *tmp;

    char *temp;

    for (i = 0; i < ac; i++) {
        temp = av[i];

        s = myStrGetToken(temp, '=', 0);
        tmp = myStrGetTokenRemainder(temp, '=', 1);

        if (!s)
            continue;

        for (j = 0; capab_info[j].token; j++) {
            if (stricmp(s, capab_info[j].token) == 0)
                uplink_capab |= capab_info[j].flag;
            /* Special cases */
            if ((stricmp(s, "NICKIP") == 0) && !ircd->nickip)
                ircd->nickip = 1;
            if ((stricmp(s, "CHANMODES") == 0) && tmp)
                ircd->chanmodes = sstrdup(tmp);
            if ((stricmp(s, "NICKCHARS") == 0) && tmp)
                ircd->nickchars = sstrdup(tmp);
        }

        if (s)
            free(s);
        if (tmp)
            free(tmp);
    }
}

/*************************************************************************/

/**
 * Search the uline servers array to find out if the server that just set the
 * mode is in our uline list
 * @param server Server Setting the mode
 * @return int 0 if not found, 1 if found
 */
int is_ulined(char *server)
{
    int j;

    for (j = 0; j < NumUlines; j++) {
        if (stricmp(Ulines[j], server) == 0) {
            return 1;
        }
    }

    return 0;
}

/*************************************************************************/

/**
 * See if the current server is synced, or has an unknown sync state
 * (in which case we pretend it is always synced)
 * @param server Server of which we want to know the state
 * @return int 0 if not synced, 1 if synced
 */
int is_sync(Server * server)
{
    if ((server->sync == SSYNC_DONE) || (server->sync == SSYNC_UNKNOWN))
        return 1;
    return 0;
}

/*************************************************************************/

/* Finish the syncing process for this server and (optionally) for all
 * it's leaves as well
 * @param serv Server to finish syncing
 * @param sync_links Should all leaves be synced as well? (1: yes, 0: no)
 * @return void
 */
void finish_sync(Server * serv, int sync_links)
{
    Server *s;

    if (!serv || is_sync(serv))
        return;

    /* Mark each server as in sync */
    s = serv;
    do {
        if (!is_sync(s)) {
            if (debug)
                alog("debug: Finishing sync for server %s", s->name);

            s->sync = SSYNC_DONE;
        }

        if (!sync_links)
            break;

        if (s->links) {
            s = s->links;
        } else if (s->next) {
            s = s->next;
        } else {
            do {
                s = s->uplink;
                if (s == serv)
                    s = NULL;
                if (s == me_server)
                    s = NULL;
            } while (s && !(s->next));
            if (s)
                s = s->next;
        }
    } while (s);

    /* Do some general stuff which should only be done once */
    restore_unsynced_topics();
    alog("Server %s is done syncing", serv->name);
}

/*******************************************************************/

/* TS6 UID generator common code.
 *
 * Derived from atheme-services, uid.c (hg 2954:116d46894b4c).
 *         -nenolod
 */
static int ts6_uid_initted = 0;
static char ts6_new_uid[10];    /* allow for \0 */
static unsigned int ts6_uid_index = 9;  /* last slot in uid buf */

void ts6_uid_init(void)
{
    /* check just in case... you can never be too safe. */
    if (TS6SID != NULL) {
        snprintf(ts6_new_uid, 10, "%sAAAAAA", TS6SID);
        ts6_uid_initted = 1;
    } else {
        alog("warning: no TS6SID specified, disabling TS6 support.");
        UseTS6 = 0;

        return;
    }
}

void ts6_uid_increment(unsigned int slot)
{
    if (slot != strlen(TS6SID)) {
        if (ts6_new_uid[slot] == 'Z')
            ts6_new_uid[slot] = '0';
        else if (ts6_new_uid[slot] == '9') {
            ts6_new_uid[slot] = 'A';
            ts6_uid_increment(slot - 1);
        } else
            ts6_new_uid[slot]++;
    } else {
        if (ts6_new_uid[slot] == 'Z')
            for (slot = 3; slot < 9; slot++)
                ts6_new_uid[slot] = 'A';
        else
            ts6_new_uid[slot]++;
    }
}

char *ts6_uid_retrieve(void)
{
    if (ts6_uid_initted != 1)
        ts6_uid_init();

    ts6_uid_increment(ts6_uid_index - 1);

    return ts6_new_uid;
}

/* EOF */
