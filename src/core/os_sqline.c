/* OperServ core functions
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

int do_sqline(User * u);
int sqline_view_callback(SList * slist, int number, void *item,
                         va_list args);
int sqline_view(int number, SXLine * sx, User * u, int *sent_header);
int sqline_list_callback(SList * slist, int number, void *item,
                         va_list args);
int sqline_list(int number, SXLine * sx, User * u, int *sent_header);

void myOperServHelp(User * u);

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

    c = createCommand("SQLINE", do_sqline, is_services_oper,
                      OPER_HELP_SQLINE, -1, -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);

    moduleSetOperHelp(myOperServHelp);
    if (!ircd->sqline) {
        return MOD_STOP;
    }
    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
    if (is_services_oper(u)) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_SQLINE);
    }
}

/**
 * The /os sqline command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_sqline(User * u)
{
    char *cmd = strtok(NULL, " ");

    if (!cmd)
        cmd = "";

    if (!stricmp(cmd, "ADD")) {
        int deleted = 0;
        char *expiry, *mask, *reason;
        time_t expires;

        mask = strtok(NULL, " ");
        if (mask && *mask == '+') {
            expiry = mask;
            mask = strtok(NULL, " ");
        } else {
            expiry = NULL;
        }

        expires = expiry ? dotime(expiry) : SQLineExpiry;
        /* If the expiry given does not contain a final letter, it's in days,
         * said the doc. Ah well.
         */
        if (expiry && isdigit(expiry[strlen(expiry) - 1]))
            expires *= 86400;
        /* Do not allow less than a minute expiry time */
        if (expires != 0 && expires < 60) {
            notice_lang(s_OperServ, u, BAD_EXPIRY_TIME);
            return MOD_CONT;
        } else if (expires > 0) {
            expires += time(NULL);
        }

        if (mask && (reason = strtok(NULL, ""))) {

            /* We first do some sanity check on the proposed mask. */
            if (strspn(mask, "*") == strlen(mask)) {
                notice_lang(s_OperServ, u, USERHOST_MASK_TOO_WIDE, mask);
                return MOD_CONT;
            }

            /* Channel SQLINEs are only supported on Bahamut servers */
            if (*mask == '#' && !ircd->chansqline) {
                notice_lang(s_OperServ, u,
                            OPER_SQLINE_CHANNELS_UNSUPPORTED);
                return MOD_CONT;
            }

            deleted = add_sqline(u, mask, u->nick, expires, reason);
            if (deleted < 0)
                return MOD_CONT;
            else if (deleted)
                notice_lang(s_OperServ, u, OPER_SQLINE_DELETED_SEVERAL,
                            deleted);
            notice_lang(s_OperServ, u, OPER_SQLINE_ADDED, mask);

            if (WallOSSQLine) {
                char buf[128];

                if (!expires) {
                    strcpy(buf, "does not expire");
                } else {
                    int wall_expiry = expires - time(NULL);
                    char *s = NULL;

                    if (wall_expiry >= 86400) {
                        wall_expiry /= 86400;
                        s = "day";
                    } else if (wall_expiry >= 3600) {
                        wall_expiry /= 3600;
                        s = "hour";
                    } else if (wall_expiry >= 60) {
                        wall_expiry /= 60;
                        s = "minute";
                    }

                    snprintf(buf, sizeof(buf), "expires in %d %s%s",
                             wall_expiry, s,
                             (wall_expiry == 1) ? "" : "s");
                }

                anope_cmd_global(s_OperServ,
                                 "%s added an SQLINE for %s (%s)", u->nick,
                                 mask, buf);
            }

            if (readonly)
                notice_lang(s_OperServ, u, READ_ONLY_MODE);

        } else {
            syntax_error(s_OperServ, u, "SQLINE", OPER_SQLINE_SYNTAX);
        }

    } else if (!stricmp(cmd, "DEL")) {

        char *mask;
        int res = 0;

        mask = strtok(NULL, "");

        if (!mask) {
            syntax_error(s_OperServ, u, "SQLINE", OPER_SQLINE_SYNTAX);
            return MOD_CONT;
        }

        if (sqlines.count == 0) {
            notice_lang(s_OperServ, u, OPER_SQLINE_LIST_EMPTY);
            return MOD_CONT;
        }

        if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)) {
            /* Deleting a range */
            res = slist_delete_range(&sqlines, mask, NULL);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_SQLINE_NO_MATCH);
                return MOD_CONT;
            } else if (res == 1) {
                notice_lang(s_OperServ, u, OPER_SQLINE_DELETED_ONE);
            } else {
                notice_lang(s_OperServ, u, OPER_SQLINE_DELETED_SEVERAL,
                            res);
            }
        } else {
            if ((res = slist_indexof(&sqlines, mask)) == -1) {
                notice_lang(s_OperServ, u, OPER_SQLINE_NOT_FOUND, mask);
                return MOD_CONT;
            }

            slist_delete(&sqlines, res);
            notice_lang(s_OperServ, u, OPER_SQLINE_DELETED, mask);
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);

    } else if (!stricmp(cmd, "LIST")) {
        char *mask;
        int res, sent_header = 0;

        if (sqlines.count == 0) {
            notice_lang(s_OperServ, u, OPER_SQLINE_LIST_EMPTY);
            return MOD_CONT;
        }

        mask = strtok(NULL, "");

        if (!mask || (isdigit(*mask)
                      && strspn(mask, "1234567890,-") == strlen(mask))) {
            res =
                slist_enum(&sqlines, mask, &sqline_list_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_SQLINE_NO_MATCH);
                return MOD_CONT;
            }
        } else {
            int i;
            char *amask;

            for (i = 0; i < sqlines.count; i++) {
                amask = ((SXLine *) sqlines.list[i])->mask;
                if (!stricmp(mask, amask)
                    || match_wild_nocase(mask, amask))
                    sqline_list(i + 1, sqlines.list[i], u, &sent_header);
            }

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_SQLINE_NO_MATCH);
            else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "SQLine");
            }
        }
    } else if (!stricmp(cmd, "VIEW")) {
        char *mask;
        int res, sent_header = 0;

        if (sqlines.count == 0) {
            notice_lang(s_OperServ, u, OPER_SQLINE_LIST_EMPTY);
            return MOD_CONT;
        }

        mask = strtok(NULL, "");

        if (!mask || (isdigit(*mask)
                      && strspn(mask, "1234567890,-") == strlen(mask))) {
            res =
                slist_enum(&sqlines, mask, &sqline_view_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_SQLINE_NO_MATCH);
                return MOD_CONT;
            }
        } else {
            int i;
            char *amask;

            for (i = 0; i < sqlines.count; i++) {
                amask = ((SXLine *) sqlines.list[i])->mask;
                if (!stricmp(mask, amask)
                    || match_wild_nocase(mask, amask))
                    sqline_view(i + 1, sqlines.list[i], u, &sent_header);
            }

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_SQLINE_NO_MATCH);
        }
    } else if (!stricmp(cmd, "CLEAR")) {
        slist_clear(&sqlines, 1);
        notice_lang(s_OperServ, u, OPER_SQLINE_CLEAR);
    } else {
        syntax_error(s_OperServ, u, "SQLINE", OPER_SQLINE_SYNTAX);
    }
    return MOD_CONT;
}

int sqline_view(int number, SXLine * sx, User * u, int *sent_header)
{
    char timebuf[32], expirebuf[256];
    struct tm tm;

    if (!sx)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_SQLINE_VIEW_HEADER);
        *sent_header = 1;
    }

    tm = *localtime(&sx->seton);
    strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT,
                  &tm);
    expire_left(u->na, expirebuf, sizeof(expirebuf), sx->expires);
    notice_lang(s_OperServ, u, OPER_SQLINE_VIEW_FORMAT, number, sx->mask,
                sx->by, timebuf, expirebuf, sx->reason);

    return 1;
}

/* Callback for enumeration purposes */

int sqline_view_callback(SList * slist, int number, void *item,
                         va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return sqline_view(number, item, u, sent_header);
}

/* Lists an SQLINE entry, prefixing it with the header if needed */

int sqline_list(int number, SXLine * sx, User * u, int *sent_header)
{
    if (!sx)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_SQLINE_LIST_HEADER);
        *sent_header = 1;
    }

    notice_lang(s_OperServ, u, OPER_SQLINE_LIST_FORMAT, number, sx->mask,
                sx->reason);

    return 1;
}

/* Callback for enumeration purposes */

int sqline_list_callback(SList * slist, int number, void *item,
                         va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return sqline_list(number, item, u, sent_header);
}
