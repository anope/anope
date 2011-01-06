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

int do_oper(User * u);
int oper_list_callback(SList * slist, int number, void *item,
                       va_list args);
int oper_list(int number, NickCore * nc, User * u, int *sent_header);
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
    c = createCommand("OPER", do_oper, NULL, OPER_HELP_OPER, -1, -1, -1,
                      -1);
    c->help_param1 = s_NickServ;
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
    notice_lang(s_OperServ, u, OPER_HELP_CMD_OPER);
}

/**
 * The /os oper command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_oper(User * u)
{
    char *cmd = strtok(NULL, " ");
    char *nick = strtok(NULL, " ");
    NickAlias *na;
    int res = 0;

    if (skeleton) {
        notice_lang(s_OperServ, u, OPER_OPER_SKELETON);
        return MOD_CONT;
    }

    if (!cmd || (!nick && stricmp(cmd, "LIST") && stricmp(cmd, "CLEAR"))) {
        syntax_error(s_OperServ, u, "OPER", OPER_OPER_SYNTAX);
    } else if (!stricmp(cmd, "ADD")) {
        if (!is_services_root(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        if (!(na = findnick(nick))) {
            notice_lang(s_OperServ, u, NICK_X_NOT_REGISTERED, nick);
            return MOD_CONT;
        }

        if (na->status & NS_VERBOTEN) {
            notice_lang(s_OperServ, u, NICK_X_FORBIDDEN, nick);
            return MOD_CONT;
        }

        if (na->nc->flags & NI_SERVICES_OPER
            || slist_indexof(&servopers, na->nc) != -1) {
            notice_lang(s_OperServ, u, OPER_OPER_EXISTS, nick);
            return MOD_CONT;
        }

        res = slist_add(&servopers, na->nc);
        if (res == -2) {
            notice_lang(s_OperServ, u, OPER_OPER_REACHED_LIMIT, nick);
            return MOD_CONT;
        } else {
            if (na->nc->flags & NI_SERVICES_ADMIN
                && (res = slist_indexof(&servadmins, na->nc)) != -1) {
                if (!is_services_root(u)) {
                    notice_lang(s_OperServ, u, PERMISSION_DENIED);
                    return MOD_CONT;
                }
                slist_delete(&servadmins, res);
                na->nc->flags |= NI_SERVICES_OPER;
                notice_lang(s_OperServ, u, OPER_OPER_MOVED, nick);
            } else {
                na->nc->flags |= NI_SERVICES_OPER;
                notice_lang(s_OperServ, u, OPER_OPER_ADDED, nick);
            }
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    } else if (!stricmp(cmd, "DEL")) {
        if (!is_services_root(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        if (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick)) {
            /* Deleting a range */
            res = slist_delete_range(&servopers, nick, NULL);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_OPER_NO_MATCH);
                return MOD_CONT;
            } else if (res == 1) {
                notice_lang(s_OperServ, u, OPER_OPER_DELETED_ONE);
            } else {
                notice_lang(s_OperServ, u, OPER_OPER_DELETED_SEVERAL, res);
            }
        } else {
            if (!(na = findnick(nick))) {
                notice_lang(s_OperServ, u, NICK_X_NOT_REGISTERED, nick);
                return MOD_CONT;
            }

            if (na->status & NS_VERBOTEN) {
                notice_lang(s_OperServ, u, NICK_X_FORBIDDEN, nick);
                return MOD_CONT;
            }

            if (!(na->nc->flags & NI_SERVICES_OPER)
                || (res = slist_indexof(&servopers, na->nc)) == -1) {
                notice_lang(s_OperServ, u, OPER_OPER_NOT_FOUND, nick);
                return MOD_CONT;
            }

            slist_delete(&servopers, res);
            notice_lang(s_OperServ, u, OPER_OPER_DELETED, nick);
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    } else if (!stricmp(cmd, "LIST")) {
        int sent_header = 0;

		if (!is_oper(u)) {
			notice_lang(s_OperServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}
		
        if (servopers.count == 0) {
            notice_lang(s_OperServ, u, OPER_OPER_LIST_EMPTY);
            return MOD_CONT;
        }

        if (!nick || (isdigit(*nick)
                      && strspn(nick, "1234567890,-") == strlen(nick))) {
            res =
                slist_enum(&servopers, nick, &oper_list_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_OPER_NO_MATCH);
                return MOD_CONT;
            } else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Oper");
            }
        } else {
            int i;

            for (i = 0; i < servopers.count; i++)
                if (!stricmp
                    (nick, ((NickCore *) servopers.list[i])->display)
                    || match_wild_nocase(nick,
                                         ((NickCore *) servopers.list[i])->
                                         display))
                    oper_list(i + 1, servopers.list[i], u, &sent_header);

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_OPER_NO_MATCH);
            else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Oper");
            }
        }
    } else if (!stricmp(cmd, "CLEAR")) {
        if (!is_services_root(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        if (servopers.count == 0) {
            notice_lang(s_OperServ, u, OPER_OPER_LIST_EMPTY);
            return MOD_CONT;
        }

        slist_clear(&servopers, 1);
        notice_lang(s_OperServ, u, OPER_OPER_CLEAR);
    } else {
        syntax_error(s_OperServ, u, "OPER", OPER_OPER_SYNTAX);
    }
    return MOD_CONT;
}

/* Lists an oper entry, prefixing it with the header if needed */

int oper_list(int number, NickCore * nc, User * u, int *sent_header)
{
    if (!nc)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_OPER_LIST_HEADER);
        *sent_header = 1;
    }

    notice_lang(s_OperServ, u, OPER_OPER_LIST_FORMAT, number, nc->display);
    return 1;
}

/* Callback for enumeration purposes */

int oper_list_callback(SList * slist, int number, void *item, va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return oper_list(number, item, u, sent_header);
}
