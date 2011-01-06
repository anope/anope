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

int do_modinfo(User * u);
void myOperServHelp(User * u);
int showModuleMsgLoaded(MessageHash * msgList, char *mod_name, User * u);
int showModuleCmdLoaded(CommandHash * cmdList, char *mod_name, User * u);

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
    c = createCommand("MODINFO", do_modinfo, NULL, -1, -1, -1, -1,
                      OPER_HELP_MODINFO);
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
    notice_lang(s_OperServ, u, OPER_HELP_CMD_MODINFO);
}

/**
 * The /os command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_modinfo(User * u)
{
    char *file;
    struct tm tm;
    char timebuf[64];
    Module *m;
    int idx = 0;

    file = strtok(NULL, "");
    if (!file) {
        syntax_error(s_OperServ, u, "MODINFO", OPER_MODULE_INFO_SYNTAX);
        return MOD_CONT;
    }
    m = findModule(file);
    if (m) {
        tm = *localtime(&m->time);
        strftime_lang(timebuf, sizeof(timebuf), u,
                      STRFTIME_DATE_TIME_FORMAT, &tm);
        notice_lang(s_OperServ, u, OPER_MODULE_INFO_LIST, m->name,
                    m->version ? m->version : "?",
                    m->author ? m->author : "?", timebuf);
        for (idx = 0; idx < MAX_CMD_HASH; idx++) {
            showModuleCmdLoaded(HOSTSERV[idx], m->name, u);
            showModuleCmdLoaded(OPERSERV[idx], m->name, u);
            showModuleCmdLoaded(NICKSERV[idx], m->name, u);
            showModuleCmdLoaded(CHANSERV[idx], m->name, u);
            showModuleCmdLoaded(BOTSERV[idx], m->name, u);
            showModuleCmdLoaded(MEMOSERV[idx], m->name, u);
            showModuleCmdLoaded(HELPSERV[idx], m->name, u);
            showModuleMsgLoaded(IRCD[idx], m->name, u);

        }
    } else {
        notice_lang(s_OperServ, u, OPER_MODULE_NO_INFO, file);
    }
    return MOD_CONT;
}

int showModuleCmdLoaded(CommandHash * cmdList, char *mod_name, User * u)
{
    Command *c;
    CommandHash *current;
    int display = 0;

    for (current = cmdList; current; current = current->next) {
        for (c = current->c; c; c = c->next) {
            if ((c->mod_name) && (stricmp(c->mod_name, mod_name) == 0)) {
                notice_lang(s_OperServ, u, OPER_MODULE_CMD_LIST,
                            c->service, c->name);
                display++;
            }
        }
    }
    return display;
}

int showModuleMsgLoaded(MessageHash * msgList, char *mod_name, User * u)
{
    Message *msg;
    MessageHash *mcurrent;
    int display = 0;
    for (mcurrent = msgList; mcurrent; mcurrent = mcurrent->next) {
        for (msg = mcurrent->m; msg; msg = msg->next) {
            if ((msg->mod_name) && (stricmp(msg->mod_name, mod_name) == 0)) {
                notice_lang(s_OperServ, u, OPER_MODULE_MSG_LIST,
                            msg->name);
                display++;
            }
        }
    }
    return display;
}
