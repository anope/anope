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

int do_del(User * u);
int del_memo_callback(User * u, int num, va_list args);
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
    c = createCommand("DEL", do_del, NULL, MEMO_HELP_DEL, -1, -1, -1, -1);
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
    notice_lang(s_MemoServ, u, MEMO_HELP_CMD_DEL);
}

/**
 * The /ms del command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_del(User * u)
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
        } else if (readonly) {
            notice_lang(s_MemoServ, u, READ_ONLY_MODE);
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

/**
 * Delete a single memo from a MemoInfo. callback function
 * @param u User Struct
 * @param int Number
 * @param va_list Variable Arguemtns
 * @return 1 if successful, 0 if it fails
 */
int del_memo_callback(User * u, int num, va_list args)
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
