/* NickServ core functions
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

int do_nickupdate(User * u);
void myNickServHelp(User * u);

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

    c = createCommand("UPDATE", do_nickupdate, NULL, NICK_HELP_UPDATE, -1,
                      -1, -1, -1);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

    moduleSetNickHelp(myNickServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User * u)
{
    notice_lang(s_NickServ, u, NICK_HELP_CMD_UPDATE);
}

/**
 * The /ns update command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_nickupdate(User * u)
{
    NickAlias *na;

    if (!nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else {
        na = u->na;
        do_setmodes(u);
        check_memos(u);
        if (na->last_realname)
            free(na->last_realname);
        na->last_realname = sstrdup(u->realname);
        na->status |= NS_IDENTIFIED;
        na->last_seen = time(NULL);
        if (ircd->vhost) {
            do_on_id(u);
        }
        notice_lang(s_NickServ, u, NICK_UPDATE_SUCCESS, s_NickServ);
    }
    return MOD_CONT;
}
