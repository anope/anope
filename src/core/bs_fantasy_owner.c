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

class BSFantasyOwner : public Module
{
 public:
	BSFantasyOwner(const std::string &modname, const std::string &creator) : Module(creator)
	{
		EvtHook *hook;

		moduleAddAuthor("Anope");
		moduleAddVersion("$Id$");
		moduleSetType(CORE);

		/* No need to load of we don't support owner */
		if (!ircd->owner)
		{
			throw ModuleException("Your ircd doesn't support the owner channelmode; bs_fantasy_owner won't be loaded");
		}

		hook = createEventHook(EVENT_BOT_FANTASY, do_fantasy);
		moduleAddEventHook(hook);
	}
};


/**
 * Handle owner/deowner fantasy commands.
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT or MOD_STOP
 **/
int do_fantasy(int argc, char **argv)
{
    User *u;
    ChannelInfo *ci;

    if (argc < 3)
        return MOD_CONT;

    if (stricmp(argv[0], "deowner") == 0) {
        u = finduser(argv[1]);
        ci = cs_findchan(argv[2]);
        if (!u || !ci)
            return MOD_CONT;

        if (is_founder(u, ci))
            bot_raw_mode(u, ci, ircd->ownerunset, u->nick);
    } else if (stricmp(argv[0], "owner") == 0) {
        u = finduser(argv[1]);
        ci = cs_findchan(argv[2]);
        if (!u || !ci)
            return MOD_CONT;

        if (is_founder(u, ci))
            bot_raw_mode(u, ci, ircd->ownerset, u->nick);
    }

    return MOD_CONT;
}

MODULE_INIT("bs_fantasy_owner", BSFantasyOwner)
