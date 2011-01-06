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

int do_staff(User * u);
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
    moduleAddVersion
        (VERSION_STRING);
    moduleSetType(CORE);
    c = createCommand("STAFF", do_staff, is_services_oper, -1, -1,
                      MEMO_HELP_STAFF, MEMO_HELP_STAFF, MEMO_HELP_STAFF);
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
    if (is_services_oper(u)) {
        notice_lang(s_MemoServ, u, MEMO_HELP_CMD_STAFF);
    }
}

/**
 * The /ms staff command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_staff(User * u)
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
        syntax_error(s_MemoServ, u, "STAFF", MEMO_STAFF_SYNTAX);
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
