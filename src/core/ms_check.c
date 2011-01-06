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

int do_memocheck(User * u);
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
    c = createCommand("CHECK", do_memocheck, NULL, MEMO_HELP_CHECK, -1, -1,
                      -1, -1);
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
    notice_lang(s_MemoServ, u, MEMO_HELP_CMD_CHECK);
}

/**
 * The /ms check command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_memocheck(User * u)
{
    NickAlias *na = NULL;
    MemoInfo *mi = NULL;
    int i, found = 0;
    char *recipient = strtok(NULL, "");
    struct tm *tm;
    char timebuf[64];

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

    if ((na->status & NS_VERBOTEN)) {
        notice_lang(s_MemoServ, u, NICK_X_FORBIDDEN, recipient);
        return MOD_CONT;
    }

    mi = &na->nc->memos;

/* Okay, I know this looks strange but we wanna get the LAST memo, so we
    have to loop backwards */

    for (i = (mi->memocount - 1); i >= 0; i--) {
        if (!stricmp(mi->memos[i].sender, u->na->nc->display)) {
            found = 1;          /* Yes, we've found the memo */

            tm = localtime(&mi->memos[i].time);
            strftime_lang(timebuf, sizeof(timebuf), u,
                          STRFTIME_DATE_TIME_FORMAT, tm);

            if (mi->memos[i].flags & MF_UNREAD)
                notice_lang(s_MemoServ, u, MEMO_CHECK_NOT_READ, na->nick,
                            timebuf);
            else
                notice_lang(s_MemoServ, u, MEMO_CHECK_READ, na->nick,
                            timebuf);
            break;
        }
    }

    if (!found)
        notice_lang(s_MemoServ, u, MEMO_CHECK_NO_MEMO, na->nick);

    return MOD_CONT;
}
