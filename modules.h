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
 * $Id: modules.h,v 1.16 2004/03/11 16:50:01 rob Exp $
 */

#ifndef MODULES_H
#define MODULES_H

#include <time.h>
#include "services.h"
#include <stdio.h>

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

/**********************************************************************
 * Module Returns
 **********************************************************************/
 #define MOD_ERR_OK		0
 #define MOD_ERR_MEMORY		1
 #define MOD_ERR_PARAMS		2
 #define MOD_ERR_EXISTS		3
 #define MOD_ERR_NOEXIST	4
 #define MOD_ERR_NOUSER		5
 #define MOD_ERR_NOLOAD         6
 #define MOD_ERR_NOUNLOAD       7
 #define MOD_ERR_SYNTAX		8
 #define MOD_ERR_NODELETE	9
 #define MOD_ERR_UNKNOWN	10
 #define MOD_ERR_FILE_IO        11
 #define MOD_ERR_NOSERVICE 12

 /*************************************************************************/

/* Structure for information about a *Serv command. */

typedef struct Command_ Command;
typedef struct CommandHash_ CommandHash;
typedef struct Module_ Module;
typedef struct ModuleHash_ ModuleHash;
typedef struct Message_ Message;
typedef struct MessageHash_ MessageHash;
typedef struct ModuleCallBack_ ModuleCallBack;


extern CommandHash *HOSTSERV[MAX_CMD_HASH];
extern CommandHash  *BOTSERV[MAX_CMD_HASH];
extern CommandHash *MEMOSERV[MAX_CMD_HASH];
extern CommandHash *NICKSERV[MAX_CMD_HASH];
extern CommandHash *CHANSERV[MAX_CMD_HASH];
extern CommandHash *HELPSERV[MAX_CMD_HASH];
extern CommandHash *OPERSERV[MAX_CMD_HASH];
extern MessageHash *IRCD[MAX_CMD_HASH];

enum MODULE_DATA_TYPE { MD_NICK_CORE = 1, MD_NICK_ALIAS = 2, MD_NICK_MEMO = 3, MD_CHAN_MEMO =4, MD_CHAN_INFO = 5};

struct Module_ {
	char *name;
	char *filename;
	void *handle;
	time_t time;
	char *version;
	char *author;

	void (*nickHelp)(User *u); /* service 1 */
	void (*chanHelp)(User *u); /* 2 */
	void (*memoHelp)(User *u); /* 3 */
	void (*botHelp)(User *u); /* 4 */
	void (*operHelp)(User *u); /* 5 */
	void (*hostHelp)(User *u); /* 6 */
	void (*helpHelp)(User *u); /* 7 */

//	CommandHash *cmdList[MAX_CMD_HASH];
	MessageHash *msgList[MAX_CMD_HASH];
};

struct ModuleHash_ {
        char *name;
        Module *m;
        ModuleHash *next;
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

/*************************************************************************/
/* Module Managment Functions */
Module *createModule(char *filename);	/* Create a new module, using the given name */
int destroyModule(Module *m);		/* Delete the module */
int addModule(Module *m);		/* Add a module to the module hash */
int delModule(Module *m);		/* Remove a module from the module hash */
Module *findModule(char *name);		/* Find a module */
int loadModule(Module *m,User *u);	/* Load the given module into the program */
int unloadModule(Module *m, User *u);	/* Unload the given module from the pro */
int prepForUnload(Module *m);		/* Prepare the module for unload */
void moduleAddVersion(char *version);
void moduleAddAuthor(char *author);
void modules_init(void);
void modules_delayed_init(void);
void moduleCallBackPrepForUnload(char *mod_name);
void moduleCallBackDeleteEntry(ModuleCallBack * prev);
char *moduleGetLastBuffer(void);
void moduleSetHelpHelp(void (*func) (User * u));
void moduleDisplayHelp(int service, User *u);
void moduleSetHostHelp(void (*func) (User * u));
void moduleSetOperHelp(void (*func) (User * u));
void moduleSetBotHelp(void (*func) (User * u));
void moduleSetMemoHelp(void (*func) (User * u));
void moduleSetChanHelp(void (*func) (User * u));
void moduleSetNickHelp(void (*func) (User * u));
int moduleAddHelp(Command * c, int (*func) (User * u));
int moduleAddRegHelp(Command * c, int (*func) (User * u));
int moduleAddOperHelp(Command * c, int (*func) (User * u));
int moduleAddAdminHelp(Command * c, int (*func) (User * u));
int moduleAddRootHelp(Command * c, int (*func) (User * u));
/*************************************************************************/
/*************************************************************************/
/* Command Managment Functions */
Command *createCommand(const char *name,int (*func)(User *u),int (*has_priv)(User *u),int help_all, int help_reg, int help_oper, int help_admin,int help_root);
int destroyCommand(Command *c);					/* destroy a command */
int addCoreCommand(CommandHash *cmdTable[], Command *c);	/* Add a command to a command table */
int moduleAddCommand(CommandHash *cmdTable[], Command *c, int pos);
int addCommand(CommandHash *cmdTable[], Command *c,int pos);
int delCommand(CommandHash *cmdTable[], Command *c,char *mod_name);		/* Del a command from a cmd table */
int moduleDelCommand(CommandHash *cmdTable[],char *name);		/* Del a command from a cmd table */
Command *findCommand(CommandHash *cmdTable[], const char *name);	/* Find a command */
/*************************************************************************/
/*************************************************************************/
/* Message Managment Functions */
Message *createMessage(char *name,int (*func)(char *source, int ac, char **av));
Message *findMessage(MessageHash *msgTable[], const char *name);	/* Find a Message */
int addMessage(MessageHash *msgTable[], Message *m, int pos);		/* Add a Message to a Message table */
int addCoreMessage(MessageHash *msgTable[], Message *m);		/* Add a Message to a Message table */
int moduleAddMessage(Message *m, int pos);
int delMessage(MessageHash *msgTable[], Message *m, char *mod_name);		/* Del a Message from a msg table */
int moduleDelMessage(char *name);
int destroyMessage(Message *m);					/* destroy a Message*/
Message *findMessage(MessageHash *msgTable[], const char *name);
/*************************************************************************/
int moduleAddCallback(char *name,time_t when,int (*func)(int argc, char *argv[]),int argc, char **argv);
void moduleDelCallback(char *name);
void moduleCallBackRun(void);

char *moduleGetData(ModuleData *md[], char *key);			/* Get the value for this key from this struct */
int moduleAddDataValue(ModuleData * md[], char *key, char *value,int persistant); /* Store the value in the struct */
int moduleAddPersistantData(ModuleData * md[], char *key, char *value); /* Set the value for this key for this struct to be saved*/
int moduleAddData(ModuleData *md[], char *key, char *value);		/* Set the value for this key for this struct */
void moduleDelData(ModuleData *md[], char *key);				/* Delete this key/value pair */
void moduleDelAllData(ModuleData *md[]);					/* Delete all key/value pairs for this module for this struct */
void moduleCleanStruct(ModuleData * moduleData[]);			/* Clean a moduleData hash */
void moduleDelAllDataMod(Module *m);					/* remove all module data from all structs for this module */
void moduleLoadAllData(Module *m);						/* Load any persistant module data settings */
void moduleSaveAllData(Module *m);						/* Save any persistant module data settings */
/*************************************************************************/

#endif
/* EOF */
