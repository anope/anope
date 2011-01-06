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

void myChanServHelp(User * u);
int do_util(User * u, CSModeUtil * util);
int do_op(User * u);
int do_deop(User * u);
int do_voice(User * u);
int do_devoice(User * u);
int do_halfop(User * u);
int do_dehalfop(User * u);
int do_protect(User * u);
int do_deprotect(User * u);
int do_owner(User * u);
int do_deowner(User * u);

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

    c = createCommand("OP", do_op, NULL, CHAN_HELP_OP, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("DEOP", do_deop, NULL, CHAN_HELP_DEOP, -1, -1, -1,
                      -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("VOICE", do_voice, NULL, CHAN_HELP_VOICE, -1, -1, -1,
                      -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("DEVOICE", do_devoice, NULL, CHAN_HELP_DEVOICE, -1,
                      -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("HALFOP", do_halfop, NULL, CHAN_HELP_HALFOP, -1,
                      -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("DEHALFOP", do_dehalfop, NULL, CHAN_HELP_DEHALFOP, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("PROTECT", do_protect, NULL, CHAN_HELP_PROTECT, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("DEPROTECT", do_deprotect, NULL, CHAN_HELP_DEPROTECT, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("OWNER", do_owner, NULL, CHAN_HELP_OWNER, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("DEOWNER", do_deowner, NULL, CHAN_HELP_DEOWNER, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("ADMIN", do_protect, NULL, CHAN_HELP_PROTECT, -1, -1, -1, -1);
    moduleAddCommand(CHANSERV, c, MOD_UNIQUE);
    c = createCommand("DEADMIN", do_deprotect, NULL, CHAN_HELP_DEPROTECT, -1, -1, -1, -1);
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
    if (ircd->owner) {
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_OWNER);
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DEOWNER);
    }
    if (ircd->protect) {
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_PROTECT);
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DEPROTECT);
    } else if (ircd->admin) {
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_ADMIN);
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DEADMIN);
    }

    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_OP);
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DEOP);
    if (ircd->halfop) {
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_HALFOP);
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DEHALFOP);
    }
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_VOICE);
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_DEVOICE);
}

/**
 * The /cs op/deop/voice/devoice etc.. commands.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/

int do_op(User * u)
{
    return do_util(u, &csmodeutils[MUT_OP]);
}

/*************************************************************************/

int do_deop(User * u)
{
    return do_util(u, &csmodeutils[MUT_DEOP]);
}

/*************************************************************************/

int do_voice(User * u)
{
    return do_util(u, &csmodeutils[MUT_VOICE]);
}

/*************************************************************************/

int do_devoice(User * u)
{
    return do_util(u, &csmodeutils[MUT_DEVOICE]);
}

/*************************************************************************/

int do_halfop(User * u)
{
    if (ircd->halfop) {
        return do_util(u, &csmodeutils[MUT_HALFOP]);
    } else {
        return MOD_CONT;
    }
}

/*************************************************************************/

int do_dehalfop(User * u)
{
    if (ircd->halfop) {
        return do_util(u, &csmodeutils[MUT_DEHALFOP]);
    } else {
        return MOD_CONT;
    }
}

/*************************************************************************/

int do_protect(User * u)
{
    if (ircd->protect || ircd->admin) {
        return do_util(u, &csmodeutils[MUT_PROTECT]);
    } else {
        return MOD_CONT;
    }
}

/*************************************************************************/

int do_deprotect(User * u)
{
    if (ircd->protect || ircd->admin) {
        return do_util(u, &csmodeutils[MUT_DEPROTECT]);
    } else {
        return MOD_CONT;
    }
}

/*************************************************************************/

int do_owner(User * u)
{
    char *av[2];
    char *chan = strtok(NULL, " ");

    Channel *c;
    ChannelInfo *ci;
    struct u_chanlist *uc;

    if (!ircd->owner) {
        return MOD_CONT;
    }

    if (!chan) {
        av[0] = sstrdup(ircd->ownerset);
        av[1] = GET_USER(u);

        /* Sets the mode to the user on every channels he is on. */

        for (uc = u->chans; uc; uc = uc->next) {
            if ((ci = uc->chan->ci) && !(ci->flags & CI_VERBOTEN)
                && is_founder(u, ci)) {
                anope_cmd_mode(whosends(ci), uc->chan->name, "%s %s",
                               av[0], GET_USER(u));
                chan_set_modes(s_ChanServ, uc->chan, 2, av, 1);
            }
        }

        free(av[0]);
        return MOD_CONT;
    }

    if (!(c = findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, c->name);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, ci->name);
    } else if (!is_founder(u, ci)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (!is_on_chan(c, u)) {
        notice_lang(s_ChanServ, u, NICK_X_NOT_ON_CHAN, u->nick, c->name);
    } else {
        anope_cmd_mode(whosends(ci), c->name, "%s %s", ircd->ownerset,
                       GET_USER(u));

        av[0] = sstrdup(ircd->ownerset);
        av[1] = GET_USER(u);
        chan_set_modes(s_ChanServ, c, 2, av, 1);
        free(av[0]);
    }
    return MOD_CONT;
}

/*************************************************************************/

int do_deowner(User * u)
{
    char *av[2];
    char *chan = strtok(NULL, " ");

    Channel *c;
    ChannelInfo *ci;
    struct u_chanlist *uc;

    if (!ircd->owner) {
        return MOD_CONT;
    }

    if (!chan) {
        av[0] = sstrdup(ircd->ownerunset);
        av[1] = GET_USER(u);

        /* Sets the mode to the user on every channels he is on. */

        for (uc = u->chans; uc; uc = uc->next) {
            if ((ci = uc->chan->ci) && !(ci->flags & CI_VERBOTEN)
                && is_founder(u, ci)) {
                anope_cmd_mode(whosends(ci), uc->chan->name, "%s %s",
                               av[0], GET_USER(u));
                chan_set_modes(s_ChanServ, uc->chan, 2, av, 1);
            }
        }

        free(av[0]);
        return MOD_CONT;
    }

    if (!(c = findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, c->name);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, ci->name);
    } else if (!is_founder(u, ci)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (!is_on_chan(c, u)) {
        notice_lang(s_ChanServ, u, NICK_X_NOT_ON_CHAN, u->nick, c->name);
    } else {
        anope_cmd_mode(whosends(ci), c->name, "%s %s", ircd->ownerunset,
                       GET_USER(u));

        av[0] = sstrdup(ircd->ownerunset);
        av[1] = GET_USER(u);
        chan_set_modes(s_ChanServ, c, 2, av, 1);
        free(av[0]);
    }
    return MOD_CONT;
}

/* do_util: not a command, but does the job of other */

int do_util(User * u, CSModeUtil * util)
{
    char *av[2];
    char *chan = strtok(NULL, " ");
    char *nick = strtok(NULL, " ");

    Channel *c;
    ChannelInfo *ci;
    User *u2;

    int is_same;

    if (!chan) {
        struct u_chanlist *uc;

        av[0] = util->mode;
        av[1] = u->nick;

        /* Sets the mode to the user on every channels he is on. */

        for (uc = u->chans; uc; uc = uc->next) {
            if ((ci = uc->chan->ci) && !(ci->flags & CI_VERBOTEN)
                && check_access(u, ci, util->levelself)) {
                anope_cmd_mode(whosends(ci), uc->chan->name, "%s %s",
                               util->mode, GET_USER(u));
                chan_set_modes(s_ChanServ, uc->chan, 2, av, 2);

                if (util->notice && ci->flags & util->notice)
                    notice(whosends(ci), uc->chan->name,
                           "%s command used for %s by %s", util->name,
                           u->nick, u->nick);
            }
        }

        return MOD_CONT;
    } else if (!nick) {
        nick = u->nick;
    }

    is_same = (nick == u->nick) ? 1 : (stricmp(nick, u->nick) == 0);

    if (!(c = findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, c->name);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, ci->name);
    } else if (is_same ? !(u2 = u) : !(u2 = finduser(nick))) {
        notice_lang(s_ChanServ, u, NICK_X_NOT_IN_USE, nick);
    } else if (is_same ? !check_access(u, ci, util->levelself) :
               !check_access(u, ci, util->level)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (*util->mode == '-' && !is_same && (ci->flags & CI_PEACE)
               && (get_access(u2, ci) >= get_access(u, ci))) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else if (*util->mode == '-' && is_protected(u2) && !is_same) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else if (!is_on_chan(c, u2)) {
        notice_lang(s_ChanServ, u, NICK_X_NOT_ON_CHAN, u2->nick, c->name);
    } else {
        anope_cmd_mode(whosends(ci), c->name, "%s %s", util->mode,
                       GET_USER(u2));

        av[0] = util->mode;
        av[1] = GET_USER(u2);
        chan_set_modes(s_ChanServ, c, 2, av, 3);

        if (util->notice && ci->flags & util->notice)
            notice(whosends(ci), c->name, "%s command used for %s by %s",
                   util->name, u2->nick, u->nick);
    }
    return MOD_CONT;
}
