/* BotServ core fantasy functions
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

int do_fantasy(int argc, char **argv);

class BSFantasy : public Module
{
 public:
	BSFantasy(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		EvtHook *hook;

		moduleAddAuthor("Anope");
		moduleAddVersion("$Id$");
		this->SetType(CORE);
		hook = createEventHook(EVENT_BOT_FANTASY, do_fantasy);
		moduleAddEventHook(hook);
	}
};

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

MODULE_INIT("bs_fantasy", BSFantasy)
