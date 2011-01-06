/* MemoServ core functions
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

int do_set(User * u);
int do_set_notify(User * u, MemoInfo * mi, char *param);
int do_set_limit(User * u, MemoInfo * mi, char *param);
void myMemoServHelp(User * u);

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

    c = createCommand("SET", do_set, NULL, MEMO_HELP_SET, -1, -1, -1, -1);
    moduleAddCommand(MEMOSERV, c, MOD_UNIQUE);

    c = createCommand("SET NOTIFY", NULL, NULL, MEMO_HELP_SET_NOTIFY, -1,
                      -1, -1, -1);
    moduleAddCommand(MEMOSERV, c, MOD_UNIQUE);

    c = createCommand("SET LIMIT", NULL, NULL, -1, MEMO_HELP_SET_LIMIT,
                      MEMO_SERVADMIN_HELP_SET_LIMIT,
                      MEMO_SERVADMIN_HELP_SET_LIMIT,
                      MEMO_SERVADMIN_HELP_SET_LIMIT);
    c->help_param1 = (char *) (long) MSMaxMemos;
    moduleAddCommand(MEMOSERV, c, MOD_UNIQUE);

    moduleSetMemoHelp(myMemoServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to anopes /hs help output.
 * @param u The user who is requesting help
 **/
void myMemoServHelp(User * u)
{
    notice_lang(s_MemoServ, u, MEMO_HELP_CMD_SET);
}

/**
 * The /ms set command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_set(User * u)
{
    char *cmd = strtok(NULL, " ");
    char *param = strtok(NULL, "");
    MemoInfo *mi = &u->na->nc->memos;

    if (readonly) {
        notice_lang(s_MemoServ, u, MEMO_SET_DISABLED);
        return MOD_CONT;
    }
    if (!param) {
        syntax_error(s_MemoServ, u, "SET", MEMO_SET_SYNTAX);
    } else if (!nick_identified(u)) {
        notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
        return MOD_CONT;
    } else if (stricmp(cmd, "NOTIFY") == 0) {
        do_set_notify(u, mi, param);
    } else if (stricmp(cmd, "LIMIT") == 0) {
        do_set_limit(u, mi, param);
    } else {
        notice_lang(s_MemoServ, u, MEMO_SET_UNKNOWN_OPTION, cmd);
        notice_lang(s_MemoServ, u, MORE_INFO, s_MemoServ, "SET");
    }
    return MOD_CONT;
}

/**
 * The /ms set notify command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_set_notify(User * u, MemoInfo * mi, char *param)
{
    if (stricmp(param, "ON") == 0) {
        u->na->nc->flags |= NI_MEMO_SIGNON | NI_MEMO_RECEIVE;
        alog("%s: %s!%s@%s set notify to ON",
             s_MemoServ, u->nick, u->username, u->host);
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_ON, s_MemoServ);
    } else if (stricmp(param, "LOGON") == 0) {
        u->na->nc->flags |= NI_MEMO_SIGNON;
        u->na->nc->flags &= ~NI_MEMO_RECEIVE;
        alog("%s: %s!%s@%s set notify to LOGON",
             s_MemoServ, u->nick, u->username, u->host);
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_LOGON, s_MemoServ);
    } else if (stricmp(param, "NEW") == 0) {
        u->na->nc->flags &= ~NI_MEMO_SIGNON;
        u->na->nc->flags |= NI_MEMO_RECEIVE;
        alog("%s: %s!%s@%s set notify to NEW",
             s_MemoServ, u->nick, u->username, u->host);
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_NEW, s_MemoServ);
    } else if (stricmp(param, "MAIL") == 0) {
        if (u->na->nc->email) {
            u->na->nc->flags |= NI_MEMO_MAIL;
            alog("%s: %s!%s@%s set notify to MAIL",
                 s_MemoServ, u->nick, u->username, u->host);
            notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_MAIL);
        } else {
            notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_INVALIDMAIL);
        }
    } else if (stricmp(param, "NOMAIL") == 0) {
        u->na->nc->flags &= ~NI_MEMO_MAIL;
        alog("%s: %s!%s@%s set notify to NOMAIL",
             s_MemoServ, u->nick, u->username, u->host);
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_NOMAIL);
    } else if (stricmp(param, "OFF") == 0) {
        u->na->nc->flags &= ~(NI_MEMO_SIGNON | NI_MEMO_RECEIVE | NI_MEMO_MAIL);
        alog("%s: %s!%s@%s set notify to OFF",
             s_MemoServ, u->nick, u->username, u->host);
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_OFF, s_MemoServ);
    } else {
        syntax_error(s_MemoServ, u, "SET NOTIFY", MEMO_SET_NOTIFY_SYNTAX);
    }
    return MOD_CONT;
}

/**
 * The /ms set limit command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_set_limit(User * u, MemoInfo * mi, char *param)
{
    char *p1 = strtok(param, " ");
    char *p2 = strtok(NULL, " ");
    char *p3 = strtok(NULL, " ");
    char *user = NULL, *chan = NULL;
    int32 limit;
    NickAlias *na = u->na;
    ChannelInfo *ci = NULL;
    int is_servadmin = is_services_admin(u);

    if (p1 && *p1 == '#') {
        chan = p1;
        p1 = p2;
        p2 = p3;
        p3 = strtok(NULL, " ");
        if (!(ci = cs_findchan(chan))) {
            notice_lang(s_MemoServ, u, CHAN_X_NOT_REGISTERED, chan);
            return MOD_CONT;
        } else if (ci->flags & CI_VERBOTEN) {
            notice_lang(s_MemoServ, u, CHAN_X_FORBIDDEN, chan);
            return MOD_CONT;
        } else if (!is_servadmin && !check_access(u, ci, CA_MEMO)) {
            notice_lang(s_MemoServ, u, ACCESS_DENIED);
            return MOD_CONT;
        }
        mi = &ci->memos;
    }
    if (is_servadmin) {
        if (p2 && stricmp(p2, "HARD") != 0 && !chan) {
            if (!(na = findnick(p1))) {
                notice_lang(s_MemoServ, u, NICK_X_NOT_REGISTERED, p1);
                return MOD_CONT;
            }
            user = p1;
            mi = &na->nc->memos;
            p1 = p2;
            p2 = p3;
        } else if (!p1) {
            syntax_error(s_MemoServ, u, "SET LIMIT",
                         MEMO_SET_LIMIT_SERVADMIN_SYNTAX);
            return MOD_CONT;
        }
        if ((!isdigit(*p1) && stricmp(p1, "NONE") != 0) ||
            (p2 && stricmp(p2, "HARD") != 0)) {
            syntax_error(s_MemoServ, u, "SET LIMIT",
                         MEMO_SET_LIMIT_SERVADMIN_SYNTAX);
            return MOD_CONT;
        }
        if (chan) {
            if (p2)
                ci->flags |= CI_MEMO_HARDMAX;
            else
                ci->flags &= ~CI_MEMO_HARDMAX;
        } else {
            if (p2)
                na->nc->flags |= NI_MEMO_HARDMAX;
            else
                na->nc->flags &= ~NI_MEMO_HARDMAX;
        }
        limit = atoi(p1);
        if (limit < 0 || limit > 32767) {
            notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_OVERFLOW, 32767);
            limit = 32767;
        }
        if (stricmp(p1, "NONE") == 0)
            limit = -1;
    } else {
        if (!p1 || p2 || !isdigit(*p1)) {
            syntax_error(s_MemoServ, u, "SET LIMIT",
                         MEMO_SET_LIMIT_SYNTAX);
            return MOD_CONT;
        }
        if (chan && (ci->flags & CI_MEMO_HARDMAX)) {
            notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_FORBIDDEN, chan);
            return MOD_CONT;
        } else if (!chan && (na->nc->flags & NI_MEMO_HARDMAX)) {
            notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT_FORBIDDEN);
            return MOD_CONT;
        }
        limit = atoi(p1);
        /* The first character is a digit, but we could still go negative
         * from overflow... watch out! */
        if (limit < 0 || (MSMaxMemos > 0 && limit > MSMaxMemos)) {
            if (chan) {
                notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_TOO_HIGH,
                            chan, MSMaxMemos);
            } else {
                notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT_TOO_HIGH,
                            MSMaxMemos);
            }
            return MOD_CONT;
        } else if (limit > 32767) {
            notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_OVERFLOW, 32767);
            limit = 32767;
        }
    }
    mi->memomax = limit;
    if (limit > 0) {
        if (!chan && na->nc == u->na->nc) {
            alog("%s: %s!%s@%s set their memo limit to %d",
                 s_MemoServ, u->nick, u->username, u->host, limit);
            notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT, limit);
        } else {
            alog("%s: %s!%s@%s set the memo limit for %s to %d",
                 s_MemoServ, u->nick, u->username, u->host,
                 chan ? chan : user, limit);
            notice_lang(s_MemoServ, u, MEMO_SET_LIMIT,
                        chan ? chan : user, limit);
        }
    } else if (limit == 0) {
        if (!chan && na->nc == u->na->nc) {
            alog("%s: %s!%s@%s set their memo limit to 0",
                 s_MemoServ, u->nick, u->username, u->host);
            notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT_ZERO);
        } else {
            alog("%s: %s!%s@%s set the memo limit for %s to 0",
                 s_MemoServ, u->nick, u->username, u->host,
                 chan ? chan : user);
            notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_ZERO,
                        chan ? chan : user);
        }
    } else {
        if (!chan && na->nc == u->na->nc) {
            alog("%s: %s!%s@%s unset their memo limit",
                 s_MemoServ, u->nick, u->username, u->host);
            notice_lang(s_MemoServ, u, MEMO_UNSET_YOUR_LIMIT);
        } else {
            alog("%s: %s!%s@%s unset the memo limit for %s",
                 s_MemoServ, u->nick, u->username, u->host,
                 chan ? chan : user);
            notice_lang(s_MemoServ, u, MEMO_UNSET_LIMIT,
                        chan ? chan : user);
        }
    }
    return MOD_CONT;
}
