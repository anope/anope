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

int do_info(User * u);
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
    c = createCommand("INFO", do_info, NULL, -1, MEMO_HELP_INFO, -1,
                      MEMO_SERVADMIN_HELP_INFO, MEMO_SERVADMIN_HELP_INFO);
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
 * Add the help response to anopes /ms help output.
 * @param u The user who is requesting help
 **/
void myMemoServHelp(User * u)
{
    notice_lang(s_MemoServ, u, MEMO_HELP_CMD_INFO);
}

/**
 * The /ms info command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_info(User * u)
{
    MemoInfo *mi;
    NickAlias *na = NULL;
    ChannelInfo *ci = NULL;
    char *name = strtok(NULL, " ");
    int is_servadmin = is_services_admin(u);
    int hardmax = 0;

    if (is_servadmin && name && *name != '#') {
        na = findnick(name);
        if (!na) {
            notice_lang(s_MemoServ, u, NICK_X_NOT_REGISTERED, name);
            return MOD_CONT;
        } else if (na->status & NS_VERBOTEN) {
            notice_lang(s_MemoServ, u, NICK_X_FORBIDDEN, name);
            return MOD_CONT;
        }
        mi = &na->nc->memos;
        hardmax = na->nc->flags & NI_MEMO_HARDMAX ? 1 : 0;
    } else if (name && *name == '#') {
        ci = cs_findchan(name);
        if (!ci) {
            notice_lang(s_MemoServ, u, CHAN_X_NOT_REGISTERED, name);
            return MOD_CONT;
        } else if (ci->flags & CI_VERBOTEN) {
            notice_lang(s_MemoServ, u, CHAN_X_FORBIDDEN, name);
            return MOD_CONT;
        } else if (!check_access(u, ci, CA_MEMO)) {
            notice_lang(s_MemoServ, u, ACCESS_DENIED);
            return MOD_CONT;
        }
        mi = &ci->memos;
        hardmax = ci->flags & CI_MEMO_HARDMAX ? 1 : 0;
    } else if (name) {          /* It's not a chan and we aren't services admin */
        notice_lang(s_MemoServ, u, ACCESS_DENIED);
        return MOD_CONT;
    } else {                    /* !name */
        if (!nick_identified(u)) {
            notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
            return MOD_CONT;
        }
        mi = &u->na->nc->memos;
        hardmax = u->na->nc->flags & NI_MEMO_HARDMAX ? 1 : 0;
    }

    if (name && (ci || na->nc != u->na->nc)) {

        if (!mi->memocount) {
            notice_lang(s_MemoServ, u, MEMO_INFO_X_NO_MEMOS, name);
        } else if (mi->memocount == 1) {
            if (mi->memos[0].flags & MF_UNREAD)
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMO_UNREAD, name);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMO, name);
        } else {
            int count = 0, i;
            for (i = 0; i < mi->memocount; i++) {
                if (mi->memos[i].flags & MF_UNREAD)
                    count++;
            }
            if (count == mi->memocount)
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS_ALL_UNREAD,
                            name, count);
            else if (count == 0)
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS, name,
                            mi->memocount);
            else if (count == 1)
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS_ONE_UNREAD,
                            name, mi->memocount);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS_SOME_UNREAD,
                            name, mi->memocount, count);
        }
        if (mi->memomax == 0) {
            if (hardmax)
                notice_lang(s_MemoServ, u, MEMO_INFO_X_HARD_LIMIT, name,
                            mi->memomax);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_X_LIMIT, name,
                            mi->memomax);
        } else if (mi->memomax > 0) {
            if (hardmax)
                notice_lang(s_MemoServ, u, MEMO_INFO_X_HARD_LIMIT, name,
                            mi->memomax);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_X_LIMIT, name,
                            mi->memomax);
        } else {
            notice_lang(s_MemoServ, u, MEMO_INFO_X_NO_LIMIT, name);
        }

        /* I ripped this code out of ircservices 4.4.5, since I didn't want
           to rewrite the whole thing (it pisses me off). */
        if (na) {
            if ((na->nc->flags & NI_MEMO_RECEIVE)
                && (na->nc->flags & NI_MEMO_SIGNON)) {
                notice_lang(s_MemoServ, u, MEMO_INFO_X_NOTIFY_ON, name);
            } else if (na->nc->flags & NI_MEMO_RECEIVE) {
                notice_lang(s_MemoServ, u, MEMO_INFO_X_NOTIFY_RECEIVE,
                            name);
            } else if (na->nc->flags & NI_MEMO_SIGNON) {
                notice_lang(s_MemoServ, u, MEMO_INFO_X_NOTIFY_SIGNON,
                            name);
            } else {
                notice_lang(s_MemoServ, u, MEMO_INFO_X_NOTIFY_OFF, name);
            }
        }

    } else {                    /* !name || (!ci || na->nc == u->na->nc) */

        if (!mi->memocount) {
            notice_lang(s_MemoServ, u, MEMO_INFO_NO_MEMOS);
        } else if (mi->memocount == 1) {
            if (mi->memos[0].flags & MF_UNREAD)
                notice_lang(s_MemoServ, u, MEMO_INFO_MEMO_UNREAD);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_MEMO);
        } else {
            int count = 0, i;
            for (i = 0; i < mi->memocount; i++) {
                if (mi->memos[i].flags & MF_UNREAD)
                    count++;
            }
            if (count == mi->memocount)
                notice_lang(s_MemoServ, u, MEMO_INFO_MEMOS_ALL_UNREAD,
                            count);
            else if (count == 0)
                notice_lang(s_MemoServ, u, MEMO_INFO_MEMOS, mi->memocount);
            else if (count == 1)
                notice_lang(s_MemoServ, u, MEMO_INFO_MEMOS_ONE_UNREAD,
                            mi->memocount);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_MEMOS_SOME_UNREAD,
                            mi->memocount, count);
        }

        if (mi->memomax == 0) {
            if (!is_servadmin && hardmax)
                notice_lang(s_MemoServ, u, MEMO_INFO_HARD_LIMIT_ZERO);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_LIMIT_ZERO);
        } else if (mi->memomax > 0) {
            if (!is_servadmin && hardmax)
                notice_lang(s_MemoServ, u, MEMO_INFO_HARD_LIMIT,
                            mi->memomax);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_LIMIT, mi->memomax);
        } else {
            notice_lang(s_MemoServ, u, MEMO_INFO_NO_LIMIT);
        }

        /* Ripped too. But differently because of a seg fault (loughs) */
        if ((u->na->nc->flags & NI_MEMO_RECEIVE)
            && (u->na->nc->flags & NI_MEMO_SIGNON)) {
            notice_lang(s_MemoServ, u, MEMO_INFO_NOTIFY_ON);
        } else if (u->na->nc->flags & NI_MEMO_RECEIVE) {
            notice_lang(s_MemoServ, u, MEMO_INFO_NOTIFY_RECEIVE);
        } else if (u->na->nc->flags & NI_MEMO_SIGNON) {
            notice_lang(s_MemoServ, u, MEMO_INFO_NOTIFY_SIGNON);
        } else {
            notice_lang(s_MemoServ, u, MEMO_INFO_NOTIFY_OFF);
        }
    }
    return MOD_CONT;            /* if (name && (ci || na->nc != u->na->nc)) */
}
