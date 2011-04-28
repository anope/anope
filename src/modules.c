
/* Modular support
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
#include "modules.h"
#include "language.h"
#include "version.h"

#if defined(USE_MODULES) && !defined(_WIN32)
#include <dlfcn.h>
/* Define these for systems without them */
#ifndef RTLD_NOW
#define RTLD_NOW 0
#endif
#ifndef RTLD_LAZY
#define RTLD_LAZY RTLD_NOW
#endif
#ifndef RTLD_GLOBAL
#define RTLD_GLOBAL 0
#endif
#ifndef RTLD_LOCAL
#define RTLD_LOCAL 0
#endif
#endif

#ifdef _WIN32
const char *ano_moderr(void);
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
User *mod_current_user;
ModuleCallBack *moduleCallBackHead = NULL;
ModuleQueue *mod_operation_queue = NULL;

int displayCommand(Command * c);
int displayCommandFromHash(CommandHash * cmdTable[], char *name);
int displayMessageFromHash(char *name);
int displayMessage(Message * m);
char *ModuleGetErrStr(int status);

char *ModuleGetErrStr(int status)
{
    const char *module_err_str[] = {
        "Module, Okay - No Error",                                             /* MOD_ERR_OK */
        "Module Error, Allocating memory",                                     /* MOD_ERR_MEMORY */
        "Module Error, Not enough parameters",                                 /* MOD_ERR_PARAMS */
        "Module Error, Already loaded",                                        /* MOD_ERR_EXISTS */
        "Module Error, File does not exist",                                   /* MOD_ERR_NOEXIST */
        "Module Error, No User",                                               /* MOD_ERR_NOUSER */
        "Module Error, Error during load time or module returned MOD_STOP",    /* MOD_ERR_NOLOAD */
        "Module Error, Unable to unload",                                      /* MOD_ERR_NOUNLOAD */
        "Module Error, Incorrect syntax",                                      /* MOD_ERR_SYNTAX */
        "Module Error, Unable to delete",                                      /* MOD_ERR_NODELETE */
        "Module Error, Unknown Error occuried",                                /* MOD_ERR_UNKOWN */
        "Module Error, File I/O Error",                                        /* MOD_ERR_FILE_IO */
        "Module Error, No Service found for request",                          /* MOD_ERR_NOSERVICE */
        "Module Error, No module name for request"                             /* MOD_ERR_NO_MOD_NAME */
    };
    return (char *) module_err_str[status];
}

/**
 * Automaticaly load modules at startup.
 * This will load modules at startup before the IRCD link is attempted, this
 * allows admins to have a module relating to ircd support load
 */
void modules_init(void)
{
#ifdef USE_MODULES
    int idx;
	int ret;
    Module *m;

    if(nothird) {
        return;
    }

    for (idx = 0; idx < ModulesNumber; idx++) {
        m = findModule(ModulesAutoload[idx]);
        if (!m) {
            m = createModule(ModulesAutoload[idx]);
            mod_current_module = m;
            mod_current_module_name = m->name;
            mod_current_user = NULL;
            alog("trying to load [%s]", mod_current_module->name);
			ret = loadModule(mod_current_module, NULL);
            alog("status: [%d][%s]", ret, ModuleGetErrStr(ret));
			if (ret != MOD_ERR_OK)
				destroyModule(m);
            mod_current_module = NULL;
            mod_current_module_name = NULL;
            mod_current_user = NULL;
        }
    }
#endif
}

/** 
 * Load up a list of core modules from the conf.
 * @param number The number of modules to load
 * @param list The list of modules to load
 **/
void modules_core_init(int number, char **list)
{
    int idx;
    Module *m;
    int status = 0;
    for (idx = 0; idx < number; idx++) {
        m = findModule(list[idx]);
        if (!m) {
            m = createModule(list[idx]);
            mod_current_module = m;
            mod_current_module_name = m->name;
            mod_current_user = NULL;
            status = loadModule(mod_current_module, NULL);
            mod_current_module = m;
            mod_current_module_name = m->name;
            if (debug || status) {
                alog("debug: trying to load core module [%s]",
                     mod_current_module->name);
                alog("debug: status: [%d][%s]", status, ModuleGetErrStr(status));
				if (status != MOD_ERR_OK)
					destroyModule(mod_current_module);
            }
            mod_current_module = NULL;
            mod_current_module_name = NULL;
            mod_current_user = NULL;
        }
    }
}
/**
 * 
 **/
int encryption_module_init(void) {
    int ret = 0;
    Module *m;
	
    m = createModule(EncModule);
    mod_current_module = m;
    mod_current_module_name = m->name;
    mod_current_user = NULL;
    alog("Loading Encryption Module: [%s]", mod_current_module->name);
    ret = loadModule(mod_current_module, NULL);
    mod_current_module = m;
    mod_current_module_name = m->name;
    moduleSetType(ENCRYPTION);
    alog("status: [%d][%s]", ret, ModuleGetErrStr(ret));
    mod_current_module = NULL;
    mod_current_module_name = NULL;
    if (ret != MOD_ERR_OK) {
        destroyModule(m);
    }
    return ret;
}

/**
 * Load the ircd protocol module up
 **/
int protocol_module_init(void)
{
    int ret = 0, noforksave = nofork;
    Module *m;
	
    m = createModule(IRCDModule);
    mod_current_module = m;
    mod_current_module_name = m->name;
    mod_current_user = NULL;
    alog("Loading IRCD Protocol Module: [%s]", mod_current_module->name);
    ret = loadModule(mod_current_module, NULL);
    mod_current_module = m;
    mod_current_module_name = m->name;
    moduleSetType(PROTOCOL);
    alog("status: [%d][%s]", ret, ModuleGetErrStr(ret));
    mod_current_module = NULL;
    mod_current_module_name = NULL;
	
	if (ret == MOD_ERR_OK) {
		/* This is really NOT the correct place to do config checks, but
		 * as we only have the ircd struct filled here, we have to over
		 * here. -GD
		 */
		if (UseTokens && !ircd->token) {
			alog("Anope does not support TOKENS for this ircd setting; unsetting UseToken");
			UseTokens = 0;
		}
		
		if (UseTS6 && !ircd->ts6) {
			alog("Chosen IRCd does not support TS6, unsetting UseTS6");
			UseTS6 = 0;
		}
		
		/* We can assume the ircd supports TS6 here */
                if (UseTS6 && !Numeric) {
                    nofork = 1; /* We're going down, set nofork so this error is printed */
                    alog("UseTS6 requires the setting of Numeric to be enabled.");
                    nofork = noforksave;
                    ret = -1;
                }
	} else {
		destroyModule(m);
	}
	
    return ret;
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
	int ret;
    Module *m;

    if(nothird) {
        return;
    }

    for (idx = 0; idx < ModulesDelayedNumber; idx++) {
        m = findModule(ModulesDelayedAutoload[idx]);
        if (!m) {
            m = createModule(ModulesDelayedAutoload[idx]);
            mod_current_module = m;
            mod_current_module_name = m->name;
            mod_current_user = NULL;
            alog("trying to load [%s]", mod_current_module->name);
			ret = loadModule(mod_current_module, NULL);
            alog("status: [%d][%s]", ret, ModuleGetErrStr(ret));
            mod_current_module = NULL;
            mod_current_module_name = NULL;
            mod_current_user = NULL;
			if (ret != MOD_ERR_OK)
				destroyModule(m);
        }
    }
#endif
}

/**
 * Unload ALL loaded modules, no matter what kind of module it is.
 * Do NEVER EVER, and i mean NEVER (and if that isn't clear enough
 * yet, i mean: NEVER AT ALL) call this unless we're shutting down,
 * or we'll fuck up Anope badly (protocol handling won't work for
 * example). If anyone calls this function without a justified need
 * for it, i reserve the right to break their legs in a painful way.
 * And if that isn't enough discouragement, you'll wake up with your
 * both legs broken tomorrow ;) -GD
 */
void modules_unload_all(boolean fini, boolean unload_proto)
{
#ifdef USE_MODULES
	int idx;
	ModuleHash *mh, *next;
        void (*func) (void);
       	
	for (idx = 0; idx < MAX_CMD_HASH; idx++) {
		mh = MODULE_HASH[idx];
		while (mh) {
			next = mh->next;
			if (unload_proto || (mh->m->type != PROTOCOL)) {
				mod_current_module = mh->m;
				mod_current_module_name = mh->m->name;
			    if(fini) {
			        func = (void (*)(void))ano_modsym(mh->m->handle, "AnopeFini");
		    	    if (func) {
		            	func();                 /* exec AnopeFini */
			        }
				
		    	    if (prepForUnload(mh->m) != MOD_ERR_OK) {
			    		mh = next;
						continue;
			        }
				
		        	if ((ano_modclose(mh->m->handle)) != 0)
			            alog("%s", ano_moderr());
			        else
			            delModule(mh->m);
		    	} else {
                	        delModule(mh->m);
				}
			mod_current_module = NULL;
			mod_current_module_name = NULL;
		    }
	   	    mh = next;
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
    int i = 0;
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

    m->type = THIRD;
    for (i = 0; i < NUM_LANGS; i++) {
        m->lang[i].argc = 0;
    }
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
    int i = 0;
    if (!m) {
        return MOD_ERR_PARAMS;
    }

    mod_current_module = m;
    mod_current_module_name = m->name;
    for (i = 0; i < NUM_LANGS; i++) {
        moduleDeleteLanguage(i);
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
    
    mod_current_module = NULL;
    mod_current_module_name = NULL;

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
        if (stricmp(m->name, current->name) == 0)
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
        if (stricmp(m->name, current->name) == 0) {
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
 * Search all loaded modules looking for a protocol module.
 * @return 1 if one is found.
 **/
int protocolModuleLoaded()
{
    int idx = 0;
    ModuleHash *current = NULL;

    for (idx = 0; idx != MAX_CMD_HASH; idx++) {
        for (current = MODULE_HASH[idx]; current; current = current->next) {
            if (current->m->type == PROTOCOL) {
                return 1;
            }
        }
    }
    return 0;
}

/**
 * Search all loaded modules looking for an encryption module.
 * @ return 1 if one is loaded
 **/
int encryptionModuleLoaded()
{
    int idx = 0;
    ModuleHash *current = NULL;

    for (idx = 0; idx != MAX_CMD_HASH; idx++) {
        for (current = MODULE_HASH[idx]; current; current = current->next) {
            if (current->m->type == ENCRYPTION) {
                return 1;
            }
        }
    }
    return 0;
}

/** 
 * Copy the module from the modules folder to the runtime folder.
 * This will prevent module updates while the modules is loaded from
 * triggering a segfault, as the actaul file in use will be in the 
 * runtime folder.
 * @param name the name of the module to copy
 * @param output the destination to copy the module to
 * @return MOD_ERR_OK on success
 */
int moduleCopyFile(char *name, char *output)
{
#ifdef USE_MODULES
    int ch;
    FILE *source, *target;
	int srcfp;
    char input[4096] = "";
    int len;

    strncpy(input, MODULE_PATH, 4095);  /* Get full path with module extension */
    len = strlen(input);
    strncat(input, name, 4095 - len);
    len = strlen(output);
    strncat(input, MODULE_EXT, 4095 - len);

#ifndef _WIN32
	if ((srcfp = mkstemp(output)) == -1)
		return MOD_ERR_FILE_IO;
#else
	if (!mktemp(output))
		return MOD_ERR_FILE_IO;
#endif
	
	if (debug)
		alog("Runtime module location: %s", output);
	
	/* Linux/UNIX should ignore the b param, why do we still have seperate
	 * calls for it here? -GD
	 */
#ifndef _WIN32
    if ((source = fopen(input, "r")) == NULL) {
        close(srcfp);
#else
    if ((source = fopen(input, "rb")) == NULL) {
#endif
        return MOD_ERR_NOEXIST;
    }
#ifndef _WIN32
    if ((target = fdopen(srcfp, "w")) == NULL) {
#else
    if ((target = fopen(output, "wb")) == NULL) {
#endif
        fclose(source);
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
    int (*version)();
    int ret = 0;
    char *argv[1];
    int argc = 0;

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
	
	/* Generate the filename for the temporary copy of the module */
    strncpy(buf, MODULE_PATH, 4095);    /* Get full path with module extension */
    len = strlen(buf);
#ifndef _WIN32
    strncat(buf, "runtime/", 4095 - len);
#else
    strncat(buf, "runtime\\", 4095 - len);
#endif
    len = strlen(buf);
    strncat(buf, m->name, 4095 - len);
    len = strlen(buf);
    strncat(buf, MODULE_EXT, 4095 - len);
	len = strlen(buf);
	strncat(buf, ".", 4095 - len);
	len = strlen(buf);
	strncat(buf, "XXXXXX", 4095 - len);
    buf[4095] = '\0';
	/* Don't skip return value checking! -GD */
    if ((ret = moduleCopyFile(m->name, buf)) != MOD_ERR_OK) {
        m->filename = sstrdup(buf);
	return ret;
	}

    m->filename = sstrdup(buf);
    ano_modclearerr();
    m->handle = ano_modopen(m->filename);
    if ( m->handle == NULL && (err = ano_moderr()) != NULL) {
        alog("%s", err);
        return MOD_ERR_NOLOAD;
    }
    ano_modclearerr();
    func = (int (*)(int, char **))ano_modsym(m->handle, "AnopeInit");
    if ( func == NULL && (err = ano_moderr()) != NULL) {
        ano_modclose(m->handle);        /* If no AnopeInit - it isnt an Anope Module, close it */
        return MOD_ERR_NOLOAD;
    }
    if (func) {
	version = (int (*)())ano_modsym(m->handle,"getAnopeBuildVersion");
	if (version) {
        if (version() >= VERSION_BUILD ) {
            if(debug) {
                alog("Module %s compiled against current or newer anope revision %d, this is %d",m->name,version(),VERSION_BUILD);
            }
        } else {
            alog("Module %s is compiled against an old version of anope (%d) current is %d", m->name, version(), VERSION_BUILD);
            alog("Rebuild module %s against the current version to resolve this error", m->name);
            ano_modclose(m->handle);
            ano_modclearerr();
            return MOD_ERR_NOLOAD;
        }
    } else {
        ano_modclose(m->handle);
        ano_modclearerr();
        alog("Module %s is compiled against an older version of anope (unknown)", m->name);
        alog("Rebuild module %s against the current version to resolve this error", m->name);
        return MOD_ERR_NOLOAD;
	}
	/* TODO */
        mod_current_module = m;
        mod_current_module_name = m->name;
        /* argv[0] is the user if there was one, or NULL if not */
        if (u) {
            argv[0] = sstrdup(u->nick);
        } else {
            argv[0] = NULL;
        }
        argc++;

        ret = func(argc, argv); /* exec AnopeInit */
        if (u) {
            free(argv[0]);
        }
        if (m->type == PROTOCOL && protocolModuleLoaded()) {
            alog("You cannot load two protocol modules");
            ret = MOD_STOP;
        } else if (m->type == ENCRYPTION && encryptionModuleLoaded()) {
            alog("You cannot load two encryption modules");
            ret = MOD_STOP;
        }
        if (ret == MOD_STOP) {
            alog("%s requested unload...", m->name);
            unloadModule(m, NULL);
            mod_current_module = NULL;
            mod_current_module_name = NULL;
            return MOD_ERR_NOLOAD;
        }

        mod_current_module = NULL;
        mod_current_module_name = NULL;
    }

    if (u) {
        anope_cmd_global(s_OperServ, "%s loaded module %s", u->nick,
                         m->name);
        notice_lang(s_OperServ, u, OPER_MODULE_LOADED, m->name);
    }
    addModule(m);

    /* Loading is complete.. send out an event in case anyone s interested.. ~ Viper */
    send_event(EVENT_MODLOAD, 1, m->name);

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
    void (*func) (void);

    if (!m || !m->handle) {
        if (u) {
            notice_lang(s_OperServ, u, OPER_MODULE_REMOVE_FAIL, m->name);
        }
        return MOD_ERR_PARAMS;
    }

    if (m->type == PROTOCOL) {
        if (u) {
            notice_lang(s_OperServ, u, OPER_MODULE_NO_UNLOAD);
        }
        return MOD_ERR_NOUNLOAD;
    } else if(m->type == ENCRYPTION) {
        if (u) {
            notice_lang(s_OperServ, u, OPER_MODULE_NO_UNLOAD);
        }
        return MOD_ERR_NOUNLOAD;
    }

    func = (void (*)(void))ano_modsym(m->handle, "AnopeFini");
    if (func) {
        mod_current_module = m;
        mod_current_module_name = m->name;
        func();                 /* exec AnopeFini */
    }

    if (prepForUnload(m) != MOD_ERR_OK) {
        return MOD_ERR_UNKNOWN;
    }

    /* Unloading is complete: AnopeFini has been called and all commands, hooks and callbacks
     * have been removed.. send out an event in case anyone s interested.. ~ Viper */
    send_event(EVENT_MODUNLOAD, 1, m->name);

    if ((ano_modclose(m->handle)) != 0) {
        alog("%s", ano_moderr());
        if (u) {
            notice_lang(s_OperServ, u, OPER_MODULE_REMOVE_FAIL, m->name);
        }
        mod_current_module = NULL;
        mod_current_module_name = NULL;
        return MOD_ERR_NOUNLOAD;
    } else {
        if (u) {
            anope_cmd_global(s_OperServ, "%s unloaded module %s", u->nick,
                             m->name);
            notice_lang(s_OperServ, u, OPER_MODULE_UNLOADED, m->name);
        }
        delModule(m);
        mod_current_module = NULL;
        mod_current_module_name = NULL;
        return MOD_ERR_OK;
    }
#else
    return MOD_ERR_NOUNLOAD;
#endif
}

/**
 * Module setType() 
 * Lets the module set a type, CORE,PROTOCOL,3RD etc..
 **/
void moduleSetType(MODType type)
{
    mod_current_module->type = type;
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
    EvtMessageHash *ecurrent = NULL;
    EvtHookHash *ehcurrent = NULL;

    Command *c;
    Message *msg;
    EvtMessage *eMsg;
    EvtHook *eHook;
    int status = 0;

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

        for (ecurrent = EVENT[idx]; ecurrent; ecurrent = ecurrent->next) {
            for (eMsg = ecurrent->evm; eMsg; eMsg = eMsg->next) {
                if ((eMsg->mod_name)
                    && (stricmp(eMsg->mod_name, m->name) == 0)) {
                    status = delEventHandler(EVENT, eMsg, m->name);
                }
            }
        }
        for (ehcurrent = EVENTHOOKS[idx]; ehcurrent;
             ehcurrent = ehcurrent->next) {
            for (eHook = ehcurrent->evh; eHook; eHook = eHook->next) {
                if ((eHook->mod_name)
                    && (stricmp(eHook->mod_name, m->name) == 0)) {
                    status = delEventHook(EVENTHOOKS, eHook, m->name);
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
    if (!name || !*name) {
        return NULL;
    }

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

    if (debug >= 2)
        displayCommandFromHash(cmdTable, c->name);
    status = addCommand(cmdTable, c, pos);
    if (debug >= 2)
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
            && stricmp(cmd->mod_name, mod_current_module->name) == 0) {
            if (debug >= 2) {
                displayCommandFromHash(cmdTable, name);
            }
            status = delCommand(cmdTable, cmd, mod_current_module->name);
            if (debug >= 2) {
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
        alog("debug: trying to display command %s", name);
    }
    for (current = cmdTable[index]; current; current = current->next) {
        if (stricmp(name, current->name) == 0) {
            displayCommand(current->c);
        }
    }
    if (debug > 1) {
        alog("debug: done displaying command %s", name);
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
        alog("%d:  0x%p", ++i, (void *) cmd);
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
        alog("debug: trying to display message %s", name);
    }
    for (current = IRCD[index]; current; current = current->next) {
        if (stricmp(name, current->name) == 0) {
            displayMessage(current->m);
        }
    }
    if (debug > 1) {
        alog("debug: done displaying message %s", name);
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
        alog("%d: 0x%p", ++i, (void *) msg);
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

    if (mod_current_module_name && !c->mod_name)
        return MOD_ERR_NO_MOD_NAME;

    index = CMD_HASH(c->name);

    for (current = cmdTable[index]; current; current = current->next) {
        if ((c->service) && (current->c) && (current->c->service)
            && (!strcmp(c->service, current->c->service) == 0)) {
            continue;
        }
        if ((stricmp(c->name, current->name) == 0)) {   /* the cmd exist's we are a addHead */
            if (pos == 1) {
                c->next = current->c;
                current->c = c;
                if (debug)
                    alog("debug: existing cmd: (0x%p), new cmd (0x%p)",
                         (void *) c->next, (void *) c);
                send_event(EVENT_ADDCOMMAND, 2, c->mod_name, c->name);
                return MOD_ERR_OK;
            } else if (pos == 2) {

                tail = current->c;
                while (tail->next)
                    tail = tail->next;
                if (debug)
                    alog("debug: existing cmd: (0x%p), new cmd (0x%p)",
                         (void *) tail, (void *) c);
                tail->next = c;
                c->next = NULL;

                send_event(EVENT_ADDCOMMAND, 2, c->mod_name, c->name);
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

    send_event(EVENT_ADDCOMMAND, 2, c->mod_name, c->name);
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
        if (stricmp(c->name, current->name) == 0) {
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
                            send_event(EVENT_DELCOMMAND, 2, c->mod_name, c->name);
                            return MOD_ERR_OK;
                        }
                        last = tail;
                        tail = tail->next;
                    }
                } else {
                    cmdTable[index] = current->next;
                    free(current->name);
                    send_event(EVENT_DELCOMMAND, 2, c->mod_name, c->name);
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
                            send_event(EVENT_DELCOMMAND, 2, c->mod_name, c->name);
                            return MOD_ERR_OK;
                        }
                        last = tail;
                        tail = tail->next;
                    }
                } else {
                    lastHash->next = current->next;
                    free(current->name);
                    send_event(EVENT_DELCOMMAND, 2, c->mod_name, c->name);
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
Message *createMessage(const char *name,
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
        if (UseTokens) {
            if (ircd->tokencaseless) {
                if (stricmp(name, current->name) == 0) {
                    return current->m;
                }
            } else {
                if (strcmp(name, current->name) == 0) {
                    return current->m;
                }
            }
        } else {
            if (stricmp(name, current->name) == 0) {
                return current->m;
            }
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
    int match = 0;

    if (!msgTable || !m || (pos < 0 || pos > 2)) {
        return MOD_ERR_PARAMS;
    }

    index = CMD_HASH(m->name);

    for (current = msgTable[index]; current; current = current->next) {
        if ((UseTokens) && (!ircd->tokencaseless)) {
            match = strcmp(m->name, current->name);
        } else {
            match = stricmp(m->name, current->name);
        }
        if (match == 0) {       /* the msg exist's we are a addHead */
            if (pos == 1) {
                m->next = current->m;
                current->m = m;
                if (debug)
                    alog("debug: existing msg: (0x%p), new msg (0x%p)",
                         (void *) m->next, (void *) m);
                return MOD_ERR_OK;
            } else if (pos == 2) {
                tail = current->m;
                while (tail->next)
                    tail = tail->next;
                if (debug)
                    alog("debug: existing msg: (0x%p), new msg (0x%p)",
                         (void *) tail, (void *) m);
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
        if (stricmp(m->name, current->name) == 0) {
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
void moduleAddVersion(const char *version)
{
    if (mod_current_module && version) {
        mod_current_module->version = sstrdup(version);
    }
}

/**
 * Add the modules author info
 * @param author the author of the module
 **/
void moduleAddAuthor(const char *author)
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
        alog("debug: added module CallBack: [%s] due to execute at %ld",
             new->name ? new->name : "?", (long int) new->when);
    return MOD_ERR_OK;
}

/**
 * Execute a stored call back
 **/
void moduleCallBackRun(void)
{
    ModuleCallBack *tmp;
	
	while ((tmp = moduleCallBackHead) && (tmp->when <= time(NULL))) {
		if (debug)
			alog("debug: executing callback: %s", tmp->name ? tmp->name : "<unknown>");
		if (tmp->func) {
			mod_current_module = findModule(tmp->owner_name);
			mod_current_module_name = tmp->owner_name;
			tmp->func(tmp->argc, tmp->argv);
			mod_current_module = NULL;
			mod_current_module_name = NULL;
			moduleCallBackDeleteEntry(NULL);
		}
	}
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
                    alog("debug: removing CallBack %s for module %s", name,
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
            alog("debug: removing CallBack for module %s", mod_name);
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
    ModuleHash *current = NULL;
	Module *calling_module = mod_current_module;
	char *calling_module_name = mod_current_module_name;

    for (idx = 0; idx != MAX_CMD_HASH; idx++) {
        for (current = MODULE_HASH[idx]; current; current = current->next) {
			mod_current_module_name = current->name;
			mod_current_module = current->m;
			
            if ((service == 1) && current->m->nickHelp) {
                current->m->nickHelp(u);
            } else if ((service == 2) && current->m->chanHelp) {
                current->m->chanHelp(u);
            } else if ((service == 3) && current->m->memoHelp) {
                current->m->memoHelp(u);
            } else if ((service == 4) && current->m->botHelp) {
                current->m->botHelp(u);
            } else if ((service == 5) && current->m->operHelp) {
                current->m->operHelp(u);
            } else if ((service == 6) && current->m->hostHelp) {
                current->m->hostHelp(u);
            } else if ((service == 7) && current->m->helpHelp) {
                current->m->helpHelp(u);
            }
        }
    }
	
	mod_current_module = calling_module;
	mod_current_module_name = calling_module_name;
#endif
}

/**
 * Output module data information into the log file.
 * This is a vwey "debug only" function to dump the whole contents
 * of a moduleData struct into the log files.
 * @param md The module data for the struct to be used
 * @return 0 is always returned;
 **/
int moduleDataDebug(ModuleData ** md)
{
    ModuleData *current = NULL;
    alog("Dumping module data....");
    for (current = *md; current; current = current->next) {
        alog("Module: [%s]", current->moduleName);
        alog(" Key [%s]\tValue [%s]", current->key, current->value);
    }
    alog("End of module data dump");
    return 0;
}

/**
 * Add module data to a struct.
 * This allows module coders to add data to an existing struct
 * @param md The module data for the struct to be used
 * @param key The Key for the key/value pair
 * @param value The value for the key/value pair, this is what will be stored for you
 * @return MOD_ERR_OK will be returned on success
 **/
int moduleAddData(ModuleData ** md, char *key, char *value)
{
    ModuleData *newData = NULL;

    if (mod_current_module_name == NULL) {
        alog("moduleAddData() called with mod_current_module_name being NULL");
        if (debug)
            do_backtrace(0);
    }

    if (!key || !value) {
        alog("A module (%s) tried to use ModuleAddData() with one or more NULL arguments... returning", mod_current_module_name);
        return MOD_ERR_PARAMS;
    }

    moduleDelData(md, key);     /* Remove any existing module data for this module with the same key */

    newData = malloc(sizeof(ModuleData));
    if (!newData) {
        return MOD_ERR_MEMORY;
    }

    newData->moduleName = sstrdup(mod_current_module_name);
    newData->key = sstrdup(key);
    newData->value = sstrdup(value);
    newData->next = *md;
    *md = newData;

    if (debug) {
        moduleDataDebug(md);
    }
    return MOD_ERR_OK;
}

/**
 * Returns the value from a key/value pair set.
 * This allows module coders to retrive any data they have previuosly stored in any given struct
 * @param md The module data for the struct to be used
 * @param key The key to find the data for
 * @return the value paired to the given key will be returned, or NULL 
 **/
char *moduleGetData(ModuleData ** md, char *key)
{
    /* See comment in moduleAddData... -GD */
    char *mod_name = sstrdup(mod_current_module_name);
    ModuleData *current = *md;

    if (mod_current_module_name == NULL) {
        alog("moduleGetData() called with mod_current_module_name being NULL");
        if (debug)
            do_backtrace(0);
    }

    if (debug > 1) {
        alog("debug: moduleGetData %p : key %s", (void *) md, key);
        alog("debug: Current Module %s", mod_name);
    }

    while (current) {
        if ((stricmp(current->moduleName, mod_name) == 0)
            && (stricmp(current->key, key) == 0)) {
            free(mod_name);
            return sstrdup(current->value);
        }
        current = current->next;
    }
    free(mod_name);
    return NULL;
}

/**
 * Delete the key/value pair indicated by "key" for the current module.
 * This allows module coders to remove a previously stored key/value pair.
 * @param md The module data for the struct to be used
 * @param key The key to delete the key/value pair for
 **/
void moduleDelData(ModuleData ** md, char *key)
{
    /* See comment in moduleAddData... -GD */
    char *mod_name = sstrdup(mod_current_module_name);
    ModuleData *current = *md;
    ModuleData *prev = NULL;
    ModuleData *next = NULL;

    if (mod_current_module_name == NULL) {
        alog("moduleDelData() called with mod_current_module_name being NULL");
        if (debug)
            do_backtrace(0);
    }

    if (key) {
        while (current) {
            next = current->next;
            if ((stricmp(current->moduleName, mod_name) == 0)
                && (stricmp(current->key, key) == 0)) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    *md = current->next;
                }
                free(current->moduleName);
                free(current->key);
                free(current->value);
                current->next = NULL;
                free(current);
            } else {
                prev = current;
            }
            current = next;
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
void moduleDelAllData(ModuleData ** md)
{
    /* See comment in moduleAddData... -GD */
    char *mod_name = sstrdup(mod_current_module_name);
    ModuleData *current = *md;
    ModuleData *prev = NULL;
    ModuleData *next = NULL;

    if (mod_current_module_name == NULL) {
        alog("moduleDelAllData() called with mod_current_module_name being NULL");
        if (debug)
            do_backtrace(0);
    }

    while (current) {
        next = current->next;
        if ((stricmp(current->moduleName, mod_name) == 0)) {
            if (prev) {
                prev->next = current->next;
            } else {
                *md = current->next;
            }
            free(current->moduleName);
            free(current->key);
            free(current->value);
            current->next = NULL;
            free(current);
        } else {
            prev = current;
        }
        current = next;
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
    int i, j;
    User *user;
    NickAlias *na;
    NickCore *nc;
    ChannelInfo *ci;

    if (!mod_current_module_name) {
        mod_current_module_name = sstrdup(m->name);
        freeme = true;
    }

    for (i = 0; i < 1024; i++) {
        /* Remove the users */
        for (user = userlist[i]; user; user = user->next) {
            moduleDelAllData(&user->moduleData);
        }
        /* Remove the nick Cores */
        for (nc = nclists[i]; nc; nc = nc->next) {
            moduleDelAllData(&nc->moduleData);
            /* Remove any memo data for this nick core */
            for (j = 0; j < nc->memos.memocount; j++) {
                moduleCleanStruct(&nc->memos.memos[j].moduleData);
            }
        }
        /* Remove the nick Aliases */
        for (na = nalists[i]; na; na = na->next) {
            moduleDelAllData(&na->moduleData);
        }
    }

    for (i = 0; i < 256; i++) {
        /* Remove any chan info data */
        for (ci = chanlists[i]; ci; ci = ci->next) {
            moduleDelAllData(&ci->moduleData);
            /* Remove any memo data for this nick core */
            for (j = 0; j < ci->memos.memocount; j++) {
                moduleCleanStruct(&ci->memos.memos[j].moduleData);
            }
        }
    }

    if (freeme) {
        free(mod_current_module_name);
        mod_current_module_name = NULL;
    }
}

/**
 * Remove any data from any module used in the given struct.
 * Useful for cleaning up when a User leave's the net, a NickCore is deleted, etc...
 * @param moduleData the moduleData struct to "clean"
 **/
void moduleCleanStruct(ModuleData ** moduleData)
{
    ModuleData *current = *moduleData;
    ModuleData *next = NULL;

    while (current) {
        next = current->next;
        free(current->moduleName);
        free(current->key);
        free(current->value);
        current->next = NULL;
        free(current);
        current = next;
    }
    *moduleData = NULL;
}

/**
 * Check the current version of anope against a given version number
 * Specifiying -1 for minor,patch or build 
 * @param major The major version of anope, the first part of the verison number
 * @param minor The minor version of anope, the second part of the version number
 * @param patch The patch version of anope, the third part of the version number
 * @param build The build revision of anope from SVN
 * @return True if the version newer than the version specified.
 **/
boolean moduleMinVersion(int major, int minor, int patch, int build)
{
    boolean ret = false;
    if (VERSION_MAJOR > major) {        /* Def. new */
        ret = true;
    } else if (VERSION_MAJOR == major) {        /* Might be newer */
        if (minor == -1) {
            return true;
        }                       /* They dont care about minor */
        if (VERSION_MINOR > minor) {    /* Def. newer */
            ret = true;
        } else if (VERSION_MINOR == minor) {    /* Might be newer */
            if (patch == -1) {
                return true;
            }                   /* They dont care about patch */
            if (VERSION_PATCH > patch) {
                ret = true;
            } else if (VERSION_PATCH == patch) {
                if (build == -1) {
                    return true;
                }               /* They dont care about build */
                if (VERSION_BUILD >= build) {
                    ret = true;
                }
            }
        }
    }
    return ret;
}

#ifdef _WIN32
const char *ano_moderr(void)
{
    static char errbuf[513];
    DWORD err = GetLastError();
    if (err == 0)
        return NULL;
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, 0, errbuf, 512,
                  NULL);
    return errbuf;
}
#endif

/**
 * Allow ircd protocol files to update the protect level info tables.
 **/
void updateProtectDetails(char *level_info_protect_word,
                          char *level_info_protectme_word,
                          char *fant_protect_add, char *fant_protect_del,
                          char *level_protect_word, char *protect_set_mode,
                          char *protect_unset_mode)
{
    int i = 0;
    CSModeUtil ptr;
    LevelInfo l_ptr;

    ptr = csmodeutils[i];
    while (ptr.name) {
        if (strcmp(ptr.name, "PROTECT") == 0) {
            csmodeutils[i].bsname = sstrdup(fant_protect_add);
            csmodeutils[i].mode = sstrdup(protect_set_mode);
        } else if (strcmp(ptr.name, "DEPROTECT") == 0) {
            csmodeutils[i].bsname = sstrdup(fant_protect_del);
            csmodeutils[i].mode = sstrdup(protect_unset_mode);
        }
        ptr = csmodeutils[++i];
    }

    i = 0;
    l_ptr = levelinfo[i];
    while (l_ptr.what != -1) {
        if (l_ptr.what == CA_PROTECT) {
            levelinfo[i].name = sstrdup(level_info_protect_word);
        } else if (l_ptr.what == CA_PROTECTME) {
            levelinfo[i].name = sstrdup(level_info_protectme_word);
        } else if (l_ptr.what == CA_AUTOPROTECT) {
            levelinfo[i].name = sstrdup(level_protect_word);
        }
        l_ptr = levelinfo[++i];
    }
}

/**
 * Deal with modules who want to lookup config directives!
 * @param h The Directive to lookup in the config file
 * @return 1 on success, 0 on error
 **/
int moduleGetConfigDirective(Directive * d)
{
    FILE *config;
    char *dir = NULL;
    char buf[1024];
	char *directive;
    int linenum = 0;
    int ac = 0;
    char *av[MAXPARAMS];
	char *str = NULL;
    char *s = NULL;
	char *t = NULL;
    int retval = 1;

    config = fopen(SERVICES_CONF, "r");
    if (!config) {
        alog("Can't open %s", SERVICES_CONF);
        return 0;
    }
    while (fgets(buf, sizeof(buf), config)) {
        linenum++;
        if (*buf == '#' || *buf == '\r' || *buf == '\n') {
            continue;
		}
        dir = myStrGetOnlyToken(buf, '\t', 0);
        if (dir) {
            str = myStrGetTokenRemainder(buf, '\t', 1);
        } else {
            dir = myStrGetOnlyToken(buf, ' ', 0);
            if (dir || (dir = myStrGetOnlyToken(buf, '\n', 0))) {
                str = myStrGetTokenRemainder(buf, ' ', 1);
            } else {
                continue;
            }
        }
		if (dir) {
			directive = normalizeBuffer(dir);
		} else {
			continue;
		}	

        if (stricmp(directive, d->name) == 0) {
            if (str) {
				s = str;
                while (isspace(*s))
                    s++;
                while (*s) {
                    if (ac >= MAXPARAMS) {
                        alog("module error: too many config. params");
                        break;
                    }
                    t = s;
                    if (*s == '"') {
                        t++;
                        s++;
                        while (*s && *s != '"') {
                            if (*s == '\\' && s[1] != 0)
                                s++;
                            s++;
                        }
                        if (!*s)
                            alog("module error: Warning: unterminated double-quoted string");
                        else
                            *s++ = 0;
                    } else {
                        s += strcspn(s, " \t\r\n");
                        if (*s)
                            *s++ = 0;
                    }
                    av[ac++] = t;
                    while (isspace(*s))
                        s++;
                }
            }
            retval = parse_directive(d, directive, ac, av, linenum, 0, s);
        }
		if (directive) {
			free(directive);
		}
    }
    if (dir)
    	free(dir);
    if (str)
       free(str);
    fclose(config);
    return retval;
}

/**
 * Allow a module to add a set of language strings to anope
 * @param langNumber the language number for the strings
 * @param ac The language count for the strings
 * @param av The language sring list.
 **/
void moduleInsertLanguage(int langNumber, int ac, char **av)
{
    int i;

    if ((mod_current_module_name) && (!mod_current_module || strcmp(mod_current_module_name, mod_current_module->name))) {
        mod_current_module = findModule(mod_current_module_name);
    }
	
	if (debug)
		alog("debug: %s Adding %d texts for language %d", mod_current_module->name, ac, langNumber);
	
    if (mod_current_module->lang[langNumber].argc > 0) {
        moduleDeleteLanguage(langNumber);
    }

    mod_current_module->lang[langNumber].argc = ac;
    mod_current_module->lang[langNumber].argv =
        malloc(sizeof(char *) * ac);
    for (i = 0; i < ac; i++) {
        mod_current_module->lang[langNumber].argv[i] = sstrdup(av[i]);
    }
}

/**
 * Send a notice to the user in the correct language, or english.
 * @param source Who sends the notice
 * @param u The user to send the message to
 * @param number The message number
 * @param ... The argument list
 **/
void moduleNoticeLang(char *source, User * u, int number, ...)
{
    va_list va;
    char buffer[4096], outbuf[4096];
    char *fmt = NULL;
    int lang = NSDefLanguage;
    char *s, *t, *buf;

    if ((mod_current_module_name) && (!mod_current_module || strcmp(mod_current_module_name, mod_current_module->name))) {
        mod_current_module = findModule(mod_current_module_name);
    }
	
    /* Find the users lang, and use it if we can */
    if (u && u->na && u->na->nc) {
        lang = u->na->nc->language;
    }

    /* If the users lang isnt supported, drop back to English */
    if (mod_current_module->lang[lang].argc == 0) {
        lang = LANG_EN_US;
    }

    /* If the requested lang string exists for the language */
    if (mod_current_module->lang[lang].argc > number) {
        fmt = mod_current_module->lang[lang].argv[number];

        buf = sstrdup(fmt);
        va_start(va, number);
        vsnprintf(buffer, 4095, buf, va);
        va_end(va);
        s = buffer;
        while (*s) {
            t = s;
            s += strcspn(s, "\n");
            if (*s)
                *s++ = '\0';
            strscpy(outbuf, t, sizeof(outbuf));
            notice_user(source, u, "%s", outbuf);
        }
		free(buf);
    } else {
        alog("%s: INVALID language string call, language: [%d], String [%d]", mod_current_module->name, lang, number);
    }
}

/**
 * Get the text of the given lanugage string in the corrent language, or
 * in english.
 * @param u The user to send the message to
 * @param number The message number
 **/
char *moduleGetLangString(User * u, int number)
{
    int lang = NSDefLanguage;

    if ((mod_current_module_name) && (!mod_current_module || strcmp(mod_current_module_name, mod_current_module->name)))
        mod_current_module = findModule(mod_current_module_name);
	
    /* Find the users lang, and use it if we can */
    if (u && u->na && u->na->nc)
        lang = u->na->nc->language;

    /* If the users lang isnt supported, drop back to English */
    if (mod_current_module->lang[lang].argc == 0)
        lang = LANG_EN_US;

    /* If the requested lang string exists for the language */
    if (mod_current_module->lang[lang].argc > number) {
        return mod_current_module->lang[lang].argv[number];
	/* Return an empty string otherwise, because we might be used without
	 * the return value being checked. If we would return NULL, bad things
	 * would happen!
	 */
	} else {
        alog("%s: INVALID language string call, language: [%d], String [%d]", mod_current_module->name, lang, number);
		return "";
    }
}

/**
 * Delete a language from a module
 * @param langNumber the language Number to delete
 **/
void moduleDeleteLanguage(int langNumber)
{
    int idx = 0;
    if ((mod_current_module_name) && (!mod_current_module || strcmp(mod_current_module_name, mod_current_module->name))) {
        mod_current_module = findModule(mod_current_module_name);
    }
    for (idx = 0; idx > mod_current_module->lang[langNumber].argc; idx++) {
        free(mod_current_module->lang[langNumber].argv[idx]);
    }
    mod_current_module->lang[langNumber].argc = 0;
}

/**
 * Enqueue a module operation (load/unload/reload)
 * @param m Module to perform the operation on
 * @param op Operation to perform on the module
 * @param u User who requested the operation
 **/
void queueModuleOperation(Module *m, ModuleOperation op, User *u)
{
	ModuleQueue *qm;
	
	qm = scalloc(1, sizeof(ModuleQueue));
	qm->m = m;
	qm->op = op;
	qm->u = u;
	qm->next = mod_operation_queue;
	mod_operation_queue = qm;
}

/**
 * Enqueue a module to load
 * @param name Name of the module to load
 * @param u User who requested the load
 * @return 1 on success, 0 on error
 **/
int queueModuleLoad(char *name, User *u)
{
	Module *m;
	
	if (!name || !u)
		return 0;
	
	if (findModule(name))
		return 0;
	m = createModule(name);
	queueModuleOperation(m, MOD_OP_LOAD, u);
	
	return 1;
}

/**
 * Enqueue a module to unload
 * @param name Name of the module to unload
 * @param u User who requested the unload
 * @return 1 on success, 0 on error
 **/
int queueModuleUnload(char *name, User *u)
{
	Module *m;
	
	if (!name || !u)
		return 0;
	
	m = findModule(name);
	if (!m)
		return 0;
	queueModuleOperation(m, MOD_OP_UNLOAD, u);
	
	return 1;
}

/**
 * Execute all queued module operations
 **/
void handleModuleOperationQueue(void)
{
	ModuleQueue *next;
	int status;
	
	if (!mod_operation_queue)
		return;
	
	while (mod_operation_queue) {
		next = mod_operation_queue->next;
		
		mod_current_module = mod_operation_queue->m;
		mod_current_user = mod_operation_queue->u;

		if (mod_operation_queue->op == MOD_OP_LOAD) {
			alog("Trying to load module [%s]", mod_operation_queue->m->name);
			status = loadModule(mod_operation_queue->m, mod_operation_queue->u);
			alog("Module loading status: %d (%s)", status, ModuleGetErrStr(status));
			if (status != MOD_ERR_OK) {
			        if(mod_current_user) {
                                   notice_lang(s_OperServ, mod_current_user, OPER_MODULE_LOAD_FAIL,mod_operation_queue->m->name);
				}
				destroyModule(mod_operation_queue->m);
			}
		} else if (mod_operation_queue->op == MOD_OP_UNLOAD) {
			alog("Trying to unload module [%s]", mod_operation_queue->m->name);
			status = unloadModule(mod_operation_queue->m, mod_operation_queue->u);
			alog("Module unloading status: %d (%s)", status, ModuleGetErrStr(status));
		}
		
		/* Remove the ModuleQueue from memory */
		free(mod_operation_queue);
		
		mod_operation_queue = next;
	}
	
	mod_current_module = NULL;
	mod_current_user = NULL;
}

void ModuleRunTimeDirCleanUp(void)
{
#ifndef _WIN32
    DIR *dirp;
    struct dirent *dp;
#else
    BOOL fFinished;
    HANDLE hList;
    TCHAR szDir[MAX_PATH + 1];
    TCHAR szSubDir[MAX_PATH + 1];
    WIN32_FIND_DATA FileData;
    char buffer[_MAX_PATH];
#endif
    char dirbuf[BUFSIZE];
    char filebuf[BUFSIZE];


#ifndef _WIN32
    snprintf(dirbuf, BUFSIZE, "%s/modules/runtime", services_dir);
#else
    snprintf(dirbuf, BUFSIZE, "\\%s", "modules/runtime");
#endif

	if (debug) {
		alog("debug: Cleaning out Module run time directory (%s) - this may take a moment please wait", dirbuf);
	}

#ifndef _WIN32
    if ((dirp = opendir(dirbuf)) == NULL) {
		if (debug) {
	        alog("debug: cannot open directory (%s)", dirbuf);
		}
        return;
    }
    while ((dp = readdir(dirp)) != NULL) {
        if (dp->d_ino == 0) {
            continue;
        }
        if (!stricmp(dp->d_name, ".") || !stricmp(dp->d_name, "..")) {
            continue;
        }
	    snprintf(filebuf, BUFSIZE, "%s/%s", dirbuf, dp->d_name);
		unlink(filebuf);
    }
    closedir(dirp);
#else
    /* Get the current working directory: */
    if (_getcwd(buffer, _MAX_PATH) == NULL) {
		if (debug) {
	        alog("debug: Unable to set Current working directory");
		}
    }
    snprintf(szDir, sizeof(szDir), "%s\\%s\\*", buffer, dirbuf);

    hList = FindFirstFile(szDir, &FileData);
    if (hList != INVALID_HANDLE_VALUE) {
        fFinished = FALSE;
        while (!fFinished) {
            if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			    snprintf(filebuf, BUFSIZE, "%s/%s", dirbuf, FileData.cFileName);
				DeleteFile(filebuf);
            }
            if (!FindNextFile(hList, &FileData)) {
                if (GetLastError() == ERROR_NO_MORE_FILES) {
                    fFinished = TRUE;
                }
            }
        }
    } else {
		if (debug) {
	        alog("debug: Invalid File Handle. GetLastError reports %d\n", GetLastError());
		}
    }
    FindClose(hList);
#endif
	if (debug) {
		alog("debug: Module run time directory has been cleaned out");
	}
}

/* EOF */
