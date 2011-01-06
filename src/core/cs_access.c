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


int do_access(User * u);
int do_levels(User * u);

void myChanServHelp(User * u);

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
    moduleAddVersion
        (VERSION_STRING);
    moduleSetType(CORE);

    c = createCommand("ACCESS", do_access, NULL, CHAN_HELP_ACCESS, -1, -1,
                      -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("ACCESS LEVELS", NULL, NULL, CHAN_HELP_ACCESS_LEVELS,
                      -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("LEVELS", do_levels, NULL, CHAN_HELP_LEVELS, -1, -1,
                      -1, -1);
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
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_ACCESS);
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_LEVELS);
}


static int access_del(User * u, ChannelInfo *ci, ChanAccess * access, int *perm, int uacc)
{
	char *nick;
    if (!access->in_use)
        return 0;
    if (!is_services_admin(u) && uacc <= access->level) {
        (*perm)++;
        return 0;
    }
    nick = access->nc->display;
    access->nc = NULL;
    access->in_use = 0;
    send_event(EVENT_ACCESS_DEL, 3, ci->name, u->nick, nick);
    return 1;
}

static int access_del_callback(User * u, int num, va_list args)
{
    ChannelInfo *ci = va_arg(args, ChannelInfo *);
    int *last = va_arg(args, int *);
    int *perm = va_arg(args, int *);
    int uacc = va_arg(args, int);
    if (num < 1 || num > ci->accesscount)
        return 0;
    *last = num;
    return access_del(u, ci, &ci->access[num - 1], perm, uacc);
}


static int access_list(User * u, int index, ChannelInfo * ci,
                       int *sent_header)
{
    ChanAccess *access = &ci->access[index];
    const char *xop;

    if (!access->in_use)
        return 0;

    if (!*sent_header) {
        notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_HEADER, ci->name);
        *sent_header = 1;
    }

    if (ci->flags & CI_XOP) {
        xop = get_xop_level(access->level);
        notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_XOP_FORMAT, index + 1,
                    xop, access->nc->display);
    } else {
        notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_AXS_FORMAT, index + 1,
                    access->level, access->nc->display);
    }
    return 1;
}

static int access_list_callback(User * u, int num, va_list args)
{
    ChannelInfo *ci = va_arg(args, ChannelInfo *);
    int *sent_header = va_arg(args, int *);
    if (num < 1 || num > ci->accesscount)
        return 0;
    return access_list(u, num - 1, ci, sent_header);
}


/**
 * The /cs access command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_access(User * u)
{
    char *chan = strtok(NULL, " ");
    char *cmd = strtok(NULL, " ");
    char *nick = strtok(NULL, " ");
    char *s = strtok(NULL, " ");
    char event_access[BUFSIZE];

    ChannelInfo *ci;
    NickAlias *na = NULL;
    NickCore *nc;
    ChanAccess *access;

    int i;
    int level = 0, ulev;
    int is_clear = (cmd && stricmp(cmd, "CLEAR") == 0);
    int is_list = (cmd && stricmp(cmd, "LIST") == 0);
    int is_servadmin = is_services_admin(u);

    /* If LIST, we don't *require* any parameters, but we can take any.
     * If DEL, we require a nick and no level.
     * Else (ADD), we require a level (which implies a nick). */
    if (!cmd || ((is_list || is_clear) ? 0 :
                 (stricmp(cmd, "DEL") == 0) ? (!nick || s) : !s)) {
        syntax_error(s_ChanServ, u, "ACCESS", CHAN_ACCESS_SYNTAX);
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
        /* We still allow LIST and CLEAR in xOP mode, but not others */
    } else if ((ci->flags & CI_XOP) && !is_list && !is_clear) {
		if (ircd->halfop)
	        notice_lang(s_ChanServ, u, CHAN_ACCESS_XOP_HOP, s_ChanServ);
		else
	        notice_lang(s_ChanServ, u, CHAN_ACCESS_XOP, s_ChanServ);
    } else if (((is_list && !check_access(u, ci, CA_ACCESS_LIST))
                || (!is_list && !check_access(u, ci, CA_ACCESS_CHANGE)))
               && !is_servadmin) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (stricmp(cmd, "ADD") == 0) {
        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_DISABLED);
            return MOD_CONT;
        }

        level = atoi(s);
        ulev = get_access(u, ci);

        if (!is_servadmin && level >= ulev) {
            notice_lang(s_ChanServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        if (level == 0) {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_NONZERO);
            return MOD_CONT;
        } else if (level <= ACCESS_INVALID || level >= ACCESS_FOUNDER) {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_RANGE,
                        ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
            return MOD_CONT;
        }

        na = findnick(nick);
        if (!na) {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_NICKS_ONLY);
            return MOD_CONT;
        }
        if (na->status & NS_VERBOTEN) {
            notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, nick);
            return MOD_CONT;
        }

        nc = na->nc;
        for (access = ci->access, i = 0; i < ci->accesscount;
             access++, i++) {
            if (access->nc == nc) {
                /* Don't allow lowering from a level >= ulev */
                if (!is_servadmin && access->level >= ulev) {
                    notice_lang(s_ChanServ, u, PERMISSION_DENIED);
                    return MOD_CONT;
                }
                if (access->level == level) {
                    notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_UNCHANGED,
                                access->nc->display, chan, level);
                    return MOD_CONT;
                }
                access->level = level;
                snprintf(event_access, BUFSIZE, "%d", access->level);
                send_event(EVENT_ACCESS_CHANGE, 4, ci->name, u->nick,
                           na->nick, event_access);
                alog("%s: %s!%s@%s (level %d) set access level %d to %s (group %s) on channel %s", s_ChanServ, u->nick, u->username, u->host, ulev, access->level, na->nick, nc->display, ci->name);
                notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_CHANGED,
                            access->nc->display, chan, level);
                return MOD_CONT;
            }
        }

        if (i < CSAccessMax) {
            ci->accesscount++;
            ci->access =
                srealloc(ci->access,
                        sizeof(ChanAccess) * ci->accesscount);
        } else {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_REACHED_LIMIT,
                        CSAccessMax);
            return MOD_CONT;
        }

        access = &ci->access[i];
        access->nc = nc;
        access->in_use = 1;
        access->level = level;
        access->last_seen = 0;

        snprintf(event_access, BUFSIZE, "%d", access->level);
        send_event(EVENT_ACCESS_ADD, 4, ci->name, u->nick, na->nick,
                   event_access);
        alog("%s: %s!%s@%s (level %d) set access level %d to %s (group %s) on channel %s", s_ChanServ, u->nick, u->username, u->host, ulev, access->level, na->nick, nc->display, ci->name);
        notice_lang(s_ChanServ, u, CHAN_ACCESS_ADDED, nc->display,
                    ci->name, access->level);
    } else if (stricmp(cmd, "DEL") == 0) {
        int deleted;
        
	if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_DISABLED);
            return MOD_CONT;
        }

        if (ci->accesscount == 0) {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_EMPTY, chan);
            return MOD_CONT;
        }

	/* Clean the access list to make sure every thing is in use */
	CleanAccess(ci);

        /* Special case: is it a number/list?  Only do search if it isn't. */
        if (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick)) {
            int count, last = -1, perm = 0;
            deleted = process_numlist(nick, &count, access_del_callback, u,
                                      ci, &last, &perm, get_access(u, ci));
            if (!deleted) {
                if (perm) {
                    notice_lang(s_ChanServ, u, PERMISSION_DENIED);
                } else if (count == 1) {
                    last = atoi(nick);
                    notice_lang(s_ChanServ, u, CHAN_ACCESS_NO_SUCH_ENTRY,
                                last, ci->name);
                } else {
                    notice_lang(s_ChanServ, u, CHAN_ACCESS_NO_MATCH,
                                ci->name);
                }
			} else {
				alog("%s: %s!%s@%s (level %d) deleted access of user%s %s on %s",
					s_ChanServ, u->nick, u->username, u->host, get_access(u, ci), (deleted == 1 ? "" : "s"), nick, chan);
				if (deleted == 1)
					notice_lang(s_ChanServ, u, CHAN_ACCESS_DELETED_ONE,
                            ci->name);
				else
					notice_lang(s_ChanServ, u, CHAN_ACCESS_DELETED_SEVERAL,
								deleted, ci->name);
			}
        } else {
            na = findnick(nick);
            if (!na) {
                notice_lang(s_ChanServ, u, NICK_X_NOT_REGISTERED, nick);
                return MOD_CONT;
            }
            nc = na->nc;
            for (i = 0; i < ci->accesscount; i++) {
                if (ci->access[i].nc == nc)
                    break;
            }
            if (i == ci->accesscount) {
                notice_lang(s_ChanServ, u, CHAN_ACCESS_NOT_FOUND, nick,
                            chan);
                return MOD_CONT;
            }
            access = &ci->access[i];
            if (!is_servadmin && get_access(u, ci) <= access->level) {
                deleted = 0;
                notice_lang(s_ChanServ, u, PERMISSION_DENIED);
            } else {
                notice_lang(s_ChanServ, u, CHAN_ACCESS_DELETED,
                            access->nc->display, ci->name);
                alog("%s: %s!%s@%s (level %d) deleted access of %s (group %s) on %s", s_ChanServ, u->nick, u->username, u->host, get_access(u, ci), na->nick, access->nc->display, chan);
                access->nc = NULL;
                access->in_use = 0;
                deleted = 1;
            }
        }

        CleanAccess(ci);

            /* We don't know the nick if someone used numbers, so we trigger the event without
             * nick param. We just do this once, even if someone enters a range. -Certus */
            if (na)
                send_event(EVENT_ACCESS_DEL, 3, ci->name, u->nick, na->nick);
            else
                send_event(EVENT_ACCESS_DEL, 2, ci->name, u->nick);
    } else if (stricmp(cmd, "LIST") == 0) {
        int sent_header = 0;

        if (ci->accesscount == 0) {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_EMPTY, chan);
            return MOD_CONT;
        }

        CleanAccess(ci);	

        if (nick && strspn(nick, "1234567890,-") == strlen(nick)) {
            process_numlist(nick, NULL, access_list_callback, u, ci,
                            &sent_header);
        } else {
            for (i = 0; i < ci->accesscount; i++) {
                if (nick && ci->access[i].nc
                    && !match_wild_nocase(nick, ci->access[i].nc->display))
                    continue;
                access_list(u, i, ci, &sent_header);
            }
        }
        if (!sent_header) {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_NO_MATCH, chan);
        } else {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_FOOTER, ci->name);
        }
    } else if (stricmp(cmd, "CLEAR") == 0) {

        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_DISABLED);
            return MOD_CONT;
        }

        if (!is_servadmin && !is_founder(u, ci)) {
            notice_lang(s_ChanServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        free(ci->access);
        ci->access = NULL;
        ci->accesscount = 0;
        
        send_event(EVENT_ACCESS_CLEAR, 2, ci->name, u->nick);

        notice_lang(s_ChanServ, u, CHAN_ACCESS_CLEAR, ci->name);
        alog("%s: %s!%s@%s (level %d) cleared access list on %s",
             s_ChanServ, u->nick, u->username, u->host,
             get_access(u, ci), chan);

    } else {
        syntax_error(s_ChanServ, u, "ACCESS", CHAN_ACCESS_SYNTAX);
    }
    return MOD_CONT;
}


int do_levels(User * u)
{
    char *chan = strtok(NULL, " ");
    char *cmd = strtok(NULL, " ");
    char *what = strtok(NULL, " ");
    char *s = strtok(NULL, " ");
    char *error;

    ChannelInfo *ci;
    int level;
    int i;

    /* If SET, we want two extra parameters; if DIS[ABLE] or FOUNDER, we want only
     * one; else, we want none.
     */
    if (!cmd
        || ((stricmp(cmd, "SET") == 0) ? !s
            : ((strnicmp(cmd, "DIS", 3) == 0)) ? (!what || s) : !!what)) {
        syntax_error(s_ChanServ, u, "LEVELS", CHAN_LEVELS_SYNTAX);
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (ci->flags & CI_XOP) {
        notice_lang(s_ChanServ, u, CHAN_LEVELS_XOP);
    } else if (!is_founder(u, ci) && !is_services_admin(u)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (stricmp(cmd, "SET") == 0) {
        level = strtol(s, &error, 10);

        if (*error != '\0') {
            syntax_error(s_ChanServ, u, "LEVELS", CHAN_LEVELS_SYNTAX);
            return MOD_CONT;
        }

        if (level <= ACCESS_INVALID || level >= ACCESS_FOUNDER) {
            notice_lang(s_ChanServ, u, CHAN_LEVELS_RANGE,
                        ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
            return MOD_CONT;
        }

        for (i = 0; levelinfo[i].what >= 0; i++) {
            if (stricmp(levelinfo[i].name, what) == 0) {
                ci->levels[levelinfo[i].what] = level;

                alog("%s: %s!%s@%s set level %s on channel %s to %d",
                     s_ChanServ, u->nick, u->username, u->host,
                     levelinfo[i].name, ci->name, level);
                notice_lang(s_ChanServ, u, CHAN_LEVELS_CHANGED,
                            levelinfo[i].name, chan, level);
                return MOD_CONT;
            }
        }

        notice_lang(s_ChanServ, u, CHAN_LEVELS_UNKNOWN, what, s_ChanServ);

    } else if (stricmp(cmd, "DIS") == 0 || stricmp(cmd, "DISABLE") == 0) {
        for (i = 0; levelinfo[i].what >= 0; i++) {
            if (stricmp(levelinfo[i].name, what) == 0) {
                ci->levels[levelinfo[i].what] = ACCESS_INVALID;

                alog("%s: %s!%s@%s disabled level %s on channel %s",
                     s_ChanServ, u->nick, u->username, u->host,
                     levelinfo[i].name, ci->name);
                notice_lang(s_ChanServ, u, CHAN_LEVELS_DISABLED,
                            levelinfo[i].name, chan);
                return MOD_CONT;
            }
        }

        notice_lang(s_ChanServ, u, CHAN_LEVELS_UNKNOWN, what, s_ChanServ);
    } else if (stricmp(cmd, "LIST") == 0) {
        int i;

        notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_HEADER, chan);

        if (!levelinfo_maxwidth) {
            for (i = 0; levelinfo[i].what >= 0; i++) {
                int len = strlen(levelinfo[i].name);
                if (len > levelinfo_maxwidth)
                    levelinfo_maxwidth = len;
            }
        }

        for (i = 0; levelinfo[i].what >= 0; i++) {
            int j = ci->levels[levelinfo[i].what];

            if (j == ACCESS_INVALID) {
                j = levelinfo[i].what;

                if (j == CA_AUTOOP || j == CA_AUTODEOP || j == CA_AUTOVOICE
                    || j == CA_NOJOIN) {
                    notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_DISABLED,
                                levelinfo_maxwidth, levelinfo[i].name);
                } else {
                    notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_DISABLED,
                                levelinfo_maxwidth, levelinfo[i].name);
                }
            } else if (j == ACCESS_FOUNDER) {
                notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_FOUNDER,
                            levelinfo_maxwidth, levelinfo[i].name);
            } else {
                notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_NORMAL,
                            levelinfo_maxwidth, levelinfo[i].name, j);
            }
        }

    } else if (stricmp(cmd, "RESET") == 0) {
        reset_levels(ci);

        alog("%s: %s!%s@%s reset levels definitions on channel %s",
             s_ChanServ, u->nick, u->username, u->host, ci->name);
        notice_lang(s_ChanServ, u, CHAN_LEVELS_RESET, chan);
    } else {
        syntax_error(s_ChanServ, u, "LEVELS", CHAN_LEVELS_SYNTAX);
    }
    return MOD_CONT;
}
