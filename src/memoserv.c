/* MemoServ functions.
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
#include "pseudo.h"

/*************************************************************************/
/* *INDENT-OFF* */

NickCore *nclists[1024];
static int delmemo(MemoInfo *mi, int num);
static int do_help(User *u);
static int do_send(User *u);
void memo_send(User *u, char *name, char *text, int z);
static int do_cancel(User *u);
static int do_list(User *u);
static int do_read(User *u);
static int do_del(User *u);
static int do_set(User *u);
static int do_set_notify(User *u, MemoInfo *mi, char *param);
static int do_set_limit(User *u, MemoInfo *mi, char *param);
static int do_info(User *u);
static int do_staff(User *u);
static int do_sendall(User *u);
void moduleAddMemoServCmds(void);
static void new_memo_mail(NickCore *nc, Memo *m);
static int do_rsend(User *u);
static int do_memocheck(User *u);
void rsend_notify(User *u, Memo *m, const char *chan);
/*************************************************************************/

void moduleAddMemoServCmds(void) {
    Command *c;
    c = createCommand("HELP",       do_help, 	NULL,  -1,                      -1,-1,-1,-1); addCoreCommand(MEMOSERV,c);
    c = createCommand("SEND",       do_send, 	NULL,  MEMO_HELP_SEND,          -1,-1,-1,-1); addCoreCommand(MEMOSERV,c);
    c = createCommand("CANCEL",     do_cancel, 	NULL,  MEMO_HELP_CANCEL,        -1,-1,-1,-1); addCoreCommand(MEMOSERV,c);
    c = createCommand("LIST",       do_list, 	NULL,  MEMO_HELP_LIST,          -1,-1,-1,-1); addCoreCommand(MEMOSERV,c);
    c = createCommand("READ",       do_read, 	NULL,  MEMO_HELP_READ,          -1,-1,-1,-1); addCoreCommand(MEMOSERV,c);
    c = createCommand("DEL",        do_del,  	NULL,  MEMO_HELP_DEL,           -1,-1,-1,-1); addCoreCommand(MEMOSERV,c);
    c = createCommand("STAFF",      do_staff,   is_services_oper,  MEMO_HELP_STAFF,         -1,-1,-1,-1); addCoreCommand(MEMOSERV,c);
    c = createCommand("SET",        do_set,  	NULL,  MEMO_HELP_SET,           -1,-1,-1,-1); addCoreCommand(MEMOSERV,c);
    c = createCommand("SET NOTIFY", NULL,    	NULL,  MEMO_HELP_SET_NOTIFY,    -1,-1,-1,-1); addCoreCommand(MEMOSERV,c);
    c = createCommand("SET LIMIT",  NULL,    	NULL,  -1,MEMO_HELP_SET_LIMIT, MEMO_SERVADMIN_HELP_SET_LIMIT,MEMO_SERVADMIN_HELP_SET_LIMIT, MEMO_SERVADMIN_HELP_SET_LIMIT); addCoreCommand(MEMOSERV,c);
    c = createCommand("INFO",       do_info, NULL,  -1,MEMO_HELP_INFO, MEMO_SERVADMIN_HELP_INFO,MEMO_SERVADMIN_HELP_INFO, MEMO_SERVADMIN_HELP_INFO); addCoreCommand(MEMOSERV,c);
    c = createCommand("SENDALL",    do_sendall, is_services_admin, MEMO_HELP_SENDALL,       -1,-1,-1,-1); addCoreCommand(MEMOSERV,c);
    c = createCommand("RSEND",    do_rsend, NULL, MEMO_HELP_RSEND,       -1,-1,-1,-1); addCoreCommand(MEMOSERV,c);
    c = createCommand("CHECK",    do_memocheck, NULL, MEMO_HELP_CHECK,       -1,-1,-1,-1); addCoreCommand(MEMOSERV,c);
}

/*************************************************************************/
/*************************************************************************/
/* *INDENT-ON* */

/* MemoServ initialization. */

void ms_init(void)
{
    Command *cmd;
    moduleAddMemoServCmds();
    cmd = findCommand(MEMOSERV, "SET LIMIT");
    if (cmd)
        cmd->help_param1 = (char *) (long) MSMaxMemos;
}

/*************************************************************************/

/* memoserv:  Main MemoServ routine.
 *            Note that the User structure passed to the do_* routines will
 *            always be valid (non-NULL) and will always have a valid
 *            NickInfo pointer in the `ni' field.
 */

void memoserv(User * u, char *buf)
{
    char *cmd, *s;

    cmd = strtok(buf, " ");
    if (!cmd) {
        return;
    } else if (stricmp(cmd, "\1PING") == 0) {
        if (!(s = strtok(NULL, "")))
            s = "\1";
        notice(s_MemoServ, u->nick, "\1PING %s", s);
    } else if (skeleton) {
        notice_lang(s_MemoServ, u, SERVICE_OFFLINE, s_MemoServ);
    } else {
        if (!u->na && stricmp(cmd, "HELP") != 0)
            notice_lang(s_MemoServ, u, NICK_NOT_REGISTERED_HELP,
                        s_NickServ);
        else
            mod_run_cmd(s_MemoServ, u, MEMOSERV, cmd);
    }
}

/*************************************************************************/

/* check_memos:  See if the given user has any unread memos, and send a
 *               NOTICE to that user if so (and if the appropriate flag is
 *               set).
 */

void check_memos(User * u)
{
    NickCore *nc;
    int i, newcnt = 0;

    if (!(nc = (u->na ? u->na->nc : NULL)) || !nick_recognized(u) ||
        !(nc->flags & NI_MEMO_SIGNON))
        return;

    for (i = 0; i < nc->memos.memocount; i++) {
        if (nc->memos.memos[i].flags & MF_UNREAD)
            newcnt++;
    }
    if (newcnt > 0) {
        notice_lang(s_MemoServ, u,
                    newcnt == 1 ? MEMO_HAVE_NEW_MEMO : MEMO_HAVE_NEW_MEMOS,
                    newcnt);
        if (newcnt == 1 && (nc->memos.memos[i - 1].flags & MF_UNREAD)) {
            notice_lang(s_MemoServ, u, MEMO_TYPE_READ_LAST, s_MemoServ);
        } else if (newcnt == 1) {
            for (i = 0; i < nc->memos.memocount; i++) {
                if (nc->memos.memos[i].flags & MF_UNREAD)
                    break;
            }
            notice_lang(s_MemoServ, u, MEMO_TYPE_READ_NUM, s_MemoServ,
                        nc->memos.memos[i].number);
        } else {
            notice_lang(s_MemoServ, u, MEMO_TYPE_LIST_NEW, s_MemoServ);
        }
    }
    if (nc->memos.memomax > 0 && nc->memos.memocount >= nc->memos.memomax) {
        if (nc->memos.memocount > nc->memos.memomax)
            notice_lang(s_MemoServ, u, MEMO_OVER_LIMIT, nc->memos.memomax);
        else
            notice_lang(s_MemoServ, u, MEMO_AT_LIMIT, nc->memos.memomax);
    }
}

/*************************************************************************/
/*********************** MemoServ private routines ***********************/
/*************************************************************************/

/* Return the MemoInfo corresponding to the given nick or channel name.
 * Return in `ischan' 1 if the name was a channel name, else 0.
 */

static MemoInfo *getmemoinfo(const char *name, int *ischan)
{
    if (*name == '#') {
        ChannelInfo *ci;
        if (ischan)
            *ischan = 1;
        ci = cs_findchan(name);
        if (ci && !(ci->flags & CI_VERBOTEN))
            return &ci->memos;
        else
            return NULL;
    } else {
        NickAlias *na;
        if (ischan)
            *ischan = 0;
        na = findnick(name);
        if (na && !(na->status & NS_VERBOTEN))
            return &na->nc->memos;
        else
            return NULL;
    }
}

/*************************************************************************/

/* Delete a memo by number.  Return 1 if the memo was found, else 0. */

static int delmemo(MemoInfo * mi, int num)
{
    int i;

    for (i = 0; i < mi->memocount; i++) {
        if (mi->memos[i].number == num)
            break;
    }
    if (i < mi->memocount) {
        moduleCleanStruct(&mi->memos[i].moduleData);
        free(mi->memos[i].text);        /* Deallocate memo text memory */
        mi->memocount--;        /* One less memo now */
        if (i < mi->memocount)  /* Move remaining memos down a slot */
            memmove(mi->memos + i, mi->memos + i + 1,
                    sizeof(Memo) * (mi->memocount - i));
        if (mi->memocount == 0) {       /* If no more memos, free array */
            free(mi->memos);
            mi->memos = NULL;
        }
        return 1;
    } else {
        return 0;
    }
}

/*************************************************************************/
/*********************** MemoServ command routines ***********************/
/*************************************************************************/

/* Return a help message. */

static int do_help(User * u)
{
    char *cmd = strtok(NULL, "");

    if (!cmd) {
        notice_help(s_MemoServ, u, MEMO_HELP);
        if (is_services_oper(u)) {
            notice_help(s_MemoServ, u, MEMO_HELP_OPER);
        }
        if (is_services_admin(u)) {
            notice_help(s_MemoServ, u, MEMO_HELP_ADMIN);
        }
        moduleDisplayHelp(3, u);
        notice_help(s_MemoServ, u, MEMO_HELP_FOOTER, s_ChanServ);
    } else {
        mod_help_cmd(s_MemoServ, u, MEMOSERV, cmd);
    }
    return MOD_CONT;
}

/*************************************************************************/

/* Send a memo to a nick/channel. */

static int do_send(User * u)
{
    char *name = strtok(NULL, " ");
    char *text = strtok(NULL, "");
    int z = 0;
    memo_send(u, name, text, z);
    return MOD_CONT;
}

/**
 * Split from do_send, this way we can easily send a memo from any point :)
 * u - sender User
 * name - target name
 * text - memo Text
 * z - output level,
 *	0 - reply to user
 *	1 - silent
 *	2 - silent with no delay timer
 *	3 - reply to user and request read receipt
 **/
void memo_send(User * u, char *name, char *text, int z)
{
    int ischan;
    Memo *m;
    MemoInfo *mi;
    time_t now = time(NULL);
    char *source = u->na->nc->display;
    int is_servoper = is_services_oper(u);
    int j;

    if (readonly) {
        notice_lang(s_MemoServ, u, MEMO_SEND_DISABLED);
    } else if (checkDefCon(DEFCON_NO_NEW_MEMOS)) {
        notice_lang(s_MemoServ, u, OPER_DEFCON_DENIED);
        return;
    } else if (!text) {
        if (z == 0)
            syntax_error(s_MemoServ, u, "SEND", MEMO_SEND_SYNTAX);

        if (z == 3)
            syntax_error(s_MemoServ, u, "RSEND", MEMO_RSEND_SYNTAX);

    } else if (!nick_recognized(u)) {
        if (z == 0 || z == 3)
            notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);

    } else if (!(mi = getmemoinfo(name, &ischan))) {
        if (z == 0 || z == 3)
            notice_lang(s_MemoServ, u,
                        ischan ? CHAN_X_NOT_REGISTERED :
                        NICK_X_NOT_REGISTERED, name);

    } else if (z != 2 && MSSendDelay > 0 &&
               u && u->lastmemosend + MSSendDelay > now && !is_servoper) {
        u->lastmemosend = now;
        if (z == 0)
            notice_lang(s_MemoServ, u, MEMO_SEND_PLEASE_WAIT, MSSendDelay);

        if (z == 3)
            notice_lang(s_MemoServ, u, MEMO_RSEND_PLEASE_WAIT,
                        MSSendDelay);

    } else if (mi->memomax == 0 && !is_servoper) {
        if (z == 0 || z == 3)
            notice_lang(s_MemoServ, u, MEMO_X_GETS_NO_MEMOS, name);

    } else if (mi->memomax > 0 && mi->memocount >= mi->memomax
               && !is_servoper) {
        if (z == 0 || z == 3)
            notice_lang(s_MemoServ, u, MEMO_X_HAS_TOO_MANY_MEMOS, name);

    } else {
        u->lastmemosend = now;
        mi->memocount++;
        mi->memos = srealloc(mi->memos, sizeof(Memo) * mi->memocount);
        m = &mi->memos[mi->memocount - 1];
        strscpy(m->sender, source, NICKMAX);
        m->moduleData = NULL;
        if (mi->memocount > 1) {
            m->number = m[-1].number + 1;
            if (m->number < 1) {
                int i;
                for (i = 0; i < mi->memocount; i++) {
                    mi->memos[i].number = i + 1;
                }
            }
        } else {
            m->number = 1;
        }
        m->time = time(NULL);
        m->text = sstrdup(text);
        m->flags = MF_UNREAD;
        /* Set receipt request flag */
        if (z == 3)
            m->flags |= MF_RECEIPT;
        if (z == 0 || z == 3)
            notice_lang(s_MemoServ, u, MEMO_SENT, name);
        if (!ischan) {
            NickAlias *na;
            NickCore *nc = (findnick(name))->nc;

            if (MSNotifyAll) {
                if ((nc->flags & NI_MEMO_RECEIVE)
                    && get_ignore(name) == NULL) {
                    int i;

                    for (i = 0; i < nc->aliases.count; i++) {
                        na = nc->aliases.list[i];
                        if (na->u && nick_identified(na->u))
                            notice_lang(s_MemoServ, na->u,
                                        MEMO_NEW_MEMO_ARRIVED, source,
                                        s_MemoServ, m->number);
                    }
                } else {
                    if ((u = finduser(name)) && nick_identified(u)
                        && (nc->flags & NI_MEMO_RECEIVE))
                        notice_lang(s_MemoServ, u, MEMO_NEW_MEMO_ARRIVED,
                                    source, s_MemoServ, m->number);
                }               /* if (flags & MEMO_RECEIVE) */
            }
            /* if (MSNotifyAll) */
            /* let's get out the mail if set in the nickcore - certus */
            if (nc->flags & NI_MEMO_MAIL)
                new_memo_mail(nc, m);
        } else {
            struct c_userlist *cu, *next;
            Channel *c;

            if (MSNotifyAll && (c = findchan(name))) {
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;
                    if (check_access(cu->user, c->ci, CA_MEMO)) {
                        if (cu->user->na
                            && (cu->user->na->nc->flags & NI_MEMO_RECEIVE)
                            && get_ignore(cu->user->nick) == NULL) {
                            notice_lang(s_MemoServ, cu->user,
                                        MEMO_NEW_X_MEMO_ARRIVED,
                                        c->ci->name, s_MemoServ,
                                        c->ci->name, m->number);
                        }
                    }
                }
            }                   /* MSNotifyAll */
        }                       /* if (!ischan) */
    }                           /* if command is valid */
}

/*************************************************************************/

static int do_cancel(User * u)
{
    int ischan;
    char *name = strtok(NULL, " ");
    MemoInfo *mi;

    if (!name) {
        syntax_error(s_MemoServ, u, "CANCEL", MEMO_CANCEL_SYNTAX);

    } else if (!nick_recognized(u)) {
        notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);

    } else if (!(mi = getmemoinfo(name, &ischan))) {
        notice_lang(s_MemoServ, u,
                    ischan ? CHAN_X_NOT_REGISTERED : NICK_X_NOT_REGISTERED,
                    name);
    } else {
        int i;

        for (i = mi->memocount - 1; i >= 0; i--) {
            if ((mi->memos[i].flags & MF_UNREAD)
                && !stricmp(mi->memos[i].sender, u->na->nc->display)) {
                delmemo(mi, mi->memos[i].number);
                notice_lang(s_MemoServ, u, MEMO_CANCELLED, name);
                return MOD_CONT;
            }
        }

        notice_lang(s_MemoServ, u, MEMO_CANCEL_NONE);
    }
    return MOD_CONT;
}

/*************************************************************************/

/* Display a single memo entry, possibly printing the header first. */

static int list_memo(User * u, int index, MemoInfo * mi, int *sent_header,
                     int new, const char *chan)
{
    Memo *m;
    char timebuf[64];
    struct tm tm;

    if (index < 0 || index >= mi->memocount)
        return 0;
    if (!*sent_header) {
        if (chan) {
            notice_lang(s_MemoServ, u,
                        new ? MEMO_LIST_CHAN_NEW_MEMOS :
                        MEMO_LIST_CHAN_MEMOS, chan, s_MemoServ, chan);
        } else {
            notice_lang(s_MemoServ, u,
                        new ? MEMO_LIST_NEW_MEMOS : MEMO_LIST_MEMOS,
                        u->nick, s_MemoServ);
        }
        notice_lang(s_MemoServ, u, MEMO_LIST_HEADER);
        *sent_header = 1;
    }
    m = &mi->memos[index];
    tm = *localtime(&m->time);
    strftime_lang(timebuf, sizeof(timebuf),
                  u, STRFTIME_DATE_TIME_FORMAT, &tm);
    timebuf[sizeof(timebuf) - 1] = 0;   /* just in case */
    notice_lang(s_MemoServ, u, MEMO_LIST_FORMAT,
                (m->flags & MF_UNREAD) ? '*' : ' ',
                m->number, m->sender, timebuf);
    return 1;
}

static int list_memo_callback(User * u, int num, va_list args)
{
    MemoInfo *mi = va_arg(args, MemoInfo *);
    int *sent_header = va_arg(args, int *);
    const char *chan = va_arg(args, const char *);
    int i;

    for (i = 0; i < mi->memocount; i++) {
        if (mi->memos[i].number == num)
            break;
    }
    /* Range checking done by list_memo() */
    return list_memo(u, i, mi, sent_header, 0, chan);
}


/* List the memos (if any) for the source nick or given channel. */

static int do_list(User * u)
{
    char *param = strtok(NULL, " "), *chan = NULL;
    ChannelInfo *ci;
    MemoInfo *mi;
    Memo *m;
    int i;

    if (param && *param == '#') {
        chan = param;
        param = strtok(NULL, " ");
        if (!(ci = cs_findchan(chan))) {
            notice_lang(s_MemoServ, u, CHAN_X_NOT_REGISTERED, chan);
            return MOD_CONT;
        } else if (ci->flags & CI_VERBOTEN) {
            notice_lang(s_MemoServ, u, CHAN_X_FORBIDDEN, chan);
            return MOD_CONT;
        } else if (!check_access(u, ci, CA_MEMO)) {
            notice_lang(s_MemoServ, u, ACCESS_DENIED);
            return MOD_CONT;
        }
        mi = &ci->memos;
    } else {
        if (!nick_identified(u)) {
            notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
            return MOD_CONT;
        }
        mi = &u->na->nc->memos;
    }
    if (param && !isdigit(*param) && stricmp(param, "NEW") != 0) {
        syntax_error(s_MemoServ, u, "LIST", MEMO_LIST_SYNTAX);
    } else if (mi->memocount == 0) {
        if (chan)
            notice_lang(s_MemoServ, u, MEMO_X_HAS_NO_MEMOS, chan);
        else
            notice_lang(s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
    } else {
        int sent_header = 0;
        if (param && isdigit(*param)) {
            process_numlist(param, NULL, list_memo_callback, u,
                            mi, &sent_header, chan);
        } else {
            if (param) {
                for (i = 0, m = mi->memos; i < mi->memocount; i++, m++) {
                    if (m->flags & MF_UNREAD)
                        break;
                }
                if (i == mi->memocount) {
                    if (chan)
                        notice_lang(s_MemoServ, u, MEMO_X_HAS_NO_NEW_MEMOS,
                                    chan);
                    else
                        notice_lang(s_MemoServ, u, MEMO_HAVE_NO_NEW_MEMOS);
                    return MOD_CONT;
                }
            }
            for (i = 0, m = mi->memos; i < mi->memocount; i++, m++) {
                if (param && !(m->flags & MF_UNREAD))
                    continue;
                list_memo(u, i, mi, &sent_header, param != NULL, chan);
            }
        }
    }
    return MOD_CONT;
}

/*************************************************************************/

/* Send a single memo to the given user. */

static int read_memo(User * u, int index, MemoInfo * mi, const char *chan)
{
    Memo *m;
    char timebuf[64];
    struct tm tm;

    if (index < 0 || index >= mi->memocount)
        return 0;
    m = &mi->memos[index];
    tm = *localtime(&m->time);
    strftime_lang(timebuf, sizeof(timebuf),
                  u, STRFTIME_DATE_TIME_FORMAT, &tm);
    timebuf[sizeof(timebuf) - 1] = 0;
    if (chan)
        notice_lang(s_MemoServ, u, MEMO_CHAN_HEADER, m->number,
                    m->sender, timebuf, s_MemoServ, chan, m->number);
    else
        notice_lang(s_MemoServ, u, MEMO_HEADER, m->number,
                    m->sender, timebuf, s_MemoServ, m->number);
    notice_lang(s_MemoServ, u, MEMO_TEXT, m->text);
    m->flags &= ~MF_UNREAD;

    /* Check if a receipt notification was requested */
    if (m->flags && MF_RECEIPT) {
        rsend_notify(u, m, chan);
    }

    return 1;
}

static int read_memo_callback(User * u, int num, va_list args)
{
    MemoInfo *mi = va_arg(args, MemoInfo *);
    const char *chan = va_arg(args, const char *);
    int i;

    for (i = 0; i < mi->memocount; i++) {
        if (mi->memos[i].number == num)
            break;
    }
    /* Range check done in read_memo */
    return read_memo(u, i, mi, chan);
}


/* Read memos. */

static int do_read(User * u)
{
    MemoInfo *mi;
    ChannelInfo *ci;
    char *numstr = strtok(NULL, " "), *chan = NULL;
    int num, count;

    if (numstr && *numstr == '#') {
        chan = numstr;
        numstr = strtok(NULL, " ");
        if (!(ci = cs_findchan(chan))) {
            notice_lang(s_MemoServ, u, CHAN_X_NOT_REGISTERED, chan);
            return MOD_CONT;
        } else if (ci->flags & CI_VERBOTEN) {
            notice_lang(s_MemoServ, u, CHAN_X_FORBIDDEN, chan);
            return MOD_CONT;
        } else if (!check_access(u, ci, CA_MEMO)) {
            notice_lang(s_MemoServ, u, ACCESS_DENIED);
            return MOD_CONT;
        }
        mi = &ci->memos;
    } else {
        if (!nick_identified(u)) {
            notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
            return MOD_CONT;
        }
        mi = &u->na->nc->memos;
    }
    num = numstr ? atoi(numstr) : -1;
    if (!numstr
        || (stricmp(numstr, "LAST") != 0 && stricmp(numstr, "NEW") != 0
            && num <= 0)) {
        syntax_error(s_MemoServ, u, "READ", MEMO_READ_SYNTAX);

    } else if (mi->memocount == 0) {
        if (chan)
            notice_lang(s_MemoServ, u, MEMO_X_HAS_NO_MEMOS, chan);
        else
            notice_lang(s_MemoServ, u, MEMO_HAVE_NO_MEMOS);

    } else {
        int i;

        if (stricmp(numstr, "NEW") == 0) {
            int readcount = 0;
            for (i = 0; i < mi->memocount; i++) {
                if (mi->memos[i].flags & MF_UNREAD) {
                    read_memo(u, i, mi, chan);
                    readcount++;
                }
            }
            if (!readcount) {
                if (chan)
                    notice_lang(s_MemoServ, u, MEMO_X_HAS_NO_NEW_MEMOS,
                                chan);
                else
                    notice_lang(s_MemoServ, u, MEMO_HAVE_NO_NEW_MEMOS);
            }
        } else if (stricmp(numstr, "LAST") == 0) {
            for (i = 0; i < mi->memocount - 1; i++);
            read_memo(u, i, mi, chan);
        } else {                /* number[s] */
            if (!process_numlist(numstr, &count, read_memo_callback, u,
                                 mi, chan)) {
                if (count == 1)
                    notice_lang(s_MemoServ, u, MEMO_DOES_NOT_EXIST, num);
                else
                    notice_lang(s_MemoServ, u, MEMO_LIST_NOT_FOUND,
                                numstr);
            }
        }

    }
    return MOD_CONT;
}

/*************************************************************************/

/* Delete a single memo from a MemoInfo. */

static int del_memo_callback(User * u, int num, va_list args)
{
    MemoInfo *mi = va_arg(args, MemoInfo *);
    int *last = va_arg(args, int *);
    int *last0 = va_arg(args, int *);
    char **end = va_arg(args, char **);
    int *left = va_arg(args, int *);

    if (delmemo(mi, num)) {
        if (num != (*last) + 1) {
            if (*last != -1) {
                int len;
                if (*last0 != *last)
                    len = snprintf(*end, *left, ",%d-%d", *last0, *last);
                else
                    len = snprintf(*end, *left, ",%d", *last);
                *end += len;
                *left -= len;
            }
            *last0 = num;
        }
        *last = num;
        return 1;
    } else {
        return 0;
    }
}


/* Delete memos. */

static int do_del(User * u)
{
    MemoInfo *mi;
    ChannelInfo *ci;
    char *numstr = strtok(NULL, ""), *chan = NULL;
    int last, last0, i;
    char buf[BUFSIZE], *end;
    int delcount, count, left;

    if (numstr && *numstr == '#') {
        chan = strtok(numstr, " ");
        numstr = strtok(NULL, "");
        if (!(ci = cs_findchan(chan))) {
            notice_lang(s_MemoServ, u, CHAN_X_NOT_REGISTERED, chan);
            return MOD_CONT;
        } else if (ci->flags & CI_VERBOTEN) {
            notice_lang(s_MemoServ, u, CHAN_X_FORBIDDEN, chan);
            return MOD_CONT;
        } else if (!check_access(u, ci, CA_MEMO)) {
            notice_lang(s_MemoServ, u, ACCESS_DENIED);
            return MOD_CONT;
        }
        mi = &ci->memos;
    } else {
        if (!nick_identified(u)) {
            notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
            return MOD_CONT;
        }
        mi = &u->na->nc->memos;
    }
    if (!numstr
        || (!isdigit(*numstr) && stricmp(numstr, "ALL") != 0
            && stricmp(numstr, "LAST") != 0)) {
        syntax_error(s_MemoServ, u, "DEL", MEMO_DEL_SYNTAX);
    } else if (mi->memocount == 0) {
        if (chan)
            notice_lang(s_MemoServ, u, MEMO_X_HAS_NO_MEMOS, chan);
        else
            notice_lang(s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
    } else {
        if (isdigit(*numstr)) {
            /* Delete a specific memo or memos. */
            last = -1;          /* Last memo deleted */
            last0 = -1;         /* Beginning of range of last memos deleted */
            end = buf;
            left = sizeof(buf);
            delcount =
                process_numlist(numstr, &count, del_memo_callback, u, mi,
                                &last, &last0, &end, &left);
            if (last != -1) {
                /* Some memos got deleted; tell them which ones. */
                if (delcount > 1) {
                    if (last0 != last)
                        end += snprintf(end, sizeof(buf) - (end - buf),
                                        ",%d-%d", last0, last);
                    else
                        end += snprintf(end, sizeof(buf) - (end - buf),
                                        ",%d", last);
                    /* "buf+1" here because *buf == ',' */
                    notice_lang(s_MemoServ, u, MEMO_DELETED_SEVERAL,
                                buf + 1);
                } else {
                    notice_lang(s_MemoServ, u, MEMO_DELETED_ONE, last);
                }
            } else {
                /* No memos were deleted.  Tell them so. */
                if (count == 1)
                    notice_lang(s_MemoServ, u, MEMO_DOES_NOT_EXIST,
                                atoi(numstr));
                else
                    notice_lang(s_MemoServ, u, MEMO_DELETED_NONE);
            }
        } else if (stricmp(numstr, "LAST") == 0) {
            /* Delete last memo. */
            for (i = 0; i < mi->memocount; i++)
                last = mi->memos[i].number;
            delmemo(mi, last);
            notice_lang(s_MemoServ, u, MEMO_DELETED_ONE, last);
        } else {
            /* Delete all memos. */
            for (i = 0; i < mi->memocount; i++) {
                free(mi->memos[i].text);
                moduleCleanStruct(&mi->memos[i].moduleData);
            }
            free(mi->memos);
            mi->memos = NULL;
            mi->memocount = 0;
            if (chan)
                notice_lang(s_MemoServ, u, MEMO_CHAN_DELETED_ALL, chan);
            else
                notice_lang(s_MemoServ, u, MEMO_DELETED_ALL);
        }

        /* Reset the order */
        for (i = 0; i < mi->memocount; i++)
            mi->memos[i].number = i + 1;
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set(User * u)
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

/*************************************************************************/

static int do_set_notify(User * u, MemoInfo * mi, char *param)
{
    if (stricmp(param, "ON") == 0) {
        u->na->nc->flags |= NI_MEMO_SIGNON | NI_MEMO_RECEIVE;
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_ON, s_MemoServ);
    } else if (stricmp(param, "LOGON") == 0) {
        u->na->nc->flags |= NI_MEMO_SIGNON;
        u->na->nc->flags &= ~NI_MEMO_RECEIVE;
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_LOGON, s_MemoServ);
    } else if (stricmp(param, "NEW") == 0) {
        u->na->nc->flags &= ~NI_MEMO_SIGNON;
        u->na->nc->flags |= NI_MEMO_RECEIVE;
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_NEW, s_MemoServ);
    } else if (stricmp(param, "MAIL") == 0) {
        if (u->na->nc->email) {
            u->na->nc->flags |= NI_MEMO_MAIL;
            notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_MAIL);
        } else {
            notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_INVALIDMAIL);
        }
    } else if (stricmp(param, "NOMAIL") == 0) {
        u->na->nc->flags &= ~NI_MEMO_MAIL;
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_NOMAIL);
    } else if (stricmp(param, "OFF") == 0) {
        u->na->nc->flags &= ~(NI_MEMO_SIGNON | NI_MEMO_RECEIVE);
        notice_lang(s_MemoServ, u, MEMO_SET_NOTIFY_OFF, s_MemoServ);
    } else {
        syntax_error(s_MemoServ, u, "SET NOTIFY", MEMO_SET_NOTIFY_SYNTAX);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_set_limit(User * u, MemoInfo * mi, char *param)
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
        if (!chan && na->nc == u->na->nc)
            notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT, limit);
        else
            notice_lang(s_MemoServ, u, MEMO_SET_LIMIT,
                        chan ? chan : user, limit);
    } else if (limit == 0) {
        if (!chan && na->nc == u->na->nc)
            notice_lang(s_MemoServ, u, MEMO_SET_YOUR_LIMIT_ZERO);
        else
            notice_lang(s_MemoServ, u, MEMO_SET_LIMIT_ZERO,
                        chan ? chan : user);
    } else {
        if (!chan && na->nc == u->na->nc)
            notice_lang(s_MemoServ, u, MEMO_UNSET_YOUR_LIMIT);
        else
            notice_lang(s_MemoServ, u, MEMO_UNSET_LIMIT,
                        chan ? chan : user);
    }
    return MOD_CONT;
}

/*************************************************************************/

static int do_info(User * u)
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
            else if (count == 0)
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS_ONE_UNREAD,
                            name, mi->memocount);
            else
                notice_lang(s_MemoServ, u, MEMO_INFO_X_MEMOS_SOME_UNREAD,
                            name, mi->memocount, count);
        }
        if (mi->memomax >= 0) {
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

/*************************************************************************/
/**
 * Allow the easy sending of memo's to all user's on the oper/admin/root lists
 * - Rob
 * Opers in several lists won't get the memo twice from now on
 * - Certus
 **/

static int do_staff(User * u)
{
    NickCore *nc;
    int i, z = 0;
    char *text = strtok(NULL, "");

    if (readonly) {
        notice_lang(s_MemoServ, u, MEMO_SEND_DISABLED);
        return MOD_CONT;
    } else if (checkDefCon(DEFCON_NO_NEW_MEMOS)) {
        notice_lang(s_MemoServ, u, OPER_DEFCON_DENIED);
        return MOD_CONT;
    } else if (text == NULL) {
        syntax_error(s_MemoServ, u, "SEND", MEMO_SEND_SYNTAX);
        return MOD_CONT;
    }

    for (i = 0; i < 1024; i++) {
        for (nc = nclists[i]; nc; nc = nc->next) {
            if (nick_is_services_oper(nc))
                memo_send(u, nc->display, text, z);
        }
    }
    return MOD_CONT;
}

/*************************************************************************/
/**
 * Send a memo to all registered nicks
 * - Certus - 06/06/2003
 **/
static int do_sendall(User * u)
{
    int i, z = 1;
    NickCore *nc;
    char *text = strtok(NULL, "");



    if (readonly) {
        notice_lang(s_MemoServ, u, MEMO_SEND_DISABLED);
        return MOD_CONT;
    } else if (checkDefCon(DEFCON_NO_NEW_MEMOS)) {
        notice_lang(s_MemoServ, u, OPER_DEFCON_DENIED);
        return MOD_CONT;
    } else if (!text) {
        syntax_error(s_MemoServ, u, "SENDALL", MEMO_SEND_SYNTAX);
        return MOD_CONT;
    }


    for (i = 0; i < 1024; i++) {
        for (nc = nclists[i]; nc; nc = nc->next) {
            if (stricmp(u->nick, nc->display) != 0)
                memo_send(u, nc->display, text, z);
        }                       /* /nc */
    }                           /* /i */

    notice_lang(s_MemoServ, u, MEMO_MASS_SENT);
    return MOD_CONT;
}

/*************************************************************************/

static void new_memo_mail(NickCore * nc, Memo * m)
{
    MailInfo *mail = NULL;

    if (!nc || !m)
        return;

    mail = MailMemoBegin(nc);
    if (!mail) {
        return;
    }
    fprintf(mail->pipe, getstring2(NULL, MEMO_MAIL_TEXT1), nc->display);
    fprintf(mail->pipe, "\n");
    fprintf(mail->pipe, getstring2(NULL, MEMO_MAIL_TEXT2), m->sender,
            m->number);
    fprintf(mail->pipe, "\n\n");
    fprintf(mail->pipe, getstring2(NULL, MEMO_MAIL_TEXT3));
    fprintf(mail->pipe, "\n\n");
    fprintf(mail->pipe, "%s", m->text);
    fprintf(mail->pipe, "\n");
    MailEnd(mail);
    return;
}

/*************************************************************************/
/* Send a memo to a nick/channel requesting a receipt. */

static int do_rsend(User * u)
{
    char *name = strtok(NULL, " ");
    char *text = strtok(NULL, "");
    int z = 3;

    if (MSMemoReceipt == 1) {
        /* Services opers and above can use rsend */
        if (is_services_oper(u)) {
            memo_send(u, name, text, z);
        } else {
            notice_lang(s_MemoServ, u, ACCESS_DENIED);
        }
    } else if (MSMemoReceipt == 2) {
        /* Everybody can use rsend */
        memo_send(u, name, text, z);
    } else {
        /* rsend has been disabled */
        notice_lang(s_MemoServ, u, MEMO_RSEND_DISABLED);
    }

    return MOD_CONT;
}

/*************************************************************************/
/* Send receipt notification to sender. */

void rsend_notify(User * u, Memo * m, const char *chan)
{
    NickAlias *na;
    NickCore *nc;
    char text[256];
    const char *fmt;

    /* Only send receipt if memos are allowed */
    if ((!readonly) && (!checkDefCon(DEFCON_NO_NEW_MEMOS))) {

        /* Get nick alias for sender */
        na = findnick(m->sender);

        if (!na) {
            return;
        }

        /* Get nick core for sender */
        nc = na->nc;

        if (!nc) {
            return;
        }

        /* Text of the memo varies if the recepient was a
           nick or channel */
        if (chan) {
            fmt = getstring(na, MEMO_RSEND_CHAN_MEMO_TEXT);
            sprintf(text, fmt, chan);
        } else {
            fmt = getstring(na, MEMO_RSEND_NICK_MEMO_TEXT);
            sprintf(text, fmt);
        }

        /* Send notification */
        memo_send(u, m->sender, text, 2);

        /* Notify recepient of the memo that a notification has
           been sent to the sender */
        notice_lang(s_MemoServ, u, MEMO_RSEND_USER_NOTIFICATION,
                    nc->display);
    }

    /* Remove receipt flag from the original memo */
    m->flags &= ~MF_RECEIPT;

    return;
}



/*************************************************************************/
/* This function checks whether the last memo you sent to person X has been read
    or not. Note that this function does only work with nicks, NOT with chans. */

static int do_memocheck(User * u)
{
    NickAlias *na = NULL;
    MemoInfo *mi = NULL;
    int i, found = 0;
    char *stime = NULL;
    char *recipient = strtok(NULL, "");

    if (!recipient) {
        syntax_error(s_MemoServ, u, "CHECK", MEMO_CHECK_SYNTAX);
        return MOD_CONT;
    } else if (!nick_recognized(u)) {
        notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
        return MOD_CONT;
    } else if (!(na = findnick(recipient))) {
        notice_lang(s_MemoServ, u, NICK_X_NOT_REGISTERED, recipient);
        return MOD_CONT;
    }

    mi = &na->nc->memos;

/* Okay, I know this looks strange but we wanna get the LAST memo, so we
    have to loop backwards */

    for (i = (mi->memocount - 1); i >= 0; i--) {
        if (!stricmp(mi->memos[i].sender, u->nick)) {
            found = 1;          /* Yes, we've found the memo */

            stime = strdup(ctime(&mi->memos[i].time));
            *(stime + strlen(stime) - 1) = ' '; /* cut the f*cking \0 terminator and replace it with a single space */

            if (mi->memos[i].flags & MF_UNREAD)
                notice_lang(s_MemoServ, u, MEMO_CHECK_NOT_READ, na->nick,
                            stime);
            else
                notice_lang(s_MemoServ, u, MEMO_CHECK_READ, na->nick,
                            stime);
            break;
        }
    }

    if (!found)
        notice_lang(s_MemoServ, u, MEMO_CHECK_NO_MEMO, na->nick);

    if (stime)
        free(stime);

    return MOD_CONT;
}


/*************************************************************************/
