/* Definitions of IRC message functions and list of messages.
 *
 * (C) 2003 Anope Team
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
#include "messages.h"
#include "language.h"

static char *uplink;
int servernum;
/* List of messages is at the bottom of the file. */

/*************************************************************************/
/*************************************************************************/

static int m_nickcoll(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    if (!skeleton && !readonly)
        introduce_user(av[0]);
    return MOD_CONT;
}

/*************************************************************************/

static int m_ping(char *source, int ac, char **av)
{
    if (ac < 1)
        return MOD_CONT;
    send_cmd(ServerName, "PONG %s %s", ac > 1 ? av[1] : ServerName, av[0]);
    return MOD_CONT;
}

/*************************************************************************/

static int m_away(char *source, int ac, char **av)
{
    User *u = finduser(source);

    if (u && (ac == 0 || *av[0] == 0))  /* un-away */
        check_memos(u);
    return MOD_CONT;
}

/*************************************************************************/


#ifdef IRC_BAHAMUT

static int m_cs(char *source, int ac, char **av)
{
    User *u;
    time_t starttime, stoptime; /* When processing started and finished */

    if (ac < 1 || skeleton)
        return MOD_CONT;

    u = finduser(source);

    if (!u) {
        alog("%s: user record for %s not found", s_ChanServ, source);
        notice(s_ChanServ, source, getstring(NULL, USER_RECORD_NOT_FOUND));
        return MOD_CONT;
    }

    /* Check if we should ignore.  Operators always get through. */
    if (allow_ignore && !is_oper(u)) {
        IgnoreData *ign = get_ignore(source);
        if (ign && ign->time > time(NULL)) {
            alog("Ignored message from %s: \"%s\"", source, inbuf);
            return MOD_CONT;
        }
    }

    starttime = time(NULL);
    if (!is_oper(u) && CSOpersOnly)
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    else
        chanserv(u, av[0]);

    /* Add to ignore list if the command took a significant amount of time. */
    if (allow_ignore) {
        stoptime = time(NULL);
        if (stoptime > starttime && *source && !strchr(source, '.'))
            add_ignore(source, stoptime - starttime);
    }
    return MOD_CONT;
}

#endif

/*************************************************************************/

#ifdef IRC_BAHAMUT

static int m_hs(char *source, int ac, char **av)
{
    User *u;
    time_t starttime, stoptime; /* When processing started and finished */

    if (ac < 1 || skeleton)
        return MOD_CONT;

    u = finduser(source);

    if (!u) {
        alog("%s: user record for %s not found", s_HelpServ, source);
        notice(s_HelpServ, source, getstring(NULL, USER_RECORD_NOT_FOUND));
        return MOD_CONT;
    }

    /* Check if we should ignore.  Operators always get through. */
    if (allow_ignore && !is_oper(u)) {
        IgnoreData *ign = get_ignore(source);
        if (ign && ign->time > time(NULL)) {
            alog("Ignored message from %s: \"%s\"", source, inbuf);
            return MOD_CONT;
        }
    }

    starttime = time(NULL);

    notice_help(s_HelpServ, u, HELP_HELP, s_NickServ, s_ChanServ,
                s_MemoServ);
    if (s_BotServ)
        notice_help(s_HelpServ, u, HELP_HELP_BOT, s_BotServ);

    /* Add to ignore list if the command took a significant amount of time. */
    if (allow_ignore) {
        stoptime = time(NULL);
        if (stoptime > starttime && *source && !strchr(source, '.'))
            add_ignore(source, stoptime - starttime);
    }
    return MOD_CONT;
}

#endif

/*************************************************************************/

static int m_join(char *source, int ac, char **av)
{
    if (ac != 1)
        return MOD_CONT;
    do_join(source, ac, av);
    return MOD_CONT;
}

/*************************************************************************/

static int m_kick(char *source, int ac, char **av)
{
    if (ac != 3)
        return MOD_CONT;
    do_kick(source, ac, av);
    return MOD_CONT;
}

/*************************************************************************/

static int m_kill(char *source, int ac, char **av)
{
    BotInfo *bi;

    if (ac != 2)
        return MOD_CONT;
    /* Recover if someone kills us. */
    if (stricmp(av[0], s_OperServ) == 0 ||
        (s_OperServAlias && stricmp(av[0], s_OperServAlias) == 0) ||
        stricmp(av[0], s_NickServ) == 0 ||
        (s_NickServAlias && stricmp(av[0], s_NickServAlias) == 0) ||
        stricmp(av[0], s_ChanServ) == 0 ||
        (s_ChanServAlias && stricmp(av[0], s_ChanServAlias) == 0) ||
        stricmp(av[0], s_MemoServ) == 0 ||
        (s_MemoServAlias && stricmp(av[0], s_MemoServAlias) == 0) ||
        (s_HostServ && stricmp(av[0], s_HostServ) == 0) ||
        (s_HostServAlias && stricmp(av[0], s_HostServAlias) == 0) ||
        (s_BotServ && stricmp(av[0], s_BotServ) == 0) ||
        (s_BotServAlias && stricmp(av[0], s_BotServAlias) == 0) ||
        stricmp(av[0], s_HelpServ) == 0 ||
        (s_HelpServAlias && stricmp(av[0], s_HelpServAlias) == 0) ||
        (s_DevNull && stricmp(av[0], s_DevNull) == 0) ||
        (s_DevNullAlias && stricmp(av[0], s_DevNullAlias) == 0) ||
        stricmp(av[0], s_GlobalNoticer) == 0 ||
        (s_GlobalNoticerAlias && stricmp(av[0], s_GlobalNoticerAlias) == 0)
        ) {
        if (!readonly && !skeleton)
            introduce_user(av[0]);
    } else if (s_BotServ && (bi = findbot(av[0]))) {
        if (!readonly && !skeleton) {
            introduce_user(av[0]);
            bot_rejoin_all(bi);
        }
    } else {
        do_kill(source, ac, av);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int m_mode(char *source, int ac, char **av)
{
    if (*av[0] == '#' || *av[0] == '&') {
        if (ac < 2)
            return MOD_CONT;
        do_cmode(source, ac, av);
    } else {
        if (ac != 2)
            return MOD_CONT;
        do_umode(source, ac, av);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int m_motd(char *source, int ac, char **av)
{
    FILE *f;
    char buf[BUFSIZE];

    f = fopen(MOTDFilename, "r");
    send_cmd(ServerName, "375 %s :- %s Message of the Day",
             source, ServerName);
    if (f) {
        while (fgets(buf, sizeof(buf), f)) {
            buf[strlen(buf) - 1] = 0;
            send_cmd(ServerName, "372 %s :- %s", source, buf);
        }
        fclose(f);
    } else {
        send_cmd(ServerName, "372 %s :- MOTD file not found!  Please "
                 "contact your IRC administrator.", source);
    }
    send_cmd(ServerName, "376 %s :End of /MOTD command.", source);
    return MOD_CONT;
}

/*************************************************************************/

#ifdef IRC_BAHAMUT

static int m_ms(char *source, int ac, char **av)
{
    User *u;
    time_t starttime, stoptime; /* When processing started and finished */

    if (ac < 1 || skeleton)
        return MOD_CONT;

    u = finduser(source);

    if (!u) {
        alog("%s: user record for %s not found", s_MemoServ, source);
        notice(s_MemoServ, source, getstring(NULL, USER_RECORD_NOT_FOUND));
        return MOD_CONT;
    }

    /* Check if we should ignore.  Operators always get through. */
    if (allow_ignore && !is_oper(u)) {
        IgnoreData *ign = get_ignore(source);
        if (ign && ign->time > time(NULL)) {
            alog("Ignored message from %s: \"%s\"", source, inbuf);
            return MOD_CONT;
        }
    }

    starttime = time(NULL);

    memoserv(u, av[0]);

    /* Add to ignore list if the command took a significant amount of time. */
    if (allow_ignore) {
        stoptime = time(NULL);
        if (stoptime > starttime && *source && !strchr(source, '.'))
            add_ignore(source, stoptime - starttime);
    }
    return MOD_CONT;
}

#endif

/*************************************************************************/

static int m_nick(char *source, int ac, char **av)
{
    if (ac != 2) {
#if defined(IRC_HYBRID)
        User *user = do_nick(source, av[0], av[4], av[5], av[6], av[7],
                             strtoul(av[2], NULL, 10), 0);
        if (user)
            set_umode(user, 1, &av[3]);
#else
#if defined(IRC_BAHAMUT)
#if defined(IRC_ULTIMATE3) || defined(IRC_RAGE2)
        User *user = do_nick(source, av[0], av[4], av[5], av[6], av[9],
                             strtoul(av[2], NULL, 10), strtoul(av[7], NULL,
                                                               0),
                             strtoul(av[8], NULL, 0), "*");
# else
        User *user = do_nick(source, av[0], av[4], av[5], av[6], av[9],
                             strtoul(av[2], NULL, 10), strtoul(av[7], NULL,
                                                               0),
                             strtoul(av[8], NULL, 0));
# endif
        if (user)
            set_umode(user, 1, &av[3]);
#elif defined(IRC_UNREAL)
        if (ac == 7) {
            /* For some reasons, Unreal sends this sometimes */
            do_nick(source, av[0], av[3], av[4], av[5], av[6],
                    strtoul(av[2], NULL, 10), 0, "*");
        } else {
            User *user = do_nick(source, av[0], av[3], av[4], av[5], av[9],
                                 strtoul(av[2], NULL, 10), strtoul(av[6],
                                                                   NULL,
                                                                   0),
                                 av[8]);

            if (user)
                set_umode(user, 1, &av[7]);
        }
#else
# if defined(IRC_ULTIMATE)
        if (ac == 7) {
            do_nick(source, av[0], av[3], av[4], av[5], av[6],
                    strtoul(av[2], NULL, 10), 0);
        } else {
            do_nick(source, av[0], av[3], av[4], av[5], av[7],
                    strtoul(av[2], NULL, 10), strtoul(av[6], NULL, 0));
        }
/* PTlink IRCd - PTS4 */
#elif defined(IRC_PTLINK)
        User *user = do_nick(source, av[0], av[4], av[6], av[7], av[8],
                             strtoul(av[2], NULL, 10), 0, av[5]);
        if (user)
            set_umode(user, 1, &av[3]);
#else
        do_nick(source, av[0], av[3], av[4], av[5], av[7],
                strtoul(av[2], NULL, 10), strtoul(av[6], NULL, 0));
# endif
#endif
#endif
    } else {
        do_nick(source, av[0], NULL, NULL, NULL, NULL,
                strtoul(av[1], NULL, 10), 0);
    }
    return MOD_CONT;
}

/*************************************************************************/

#ifdef IRC_ULTIMATE3

static int m_client(char *source, int ac, char **av)
{
    if (ac != 2) {
        User *user = do_nick(source, av[0], av[5], av[6], av[8], av[11],
                             strtoul(av[2], NULL, 10), strtoul(av[9], NULL,
                                                               0),
                             strtoul(av[10], NULL, 0), av[7]);
        if (user) {
            set_umode(user, 1, &av[3]);
        }
    }
    return MOD_CONT;
}

#endif

/*************************************************************************/

#ifdef IRC_RAGE2

static int m_snick(char *source, int ac, char **av)
{
    if (ac != 2) {
        User *user = do_nick(source, av[0], av[3], av[4], av[8], av[10],
                             strtoul(av[1], NULL, 10), strtoul(av[7], NULL,
                                                               0),
                             strtoul(av[5], NULL, 0), av[6]);
        if (user) {
            set_umode(user, 1, &av[9]);
        }
    }
    return MOD_CONT;
}

#endif

/*************************************************************************/

#ifdef IRC_BAHAMUT

static int m_ns(char *source, int ac, char **av)
{
    User *u;
    time_t starttime, stoptime; /* When processing started and finished */

    if (ac < 1 || skeleton)
        return MOD_CONT;

    u = finduser(source);

    if (!u) {
        alog("%s: user record for %s not found", s_NickServ, source);
        notice(s_NickServ, source, getstring(NULL, USER_RECORD_NOT_FOUND));
        return MOD_CONT;
    }

    /* Check if we should ignore.  Operators always get through. */
    if (allow_ignore && !is_oper(u)) {
        IgnoreData *ign = get_ignore(source);
        if (ign && ign->time > time(NULL)) {
            alog("Ignored message from %s: \"%s\"", source, inbuf);
            return MOD_CONT;
        }
    }

    starttime = time(NULL);

    nickserv(u, av[0]);

    /* Add to ignore list if the command took a significant amount of time. */
    if (allow_ignore) {
        stoptime = time(NULL);
        if (stoptime > starttime && *source && !strchr(source, '.'))
            add_ignore(source, stoptime - starttime);
    }
    return MOD_CONT;
}

#endif

/*************************************************************************/

#ifdef IRC_PTLINK
/*
 * Note: This function has no validation whatsoever. Also, as of PTlink6.15.1
 * when you /deoper you get to keep your vindent, but you lose your vhost. In
 * that case serives will *NOT* modify it's internal record for the vhost. We
 * need to address this in the future.
 */
static int m_newmask(char *source, int ac, char **av)
{
    User *u;
    char *newhost = NULL, *newuser = NULL;

    if (ac != 1)
        return MOD_CONT;
    u = finduser(source);

    if (!u) {
        alog("user: NEWMASK for nonexistent user %s", av[0]);
        return MOD_CONT;
    }

    newuser = myStrGetOnlyToken(av[0], '@', 0);
    if (newuser) {
        newhost = myStrGetTokenRemainder(av[0], '@', 1);
        change_user_username(u, newuser);
    } else {
        newhost = av[0];
    }

    if (*newhost == '@')
        newhost++;

    if (newhost) {
        change_user_host(u, newhost);
    }

    return MOD_CONT;
}
#endif


/*************************************************************************/

#ifdef IRC_BAHAMUT

static int m_os(char *source, int ac, char **av)
{
    User *u;
    time_t starttime, stoptime; /* When processing started and finished */

    if (ac < 1)
        return MOD_CONT;

    u = finduser(source);

    if (!u) {
        alog("%s: user record for %s not found", s_OperServ, source);
        notice(s_OperServ, source, getstring(NULL, USER_RECORD_NOT_FOUND));
        return MOD_CONT;
    }

    /* Check if we should ignore.  Operators always get through. */
    if (allow_ignore && !is_oper(u)) {
        IgnoreData *ign = get_ignore(source);
        if (ign && ign->time > time(NULL)) {
            alog("Ignored message from %s: \"%s\"", source, inbuf);
            return MOD_CONT;
        }
    }

    starttime = time(NULL);

    if (is_oper(u)) {
        operserv(u, av[0]);
    } else {
        notice_lang(s_OperServ, u, ACCESS_DENIED);

        if (WallBadOS)
            wallops(s_OperServ,
                    "Denied access to %s from %s!%s@%s (non-oper)",
                    s_OperServ, u->nick, u->username, u->host);
    }

    /* Add to ignore list if the command took a significant amount of time. */
    if (allow_ignore) {
        stoptime = time(NULL);
        if (stoptime > starttime && *source && !strchr(source, '.'))
            add_ignore(source, stoptime - starttime);
    }
    return MOD_CONT;
}

#endif

/*************************************************************************/

static int m_part(char *source, int ac, char **av)
{
    if (ac < 1 || ac > 2)
        return MOD_CONT;
    do_part(source, ac, av);
    return MOD_CONT;
}

/*************************************************************************/

static int m_privmsg(char *source, int ac, char **av)
{
    char *s;
    time_t starttime, stoptime; /* When processing started and finished */

    BotInfo *bi;
    ChannelInfo *ci;
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(source);

    if (!u) {
        alog("%s: user record for %s not found", av[1], source);
        notice(av[1], source, getstring(NULL, USER_RECORD_NOT_FOUND));
        return MOD_CONT;
    }

    if (*av[0] == '#') {
        if (s_BotServ && (ci = cs_findchan(av[0])))
            if (!(ci->flags & CI_VERBOTEN) && ci->c && ci->bi)  /* Some paranoia checks */
                botchanmsgs(u, ci, av[1]);
    } else {

        /* Check if we should ignore.  Operators always get through. */
        if (allow_ignore && !is_oper(u)) {
            IgnoreData *ign = get_ignore(source);
            if (ign && ign->time > time(NULL)) {
                alog("Ignored message from %s: \"%s\"", source, inbuf);
                return MOD_CONT;
            }
        }

        /* If a server is specified (nick@server format), make sure it matches
         * us, and strip it off. */
        s = strchr(av[0], '@');
        if (s) {
            *s++ = 0;
            if (stricmp(s, ServerName) != 0)
                return MOD_CONT;
        }

        starttime = time(NULL);

        if ((stricmp(av[0], s_OperServ) == 0)
            || (s_OperServAlias && (stricmp(av[0], s_OperServAlias) == 0))) {
            if (is_oper(u)) {
                operserv(u, av[1]);
            } else {
                notice_lang(s_OperServ, u, ACCESS_DENIED);

                if (WallBadOS)
                    wallops(s_OperServ,
                            "Denied access to %s from %s!%s@%s (non-oper)",
                            s_OperServ, u->nick, u->username, u->host);
            }
        } else if ((stricmp(av[0], s_NickServ) == 0)
                   || (s_NickServAlias
                       && (stricmp(av[0], s_NickServAlias) == 0))) {
            nickserv(u, av[1]);
        } else if ((stricmp(av[0], s_ChanServ) == 0)
                   || (s_ChanServAlias
                       && (stricmp(av[0], s_ChanServAlias) == 0))) {
            if (!is_oper(u) && CSOpersOnly)
                notice_lang(s_ChanServ, u, ACCESS_DENIED);
            else
                chanserv(u, av[1]);
        } else if ((stricmp(av[0], s_MemoServ) == 0)
                   || (s_MemoServAlias
                       && (stricmp(av[0], s_MemoServAlias) == 0))) {
            memoserv(u, av[1]);
        } else if (s_HostServ && ((stricmp(av[0], s_HostServ) == 0)
                                  || (s_HostServAlias
                                      && (stricmp(av[0], s_HostServAlias)
                                          == 0)))) {
            hostserv(u, av[1]);
        } else if (s_HelpServ && ((stricmp(av[0], s_HelpServ) == 0)
                                  || (s_HelpServAlias
                                      && (stricmp(av[0], s_HelpServAlias)
                                          == 0)))) {
            helpserv(u, av[1]);
        } else if (s_BotServ && ((stricmp(av[0], s_BotServ) == 0)
                                 || (s_BotServAlias
                                     && (stricmp(av[0], s_BotServAlias) ==
                                         0)))) {
            botserv(u, av[1]);
/* This HelpServ code is history since HelpServ is a REAL service */

/*        } else if ((stricmp(av[0], s_HelpServ) == 0)
                   || (s_HelpServAlias
                       && (stricmp(av[0], s_HelpServAlias) == 0))) {
            notice_help(s_HelpServ, u, HELP_HELP, s_NickServ, s_ChanServ,
                        s_MemoServ);
            if (s_BotServ)
                notice_help(s_HelpServ, u, HELP_HELP_BOT, s_BotServ); */
        } else if (s_BotServ && (bi = findbot(av[0]))) {
            botmsgs(u, bi, av[1]);
        }

        /* Add to ignore list if the command took a significant amount of time. */
        if (allow_ignore) {
            stoptime = time(NULL);
            if (stoptime > starttime && *source && !strchr(source, '.'))
                add_ignore(source, stoptime - starttime);
        }
    }
    return MOD_CONT;
}

/*************************************************************************/

static int m_quit(char *source, int ac, char **av)
{
    if (ac != 1)
        return MOD_CONT;
    do_quit(source, ac, av);
    return MOD_CONT;
}

/*************************************************************************/

static int m_server(char *source, int ac, char **av)
{
    if (!stricmp(av[1], "1"))
        uplink = sstrdup(av[0]);
    return MOD_CONT;
}

/*************************************************************************/

#if defined(IRC_ULTIMATE3)

static int m_sethost(char *source, int ac, char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(av[0]);
    if (!u) {
        if (debug)
            alog("user: SETHOST for nonexistent user %s", av[0]);
        return MOD_CONT;
    }

    change_user_host(u, av[1]);
    return MOD_CONT;
}

#endif

/*************************************************************************/

#ifdef IRC_RAGE2

static int m_vhost(char *source, int ac, char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(av[0]);
    if (!u) {
        if (debug)
            alog("user: VHOST for nonexistent user %s", av[0]);
        return MOD_CONT;
    }

    change_user_host(u, av[1]);
    return MOD_CONT;
}

#endif

/*************************************************************************/

#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_VIAGRA)

static int m_chghost(char *source, int ac, char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(av[0]);
    if (!u) {
        alog("user: CHGHOST for nonexistent user %s", av[0]);
        return MOD_CONT;
    }

    change_user_host(u, av[1]);
    return MOD_CONT;
}

/*************************************************************************/

static int m_sethost(char *source, int ac, char **av)
{
    User *u;

    if (ac != 1)
        return MOD_CONT;

    u = finduser(source);
    if (!u) {
        if (debug)
            alog("user: SETHOST for nonexistent user %s", source);
        return MOD_CONT;
    }

    change_user_host(u, av[0]);
    return MOD_CONT;
}

/*************************************************************************/

static int m_chgident(char *source, int ac, char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(av[0]);
    if (!u) {
        alog("user: CHGIDENT for nonexistent user %s", av[0]);
        return MOD_CONT;
    }

    change_user_username(u, av[1]);
    return MOD_CONT;
}

/*************************************************************************/

static int m_setident(char *source, int ac, char **av)
{
    User *u;

    if (ac != 1)
        return MOD_CONT;

    u = finduser(source);
    if (!u) {
        alog("user: SETIDENT for nonexistent user %s", source);
        return MOD_CONT;
    }

    change_user_username(u, av[0]);
    return MOD_CONT;
}

/*************************************************************************/

static int m_chgname(char *source, int ac, char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(av[0]);
    if (!u) {
        alog("user: CHGNAME for nonexistent user %s", av[0]);
        return MOD_CONT;
    }

    change_user_realname(u, av[1]);
    return MOD_CONT;
}

/*************************************************************************/

static int m_setname(char *source, int ac, char **av)
{
    User *u;

    if (ac != 1)
        return MOD_CONT;

    u = finduser(source);
    if (!u) {
        alog("user: SETNAME for nonexistent user %s", source);
        return MOD_CONT;
    }

    change_user_realname(u, av[0]);
    return MOD_CONT;
}

#endif

/*************************************************************************/

#if defined(IRC_BAHAMUT) || defined(IRC_HYBRID) || defined(IRC_PTLINK) || defined(IRC_RAGE2)

static int m_sjoin(char *source, int ac, char **av)
{
    do_sjoin(source, ac, av);
    return MOD_CONT;
}

#endif

/*************************************************************************/

static int m_stats(char *source, int ac, char **av)
{
    int i;
    User *u;
    NickCore *nc;

    if (ac < 1)
        return MOD_CONT;

    switch (*av[0]) {
    case 'l':
        u = finduser(source);

        if (u && is_oper(u)) {

            if (servernum == 1) {
                send_cmd(NULL,
                         "211 %s Server SendBuf SentBytes SentMsgs RecvBuf "
                         "RecvBytes RecvMsgs ConnTime", source);
                send_cmd(NULL, "211 %s %s %d %d %d %d %d %d %ld", source,
                         RemoteServer, write_buffer_len(), total_written,
                         -1, read_buffer_len(), total_read, -1,
                         time(NULL) - start_time);
            } else if (servernum == 2) {
                send_cmd(NULL,
                         "211 %s Server SendBuf SentBytes SentMsgs RecvBuf "
                         "RecvBytes RecvMsgs ConnTime", source);
                send_cmd(NULL, "211 %s %s %d %d %d %d %d %d %ld", source,
                         RemoteServer2, write_buffer_len(), total_written,
                         -1, read_buffer_len(), total_read, -1,
                         time(NULL) - start_time);
            } else if (servernum == 3) {
                send_cmd(NULL,
                         "211 %s Server SendBuf SentBytes SentMsgs RecvBuf "
                         "RecvBytes RecvMsgs ConnTime", source);
                send_cmd(NULL, "211 %s %s %d %d %d %d %d %d %ld", source,
                         RemoteServer3, write_buffer_len(), total_written,
                         -1, read_buffer_len(), total_read, -1,
                         time(NULL) - start_time);
            }
        }

        send_cmd(NULL, "219 %s l :End of /STATS report.", source);
        break;
    case 'o':
    case 'O':
/* Check whether the user is an operator */
        u = finduser(source);
        if (u && !is_oper(u) && HideStatsO) {
            send_cmd(NULL, "219 %s %c :End of /STATS report.", source,
                     *av[0]);
        } else {
            for (i = 0; i < RootNumber; i++)
                send_cmd(NULL, "243 %s O * * %s Root 0", source,
                         ServicesRoots[i]);
            for (i = 0; i < servadmins.count && (nc = servadmins.list[i]);
                 i++)
                send_cmd(NULL, "243 %s O * * %s Admin 0", source,
                         nc->display);
            for (i = 0; i < servopers.count && (nc = servopers.list[i]);
                 i++)
                send_cmd(NULL, "243 %s O * * %s Oper 0", source,
                         nc->display);

            send_cmd(NULL, "219 %s %c :End of /STATS report.", source,
                     *av[0]);
        }

        break;

    case 'u':{
            int uptime = time(NULL) - start_time;
            send_cmd(NULL, "242 %s :Services up %d day%s, %02d:%02d:%02d",
                     source, uptime / 86400,
                     (uptime / 86400 == 1) ? "" : "s",
                     (uptime / 3600) % 24, (uptime / 60) % 60,
                     uptime % 60);
            send_cmd(NULL,
                     "250 %s :Current users: %d (%d ops); maximum %d",
                     source, usercnt, opcnt, maxusercnt);
            send_cmd(NULL, "219 %s u :End of /STATS report.", source);
            break;
        }                       /* case 'u' */

    default:
        send_cmd(NULL, "219 %s %c :End of /STATS report.", source, *av[0]);
        break;
    }
    return MOD_CONT;
}

/*************************************************************************/

static int m_time(char *source, int ac, char **av)
{
    time_t t;
    struct tm *tm;
    char buf[64];

    time(&t);
    tm = localtime(&t);
    strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Y %Z", tm);
    send_cmd(NULL, "391 :%s", buf);
    return MOD_CONT;
}

/*************************************************************************/
#ifdef IRC_HYBRID
static int m_topic(char *source, int ac, char **av)
{
    if (ac == 4) {
        do_topic(source, ac, av);
    } else {
        Channel *c = findchan(av[0]);
        time_t topic_time = time(NULL);

        if (!c) {
            alog("channel: TOPIC %s for nonexistent channel %s",
                 merge_args(ac - 1, av + 1), av[0]);
            return MOD_CONT;
        }

        if (check_topiclock(c, topic_time))
            return MOD_CONT;

        if (c->topic) {
            free(c->topic);
            c->topic = NULL;
        }
        if (ac > 1 && *av[1])
            c->topic = sstrdup(av[1]);

        strscpy(c->topic_setter, source, sizeof(c->topic_setter));
        c->topic_time = topic_time;

        record_topic(av[0]);
    }
    return MOD_CONT;
}
#else
/*************************************************************************/
static int m_topic(char *source, int ac, char **av)
{
    if (ac != 4)
        return MOD_CONT;
    do_topic(source, ac, av);
    return MOD_CONT;
}
#endif
/*************************************************************************/

int m_version(char *source, int ac, char **av)
{
    if (source)
        send_cmd(ServerName, "351 %s Anope-%s %s :%s -- %s",
                 source, version_number, ServerName, version_flags,
                 version_build);
    return MOD_CONT;
}

/*************************************************************************/

int m_whois(char *source, int ac, char **av)
{
    BotInfo *bi;
    const char *clientdesc;

    if (source && ac >= 1) {
        if (stricmp(av[0], s_NickServ) == 0)
            clientdesc = desc_NickServ;
        else if (stricmp(av[0], s_ChanServ) == 0)
            clientdesc = desc_ChanServ;
        else if (stricmp(av[0], s_MemoServ) == 0)
            clientdesc = desc_MemoServ;
        else if (s_BotServ && stricmp(av[0], s_BotServ) == 0)
            clientdesc = desc_BotServ;
        else if (s_HostServ && stricmp(av[0], s_HostServ) == 0)
            clientdesc = desc_HostServ;
        else if (stricmp(av[0], s_HelpServ) == 0)
            clientdesc = desc_HelpServ;
        else if (stricmp(av[0], s_OperServ) == 0)
            clientdesc = desc_OperServ;
        else if (stricmp(av[0], s_GlobalNoticer) == 0)
            clientdesc = desc_GlobalNoticer;
        else if (s_DevNull && stricmp(av[0], s_DevNull) == 0)
            clientdesc = desc_DevNull;
        else if (s_BotServ && (bi = findbot(av[0]))) {
            /* Bots are handled separately */
            send_cmd(ServerName, "311 %s %s %s %s * :%s", source, bi->nick,
                     bi->user, bi->host, bi->real);
            send_cmd(ServerName, "307 %s :%s is a registered nick", source,
                     bi->nick);
            send_cmd(ServerName, "312 %s %s %s :%s", source, bi->nick,
                     ServerName, ServerDesc);
            send_cmd(ServerName,
                     "317 %s %s %ld %ld :seconds idle, signon time",
                     source, bi->nick, time(NULL) - bi->lastmsg,
                     start_time);
            send_cmd(ServerName, "318 %s %s :End of /WHOIS list.", source,
                     bi->nick);
            return MOD_CONT;
        } else {
            send_cmd(ServerName, "401 %s %s :No such service.", source,
                     av[0]);
            return MOD_CONT;
        }
        send_cmd(ServerName, "311 %s %s %s %s * :%s", source, av[0],
                 ServiceUser, ServiceHost, clientdesc);
        send_cmd(ServerName, "312 %s %s %s :%s", source, av[0], ServerName,
                 ServerDesc);
        send_cmd(ServerName,
                 "317 %s %s %ld %ld :seconds idle, signon time", source,
                 av[0], time(NULL) - start_time, start_time);
        send_cmd(ServerName, "318 %s %s :End of /WHOIS list.", source,
                 av[0]);
    }
    return MOD_CONT;
}

/*************************************************************************/

#ifdef IRC_VIAGRA
int m_vs(char *source, int ac, char **av)
{
    User *u;

    if (ac != 2)
        return MOD_CONT;

    u = finduser(av[0]);
    if (!u) {
        alog("user: VS for nonexistent user %s", av[0]);
        return MOD_CONT;
    }

    change_user_host(u, av[1]);
    return MOD_CONT;

}
#endif

/*************************************************************************/

/* *INDENT-OFF* */
void moduleAddMsgs(void) {
    Message *m;
    m = createMessage("401",       NULL); addCoreMessage(IRCD,m);
    m = createMessage("436",       m_nickcoll); addCoreMessage(IRCD,m);
    m = createMessage("AWAY",      m_away); addCoreMessage(IRCD,m);
    m = createMessage("INVITE",    NULL); addCoreMessage(IRCD,m);
    m = createMessage("JOIN",      m_join); addCoreMessage(IRCD,m);
    m = createMessage("KICK",      m_kick); addCoreMessage(IRCD,m);
    m = createMessage("KILL",      m_kill); addCoreMessage(IRCD,m);
    m = createMessage("MODE",      m_mode); addCoreMessage(IRCD,m);
    m = createMessage("MOTD",      m_motd); addCoreMessage(IRCD,m);
    m = createMessage("NICK",      m_nick); addCoreMessage(IRCD,m);
    m = createMessage("NOTICE",    NULL); addCoreMessage(IRCD,m);
    m = createMessage("PART",      m_part); addCoreMessage(IRCD,m);
    m = createMessage("PASS",      NULL); addCoreMessage(IRCD,m);
    m = createMessage("PING",      m_ping); addCoreMessage(IRCD,m);
    m = createMessage("PRIVMSG",   m_privmsg); addCoreMessage(IRCD,m);
    m = createMessage("QUIT",      m_quit); addCoreMessage(IRCD,m);
    m = createMessage("SERVER",    m_server); addCoreMessage(IRCD,m);
    m = createMessage("SQUIT",     NULL); addCoreMessage(IRCD,m);
    m = createMessage("STATS",     m_stats); addCoreMessage(IRCD,m);
    m = createMessage("TIME",      m_time); addCoreMessage(IRCD,m);
    m = createMessage("TOPIC",     m_topic); addCoreMessage(IRCD,m);
    m = createMessage("USER",      NULL); addCoreMessage(IRCD,m);
    m = createMessage("VERSION",   m_version); addCoreMessage(IRCD,m);
    m = createMessage("WALLOPS",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("WHOIS",     m_whois); addCoreMessage(IRCD,m);

    /* DALnet specific messages */
    m = createMessage("AKILL",     NULL); addCoreMessage(IRCD,m);
    m = createMessage("GLOBOPS",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("GNOTICE",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("GOPER",     NULL); addCoreMessage(IRCD,m);
    m = createMessage("RAKILL",    NULL); addCoreMessage(IRCD,m);
    m = createMessage("SILENCE",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("SVSKILL",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("SVSMODE",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("SVSNICK",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("SVSNOOP",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("SQLINE",    NULL); addCoreMessage(IRCD,m);
    m = createMessage("UNSQLINE",  NULL); addCoreMessage(IRCD,m);

    /* DreamForge specific messages */
#ifdef IRC_DREAMFORGE
    m = createMessage("PROTOCTL",  NULL); addCoreMessage(IRCD,m);
#endif

    /* Bahamut specific messages */
#ifdef IRC_BAHAMUT
    m = createMessage("CAPAB", 	   NULL); addCoreMessage(IRCD,m);
    m = createMessage("CS",        m_cs); addCoreMessage(IRCD,m);
    m = createMessage("HS",        m_hs); addCoreMessage(IRCD,m);
    m = createMessage("MS",        m_ms); addCoreMessage(IRCD,m);
    m = createMessage("NS",        m_ns); addCoreMessage(IRCD,m);
    m = createMessage("OS",        m_os); addCoreMessage(IRCD,m);
    m = createMessage("RS",        NULL); addCoreMessage(IRCD,m);
    m = createMessage("SGLINE",    NULL); addCoreMessage(IRCD,m);
    m = createMessage("SJOIN",     m_sjoin); addCoreMessage(IRCD,m);
    m = createMessage("SS",        NULL); addCoreMessage(IRCD,m);
    m = createMessage("SVINFO",    NULL); addCoreMessage(IRCD,m);
    m = createMessage("SZLINE",    NULL); addCoreMessage(IRCD,m);
    m = createMessage("UNSGLINE",  NULL); addCoreMessage(IRCD,m);
    m = createMessage("UNSZLINE",  NULL); addCoreMessage(IRCD,m);
#endif
 /* Hyb Messages */
#ifdef IRC_HYBRID
    m = createMessage("CAPAB",     NULL); addCoreMessage(IRCD,m);
    m = createMessage("SJOIN",     m_sjoin); addCoreMessage(IRCD,m);
    m = createMessage("SVINFO",    NULL); addCoreMessage(IRCD,m);
#endif


#ifdef IRC_ULTIMATE
    m = createMessage("CHGHOST",   m_chghost); addCoreMessage(IRCD,m);
    m = createMessage("CHGIDENT",  m_chgident); addCoreMessage(IRCD,m);
    m = createMessage("CHGNAME",   m_chgname); addCoreMessage(IRCD,m);
    m = createMessage("NETINFO",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("SETHOST",   m_sethost); addCoreMessage(IRCD,m);
    m = createMessage("SETIDENT",  m_setident); addCoreMessage(IRCD,m);
    m = createMessage("SETNAME",   m_setname); addCoreMessage(IRCD,m);
    m = createMessage("VCTRL",     NULL); addCoreMessage(IRCD,m);
#endif

#ifdef IRC_PTLINK
    m = createMessage("NEWMASK" , m_newmask); addCoreMessage(IRCD,m);
    m = createMessage("CAPAB"  , NULL); addCoreMessage(IRCD,m);
    m = createMessage("SVINFO" , NULL); addCoreMessage(IRCD,m);
    m = createMessage("SVSINFO"        , NULL); addCoreMessage(IRCD,m);
    m = createMessage("SJOIN",     m_sjoin); addCoreMessage(IRCD,m);
#endif

#ifdef IRC_UNREAL
    m = createMessage("CHGHOST",   m_chghost); addCoreMessage(IRCD,m);
    m = createMessage("CHGIDENT",  m_chgident); addCoreMessage(IRCD,m);
    m = createMessage("CHGNAME",   m_chgname); addCoreMessage(IRCD,m);
    m = createMessage("NETINFO",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("SETHOST",   m_sethost); addCoreMessage(IRCD,m);
    m = createMessage("SETIDENT",  m_setident); addCoreMessage(IRCD,m);
    m = createMessage("SETNAME",   m_setname); addCoreMessage(IRCD,m);
#endif
#ifdef IRC_VIAGRA
    m = createMessage("CHGHOST",   m_chghost); addCoreMessage(IRCD,m);
    m = createMessage("CHGIDENT",  m_chgident); addCoreMessage(IRCD,m);
    m = createMessage("CHGNAME",   m_chgname); addCoreMessage(IRCD,m);
    m = createMessage("SETHOST",   m_sethost); addCoreMessage(IRCD,m);
    m = createMessage("SETIDENT",  m_setident); addCoreMessage(IRCD,m);
    m = createMessage("SETNAME",   m_setname); addCoreMessage(IRCD,m);
    m = createMessage("VS",        m_vs); addCoreMessage(IRCD,m);
#endif
#ifdef IRC_ULTIMATE3
    m = createMessage("SETHOST",   m_sethost); addCoreMessage(IRCD,m);
    m = createMessage("NETINFO",   NULL); addCoreMessage(IRCD,m);
    m = createMessage("GCONNECT",	NULL); addCoreMessage(IRCD,m);
    m = createMessage("NETGLOBAL", NULL); addCoreMessage(IRCD,m);
    m = createMessage("CHATOPS",	NULL); addCoreMessage(IRCD,m);
    m = createMessage("NETCTRL",	NULL); addCoreMessage(IRCD,m);
    m = createMessage("CLIENT",	m_client); addCoreMessage(IRCD,m);
    m = createMessage("SMODE",	NULL); addCoreMessage(IRCD,m);
#endif
#ifdef IRC_RAGE2
    m = createMessage("SNICK", m_snick); addCoreMessage(IRCD,m);
    m = createMessage("VHOST", m_vhost); addCoreMessage(IRCD,m);
#endif
}

/* *INDENT-ON* */
/*************************************************************************/

Message *find_message(const char *name)
{
    Message *m;
    m = findMessage(IRCD, name);
    return m;
}

/*************************************************************************/
