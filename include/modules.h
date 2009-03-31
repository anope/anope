/* Modular support
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 */

#ifndef MODULES_H
#define MODULES_H

#include <time.h>
#include "services.h"
#include <stdio.h>

/* Cross OS compatibility macros */
#ifdef _WIN32
	typedef HMODULE ano_module_t;

	#define dlopen(file, unused)		LoadLibrary(file)
	E const char *dlerror();
	#define dlsym(file, symbol)	(HMODULE)GetProcAddress(file, symbol)
	#define dlclose(file)		FreeLibrary(file) ? 0 : 1
	#define ano_modclearerr()		SetLastError(0)
	#define MODULE_EXT			".so"
#else
	typedef void *	ano_module_t;

	/* We call dlerror() here because it clears the module error after being
	 * called. This previously read 'errno = 0', but that didn't work on
	 * all POSIX-compliant architectures. This way the error is guaranteed
	 * to be cleared, POSIX-wise. -GD
	 */
	#define ano_modclearerr()		dlerror()
	#define MODULE_EXT			".so"
#endif

/** Possible return types from events.
 */
enum EventReturn
{
	EVENT_STOP,
	EVENT_CONTINUE,
	EVENT_ALLOW
};


/**
 * This #define allows us to call a method in all
 * loaded modules in a readable simple way, e.g.:
 * 'FOREACH_MOD(I_OnConnect,OnConnect(user));'
 */
#define FOREACH_MOD(y,x) do { \
	std::vector<Module*>::iterator safei; \
	for (std::vector<Module*>::iterator _i = ModuleManager::EventHandlers[y].begin(); _i != ModuleManager::EventHandlers[y].end(); ) \
	{ \
		safei = _i; \
		++safei; \
		try \
		{ \
			(*_i)->x ; \
		} \
		catch (CoreException& modexcept) \
		{ \
			alog("Exception caught: %s",modexcept.GetReason()); \
		} \
		_i = safei; \
	} \
} while (0);

/**
 * This define is similar to the one above but returns a result in MOD_RESULT.
 * The first module to return a nonzero result is the value to be accepted,
 * and any modules after are ignored.
 */
#define FOREACH_RESULT(y,x) \
do { \
	std::vector<Module*>::iterator safei; \
	MOD_RESULT = EVENT_CONTINUE; \
	for (std::vector<Module*>::iterator _i = ModuleManager::EventHandlers[y].begin(); _i != ModuleManager::EventHandlers[y].end(); ) \
	{ \
		safei = _i; \
		++safei; \
		try \
		{ \
			EventReturn res = (*_i)->x ; \
			if (res != EVENT_CONTINUE) { \
				MOD_RESULT = res; \
				break; \
			} \
		} \
		catch (CoreException& modexcept) \
		{ \
			alog("Exception caught: %s",modexcept.GetReason()); \
		} \
		_i = safei; \
	} \
} while(0);



/** Priority types which can be returned from Module::Prioritize()
 */
enum Priority { PRIORITY_FIRST, PRIORITY_DONTCARE, PRIORITY_LAST, PRIORITY_BEFORE, PRIORITY_AFTER };



/*************************************************************************/
#define CMD_HASH(x)	  (((x)[0]&31)<<5 | ((x)[1]&31))	/* Will gen a hash from a string :) */
#define MAX_CMD_HASH 1024

/** The return value from commands.
 */
enum CommandReturn
{
	MOD_CONT,
	MOD_STOP
};

#define HOSTSERV HS_cmdTable /* using HOSTSERV etc. looks nicer than HS_cmdTable for modules */
#define BOTSERV BS_cmdTable
#define MEMOSERV MS_cmdTable
#define NICKSERV NS_cmdTable
#define CHANSERV CS_cmdTable
#define OPERSERV OS_cmdTable
#define IRCD IRCD_cmdTable
#define MODULE_HASH Module_table
#define EVENT EVENT_cmdTable
#define EVENTHOOKS HOOK_cmdTable

/**********************************************************************
 * Module Returns
 **********************************************************************/
#define MOD_ERR_OK		  0
#define MOD_ERR_MEMORY	  1
#define MOD_ERR_PARAMS	  2
#define MOD_ERR_EXISTS	  3
#define MOD_ERR_NOEXIST	 4
#define MOD_ERR_NOUSER	  5
#define MOD_ERR_NOLOAD	  6
#define MOD_ERR_NOUNLOAD	7
#define MOD_ERR_SYNTAX	  8
#define MOD_ERR_NODELETE	9
#define MOD_ERR_UNKNOWN	 10
#define MOD_ERR_FILE_IO	 11
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

#if !defined(_WIN32)
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
#else
	const char *ano_moderr();
#endif

typedef enum { CORE,PROTOCOL,THIRD,SUPPORTED,QATESTED,ENCRYPTION } MODType;
typedef enum { MOD_OP_LOAD, MOD_OP_UNLOAD } ModuleOperation;

/*************************************************************************/
/* Structure for information about a *Serv command. */

struct CommandHash;
typedef struct ModuleLang_ ModuleLang;
typedef struct ModuleHash_ ModuleHash;
typedef struct Message_ Message;
typedef struct MessageHash_ MessageHash;
typedef struct ModuleCallBack_ ModuleCallBack;

/*************************************************************************/




extern MDE CommandHash *HOSTSERV[MAX_CMD_HASH];
extern MDE CommandHash  *BOTSERV[MAX_CMD_HASH];
extern MDE CommandHash *MEMOSERV[MAX_CMD_HASH];
extern MDE CommandHash *NICKSERV[MAX_CMD_HASH];
extern MDE CommandHash *CHANSERV[MAX_CMD_HASH];
extern MDE CommandHash *OPERSERV[MAX_CMD_HASH];
extern MDE MessageHash *IRCD[MAX_CMD_HASH];
extern MDE ModuleHash *MODULE_HASH[MAX_CMD_HASH];

struct ModuleLang_ {
	int argc;
	char **argv;
};

enum CommandFlags
{
	CFLAG_ALLOW_UNREGISTERED = 1
};

/** Every services command is a class, inheriting from Command.
 */
class CoreExport Command
{
	int flags;
 public:
	size_t MaxParams;
	size_t MinParams;
	std::string name;

	/** Create a new command.
	 * @param min_params The minimum number of parameters the parser will require to execute this command
	 * @param max_params The maximum number of parameters the parser will create, after max_params, all will be combined into the last argument.
	 * NOTE: If max_params is not set (default), there is no limit to the max number of params.
	 */
	Command(const std::string &sname, size_t min_params, size_t max_params = 0);

	virtual ~Command();

	/** Execute this command.
	 * @param u The user executing the command.
	 */
	virtual CommandReturn Execute(User *u, std::vector<std::string> &);

	/** Requested when the user is requesting help on this command. Help on this command should be sent to the user.
	 * @param u The user requesting help
	 * @param subcommand The subcommand the user is requesting help on, or an empty string. (e.g. /ns help set foo bar lol gives a subcommand of "FOO BAR LOL")
	 * @return true if help was provided to the user, false otherwise.
	 */
	virtual bool OnHelp(User *u, const std::string &subcommand);

	/** Requested when the user provides bad syntax to this command (not enough params, etc).
	 * @param u The user executing the command.
	 */
	virtual void OnSyntaxError(User *u);

	/** Set a certain flag on this command.
	 * @param flag The CommandFlag to set on this command.
	 */
	void SetFlag(CommandFlags flag);

	/** Remove a certain flag from this command.
	 * @param flag The CommandFlag to unset.
	 */
	void UnsetFlag(CommandFlags flag);

	/** Check whether a certain flag is set on this command.
	 * @param flag The CommandFlag to check.
	 * @return bool True if the flag is set, false else.
	 */
	bool HasFlag(CommandFlags flag) const;

	char *help_param1;
	char *help_param2;
	char *help_param3;
	char *help_param4;

	/* Module related stuff */
	int core;		   /* Can this command be deleted? */
	char *mod_name;	/* Name of the module who owns us, NULL for core's  */
	char *service;	/* Service we provide this command for */
	int (*all_help)(User *u);
	int (*regular_help)(User *u);
	int (*oper_help)(User *u);
	int (*admin_help)(User *u);
	int (*root_help)(User *u);

	Command *next;	/* Next command responsible for the same command */
};
/** Every module in Anope is actually a class.
 */
class CoreExport Module
{
 private:
	bool permanent;
 public:
	/** The module name (e.g. os_modload)
	 */
	std::string name;

	/** The temporary path/filename
	 */
	std::string filename;

	ano_module_t handle;
	time_t created;
	std::string version;
	std::string author;

	MODType type;

	void (*nickHelp)(User *u); /* service 1 */
	void (*chanHelp)(User *u); /* 2 */
	void (*memoHelp)(User *u); /* 3 */
	void (*botHelp)(User *u); /* 4 */
	void (*operHelp)(User *u); /* 5 */
	void (*hostHelp)(User *u); /* 6 */

	MessageHash *msgList[MAX_CMD_HASH];
	ModuleLang lang[NUM_LANGS];

	/** Creates and initialises a new module.
	 * @param loadernick The nickname of the user loading the module.
	 */
	Module(const std::string &modname, const std::string &loadernick);

	/** Destroys a module, freeing resources it has allocated.
	 */
	virtual ~Module();

	/** Sets a given type (CORE,PROTOCOL,3RD etc) on a module.
	 * @param type The type to set the module as.
	 */
	void SetType(MODType type);

	/** Toggles the permanent flag on a module. If a module is permanent,
	 * then it may not be unloaded.
	 *
	 * Naturally, this setting should be used sparingly!
	 *
	 * @param state True if this module should be permanent, false else.
	 */
	void SetPermanent(bool state);

	/** Retrieves whether or not a given module is permanent.
	 * @return true if the module is permanent, false else.
	 */
	bool GetPermanent();

	/** Set the modules version info.
	 * @param version the version of the module
	 */
	void SetVersion(const std::string &version);

	/** Set the modules author info
	 * @param author the author of the module
	 */
	void SetAuthor(const std::string &author);

	/**
	 * Add output to nickserv help.
	 * when doing a /msg nickserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
	 * @param func a pointer to the function which will display the code
	 **/
	void SetNickHelp(void (*func)(User *));

	/**
	 * Add output to chanserv help.
	 * when doing a /msg chanserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
	 * @param func a pointer to the function which will display the code
	 **/
	void SetChanHelp(void (*func)(User *));

	/**
	 * Add output to memoserv help.
	 * when doing a /msg memoserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
	 * @param func a pointer to the function which will display the code
	 **/
	void SetMemoHelp(void (*func)(User *));

	/**
	 * Add output to botserv help.
	 * when doing a /msg botserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
	 * @param func a pointer to the function which will display the code
	 **/
	void SetBotHelp(void (*func)(User *));

	/**
	 * Add output to operserv help.
	 * when doing a /msg operserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
	 * @param func a pointer to the function which will display the code
	 **/
	void SetOperHelp(void (*func)(User *));

	/**
	 * Add output to hostserv help.
	 * when doing a /msg hostserv help, your function will be called to allow it to send out a notice() with the code you wish to dispaly
	 * @param func a pointer to the function which will display the code
	 **/
	void SetHostHelp(void (*func)(User *));

	/**
	 * Allow a module to add a set of language strings to anope
	 * @param langNumber the language number for the strings
	 * @param ac The language count for the strings
	 * @param av The language sring list.
	 **/
	void InsertLanguage(int langNumber, int ac, const char **av);

	/**
	 * Delete a language from a module
	 * @param langNumber the language Number to delete
	 **/
	void DeleteLanguage(int langNumber);

	/**
	 * Get the text of the given lanugage string in the corrent language, or
	 * in english.
	 * @param u The user to send the message to
	 * @param number The message number
	 **/
	const char *GetLangString(User *u, int number);

	/**
	 * Send a notice to the user in the correct language, or english.
	 * @param source Who sends the notice
	 * @param u The user to send the message to
	 * @param number The message number
	 * @param ... The argument list
	 **/
	void NoticeLang(char *source, User * u, int number, ...);

	/**
	 * Add a module provided command to the given service.
	 * e.g. AddCommand(NICKSERV,c,MOD_HEAD);
	 * @param cmdTable the services to add the command to
	 * @param c the command to add
	 * @param pos the position to add to, MOD_HEAD, MOD_TAIL, MOD_UNIQUE
	 * @see createCommand
	 * @return MOD_ERR_OK on successfully adding the command
	 */
	int AddCommand(CommandHash *cmdTable[], Command * c, int pos);

	/**
	 * Delete a command from the service given.
	 * @param cmdTable the cmdTable for the services to remove the command from
	 * @param name the name of the command to delete from the service
	 * @return returns MOD_ERR_OK on success
	 */
	int DelCommand(CommandHash * cmdTable[], const char *name);

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
	int AddCallback(const char *name, time_t when, int (*func) (int argc, char *argv[]), int argc, char **argv);

	/**
	 * Allow modules to delete a timed callback by name.
	 * @param name the name of the callback they wish to delete
	 **/
	void DelCallback(const char *name);



	/** Called when the ircd notifies that a user has been kicked from a channel.
	 * @param c The channel the user has been kicked from.
	 * @param target The user that has been kicked.
	 * @param kickmsg The reason for the kick.
	 * NOTE: We may want to add a second User arg for sender in the future.
	 */
	virtual void OnUserKicked(Channel *c, User *target, const std::string &kickmsg) { }

	/** Called when Services' configuration has been loaded.
	 * @param startup True if Services is starting for the first time, false otherwise.
	 */
	virtual void OnReload(bool startup) {}

	/** Called before a bot is assigned to a channel.
	 * @param sender The user assigning the bot
	 * @param ci The channel the bot is to be assigned to.
	 * @param bi The bot being assigned.
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to deny the assign.
	 */
	virtual EventReturn OnBotAssign(User *sender, ChannelInfo *ci, BotInfo *bi) { return EVENT_CONTINUE; }

	/** Called before a bot is unassigned from a channel.
	 * @param sender The user unassigning the bot
	 * @param ci The channel the bot is being removed from
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to deny the unassign.
	 */
	virtual EventReturn OnBotUnAssign(User *sender, ChannelInfo *ci) { return EVENT_CONTINUE; }

	/** Called when a new user connects to the network.
	 * @param u The connecting user.
	 */
	virtual void OnUserConnect(User *u) { }

	/** Called when a new server connects to the network.
	 * @param s The server that has connected to the network
	 */
	virtual void OnServerConnect(Server *s) { }

	/** Called when anope saves databases.
	 * NOTE: This event is deprecated pending new database handling.
	 * XXX.
	 */
	virtual void OnSaveDatabase() MARK_DEPRECATED { }

	/** Called when anope backs up databases.
	 * NOTE: This event is deprecated pending new database handling.
	 */
	virtual void OnBackupDatabase() MARK_DEPRECATED { }
};


/** Implementation-specific flags which may be set in Module::Implements()
 */
enum Implementation
{
	I_BEGIN,
		I_OnUserKicked, I_OnReload, I_OnBotAssign, I_OnBotUnAssign, I_OnUserConnect, I_OnServerConnect,
		I_OnSaveDatabase, I_OnBackupDatabase,
	I_END
};


/** Used to manage modules.
 */
class CoreExport ModuleManager
{
 public:
	/** Event handler hooks.
	 * This needs to be public to be used by FOREACH_MOD and friends.
	 */
	static std::vector<Module *> EventHandlers[I_END];

	/** Load up a list of modules.
	 * @param total_modules The number of modules to load
	 * @param module_list The list of modules to load
	 **/
	static void LoadModuleList(int total_modules, char **module_list);

	/** Loads a given module.
	 * @param m the module to load
	 * @param u the user who loaded it, NULL for auto-load
	 * @return MOD_ERR_OK on success, anything else on fail
	 */
	static int LoadModule(const std::string &modname, User * u);

	/** Unload the given module.
	 * @param m the module to unload
	 * @param u the user who unloaded it
	 * @return MOD_ERR_OK on success, anything else on fail
	 */
	static int UnloadModule(Module *m, User * u);

	/** Run all pending module timer callbacks.
	 */
	static void RunCallbacks();







	/** Change the priority of one event in a module.
	 * Each module event has a list of modules which are attached to that event type. If you wish to be called before or after other specific modules, you may use this
	 * method (usually within void Module::Prioritize()) to set your events priority. You may use this call in other methods too, however, this is not supported behaviour
	 * for a module.
	 * @param mod The module to change the priority of
	 * @param i The event to change the priority of
	 * @param s The state you wish to use for this event. Use one of
	 * PRIO_FIRST to set the event to be first called, PRIO_LAST to set it to be the last called, or PRIO_BEFORE and PRIO_AFTER
	 * to set it to be before or after one or more other modules.
	 * @param modules If PRIO_BEFORE or PRIO_AFTER is set in parameter 's', then this contains a list of one or more modules your module must be
	 * placed before or after. Your module will be placed before the highest priority module in this list for PRIO_BEFORE, or after the lowest
	 * priority module in this list for PRIO_AFTER.
	 * @param sz The number of modules being passed for PRIO_BEFORE and PRIO_AFTER. Defaults to 1, as most of the time you will only want to prioritize your module
	 * to be before or after one other module.
	 */
	static bool SetPriority(Module* mod, Implementation i, Priority s, Module** modules = NULL, size_t sz = 1);

	/** Change the priority of all events in a module.
	 * @param mod The module to set the priority of
	 * @param s The priority of all events in the module.
	 * Note that with this method, it is not possible to effectively use PRIO_BEFORE or PRIO_AFTER, you should use the more fine tuned
	 * SetPriority method for this, where you may specify other modules to be prioritized against.
	 */
	static bool SetPriority(Module* mod, Priority s);

	/** Attach an event to a module.
	 * You may later detatch the event with ModuleManager::Detach(). If your module is unloaded, all events are automatically detatched.
	 * @param i Event type to attach
	 * @param mod Module to attach event to
	 * @return True if the event was attached
	 */
	static bool Attach(Implementation i, Module* mod);

	/** Detatch an event from a module.
	 * This is not required when your module unloads, as the core will automatically detatch your module from all events it is attached to.
	 * @param i Event type to detach
	 * @param mod Module to detach event from
	 * @param Detach true if the event was detached
	 */
	static bool Detach(Implementation i, Module* mod);

	/** Detach all events from a module (used on unload)
	 * @param mod Module to detach from
	 */
	static void DetachAll(Module* mod);

	/** Attach an array of events to a module
	 * @param i Event types (array) to attach
	 * @param mod Module to attach events to
	 */
	static void Attach(Implementation* i, Module* mod, size_t sz);
private:
	/** Call the module_delete function to safely delete the module
	 * @param m the module to delete
	 */
	static void DeleteModule(Module *m);
};



struct ModuleHash_ {
		char *name;
		Module *m;
		ModuleHash *next;
};

struct CommandHash {
		char *name;	/* Name of the command */
		Command *c;	/* Actual command */
		CommandHash *next; /* Next command */
};

struct Message_ {
	char *name;
	int (*func)(const char *source, int ac, const char **av);
	int core;
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
MDE Module *findModule(const char *name);				/* Find a module */

int encryption_module_init(); /* Load the encryption module */
int protocol_module_init();	/* Load the IRCD Protocol Module up*/
void moduleCallBackPrepForUnload(const char *mod_name);
MDE void moduleCallBackDeleteEntry(ModuleCallBack * prev);
MDE char *moduleGetLastBuffer();
MDE void moduleDisplayHelp(int service, User *u);
MDE int moduleAddHelp(Command * c, int (*func) (User * u));
MDE int moduleAddRegHelp(Command * c, int (*func) (User * u));
MDE int moduleAddOperHelp(Command * c, int (*func) (User * u));
MDE int moduleAddAdminHelp(Command * c, int (*func) (User * u));
MDE int moduleAddRootHelp(Command * c, int (*func) (User * u));
extern MDE char *mod_current_buffer;

/*************************************************************************/
/*************************************************************************/
/* Command Managment Functions */
Command *findCommand(CommandHash *cmdTable[], const char *name);	/* Find a command */

/*************************************************************************/

/* Message Managment Functions */
MDE Message *createMessage(const char *name,int (*func)(const char *source, int ac, const char **av));
Message *findMessage(MessageHash *msgTable[], const char *name);	/* Find a Message */
MDE int addMessage(MessageHash *msgTable[], Message *m, int pos);		/* Add a Message to a Message table */
MDE int addCoreMessage(MessageHash *msgTable[], Message *m);		/* Add a Message to a Message table */
int delMessage(MessageHash *msgTable[], Message *m);		/* Del a Message from a msg table */
int destroyMessage(Message *m);					/* destroy a Message*/

/*************************************************************************/

MDE bool moduleMinVersion(int major,int minor,int patch,int build);	/* Checks if the current version of anope is before or after a given verison */

/*************************************************************************/
/* Some IRCD protocol module support functions */

/** Update the protect deatials, could be either protect or admin etc.. */
MDE void updateProtectDetails(const char *level_info_protect_word, const char *level_info_protectme_word, const char *fant_protect_add, const char *fant_protect_del, const char *level_protect_word, const char *protect_set_mode, const char *protect_unset_mode);
MDE void updateOwnerDetails(const char *fant_owner_add, const char *fant_owner_del, const char *owner_set_mode, const char *owner_del_mode);

/************************************************************************/

#endif
/* EOF */
