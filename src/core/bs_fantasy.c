/* BotServ core fantasy functions
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

int do_fantasy(int argc, char **argv);

/**
 * Create the hook, and tell anope about it.
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT to allow the module, MOD_STOP to stop it
 **/
int AnopeInit(int argc, char **argv)
{
    EvtHook *hook;

    moduleAddAuthor("Anope");
    moduleAddVersion(VERSION_STRING);
    moduleSetType(CORE);
    hook = createEventHook(EVENT_BOT_FANTASY, do_fantasy);
    moduleAddEventHook(hook);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}

/**
 * Handle all csmodeutils fantasy commands.
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT or MOD_STOP
 **/
int do_fantasy(int argc, char **argv)
{
    User *u;
    ChannelInfo *ci;
    CSModeUtil *util = csmodeutils;
    char *target;

    if (argc < 3)
        return MOD_CONT;

    do {
        if (stricmp(argv[0], util->bsname) == 0) {
            /* This could have been moved to its own module
               however it would require more coding to handle the pass holders
               similar to how PROTECT is done 
            */
            if (!ircd->halfop) {
                if (!stricmp(argv[0], "halfop") || !stricmp(argv[0], "dehalfop")) {
                    return MOD_CONT;
                }
            }
            u = finduser(argv[1]);
            ci = cs_findchan(argv[2]);
            if (!u || !ci)
                return MOD_CONT;

            target = ((argc == 4) ? argv[3] : NULL);

            if (!target && check_access(u, ci, util->levelself))
                bot_raw_mode(u, ci, util->mode, u->nick);
            else if (target && check_access(u, ci, util->level))
                bot_raw_mode(u, ci, util->mode, target);
        }
    } while ((++util)->name != NULL);

    return MOD_CONT;
}
