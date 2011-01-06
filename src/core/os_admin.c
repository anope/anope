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

int do_admin(User * u);
int admin_list_callback(SList * slist, int number, void *item,
                        va_list args);
int admin_list(int number, NickCore * nc, User * u, int *sent_header);
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

    c = createCommand("ADMIN", do_admin, NULL, OPER_HELP_ADMIN, -1, -1,
                      -1, -1);
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
    notice_lang(s_OperServ, u, OPER_HELP_CMD_ADMIN);
}

/**
 * The /os admin command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_admin(User * u)
{
    char *cmd = strtok(NULL, " ");
    char *nick = strtok(NULL, " ");
    NickAlias *na;
    int res = 0;

    if (skeleton) {
        notice_lang(s_OperServ, u, OPER_ADMIN_SKELETON);
        return MOD_CONT;
    }

    if (!cmd || (!nick && stricmp(cmd, "LIST") && stricmp(cmd, "CLEAR"))) {
        syntax_error(s_OperServ, u, "ADMIN", OPER_ADMIN_SYNTAX);
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

        if (na->nc->flags & NI_SERVICES_ADMIN
            || slist_indexof(&servadmins, na->nc) != -1) {
            notice_lang(s_OperServ, u, OPER_ADMIN_EXISTS, nick);
            return MOD_CONT;
        }

        res = slist_add(&servadmins, na->nc);
        if (res == -2) {
            notice_lang(s_OperServ, u, OPER_ADMIN_REACHED_LIMIT, nick);
            return MOD_CONT;
        } else {
            if (na->nc->flags & NI_SERVICES_OPER
                && (res = slist_indexof(&servopers, na->nc)) != -1) {
                slist_delete(&servopers, res);
                na->nc->flags |= NI_SERVICES_ADMIN;
                notice_lang(s_OperServ, u, OPER_ADMIN_MOVED, nick);
            } else {
                na->nc->flags |= NI_SERVICES_ADMIN;
                notice_lang(s_OperServ, u, OPER_ADMIN_ADDED, nick);
            }
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    } else if (!stricmp(cmd, "DEL")) {
        if (!is_services_root(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        if (servadmins.count == 0) {
            notice_lang(s_OperServ, u, OPER_ADMIN_LIST_EMPTY);
            return MOD_CONT;
        }

        if (isdigit(*nick) && strspn(nick, "1234567890,-") == strlen(nick)) {
            /* Deleting a range */
            res = slist_delete_range(&servadmins, nick, NULL);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_ADMIN_NO_MATCH);
                return MOD_CONT;
            } else if (res == 1) {
                notice_lang(s_OperServ, u, OPER_ADMIN_DELETED_ONE);
            } else {
                notice_lang(s_OperServ, u, OPER_ADMIN_DELETED_SEVERAL,
                            res);
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

            if (!(na->nc->flags & NI_SERVICES_ADMIN)
                || (res = slist_indexof(&servadmins, na->nc)) == -1) {
                notice_lang(s_OperServ, u, OPER_ADMIN_NOT_FOUND, nick);
                return MOD_CONT;
            }

            slist_delete(&servadmins, res);
            notice_lang(s_OperServ, u, OPER_ADMIN_DELETED, nick);
        }

        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    } else if (!stricmp(cmd, "LIST")) {
        int sent_header = 0;
		
        if (servadmins.count == 0) {
            notice_lang(s_OperServ, u, OPER_ADMIN_LIST_EMPTY);
            return MOD_CONT;
        }

        if (!nick || (isdigit(*nick)
                      && strspn(nick, "1234567890,-") == strlen(nick))) {
            res =
                slist_enum(&servadmins, nick, &admin_list_callback, u,
                           &sent_header);
            if (res == 0) {
                notice_lang(s_OperServ, u, OPER_ADMIN_NO_MATCH);
                return MOD_CONT;
            } else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Admin");
            }
        } else {
            int i;

            for (i = 0; i < servadmins.count; i++)
                if (!stricmp
                    (nick, ((NickCore *) servadmins.list[i])->display)
                    || match_wild_nocase(nick,
                                         ((NickCore *) servadmins.
                                          list[i])->display))
                    admin_list(i + 1, servadmins.list[i], u, &sent_header);

            if (!sent_header)
                notice_lang(s_OperServ, u, OPER_ADMIN_NO_MATCH);
            else {
                notice_lang(s_OperServ, u, END_OF_ANY_LIST, "Admin");
            }
        }
    } else if (!stricmp(cmd, "CLEAR")) {
        if (!is_services_root(u)) {
            notice_lang(s_OperServ, u, PERMISSION_DENIED);
            return MOD_CONT;
        }

        if (servadmins.count == 0) {
            notice_lang(s_OperServ, u, OPER_ADMIN_LIST_EMPTY);
            return MOD_CONT;
        }

        slist_clear(&servadmins, 1);
        notice_lang(s_OperServ, u, OPER_ADMIN_CLEAR);
    } else {
        syntax_error(s_OperServ, u, "ADMIN", OPER_ADMIN_SYNTAX);
    }
    return MOD_CONT;
}

int admin_list_callback(SList * slist, int number, void *item,
                        va_list args)
{
    User *u = va_arg(args, User *);
    int *sent_header = va_arg(args, int *);

    return admin_list(number, item, u, sent_header);
}

int admin_list(int number, NickCore * nc, User * u, int *sent_header)
{
    if (!nc)
        return 0;

    if (!*sent_header) {
        notice_lang(s_OperServ, u, OPER_ADMIN_LIST_HEADER);
        *sent_header = 1;
    }

    notice_lang(s_OperServ, u, OPER_ADMIN_LIST_FORMAT, number,
                nc->display);
    return 1;
}
