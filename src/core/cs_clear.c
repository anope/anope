/* ChanServ core functions
 *
 * (C) 2003-2007 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

int do_clear(User * u);
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
    moduleAddVersion("$Id$");
    moduleSetType(CORE);

    c = createCommand("CLEAR", do_clear, NULL, CHAN_HELP_CLEAR, -1, -1, -1,
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
    notice_lang(s_ChanServ, u, CHAN_HELP_CMD_CLEAR);
}

/**
 * The /cs clear command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_clear(User * u)
{
    char *chan = strtok(NULL, " ");
    char *what = strtok(NULL, " ");
    char tmp[BUFSIZE];
    Channel *c;
    ChannelInfo *ci;

    if (!what) {
        syntax_error(s_ChanServ, u, "CLEAR", CHAN_CLEAR_SYNTAX);
    } else if (!(c = findchan(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
    } else if (!(ci = c->ci)) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CI_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!u || !check_access(u, ci, CA_CLEAR)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
    } else if (stricmp(what, "bans") == 0) {
        char *av[2];
        int i;

        /* Save original ban info */
        int count = c->bancount;
        char **bans = scalloc(sizeof(char *) * count, 1);
        for (i = 0; i < count; i++)
            bans[i] = sstrdup(c->bans[i]);

        for (i = 0; i < count; i++) {
            av[0] = sstrdup("-b");
            av[1] = bans[i];
            anope_cmd_mode(whosends(ci), chan, "%s %s", av[0], av[1]);
            chan_set_modes(whosends(ci), c, 2, av, 0);
            free(av[1]);
            free(av[0]);
        }
        notice_lang(s_ChanServ, u, CHAN_CLEARED_BANS, chan);
        free(bans);
    } else if (ircd->except && stricmp(what, "excepts") == 0) {
        char *av[2];
        int i;

        /* Save original except info */
        int count = c->exceptcount;
        char **excepts = scalloc(sizeof(char *) * count, 1);
        for (i = 0; i < count; i++)
            excepts[i] = sstrdup(c->excepts[i]);

        for (i = 0; i < count; i++) {
            av[0] = sstrdup("-e");
            av[1] = excepts[i];
            anope_cmd_mode(whosends(ci), chan, "%s %s", av[0], av[1]);
            chan_set_modes(whosends(ci), c, 2, av, 0);
            free(av[1]);
            free(av[0]);
        }
        notice_lang(s_ChanServ, u, CHAN_CLEARED_EXCEPTS, chan);
        free(excepts);

    } else if (ircd->invitemode && stricmp(what, "invites") == 0) {
        char *av[2];
        int i;

        /* Save original except info */
        int count = c->invitecount;
        char **invites = scalloc(sizeof(char *) * count, 1);
        for (i = 0; i < count; i++)
            invites[i] = sstrdup(c->invite[i]);

        for (i = 0; i < count; i++) {
            av[0] = sstrdup("-I");
            av[1] = invites[i];
            anope_cmd_mode(whosends(ci), chan, "%s %s", av[0], av[1]);
            chan_set_modes(whosends(ci), c, 2, av, 0);
            free(av[1]);
            free(av[0]);
        }
        notice_lang(s_ChanServ, u, CHAN_CLEARED_INVITES, chan);
        free(invites);

    } else if (stricmp(what, "modes") == 0) {
        char buf[BUFSIZE], *end = buf;
        char *argv[2];

        if (c->mode) {
            /* Clear modes the bulk of the modes */
            anope_cmd_mode(whosends(ci), c->name, "%s",
                           ircd->modestoremove);
            argv[0] = sstrdup(ircd->modestoremove);
            chan_set_modes(whosends(ci), c, 1, argv, 0);
            free(argv[0]);

            /* to prevent the internals from complaining send -k, -L, -f by themselves if we need
               to send them - TSL */
            if (c->key) {
                anope_cmd_mode(whosends(ci), c->name, "-k %s", c->key);
                argv[0] = sstrdup("-k");
                argv[1] = c->key;
                chan_set_modes(whosends(ci), c, 2, argv, 0);
                free(argv[0]);
            }
            if (ircd->Lmode && c->redirect) {
                anope_cmd_mode(whosends(ci), c->name, "-L %s",
                               c->redirect);
                argv[0] = sstrdup("-L");
                argv[1] = c->redirect;
                chan_set_modes(whosends(ci), c, 2, argv, 0);
                free(argv[0]);
            }
            if (ircd->fmode && c->flood) {
                if (flood_mode_char_remove) {
                    anope_cmd_mode(whosends(ci), c->name, "%s %s",
                                   flood_mode_char_remove, c->flood);
                    argv[0] = sstrdup(flood_mode_char_remove);
                    argv[1] = c->flood;
                    chan_set_modes(whosends(ci), c, 2, argv, 0);
                    free(argv[0]);
                } else {
                    if (debug) {
                        alog("debug: flood_mode_char_remove was not set unable to remove flood/throttle modes");
                    }
                }
            }
            check_modes(c);
        }



        /* TODO: decide if the above implementation is better than this one. */

        if (0) {
            CBModeInfo *cbmi = cbmodeinfos;
            CBMode *cbm;

            do {
                if (c->mode & cbmi->flag)
                    *end++ = cbmi->mode;
            } while ((++cbmi)->flag != 0);

            cbmi = cbmodeinfos;

            do {
                if (cbmi->getvalue && (c->mode & cbmi->flag)
                    && !(cbmi->flags & CBM_MINUS_NO_ARG)) {
                    char *value = cbmi->getvalue(c);

                    if (value) {
                        *end++ = ' ';
                        while (*value)
                            *end++ = *value++;

                        /* Free the value */
                        cbm = &cbmodes[(int) cbmi->mode];
                        cbm->setvalue(c, NULL);
                    }
                }
            } while ((++cbmi)->flag != 0);

            *end = 0;

            anope_cmd_mode(whosends(ci), c->name, "-%s", buf);
            c->mode = 0;
            check_modes(c);
        }
        notice_lang(s_ChanServ, u, CHAN_CLEARED_MODES, chan);
    } else if (stricmp(what, "ops") == 0) {
        char *av[3];
        struct c_userlist *cu, *next;

        if (ircd->svsmode_ucmode) {
            av[0] = sstrdup(chan);
            anope_cmd_svsmode_chan(av[0], "-o", NULL);
            if (ircd->owner) {
                anope_cmd_svsmode_chan(av[0], ircd->ownerunset, NULL);
            }
            if (ircd->protect || ircd->admin) {
                anope_cmd_svsmode_chan(av[0], ircd->adminunset, NULL);
            }
            for (cu = c->users; cu; cu = next) {
                next = cu->next;
                av[0] = sstrdup(chan);
                if (!chan_has_user_status(c, cu->user, CUS_OP)) {
                    if (!chan_has_user_status(c, cu->user, CUS_PROTECT)) {
                        if (!chan_has_user_status(c, cu->user, CUS_OWNER)) {
                            continue;
                        } else {
                            snprintf(tmp, BUFSIZE, "%so",
                                     ircd->ownerunset);
                            av[1] = sstrdup(tmp);

                        }
                    } else {
                        snprintf(tmp, BUFSIZE, "%so", ircd->adminunset);
                        av[1] = sstrdup(tmp);
                    }
                } else {
                    av[1] = sstrdup("-o");
                }
                av[2] = sstrdup(cu->user->nick);
                do_cmode(s_ChanServ, 3, av);
                free(av[2]);
                free(av[1]);
                free(av[0]);
            }
        } else {
            for (cu = c->users; cu; cu = next) {
                next = cu->next;
                av[0] = sstrdup(chan);
                if (!chan_has_user_status(c, cu->user, CUS_OP)) {
                    if (!chan_has_user_status(c, cu->user, CUS_PROTECT)) {
                        if (!chan_has_user_status(c, cu->user, CUS_OWNER)) {
                            continue;
                        } else {
                            snprintf(tmp, BUFSIZE, "%so",
                                     ircd->ownerunset);
                            av[1] = sstrdup(tmp);
                        }
                    } else {
                        snprintf(tmp, BUFSIZE, "%so", ircd->adminunset);
                        av[1] = sstrdup(tmp);
                    }
                } else {
                    av[1] = sstrdup("-o");
                }
                av[2] = sstrdup(cu->user->nick);
                anope_cmd_mode(whosends(ci), av[0], "%s :%s", av[1],
                               av[2]);
                do_cmode(s_ChanServ, 3, av);
                free(av[2]);
                free(av[1]);
                free(av[0]);
            }
        }
        notice_lang(s_ChanServ, u, CHAN_CLEARED_OPS, chan);
    } else if (ircd->halfop && stricmp(what, "hops") == 0) {
        char *av[3];
        struct c_userlist *cu, *next;

        for (cu = c->users; cu; cu = next) {
            next = cu->next;
            if (!chan_has_user_status(c, cu->user, CUS_HALFOP))
                continue;
            av[0] = sstrdup(chan);
            av[1] = sstrdup("-h");
            av[2] = sstrdup(cu->user->nick);
            if (ircd->svsmode_ucmode) {
                anope_cmd_svsmode_chan(av[0], av[1], NULL);
                do_cmode(s_ChanServ, 3, av);
                break;
            } else {
                anope_cmd_mode(whosends(ci), av[0], "%s :%s", av[1],
                               av[2]);
            }
            do_cmode(s_ChanServ, 3, av);
            free(av[2]);
            free(av[1]);
            free(av[0]);
        }
        notice_lang(s_ChanServ, u, CHAN_CLEARED_HOPS, chan);
    } else if (stricmp(what, "voices") == 0) {
        char *av[3];
        struct c_userlist *cu, *next;

        for (cu = c->users; cu; cu = next) {
            next = cu->next;
            if (!chan_has_user_status(c, cu->user, CUS_VOICE))
                continue;
            av[0] = sstrdup(chan);
            av[1] = sstrdup("-v");
            av[2] = sstrdup(cu->user->nick);
            if (ircd->svsmode_ucmode) {
                anope_cmd_svsmode_chan(av[0], av[1], NULL);
                do_cmode(s_ChanServ, 3, av);
                break;
            } else {
                anope_cmd_mode(whosends(ci), av[0], "%s :%s", av[1],
                               av[2]);
            }
            do_cmode(s_ChanServ, 3, av);
            free(av[2]);
            free(av[1]);
            free(av[0]);
        }
        notice_lang(s_ChanServ, u, CHAN_CLEARED_VOICES, chan);
    } else if (stricmp(what, "users") == 0) {
        char *av[3];
        struct c_userlist *cu, *next;
        char buf[256];

        snprintf(buf, sizeof(buf), "CLEAR USERS command from %s", u->nick);

        for (cu = c->users; cu; cu = next) {
            next = cu->next;
            av[0] = sstrdup(chan);
            av[1] = sstrdup(cu->user->nick);
            av[2] = sstrdup(buf);
            anope_cmd_kick(whosends(ci), av[0], av[1], av[2]);
            do_kick(s_ChanServ, 3, av);
            free(av[2]);
            free(av[1]);
            free(av[0]);
        }
        notice_lang(s_ChanServ, u, CHAN_CLEARED_USERS, chan);
    } else {
        syntax_error(s_ChanServ, u, "CLEAR", CHAN_CLEAR_SYNTAX);
    }
    return MOD_CONT;
}
