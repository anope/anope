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

int do_xop(User * u, char *xname, int xlev, int *xmsgs);
int do_aop(User * u);
int do_hop(User * u);
int do_sop(User * u);
int do_vop(User * u);

void myChanServHelp(User * u);

int xop_msgs[4][14] = {
    {CHAN_AOP_SYNTAX,
     CHAN_AOP_DISABLED,
     CHAN_AOP_NICKS_ONLY,
     CHAN_AOP_ADDED,
     CHAN_AOP_MOVED,
     CHAN_AOP_NO_SUCH_ENTRY,
     CHAN_AOP_NOT_FOUND,
     CHAN_AOP_NO_MATCH,
     CHAN_AOP_DELETED,
     CHAN_AOP_DELETED_ONE,
     CHAN_AOP_DELETED_SEVERAL,
     CHAN_AOP_LIST_EMPTY,
     CHAN_AOP_LIST_HEADER,
     CHAN_AOP_CLEAR},
    {CHAN_SOP_SYNTAX,
     CHAN_SOP_DISABLED,
     CHAN_SOP_NICKS_ONLY,
     CHAN_SOP_ADDED,
     CHAN_SOP_MOVED,
     CHAN_SOP_NO_SUCH_ENTRY,
     CHAN_SOP_NOT_FOUND,
     CHAN_SOP_NO_MATCH,
     CHAN_SOP_DELETED,
     CHAN_SOP_DELETED_ONE,
     CHAN_SOP_DELETED_SEVERAL,
     CHAN_SOP_LIST_EMPTY,
     CHAN_SOP_LIST_HEADER,
     CHAN_SOP_CLEAR},
    {CHAN_VOP_SYNTAX,
     CHAN_VOP_DISABLED,
     CHAN_VOP_NICKS_ONLY,
     CHAN_VOP_ADDED,
     CHAN_VOP_MOVED,
     CHAN_VOP_NO_SUCH_ENTRY,
     CHAN_VOP_NOT_FOUND,
     CHAN_VOP_NO_MATCH,
     CHAN_VOP_DELETED,
     CHAN_VOP_DELETED_ONE,
     CHAN_VOP_DELETED_SEVERAL,
     CHAN_VOP_LIST_EMPTY,
     CHAN_VOP_LIST_HEADER,
     CHAN_VOP_CLEAR},
    {CHAN_HOP_SYNTAX,
     CHAN_HOP_DISABLED,
     CHAN_HOP_NICKS_ONLY,
     CHAN_HOP_ADDED,
     CHAN_HOP_MOVED,
     CHAN_HOP_NO_SUCH_ENTRY,
     CHAN_HOP_NOT_FOUND,
     CHAN_HOP_NO_MATCH,
     CHAN_HOP_DELETED,
     CHAN_HOP_DELETED_ONE,
     CHAN_HOP_DELETED_SEVERAL,
     CHAN_HOP_LIST_EMPTY,
     CHAN_HOP_LIST_HEADER,
     CHAN_HOP_CLEAR}
};

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

    c = createCommand("AOP", do_aop, NULL, CHAN_HELP_AOP, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("HOP", do_hop, NULL, CHAN_HELP_HOP, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("SOP", do_sop, NULL, CHAN_HELP_SOP, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("VOP", do_vop, NULL, CHAN_HELP_VOP, -1, -1, -1, -1);
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
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_SOP);
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_AOP);
    if (ircd->halfop) {
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_HOP);
    }
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_VOP);
}

/**
 * The /cs xop command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_aop(User * u)
{
    return do_xop(u, "AOP", ACCESS_AOP, xop_msgs[0]);
}

/*************************************************************************/

int do_hop(User * u)
{
    if (ircd->halfop)
        return do_xop(u, "HOP", ACCESS_HOP, xop_msgs[3]);
    return MOD_CONT;
}

/*************************************************************************/

int do_sop(User * u)
{
    return do_xop(u, "SOP", ACCESS_SOP, xop_msgs[1]);
}

/*************************************************************************/

int do_vop(User * u)
{
    return do_xop(u, "VOP", ACCESS_VOP, xop_msgs[2]);
}

/* `last' is set to the last index this routine was called with
 * `perm' is incremented whenever a permission-denied error occurs
 */

int xop_del(User * u, ChannelInfo * ci, ChanAccess * access, int *perm, int uacc, int xlev)
{
    char *nick;
    if (!access->in_use || !access->nc || access->level != xlev)
        return 0;
    nick = access->nc->display;
    if (!is_services_admin(u) && uacc <= access->level) {
        (*perm)++;
        return 0;
    }
    access->nc = NULL;
    access->in_use = 0;
    send_event(EVENT_ACCESS_DEL, 3, ci->name, u->nick, nick);
    return 1;
}

int xop_del_callback(User * u, int num, va_list args)
{
    ChannelInfo *ci = va_arg(args, ChannelInfo *);
    int *last = va_arg(args, int *);
    int *perm = va_arg(args, int *);
    int uacc = va_arg(args, int);
    int xlev = va_arg(args, int);

    if (num < 1 || num > ci->accesscount)
        return 0;
    *last = num;

    return xop_del(u, ci, &ci->access[num - 1], perm, uacc, xlev);
}


int xop_list(User * u, int index, ChannelInfo * ci,
             int *sent_header, int xlev, int xmsg)
{
    ChanAccess *access = &ci->access[index];

    if (!access->in_use || access->level != xlev)
        return 0;

    if (!*sent_header) {
        notice_lang(s_ChanServ, u, xmsg, ci->name);
        *sent_header = 1;
    }

    notice_lang(s_ChanServ, u, CHAN_XOP_LIST_FORMAT, index + 1,
                access->nc->display);
    return 1;
}

int xop_list_callback(User * u, int num, va_list args)
{
    ChannelInfo *ci = va_arg(args, ChannelInfo *);
    int *sent_header = va_arg(args, int *);
    int xlev = va_arg(args, int);
    int xmsg = va_arg(args, int);

    if (num < 1 || num > ci->accesscount)
        return 0;

    return xop_list(u, num - 1, ci, sent_header, xlev, xmsg);
}


int do_xop(User * u, char *xname, int xlev, int *xmsgs)
{
    char *chan = strtok(NULL, " ");
    char *cmd = strtok(NULL, " ");
    char *nick = strtok(NULL, " ");
    char event_access[BUFSIZE];

    ChannelInfo *ci;
    NickAlias *na;
    NickCore *nc;

    int i;
    int change = 0;
    short ulev;
    int is_list = (cmd && stricmp(cmd, "LIST") == 0);
    int is_servadmin = is_services_admin(u);
    ChanAccess *access;

    /* If CLEAR, we don't need any parameters.
     * If LIST, we don't *require* any parameters, but we can take any.
     * If DEL or ADD we require a nick. */
    if (!cmd || ((is_list || !stricmp(cmd, "CLEAR")) ? 0 : !nick)) {
        syntax_error(s_ChanServ, u, xname, xmsgs[0]);
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!(ci->flags & CI_XOP)) {
        notice_lang(s_ChanServ, u, CHAN_XOP_ACCESS, s_ChanServ);
    } else if (stricmp(cmd, "ADD") == 0) {
        if (readonly) {
            notice_lang(s_ChanServ, u, xmsgs[1]);
            return MOD_CONT;
        }

        ulev = get_access(u, ci);

        if ((xlev >= ulev || ulev < ACCESS_AOP) && !is_servadmin) {
            notice_lang(s_ChanServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        na = findnick(nick);
        if (!na) {
            notice_lang(s_ChanServ, u, xmsgs[2]);
            return MOD_CONT;
        } else if (na->status & NS_VERBOTEN) {
            notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, na->nick);
            return MOD_CONT;
        }

        nc = na->nc;
        for (access = ci->access, i = 0; i < ci->accesscount;
             access++, i++) {
            if (access->nc == nc) {
                /**
                 * Patch provided by PopCorn to prevert AOP's reducing SOP's levels
                 **/
                if ((access->level >= ulev) && (!is_servadmin)) {
                    notice_lang(s_ChanServ, u, PERMISSION_DENIED);
                    return MOD_CONT;
                }
                change++;
                break;
            }
        }

        if (!change) {
            if (i < CSAccessMax) {
                ci->accesscount++;
                ci->access =
                    srealloc(ci->access,
                             sizeof(ChanAccess) * ci->accesscount);
            } else {
                notice_lang(s_ChanServ, u, CHAN_XOP_REACHED_LIMIT,
                            CSAccessMax);
                return MOD_CONT;
            }

            access = &ci->access[i];
            access->nc = nc;
        }

        access->in_use = 1;
        access->level = xlev;
        access->last_seen = 0;

        alog("%s: %s!%s@%s (level %d) %s access level %d to %s (group %s) on channel %s", s_ChanServ, u->nick, u->username, u->host, ulev, change ? "changed" : "set", access->level, na->nick, nc->display, ci->name);

        snprintf(event_access, BUFSIZE, "%d", access->level);

        if (!change) {
            send_event(EVENT_ACCESS_ADD, 4, ci->name, u->nick, na->nick,
                       event_access);
            notice_lang(s_ChanServ, u, xmsgs[3], access->nc->display,
                        ci->name);
        } else {
            send_event(EVENT_ACCESS_CHANGE, 4, ci->name, u->nick, na->nick,
                       event_access);
            notice_lang(s_ChanServ, u, xmsgs[4], access->nc->display,
                        ci->name);
        }

    } else if (stricmp(cmd, "DEL") == 0) {
        int deleted;
        if (readonly) {
            notice_lang(s_ChanServ, u, xmsgs[1]);
            return MOD_CONT;
        }

        if (ci->accesscount == 0) {
            notice_lang(s_ChanServ, u, xmsgs[11], chan);
            return MOD_CONT;
        }

        ulev = get_access(u, ci);

        if ((xlev >= ulev || ulev < ACCESS_AOP) && !is_servadmin) {
            notice_lang(s_ChanServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        /* Special case: is it a number/list?  Only do search if it isn't. */
        if (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick)) {
            int count, last = -1, perm = 0;
            deleted =
                process_numlist(nick, &count, xop_del_callback, u, ci,
                                &last, &perm, ulev, xlev);
            if (!deleted) {
                if (perm) {
                    notice_lang(s_ChanServ, u, PERMISSION_DENIED);
                } else if (count == 1) {
                    last = atoi(nick);
                    notice_lang(s_ChanServ, u, xmsgs[5], last, ci->name);
                } else {
                    notice_lang(s_ChanServ, u, xmsgs[7], ci->name);
                }
            } else if (deleted == 1) {
                alog("%s: %s!%s@%s (level %d) deleted access of user %s on %s", s_ChanServ, u->nick, u->username, u->host, get_access(u, ci), nick, ci->name);
                notice_lang(s_ChanServ, u, xmsgs[9], ci->name);
            } else {
                alog("%s: %s!%s@%s (level %d) deleted access of users %s on %s", s_ChanServ, u->nick, u->username, u->host, get_access(u, ci), nick, ci->name);
                notice_lang(s_ChanServ, u, xmsgs[10], deleted, ci->name);
            }
        } else {
            na = findnick(nick);
            if (!na) {
                notice_lang(s_ChanServ, u, NICK_X_NOT_REGISTERED, nick);
                return MOD_CONT;
            }
            nc = na->nc;

            for (i = 0; i < ci->accesscount; i++)
                if (ci->access[i].nc == nc && ci->access[i].level == xlev)
                    break;

            if (i == ci->accesscount) {
                notice_lang(s_ChanServ, u, xmsgs[6], nick, chan);
                return MOD_CONT;
            }

            access = &ci->access[i];
            if (!is_servadmin && ulev <= access->level) {
                deleted = 0;
                notice_lang(s_ChanServ, u, PERMISSION_DENIED);
            } else {
                alog("%s: %s!%s@%s (level %d) deleted access of %s on %s", s_ChanServ, u->nick, u->username, u->host, get_access(u, ci), access->nc->display, ci->name);
                notice_lang(s_ChanServ, u, xmsgs[8], access->nc->display,
                            ci->name);
                access->nc = NULL;
                access->in_use = 0;
                send_event(EVENT_ACCESS_DEL, 3, ci->name, u->nick,
                           na->nick);
                deleted = 1;
            }
        }
        if (deleted)
            CleanAccess(ci);
    } else if (stricmp(cmd, "LIST") == 0) {
        int sent_header = 0;

        ulev = get_access(u, ci);

        if (!is_servadmin && ulev < ACCESS_AOP) {
            notice_lang(s_ChanServ, u, ACCESS_DENIED);
            return MOD_CONT;
        }

        if (ci->accesscount == 0) {
            notice_lang(s_ChanServ, u, xmsgs[11], ci->name);
            return MOD_CONT;
        }

        if (nick && strspn(nick, "1234567890,-") == strlen(nick)) {
            process_numlist(nick, NULL, xop_list_callback, u, ci,
                            &sent_header, xlev, xmsgs[12]);
        } else {
            for (i = 0; i < ci->accesscount; i++) {
                if (nick && ci->access[i].nc
                    && !match_wild_nocase(nick, ci->access[i].nc->display))
                    continue;
                xop_list(u, i, ci, &sent_header, xlev, xmsgs[12]);
            }
        }
        if (!sent_header)
            notice_lang(s_ChanServ, u, xmsgs[7], chan);
    } else if (stricmp(cmd, "CLEAR") == 0) {
        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_DISABLED);
            return MOD_CONT;
        }

        if (ci->accesscount == 0) {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_EMPTY, chan);
            return MOD_CONT;
        }

        if (!is_servadmin && !is_founder(u, ci)) {
            notice_lang(s_ChanServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        for (i = 0; i < ci->accesscount; i++) {
            if (ci->access[i].in_use && ci->access[i].level == xlev) {
                ci->access[i].nc = NULL;
                ci->access[i].in_use = 0;
            }
        }

        CleanAccess(ci);

        alog("%s: %s!%s@%s cleared the %s list of %s", s_ChanServ, u->nick, u->username, u->host, xname, ci->name);

        send_event(EVENT_ACCESS_CLEAR, 2, ci->name, u->nick);
        
        notice_lang(s_ChanServ, u, xmsgs[13], ci->name);
    } else {
        syntax_error(s_ChanServ, u, xname, xmsgs[0]);
    }
    return MOD_CONT;
}
