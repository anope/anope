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

int do_suspend(User * u);
int do_unsuspend(User * u);
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

    c = createCommand("SUSPEND", do_suspend, is_services_oper, -1, -1,
                      CHAN_SERVADMIN_HELP_SUSPEND,
                      CHAN_SERVADMIN_HELP_SUSPEND,
                      CHAN_SERVADMIN_HELP_SUSPEND);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("UNSUSPEND", do_unsuspend, is_services_oper, -1, -1,
                      CHAN_SERVADMIN_HELP_UNSUSPEND, 
                      CHAN_SERVADMIN_HELP_UNSUSPEND,
                      CHAN_SERVADMIN_HELP_UNSUSPEND);
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
    if (is_services_oper(u)) {
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_SUSPEND);
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_UNSUSPEND);
    }
}

/**
 * The /cs (un)suspend command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_suspend(User * u)
{
    ChannelInfo *ci;
    char *chan = strtok(NULL, " ");
    char *reason = strtok(NULL, "");

    Channel *c;

    /* Assumes that permission checking has already been done. */
    if (!chan || (ForceForbidReason && !reason)) {
        syntax_error(s_ChanServ, u, "SUSPEND",
                     (ForceForbidReason ? CHAN_SUSPEND_SYNTAX_REASON :
                      CHAN_SUSPEND_SYNTAX));
        return MOD_CONT;
    }

    if (chan[0] != '#') {
        notice_lang(s_ChanServ, u, CHAN_UNSUSPEND_ERROR);
        return MOD_CONT;
    }
    
    /* Only SUSPEND existing channels, otherwise use FORBID (bug #54) */
    if ((ci = cs_findchan(chan)) == NULL) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
        return MOD_CONT;
    }

    /* You should not SUSPEND a FORBIDEN channel */
    if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_MAY_NOT_BE_REGISTERED, chan);
        return MOD_CONT;
    }

    if (readonly)
        notice_lang(s_ChanServ, u, READ_ONLY_MODE);

    if (ci) {
        ci->flags |= CI_SUSPENDED;
        ci->forbidby = sstrdup(u->nick);
        if (reason)
            ci->forbidreason = sstrdup(reason);

        if ((c = findchan(ci->name))) {
            struct c_userlist *cu, *next;
            char *av[3];

            for (cu = c->users; cu; cu = next) {
                next = cu->next;

                if (is_oper(cu->user))
                    continue;

                av[0] = c->name;
                av[1] = cu->user->nick;
                av[2] = reason ? reason : "CHAN_SUSPEND_REASON";
                anope_cmd_kick(s_ChanServ, av[0], av[1], av[2]);
                do_kick(s_ChanServ, 3, av);
            }
        }

        if (WallForbid)
            anope_cmd_global(s_ChanServ,
                             "\2%s\2 used SUSPEND on channel \2%s\2",
                             u->nick, ci->name);

        alog("%s: %s set SUSPEND for channel %s", s_ChanServ, u->nick,
             ci->name);
        notice_lang(s_ChanServ, u, CHAN_SUSPEND_SUCCEEDED, chan);
        send_event(EVENT_CHAN_SUSPENDED, 1, chan);
    } else {
        alog("%s: Valid SUSPEND for %s by %s failed", s_ChanServ, ci->name,
             u->nick);
        notice_lang(s_ChanServ, u, CHAN_SUSPEND_FAILED, chan);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_unsuspend(User * u)
{
    ChannelInfo *ci;
    char *chan = strtok(NULL, " ");

    /* Assumes that permission checking has already been done. */
    if (!chan) {
        syntax_error(s_ChanServ, u, "UNSUSPEND", CHAN_UNSUSPEND_SYNTAX);
        return MOD_CONT;
    }
    if (chan[0] != '#') {
        notice_lang(s_ChanServ, u, CHAN_UNSUSPEND_ERROR);
        return MOD_CONT;
    }
    if (readonly)
        notice_lang(s_ChanServ, u, READ_ONLY_MODE);

    /* Only UNSUSPEND already suspended channels */
    if ((ci = cs_findchan(chan)) == NULL) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
        return MOD_CONT;
    }
    
    if (!(ci->flags & CI_SUSPENDED))
    {
        notice_lang(s_ChanServ, u, CHAN_UNSUSPEND_FAILED, chan);
        return MOD_CONT;
    }

    if (ci) {
        ci->flags &= ~CI_SUSPENDED;
        if (ci->forbidreason)
        {
        	free(ci->forbidreason);
        	ci->forbidreason = NULL;
        }
        if (ci->forbidby)
        {
        	free(ci->forbidby);
        	ci->forbidby = NULL;
        }

        if (WallForbid)
            anope_cmd_global(s_ChanServ,
                             "\2%s\2 used UNSUSPEND on channel \2%s\2",
                             u->nick, ci->name);

        alog("%s: %s set UNSUSPEND for channel %s", s_ChanServ, u->nick,
             ci->name);
        notice_lang(s_ChanServ, u, CHAN_UNSUSPEND_SUCCEEDED, chan);
        send_event(EVENT_CHAN_UNSUSPEND, 1, chan);
    } else {
        alog("%s: Valid UNSUSPEND for %s by %s failed", s_ChanServ,
             chan, u->nick);
        notice_lang(s_ChanServ, u, CHAN_UNSUSPEND_FAILED, chan);
    }
    return MOD_CONT;
}
