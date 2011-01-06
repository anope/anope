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

int do_unban(User * u);
int do_ban(User * u);
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

    c = createCommand("BAN", do_ban, NULL, CHAN_HELP_BAN, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("UNBAN", do_unban, NULL, CHAN_HELP_UNBAN, -1, -1, -1,
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
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_BAN);
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_UNBAN);
}

/**
 * The /cs ban command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_ban(User * u)
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

        /* Bans the user on every channels he is on. */

        for (uc = u->chans; uc; uc = next) {
            next = uc->next;
            if ((ci = uc->chan->ci) && !(ci->flags & CI_VERBOTEN)
                && check_access(u, ci, CA_BANME)) {
                char *av[3];
                char mask[BUFSIZE];

                /*
                 * Dont ban/kick the user on channels where he is excepted
                 * to prevent services <-> server wars.
                 */
                if (ircd->except) {
                    if (is_excepted(ci, u))
                        notice_lang(s_ChanServ, u, CHAN_EXCEPTED,
                                    u->nick, ci->name);
                    continue;
                }
                if (is_protected(u)) {
                    notice_lang(s_ChanServ, u, PERMISSION_DENIED);
                    continue;
                }

                av[0] = sstrdup("+b");
                get_idealban(ci, u, mask, sizeof(mask));
                av[1] = mask;
                anope_cmd_mode(whosends(ci), uc->chan->name, "+b %s",
                               av[1]);
                chan_set_modes(s_ChanServ, uc->chan, 2, av, 1);
                free(av[0]);

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
    } else if (!is_same ? !check_access(u, ci, CA_BAN) :
               !check_access(u, ci, CA_BANME)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (!is_same && (ci->flags & CI_PEACE)
               && (get_access(u2, ci) >= get_access(u, ci))) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
        /*
         * Dont ban/kick the user on channels where he is excepted
         * to prevent services <-> server wars.
         */
    } else if (ircd->except && is_excepted(ci, u2)) {
        notice_lang(s_ChanServ, u, CHAN_EXCEPTED, u2->nick, ci->name);
    } else if (ircd->protectedumode && is_protected(u2)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else {
        char *av[3];
        char mask[BUFSIZE];

        av[0] = sstrdup("+b");
        get_idealban(ci, u2, mask, sizeof(mask));
        av[1] = mask;
        anope_cmd_mode(whosends(ci), c->name, "+b %s", av[1]);
        chan_set_modes(s_ChanServ, c, 2, av, 1);
        free(av[0]);

        /* We still allow host banning while not allowing to kick */
        if (!is_on_chan(c, u2))
            return MOD_CONT;

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

/**
 * The /cs unban command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_unban(User * u)
{
    char *chan = strtok(NULL, " ");
    Channel *c;
    ChannelInfo *ci;

    if (!chan) {
        syntax_error(s_ChanServ, u, "UNBAN", CHAN_UNBAN_SYNTAX);
    } else if (!(c = findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!check_access(u, ci, CA_UNBAN)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else {
        common_unban_full(ci, u->nick, true);
        notice_lang(s_ChanServ, u, CHAN_UNBANNED, chan);
    }
    return MOD_CONT;
}
