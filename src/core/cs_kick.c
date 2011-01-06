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

int do_cs_kick(User * u);
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

    c = createCommand("KICK", do_cs_kick, NULL, CHAN_HELP_KICK, -1, -1, -1,
                      -1);
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
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_KICK);
}

/**
 * The /cs kick command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_cs_kick(User * u)
{
    char *chan = strtok(NULL, " ");
    char *params = strtok(NULL, " ");
    char *reason = strtok(NULL, "");

    Channel *c;
    ChannelInfo *ci;
    User *u2;

    int is_same;

    if (!reason) {
        reason = "Requested";
    } else {
        if (strlen(reason) > 200)
            reason[200] = '\0';
    }

    if (!chan) {
        struct u_chanlist *uc, *next;

        /* Kicks the user on every channels he is on. */

        for (uc = u->chans; uc; uc = next) {
            next = uc->next;
            if ((ci = uc->chan->ci) && !(ci->flags & CI_VERBOTEN)
                && check_access(u, ci, CA_KICKME)) {
                char *av[3];

                if ((ci->flags & CI_SIGNKICK)
                    || ((ci->flags & CI_SIGNKICK_LEVEL)
                        && !check_access(u, ci, CA_SIGNKICK)))
                    anope_cmd_kick(whosends(ci), ci->name, u->nick,
                                   "%s (%s)", reason, u->nick);
                else
                    anope_cmd_kick(whosends(ci), ci->name, u->nick, "%s",
                                   reason);
                av[0] = ci->name;
                av[1] = u->nick;
                av[2] = reason;
                do_kick(s_ChanServ, 3, av);
            }
        }

        return MOD_CONT;
    } else if (!params) {
        params = u->nick;
    }

    is_same = (params == u->nick) ? 1 : (stricmp(params, u->nick) == 0);

    if (!(c = findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (is_same ? !(u2 = u) : !(u2 = finduser(params))) {
        notice_lang(s_ChanServ, u, NICK_X_NOT_IN_USE, params);
    } else if (!is_same ? !check_access(u, ci, CA_KICK) :
               !check_access(u, ci, CA_KICKME)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (!is_same && (ci->flags & CI_PEACE)
               && (get_access(u2, ci) >= get_access(u, ci))) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else if (is_protected(u2)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else if (!is_on_chan(c, u2)) {
        notice_lang(s_ChanServ, u, NICK_X_NOT_ON_CHAN, u2->nick, c->name);
    } else {
        char *av[3];

        if ((ci->flags & CI_SIGNKICK)
            || ((ci->flags & CI_SIGNKICK_LEVEL)
                && !check_access(u, ci, CA_SIGNKICK)))
            anope_cmd_kick(whosends(ci), ci->name, params, "%s (%s)",
                           reason, u->nick);
        else
            anope_cmd_kick(whosends(ci), ci->name, params, "%s", reason);
        av[0] = ci->name;
        av[1] = params;
        av[2] = reason;
        do_kick(s_ChanServ, 3, av);
    }
    return MOD_CONT;
}
