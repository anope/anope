/* ChanServ core functions
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
void myChanServHelp(User * u);

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
                      CHAN_SERVADMIN_HELP_GETPASS,
                      CHAN_SERVADMIN_HELP_GETPASS);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);

    moduleSetChanHelp(myChanServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
    if (is_services_admin(u)) {
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_GETPASS);
    }
}

/**
 * The /cs getpass command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/

int do_getpass(User * u)
{
    char *chan = strtok(NULL, " ");
    char tmp_pass[PASSMAX];
    ChannelInfo *ci;

    if (!chan) {
        syntax_error(s_ChanServ, u, "GETPASS", CHAN_GETPASS_SYNTAX);
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (CSRestrictGetPass && !is_services_root(u)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else {
        if(enc_decrypt(ci->founderpass, tmp_pass, PASSMAX - 1)==1) {
            alog("%s: %s!%s@%s used GETPASS on %s",
                 s_ChanServ, u->nick, u->username, u->host, ci->name);
            if (WallGetpass) {
                anope_cmd_global(s_ChanServ,
                                 "\2%s\2 used GETPASS on channel \2%s\2",
                                 u->nick, chan);
            }
            notice_lang(s_ChanServ, u, CHAN_GETPASS_PASSWORD_IS,
                        chan, tmp_pass);
        } else {
            notice_lang(s_ChanServ, u, CHAN_GETPASS_UNAVAILABLE);
        }
    }
    return MOD_CONT;
}
