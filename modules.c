/* Modular support
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
#include "modules.h"
#include "language.h"

/*
 * Disable the modules on OpenBSD (for now) 
 * there is work in progress for this.
 */
#ifdef __OpenBSD__
#ifdef USE_MODULES
#undef USE_MODULES
#endif                          /* USE_MODULES */
#endif                          /* __OpenBSD__ */

#ifdef USE_MODULES
#include <dlfcn.h>
#endif

/**
 * Declare all the list's we want to use here
 **/
CommandHash *HOSTSERV[MAX_CMD_HASH];
CommandHash *BOTSERV[MAX_CMD_HASH];
CommandHash *MEMOSERV[MAX_CMD_HASH];
CommandHash *NICKSERV[MAX_CMD_HASH];
CommandHash *CHANSERV[MAX_CMD_HASH];
CommandHash *HELPSERV[MAX_CMD_HASH];
CommandHash *OPERSERV[MAX_CMD_HASH];
MessageHash *IRCD[MAX_CMD_HASH];
ModuleHash *MODULE_HASH[MAX_CMD_HASH];

Module *mod_current_module;
char *mod_current_module_name = NULL;
char *mod_current_buffer = NULL;
int mod_current_op;
User *mod_current_user;
ModuleCallBack *moduleCallBackHead = NULL;
int displayCommand(Command * c);
int displayCommandFromHash(CommandHash * cmdTable[], char *name);
int displayMessageFromHashl(char *name);
int displayMessage(Message * m);

/*******************************************************************************
 * Module Functions
 *******************************************************************************/
void modules_init(void)
{
#ifdef USE_MODULES
    int idx;
    Module *m;
    for (idx = 0; idx < ModulesNumber; idx++) {
        m = findModule(ModulesAutoload[idx]);
        if (!m) {
            m = createModule(ModulesAutoload[idx]);
            mod_current_module = m;
            mod_current_user = NULL;
            alog("trying to load [%s]", mod_current_module->name);
            alog("status: [%d]", loadModule(mod_current_module, NULL));
            mod_current_module = NULL;
            mod_current_user = NULL;
        }
    }
#endif
}

void modules_delayed_init(void)
{
#ifdef USE_MODULES
    int idx;
    Module *m;
    for (idx = 0; idx < ModulesDelayedNumber; idx++) {
        m = findModule(ModulesDelayedAutoload[idx]);
        if (!m) {
            m = createModule(ModulesDelayedAutoload[idx]);
            mod_current_module = m;
            mod_current_user = NULL;
            alog("trying to load [%s]", mod_current_module->name);
            alog("status: [%d]", loadModule(mod_current_module, NULL));
            mod_current_module = NULL;
            mod_current_user = NULL;
        }
    }
#endif
}

Module *createModule(char *filename)
{
    Module *m;
    if (!filename) {
        return NULL;
    }
    if ((m = malloc(sizeof(Module))) == NULL) {
        fatal("Out of memory!");
    }
    m->name = sstrdup(filename);        /* Our Name */
    m->handle = NULL;           /* Handle */
    m->version = NULL;
    m->author = NULL;
    m->nickHelp = NULL;
    m->chanHelp = NULL;
    m->memoHelp = NULL;
    m->botHelp = NULL;
    m->operHelp = NULL;
    m->hostHelp = NULL;
    m->helpHelp = NULL;

    return m;                   /* return a nice new module */
}

int destroyModule(Module * m)
{
    if (!m) {
        return MOD_ERR_PARAMS;
    }
    if (m->name) {
        free(m->name);
    }
    if (m->filename) {
        remove(m->filename);
        free(m->filename);
    }
    m->handle = NULL;
    if (m->author) {
        free(m->author);
    }
    if (m->version) {
        free(m->version);
    }
    /* No need to free our cmd/msg list, as they will always be empty by the module is destroyed */
    free(m);
    return MOD_ERR_OK;
}

int addModule(Module * m)
{
    int index = 0;
    ModuleHash *current = NULL;
    ModuleHash *newHash = NULL;
    ModuleHash *lastHash = NULL;

    index = CMD_HASH(m->name);

    for (current = MODULE_HASH[index]; current; current = current->next) {
        if (strcasecmp(m->name, current->name) == 0)
            return MOD_ERR_EXISTS;
        lastHash = current;
    }

    if ((newHash = malloc(sizeof(ModuleHash))) == NULL) {
        fatal("Out of memory");
    }
    m->time = time(NULL);
    newHash->next = NULL;
    newHash->name = sstrdup(m->name);
    newHash->m = m;

    if (lastHash == NULL)
        MODULE_HASH[index] = newHash;
    else
        lastHash->next = newHash;
    return MOD_ERR_OK;
}

int delModule(Module * m)
{
    int index = 0;
    ModuleHash *current = NULL;
    ModuleHash *lastHash = NULL;

    if (!m) {
        return MOD_ERR_PARAMS;
    }

    index = CMD_HASH(m->name);

    for (current = MODULE_HASH[index]; current; current = current->next) {
        if (strcasecmp(m->name, current->name) == 0) {
            if (!lastHash) {
                MODULE_HASH[index] = current->next;
            } else {
                lastHash->next = current->next;
            }
            destroyModule(current->m);
            free(current->name);
            free(current);
            return MOD_ERR_OK;
        }
        lastHash = current;
    }
    return MOD_ERR_NOEXIST;
}

Module *findModule(char *name)
{
    int idx;
    ModuleHash *current = NULL;
    if (!name) {
        return NULL;
    }
    idx = CMD_HASH(name);

    for (current = MODULE_HASH[idx]; current; current = current->next) {
        if (stricmp(name, current->name) == 0) {
            return current->m;
        }
    }
    return NULL;

}

int moduleCopyFile(char *name)
{
#ifdef USE_MODULES
    int ch;
    FILE *source, *target;
    char output[4096];
    char input[4096];
    int len;

    strncpy(output, MODULE_PATH, 4095); /* Get full path with .so extension */
    strncpy(input, MODULE_PATH, 4095);  /* Get full path with .so extension */
    len = strlen(output);
    strncat(output, "runtime/", 4095 - len);
    len += strlen(output);
    strncat(output, name, 4095 - len);
    strncat(input, name, 4095 - len);
    len += strlen(output);
    strncat(output, ".so", 4095 - len);
    strncat(input, ".so", 4095 - len);

    if ((source = fopen(input, "r")) == NULL) {
        return MOD_ERR_NOEXIST;
    }

    if ((target = fopen(output, "w")) == NULL) {
        return MOD_ERR_FILE_IO;
    }
    while ((ch = fgetc(source)) != EOF) {
        fputc(ch, target);
    }
    fclose(source);
    if (fclose(target) != 0) {
        return MOD_ERR_FILE_IO;
    }
#endif
    return MOD_ERR_OK;
}

int loadModule(Module * m, User * u)
{
#ifdef USE_MODULES
    char buf[4096];
    int len;
    const char *err;
    int (*func) (int, char **);
    int ret = 0;
    Module *m2;
    if (!m || !m->name) {
        return MOD_ERR_PARAMS;
    }
    if (m->handle) {
        return MOD_ERR_EXISTS;
    }
    if ((m2 = findModule(m->name)) != NULL) {
        return MOD_ERR_EXISTS;
    }

    moduleCopyFile(m->name);

    strncpy(buf, MODULE_PATH, 4095);    /* Get full path with .so extension */
    len = strlen(buf);
    strncat(buf, "runtime/", 4095 - len);
    len += strlen(buf);
    strncat(buf, m->name, 4095 - len);
    len += strlen(buf);
    strncat(buf, ".so", 4095 - len);

    m->filename = sstrdup(buf);
#ifdef HAS_RTLD_LOCAL
    m->handle = dlopen(m->filename, RTLD_LAZY | RTLD_LOCAL);
#else
    m->handle = dlopen(m->filename, RTLD_LAZY);
#endif
    if ((err = dlerror()) != NULL) {
        alog(err);
        if (u) {
            notice_lang(s_OperServ, u, OPER_MODULE_LOAD_FAIL, m->name);
        }
        return MOD_ERR_NOLOAD;
    }

    func = dlsym(m->handle, "AnopeInit");
    if ((err = dlerror()) != NULL) {
        dlclose(m->handle);     /* If no AnopeInit - it isnt an Anope Module, close it */
        return MOD_ERR_NOLOAD;
    }
    if (func) {
        mod_current_module_name = m->name;
        ret = func(0, NULL);    /* exec AnopeInit */
        if (ret == MOD_STOP) {
            alog("%s requested unload...", m->name);
            unloadModule(m, NULL);
            mod_current_module_name = NULL;
            return MOD_ERR_NOLOAD;
        }

        mod_current_module_name = NULL;
    }

    if (u) {
        wallops(s_OperServ, "%s loaded module %s", u->nick, m->name);
        notice_lang(s_OperServ, u, OPER_MODULE_LOADED, m->name);
    }
    addModule(m);
    return MOD_ERR_OK;

#else
    return MOD_ERR_NOLOAD;
#endif
}

int unloadModule(Module * m, User * u)
{
#ifdef USE_MODULES
    void (*func) ();

    if (!m || !m->handle) {
        if (u) {
            notice_lang(s_OperServ, u, OPER_MODULE_REMOVE_FAIL, m->name);
        }
        return MOD_ERR_PARAMS;
    }

    if (prepForUnload(mod_current_module) != MOD_ERR_OK) {
        return MOD_ERR_UNKNOWN;
    }

    func = dlsym(m->handle, "AnopeFini");
    if (func) {
        func();                 /* exec AnopeFini */
    }

    if ((dlclose(m->handle)) != 0) {
        alog(dlerror());
        if (u) {
            notice_lang(s_OperServ, u, OPER_MODULE_REMOVE_FAIL, m->name);
        }
        return MOD_ERR_NOUNLOAD;
    } else {
        if (u) {
            wallops(s_OperServ, "%s unloaded module %s", u->nick, m->name);
            notice_lang(s_OperServ, u, OPER_MODULE_UNLOADED, m->name);
        }
        delModule(m);
        return MOD_ERR_OK;
    }
#else
    return MOD_ERR_NOUNLOAD;
#endif
}

int prepForUnload(Module * m)
{
    int idx;
    CommandHash *current = NULL;
    MessageHash *mcurrent = NULL;
    Command *c;
    Message *msg;

    if (!m) {
        return MOD_ERR_PARAMS;
    }

    /* Kill any active callbacks this module has */
    moduleCallBackPrepForUnload(m->name);

    /**
     * ok, im going to walk every hash looking for commands we own, now, not exactly elegant or efficiant :)
     **/
    for (idx = 0; idx < MAX_CMD_HASH; idx++) {
        for (current = HS_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (strcmp(c->mod_name, m->name) == 0)) {
                    moduleDelCommand(HOSTSERV, c->name);
                }
            }
        }

        for (current = BS_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (strcmp(c->mod_name, m->name) == 0)) {
                    moduleDelCommand(BOTSERV, c->name);
                }
            }
        }

        for (current = MS_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (strcmp(c->mod_name, m->name) == 0)) {
                    moduleDelCommand(MEMOSERV, c->name);
                }
            }
        }

        for (current = NS_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (strcmp(c->mod_name, m->name) == 0)) {
                    moduleDelCommand(NICKSERV, c->name);
                }
            }
        }

        for (current = CS_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (strcmp(c->mod_name, m->name) == 0)) {
                    moduleDelCommand(CHANSERV, c->name);
                }
            }
        }

        for (current = HE_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (strcmp(c->mod_name, m->name) == 0)) {
                    moduleDelCommand(HELPSERV, c->name);
                }
            }
        }

        for (current = OS_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (stricmp(c->mod_name, m->name) == 0)) {
                    moduleDelCommand(OPERSERV, c->name);
                }
            }
        }

        for (mcurrent = IRCD[idx]; mcurrent; mcurrent = mcurrent->next) {
            for (msg = mcurrent->m; msg; msg = msg->next) {
                if ((msg->mod_name)
                    && (stricmp(msg->mod_name, m->name) == 0)) {
                    moduleDelMessage(msg->name);
                }
            }
        }
    }
    return MOD_ERR_OK;
}

/*******************************************************************************
 * Command Functions
 *******************************************************************************/
Command *createCommand(const char *name, int (*func) (User * u),
                       int (*has_priv) (User * u), int help_all,
                       int help_reg, int help_oper, int help_admin,
                       int help_root)
{
    Command *c;
    if ((c = malloc(sizeof(Command))) == NULL) {
        fatal("Out of memory!");
    }
    c->name = sstrdup(name);
    c->routine = func;
    c->has_priv = has_priv;
    c->helpmsg_all = help_all;
    c->helpmsg_reg = help_reg;
    c->helpmsg_oper = help_oper;
    c->helpmsg_admin = help_admin;
    c->helpmsg_root = help_root;
    c->help_param1 = NULL;
    c->help_param2 = NULL;
    c->help_param3 = NULL;
    c->help_param4 = NULL;
    c->next = NULL;
    c->mod_name = NULL;
    c->service = NULL;
    c->all_help = NULL;
    c->regular_help = NULL;
    c->oper_help = NULL;
    c->admin_help = NULL;
    c->root_help = NULL;
    return c;
}

int destroyCommand(Command * c)
{
    if (!c) {
        return MOD_ERR_PARAMS;
    }
    if (c->core == 1) {
        return MOD_ERR_UNKNOWN;
    }
    if (c->name) {
        free(c->name);
    }
    c->routine = NULL;
    c->has_priv = NULL;
    c->helpmsg_all = -1;
    c->helpmsg_reg = -1;
    c->helpmsg_oper = -1;
    c->helpmsg_admin = -1;
    c->helpmsg_root = -1;
    if (c->help_param1) {
        free(c->help_param1);
    }
    if (c->help_param2) {
        free(c->help_param2);
    }
    if (c->help_param3) {
        free(c->help_param3);
    }
    if (c->help_param4) {
        free(c->help_param4);
    }
    if (c->mod_name) {
        free(c->mod_name);
    }
    if (c->service) {
        free(c->service);
    }
    c->next = NULL;
    free(c);
    return MOD_ERR_OK;
}

int addCoreCommand(CommandHash * cmdTable[], Command * c)
{
    if (!cmdTable || !c) {
        return MOD_ERR_PARAMS;
    }
    c->core = 1;
    c->next = NULL;
    return addCommand(cmdTable, c, 0);
}

int moduleAddCommand(CommandHash * cmdTable[], Command * c, int pos)
{
    int status;

    if (!cmdTable || !c) {
        return MOD_ERR_PARAMS;
    }
    if (!mod_current_module) {
        return MOD_ERR_UNKNOWN;
    }                           /* shouldnt happen */
    c->core = 0;
    c->mod_name = sstrdup(mod_current_module->name);

    if (cmdTable == HOSTSERV) {
        c->service = sstrdup(s_HostServ);
    } else if (cmdTable == BOTSERV) {
        c->service = sstrdup(s_BotServ);
    } else if (cmdTable == MEMOSERV) {
        c->service = sstrdup(s_MemoServ);
    } else if (cmdTable == CHANSERV) {
        c->service = sstrdup(s_ChanServ);
    } else if (cmdTable == NICKSERV) {
        c->service = sstrdup(s_NickServ);
    } else if (cmdTable == HELPSERV) {
        c->service = sstrdup(s_HelpServ);
    } else if (cmdTable == OPERSERV) {
        c->service = sstrdup(s_OperServ);
    } else
        c->service = sstrdup("Unknown");

    if (debug)
        displayCommandFromHash(cmdTable, c->name);
    status = addCommand(cmdTable, c, pos);
    if (debug)
        displayCommandFromHash(cmdTable, c->name);
    if (status != MOD_ERR_OK) {
        alog("ERROR! [%d]", status);
    }
    return status;
}

int moduleDelCommand(CommandHash * cmdTable[], char *name)
{
    Command *c = NULL;
    Command *cmd = NULL;
    int status = 0;

    if (!mod_current_module) {
        return MOD_ERR_UNKNOWN;
    }

    c = findCommand(cmdTable, name);
    if (!c) {
        return MOD_ERR_NOEXIST;
    }


    for (cmd = c; cmd; cmd = cmd->next) {
        if (cmd->mod_name
            && strcasecmp(cmd->mod_name, mod_current_module->name) == 0) {
            if (debug) {
                displayCommandFromHash(cmdTable, name);
            }
            status = delCommand(cmdTable, cmd, mod_current_module->name);
            if (debug) {
                displayCommandFromHash(cmdTable, name);
            }
        }
    }
    return status;
}

int displayCommandFromHash(CommandHash * cmdTable[], char *name)
{
    CommandHash *current = NULL;
    int index = 0;
    index = CMD_HASH(name);
    if (debug > 1) {
        alog("trying to display command %s", name);
    }
    for (current = cmdTable[index]; current; current = current->next) {
        if (strcasecmp(name, current->name) == 0) {
            displayCommand(current->c);
        }
    }
    if (debug > 1) {
        alog("done displaying command %s", name);
    }
    return 0;
}


int displayCommand(Command * c)
{
    Command *cmd = NULL;
    int i = 0;
    alog("Displaying command list for %s", c->name);
    for (cmd = c; cmd; cmd = cmd->next) {
        alog("%d:  %p", ++i, cmd);
    }
    alog("end");
    return 0;
}

int displayMessageFromHash(char *name)
{
    MessageHash *current = NULL;
    int index = 0;
    index = CMD_HASH(name);
    if (debug > 1) {
        alog("trying to display message %s", name);
    }
    for (current = IRCD[index]; current; current = current->next) {
        if (strcasecmp(name, current->name) == 0) {
            displayMessage(current->m);
        }
    }
    if (debug > 1) {
        alog("done displaying message %s", name);
    }
    return 0;
}

int displayMessage(Message * m)
{
    Message *msg = NULL;
    int i = 0;
    alog("Displaying message list for %s", m->name);
    for (msg = m; msg; msg = msg->next) {
        alog("%d: %p", ++i, msg);
    }
    alog("end");
    return 0;
}


/**
 * Add a command to a command table
 * only add if were unique, pos = 0;
 * if we want it at the "head" of that command, pos = 1
 * at the tail, pos = 2
 **/
int addCommand(CommandHash * cmdTable[], Command * c, int pos)
{
    /* We can assume both param's have been checked by this point.. */
    int index = 0;
    CommandHash *current = NULL;
    CommandHash *newHash = NULL;
    CommandHash *lastHash = NULL;
    Command *tail = NULL;

    if (!cmdTable || !c || (pos < 0 || pos > 2)) {
        return MOD_ERR_PARAMS;
    }

    index = CMD_HASH(c->name);

    for (current = cmdTable[index]; current; current = current->next) {
        if ((c->service) && (current->c) && (current->c->service)
            && (!strcmp(c->service, current->c->service) == 0)) {
            continue;
        }
        if ((strcasecmp(c->name, current->name) == 0)) {        /* the cmd exist's we are a addHead */
            if (pos == 1) {
                c->next = current->c;
                current->c = c;
                if (debug)
                    alog("existing cmd: (%p), new cmd (%p)", c->next, c);
                return MOD_ERR_OK;
            } else if (pos == 2) {

                tail = current->c;
                while (tail->next)
                    tail = tail->next;
                if (debug)
                    alog("existing cmd: (%p), new cmd (%p)", tail, c);
                tail->next = c;
                c->next = NULL;

                return MOD_ERR_OK;
            } else
                return MOD_ERR_EXISTS;
        }
        lastHash = current;
    }

    if ((newHash = malloc(sizeof(CommandHash))) == NULL) {
        fatal("Out of memory");
    }
    newHash->next = NULL;
    newHash->name = sstrdup(c->name);
    newHash->c = c;

    if (lastHash == NULL)
        cmdTable[index] = newHash;
    else
        lastHash->next = newHash;

    return MOD_ERR_OK;
}

int delCommand(CommandHash * cmdTable[], Command * c, char *mod_name)
{
    int index = 0;
    CommandHash *current = NULL;
    CommandHash *lastHash = NULL;
    Command *tail = NULL, *last = NULL;

    if (!c || !cmdTable) {
        return MOD_ERR_PARAMS;
    }

    index = CMD_HASH(c->name);
    for (current = cmdTable[index]; current; current = current->next) {
        if (strcasecmp(c->name, current->name) == 0) {
            if (!lastHash) {
                tail = current->c;
                if (tail->next) {
                    while (tail) {
                        if (mod_name && tail->mod_name
                            && (stricmp(mod_name, tail->mod_name) == 0)) {
                            if (last) {
                                last->next = tail->next;
                            } else {
                                current->c = tail->next;
                            }
                            return MOD_ERR_OK;
                        }
                        last = tail;
                        tail = tail->next;
                    }
                } else {
                    cmdTable[index] = current->next;
                    free(current->name);
                    return MOD_ERR_OK;
                }
            } else {
                tail = current->c;
                if (tail->next) {
                    while (tail) {
                        if (mod_name && tail->mod_name
                            && (stricmp(mod_name, tail->mod_name) == 0)) {
                            if (last) {
                                last->next = tail->next;
                            } else {
                                current->c = tail->next;
                            }
                            return MOD_ERR_OK;
                        }
                        last = tail;
                        tail = tail->next;
                    }
                } else {
                    lastHash->next = current->next;
                    free(current->name);
                    return MOD_ERR_OK;
                }
            }
        }
        lastHash = current;
    }
    return MOD_ERR_NOEXIST;
}

Command *findCommand(CommandHash * cmdTable[], const char *name)
{
    int idx;
    CommandHash *current = NULL;
    if (!cmdTable || !name) {
        return NULL;
    }

    idx = CMD_HASH(name);

    for (current = cmdTable[idx]; current; current = current->next) {
        if (stricmp(name, current->name) == 0) {
            return current->c;
        }
    }
    return NULL;
}

/*******************************************************************************
 * Message Functions
 *******************************************************************************/

Message *createMessage(char *name,
                       int (*func) (char *source, int ac, char **av))
{
    Message *m = NULL;
    if (!name || !func) {
        return NULL;
    }
    if ((m = malloc(sizeof(Message))) == NULL) {
        fatal("Out of memory!");
    }
    m->name = sstrdup(name);
    m->func = func;
    m->mod_name = NULL;
    m->next = NULL;
    return m;
}

Message *findMessage(MessageHash * msgTable[], const char *name)
{
    int idx;
    MessageHash *current = NULL;
    if (!msgTable || !name) {
        return NULL;
    }
    idx = CMD_HASH(name);

    for (current = msgTable[idx]; current; current = current->next) {
        if (stricmp(name, current->name) == 0) {
            return current->m;
        }
    }
    return NULL;
}

int addMessage(MessageHash * msgTable[], Message * m, int pos)
{
    /* We can assume both param's have been checked by this point.. */
    int index = 0;
    MessageHash *current = NULL;
    MessageHash *newHash = NULL;
    MessageHash *lastHash = NULL;
    Message *tail = NULL;

    if (!msgTable || !m || (pos < 0 || pos > 2)) {
        return MOD_ERR_PARAMS;
    }

    index = CMD_HASH(m->name);

    for (current = msgTable[index]; current; current = current->next) {
        if (strcasecmp(m->name, current->name) == 0) {  /* the msg exist's we are a addHead */
            if (pos == 1) {
                m->next = current->m;
                current->m = m;
                if (debug)
                    alog("existing msg: (%p), new msg (%p)", m->next, m);
                return MOD_ERR_OK;
            } else if (pos == 2) {
                tail = current->m;
                while (tail->next)
                    tail = tail->next;
                if (debug)
                    alog("existing msg: (%p), new msg (%p)", tail, m);
                tail->next = m;
                m->next = NULL;
                return MOD_ERR_OK;
            } else
                return MOD_ERR_EXISTS;
        }
        lastHash = current;
    }

    if ((newHash = malloc(sizeof(MessageHash))) == NULL) {
        fatal("Out of memory");
    }
    newHash->next = NULL;
    newHash->name = sstrdup(m->name);
    newHash->m = m;

    if (lastHash == NULL)
        msgTable[index] = newHash;
    else
        lastHash->next = newHash;
    return MOD_ERR_OK;
}

int addCoreMessage(MessageHash * msgTable[], Message * m)
{
    if (!msgTable || !m) {
        return MOD_ERR_PARAMS;
    }
    m->core = 1;
    return addMessage(msgTable, m, 0);
}

int moduleAddMessage(Message * m, int pos)
{
    int status;

    if (!m) {
        return MOD_ERR_PARAMS;
    }
    if (!mod_current_module) {
        return MOD_ERR_UNKNOWN;
    }                           /* shouldnt happen */
    m->core = 0;
    m->mod_name = sstrdup(mod_current_module->name);

    status = addMessage(IRCD, m, pos);
    if (debug) {
        displayMessageFromHash(m->name);
    }
    return status;
}

int moduleDelMessage(char *name)
{
    Message *m;
    int status;

    if (!mod_current_module) {
        return MOD_ERR_UNKNOWN;
    }
    m = findMessage(IRCD, name);
    if (!m) {
        return MOD_ERR_NOEXIST;
    }

    status = delMessage(IRCD, m, mod_current_module->name);
    if (debug) {
        displayMessageFromHash(m->name);
    }
    return status;
}

int delMessage(MessageHash * msgTable[], Message * m, char *mod_name)
{
    int index = 0;
    MessageHash *current = NULL;
    MessageHash *lastHash = NULL;
    Message *tail = NULL, *last = NULL;

    if (!m || !msgTable) {
        return MOD_ERR_PARAMS;
    }

    index = CMD_HASH(m->name);

    for (current = msgTable[index]; current; current = current->next) {
        if (strcasecmp(m->name, current->name) == 0) {
            if (!lastHash) {
                tail = current->m;
                if (tail->next) {
                    while (tail) {
                        if (mod_name && tail->mod_name
                            && (stricmp(mod_name, tail->mod_name) == 0)) {
                            if (last) {
                                last->next = tail->next;
                            } else {
                                current->m = tail->next;
                            }
                            return MOD_ERR_OK;
                        }
                        last = tail;
                        tail = tail->next;
                    }
                } else {
                    msgTable[index] = current->next;
                    free(current->name);
                    return MOD_ERR_OK;
                }
            } else {
                tail = current->m;
                if (tail->next) {
                    while (tail) {
                        if (mod_name && tail->mod_name
                            && (stricmp(mod_name, tail->mod_name) == 0)) {
                            if (last) {
                                last->next = tail->next;
                            } else {
                                current->m = tail->next;
                            }
                            return MOD_ERR_OK;
                        }
                        last = tail;
                        tail = tail->next;
                    }
                } else {
                    lastHash->next = current->next;
                    free(current->name);
                    return MOD_ERR_OK;
                }
            }
        }
        lastHash = current;
    }
    return MOD_ERR_NOEXIST;
}

int destroyMessage(Message * m)
{
    if (!m) {
        return MOD_ERR_PARAMS;
    }
    if (m->name) {
        free(m->name);
    }
    m->func = NULL;
    if (m->mod_name) {
        free(m->mod_name);
    }
    m->next = NULL;
    return MOD_ERR_OK;
}

void moduleAddVersion(char *version)
{
    if (mod_current_module && version) {
        mod_current_module->version = sstrdup(version);
    }
}

void moduleAddAuthor(char *author)
{
    if (mod_current_module && author) {
        mod_current_module->author = sstrdup(author);
    }
}

/*******************************************************************************
 * Module Callback Functions
 *******************************************************************************/
int moduleAddCallback(char *name, time_t when,
                      int (*func) (int argc, char *argv[]), int argc,
                      char **argv)
{
    ModuleCallBack *new, *tmp, *prev;
    int i;
    new = malloc(sizeof(ModuleCallBack));
    if (!new)
        return MOD_ERR_MEMORY;

    if (name)
        new->name = sstrdup(name);
    else
        new->name = NULL;
    new->when = when;
    if (mod_current_module_name) {
        new->owner_name = sstrdup(mod_current_module_name);
    } else {
        new->owner_name = NULL;
    }
    new->func = func;
    new->argc = argc;
    new->argv = malloc(sizeof(char *) * argc);
    for (i = 0; i < argc; i++) {
        new->argv[i] = sstrdup(argv[i]);
    }
    new->next = NULL;

    if (moduleCallBackHead == NULL) {
        moduleCallBackHead = new;
    } else {                    /* find place in list */
        tmp = moduleCallBackHead;
        prev = tmp;
        if (new->when < tmp->when) {
            new->next = tmp;
            moduleCallBackHead = new;
        } else {
            while (tmp && new->when >= tmp->when) {
                prev = tmp;
                tmp = tmp->next;
            }
            prev->next = new;
            new->next = tmp;
        }
    }
    if (debug)
        alog("Added module CallBack: [%s] due to execute at %ld",
             new->name ? new->name : "?", new->when);
    return MOD_ERR_OK;
}

void moduleCallBackRun(void)
{
    ModuleCallBack *tmp;
    if (!moduleCallBackHead) {
        return;
    }
    tmp = moduleCallBackHead;
    if (tmp->when <= time(NULL)) {
        if (debug)
            alog("Executing callback: %s", tmp->name ? tmp->name : "?");
        if (tmp->func) {
            tmp->func(tmp->argc, tmp->argv);
            moduleCallBackDeleteEntry(NULL);    /* delete the head */
        }
    }
    return;
}

void moduleCallBackDeleteEntry(ModuleCallBack * prev)
{
    ModuleCallBack *tmp = NULL;
    int i;
    if (prev == NULL) {
        tmp = moduleCallBackHead;
        moduleCallBackHead = tmp->next;
    } else {
        tmp = prev->next;
        prev->next = tmp->next;
    }
    if (tmp->name)
        free(tmp->name);
    if (tmp->owner_name)
        free(tmp->owner_name);
    tmp->func = NULL;
    for (i = 0; i < tmp->argc; i++) {
        free(tmp->argv[i]);
    }
    tmp->argc = 0;
    tmp->next = NULL;
    free(tmp);
}


ModuleCallBack *moduleCallBackFindEntry(char *mod_name, boolean * found)
{
    ModuleCallBack *prev = NULL, *current = NULL;
    *found = false;
    current = moduleCallBackHead;
    while (current != NULL) {
        if (current->owner_name
            && (strcmp(mod_name, current->owner_name) == 0)) {
            *found = true;
            break;
        } else {
            prev = current;
            current = current->next;
        }
    }
    if (current == moduleCallBackHead) {
        return NULL;
    } else {
        return prev;
    }
}

void moduleDelCallback(char *name)
{
    ModuleCallBack *current = NULL;
    ModuleCallBack *prev = NULL, *tmp = NULL;
    int del = 0;
    if (!mod_current_module_name) {
        return;
    }
    if (!name) {
        return;
    }
    current = moduleCallBackHead;
    while (current) {
        if ((current->owner_name) && (current->name)) {
            if ((strcmp(mod_current_module_name, current->owner_name) == 0)
                && (strcmp(current->name, name) == 0)) {
                if (debug) {
                    alog("Removing CallBack %s for module %s", name,
                         mod_current_module_name);
                }
                tmp = current->next;    /* get a pointer to the next record, as once we delete this record, we'll lose it :) */
                moduleCallBackDeleteEntry(prev);        /* delete this record */
                del = 1;        /* set the record deleted flag */
            }
        }
        if (del == 1) {         /* if a record was deleted */
            current = tmp;      /* use the value we stored in temp */
            tmp = NULL;         /* clear it for next time */
            del = 0;            /* reset the flag */
        } else {
            prev = current;     /* just carry on as normal */
            current = current->next;
        }
    }
}

void moduleCallBackPrepForUnload(char *mod_name)
{
    boolean found = false;
    ModuleCallBack *tmp = NULL;

    tmp = moduleCallBackFindEntry(mod_name, &found);
    while (found) {
        if (debug) {
            alog("Removing CallBack for module %s", mod_name);
        }
        moduleCallBackDeleteEntry(tmp);
        tmp = moduleCallBackFindEntry(mod_name, &found);
    }
}

char *moduleGetLastBuffer(void)
{
    char *tmp = NULL;
    if (mod_current_buffer) {
        tmp = strchr(mod_current_buffer, ' ');
        if (tmp) {
            tmp++;
        }
    }
    return tmp;
}

/*******************************************************************************
 * Module HELP Functions
 *******************************************************************************/
int moduleAddRootHelp(Command * c, int (*func) (User * u))
{
    if (c) {
        c->root_help = func;
        return MOD_STOP;
    }
    return MOD_CONT;
}

int moduleAddAdminHelp(Command * c, int (*func) (User * u))
{
    if (c) {
        c->admin_help = func;
        return MOD_STOP;
    }
    return MOD_CONT;
}

int moduleAddOperHelp(Command * c, int (*func) (User * u))
{
    if (c) {
        c->oper_help = func;
        return MOD_STOP;
    }
    return MOD_CONT;
}

int moduleAddRegHelp(Command * c, int (*func) (User * u))
{
    if (c) {
        c->regular_help = func;
        return MOD_STOP;
    }
    return MOD_CONT;
}

int moduleAddHelp(Command * c, int (*func) (User * u))
{
    if (c) {
        c->all_help = func;
        return MOD_STOP;
    }
    return MOD_CONT;
}

void moduleSetNickHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->nickHelp = func;
    }
}

void moduleSetChanHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->chanHelp = func;
    }
}

void moduleSetMemoHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->memoHelp = func;
    }
}

void moduleSetBotHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->botHelp = func;
    }
}

void moduleSetOperHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->operHelp = func;
    }
}

void moduleSetHostHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->hostHelp = func;
    }
}

void moduleSetHelpHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->helpHelp = func;
    }
}

void moduleDisplayHelp(int service, User * u)
{
#ifdef USE_MODULES
    int idx;
    int header_shown = 0;
    ModuleHash *current = NULL;

    for (idx = 0; idx != MAX_CMD_HASH; idx++) {
        for (current = MODULE_HASH[idx]; current; current = current->next) {
            if ((service == 1) && current->m->nickHelp) {
                if (header_shown == 0) {
                    notice_lang(s_NickServ, u, MODULE_HELP_HEADER);
                    header_shown = 1;
                }
                current->m->nickHelp(u);
            } else if ((service == 2) && current->m->chanHelp) {
                if (header_shown == 0) {
                    notice_lang(s_ChanServ, u, MODULE_HELP_HEADER);
                    header_shown = 1;
                }
                current->m->chanHelp(u);
            } else if ((service == 3) && current->m->memoHelp) {
                if (header_shown == 0) {
                    notice_lang(s_MemoServ, u, MODULE_HELP_HEADER);
                    header_shown = 1;
                }
                current->m->memoHelp(u);
            } else if ((service == 4) && current->m->botHelp) {
                if (header_shown == 0) {
                    notice_lang(s_BotServ, u, MODULE_HELP_HEADER);
                    header_shown = 1;
                }
                current->m->botHelp(u);
            } else if ((service == 5) && current->m->operHelp) {
                if (header_shown == 0) {
                    notice_lang(s_OperServ, u, MODULE_HELP_HEADER);
                    header_shown = 1;
                }
                current->m->operHelp(u);
            } else if ((service == 6) && current->m->hostHelp) {
                if (header_shown == 0) {
                    notice_lang(s_HostServ, u, MODULE_HELP_HEADER);
                    header_shown = 1;
                }
                current->m->hostHelp(u);
            } else if ((service == 7) && current->m->helpHelp) {
                if (header_shown == 0) {
                    notice_lang(s_HelpServ, u, MODULE_HELP_HEADER);
                    header_shown = 1;
                }
                current->m->helpHelp(u);
            }
        }
    }
#endif
}


/* EOF */
