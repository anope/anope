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

class BSFantasyUnban : public Module
{
 public:
	BSFantasyUnban(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		EvtHook *hook;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		hook = createEventHook(EVENT_BOT_FANTASY, do_fantasy);
		this->AddEventHook(hook);
	}
};

/**
 * Handle unban fantasy command.
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT or MOD_STOP
 **/
int do_fantasy(int argc, char **argv)
{
    User *u;
    ChannelInfo *ci;
    char *target = NULL;

    if (argc < 3)
        return MOD_CONT;

    if (stricmp(argv[0], "unban") == 0) {
        u = finduser(argv[1]);
        ci = cs_findchan(argv[2]);
        if (!u || !ci || !check_access(u, ci, CA_UNBAN))
            return MOD_CONT;

        if (argc >= 4)
            target = myStrGetToken(argv[3], ' ', 0);
        if (!target)
            common_unban(ci, u->nick);
        else
            common_unban(ci, target);

        /* free target if needed (#852) */
        Anope_Free(target);
    }

    return MOD_CONT;
}

MODULE_INIT("bs_fantasy_unban", BSFantasyUnban)
