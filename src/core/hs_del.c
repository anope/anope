/* HostServ core functions
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
void myHostServHelp(User * u);

/**
 * Create the off command, and tell anope about it.
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

    c = createCommand("DEL", do_del, is_host_remover, HOST_HELP_DEL, -1,
                      -1, -1, -1);
    moduleAddCommand(HOSTSERV, c, MOD_UNIQUE);

    moduleSetHostHelp(myHostServHelp);

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
void myHostServHelp(User * u)
{
    if (is_host_remover(u)) {
        notice_lang(s_HostServ, u, HOST_HELP_CMD_DEL);
    }
}

/**
 * The /hs del command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_del(User * u)
{
    NickAlias *na;
    char *nick = strtok(NULL, " ");
    if (nick) {
        if ((na = findnick(nick))) {
            if (na->status & NS_VERBOTEN) {
                notice_lang(s_HostServ, u, NICK_X_FORBIDDEN, nick);
                return MOD_CONT;
            }
            alog("vHost for user \002%s\002 deleted by oper \002%s\002",
                 nick, u->nick);
            delHostCore(nick);
            notice_lang(s_HostServ, u, HOST_DEL, nick);
        } else {
            notice_lang(s_HostServ, u, HOST_NOREG, nick);
        }
    } else {
    	syntax_error(s_HostServ, u, "DEL", HOST_DEL_SYNTAX);
    }
    return MOD_CONT;
}
