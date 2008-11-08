/* OperServ core functions
 *
 * (C) 2003-2008 Anope Team
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

int do_clearmodes(User * u);
void myOperServHelp(User * u);

class OSClearModes : public Module
{
 public:
	OSClearModes(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		moduleAddAuthor("Anope");
		moduleAddVersion
		("$Id$");
		moduleSetType(this, CORE);

		c = createCommand("CLEARMODES", do_clearmodes, is_services_oper, OPER_HELP_CLEARMODES, -1, -1, -1, -1);
		moduleAddCommand(OPERSERV, c, MOD_UNIQUE);

		moduleSetOperHelp(myOperServHelp);
	}
};


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
    if (is_services_oper(u)) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_CLEARMODES);
    }
}

/**
 * The /os clearmodes command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_clearmodes(User * u)
{
    char *s;
    const char *argv[2];
    char *chan = strtok(NULL, " ");
    Channel *c;
    int all = 0;
    struct c_userlist *cu, *next;
    Entry *entry, *nexte;

    if (!chan) {
        syntax_error(s_OperServ, u, "CLEARMODES", OPER_CLEARMODES_SYNTAX);
        return MOD_CONT;
    } else if (!(c = findchan(chan))) {
        notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
        return MOD_CONT;
    } else if (c->bouncy_modes) {
        notice_lang(s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
        return MOD_CONT;
    } else {
        s = strtok(NULL, " ");
        if (s) {
            if (stricmp(s, "ALL") == 0) {
                all = 1;
            } else {
                syntax_error(s_OperServ, u, "CLEARMODES",
                             OPER_CLEARMODES_SYNTAX);
                return MOD_CONT;
            }
        }

        if (WallOSClearmodes) {
            ircdproto->SendGlobops(s_OperServ, "%s used CLEARMODES%s on %s",
                             u->nick, all ? " ALL" : "", chan);
        }
        if (all) {
            /* Clear mode +o */
            if (ircd->svsmode_ucmode) {
                ircdproto->SendSVSModeChan(c->name, "-o", NULL);
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;
                    if (!chan_has_user_status(c, cu->user, CUS_OP)) {
                        continue;
                    }
                    argv[0] = "-o";
                    argv[1] = cu->user->nick;
                    chan_set_modes(s_OperServ, c, 2, argv, 0);
                }
            } else {
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;
                    if (!chan_has_user_status(c, cu->user, CUS_OP))
                        continue;
                    argv[0] = "-o";
                    argv[1] = cu->user->nick;
                    ircdproto->SendMode(findbot(s_OperServ), c->name, "-o %s",
                                   cu->user->nick);
                    chan_set_modes(s_OperServ, c, 2, argv, 0);
                }
            }

            if (ircd->svsmode_ucmode) {
                ircdproto->SendSVSModeChan(c->name, "-v", NULL);
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;
                    if (!chan_has_user_status(c, cu->user, CUS_VOICE)) {
                        continue;
                    }
                    argv[0] = "-v";
                    argv[1] = cu->user->nick;
                    chan_set_modes(s_OperServ, c, 2, argv, 0);
                }
            } else {
                /* Clear mode +v */
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;
                    if (!chan_has_user_status(c, cu->user, CUS_VOICE))
                        continue;
                    argv[0] = "-v";
                    argv[1] = cu->user->nick;
                    ircdproto->SendMode(findbot(s_OperServ), c->name, "-v %s",
                                   cu->user->nick);
                    chan_set_modes(s_OperServ, c, 2, argv, 0);
                }
            }

            /* Clear mode +h */
            if (ircd->svsmode_ucmode && ircd->halfop) {
                ircdproto->SendSVSModeChan(c->name, "-h", NULL);
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;
                    if (!chan_has_user_status(c, cu->user, CUS_HALFOP)) {
                        continue;
                    }
                    argv[0] = "-h";
                    argv[1] = cu->user->nick;
                    chan_set_modes(s_OperServ, c, 2, argv, 0);
                }
            } else {
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;
                    if (!chan_has_user_status(c, cu->user, CUS_HALFOP))
                        continue;
                    argv[0] = "-h";
                    argv[1] = cu->user->nick;
                    ircdproto->SendMode(findbot(s_OperServ), c->name, "-h %s",
                                   cu->user->nick);
                    chan_set_modes(s_OperServ, c, 2, argv, 0);
                }
            }
            /* Clear mode Owners */
            if (ircd->svsmode_ucmode && ircd->owner) {
                ircdproto->SendSVSModeChan(c->name, ircd->ownerunset, NULL);
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;
                    if (!chan_has_user_status(c, cu->user, CUS_HALFOP)) {
                        continue;
                    }
                    argv[0] = ircd->ownerunset;
                    argv[1] = cu->user->nick;
                    chan_set_modes(s_OperServ, c, 2, argv, 0);
                }
            } else {
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;
                    if (!chan_has_user_status(c, cu->user, CUS_OWNER))
                        continue;
                    argv[0] = ircd->ownerunset;
                    argv[1] = cu->user->nick;
                    ircdproto->SendMode(findbot(s_OperServ), c->name, "%s %s",
                                   ircd->ownerunset, cu->user->nick);
                    chan_set_modes(s_OperServ, c, 2, argv, 0);
                }
            }
            /* Clear mode protected or admins */
            if (ircd->svsmode_ucmode && (ircd->protect || ircd->admin)) {

                ircdproto->SendSVSModeChan(c->name, ircd->adminunset, NULL);
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;
                    if (!chan_has_user_status(c, cu->user, CUS_HALFOP)) {
                        continue;
                    }
                    argv[0] = ircd->adminunset;
                    argv[1] = cu->user->nick;
                    chan_set_modes(s_OperServ, c, 2, argv, 0);
                }
            } else {
                for (cu = c->users; cu; cu = next) {
                    next = cu->next;
                    if (!chan_has_user_status(c, cu->user, CUS_PROTECT))
                        continue;
                    argv[0] = ircd->adminunset;
                    argv[1] = cu->user->nick;
                    ircdproto->SendMode(findbot(s_OperServ), c->name, "%s %s",
                                   ircd->adminunset, cu->user->nick);
                    chan_set_modes(s_OperServ, c, 2, argv, 0);
                }
            }


        }

        if (c->mode) {
            /* Clear modes the bulk of the modes */
            ircdproto->SendMode(findbot(s_OperServ), c->name, "%s", ircd->modestoremove);
            argv[0] = ircd->modestoremove;
            chan_set_modes(s_OperServ, c, 1, argv, 0);

            /* to prevent the internals from complaining send -k, -L, -f by themselves if we need
               to send them - TSL */
            if (c->key) {
                ircdproto->SendMode(findbot(s_OperServ), c->name, "-k %s", c->key);
                argv[0] = "-k";
                argv[1] = c->key;
                chan_set_modes(s_OperServ, c, 2, argv, 0);
            }
            if (ircd->Lmode && c->redirect) {
                ircdproto->SendMode(findbot(s_OperServ), c->name, "-L %s", c->redirect);
                argv[0] = "-L";
                argv[1] = c->redirect;
                chan_set_modes(s_OperServ, c, 2, argv, 0);
            }
            if (ircd->fmode && c->flood) {
                if (flood_mode_char_remove) {
                    ircdproto->SendMode(findbot(s_OperServ), c->name, "%s %s",
                                   flood_mode_char_remove, c->flood);
                    argv[0] = flood_mode_char_remove;
                    argv[1] = c->flood;
                    chan_set_modes(s_OperServ, c, 2, argv, 0);
                } else {
                    if (debug) {
                        alog("debug: flood_mode_char_remove was not set unable to remove flood/throttle modes");
                    }
                }
            }
        }

        /* Clear bans */
        if (c->bans && c->bans->count) {
            for (entry = c->bans->entries; entry; entry = nexte) {
                nexte = entry->next;
                argv[0] = "-b";
                argv[1] = entry->mask;
                ircdproto->SendMode(findbot(s_OperServ), c->name, "-b %s", entry->mask);
                chan_set_modes(s_OperServ, c, 2, argv, 0);
            }
        }

        /* Clear excepts */
        if (ircd->except && c->excepts && c->excepts->count) {
            for (entry = c->excepts->entries; entry; entry = nexte) {
                nexte = entry->next;
                argv[0] = "-e";
                argv[1] = entry->mask;
                ircdproto->SendMode(findbot(s_OperServ), c->name, "-e %s", entry->mask);
                chan_set_modes(s_OperServ, c, 2, argv, 0);
            }
        }

        /* Clear invites */
        if (ircd->invitemode && c->invites && c->invites->count) {
            for (entry = c->invites->entries; entry; entry = nexte) {
                nexte = entry->next;
                argv[0] = "-I";
                argv[1] = entry->mask;
                ircdproto->SendMode(findbot(s_OperServ), c->name, "-I %s", entry->mask);
                chan_set_modes(s_OperServ, c, 2, argv, 0);
            }
        }
    }

    if (all) {
        notice_lang(s_OperServ, u, OPER_CLEARMODES_ALL_DONE, chan);
    } else {
        notice_lang(s_OperServ, u, OPER_CLEARMODES_DONE, chan);
    }
    return MOD_CONT;
}

MODULE_INIT("os_clearmodes", OSClearModes)
