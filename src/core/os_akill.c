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

int do_akill(User * u);
int akill_view_callback(SList * slist, int number, void *item,
                        va_list args);
int akill_view(int number, Akill * ak, User * u, int *sent_header);
int akill_list_callback(SList * slist, int number, void *item,
                        va_list args);
int akill_list(int number, Akill * ak, User * u, int *sent_header);
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
    moduleAddVersion
        (VERSION_STRING);
    moduleSetType(CORE);
    c = createCommand("AKILL", do_akill, is_services_oper, OPER_HELP_AKILL,
                      -1, -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);

    moduleSetOperHelp(myOperServHelp);

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
        notice_lang(s_OperServ, u, OPER_HELP_CMD_AKILL);
    }
}

/**
 * The /os command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
/* Manage the AKILL list. */

int do_akill(User * u)
{
    char *cmd = strtok(NULL, " ");
    char breason[BUFSIZE];

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

        expires = expiry ? dotime(expiry) : AutokillExpiry;
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
            if (strchr(mask, '!')) {
                notice_lang(s_OperServ, u, OPER_AKILL_NO_NICK);
                return MOD_CONT;
            }

            if (!strchr(mask, '@')) {
                notice_lang(s_OperServ, u, BAD_USERHOST_MASK);
                return MOD_CONT;
            }

            if (mask && strspn(mask, "~@.*?") == strlen(mask)) {
                notice_lang(s_OperServ, u, USERHOST_MASK_TOO_WIDE, mask);
                return MOD_CONT;
            }

            /**
             * Changed sprintf() to snprintf()and increased the size of
             * breason to match bufsize
             * -Rob
             **/
            if (AddAkiller) {
                snprintf(breason, sizeof(breason), "[%s] %s", u->nick,
                         reason);
                reason = sstrdup(breason);
            }

            deleted = add_akill(u, mask, u->nick, expires, reason);
            if (deleted < 0) {
                if (AddAkiller) {
                    free(reason);
                }
                return MOD_CONT;
            } else if (deleted) {
                notice_lang(s_OperServ, u, OPER_AKILL_DELETED_SEVERAL,
                            deleted);
            }
            notice_lang(s_OperServ, u, OPER_AKILL_ADDED, mask);

            if (WallOSAkill) {
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
                                 "%s added an AKILL for %s (%s) (%s)",
                                 u->nick, mask, reason, buf);
            }

            if (readonly) {
                notice_lang(s_OperServ, u, READ_ONLY_MODE);
            }
            if (AddAkiller) {
                free(reason);
            }
        } else {
            syntax_error(s_OperServ, u, "AKILL", OPER_AKILL_SYNTAX);
        }

    } else if (!stricmp(cmd, "DEL")) {

        char *mask;
        int res = 0;

        mask = strtok(NULL, " ");

        if (!mask) {
            syntax_error(s_OperServ, u, "AKILL", OPER_AKILL_SYNTAX);
            return MOD_CONT;
        }

        if (akills.count == 0) {
            notice_lang(s_OperServ, u, OPER_AKILL_LIST_EMPTY);
            return MOD_CONT;
        }

        if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)) {
            /* Deleting a range */
            res = slist_delete_range(&akills, mask, NULL);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
                return MOD_CONT;
            } else if (res == 1) {
                notice_lang(s_OperServ, u, OPER_AKILL_DELETED_ONE);
            } else {
                notice_lang(s_OperServ, u, OPER_AKILL_DELETED_SEVERAL,
                            res);
            }
        } else {
            if ((res = slist_indexof(&akills, mask)) == -1) {
                notice_lang(s_OperServ, u, OPER_AKILL_NOT_FOUND, mask);
                return MOD_CONT;
            }

            slist_delete(&akills, res);
            notice_lang(s_OperServ, u, OPER_AKILL_DELETED, mask);
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);

    } else if (!stricmp(cmd, "LIST")) {
        char *mask;
        int res, sent_header = 0;

        if (akills.count == 0) {
            notice_lang(s_OperServ, u, OPER_AKILL_LIST_EMPTY);
            return MOD_CONT;
        }

        mask = strtok(NULL, " ");

        if (!mask || (isdigit(*mask)
                      && strspn(mask, "1234567890,-") == strlen(mask))) {
            res =
                slist_enum(&akills, mask, &akill_list_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
                return MOD_CONT;
            } else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Akill");
            }
        } else {
            int i;
            char amask[BUFSIZE];

            for (i = 0; i < akills.count; i++) {
                snprintf(amask, sizeof(amask), "%s@%s",
                         ((Akill *) akills.list[i])->user,
                         ((Akill *) akills.list[i])->host);
                if (!stricmp(mask, amask)
                    || match_wild_nocase(mask, amask))
                    akill_list(i + 1, akills.list[i], u, &sent_header);
            }

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
            else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Akill");
            }
        }
    } else if (!stricmp(cmd, "VIEW")) {
        char *mask;
        int res, sent_header = 0;

        if (akills.count == 0) {
            notice_lang(s_OperServ, u, OPER_AKILL_LIST_EMPTY);
            return MOD_CONT;
        }

        mask = strtok(NULL, " ");

        if (!mask || (isdigit(*mask)
                      && strspn(mask, "1234567890,-") == strlen(mask))) {
            res =
                slist_enum(&akills, mask, &akill_view_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
                return MOD_CONT;
            }
        } else {
            int i;
            char amask[BUFSIZE];

            for (i = 0; i < akills.count; i++) {
                snprintf(amask, sizeof(amask), "%s@%s",
                         ((Akill *) akills.list[i])->user,
                         ((Akill *) akills.list[i])->host);
                if (!stricmp(mask, amask)
                    || match_wild_nocase(mask, amask))
                    akill_view(i + 1, akills.list[i], u, &sent_header);
            }

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_AKILL_NO_MATCH);
        }
    } else if (!stricmp(cmd, "CLEAR")) {
        slist_clear(&akills, 1);
        notice_lang(s_OperServ, u, OPER_AKILL_CLEAR);
    } else {
        syntax_error(s_OperServ, u, "AKILL", OPER_AKILL_SYNTAX);
    }
    return MOD_CONT;
}

int akill_view(int number, Akill * ak, User * u, int *sent_header)
{
    char mask[BUFSIZE];
    char timebuf[32], expirebuf[256];
    struct tm tm;

    if (!ak)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_AKILL_VIEW_HEADER);
        *sent_header = 1;
    }

    snprintf(mask, sizeof(mask), "%s@%s", ak->user, ak->host);
    tm = *localtime(&ak->seton);
    strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT,
                  &tm);
    expire_left(u->na, expirebuf, sizeof(expirebuf), ak->expires);
    notice_lang(s_OperServ, u, OPER_AKILL_VIEW_FORMAT, number, mask,
                ak->by, timebuf, expirebuf, ak->reason);

    return 1;
}

/* Lists an AKILL entry, prefixing it with the header if needed */

int akill_list_callback(SList * slist, int number, void *item,
                        va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return akill_list(number, item, u, sent_header);
}

/* Callback for enumeration purposes */

int akill_view_callback(SList * slist, int number, void *item,
                        va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return akill_view(number, item, u, sent_header);
}

/* Lists an AKILL entry, prefixing it with the header if needed */
int akill_list(int number, Akill * ak, User * u, int *sent_header)
{
    char mask[BUFSIZE];

    if (!ak)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_AKILL_LIST_HEADER);
        *sent_header = 1;
    }

    snprintf(mask, sizeof(mask), "%s@%s", ak->user, ak->host);
    notice_lang(s_OperServ, u, OPER_AKILL_LIST_FORMAT, number, mask,
                ak->reason);

    return 1;
}
