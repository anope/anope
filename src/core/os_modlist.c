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

int do_modlist(User * u);
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

    c = createCommand("MODLIST", do_modlist, NULL, -1, -1, -1, -1,
                      OPER_HELP_MODLIST);
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
    notice_lang(s_OperServ, u, OPER_HELP_CMD_MODLIST);
}

/**
 * The /os modlist command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_modlist(User * u)
{
    int idx;
    int count = 0;
    int showCore = 0;
    int showThird = 1;
    int showProto = 1;
    int showEnc = 1;
    int showSupported = 1;
    int showQA = 1;

    char *param;
    ModuleHash *current = NULL;

    char core[] = "Core";
    char third[] = "3rd";
    char proto[] = "Protocol";
    char enc[] = "Encryption";
    char supported[] = "Supported";
    char qa[] = "QATested";

    param = strtok(NULL, "");
    if (param) {
        if (stricmp(param, core) == 0) {
            showCore = 1;
            showThird = 0;
            showProto = 0;
            showEnc = 0;
            showSupported = 0;
            showQA = 0;
        } else if (stricmp(param, third) == 0) {
            showCore = 0;
            showThird = 1;
            showSupported = 0;
            showQA = 0;
            showProto = 0;
            showEnc = 0;
        } else if (stricmp(param, proto) == 0) {
            showCore = 0;
            showThird = 0;
            showProto = 1;
            showEnc = 0;
            showSupported = 0;
            showQA = 0;
        } else if (stricmp(param, supported) == 0) {
            showCore = 0;
            showThird = 0;
            showProto = 0;
            showSupported = 1;
            showEnc = 0;
            showQA = 0;
        } else if (stricmp(param, qa) == 0) {
            showCore = 0;
            showThird = 0;
            showProto = 0;
            showSupported = 0;
            showEnc = 0;
            showQA = 1;
        } else if (stricmp(param, enc) == 0) {
            showCore = 0;
            showThird = 0;
            showProto = 0;
            showSupported = 0;
            showEnc = 1;
            showQA = 0;
        }
    }

    notice_lang(s_OperServ, u, OPER_MODULE_LIST_HEADER);

    for (idx = 0; idx != MAX_CMD_HASH; idx++) {
        for (current = MODULE_HASH[idx]; current; current = current->next) {
            switch (current->m->type) {
            case CORE:
                if (showCore) {
                    notice_lang(s_OperServ, u, OPER_MODULE_LIST,
                                current->name, current->m->version, core);
                    count++;
                }
                break;
            case THIRD:
                if (showThird) {
                    notice_lang(s_OperServ, u, OPER_MODULE_LIST,
                                current->name, current->m->version, third);
                    count++;
                }
                break;
            case PROTOCOL:
                if (showProto) {
                    notice_lang(s_OperServ, u, OPER_MODULE_LIST,
                                current->name, current->m->version, proto);
                    count++;
                }
                break;
            case SUPPORTED:
                if (showSupported) {
                    notice_lang(s_OperServ, u, OPER_MODULE_LIST,
                                current->name, current->m->version,
                                supported);
                    count++;
                }
                break;
            case QATESTED:
                if (showQA) {
                    notice_lang(s_OperServ, u, OPER_MODULE_LIST,
                                current->name, current->m->version, qa);
                    count++;
                }
                break;
            case ENCRYPTION:
                if (showEnc) {
                    notice_lang(s_OperServ, u, OPER_MODULE_LIST,
                                current->name, current->m->version, enc);
                    count++;
                }
                break;

            }

        }
    }
    if (count == 0) {
        notice_lang(s_OperServ, u, OPER_MODULE_NO_LIST);
    } else {
        notice_lang(s_OperServ, u, OPER_MODULE_LIST_FOOTER, count);
    }

    return MOD_CONT;
}
