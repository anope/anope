/* ChanServ core functions
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

#include "module.h"


int do_akick(User * u);
void myChanServHelp(User * u);
int get_access_nc(NickCore *nc, ChannelInfo *ci);

/**
 * Create the command, and tell anope about it.
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT to allow the module, MOD_STOP to stop it
 **/
int AnopeInit(int argc, char **argv)
{
    Command *c;

    moduleAddAuthor("Anope");
    moduleAddVersion(VERSION_STRING);
    moduleSetType(CORE);

    c = createCommand("AKICK", do_akick, NULL, CHAN_HELP_AKICK, -1, -1, -1,
                      -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);

    moduleSetChanHelp(myChanServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_AKICK);
}

/**
 * The /cs akick command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
 /* `last' is set to the last index this routine was called with */
int akick_del(User * u, AutoKick * akick)
{
    if (!(akick->flags & AK_USED))
        return 0;
    if (akick->flags & AK_ISNICK) {
        akick->u.nc = NULL;
    } else {
        free(akick->u.mask);
        akick->u.mask = NULL;
    }
    if (akick->reason) {
        free(akick->reason);
        akick->reason = NULL;
    }
    if (akick->creator) {
        free(akick->creator);
        akick->creator = NULL;
    }
    akick->addtime = 0;
    akick->flags = 0;
    return 1;
}

int akick_del_callback(User * u, int num, va_list args)
{
    ChannelInfo *ci = va_arg(args, ChannelInfo *);
    int *last = va_arg(args, int *);

    *last = num;

    if (num < 1 || num > ci->akickcount)
        return 0;

    return akick_del(u, &ci->akick[num - 1]);
}


int akick_list(User * u, int index, ChannelInfo * ci, int *sent_header)
{
    AutoKick *akick = &ci->akick[index];

    if (!(akick->flags & AK_USED))
        return 0;
    if (!*sent_header) {
        notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_HEADER, ci->name);
        *sent_header = 1;
    }

    notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_FORMAT, index + 1,
                ((akick->flags & AK_ISNICK) ? akick->u.nc->
                 display : akick->u.mask),
                (akick->reason ? akick->
                 reason : getstring(u->na, NO_REASON)));
    return 1;
}

int akick_list_callback(User * u, int num, va_list args)
{
    ChannelInfo *ci = va_arg(args, ChannelInfo *);
    int *sent_header = va_arg(args, int *);
    if (num < 1 || num > ci->akickcount)
        return 0;
    return akick_list(u, num - 1, ci, sent_header);
}

int akick_view(User * u, int index, ChannelInfo * ci, int *sent_header)
{
    AutoKick *akick = &ci->akick[index];
    char timebuf[64];
    struct tm tm;

    if (!(akick->flags & AK_USED))
        return 0;
    if (!*sent_header) {
        notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_HEADER, ci->name);
        *sent_header = 1;
    }

    if (akick->addtime) {
        tm = *localtime(&akick->addtime);
        strftime_lang(timebuf, sizeof(timebuf), u,
                      STRFTIME_SHORT_DATE_FORMAT, &tm);
    } else {
        snprintf(timebuf, sizeof(timebuf), getstring(u->na, UNKNOWN));
    }

    notice_lang(s_ChanServ, u,
                ((akick->
                  flags & AK_STUCK) ? CHAN_AKICK_VIEW_FORMAT_STUCK :
                 CHAN_AKICK_VIEW_FORMAT), index + 1,
                ((akick->flags & AK_ISNICK) ? akick->u.nc->
                 display : akick->u.mask),
                akick->creator ? akick->creator : getstring(u->na,
                                                            UNKNOWN),
                timebuf,
                (akick->reason ? akick->
                 reason : getstring(u->na, NO_REASON)));
    return 1;
}

int akick_view_callback(User * u, int num, va_list args)
{
    ChannelInfo *ci = va_arg(args, ChannelInfo *);
    int *sent_header = va_arg(args, int *);
    if (num < 1 || num > ci->akickcount)
        return 0;
    return akick_view(u, num - 1, ci, sent_header);
}



int do_akick(User * u)
{
    char *chan = strtok(NULL, " ");
    char *cmd = strtok(NULL, " ");
    char *mask = strtok(NULL, " ");
    char *reason = strtok(NULL, "");
    ChannelInfo *ci;
    AutoKick *akick;
    int i;
    Channel *c;
    struct c_userlist *cu = NULL;
    struct c_userlist *next;
    User *u2;
    char *argv[3];
    int count = 0;

    if (!cmd || (!mask && (!stricmp(cmd, "ADD") || !stricmp(cmd, "STICK")
                           || !stricmp(cmd, "UNSTICK")
                           || !stricmp(cmd, "DEL")))) {

        syntax_error(s_ChanServ, u, "AKICK", CHAN_AKICK_SYNTAX);
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!check_access(u, ci, CA_AKICK) && !is_services_admin(u)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (stricmp(cmd, "ADD") == 0) {
        NickAlias *na = findnick(mask), *na2;
        NickCore *nc = NULL;
        char *nick, *user, *host;
        int freemask = 0;

        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_DISABLED);
            return MOD_CONT;
        }

        if (!na) {
            split_usermask(mask, &nick, &user, &host);
            mask =
                scalloc(strlen(nick) + strlen(user) + strlen(host) + 3, 1);
            freemask = 1;
            sprintf(mask, "%s!%s@%s", nick, user, host);
            free(nick);
            free(user);
            free(host);
        } else {
            if (na->status & NS_VERBOTEN) {
                notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, mask);
                return MOD_CONT;
            }
            nc = na->nc;
        }

        /* Check excepts BEFORE we get this far */
        if (ircd->except) {
            if (is_excepted_mask(ci, mask) == 1) {
                notice_lang(s_ChanServ, u, CHAN_EXCEPTED, mask, chan);
                if (freemask)
                    free(mask);
                return MOD_CONT;
            }
        }

        /* Check whether target nick has equal/higher access 
         * or whether the mask matches a user with higher/equal access - Viper */
        if ((ci->flags & CI_PEACE) && nc) {
            if ((nc == ci->founder) || (get_access_nc(nc, ci) >= get_access(u, ci))) {
                notice_lang(s_ChanServ, u, PERMISSION_DENIED);
                if (freemask)
                    free(mask);
                return MOD_CONT;
            }
        } else if ((ci->flags & CI_PEACE)) {
            char buf[BUFSIZE];
            /* Match against all currently online users with equal or
             * higher access. - Viper */
            for (i = 0; i < 1024; i++) {
                for (u2 = userlist[i]; u2; u2 = u2->next) {
                    if (is_founder(u2, ci) || (get_access(u2, ci) >= get_access(u, ci))) {
                        if (match_usermask(mask, u2)) {
                            notice_lang(s_ChanServ, u, PERMISSION_DENIED);
                            free(mask);
                            return MOD_CONT;
                        }
                    }
                }
            }

            /* Match against the lastusermask of all nickalias's with equal
             * or higher access. - Viper */
            for (i = 0; i < 1024; i++) {
                for (na2 = nalists[i]; na2; na2 = na2->next) {
                    if (na2->status & NS_VERBOTEN)
                        continue;

                    if (na2->nc && ((na2->nc == ci->founder) || (get_access_nc(na2->nc, ci) 
                            >= get_access(u, ci)))) {
                        snprintf(buf, BUFSIZE, "%s!%s", na2->nick, na2->last_usermask);
                        if (match_wild_nocase(mask, buf)) {
                            notice_lang(s_ChanServ, u, PERMISSION_DENIED);
                            free(mask);
                            return MOD_CONT;
                        }
                    }
                }
            }
        }

        for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
            if (!(akick->flags & AK_USED))
                continue;
            if ((akick->flags & AK_ISNICK) ? akick->u.nc == nc
                : stricmp(akick->u.mask, mask) == 0) {
                notice_lang(s_ChanServ, u, CHAN_AKICK_ALREADY_EXISTS,
                            (akick->flags & AK_ISNICK) ? akick->u.nc->
                            display : akick->u.mask, chan);
                if (freemask)
                    free(mask);
                return MOD_CONT;
            }
        }

        /* All entries should be in use so we don't have to go over
         * the entire list. We simply add new entries at the end. */
        if (ci->akickcount >= CSAutokickMax) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_REACHED_LIMIT, CSAutokickMax);
            if (freemask)
                free(mask);
            return MOD_CONT;
        }
        ci->akickcount++;
        ci->akick =
            srealloc(ci->akick, sizeof(AutoKick) * ci->akickcount);
        akick = &ci->akick[i];
        akick->flags = AK_USED;
        if (nc) {
            akick->flags |= AK_ISNICK;
            akick->u.nc = nc;
        } else {
            akick->u.mask = sstrdup(mask);
        }
        akick->creator = sstrdup(u->nick);
        akick->addtime = time(NULL);
        if (reason) {
            if (strlen(reason) > 200)
                reason[200] = '\0';
            akick->reason = sstrdup(reason);
        } else {
            akick->reason = NULL;
        }

        /* Auto ENFORCE #63 */
        c = findchan(ci->name);
        if (c) {
            cu = c->users;
            while (cu) {
                next = cu->next;
                if (check_kick(cu->user, c->name, c->creation_time)) {
                    argv[0] = sstrdup(c->name);
                    argv[1] = sstrdup(cu->user->nick);
                    if (akick->reason)
                        argv[2] = sstrdup(akick->reason);
                    else
                        argv[2] = sstrdup("none");

                    do_kick(s_ChanServ, 3, argv);

                    free(argv[2]);
                    free(argv[1]);
                    free(argv[0]);
                    count++;

                }
                cu = next;
            }
        }
        alog("%s: %s!%s@%s added akick for %s to %s",
              s_ChanServ, u->nick, u->username, u->host, mask, chan);
        notice_lang(s_ChanServ, u, CHAN_AKICK_ADDED, mask, chan);

        if (count)
            notice_lang(s_ChanServ, u, CHAN_AKICK_ENFORCE_DONE, chan,
                        count);

        if (freemask)
            free(mask);

    } else if (stricmp(cmd, "STICK") == 0) {
        NickAlias *na;
        NickCore *nc;

        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_DISABLED);
            return MOD_CONT;
        }

        if (ci->akickcount == 0) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, ci->name);
            return MOD_CONT;
        }

        na = findnick(mask);
        nc = (na ? na->nc : NULL);

        for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
            if (!(akick->flags & AK_USED) || (akick->flags & AK_ISNICK))
                continue;
            if (!stricmp(akick->u.mask, mask))
                break;
        }

        if (i == ci->akickcount) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_NOT_FOUND, mask,
                        ci->name);
            return MOD_CONT;
        }

        akick->flags |= AK_STUCK;
        alog("%s: %s!%s@%s set STICK on akick %s on %s",
             s_ChanServ, u->nick, u->username, u->host, akick->u.mask, ci->name);
        notice_lang(s_ChanServ, u, CHAN_AKICK_STUCK, akick->u.mask,
                    ci->name);

        if (ci->c)
            stick_mask(ci, akick);
    } else if (stricmp(cmd, "UNSTICK") == 0) {
        NickAlias *na;
        NickCore *nc;

        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_DISABLED);
            return MOD_CONT;
        }

        if (ci->akickcount == 0) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, ci->name);
            return MOD_CONT;
        }

        na = findnick(mask);
        nc = (na ? na->nc : NULL);

        for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
            if (!(akick->flags & AK_USED) || (akick->flags & AK_ISNICK))
                continue;
            if (!stricmp(akick->u.mask, mask))
                break;
        }

        if (i == ci->akickcount) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_NOT_FOUND, mask,
                        ci->name);
            return MOD_CONT;
        }

        akick->flags &= ~AK_STUCK;
        alog("%s: %s!%s@%s unset STICK on akick %s on %s",
             s_ChanServ, u->nick, u->username, u->host, akick->u.mask, ci->name);
        notice_lang(s_ChanServ, u, CHAN_AKICK_UNSTUCK, akick->u.mask,
                    ci->name);

    } else if (stricmp(cmd, "DEL") == 0) {
        int deleted, a, b;

        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_DISABLED);
            return MOD_CONT;
        }

        if (ci->akickcount == 0) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, chan);
            return MOD_CONT;
        }

        /* Special case: is it a number/list?  Only do search if it isn't. */
        if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)) {
            int count, last = -1;
            deleted = process_numlist(mask, &count, akick_del_callback, u,
                                      ci, &last);
            if (!deleted) {
                if (count == 1) {
                    notice_lang(s_ChanServ, u, CHAN_AKICK_NO_SUCH_ENTRY,
                                last, ci->name);
                } else {
                    notice_lang(s_ChanServ, u, CHAN_AKICK_NO_MATCH,
                                ci->name);
                }
            } else if (deleted == 1) {
                alog("%s: %s!%s@%s deleted 1 akick on %s",
                     s_ChanServ, u->nick, u->username, u->host, ci->name);
                notice_lang(s_ChanServ, u, CHAN_AKICK_DELETED_ONE,
                            ci->name);
            } else {
                alog("%s: %s!%s@%s deleted %d akicks on %s",
                     s_ChanServ, u->nick, u->username, u->host, deleted,
                     ci->name);
                notice_lang(s_ChanServ, u, CHAN_AKICK_DELETED_SEVERAL,
                            deleted, ci->name);
            }
        } else {
            NickAlias *na = findnick(mask);
            NickCore *nc = (na ? na->nc : NULL);

            for (akick = ci->akick, i = 0; i < ci->akickcount;
                 akick++, i++) {
                if (!(akick->flags & AK_USED))
                    continue;
                if (((akick->flags & AK_ISNICK) && akick->u.nc == nc)
                    || (!(akick->flags & AK_ISNICK)
                        && stricmp(akick->u.mask, mask) == 0))
                    break;
            }
            if (i == ci->akickcount) {
                notice_lang(s_ChanServ, u, CHAN_AKICK_NOT_FOUND, mask,
                            chan);
                return MOD_CONT;
            }
            alog("%s: %s!%s@%s deleted akick %s on %s",
                 s_ChanServ, u->nick, u->username, u->host, mask, chan);
            notice_lang(s_ChanServ, u, CHAN_AKICK_DELETED, mask, chan);
            akick_del(u, akick);
            deleted = 1;
        }
        if (deleted) {
            /* Reordering - DrStein */
            for (b = 0; b < ci->akickcount; b++) {
                if (ci->akick[b].flags & AK_USED) {
                    for (a = 0; a < ci->akickcount; a++) {
                        if (a > b)
                            break;
                        if (!(ci->akick[a].flags & AK_USED)) {
                            ci->akick[a].flags = ci->akick[b].flags;
                            if (ci->akick[b].flags & AK_ISNICK) {
                                ci->akick[a].u.nc = ci->akick[b].u.nc;
                            } else {
                                ci->akick[a].u.mask =
                                    sstrdup(ci->akick[b].u.mask);
                            }
                            /* maybe we should first check whether there
                               is a reason before we sstdrup it -Certus */
                            if (ci->akick[b].reason)
                                ci->akick[a].reason =
                                    sstrdup(ci->akick[b].reason);
                            else
                                ci->akick[a].reason = NULL;
                            ci->akick[a].creator =
                                sstrdup(ci->akick[b].creator);
                            ci->akick[a].addtime = ci->akick[b].addtime;

                            akick_del(u, &ci->akick[b]);
                            break;
                        }
                    }
                }
            }
            /* After reordering only the entries at the end could still be empty.
             * We ll free the places no longer in use... - Viper */
            for (i = ci->akickcount - 1; i >= 0; i--) {
                if (ci->akick[i].flags & AK_USED)
                    break;

                ci->akickcount--;
            }
            ci->akick =
                srealloc(ci->akick,sizeof(AutoKick) * ci->akickcount);
        }
    } else if (stricmp(cmd, "LIST") == 0) {
        int sent_header = 0;

        if (ci->akickcount == 0) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, chan);
            return MOD_CONT;
        }
        if (mask && isdigit(*mask) &&
            strspn(mask, "1234567890,-") == strlen(mask)) {
            process_numlist(mask, NULL, akick_list_callback, u, ci,
                            &sent_header);
        } else {
            for (akick = ci->akick, i = 0; i < ci->akickcount;
                 akick++, i++) {
                if (!(akick->flags & AK_USED))
                    continue;
                if (mask) {
                    if (!(akick->flags & AK_ISNICK)
                        && !match_wild_nocase(mask, akick->u.mask))
                        continue;
                    if ((akick->flags & AK_ISNICK)
                        && !match_wild_nocase(mask, akick->u.nc->display))
                        continue;
                }
                akick_list(u, i, ci, &sent_header);
            }
        }
        if (!sent_header)
            notice_lang(s_ChanServ, u, CHAN_AKICK_NO_MATCH, chan);

    } else if (stricmp(cmd, "VIEW") == 0) {
        int sent_header = 0;

        if (ci->akickcount == 0) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, chan);
            return MOD_CONT;
        }
        if (mask && isdigit(*mask) &&
            strspn(mask, "1234567890,-") == strlen(mask)) {
            process_numlist(mask, NULL, akick_view_callback, u, ci,
                            &sent_header);
        } else {
            for (akick = ci->akick, i = 0; i < ci->akickcount;
                 akick++, i++) {
                if (!(akick->flags & AK_USED))
                    continue;
                if (mask) {
                    if (!(akick->flags & AK_ISNICK)
                        && !match_wild_nocase(mask, akick->u.mask))
                        continue;
                    if ((akick->flags & AK_ISNICK)
                        && !match_wild_nocase(mask, akick->u.nc->display))
                        continue;
                }
                akick_view(u, i, ci, &sent_header);
            }
        }
        if (!sent_header)
            notice_lang(s_ChanServ, u, CHAN_AKICK_NO_MATCH, chan);

    } else if (stricmp(cmd, "ENFORCE") == 0) {
        Channel *c = findchan(ci->name);
        struct c_userlist *cu = NULL;
        struct c_userlist *next;
        char *argv[3];
        int count = 0;

        if (!c) {
            notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, ci->name);
            return MOD_CONT;
        }

        cu = c->users;

        while (cu) {
            next = cu->next;
            if (check_kick(cu->user, c->name, c->creation_time)) {
                argv[0] = sstrdup(c->name);
                argv[1] = sstrdup(cu->user->nick);
                argv[2] = sstrdup(CSAutokickReason);

                do_kick(s_ChanServ, 3, argv);

                free(argv[2]);
                free(argv[1]);
                free(argv[0]);

                count++;
            }
            cu = next;
        }

        notice_lang(s_ChanServ, u, CHAN_AKICK_ENFORCE_DONE, chan, count);

    } else if (stricmp(cmd, "CLEAR") == 0) {

        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_AKICK_DISABLED);
            return MOD_CONT;
        }

        for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
            if (!(akick->flags & AK_USED))
                continue;
            akick_del(u, akick);
        }

        free(ci->akick);
        ci->akick = NULL;
        ci->akickcount = 0;

        alog("%s: %s!%s@%s cleared akicks on %s",
              s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_AKICK_CLEAR, ci->name);

    } else {
        syntax_error(s_ChanServ, u, "AKICK", CHAN_AKICK_SYNTAX);
    }
    return MOD_CONT;
}


int get_access_nc(NickCore *nc, ChannelInfo *ci)
{
    ChanAccess *access;
    if (!ci || !nc)
        return 0;

    if ((access = get_access_entry(nc, ci)))
        return access->level;
    return 0;
}

/* EOF */
