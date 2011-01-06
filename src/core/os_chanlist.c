/* OperServ core functions
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

int do_chanlist(User * u);
void myOperServHelp(User * u);
#ifdef _WIN32
extern MDE int anope_get_private_mode();
#endif

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

    c = createCommand("CHANLIST", do_chanlist, is_services_oper,
                      OPER_HELP_CHANLIST, -1, -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);

    moduleSetOperHelp(myOperServHelp);

    return MOD_CONT;
}


/**
 * Unload the module
 **/
void AnopeFini(void)
{

}


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
    notice_lang(s_OperServ, u, OPER_HELP_CMD_CHANLIST);
}

/**
 * The /os chanlist command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_chanlist(User * u)
{
    char *pattern = strtok(NULL, " ");
    char *opt = strtok(NULL, " ");

    int modes = 0;
    User *u2;

    if (opt && !stricmp(opt, "SECRET"))
        modes |= (anope_get_secret_mode() | anope_get_private_mode());

    if (pattern && (u2 = finduser(pattern))) {
        struct u_chanlist *uc;

        notice_lang(s_OperServ, u, OPER_CHANLIST_HEADER_USER, u2->nick);

        for (uc = u2->chans; uc; uc = uc->next) {
            if (modes && !(uc->chan->mode & modes))
                continue;
            notice_lang(s_OperServ, u, OPER_CHANLIST_RECORD,
                        uc->chan->name, uc->chan->usercount,
                        chan_get_modes(uc->chan, 1, 1),
                        (uc->chan->topic ? uc->chan->topic : ""));
        }
    } else {
        int i;
        Channel *c;

        notice_lang(s_OperServ, u, OPER_CHANLIST_HEADER);

        for (i = 0; i < 1024; i++) {
            for (c = chanlist[i]; c; c = c->next) {
                if (pattern && !match_wild_nocase(pattern, c->name))
                    continue;
                if (modes && !(c->mode & modes))
                    continue;
                notice_lang(s_OperServ, u, OPER_CHANLIST_RECORD, c->name,
                            c->usercount, chan_get_modes(c, 1, 1),
                            (c->topic ? c->topic : ""));
            }
        }
    }

    notice_lang(s_OperServ, u, OPER_CHANLIST_END);
    return MOD_CONT;
}
