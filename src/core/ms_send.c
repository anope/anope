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

int do_send(User * u);
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
    c = createCommand("SEND", do_send, NULL, MEMO_HELP_SEND, -1, -1, -1,
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
    notice_lang(s_MemoServ, u, MEMO_HELP_CMD_SEND);
}

/**
 * The /ms send command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_send(User * u)
{
    char *name = strtok(NULL, " ");
    char *text = strtok(NULL, "");
    int z = 0;
    memo_send(u, name, text, z);
    return MOD_CONT;
}
