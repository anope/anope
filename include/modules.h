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
 */

#ifndef MODULES_H
#define MODULES_H

#include <time.h>
#include "services.h"
#include <stdio.h>

/* Cross OS compatibility macros */
#ifdef _WIN32
typedef HMODULE ano_module_t;

#define ano_modopen(file)		LoadLibrary(file)
/* ano_moderr in modules.c */
#define ano_modsym(file, symbol)	(void *)GetProcAddress(file, symbol)
#define ano_modclose(file)		FreeLibrary(file) ? 0 : 1
#define ano_modclearerr()		SetLastError(0)
#define MODULE_EXT			".dll"

#else
typedef void *	ano_module_t;

#define ano_modopen(file) 		dlopen(file, RTLD_LAZY)
#define ano_moderr()			dlerror()
#define ano_modsym(file, symbol)	dlsym(file, DL_PREFIX symbol)
#define ano_modclose(file)		dlclose(file)
/* We call dlerror() here because it clears the module error after being
 * called. This previously read 'errno = 0', but that didn't work on
 * all POSIX-compliant architectures. This way the error is guaranteed
 * to be cleared, POSIX-wise. -GD
 */
#define ano_modclearerr()		dlerror()
#define MODULE_EXT			".so"

#endif


/*************************************************************************/
#define CMD_HASH(x)      (((x)[0]&31)<<5 | ((x)[1]&31))	/* Will gen a hash from a string :) */
#define MAX_CMD_HASH 1024

#define MOD_STOP 1
#define MOD_CONT 0

#define HOSTSERV HS_cmdTable /* using HOSTSERV etc. looks nicer than HS_cmdTable for modules */
#define BOTSERV BS_cmdTable
#define MEMOSERV MS_cmdTable
#define NICKSERV NS_cmdTable
#define CHANSERV CS_cmdTable
#define HELPSERV HE_cmdTable
#define OPERSERV OS_cmdTable
#define IRCD IRCD_cmdTable
#define MODULE_HASH Module_table
#define EVENT EVENT_cmdTable
#define EVENTHOOKS HOOK_cmdTable

/**********************************************************************
 * Module Returns
 **********************************************************************/
 #define MOD_ERR_OK          0
 #define MOD_ERR_MEMORY      1
 #define MOD_ERR_PARAMS      2
 #define MOD_ERR_EXISTS      3
 #define MOD_ERR_NOEXIST     4
 #define MOD_ERR_NOUSER      5
 #define MOD_ERR_NOLOAD      6
 #define MOD_ERR_NOUNLOAD    7
 #define MOD_ERR_SYNTAX      8
 #define MOD_ERR_NODELETE    9
 #define MOD_ERR_UNKNOWN     10
 #define MOD_ERR_FILE_IO     11
 #define MOD_ERR_NOSERVICE   12
 #define MOD_ERR_NO_MOD_NAME 13

/*************************************************************************/
/* Macros to export the Module API functions/variables */
#ifndef _WIN32
#define MDE
#else
#ifndef MODULE_COMPILE
#define MDE __declspec(dllexport)
#else
#define MDE __declspec(dllimport)
#endif
#endif
/*************************************************************************/

typedef enum { CORE,PROTOCOL,THIRD,SUPPORTED,QATESTED,ENCRYPTION } MODType;
typedef enum { MOD_OP_LOAD, MOD_OP_UNLOAD } ModuleOperation;

/*************************************************************************/
/* Structure for information about a *Serv command. */

typedef struct Command_ Command;
typedef struct CommandHash_ CommandHash;
typedef struct Module_ Module;
typedef struct ModuleLang_ ModuleLang;
typedef struct ModuleHash_ ModuleHash;
typedef struct ModuleQueue_ ModuleQueue;
typedef struct Message_ Message;
typedef struct MessageHash_ MessageHash;
typedef struct ModuleCallBack_ ModuleCallBack;
typedef struct EvtMessage_ EvtMessage;
typedef struct EvtMessageHash_ EvtMessageHash;
typedef struct EvtHook_ EvtHook;
typedef struct EvtHookHash_ EvtHookHash;

extern MDE CommandHash *HOSTSERV[MAX_CMD_HASH];
extern MDE CommandHash  *BOTSERV[MAX_CMD_HASH];
extern MDE CommandHash *MEMOSERV[MAX_CMD_HASH];
extern MDE CommandHash *NICKSERV[MAX_CMD_HASH];
extern MDE CommandHash *CHANSERV[MAX_CMD_HASH];
extern MDE CommandHash *HELPSERV[MAX_CMD_HASH];
extern MDE CommandHash *OPERSERV[MAX_CMD_HASH];
extern MDE MessageHash *IRCD[MAX_CMD_HASH];
extern MDE ModuleHash *MODULE_HASH[MAX_CMD_HASH];
extern MDE EvtMessageHash *EVENT[MAX_CMD_HASH];
extern MDE EvtHookHash *EVENTHOOKS[MAX_CMD_HASH];

struct ModuleLang_ {
    int argc;
    char **argv;
};

struct Module_ {
	char *name;
	char *filename;
	void *handle;
	time_t time;
	char *version;
	char *author;

	MODType type;

	void (*nickHelp)(User *u); /* service 1 */
	void (*chanHelp)(User *u); /* 2 */
	void (*memoHelp)(User *u); /* 3 */
	void (*botHelp)(User *u); /* 4 */
	void (*operHelp)(User *u); /* 5 */
	void (*hostHelp)(User *u); /* 6 */
	void (*helpHelp)(User *u); /* 7 */

/*	CommandHash *cmdList[MAX_CMD_HASH]; */
	MessageHash *msgList[MAX_CMD_HASH];
	ModuleLang lang[NUM_LANGS];
};

struct ModuleHash_ {
        char *name;
        Module *m;
        ModuleHash *next;
};

struct ModuleQueue_ {
	Module *m;
	ModuleOperation op;
	User *u;
	
	ModuleQueue *next;
};

struct Command_ {
    char *name;
    int (*routine)(User *u);
    int (*has_priv)(User *u);	/* Returns 1 if user may use command, else 0 */

    /* Regrettably, these are hard-coded to correspond to current privilege
     * levels (v4.0).  Suggestions for better ways to do this are
     * appreciated.
     */
    int helpmsg_all;	/* Displayed to all users; -1 = no message */
    int helpmsg_reg;	/* Displayed to regular users only */
    int helpmsg_oper;	/* Displayed to Services operators only */
    int helpmsg_admin;	/* Displayed to Services admins only */
    int helpmsg_root;	/* Displayed to Services root only */
    char *help_param1;
    char *help_param2;
    char *help_param3;
    char *help_param4;

    /* Module related stuff */
    int core;           /* Can this command be deleted? */
    char *mod_name;	/* Name of the module who owns us, NULL for core's  */
    char *service;	/* Service we provide this command for */
    int (*all_help)(User *u);
    int (*regular_help)(User *u);
    int (*oper_help)(User *u);
    int (*admin_help)(User *u);
    int (*root_help)(User *u);

    Command *next;	/* Next command responsible for the same command */
};

struct CommandHash_ {
        char *name;	/* Name of the command */
        Command *c;	/* Actual command */
        CommandHash *next; /* Next command */
};

struct Message_ {
    char *name;
    int (*func)(char *source, int ac, char **av);
    int core;
    char *mod_name;
    Message *next;
};

struct MessageHash_ {
        char *name;
        Message *m;
        MessageHash *next;
};

struct ModuleCallBack_ {
	char *name;
	char *owner_name;
	time_t when;
	int (*func)(int argc, char *argv[]);
	int argc;
	char **argv;
	ModuleCallBack *next;
};

struct EvtMessage_ {
    char *name;
    int (*func)(char *source, int ac, char **av);
    int core;
    char *mod_name;
    EvtMessage *next;
};

struct EvtMessageHash_ {
        char *name;
        EvtMessage *evm;
        EvtMessageHash *next;
};


struct EvtHook_ {
    int (*func)(int argc, char **argv);
    int core;
	char *name;
    char *mod_name;
    EvtHook *next;
};

struct EvtHookHash_ {
        char *name;
        EvtHook *evh;
        EvtHookHash *next;
};


/*************************************************************************/
/* Module Managment Functions */
MDE Module *createModule(char *filename);        /* Create a new module, using the given name */
int destroyModule(Module *m);		/* Delete the module */
int addModule(Module *m);		/* Add a module to the module hash */
int delModule(Module *m);		/* Remove a module from the module hash */
MDE Module *findModule(char *name);                /* Find a module */
int loadModule(Module *m,User *u);	/* Load the given module into the program */
int encryption_module_init(void); /* Load the encryption module */
int protocol_module_init(void);	/* Load the IRCD Protocol Module up*/
int unloadModule(Module *m, User *u);	/* Unload the given module from the pro */
int prepForUnload(Module *m);		/* Prepare the module for unload */
MDE void moduleAddVersion(const char *version);
MDE void moduleAddAuthor(const char *author);
void modules_init(void);
void modules_delayed_init(void);
void moduleCallBackPrepForUnload(char *mod_name);
MDE void moduleCallBackDeleteEntry(ModuleCallBack * prev);
MDE char *moduleGetLastBuffer(void);
MDE void moduleSetHelpHelp(void (*func) (User * u));
MDE void moduleDisplayHelp(int service, User *u);
MDE void moduleSetHostHelp(void (*func) (User * u));
MDE void moduleSetOperHelp(void (*func) (User * u));
MDE void moduleSetBotHelp(void (*func) (User * u));
MDE void moduleSetMemoHelp(void (*func) (User * u));
MDE void moduleSetChanHelp(void (*func) (User * u));
MDE void moduleSetNickHelp(void (*func) (User * u));
MDE int moduleAddHelp(Command * c, int (*func) (User * u));
MDE int moduleAddRegHelp(Command * c, int (*func) (User * u));
MDE int moduleAddOperHelp(Command * c, int (*func) (User * u));
MDE int moduleAddAdminHelp(Command * c, int (*func) (User * u));
MDE int moduleAddRootHelp(Command * c, int (*func) (User * u));
MDE void moduleSetType(MODType type);
extern MDE Module *mod_current_module;
extern MDE char *mod_current_module_name;
extern MDE char *mod_current_buffer;
extern MDE int mod_current_op;
extern MDE User *mod_current_user;

MDE int moduleGetConfigDirective(Directive *h);
/*************************************************************************/
/*************************************************************************/
/* Command Managment Functions */
MDE Command *createCommand(const char *name,int (*func)(User *u),int (*has_priv)(User *u),int help_all, int help_reg, int help_oper, int help_admin,int help_root);
MDE int destroyCommand(Command *c);					/* destroy a command */
MDE int addCoreCommand(CommandHash *cmdTable[], Command *c);	/* Add a command to a command table */
MDE int moduleAddCommand(CommandHash *cmdTable[], Command *c, int pos);
MDE int addCommand(CommandHash *cmdTable[], Command *c,int pos);
MDE int delCommand(CommandHash *cmdTable[], Command *c,char *mod_name);		/* Del a command from a cmd table */
MDE int moduleDelCommand(CommandHash *cmdTable[],char *name);		/* Del a command from a cmd table */
MDE Command *findCommand(CommandHash *cmdTable[], const char *name);	/* Find a command */

/*************************************************************************/

/* Message Managment Functions */
MDE Message *createMessage(const char *name,int (*func)(char *source, int ac, char **av));
Message *findMessage(MessageHash *msgTable[], const char *name);	/* Find a Message */
MDE int addMessage(MessageHash *msgTable[], Message *m, int pos);		/* Add a Message to a Message table */
MDE int addCoreMessage(MessageHash *msgTable[], Message *m);		/* Add a Message to a Message table */
MDE int moduleAddMessage(Message *m, int pos);
int delMessage(MessageHash *msgTable[], Message *m, char *mod_name);		/* Del a Message from a msg table */
MDE int moduleDelMessage(char *name);
int destroyMessage(Message *m);					/* destroy a Message*/

/*************************************************************************/

MDE EvtMessage *createEventHandler(char *name, int (*func) (char *source, int ac, char **av));
EvtMessage *findEventHandler(EvtMessageHash * msgEvtTable[], const char *name);
int addCoreEventHandler(EvtMessageHash * msgEvtTable[], EvtMessage * evm);
MDE int moduleAddEventHandler(EvtMessage * evm);
MDE int moduleEventDelHandler(char *name);
int delEventHandler(EvtMessageHash * msgEvtTable[], EvtMessage * evm, char *mod_name);
int destroyEventHandler(EvtMessage * evm);
int addEventHandler(EvtMessageHash * msgEvtTable[], EvtMessage * evm);

MDE EvtHook *createEventHook(char *name, int (*func) (int argc, char **argv));
EvtHook *findEventHook(EvtHookHash * HookEvtTable[], const char *name);
int addCoreEventHook(EvtHookHash * HookEvtTable[], EvtHook * evh);
MDE int moduleAddEventHook(EvtHook * evh);
MDE int moduleEventDelHook(const char *name);
int delEventHook(EvtHookHash * HookEvtTable[], EvtHook * evh, char *mod_name);
int destroyEventHook(EvtHook * evh);
extern char *mod_current_evtbuffer;

MDE void moduleInsertLanguage(int langNumber, int ac, char **av);
MDE void moduleNoticeLang(char *source, User *u, int number, ...);
MDE char *moduleGetLangString(User * u, int number);
MDE void moduleDeleteLanguage(int langNumber);

/*************************************************************************/

MDE int moduleAddCallback(char *name,time_t when,int (*func)(int argc, char *argv[]),int argc, char **argv);
MDE void moduleDelCallback(char *name);

MDE char *moduleGetData(ModuleData **md, char *key);			/* Get the value for this key from this struct */
MDE int moduleAddData(ModuleData **md, char *key, char *value);		/* Set the value for this key for this struct */
MDE void moduleDelData(ModuleData **md, char *key);				/* Delete this key/value pair */
MDE void moduleDelAllData(ModuleData **md);					/* Delete all key/value pairs for this module for this struct */
void moduleDelAllDataMod(Module *m);					/* remove all module data from all structs for this module */
int moduleDataDebug(ModuleData **md);					/* Allow for debug output of a moduleData struct */
MDE boolean moduleMinVersion(int major,int minor,int patch,int build);	/* Checks if the current version of anope is before or after a given verison */

/*************************************************************************/
/* Module Queue Operations */
MDE int queueModuleLoad(char *name, User *u);
MDE int queueModuleUnload(char *name, User *u);
MDE void handleModuleOperationQueue(void);

/*************************************************************************/
/* Some IRCD protocol module support functions */

/** Update the protect deatials, could be either protect or admin etc.. */
MDE void updateProtectDetails(char *level_info_protect_word, char *level_info_protectme_word, char *fant_protect_add, char *fant_protect_del, char *level_protect_word, char *protect_set_mode, char *protect_unset_mode);

/************************************************************************/

#endif
/* EOF */
