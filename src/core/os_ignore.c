/* OperServ core functions
 *
 * (C) 2003-2005 Anope Team
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

int do_ignorelist(User * u);
void myOperServHelp(User * u);
int do_ignoreuser(User * u);
int do_clearignore(User * u);
void delete_ignore(const char *nick);

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
        ("$Id$");
    moduleSetType(CORE);
    c = createCommand("IGNORE", do_ignoreuser, is_services_admin,
                      OPER_HELP_IGNORE, -1, -1, -1, -1);
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
    if (is_services_admin(u)) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_IGNORE);
    }
}

/**
 * The /os ignore command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_ignoreuser(User * u)
{
    char *cmd = strtok(NULL, " ");
    int t;

    if (!cmd) {
        notice_lang(s_OperServ, u, OPER_IGNORE_SYNTAX);
        return MOD_CONT;
    }

    if (!stricmp(cmd, "ADD")) {

        char *time = strtok(NULL, " ");
        char *nick = strtok(NULL, " ");
        char *rest = strtok(NULL, "");

        if (!nick) {
            notice_lang(s_OperServ, u, OPER_IGNORE_SYNTAX);
            return MOD_CONT;
        } else if (!time) {
            notice_lang(s_OperServ, u, OPER_IGNORE_SYNTAX);
            return MOD_CONT;
        } else {
            t = dotime(time);
            rest = NULL;

            if (t <= -1) {
                notice_lang(s_OperServ, u, OPER_IGNORE_VALID_TIME);
                return MOD_CONT;
            } else if (t == 0) {
                t = 157248000;  /* if 0 is given, we set time to 157248000 seconds == 5 years (let's hope the next restart will  be before that time ;-)) */
                add_ignore(nick, t);
                notice_lang(s_OperServ, u, OPER_IGNORE_PERM_DONE, nick);
            } else {
                add_ignore(nick, t);
                notice_lang(s_OperServ, u, OPER_IGNORE_TIME_DONE, nick,
                            time);
            }
        }
    } else if (!stricmp(cmd, "LIST")) {
        do_ignorelist(u);
    }

    else if (!stricmp(cmd, "DEL")) {
        char *nick = strtok(NULL, " ");
        if (!nick) {
            notice_lang(s_OperServ, u, OPER_IGNORE_SYNTAX);
        } else {
            if (get_ignore(nick) == 0) {
                notice_lang(s_OperServ, u, OPER_IGNORE_LIST_NOMATCH, nick);
                return MOD_CONT;
            } else {
                delete_ignore(nick);
                notice_lang(s_OperServ, u, OPER_IGNORE_DEL_DONE, nick);
            }
        }
    } else if (!stricmp(cmd, "CLEAR")) {
        do_clearignore(u);

    } else
        notice_lang(s_OperServ, u, OPER_IGNORE_SYNTAX);
    return MOD_CONT;
}

/* shows the Services ignore list */

int do_ignorelist(User * u)
{
    int sent_header = 0;
    IgnoreData *id;
    int i;

    for (i = 0; i < 256; i++) {
        for (id = ignore[i]; id; id = id->next) {
            if (!sent_header) {
                notice_lang(s_OperServ, u, OPER_IGNORE_LIST);
                sent_header = 1;
            }
            notice_user(s_OperServ, u, "%s", id->who);
        }
    }
    if (!sent_header)
        notice_lang(s_OperServ, u, OPER_IGNORE_LIST_EMPTY);
    return MOD_CONT;
}

/* deletes a nick from the ignore list  */

void delete_ignore(const char *nick)
{
    IgnoreData *ign, *prev;
    IgnoreData **whichlist;

    if (!nick || !*nick) {
        return;
    }

    whichlist = &ignore[tolower(nick[0])];

    for (ign = *whichlist, prev = NULL; ign; prev = ign, ign = ign->next) {
        if (stricmp(ign->who, nick) == 0)
            break;
    }
    /* If the ignore was not found, bail out -GD */
    if (!ign)
        return;
    if (prev)
        prev->next = ign->next;
    else
        *whichlist = ign->next;
    free(ign);
    ign = NULL;
}

/**************************************************************************/
/* Cleares the Services ignore list */

int do_clearignore(User * u)
{
    IgnoreData *id = NULL, *next = NULL;
    int i;
    for (i = 0; i < 256; i++) {
        for (id = ignore[i]; id; id = next) {
            next = id->next;
            free(id);
            if (!next) {
                ignore[i] = NULL;
            }
        }
    }
    notice_lang(s_OperServ, u, OPER_IGNORE_LIST_CLEARED);
    return MOD_CONT;
}
