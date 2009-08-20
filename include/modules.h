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
#include "timers.h"
#include "hashcomp.h"

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
	CFLAG_ALLOW_UNREGISTERED = 1,
	CFLAG_ALLOW_FORBIDDEN = 2,
	CFLAG_ALLOW_SUSPENDED = 4,
	CFLAG_ALLOW_UNREGISTEREDCHANNEL = 8,
	CFLAG_STRIP_CHANNEL = 16,
	CFLAG_DISABLE_FANTASY = 32
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
	std::string permission;

	/** Create a new command.
	 * @param min_params The minimum number of parameters the parser will require to execute this command
	 * @param max_params The maximum number of parameters the parser will create, after max_params, all will be combined into the last argument.
	 * NOTE: If max_params is not set (default), there is no limit to the max number of params.
	 */
	Command(const std::string &sname, size_t min_params, size_t max_params = 0, const std::string &spermission = "");

	virtual ~Command();

	/** Execute this command.
	 * @param u The user executing the command.
	 */
	virtual CommandReturn Execute(User *u, std::vector<ci::string> &);

	/** Requested when the user is requesting help on this command. Help on this command should be sent to the user.
	 * @param u The user requesting help
	 * @param subcommand The subcommand the user is requesting help on, or an empty string. (e.g. /ns help set foo bar lol gives a subcommand of "FOO BAR LOL")
	 * @return true if help was provided to the user, false otherwise.
	 */
	virtual bool OnHelp(User *u, const ci::string &subcommand);

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

	/** Set which command permission (e.g. chanserv/forbid) is required for this command.
	 * @param reststr The permission required to successfully execute this command
	 */
	void SetPermission(const std::string &reststr);

	char *help_param1;
	char *help_param2;
	char *help_param3;
	char *help_param4;

	/* Module related stuff */
	int core;		   /* Can this command be deleted? */
	char *mod_name;	/* Name of the module who owns us, NULL for core's  */
	char *service;	/* Service we provide this command for */
	Command *next;
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

	/** Timers used in this module
	 */
	std::list<Timer *> CallBacks;

	ano_module_t handle;
	time_t created;
	std::string version;
	std::string author;

	MODType type;

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

	/** Add output to NickServ Help.
	 * When doing /msg NickServ HELP, this function will be calloed to allow it to send out
	 * a notice witht he code you wish to display
	 * @Param u The user executing the command
	 */
	virtual void NickServHelp(User *u) { }

	/** Add output to ChanServ Help.
	 * When doing /msg ChanServ HELP, this function will be calloed to allow it to send out
	 * a notice witht he code you wish to display
	 * @Param u The user executing the command
	 */
	virtual void ChanServHelp(User *u) { }

	/** Add output to MemoServ Help.
	 * When doing /msg MemoServ HELP, this function will be calloed to allow it to send out
	 * a notice witht he code you wish to display
	 * @Param u The user executing the command
	 */
	virtual void MemoServHelp(User *u) { }

	/** Add output to BotServ Help.
	 * When doing /msg BotServ HELP, this function will be calloed to allow it to send out
	 * a notice witht he code you wish to display
	 * @Param u The user executing the command
	 */
	virtual void BotServHelp(User *u) { }

	/** Add output to OperServ Help.
	 * When doing /msg OperServ HELP, this function will be calloed to allow it to send out
	 * a notice witht he code you wish to display
	 * @Param u The user executing the command
	 */
	virtual void OperServHelp(User *u) { }

	/** Add output to HostServ Help.
	 * When doing /msg HostServ HELP, this function will be calloed to allow it to send out
	 * a notice witht he code you wish to display
	 * @Param u The user executing the command
	 */
	virtual void HostServHelp(User *u) { }

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
	 * Adds a timer to the current module
	 * The timer handling will take care of everything for this timer, this is only here
	 * so we have a list of timers to destroy when this module is unloaded
	 * @param t A timer derived class
	 */
	void AddCallBack(Timer *t);

	/**
	 * Deletes a timer for the current module
	 * @param t The timer
	 */
	bool DelCallBack(Timer *t);

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

	/** Called after a user changed the nick
	 * @param u The user.
	 * @param oldnick the old nick of the user
	 */
	virtual void OnUserNickChange(User *u, const char *oldnick) { }

	/** Called before a command is due to be executed.
	 * @param u The user executing the command
	 * @param service The service the command is associated with
	 * @param command The command the user is executing
	 * @param params The parameters the user is sending
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnPreCommand(User *u, const std::string &service, const ci::string &command, const std::vector<ci::string> &params) { return EVENT_CONTINUE; }

	/** Called after a command has been executed.
	 * @param u The user executing the command
	 * @param service The service the command is associated with
	 * @param command The command the user executed
	 * @param params The parameters the user sent
	 */
	virtual void OnPostCommand(User *u, const std::string &service, const ci::string &command, const std::vector<ci::string> &params) { }

	/** Called when anope saves databases.
	 * NOTE: This event is deprecated pending new database handling.
	 * XXX.
	 */
	virtual void OnSaveDatabase() MARK_DEPRECATED { }

	/** Called when anope backs up databases.
	 * NOTE: This event is deprecated pending new database handling.
	 */
	virtual void OnBackupDatabase() MARK_DEPRECATED { }

	/** Called when anope needs to check passwords against encryption
	 *  see src/encrypt.c for detailed informations
	 */
	virtual EventReturn OnEncrypt(const char *src,int len,char *dest,int size) { return EVENT_CONTINUE; }
	virtual EventReturn OnEncryptInPlace(char *buf, int size) { return EVENT_CONTINUE; }
	virtual EventReturn OnEncryptCheckLen(int passlen, int bufsize) { return EVENT_CONTINUE; }
	virtual EventReturn OnDecrypt(const char *src, char *dest, int size) { return EVENT_CONTINUE; }
	virtual EventReturn OnCheckPassword(const char *plaintext, const char *password) { return EVENT_CONTINUE; }

	/** Called on fantasy command
	 * @param command The command
	 * @param u The user using the command
	 * @param ci The channel it's being used in
	 * @param params The params
	 */
	virtual void OnBotFantasy(char *command, User *u, ChannelInfo *ci, char *params) { }

	/** Called on fantasy command without access
	 * @param command The command
	 * @param u The user using the command
	 * @param ci The channel it's being used in
	 * @param params The params
	 */
	virtual void OnBotNoFantasyAccess(const char *command, User *u, ChannelInfo *ci, const char *params) { }

	/** Called after a bot joins a channel
	 * @param ci The channael
	 * @param bi The bot
	 */
	virtual void OnBotJoin(ChannelInfo *ci, BotInfo *bi) { }

	/** Called when a bot places a ban
	 * @param u User being banned
	 * @param ci Channel the ban is placed on
	 * @param mask The mask being banned
	 */
	virtual void OnBotBan(User *u, ChannelInfo *ci, const char *mask) { }

	/** Called when a bot kicks a user
	 * @param u The user being kicked
	 * @param ci The channel
	 * @param reason The reason
	 */
	virtual void OnBotKick(User *u, ChannelInfo *ci, const char *reason) { }

	/** Called before a user parts a channel
	 * @param u The user
	 * @param c The channel
	 */
	virtual void OnPrePartChannel(User *u, Channel *c) {}

	/** Called when a user parts a channel
	 * @param u The user
	 * @param c The channel
	 */
	virtual void OnPartChannel(User *u, Channel *c) { }

	/** Called before a user joins a channel
	 * @param u The user
	 * @param channel The channel
	 */
	virtual void OnPreJoinChannel(User *u, const char *channel) { }

	/** Called when a user joins a channel
	 * @param u The user
	 * @param channel The channel
	 */
	virtual void OnJoinChannel(User *u, Channel *c) { }

	/** Called when a new topic is set
	 * @param c The channel
	 * @param topic The new topic
	 */
	virtual void OnTopicUpdated(Channel *c, const char *topic) { }

	/** Called when a channel expires
	 * @param chname The channel name
	 */
	virtual void OnChanExpire(const char *chname) { }

	/** Called before anope connects to its uplink
	 */
	virtual void OnPreServerConnect() { }

	/** Called when anope connects to its uplink
	 */
	virtual void OnServerConnect() { }

	/** Called before the database expire routines are called
	* Note: Code that is in seperate expiry routines should just be done
	* when we save the DB, theres no need to have both
	*/
	virtual void OnPreDatabaseExpire() MARK_DEPRECATED { }

	/** Called when the database expire routines are called
	 */
	virtual void OnDatabaseExpire() MARK_DEPRECATED { }

	/** Called before services restart
	*/
	virtual void OnPreRestart() { }

	/** Called when services restart
	*/
	virtual void OnRestart() { }

	/** Called before services shutdown
	 */
	virtual void OnPreShutdown() { }

	/** Called when services shutdown
	 */
	virtual void OnShutdown() { }

	/** Called on signal
	 * @param msg The quitmsg
	 */
	virtual void OnSignal(const char *msg) { }

	/** Called when a nick drops
	 * @param nick The nick
	 */
	virtual void OnNickExpire(const char *nick) { }

	/** Called when defcon level changes
	 * @param level The level
	 */
	virtual void OnDefconLevel(const char *level) { }

	/** Called when a server quits
	 * @param server The server
	 */
	virtual void OnServerQuit(Server *server) { }

	/** Called when a user disconnects
	 * @param nick The name of the user
	 */
	virtual void OnUserLogoff(User *u) { }

	/** Called when a new bot is made
	 * @param bi The bot
	 */
	virtual void OnBotCreate(BotInfo *bi) { }

	/** Called when a bot is changed
	 * @param bi The bot
	 */
	virtual void OnBotChange(BotInfo *bi) { }

	/** Called when a bot is deleted
	 * @param bi The bot
	 */
	virtual void OnBotDelete(BotInfo *bi) { }

	/** Called when access is deleted from a channel
	 * @param ci The channel
	 * @param u The user who removed the access
	 * @param nick The name of the user whos access was removed
	 */
	virtual void OnAccessDel(ChannelInfo *ci, User *u, const char *nick) { }

	/** Called when access is changed
	 * @param ci The channel
	 * @param u The user who changed the access
	 * @param nick The nick whos access was changed
	 * @param level The level of the new access
	 */
	virtual void OnAccessChange(ChannelInfo *ci, User *u, const char *nick, int level) { }

	/** Called when access is added
	 * @param ci The channel
	 * @param u The user who added the access
	 * @param nick The nick who was added to access
	 * @param level The level they were added at
	 */
	virtual void OnAccessAdd(ChannelInfo *ci, User *u, const char *nick, int level) { }

	/** Called when the access list is cleared
	 * @param ci The channel
	 * @param u The user who cleared the access
	 */
	virtual void OnAccessClear(ChannelInfo *ci, User *u) { }

	/** Called when a channel is dropped
	 * @param chname The channel name
	 */
	virtual void OnChanDrop(const char *chname) { }

	/** Called when a channel is forbidden
	 * @param ci The channel
	 */
	virtual void OnChanForbidden(ChannelInfo *ci) { }

	/** Called when a channel is registered
	 * @param ci The channel
	 */
	virtual void OnChanRegistered(ChannelInfo *ci) { }

	/** Called when a channel is suspended
	 * @param ci The channel
	 */
	virtual void OnChanSuspend(ChannelInfo *ci) { }

	/** Called when a channel is unsuspended
	 * @param ci The channel
	 */
	virtual void OnChanUnsuspend(ChannelInfo *ci) { }

	/** Called when a channel is being deleted, for any reason
	 * @param ci The channel
	 */
	virtual void OnDelChan(ChannelInfo *ci) { }

	/** Called when a nick is dropped
	 * @param nick The nick
	 */
	virtual void OnNickDrop(const char *nick) { }

	/** Called when a nick is forbidden
	 * @param na The nick alias of the forbidden nick
	 */
	virtual void OnNickForbidden(NickAlias *na) { }

	/** Called when a user groups their nick
	 * @param u The user grouping
	 * @param target The target they're grouping to
	 */
	virtual void OnNickGroup(User *u, NickAlias *target) { }

	/** Called when a user identifies
	 * @param u The user
	 */
	virtual void OnNickIdentify(User *u) { }

	/** Called when a nick logs out
	 * @param u The nick
	 */
	virtual void OnNickLogout(User *u) { }

	/** Called when a nick is registered
	 * @param The user
	 */
	virtual void OnNickRegister(User *u) { }

	/** Called when a nick is suspended
	 * @param na The nick alias
	 */
	virtual void OnNickSuspend(NickAlias *na) { }

	/** Called when a nick is unsuspneded
	 * @param na The nick alias
	 */
	virtual void OnNickUnsuspended(NickAlias *na) { }

	/** Called when the defcon level is changed
	 * @param level The level
	 */
	virtual void OnDefconLevel(int level) { }

	/** Called on findnick()
	 * @param nick nickname to be searched for
	 */
	virtual void OnFindNick(const std::string &nick) { }

	/** Called on delnick()
	 * @ param na pointer to the nickalias
	 */
	virtual void OnDelNick(NickAlias *na) { }

	/* Called on findcore()
	 * @param nick nickname to be searched for (nc->display)
	 */
	virtual void OnFindCore(const std::string &nick) { }

	/** Called on delcore()
	 * @param nc pointer to the NickCore
	 */
	virtual void OnDelCore(NickCore *nc) { }

	/** Called on change_core_display()
	 * @param nc pointer to the NickCore
	 * @param newdisplay the new display
	 */
	virtual void OnChangeCoreDisplay(NickCore *nc, const std::string &newdisplay) { }

	/** Called on findrequestnick()
	 * @param nick nicname to be searched for
	 */
	virtual void OnFindRequestNick(const std::string &nick) { }

	/** called from ns_register.c, after the NickRequest have been created
	 * @param nr pointer to the NickRequest
	 */
	virtual void OnMakeNickRequest(NickRequest *nr) { }

	/** called on delnickrequest()
	 * @param nr pointer to the NickRequest
	 */
	virtual void OnDelNickRequest(NickRequest *nr) { }

	/** called from NickCore::ClearAccess()
	 * @param nc pointer to the NickCore
	 */
	virtual void OnNickClearAccess(NickCore *nc) { }
};


/** Implementation-specific flags which may be set in ModuleManager::Attach()
 */
enum Implementation
{
	I_BEGIN,
		/* NickServ */
		I_OnNickExpire, I_OnNickForbidden, I_OnNickGroup, I_OnNickLogout, I_OnNickIdentify, I_OnNickDrop,
		I_OnNickRegister, I_OnNickSuspended, I_OnNickUnsuspended,
		I_OnFindNick, I_OnDelNick, I_OnFindCore, I_OnDelCore, I_OnChangeCoreDisplay,
		I_OnFindRequestNick, I_OnDelNickRequest, I_OnMakeNickRequest, I_OnNickClearAccess,

		/* ChanServ */
		I_OnChanForbidden, I_OnChanSuspend, I_OnChanDrop, I_OnChanExpire, I_OnAccessAdd, I_OnAccessChange,
		I_OnAccessDel, I_OnAccessClear, I_OnChanRegistered, I_OnChanUnsuspend, I_OnDelChan,

		/* BotServ */
		I_OnBotJoin, I_OnBotKick, I_OnBotCreate, I_OnBotChange, I_OnBotDelete, I_OnBotAssign, I_OnBotUnAssign,
		I_OnUserKicked, I_OnBotFantasy, I_OnBotNoFantasyAccess, I_OnBotBan,

		/* Users */
		I_OnUserConnect, I_OnUserNickChange, I_OnUserLogoff, I_OnPreJoinChannel, I_OnJoinChannel,
		I_OnPrePartChannel, I_OnPartChannel,

		/* OperServ */
		I_OnDefconLevel,

		/* Other */
		I_OnReload, I_OnPreServerConnect, I_OnServerConnect, I_OnPreCommand, I_OnPostCommand, I_OnSaveDatabase, I_OnBackupDatabase,
		I_OnPreDatabaseExpire, I_OnDatabaseExpire, I_OnPreRestart, I_OnRestart, I_OnPreShutdown, I_OnShutdown, I_OnSignal,
		I_OnServerQuit, I_OnTopicUpdated,
		I_OnEncrypt, I_OnEncryptInPlace, I_OnEncryptCheckLen, I_OnDecrypt, I_OnCheckPassword,
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

	/** Delete all timers attached to a module
	 * @param m The module
	 */
	static void ClearTimers(Module *m);

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

/*************************************************************************/
/* Module Managment Functions */
MDE Module *findModule(const char *name);	/* Find a module */

int encryption_module_init();	/* Load the encryption module */
int protocol_module_init();	/* Load the IRCD Protocol Module up*/
MDE void moduleDisplayHelp(const char *service, User *u);

/*************************************************************************/
/*************************************************************************/
/* Command Managment Functions */
MDE Command *findCommand(CommandHash *cmdTable[], const char *name);	/* Find a command */

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
