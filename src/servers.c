/* Routines to maintain a list of connected servers
 *
 * (C) 2004 Anope Team / GeniusDex
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

Server *servlist = NULL;
Server *me_server = NULL;
uint32 uplink_capab;
char *uplink;
char *TS6UPLINK;
char *TS6SID;

/* For first_server / next_server */
static Server *server_cur;

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
    } while (server_cur && ((flags > -1) || (server_cur->flags != flags)));

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

    serv = scalloc(sizeof(Server), 1);
    if (!name)
        name = "";
    serv->name = sstrdup(name);
    serv->desc = sstrdup(desc);
    serv->flags = flags;
    serv->uplink = uplink;
    serv->suid = sstrdup(suid);
    serv->sync = -1;
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
        if (debug) {
            alog("debug: delete_server() called with NULL arg!");
        }
        return;
    }

    if (debug)
        alog("debug: delete_server() called for %s", serv->name);

    if (ircdcap->noquit || ircdcap->qs) {
        if ((uplink_capab & ircdcap->noquit)
            || (uplink_capab & ircdcap->qs)) {
            u = firstuser();
            while (u) {
                unext = nextuser();
                if (u->server == serv) {
                    if ((na = u->na) && !(na->status & NS_VERBOTEN)
                        && (na->status & (NS_IDENTIFIED | NS_RECOGNIZED))) {
                        na->last_seen = time(NULL);
                        if (na->last_quit)
                            free(na->last_quit);
                        na->last_quit =
                            (quitreason ? sstrdup(quitreason) : NULL);
                    }
                    if (LimitSessions) {
                        del_session(u->host);
                    }
                    delete_user(u);
                }
                u = unext;
            }
            if (debug >= 2)
                alog("debug: delete_server() cleared all users");
        }
    }

    s = serv->links;
    while (s) {
        snext = s->next;
        delete_server(s, quitreason);
        s = snext;
    }

    if (debug >= 2)
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

    if (!s) {
        return 0;
    }
    if (s->sync) {
        return s->sync;
    } else {
        return 0;
    }
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
    else
        s = findserver(servlist, source);

    new_server(s, servername, descript, 0, numeric);
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
    char *s, *tmp;

    char *temp;

    for (i = 0; i < ac; i++) {
        temp = av[i];

        s = myStrGetToken(temp, '=', 0);
        tmp = myStrGetTokenRemainder(temp, '=', 1);

        if (!s) {
            continue;
        }

        if (!stricmp(s, "NOQUIT")) {
            uplink_capab |= CAPAB_NOQUIT;
        }
        if (!stricmp(s, "TSMODE")) {
            uplink_capab |= CAPAB_TSMODE;
        }
        if (!stricmp(av[i], "UNCONNECT")) {
            uplink_capab |= CAPAB_UNCONNECT;
        }
        if (!stricmp(s, "NICKIP")) {
            uplink_capab |= CAPAB_NICKIP;
            /* Update the struct so that we know we can get NICKIP */
            if (!ircd->nickip) {
                ircd->nickip = 1;
            }
        }
        if (!stricmp(s, "SSJOIN")) {
            uplink_capab |= CAPAB_NSJOIN;
        }
        if (!stricmp(s, "ZIP")) {
            uplink_capab |= CAPAB_ZIP;
        }
        if (!stricmp(s, "BURST")) {
            uplink_capab |= CAPAB_BURST;
        }
        if (!stricmp(s, "TS5")) {
            uplink_capab |= CAPAB_TS5;
        }
        if (!stricmp(s, "TS3")) {
            uplink_capab |= CAPAB_TS3;
        }
        if (!stricmp(s, "DKEY")) {
            uplink_capab |= CAPAB_DKEY;
        }
        if (!stricmp(s, "PT4")) {
            uplink_capab |= CAPAB_PT4;
        }
        if (!stricmp(s, "SCS")) {
            uplink_capab |= CAPAB_SCS;
        }
        if (!stricmp(s, "QS")) {
            uplink_capab |= CAPAB_QS;
        }
        if (!stricmp(s, "UID")) {
            uplink_capab |= CAPAB_UID;
        }
        if (!stricmp(s, "KNOCK")) {
            uplink_capab |= CAPAB_KNOCK;
        }
        if (!stricmp(s, "CLIENT")) {
            uplink_capab |= CAPAB_CLIENT;
        }
        if (!stricmp(s, "IPV6")) {
            uplink_capab |= CAPAB_IPV6;
        }
        if (!stricmp(s, "SSJ5")) {
            uplink_capab |= CAPAB_SSJ5;
        }
        if (!stricmp(s, "SN2")) {
            uplink_capab |= CAPAB_SN2;
        }
        if (!stricmp(s, "TOK1")) {
            uplink_capab |= CAPAB_TOKEN;
        }
        if (!stricmp(s, "TOKEN")) {
            uplink_capab |= CAPAB_TOKEN;
        }
        if (!stricmp(s, "VHOST")) {
            uplink_capab |= CAPAB_VHOST;
        }
        if (!stricmp(s, "SSJ3")) {
            uplink_capab |= CAPAB_SSJ3;
        }
        if (!stricmp(s, "SJB64")) {
            uplink_capab |= CAPAB_SJB64;
        }
        if (!stricmp(s, "CHANMODES")) {
            uplink_capab |= CAPAB_CHANMODE;
            if (tmp) {
                ircd->chanmodes = sstrdup(tmp);
            }
        }
        if (s) {
            free(s);
        }
        if (tmp) {
            free(tmp);
        }

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

/* EOF */
