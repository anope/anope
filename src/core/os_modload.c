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

extern MDE Module *mod_current_module;
extern MDE int mod_current_op;
extern MDE User *mod_current_user;
extern MDE ModuleHash *MODULE_HASH[MAX_CMD_HASH];

extern MDE Module *createModule(char *filename);

int do_modload(User * u);
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
    moduleAddVersion("$Id$");
    moduleSetType(CORE);

    c = createCommand("MODLOAD", do_modload, is_services_root, -1, -1, -1,
                      -1, OPER_HELP_MODLOAD);
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
    if (is_services_root(u)) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_MODLOAD);
    }
}

/**
 * The /os modload command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_modload(User * u)
{
    char *name;
    Module *m;

    name = strtok(NULL, "");
    if (!name) {
        syntax_error(s_OperServ, u, "MODLOAD", OPER_MODULE_LOAD_SYNTAX);
        return MOD_CONT;
    }
    m = findModule(name);
    if (!m) {
        m = createModule(name);
        mod_current_module = m;
        mod_current_user = u;
        mod_current_op = 1;
    } else {
        notice_lang(s_OperServ, u, OPER_MODULE_LOAD_FAIL, name);
    }
    return MOD_CONT;
}
