/* NickServ core functions
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

int do_alist(User * u);
void myNickServHelp(User * u);

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

    c = createCommand("ALIST", do_alist, NULL, -1, NICK_HELP_ALIST, -1,
                      NICK_SERVADMIN_HELP_ALIST,
                      NICK_SERVADMIN_HELP_ALIST);
    moduleAddCommand(NICKSERV, c, MOD_UNIQUE);

    moduleSetNickHelp(myNickServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to anopes /ns help output.
 * @param u The user who is requesting help
 **/
void myNickServHelp(User * u)
{
    notice_lang(s_NickServ, u, NICK_HELP_CMD_ALIST);
}

/**
 * The /ns alist command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_alist(User * u)
{

/*
 * List the channels that the given nickname has access on
 *
 * /ns ALIST [level]
 * /ns ALIST [nickname] [level]
 *
 * -jester
 */

    char *nick = NULL;
    char *lev = NULL;

    NickAlias *na;

    int min_level = 0;
    int is_servadmin = is_services_admin(u);

    if (!is_servadmin) {
        /* Non service admins can only see their own levels */
        na = u->na;
    } else {
        /* Services admins can request ALIST on nicks.
         * The first argument for service admins must
         * always be a nickname.
         */
        nick = strtok(NULL, " ");

        /* If an argument was passed, use it as the nick to see levels
         * for, else check levels for the user calling the command */
        if (nick) {
            na = findnick(nick);
        } else {
            na = u->na;
        }
    }

    /* If available, get level from arguments */
    lev = strtok(NULL, " ");

    /* if a level was given, make sure it's an int for later */
    if (lev) {
        if (stricmp(lev, "FOUNDER") == 0) {
            min_level = ACCESS_FOUNDER;
        } else if (stricmp(lev, "SOP") == 0) {
            min_level = ACCESS_SOP;
        } else if (stricmp(lev, "AOP") == 0) {
            min_level = ACCESS_AOP;
        } else if (stricmp(lev, "HOP") == 0) {
            min_level = ACCESS_HOP;
        } else if (stricmp(lev, "VOP") == 0) {
            min_level = ACCESS_VOP;
        } else {
            min_level = atoi(lev);
        }
    }

    if (!nick_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (is_servadmin && nick && !na) {
        notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
    } else if (na->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, na->nick);
    } else if (min_level <= ACCESS_INVALID || min_level > ACCESS_FOUNDER) {
        notice_lang(s_NickServ, u, CHAN_ACCESS_LEVEL_RANGE,
                    ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
    } else {
        int i, level;
        int chan_count = 0;
        int match_count = 0;
        ChannelInfo *ci;

        notice_lang(s_NickServ, u, (is_servadmin ? NICK_ALIST_HEADER_X :
                                    NICK_ALIST_HEADER), na->nick);

        for (i = 0; i < 256; i++) {
            for ((ci = chanlists[i]); ci; (ci = ci->next)) {

                if ((level = get_access_level(ci, na))) {
                    chan_count++;

                    if (min_level > level) {
                        continue;
                    }

                    match_count++;

                    if ((ci->flags & CI_XOP) || (level == ACCESS_FOUNDER)) {
                        const char *xop;

                        xop = get_xop_level(level);

                        notice_lang(s_NickServ, u, NICK_ALIST_XOP_FORMAT,
                                    match_count,
                                    ((ci->
                                      flags & CI_NO_EXPIRE) ? '!' : ' '),
                                    ci->name, xop,
                                    (ci->desc ? ci->desc : ""));
                    } else {
                        notice_lang(s_NickServ, u,
                                    NICK_ALIST_ACCESS_FORMAT, match_count,
                                    ((ci->
                                      flags & CI_NO_EXPIRE) ? '!' : ' '),
                                    ci->name, level,
                                    (ci->desc ? ci->desc : ""));

                    }
                }
            }
        }

        notice_lang(s_NickServ, u, NICK_ALIST_FOOTER, match_count,
                    chan_count);
    }
    return MOD_CONT;
}
