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

int do_getpass(User * u);
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

    c = createCommand("GETPASS", do_getpass, is_services_admin, -1, -1, -1,
                      NICK_SERVADMIN_HELP_GETPASS,
                      NICK_SERVADMIN_HELP_GETPASS);
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
    if (is_services_admin(u)) {
        notice_lang(s_NickServ, u, NICK_HELP_CMD_GETPASS);
    }
}

/**
 * The /ns getpass command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_getpass(User * u)
{
    char *nick = strtok(NULL, " ");
    char tmp_pass[PASSMAX];
    NickAlias *na;
    NickRequest *nr = NULL;

    if (!nick) {
        syntax_error(s_NickServ, u, "GETPASS", NICK_GETPASS_SYNTAX);
    } else if (!(na = findnick(nick))) {
        if ((nr = findrequestnick(nick))) {
            alog("%s: %s!%s@%s used GETPASS on %s", s_NickServ, u->nick,
                 u->username, u->host, nick);
            if (WallGetpass)
                anope_cmd_global(s_NickServ,
                                 "\2%s\2 used GETPASS on \2%s\2", u->nick,
                                 nick);
            notice_lang(s_NickServ, u, NICK_GETPASS_PASSCODE_IS, nick,
                        nr->passcode);
        } else {
            notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
        }
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else if (NSSecureAdmins && nick_is_services_admin(na->nc)
               && !is_services_root(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
    } else if (NSRestrictGetPass && !is_services_root(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
    } else {
        if(enc_decrypt(na->nc->pass,tmp_pass,PASSMAX - 1)==1) {
            alog("%s: %s!%s@%s used GETPASS on %s", s_NickServ, u->nick,
                 u->username, u->host, nick);
            if (WallGetpass)
                anope_cmd_global(s_NickServ, "\2%s\2 used GETPASS on \2%s\2",
                                 u->nick, nick);
            notice_lang(s_NickServ, u, NICK_GETPASS_PASSWORD_IS, nick,
                        tmp_pass);
        } else {
            notice_lang(s_NickServ, u, NICK_GETPASS_UNAVAILABLE);
        }
    }
    return MOD_CONT;
}
