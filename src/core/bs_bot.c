/* BotServ core functions
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

int do_bot(User * u);
int delbot(BotInfo * bi);
void myBotServHelp(User * u);
void change_bot_nick(BotInfo * bi, char *newnick);

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

    c = createCommand("BOT", do_bot, is_services_admin, -1, -1, -1,
                      BOT_SERVADMIN_HELP_BOT, BOT_SERVADMIN_HELP_BOT);
    moduleAddCommand(BOTSERV, c, MOD_UNIQUE);

    moduleSetBotHelp(myBotServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}



/**
 * Add the help response to Anopes /bs help output.
 * @param u The user who is requesting help
 **/
void myBotServHelp(User * u)
{
    if (is_services_admin(u)) {
        notice_lang(s_BotServ, u, BOT_HELP_CMD_BOT);
    }
}

/**
 * The /bs bot command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_bot(User * u)
{
    BotInfo *bi;
    char *cmd = strtok(NULL, " ");
    char *ch = NULL;

    if (!cmd)
        syntax_error(s_BotServ, u, "BOT", BOT_BOT_SYNTAX);
    else if (!stricmp(cmd, "ADD")) {
        char *nick = strtok(NULL, " ");
        char *user = strtok(NULL, " ");
        char *host = strtok(NULL, " ");
        char *real = strtok(NULL, "");

        if (!nick || !user || !host || !real)
            syntax_error(s_BotServ, u, "BOT", BOT_BOT_SYNTAX);
        else if (readonly)
            notice_lang(s_BotServ, u, BOT_BOT_READONLY);
        else if (findbot(nick))
            notice_lang(s_BotServ, u, BOT_BOT_ALREADY_EXISTS, nick);
        else if (strlen(nick) > NickLen)
            notice_lang(s_BotServ, u, BOT_BAD_NICK);
        else if (strlen(user) >= USERMAX)
            notice_lang(s_BotServ, u, BOT_LONG_IDENT, USERMAX - 1);
        else if (strlen(user) > HOSTMAX)
            notice_lang(s_BotServ, u, BOT_LONG_HOST, HOSTMAX);
        else {
            NickAlias *na;

                /**
		 * Check the nick is valid re RFC 2812
		 **/
            if (isdigit(nick[0]) || nick[0] == '-') {
                notice_lang(s_BotServ, u, BOT_BAD_NICK);
                return MOD_CONT;
            }
            for (ch = nick; *ch && (ch - nick) < NICKMAX; ch++) {
                if (!isvalidnick(*ch)) {
                    notice_lang(s_BotServ, u, BOT_BAD_NICK);
                    return MOD_CONT;
                }
            }

            /* check for hardcored ircd forbidden nicks */
            if (!anope_valid_nick(nick)) {
                notice_lang(s_BotServ, u, BOT_BAD_NICK);
                return MOD_CONT;
            }

            if (!isValidHost(host, 3)) {
                notice_lang(s_BotServ, u, BOT_BAD_HOST);
                return MOD_CONT;
            }
            for (ch = user; *ch && (ch - user) < USERMAX; ch++) {
                if (!isalnum(*ch)) {
                    notice_lang(s_BotServ, u, BOT_BAD_IDENT, USERMAX - 1);
                    return MOD_CONT;
                }
            }

                /**
		 * Check the host is valid re RFC 2812
		 **/

            /* Check whether it's a services client's nick and return if so - Certus */
            /* use nickIsServices reduce the total number lines of code  - TSL */

            if (nickIsServices(nick, 0)) {
                notice_lang(s_BotServ, u, BOT_BOT_CREATION_FAILED);
                return MOD_CONT;
            }

            /* We check whether the nick is registered, and inform the user
             * if so. You need to drop the nick manually before you can use
             * it as a bot nick from now on -GD
             */
            if ((na = findnick(nick))) {
                notice_lang(s_BotServ, u, NICK_ALREADY_REGISTERED, nick);
                return MOD_CONT;
            }

            bi = makebot(nick);
            if (!bi) {
                notice_lang(s_BotServ, u, BOT_BOT_CREATION_FAILED);
                return MOD_CONT;
            }

            bi->user = sstrdup(user);
            bi->host = sstrdup(host);
            bi->real = sstrdup(real);
            bi->created = time(NULL);
            bi->chancount = 0;

            /* We check whether user with this nick is online, and kill it if so */
            EnforceQlinedNick(nick, s_BotServ);

            /* We make the bot online, ready to serve */
            anope_cmd_bot_nick(bi->nick, bi->user, bi->host, bi->real,
                               ircd->botserv_bot_mode);

            alog("%s: %s!%s@%s added bot %s!%s@%s (%s) to the bot list",
                 s_BotServ, u->nick, u->username, u->host, bi->nick, bi->user,
                 bi->host, bi->real);
            notice_lang(s_BotServ, u, BOT_BOT_ADDED, bi->nick, bi->user,
                        bi->host, bi->real);

            send_event(EVENT_BOT_CREATE, 1, bi->nick);
        }
    } else if (!stricmp(cmd, "CHANGE")) {
        char *oldnick = strtok(NULL, " ");
        char *nick = strtok(NULL, " ");
        char *user = strtok(NULL, " ");
        char *host = strtok(NULL, " ");
        char *real = strtok(NULL, "");

        if (!oldnick || !nick)
            syntax_error(s_BotServ, u, "BOT", BOT_BOT_SYNTAX);
        else if (readonly)
            notice_lang(s_BotServ, u, BOT_BOT_READONLY);
        else if (!(bi = findbot(oldnick)))
            notice_lang(s_BotServ, u, BOT_DOES_NOT_EXIST, oldnick);
        else if (strlen(nick) > NickLen)
            notice_lang(s_BotServ, u, BOT_BAD_NICK);
        else if (user && strlen(user) >= USERMAX)
            notice_lang(s_BotServ, u, BOT_LONG_IDENT, USERMAX - 1);
        else if (host && strlen(host) > HOSTMAX)
            notice_lang(s_BotServ, u, BOT_LONG_HOST, HOSTMAX);
        else {
            NickAlias *na;

            /* Checks whether there *are* changes.
             * Case sensitive because we may want to change just the case.
             * And we must finally check that the nick is not already
             * taken by another bot.
             */
            if (!strcmp(bi->nick, nick)
                && ((user) ? !strcmp(bi->user, user) : 1)
                && ((host) ? !strcmp(bi->host, host) : 1)
                && ((real) ? !strcmp(bi->real, real) : 1)) {
                notice_lang(s_BotServ, u, BOT_BOT_ANY_CHANGES);
                return MOD_CONT;
            }

            /* Check whether it's a services client's nick and return if so - Certus */
            /* use nickIsServices() to reduce the number of lines of code  - TSL */
            if (nickIsServices(nick, 0)) {
                notice_lang(s_BotServ, u, BOT_BOT_CREATION_FAILED);
                return MOD_CONT;
            }

               /**
		 * Check the nick is valid re RFC 2812
		 **/
            if (isdigit(nick[0]) || nick[0] == '-') {
                notice_lang(s_BotServ, u, BOT_BAD_NICK);
                return MOD_CONT;
            }
            for (ch = nick; *ch && (ch - nick) < NICKMAX; ch++) {
                if (!isvalidnick(*ch)) {
                    notice_lang(s_BotServ, u, BOT_BAD_NICK);
                    return MOD_CONT;
                }
            }

            /* check for hardcored ircd forbidden nicks */
            if (!anope_valid_nick(nick)) {
                notice_lang(s_BotServ, u, BOT_BAD_NICK);
                return MOD_CONT;
            }

            if (host && !isValidHost(host, 3)) {
                notice_lang(s_BotServ, u, BOT_BAD_HOST);
                return MOD_CONT;
            }

            if (user) {
                for (ch = user; *ch && (ch - user) < USERMAX; ch++) {
                    if (!isalnum(*ch)) {
                        notice_lang(s_BotServ, u, BOT_BAD_IDENT, USERMAX - 1);
                        return MOD_CONT;
                    }
                }
            }

            if (stricmp(bi->nick, nick) && findbot(nick)) {
                notice_lang(s_BotServ, u, BOT_BOT_ALREADY_EXISTS, nick);
                return MOD_CONT;
            }

            if (stricmp(bi->nick, nick)) {
                /* We check whether the nick is registered, and inform the user
                 * if so. You need to drop the nick manually before you can use
                 * it as a bot nick from now on -GD
                 */
                if ((na = findnick(nick))) {
                    notice_lang(s_BotServ, u, NICK_ALREADY_REGISTERED,
                                nick);
                    return MOD_CONT;
                }

                /* The new nick is really different, so we remove the Q line for
                   the old nick. */
                if (ircd->sqline) {
                    anope_cmd_unsqline(bi->nick);
                }

                /* We check whether user with this nick is online, and kill it if so */
                EnforceQlinedNick(nick, s_BotServ);
            }

            /* Send the QUIT before changing the bot internally, so proto mods (InspIRCD 1.2)
             * can get the uid if needed (or other things )and send that - Adam
             */
            if (user)
                anope_cmd_quit(bi->nick, "Quit: Be right back");

            if (strcmp(nick, bi->nick))
                change_bot_nick(bi, nick);

            if (user && strcmp(user, bi->user)) {
                free(bi->user);
                bi->user = sstrdup(user);
            }
            if (host && strcmp(host, bi->host)) {
                free(bi->host);
                bi->host = sstrdup(host);
            }
            if (real && strcmp(real, bi->real)) {
                free(bi->real);
                bi->real = sstrdup(real);
            }

            /* If only the nick changes, we just make the bot change his nick,
               else we must make it quit and rejoin. We must not forget to set
			   the Q:Line either (it's otherwise set in anope_cmd_bot_nick) */
            if (!user) {
                anope_cmd_chg_nick(oldnick, bi->nick);
				anope_cmd_sqline(bi->nick, "Reserved for services");
            } else {
                anope_cmd_bot_nick(bi->nick, bi->user, bi->host, bi->real,
                                   ircd->botserv_bot_mode);
                bot_rejoin_all(bi);
            }

            alog("%s: %s!%s@%s changed bot %s to: %s!%s@%s (%s)",
                 s_BotServ, u->nick, u->username, u->host, oldnick, bi->nick, bi->user,
                 bi->host, bi->real);
            notice_lang(s_BotServ, u, BOT_BOT_CHANGED, oldnick, bi->nick,
                        bi->user, bi->host, bi->real);

            send_event(EVENT_BOT_CHANGE, 1, bi->nick);
        }
    } else if (!stricmp(cmd, "DEL")) {
        char *nick = strtok(NULL, " ");

        if (!nick)
            syntax_error(s_BotServ, u, "BOT", BOT_BOT_SYNTAX);
        else if (readonly)
            notice_lang(s_BotServ, u, BOT_BOT_READONLY);
        else if (!(bi = findbot(nick)))
            notice_lang(s_BotServ, u, BOT_DOES_NOT_EXIST, nick);
        else {
            send_event(EVENT_BOT_DEL, 1, bi->nick);
            anope_cmd_quit(bi->nick,
                           "Quit: Help! I'm being deleted by %s!",
                           u->nick);
            if (ircd->sqline) {
                anope_cmd_unsqline(bi->nick);
            }
            delbot(bi);

            alog("%s: %s!%s@%s deleted bot %s from the bot list",
                 s_BotServ, u->nick, u->username, u->host, nick);
            notice_lang(s_BotServ, u, BOT_BOT_DELETED, nick);
        }
    } else
        syntax_error(s_BotServ, u, "BOT", BOT_BOT_SYNTAX);

    return MOD_CONT;
}

int delbot(BotInfo * bi)
{
    cs_remove_bot(bi);

    if (bi->next)
        bi->next->prev = bi->prev;
    if (bi->prev)
        bi->prev->next = bi->next;
    else
        botlists[tolower(*bi->nick)] = bi->next;

    nbots--;

    free(bi->nick);
    free(bi->user);
    free(bi->host);
    free(bi->real);

    free(bi);

    return 1;
}

void change_bot_nick(BotInfo * bi, char *newnick)
{
    if (bi->next)
        bi->next->prev = bi->prev;
    if (bi->prev)
        bi->prev->next = bi->next;
    else
        botlists[tolower(*bi->nick)] = bi->next;

    if (bi->nick)
        free(bi->nick);
    bi->nick = sstrdup(newnick);

    insert_bot(bi);
}
