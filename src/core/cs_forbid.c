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

int do_forbid(User * u);
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

    c = createCommand("FORBID", do_forbid, is_services_admin, -1, -1, -1,
                      CHAN_SERVADMIN_HELP_FORBID,
                      CHAN_SERVADMIN_HELP_FORBID);
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
        notice_lang(s_ChanServ, u, CHAN_HELP_CMD_FORBID);
    }
}

/**
 * The /cs forbid command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_forbid(User * u)
{
    Channel *c;
    ChannelInfo *ci;
    char *chan = strtok(NULL, " ");
    char *reason = strtok(NULL, "");
    Entry *cur, *enext;

    /* Assumes that permission checking has already been done. */
    if (!chan || (ForceForbidReason && !reason)) {
        syntax_error(s_ChanServ, u, "FORBID",
                     (ForceForbidReason ? CHAN_FORBID_SYNTAX_REASON :
                      CHAN_FORBID_SYNTAX));
        return MOD_CONT;
    }
    if (*chan != '#') {
        notice_lang(s_ChanServ, u, CHAN_SYMBOL_REQUIRED);
        return MOD_CONT;
    } else if (!anope_valid_chan(chan)) {
        notice_lang(s_ChanServ, u, CHAN_X_INVALID, chan);
        return MOD_CONT;
    }
    if (readonly)
        notice_lang(s_ChanServ, u, READ_ONLY_MODE);
    if ((ci = cs_findchan(chan)) != NULL)
        delchan(ci);
    ci = makechan(chan);
    if (ci) {
        ci->flags |= CI_VERBOTEN;
        ci->forbidby = sstrdup(u->nick);
        if (reason)
            ci->forbidreason = sstrdup(reason);

        if ((c = findchan(ci->name))) {
            struct c_userlist *cu, *next;
            char *av[3];

            /* Before banning everyone, it might be prudent to clear +e and +I lists.. 
             * to prevent ppl from rejoining.. ~ Viper */
            if (ircd->except && c->excepts && c->excepts->count) {
                av[0] = sstrdup("-e");
                for (cur = c->excepts->entries; cur; cur = enext) {
                    enext = cur->next;
                    av[1] = sstrdup(cur->mask);
                    anope_cmd_mode(whosends(ci), chan, "-e %s", cur->mask);
                    chan_set_modes(whosends(ci), c, 2, av, 0);
                    free(av[1]);
                }
                free(av[0]);
            }
            if (ircd->invitemode && c->invites && c->invites->count) {
                av[0] = sstrdup("-I");
                for (cur = c->invites->entries; cur; cur = enext) {
                    enext = cur->next;
                    av[1] = sstrdup(cur->mask);
                    anope_cmd_mode(whosends(ci), chan, "-I %s", cur->mask);
                    chan_set_modes(whosends(ci), c, 2, av, 0);
                    free(av[1]);
                }
                free(av[0]);
            }

            for (cu = c->users; cu; cu = next) {
                next = cu->next;

                if (is_oper(cu->user))
                    continue;

                av[0] = c->name;
                av[1] = cu->user->nick;
                av[2] = reason ? reason : "CHAN_FORBID_REASON";
                anope_cmd_kick(s_ChanServ, av[0], av[1], av[2]);
                do_kick(s_ChanServ, 3, av);
            }
        }

        if (WallForbid)
            anope_cmd_global(s_ChanServ,
                             "\2%s\2 used FORBID on channel \2%s\2",
                             u->nick, ci->name);

        if (ircd->chansqline) {
            anope_cmd_sqline(ci->name, ((reason) ? reason : "Forbidden"));
        }

        alog("%s: %s set FORBID for channel %s", s_ChanServ, u->nick,
             ci->name);
        notice_lang(s_ChanServ, u, CHAN_FORBID_SUCCEEDED, chan);
        send_event(EVENT_CHAN_FORBIDDEN, 1, chan);
    } else {
        alog("%s: Valid FORBID for %s by %s failed", s_ChanServ, ci->name,
             u->nick);
        notice_lang(s_ChanServ, u, CHAN_FORBID_FAILED, chan);
    }
    return MOD_CONT;
}
