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

int do_drop(User * u);
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

    c = createCommand("DROP", do_drop, NULL, -1, CHAN_HELP_DROP, -1,
                      CHAN_SERVADMIN_HELP_DROP, CHAN_SERVADMIN_HELP_DROP);
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
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DROP);
}

/**
 * The /cs command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_drop(User * u)
{
    char *chan = strtok(NULL, " ");
    ChannelInfo *ci;
    int is_servadmin = is_services_admin(u);

    if (readonly && !is_servadmin) {
        notice_lang(s_ChanServ, u, CHAN_DROP_DISABLED);
        return MOD_CONT;
    }

    if (!chan) {
        syntax_error(s_ChanServ, u, "DROP", CHAN_DROP_SYNTAX);
    } else if (!(ci = cs_findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (!is_servadmin && (ci->flags & CI_VERBOTEN)) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!is_servadmin && (ci->flags & CI_SUSPENDED)) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!is_servadmin
               && (ci->
                   flags & CI_SECUREFOUNDER ? !is_real_founder(u,
                                                               ci) :
                   !is_founder(u, ci))) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else {
        int level = get_access(u, ci);

        if (readonly)           /* in this case we know they're a Services admin */
            notice_lang(s_ChanServ, u, READ_ONLY_MODE);

        if (ci->c) {
            if (ircd->regmode) {
                ci->c->mode &= ~ircd->regmode;
                anope_cmd_mode(whosends(ci), ci->name, "-r");
            }
        }

        if (ircd->chansqline && (ci->flags & CI_VERBOTEN)) {
            anope_cmd_unsqline(ci->name);
        }

        alog("%s: Channel %s dropped by %s!%s@%s (founder: %s)",
             s_ChanServ, ci->name, u->nick, u->username,
             u->host, (ci->founder ? ci->founder->display : "(none)"));

        delchan(ci);

        /* We must make sure that the Services admin has not normally the right to
         * drop the channel before issuing the wallops.
         */
        if (WallDrop && is_servadmin && level < ACCESS_FOUNDER)
            anope_cmd_global(s_ChanServ,
                             "\2%s\2 used DROP on channel \2%s\2", u->nick,
                             chan);

        notice_lang(s_ChanServ, u, CHAN_DROPPED, chan);
        send_event(EVENT_CHAN_DROP, 1, chan);
    }
    return MOD_CONT;
}
