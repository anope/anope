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

int do_read(User * u);
int read_memo_callback(User * u, int num, va_list args);
int read_memo(User * u, int index, MemoInfo * mi, const char *chan);
void myMemoServHelp(User * u);
extern void rsend_notify(User * u, Memo * m, const char *chan);


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
    c = createCommand("READ", do_read, NULL, MEMO_HELP_READ, -1, -1, -1,
                      -1);
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
    notice_lang(s_MemoServ, u, MEMO_HELP_CMD_READ);
}

/**
 * The /ms read command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_read(User * u)
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

/**
 * Read a memo callback function
 * @param u User Struct
 * @param int Index number
 * @param va_list variable arguements
 * @return result of read_memo()
 */
int read_memo_callback(User * u, int num, va_list args)
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

/**
 * Read a memo
 * @param u User Struct
 * @param int Index number
 * @param mi MemoInfo struct
 * @param chan Channel Name
 * @return 1 on success, 0 if failed
 */
int read_memo(User * u, int index, MemoInfo * mi, const char *chan)
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
    if (m->flags & MF_RECEIPT) {
        rsend_notify(u, m, chan);
    }

    return 1;
}
