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

/**
 * Automaticaly load modules at startup.
 * This will load modules at startup before the IRCD link is attempted, this
 * allows admins to have a module relating to ircd support load
 */
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

/**
 * Automaticaly load modules at startup, delayed.
 * This function waits until the IRCD link has been made, and then attempts
 * to load the specified modules.
 */
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

/**
 * Create a new module, setting up the default values as needed.
 * @param filename the filename of the new module
 * @return a newly created module struct
 */
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

/**
 * Destory the module.
 * free up all memory used by our module struct.
 * @param m the module to free
 * @return MOD_ERR_OK on success, anything else on fail
 */
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

/**
 * Add the module to the list of currently loaded modules.
 * @param m the currently loaded module
 * @return MOD_ERR_OK on success, anything else on fail
 */
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

/**
 * Remove the module from the list of loaded modules.
 * @param m module to remove
 * @return MOD_ERR_OK on success anything else on fail
 */
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

/**
 * Search the list of loaded modules for the given name.
 * @param name the name of the module to find
 * @return a pointer to the module found, or NULL
 */
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

/** 
 * Copy the module from the modules folder to the runtime folder.
 * This will prevent module updates while the modules is loaded from
 * triggering a segfault, as the actaul file in use will be in the 
 * runtime folder.
 * @param name the name of the module to copy
 * @return MOD_ERR_OK on success
 */
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

/**
 * Loads a given module.
 * @param m the module to load
 * @param u the user who loaded it, NULL for auto-load
 * @return MOD_ERR_OK on success, anything else on fail
 */
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

/**
 * Unload the given module.
 * @param m the module to unload
 * @param u the user who unloaded it
 * @return MOD_ERR_OK on success, anything else on fail
 */
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

/**
 * Prepare a module to be unloaded.
 * Remove all commands and messages this module is providing, and delete 
 * any callbacks which are still pending.
 * @param m the module to prepare for unload
 * @return MOD_ERR_OK on success
 */
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

    /* Remove any stored data this module has */
    moduleDelAllDataMod(m);

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
/**
 * Create a Command struct ready for use in anope.
 * @param name the name of the command
 * @param func pointer to the function to execute when command is given
 * @param has_priv pointer to function to check user priv's
 * @param help_all help file index for all users
 * @param help_reg help file index for all regustered users
 * @param help_oper help file index for all opers
 * @param help_admin help file index for all admins
 * @param help_root help file indenx for all services roots
 * @return a "ready to use" Command struct will be returned
 */
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

/**
 * Destroy a command struct freeing any memory.
 * @param c Command to destroy
 * @return MOD_ERR_OK on success, anything else on fail
 */
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

/**
 * Add a CORE command ot the given command hash
 * @param cmdTable the command table to add the command to
 * @param c the command to add
 * @return MOD_ERR_OK on success
 */
int addCoreCommand(CommandHash * cmdTable[], Command * c)
{
    if (!cmdTable || !c) {
        return MOD_ERR_PARAMS;
    }
    c->core = 1;
    c->next = NULL;
    return addCommand(cmdTable, c, 0);
}

/**
 * Add a module provided command to the given service.
 * e.g. moduleAddCommand(NICKSERV,c,MOD_HEAD);
 * @param cmdTable the services to add the command to
 * @param c the command to add
 * @param pos the position to add to, MOD_HEAD, MOD_TAIL, MOD_UNIQUE
 * @see createCommand
 * @return MOD_ERR_OK on successfully adding the command
 */
int moduleAddCommand(CommandHash * cmdTable[], Command * c, int pos)
{
    int status;

    if (!cmdTable || !c) {
        return MOD_ERR_PARAMS;
    }
    /* ok, this appears to be a module adding a command from outside of AnopeInit, try to look up its module struct for it */
    if ((mod_current_module_name) && (!mod_current_module)) {
        mod_current_module = findModule(mod_current_module_name);
    }

    if (!mod_current_module) {
        return MOD_ERR_UNKNOWN;
    }                           /* shouldnt happen */
    c->core = 0;
    if (!c->mod_name) {
        c->mod_name = sstrdup(mod_current_module->name);
    }


    if (cmdTable == HOSTSERV) {
        if (s_HostServ) {
            c->service = sstrdup(s_HostServ);
        } else {
            return MOD_ERR_NOSERVICE;
        }
    } else if (cmdTable == BOTSERV) {
        if (s_BotServ) {
            c->service = sstrdup(s_BotServ);
        } else {
            return MOD_ERR_NOSERVICE;
        }
    } else if (cmdTable == MEMOSERV) {
        if (s_MemoServ) {
            c->service = sstrdup(s_MemoServ);
        } else {
            return MOD_ERR_NOSERVICE;
        }
    } else if (cmdTable == CHANSERV) {
        if (s_ChanServ) {
            c->service = sstrdup(s_ChanServ);
        } else {
            return MOD_ERR_NOSERVICE;
        }
    } else if (cmdTable == NICKSERV) {
        if (s_NickServ) {
            c->service = sstrdup(s_NickServ);
        } else {
            return MOD_ERR_NOSERVICE;
        }
    } else if (cmdTable == HELPSERV) {
        if (s_HelpServ) {
            c->service = sstrdup(s_HelpServ);
        } else {
            return MOD_ERR_NOSERVICE;
        }
    } else if (cmdTable == OPERSERV) {
        if (s_OperServ) {
            c->service = sstrdup(s_OperServ);
        } else {
            return MOD_ERR_NOSERVICE;
        }
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

/**
 * Delete a command from the service given.
 * @param cmdTable the cmdTable for the services to remove the command from
 * @param name the name of the command to delete from the service
 * @return returns MOD_ERR_OK on success
 */
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

/**
 * Output the command stack into the log files.
 * This will print the call-stack for a given command into the log files, very useful for debugging.
 * @param cmdTable the command table to read from
 * @param name the name of the command to print
 * @return 0 is returned, it has no relevence yet :)
 */
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

/**
 * Output the command stack into the log files.
 * This will print the call-stack for a given command into the log files, very useful for debugging.
 * @param c the command struct to print
 * @return 0 is returned, it has no relevence yet :)
 */

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

/**
 * Display the message call stak.
 * Prints the call stack for a message based on the message name, again useful for debugging and little lese :)
 * @param name the name of the message to print info for
 * @return the return int has no relevence atm :)
 */
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

/**
 * Displays a message list for a given message.
 * Again this is of little use other than debugging.
 * @param m the message to display
 * @return 0 is returned and has no meaning 
 */
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
 * Add a command to a command table.
 * only add if were unique, pos = 0;
 * if we want it at the "head" of that command, pos = 1
 * at the tail, pos = 2
 * @param cmdTable the table to add the command to
 * @param c the command to add
 * @param pos the position in the cmd call stack to add the command
 * @return MOD_ERR_OK will be returned on success.
 */
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

/**
 * Remove a command from the command hash.
 * @param cmdTable the command table to remove the command from
 * @param c the command to remove
 * @param mod_name the name of the module who owns the command
 * @return MOD_ERR_OK will be returned on success
 */
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

/**
 * Search the command table gieven for a command.
 * @param cmdTable the name of the command table to search
 * @param name the name of the command to look for
 * @return returns a pointer to the found command struct, or NULL
 */
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

 /**
  * Create a new Message struct.
  * @param name the name of the message
  * @param func a pointer to the function to call when we recive this message
  * @return a new Message object
  **/
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

/** 
 * find a message in the given table. 
 * Looks up the message <name> in the MessageHash given
 * @param MessageHash the message table to search for this command, will almost always be IRCD
 * @param name the name of the command were looking for
 * @return NULL if we cant find it, or a pointer to the Message if we can
 **/
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

/**
 * Add a message to the MessageHash.
 * @param msgTable the MessageHash we want to add a message to
 * @param m the Message we want to add
 * @param pos the position we want to add the message to, E.G. MOD_HEAD, MOD_TAIL, MOD_UNIQUE 
 * @return MOD_ERR_OK on a successful add.
 **/

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

/**
 * Add the given message (m) to the MessageHash marking it as a core command
 * @param msgTable the MessageHash we want to add to
 * @param m the Message we are adding
 * @return MOD_ERR_OK on a successful add.
 **/
int addCoreMessage(MessageHash * msgTable[], Message * m)
{
    if (!msgTable || !m) {
        return MOD_ERR_PARAMS;
    }
    m->core = 1;
    return addMessage(msgTable, m, 0);
}

/**
 * Add a module message to the IRCD message hash
 * @param m the Message to add
 * @param pos the Position to add the message to, e.g. MOD_HEAD, MOD_TAIL, MOD_UNIQUE
 * @return MOD_ERR_OK on success, althing else on fail.
 **/
int moduleAddMessage(Message * m, int pos)
{
    int status;

    if (!m) {
        return MOD_ERR_PARAMS;
    }

    /* ok, this appears to be a module adding a message from outside of AnopeInit, try to look up its module struct for it */
    if ((mod_current_module_name) && (!mod_current_module)) {
        mod_current_module = findModule(mod_current_module_name);
    }

    if (!mod_current_module) {
        return MOD_ERR_UNKNOWN;
    }                           /* shouldnt happen */
    m->core = 0;
    if (!m->mod_name) {
        m->mod_name = sstrdup(mod_current_module->name);
    }

    status = addMessage(IRCD, m, pos);
    if (debug) {
        displayMessageFromHash(m->name);
    }
    return status;
}

/**
 * remove the given message from the IRCD message hash
 * @param name the name of the message to remove
 * @return MOD_ERR_OK on success, althing else on fail.
 **/
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

/**
 * remove the given message from the given message hash, for the given module
 * @param msgTable which MessageHash we are removing from
 * @param m the Message we want to remove
 * @mod_name the name of the module we are removing
 * @return MOD_ERR_OK on success, althing else on fail.
 **/
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

/**
 * Destory a message, freeing its memory.
 * @param m the message to be destroyed
 * @return MOD_ERR_SUCCESS on success
 **/
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

/**
 * Add the modules version info.
 * @param version the version of the current module
 **/
void moduleAddVersion(char *version)
{
    if (mod_current_module && version) {
        mod_current_module->version = sstrdup(version);
    }
}

/**
 * Add the modules author info
 * @param author the author of the module
 **/
void moduleAddAuthor(char *author)
{
    if (mod_current_module && author) {
        mod_current_module->author = sstrdup(author);
    }
}

/*******************************************************************************
 * Module Callback Functions
 *******************************************************************************/
 /**
  * Adds a timed callback for the current module.
  * This allows modules to request that anope executes one of there functions at a time in the future, without an event to trigger it
  * @param name the name of the callback, this is used for refrence mostly, but is needed it you want to delete this particular callback later on
  * @param when when should the function be executed, this is a time in the future, seconds since 00:00:00 1970-01-01 UTC
  * @param func the function to be executed when the callback is ran, its format MUST be int func(int argc, char **argv); 
  * @param argc the argument count for the argv paramter
  * @param atgv a argument list to be passed to the called function.
  * @return MOD_ERR_OK on success, anything else on fail.
  * @see moduleDelCallBack
  **/
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

/**
 * Execute a stored call back
 **/
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
            mod_current_module_name = tmp->owner_name;
            tmp->func(tmp->argc, tmp->argv);
            mod_current_module = NULL;
            moduleCallBackDeleteEntry(NULL);    /* delete the head */
        }
    }
    return;
}

/**
 * Removes a entry from the modules callback list
 * @param prev a pointer to the previous entry in the list, NULL for the head
 **/
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

/**
 * Search the module callback list for a given module
 * @param mod_name the name of the module were looking for
 * @param found have we found it?
 * @return a pointer to the ModuleCallBack struct or NULL - dont forget to check the found paramter!
 **/
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

/**
 * Allow module coders to delete a callback by name.
 * @param name the name of the callback they wish to delete
 **/
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

/**
 * Remove all outstanding module callbacks for the given module.
 * When a module is unloaded, any callbacks it had outstanding must be removed, else when they attempt to execute the func pointer will no longer be valid, and we'll seg.
 * @param mod_name the name of the module we are preping for unload 
 **/
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

/**
 * Return a copy of the complete last buffer.
 * This is needed for modules who cant trust the strtok() buffer, as we dont know who will have already messed about with it.
 * @reutrn a pointer to a copy of the last buffer - DONT mess with this, copy if first if you must do things to it.
 **/
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
 /**
  * Add help for Root admins.
  * @param c the Command to add help for
  * @param func the function to run when this help is asked for
  **/
int moduleAddRootHelp(Command * c, int (*func) (User * u))
{
    if (c) {
        c->root_help = func;
        return MOD_STOP;
    }
    return MOD_CONT;
}

 /**
  * Add help for Admins.
  * @param c the Command to add help for
  * @param func the function to run when this help is asked for
  **/
int moduleAddAdminHelp(Command * c, int (*func) (User * u))
{
    if (c) {
        c->admin_help = func;
        return MOD_STOP;
    }
    return MOD_CONT;
}

 /**
  * Add help for opers..
  * @param c the Command to add help for
  * @param func the function to run when this help is asked for
  **/
int moduleAddOperHelp(Command * c, int (*func) (User * u))
{
    if (c) {
        c->oper_help = func;
        return MOD_STOP;
    }
    return MOD_CONT;
}

/**
  * Add help for registered users
  * @param c the Command to add help for
  * @param func the function to run when this help is asked for
  **/
int moduleAddRegHelp(Command * c, int (*func) (User * u))
{
    if (c) {
        c->regular_help = func;
        return MOD_STOP;
    }
    return MOD_CONT;
}

/**
  * Add help for all users
  * @param c the Command to add help for
  * @param func the function to run when this help is asked for
  **/
int moduleAddHelp(Command * c, int (*func) (User * u))
{
    if (c) {
        c->all_help = func;
        return MOD_STOP;
    }
    return MOD_CONT;
}

/**
  * Add output to nickserv help.
  * when doing a /msg nickserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
  * @param func a pointer to the function which will display the code
  **/
void moduleSetNickHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->nickHelp = func;
    }
}

/**
  * Add output to chanserv help.
  * when doing a /msg chanserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
  * @param func a pointer to the function which will display the code
  **/
void moduleSetChanHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->chanHelp = func;
    }
}

/**
  * Add output to memoserv help.
  * when doing a /msg memoserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
  * @param func a pointer to the function which will display the code
  **/
void moduleSetMemoHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->memoHelp = func;
    }
}

/**
  * Add output to botserv help.
  * when doing a /msg botserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
  * @param func a pointer to the function which will display the code
  **/
void moduleSetBotHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->botHelp = func;
    }
}

/**
  * Add output to operserv help.
  * when doing a /msg operserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
  * @param func a pointer to the function which will display the code
  **/
void moduleSetOperHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->operHelp = func;
    }
}

/**
  * Add output to hostserv help.
  * when doing a /msg hostserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
  * @param func a pointer to the function which will display the code
  **/
void moduleSetHostHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->hostHelp = func;
    }
}

/**
  * Add output to helpserv help.
  * when doing a /msg helpserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
  * @param func a pointer to the function which will display the code
  **/
void moduleSetHelpHelp(void (*func) (User * u))
{
    if (mod_current_module) {
        mod_current_module->helpHelp = func;
    }
}

/**
 * Display any extra module help for the given service.
 * @param services which services is help being dispalyed for?
 * @param u which user is requesting the help
 **/
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

/**
 * Add module data to a struct.
 * This allows module coders to add data to an existing struct
 * @param md The module data for the struct to be used
 * @param key The Key for the key/value pair
 * @param value The value for the key/value pair, this is what will be stored for you
 * @return MOD_ERR_OK will be returned on success
 **/
int moduleAddData(ModuleData * md[], char *key, char *value)
{
    char *mod_name = sstrdup(mod_current_module_name);

    int index = 0;
    ModuleData *current = NULL;
    ModuleData *newHash = NULL;
    ModuleData *lastHash = NULL;
    ModuleDataItem *item = NULL;
    ModuleDataItem *itemCurrent = NULL;
    ModuleDataItem *lastItem = NULL;
    index = CMD_HASH(mod_name);

    for (current = md[index]; current; current = current->next) {
        if (strcasecmp(current->moduleName, mod_name) == 0)
            lastHash = current;
    }

    if (!lastHash) {
        newHash = malloc(sizeof(ModuleData));
        if (!newHash) {
            return MOD_ERR_MEMORY;
        }
        newHash->next = NULL;
        newHash->di = NULL;
        newHash->moduleName = strdup(mod_name);
        md[index] = newHash;
        lastHash = newHash;
    }

        /**
	 * Ok, at this point lastHash will always be a valid ModuleData struct, and will always be "our" module Data Struct
	 **/
    for (itemCurrent = lastHash->di; itemCurrent;
         itemCurrent = itemCurrent->next) {
        alog("key: [%s] itemKey: [%s]", key, itemCurrent->key);
        if (strcasecmp(itemCurrent->key, key) == 0) {
            item = itemCurrent;
        }
        lastItem = itemCurrent;
    }
    if (!item) {
        item = malloc(sizeof(ModuleDataItem));
        if (!item) {
            return MOD_ERR_MEMORY;
        }
        item->next = NULL;
        item->key = strdup(key);
        item->value = strdup(value);
        if (lastItem)
            lastItem->next = item;
        else
            lastHash->di = item;
    } else {
        free(item->key);
        free(item->value);
        item->key = strdup(key);
        item->value = strdup(value);
    }
    free(mod_name);
    return MOD_ERR_OK;

}

/**
 * Returns the value from a key/value pair set.
 * This allows module coders to retrive any data they have previuosly stored in any given struct
 * @param md The module data for the struct to be used
 * @param key The key to find the data for
 * @return the value paired to the given key will be returned, or NULL 
 **/
char *moduleGetData(ModuleData * md[], char *key)
{
    char *mod_name = sstrdup(mod_current_module_name);
    int index = 0;
    char *ret = NULL;
    ModuleData *current = NULL;
    ModuleData *lastHash = NULL;
    ModuleDataItem *itemCurrent = NULL;
    index = CMD_HASH(mod_name);

    for (current = md[index]; current; current = current->next) {
        if (strcasecmp(current->moduleName, mod_name) == 0)
            lastHash = current;
    }

    if (lastHash) {
        for (itemCurrent = lastHash->di; itemCurrent;
             itemCurrent = itemCurrent->next) {
            if (strcmp(itemCurrent->key, key) == 0) {
                ret = strdup(itemCurrent->value);
            }
        }
    }
    free(mod_name);
    return ret;
}

/**
 * Delete the key/value pair indicated by "key" for the current module.
 * This allows module coders to remove a previously stored key/value pair.
 * @param md The module data for the struct to be used
 * @param key The key to delete the key/value pair for
 **/
void moduleDelData(ModuleData * md[], char *key)
{
    char *mod_name = sstrdup(mod_current_module_name);
    int index = 0;
    ModuleData *current = NULL;
    ModuleData *lastHash = NULL;

    ModuleDataItem *itemCurrent = NULL;
    ModuleDataItem *prev = NULL;
    ModuleDataItem *next = NULL;
    index = CMD_HASH(mod_name);

    for (current = md[index]; current; current = current->next) {
        if (strcasecmp(current->moduleName, mod_name) == 0)
            lastHash = current;
    }
    if (lastHash) {
        for (itemCurrent = lastHash->di; itemCurrent; itemCurrent = next) {
            next = itemCurrent->next;
            if (strcmp(key, itemCurrent->key) == 0) {
                free(itemCurrent->key);
                free(itemCurrent->value);
                itemCurrent->next = NULL;
                free(itemCurrent);
                if (prev) {
                    prev->next = next;
                } else {
                    lastHash->di = next;
                }
            } else {
                prev = itemCurrent;
            }
        }
    }
    free(mod_name);
}

/**
 * This will remove all data for a particular module from existing structs.
 * Its primary use is modulePrepForUnload() however, based on past expericance with module coders wanting to 
 * do just about anything and everything, its safe to use from inside the module.
 * @param md The module data for the struct to be used
 **/
void moduleDelAllData(ModuleData * md[])
{
    char *mod_name = sstrdup(mod_current_module_name);
    int index = 0;
    ModuleData *current = NULL;
    ModuleData *lastHash = NULL;

    ModuleDataItem *itemCurrent = NULL;
    ModuleDataItem *prev = NULL;
    ModuleDataItem *next = NULL;
    index = CMD_HASH(mod_name);

    for (current = md[index]; current; current = current->next) {
        if (strcasecmp(current->moduleName, mod_name) == 0)
            lastHash = current;
    }
    if (lastHash) {
        for (itemCurrent = lastHash->di; itemCurrent; itemCurrent = next) {
            next = itemCurrent->next;
            free(itemCurrent->key);
            free(itemCurrent->value);
            itemCurrent->next = NULL;
            free(itemCurrent);
            if (prev) {
                prev->next = next;
            } else {
                lastHash->di = next;
            }
        }
    }
    free(mod_name);
}

/**
 * This will delete all module data used in any struct by module m.
 * @param m The module to clear all data for
 **/
void moduleDelAllDataMod(Module * m)
{
    boolean freeme = false;
    int i;
    User *user;
    if (!mod_current_module_name) {
        mod_current_module_name = sstrdup(m->name);
        freeme = true;
    }

    for (i = 0; i < 1024; i++) {
        for (user = userlist[i]; user; user = user->next) {
            moduleDelAllData(user->moduleData);
        }
    }

    if (freeme) {
        free(mod_current_module_name);
        mod_current_module_name = NULL;
    }
}



/* EOF */
