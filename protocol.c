/* Simple interfaces to various protocols.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id: protocol.c,v 1.11 2003/07/20 01:15:50 dane Exp $ 
 *
 */

#include "services.h"

/* Makes an permanent ban from all the servers. Assumes that the matching clients are killed. */

void s_akill(char *user, char *host, char *who, time_t when,
             time_t expires, char *reason)
{
#if defined(IRC_BAHAMUT)
    /* send_cmd(NULL, "AKILL %s %s %d %s %ld :%s", host, user, 86400*2, who, when, reason); */
    send_cmd(NULL, "AKILL %s %s %d %s %ld :%s", host, user,
             86400 * 2, who, time(NULL), reason);

#elif defined(IRC_UNREAL)
    send_cmd(NULL, "TKL + G %s %s %s %ld %ld :%s", user, host, who, time(NULL) + 86400 * 2,     /* Avoids filling the akill list of servers too much */
             when, reason);
#elif defined(IRC_DREAMFORGE)
    send_cmd(NULL, "AKILL %s %s :%s", host, user, reason);
#elif defined(IRC_PTLINK)
    send_cmd(ServerName, "GLINE %s@%s %i %s :%s", user, host, 86400 * 2,
             who, reason);
#elif defined(IRC_HYBRID)
    send_cmd(s_OperServ, "KLINE * %ld %s %s :%s",
             (expires - (long) time(NULL)), user, host, reason);
#endif
}

/*************************************************************************/

/* Removes a permanent ban from all the servers. */

void s_rakill(char *user, char *host)
{
#if defined(IRC_PTLINK)
    send_cmd(NULL, "UNGLINE %s@%s", user, host);
#elif defined(IRC_UNREAL)
    send_cmd(NULL, "TKL - G %s %s %s", user, host, s_OperServ);
#elif !defined(IRC_HYBRID)
    send_cmd(NULL, "RAKILL %s %s", host, user);
#endif
}

/*************************************************************************/

void s_sgline(char *mask, char *reason)
{
#ifdef IRC_BAHAMUT
    /* User *u; */

    send_cmd(NULL, "SGLINE %d :%s:%s", strlen(mask), mask, reason);

    /* Do things properly: kill all corresponding users as this is 
       unfortunately not done by the IRCds :/ */
    /* Breaks things currently! */
    /* for (u = firstuser(); u; u = nextuser())
       if (match_wild_nocase(mask, u->realname))
       send_cmd(NULL, "SVSKILL %s :G-Lined: %s", u->nick, reason); */
#endif
}

/*************************************************************************/

void s_sqline(char *mask, char *reason)
{
#ifdef IRC_BAHAMUT
    if (*mask == '#') {
        int i;
        Channel *c, *next;

        char *av[3];
        struct c_userlist *cu, *cunext;

        send_cmd(NULL, "SQLINE %s :%s", mask, reason);

        for (i = 0; i < 1024; i++) {
            for (c = chanlist[i]; c; c = next) {
                next = c->next;

                if (!match_wild_nocase(mask, c->name))
                    continue;

                for (cu = c->users; cu; cu = cunext) {
                    cunext = cu->next;

                    if (is_oper(cu->user))
                        continue;

                    av[0] = c->name;
                    av[1] = cu->user->nick;
                    av[2] = reason;
                    send_cmd(s_OperServ, "KICK %s %s :Q-Lined: %s", av[0],
                             av[1], av[2]);
                    do_kick(s_ChanServ, 3, av);
                }
            }
        }
    } else {
#endif
        /* int i;
           User *u, *next; */

        send_cmd(NULL, "SQLINE %s :%s", mask, reason);

        /* for (i = 0; i < 1024; i++) {
           for (u = userlist[i]; u; u = next) {
           next = u->next;
           if (match_wild_nocase(mask, u->nick))
           #ifdef IRC_BAHAMUT
           send_cmd(NULL, "SVSKILL %s :%s", u->nick, reason);
           #else
           kill_user(s_OperServ, u->nick, reason);                      
           #endif
           }
           } */
#ifdef IRC_BAHAMUT
    }
#endif
}

/*************************************************************************/

void s_svsnoop(char *server, int set)
{
#ifndef IRC_HYBRID
#ifdef IRC_PTLINK
    send_cmd(NULL, "SVSADMIN %s :%s", server, set ? "noopers" : "rehash");
#else
    send_cmd(NULL, "SVSNOOP %s %s", server, (set ? "+" : "-"));
#endif
#endif
}

/*************************************************************************/

void s_szline(char *mask, char *reason)
{
#ifdef IRC_BAHAMUT
    send_cmd(NULL, "SZLINE %s :%s", mask, reason);
#endif
}

/*************************************************************************/

void s_unsgline(char *mask)
{
#ifdef IRC_BAHAMUT
    send_cmd(NULL, "UNSGLINE 0 :%s", mask);
#endif
}

/*************************************************************************/

void s_unsqline(char *mask)
{
#ifdef IRC_BAHAMUT
    send_cmd(NULL, "UNSQLINE 0 %s", mask);
#else
    send_cmd(NULL, "UNSQLINE %s", mask);
#endif
}

/*************************************************************************/

void s_unszline(char *mask)
{
#ifdef IRC_BAHAMUT
    send_cmd(NULL, "UNSZLINE 0 %s", mask);
#endif
}
