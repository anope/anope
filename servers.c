/* Routines to maintain a list of connected servers
 *
 * (C) 2004 Anope Team / GeniusDex
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
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
#ifdef IRC_BAHAMUT
uint16 uplink_capab;
#endif

/* For first_server / next_server */
static Server *server_cur;

/*************************************************************************/

/* Walk through the servers list */
Server *first_server(int flags)
{
    server_cur = servlist;
    if (flags > -1) {
        while (server_cur && (server_cur->flags != flags))
            server_cur = next_server(flags);
    }
    return server_cur;
}

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

/* This function makes a new Server structure and links it in the right
 * places in the linked list if a Server struct to it's uplink if provided.
 * It can also be NULL to indicate it's the uplink and should be first in
 * the server list.
 */

Server *new_server(Server * uplink, const char *name, const char *desc,
                   uint16 flags)
{
    Server *serv;

    serv = scalloc(sizeof(Server), 1);
    if (!name)
        name = "";
    serv->name = sstrdup(name);
    serv->desc = sstrdup(desc);
    serv->flags = flags;
    serv->uplink = uplink;
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

/* Remove and free a Server structure. This function is the most complete
 * remove treatment a server can get, as it first quits all clients which
 * still pretend to be on this server, then it walks through all connected
 * servers and disconnects them too. If all mess is cleared, the server
 * itself will be too.
 */

static void delete_server(Server * serv, const char *quitreason)
{
    Server *s, *snext;
#ifdef IRC_BAHAMUT
    User *u, *unext;
    NickAlias *na;
#endif

    if (!serv) {
        alog("delete_server() called with NULL arg!");
        return;
    }

    if (debug)
        alog("delete_server() called for %s", serv->name);

#ifdef IRC_BAHAMUT
    if (uplink_capab & CAPAB_NOQUIT) {
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
#ifndef STREAMLINED
                if (LimitSessions)
                    del_session(u->host);
#endif
                delete_user(u);
            }
            u = unext;
        }
        if (debug >= 2)
            alog("delete_server() cleared all users");
    }
#endif

    s = serv->links;
    while (s) {
        snext = s->next;
        delete_server(s, quitreason);
        s = snext;
    }

    if (debug >= 2)
        alog("delete_server() cleared all servers");

    free(serv->name);
    free(serv->desc);
    if (serv->prev)
        serv->prev->next = serv->next;
    if (serv->next)
        serv->next->prev = serv->prev;
    if (serv->uplink->links == serv)
        serv->uplink->links = serv->next;

    if (debug)
        alog("delete_server() completed");
}

/*************************************************************************/

/* Find a server by name, returns NULL if not found */

Server *findserver(Server * s, const char *name)
{
    Server *sl;

    if (debug >= 3)
        alog("debug: findserver(%p)", name);
    while (s && (stricmp(s->name, name) != 0)) {
        if (s->links) {
            sl = findserver(s->links, name);
            if (sl)
                s = sl;
            else
                s = s->next;
        } else {
            s = s->next;
        }
    }
    if (debug >= 3)
        alog("debug: findserver(%s) -> %p", name, s);
    return s;
}

/*************************************************************************/
/* :<introducing server> SERVER <servername> <hops> :<description>
 */
void do_server(const char *source, int ac, char **av)
{
    Server *s;

    if (debug)
        alog("debug: Server introduced (%s) from %s", av[0], source);

    if (source[0] == '\0')
        s = me_server;
    else
        s = findserver(servlist, source);
#ifdef IRC_PTLINK
    if (ac < 4)
        alog("Malformed SERVER received (less than 4 params)");
    else
        new_server(s, av[0], av[3], 0);
#else
    if (ac < 3)
        alog("Malformed SERVER received (less than 3 params)");
    else
        new_server(s, av[0], av[2], 0);
#endif
}

/*************************************************************************/
/* SQUIT <server> :<comment>
 */
void do_squit(const char *source, int ac, char **av)
{
    char buf[BUFSIZE];
    Server *s;

    s = findserver(servlist, av[0]);
    if (!s) {
        alog("SQUIT for nonexistent server (%s)!!", av[0]);
        return;
    }

    snprintf(buf, sizeof(buf), "%s %s", s->name,
             (s->uplink ? s->uplink->name : ""));

#ifdef IRC_BAHAMUT
    if ((s->uplink == me_server) && (uplink_capab & CAPAB_UNCONNECT)) {
        if (debug)
            alog("debuf: Sending UNCONNECT SQUIT for %s", s->name);
        send_cmd(ServerName, "SQUIT %s :%s", s->name, buf);
    }
#endif

    delete_server(s, buf);
}

/* EOF */
