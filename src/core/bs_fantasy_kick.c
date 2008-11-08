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

class BSFantasyKick : public Module
{
 public:
	BSFantasyKick(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		EvtHook *hook;

		moduleAddAuthor("Anope");
		moduleAddVersion("$Id$");
		moduleSetType(CORE);

		hook = createEventHook(EVENT_BOT_FANTASY, do_fantasy);
		moduleAddEventHook(hook);
	}
};

/**
 * Handle kick/k fantasy commands.
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT or MOD_STOP
 **/
int do_fantasy(int argc, char **argv)
{
    User *u, *u2;
    ChannelInfo *ci;
    char *target = NULL;
    char *reason = NULL;

    if (argc < 3)
        return MOD_CONT;

    if ((stricmp(argv[0], "kick") == 0) || (stricmp(argv[0], "k") == 0)) {
        u = finduser(argv[1]);
        ci = cs_findchan(argv[2]);
        if (!u || !ci)
            return MOD_CONT;

        if (argc >= 4) {
            target = myStrGetToken(argv[3], ' ', 0);
            reason = myStrGetTokenRemainder(argv[3], ' ', 1);
        }
        if (!target && check_access(u, ci, CA_KICKME)) {
            bot_raw_kick(u, ci, u->nick, "Requested");
        } else if (target && check_access(u, ci, CA_KICK)) {
            if (!stricmp(target, ci->bi->nick))
                bot_raw_kick(u, ci, u->nick, "Oops!");
            else {
                u2 = finduser(target);
                if (u2 && ci->c && is_on_chan(ci->c, u2)) {
                    if (!reason && !is_protected(u2))
                        bot_raw_kick(u, ci, target, "Requested");
                    else if (!is_protected(u2))
                        bot_raw_kick(u, ci, target, reason);
                }
            }
        }
    }

    if (target)
       free(target);
    if (reason)
       free(reason);

    return MOD_CONT;
}

MODULE_INIT("bs_fantasy_kick", BSFantasyKick)
