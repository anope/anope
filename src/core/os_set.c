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

int do_set(User * u);
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

    c = createCommand("SET", do_set, is_services_root, OPER_HELP_SET, -1,
                      -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);
    c = createCommand("SET LIST", NULL, NULL, OPER_HELP_SET_LIST, -1, -1,
                      -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);
    c = createCommand("SET READONLY", NULL, NULL, OPER_HELP_SET_READONLY,
                      -1, -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);
    c = createCommand("SET LOGCHAN", NULL, NULL, OPER_HELP_SET_LOGCHAN, -1,
                      -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);
    c = createCommand("SET DEBUG", NULL, NULL, OPER_HELP_SET_DEBUG, -1, -1,
                      -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);
    c = createCommand("SET NOEXPIRE", NULL, NULL, OPER_HELP_SET_NOEXPIRE,
                      -1, -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);
    c = createCommand("SET IGNORE", NULL, NULL, OPER_HELP_SET_IGNORE, -1,
                      -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);
    c = createCommand("SET SUPERADMIN", NULL, NULL,
                      OPER_HELP_SET_SUPERADMIN, -1, -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);
#ifdef USE_MYSQL
    c = createCommand("SET SQL", NULL, NULL, OPER_HELP_SET_SQL, -1, -1, -1,
                      -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);
#endif

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
    if (is_services_root(u)) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_SET);
    }
}

/**
 * The /os set command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_set(User * u)
{
    char *option = strtok(NULL, " ");
    char *setting = strtok(NULL, " ");
    int index;
    Channel *c;

    if (!option) {
        syntax_error(s_OperServ, u, "SET", OPER_SET_SYNTAX);
    } else if (stricmp(option, "LIST") == 0) {
        index =
            (allow_ignore ? OPER_SET_LIST_OPTION_ON :
             OPER_SET_LIST_OPTION_OFF);
        notice_lang(s_OperServ, u, index, "IGNORE");
        index =
            (readonly ? OPER_SET_LIST_OPTION_ON :
             OPER_SET_LIST_OPTION_OFF);
        notice_lang(s_OperServ, u, index, "READONLY");
        index =
            (logchan ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF);
        notice_lang(s_OperServ, u, index, "LOGCHAN");
        index =
            (debug ? OPER_SET_LIST_OPTION_ON : OPER_SET_LIST_OPTION_OFF);
        notice_lang(s_OperServ, u, index, "DEBUG");
        index =
            (noexpire ? OPER_SET_LIST_OPTION_ON :
             OPER_SET_LIST_OPTION_OFF);
        notice_lang(s_OperServ, u, index, "NOEXPIRE");
#ifdef USE_MYSQL
        index =
            (do_mysql ? OPER_SET_LIST_OPTION_ON :
             OPER_SET_LIST_OPTION_OFF);
        notice_lang(s_OperServ, u, index, "SQL");
#endif
    } else if (!setting) {
        syntax_error(s_OperServ, u, "SET", OPER_SET_SYNTAX);
    } else if (stricmp(option, "IGNORE") == 0) {
        if (stricmp(setting, "on") == 0) {
            allow_ignore = 1;
            notice_lang(s_OperServ, u, OPER_SET_IGNORE_ON);
        } else if (stricmp(setting, "off") == 0) {
            allow_ignore = 0;
            notice_lang(s_OperServ, u, OPER_SET_IGNORE_OFF);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_IGNORE_ERROR);
        }
#ifdef USE_MYSQL
    } else if (stricmp(option, "SQL") == 0) {
        if (stricmp(setting, "on") == 0) {
            if (!MysqlHost) {
                notice_lang(s_OperServ, u, OPER_SET_SQL_ERROR_DISABLED);
            } else {
                if (rdb_init()) {
                    notice_lang(s_OperServ, u, OPER_SET_SQL_ON);
                } else {
                    notice_lang(s_OperServ, u, OPER_SET_SQL_ERROR_INIT);
                }
            }
        } else if (stricmp(setting, "off") == 0) {
            if (!MysqlHost) {
                notice_lang(s_OperServ, u, OPER_SET_SQL_ERROR_DISABLED);
            } else {
                /* could call rdb_close() but that does nothing - TSL */
                do_mysql = 0;
                notice_lang(s_OperServ, u, OPER_SET_SQL_OFF);
            }
        } else {
            notice_lang(s_OperServ, u, OPER_SET_SQL_ERROR);
        }
#endif
    } else if (stricmp(option, "READONLY") == 0) {
        if (stricmp(setting, "on") == 0) {
            readonly = 1;
            alog("Read-only mode activated");
            close_log();
            notice_lang(s_OperServ, u, OPER_SET_READONLY_ON);
        } else if (stricmp(setting, "off") == 0) {
            readonly = 0;
            open_log();
            alog("Read-only mode deactivated");
            notice_lang(s_OperServ, u, OPER_SET_READONLY_OFF);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_READONLY_ERROR);
        }

    } else if (stricmp(option, "LOGCHAN") == 0) {
        /* Unlike the other SET commands where only stricmp is necessary,
         * we also have to ensure that LogChannel is defined or we can't
         * send to it.
         *
         * -jester
         */
        if (LogChannel && (stricmp(setting, "on") == 0)) {
            if (ircd->join2msg) {
                c = findchan(LogChannel);
                anope_cmd_join(s_GlobalNoticer, LogChannel, c ? c->creation_time : time(NULL));
            }
            logchan = 1;
            alog("Now sending log messages to %s", LogChannel);
            notice_lang(s_OperServ, u, OPER_SET_LOGCHAN_ON, LogChannel);
        } else if (LogChannel && (stricmp(setting, "off") == 0)) {
            alog("No longer sending log messages to a channel");
            if (ircd->join2msg) {
                anope_cmd_part(s_GlobalNoticer, LogChannel, NULL);
            }
            logchan = 0;
            notice_lang(s_OperServ, u, OPER_SET_LOGCHAN_OFF);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_LOGCHAN_ERROR);
        }
        /**
         * Allow the user to turn super admin on/off
	 *
	 * Rob
         **/
    } else if (stricmp(option, "SUPERADMIN") == 0) {
        if (!SuperAdmin) {
            notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_NOT_ENABLED);
        } else if (stricmp(setting, "on") == 0) {
            u->isSuperAdmin = 1;
            notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_ON);
            alog("%s: %s is a SuperAdmin ", s_OperServ, u->nick);
            anope_cmd_global(s_OperServ,
                             getstring2(NULL, OPER_SUPER_ADMIN_WALL_ON),
                             u->nick);
        } else if (stricmp(setting, "off") == 0) {
            u->isSuperAdmin = 0;
            notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_OFF);
            alog("%s: %s is no longer a SuperAdmin", s_OperServ, u->nick);
            anope_cmd_global(s_OperServ,
                             getstring2(NULL, OPER_SUPER_ADMIN_WALL_OFF),
                             u->nick);
        } else {
            notice_lang(s_OperServ, u, OPER_SUPER_ADMIN_SYNTAX);
        }
    } else if (stricmp(option, "DEBUG") == 0) {
        if (stricmp(setting, "on") == 0) {
            debug = 1;
            alog("Debug mode activated");
            notice_lang(s_OperServ, u, OPER_SET_DEBUG_ON);
        } else if (stricmp(setting, "off") == 0 ||
                   (*setting == '0' && atoi(setting) == 0)) {
            alog("Debug mode deactivated");
            debug = 0;
            notice_lang(s_OperServ, u, OPER_SET_DEBUG_OFF);
        } else if (isdigit(*setting) && atoi(setting) > 0) {
            debug = atoi(setting);
            alog("Debug mode activated (level %d)", debug);
            notice_lang(s_OperServ, u, OPER_SET_DEBUG_LEVEL, debug);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_DEBUG_ERROR);
        }

    } else if (stricmp(option, "NOEXPIRE") == 0) {
        if (stricmp(setting, "ON") == 0) {
            noexpire = 1;
            alog("No expire mode activated");
            notice_lang(s_OperServ, u, OPER_SET_NOEXPIRE_ON);
        } else if (stricmp(setting, "OFF") == 0) {
            noexpire = 0;
            alog("No expire mode deactivated");
            notice_lang(s_OperServ, u, OPER_SET_NOEXPIRE_OFF);
        } else {
            notice_lang(s_OperServ, u, OPER_SET_NOEXPIRE_ERROR);
        }
    } else {
        notice_lang(s_OperServ, u, OPER_SET_UNKNOWN_OPTION, option);
    }
    return MOD_CONT;
}
