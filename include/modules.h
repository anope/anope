/* Modular support
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
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
#include "timers.h"
#include "hashcomp.h"
#include "version.h"

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
			Alog() << "Exception caught: " << modexcept.GetReason(); \
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
			Alog() << "Exception caught: " << modexcept.GetReason(); \
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

typedef enum { CORE,PROTOCOL,THIRD,SUPPORTED,QATESTED,ENCRYPTION,DATABASE } MODType;
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

enum CommandFlag
{
	CFLAG_ALLOW_UNREGISTERED,
	CFLAG_ALLOW_FORBIDDEN,
	CFLAG_ALLOW_SUSPENDED,
	CFLAG_ALLOW_UNREGISTEREDCHANNEL,
	CFLAG_STRIP_CHANNEL,
	CFLAG_DISABLE_FANTASY
};

/** Every services command is a class, inheriting from Command.
 */
class CoreExport Command : public Flags<CommandFlag>
{
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
	virtual CommandReturn Execute(User *u, const std::vector<ci::string> &);

	/** Requested when the user is requesting help on this command. Help on this command should be sent to the user.
	 * @param u The user requesting help
	 * @param subcommand The subcommand the user is requesting help on, or an empty string. (e.g. /ns help set foo bar lol gives a subcommand of "FOO BAR LOL")
	 * @return true if help was provided to the user, false otherwise.
	 */
	virtual bool OnHelp(User *u, const ci::string &subcommand);

	/** Requested when the user provides bad syntax to this command (not enough params, etc).
	 * @param u The user executing the command.
	 * @param subcommand The subcommand the user tried to use
	 */
	virtual void OnSyntaxError(User *u, const ci::string &subcommand);

	/** Set which command permission (e.g. chanserv/forbid) is required for this command.
	 * @param reststr The permission required to successfully execute this command
	 */
	void SetPermission(const std::string &reststr);

	/* Module related stuff */
	int core;		   /* Can this command be deleted? */
	char *mod_name;	/* Name of the module who owns us, NULL for core's  */
	char *service;	/* Service we provide this command for */
	Command *next;
};

class Version
{
  private:
	  unsigned Major;
	  unsigned Minor;
	  unsigned Build;

  public:
	/** Constructor
	 * @param vMajor The major version numbber
	 * @param vMinor The minor version numbber
	 * @param vBuild The build version numbber
	 */
	Version(unsigned vMajor, unsigned vMinor, unsigned vBuild);

	/** Destructor
	 */
	virtual ~Version();

	/** Get the major version of Anope this was built against
	 * @return The major version
	 */
	const unsigned GetMajor();

	/** Get the minor version of Anope this was built against
	 * @return The minor version
	 */
	const unsigned GetMinor();

	/** Get the build version this was built against
	 * @return The build version
	 */
	const unsigned GetBuild();
};

/* Forward declaration of CallBack class for the Module class */
class CallBack;

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

	/** Callbacks used in this module
	 */
	std::list<CallBack *> CallBacks;

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

	/** Get the version of Anope this module was
	 * compiled against
	 * @return The version
	 */
	virtual Version GetVersion() { return Version(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD); }

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
	void NoticeLang(const char *source, User * u, int number, ...);

	/**
	 * Add a module provided command to the given service.
	 * e.g. AddCommand(NICKSERV,c,MOD_HEAD);
	 * @param cmdTable the services to add the command to
	 * @param c the command to add
	 * @return MOD_ERR_OK on successfully adding the command
	 */
	int AddCommand(CommandHash *cmdTable[], Command * c);

	/**
	 * Delete a command from the service given.
	 * @param cmdTable the cmdTable for the services to remove the command from
	 * @param name the name of the command to delete from the service
	 * @return returns MOD_ERR_OK on success
	 */
	int DelCommand(CommandHash * cmdTable[], const char *name);

	/** Called on NickServ HELP
	 * @param u The user requesting help
	 */
	virtual void OnNickServHelp(User *u) { }

	/** Called on ChanServ HELP
	 * @param u The user requesting help
	 */
	virtual void OnChanServHelp(User *u) { }

	/** Called on Botserv HELP
	 * @param u The user requesting help
	 */
	virtual void OnBotServHelp(User *u) { }

	/** Called on HostServ HELP
	 * @param u The user requesting help
	 */
	virtual void OnHostServHelp(User *u) { }

	/** Called on OperServ HELP
	 * @param u The user requesting help
	 */
	virtual void OnOperServHelp(User *u) { }

	/** Called on MemoServ HELP
	 * @param u The user requesting help
	 */
	virtual void OnMemoServHelp(User *u) { }

	/** Called when the ircd notifies that a user has been kicked from a channel.
	 * @param c The channel the user has been kicked from.
	 * @param target The user that has been kicked.
	 * @param source The nick of the sender.
	 * @param kickmsg The reason for the kick.
	 */
	virtual void OnUserKicked(Channel *c, User *target, const std::string &source, const std::string &kickmsg) { }

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

	/** Called after a user has been introduced, but before any type
	 * of checking has been done (akills, defcon, s*lines, etc)
	 * return EVENT_STOP here to allow the user to get by untouched,
	 * or kill them then return EVENT_STOP to tell Anope the user no
	 * longer exists
	 * @param u The user
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to stop processing
	 */
	virtual EventReturn OnPreUserConnect(User *u) { return EVENT_CONTINUE; }

	/** Called when a new user connects to the network.
	 * @param u The connecting user.
	 */
	virtual void OnUserConnect(User *u) { }

	/** Called when a new server connects to the network.
	 * @param s The server that has connected to the network
	 */
	virtual void OnNewServer(Server *s) { }

	/** Called after a user changed the nick
	 * @param u The user.
	 * @param oldnick The old nick of the user
	 */
	virtual void OnUserNickChange(User *u, const std::string &oldnick) { }

	/** Called immediatly when a user tries to run a command
	 * @param service The service
	 * @param u The user
	 * @param cmd The command
	 * @param c The command class (if it exists)
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnPreCommandRun(const std::string &service, User *u, const char *cmd, Command *c) { return EVENT_CONTINUE; }

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

	/** Called after the core has finished loading the databases, but before
	 * we connect to the server
	 */
	virtual void OnPostLoadDatabases() { }

	/** Called when the databases are saved
	 * @return EVENT_CONTINUE to let other modules continue saving, EVENT_STOP to stop
	 */
	virtual EventReturn OnSaveDatabase() { return EVENT_CONTINUE; }

	/** Called when the databases are loaded
	 * @return EVENT_CONTINUE to let other modules continue saving, EVENT_STOP to stop
	 */
	virtual EventReturn OnLoadDatabase() { return EVENT_CONTINUE; }

	/** Called when anope needs to check passwords against encryption
	 *  see src/encrypt.c for detailed informations
	 */
	virtual EventReturn OnEncrypt(const std::string &src, std::string &dest) { return EVENT_CONTINUE; }
	virtual EventReturn OnEncryptInPlace(std::string &buf) { return EVENT_CONTINUE; }
	virtual EventReturn OnDecrypt(const std::string &hashm, const std::string &src, std::string &dest) { return EVENT_CONTINUE; }
	virtual EventReturn OnCheckPassword(const std::string &hashm, std::string &plaintext, std::string &password) { return EVENT_CONTINUE; }

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
	
	/** Called before a badword is added to the badword list
	 * @param ci The channel
	 * @param bw The badword
	 */
	virtual void OnBadWordAdd(ChannelInfo *ci, BadWord *bw) { }

	/** Called before a badword is deleted from a channel
	 * @param ci The channel
	 * @param bw The badword
	 */
	virtual void OnBadWordDel(ChannelInfo *ci, BadWord *bw) { }

	/** Called before a bot kicks a user
	 * @param bi The bot sending the kick
	 * @param c The channel the user is being kicked on
	 * @param u The user being kicked
	 * @param reason The reason
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnBotKick(BotInfo *bi, Channel *c, User *u, const std::string &reason) { return EVENT_CONTINUE; }

	/** Called before a user parts a channel
	 * @param u The user
	 * @param c The channel
	 */
	virtual void OnPrePartChannel(User *u, Channel *c) {}

	/** Called when a user parts a channel
	 * @param u The user
	 * @param c The channel, may be NULL if the channel no longer exists
	 * @param channel The channel name
	 * @param msg The part reason
	 */
	virtual void OnPartChannel(User *u, Channel *c, const std::string &channel, const std::string &msg) { }

	/** Called before a user joins a channel
	 * @param u The user
	 * @param c The channel
	 * @return EVENT_STOP to allow the user to join the channel through restrictions, EVENT_CONTINUE to let other modules decide
	 */
	virtual EventReturn OnPreJoinChannel(User *u, Channel *c) { return EVENT_CONTINUE; }

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

	/** Called before a channel expires
	 * @param ci The channel
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnPreChanExpire(ChannelInfo *ci) { return EVENT_CONTINUE; }

	/** Called when a channel expires
	 * @param chname The channel name
	 */
	virtual void OnChanExpire(const char *chname) { }

	/** Called before anope connects to its uplink
	 */
	virtual void OnPreServerConnect() { }

	/** Called when Anope connects to its uplink
	 */
	virtual void OnServerConnect() { }

	/** Called when we are done synching with the uplink, just before we send the EOB
	 */
	virtual void OnFinishSync(Server *serv) { }

	/** Called when Anope disconnects from its uplink, before it tries to reconnect
	 */
	virtual void OnServerDisconnect() { }

	/** Called before the database expire routines are called
	* Note: Code that is in seperate expiry routines should just be done
	* when we save the DB, theres no need to have both
	*/
	virtual void OnPreDatabaseExpire() MARK_DEPRECATED { }

	/** Called when the database expire routines are called
	 */
	virtual void OnDatabaseExpire() MARK_DEPRECATED { }

	/** Called when the flatfile dbs are being written
	 * @param Write A callback to the function used to insert a line into the database
	 */
	virtual void OnDatabaseWrite(void (*Write)(const std::string &)) { }

	/** Called when a line is read from the database
	 * @param params The params from the database
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to stop processing
	 */
	virtual EventReturn OnDatabaseRead(const std::vector<std::string> &params) { return EVENT_CONTINUE; }

	/** Called when nickcore metadata is read from the database
	 * @param nc The nickcore
	 * @param key The metadata key
	 * @param params The params from the database
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to stop processing
	 */
	virtual EventReturn OnDatabaseReadMetadata(NickCore *nc, const std::string &key, const std::vector<std::string> &params) { return EVENT_CONTINUE; }

	/** Called when nickcore metadata is read from the database
	 * @param na The nickalias
	 * @param key The metadata key
	 * @param params The params from the database
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to stop processing
	 */
	virtual EventReturn OnDatabaseReadMetadata(NickAlias *na, const std::string &key, const std::vector<std::string> &params) { return EVENT_CONTINUE; }

	/** Called when botinfo metadata is read from the database
	 * @param bi The botinfo
	 * @param key The metadata key
	 * @param params The params from the database
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to stop processing
	 */
	virtual EventReturn OnDatabaseReadMetadata(BotInfo *bi, const std::string &key, const std::vector<std::string> &params) { return EVENT_CONTINUE; }

	/** Called when chaninfo metadata is read from the database
	 * @param ci The chaninfo
	 * @param key The metadata key
	 * @param params The params from the database
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to stop processing
	 */
	virtual EventReturn OnDatabaseReadMetadata(ChannelInfo *ci, const std::string &key, const std::vector<std::string> &params) { return EVENT_CONTINUE; }

	/** Called when we are writing metadata for a nickcore
	 * @param WriteMetata A callback function used to insert the metadata
	 * @param nc The nickcore
	 */
	virtual void OnDatabaseWriteMetadata(void (*WriteMetadata)(const std::string &, const std::string &), NickCore *nc) { }

	/** Called when we are wrting metadata for a nickalias
	 * @param WriteMetata A callback function used to insert the metadata
	 * @param na The nick alias
	 */
	virtual void OnDatabaseWriteMetadata(void (*WriteMetadata)(const std::string &, const std::string &), NickAlias *na) { }

	/** Called when we are writing metadata for a botinfo
	 * @param WriteMetata A callback function used to insert the metadata
	 * @param bi The botinfo
	 */
	virtual void OnDatabaseWriteMetadata(void (*WriteMetadata)(const std::string &, const std::string &), BotInfo *bi) { }

	/** Called when are are writing metadata for a channelinfo
	 * @param WriteMetata A callback function used to insert the metadata
	 * @param bi The channelinfo
	 */
	virtual void OnDatabaseWriteMetadata(void (*WriteMetadata)(const std::string &, const std::string &), ChannelInfo *ci) { }

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

	/** Called before a nick expires
	 * @param na The nick
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnPreNickExpire(NickAlias *na) { return EVENT_CONTINUE; }

	/** Called when a nick drops
	 * @param nick The nick
	 */
	virtual void OnNickExpire(const char *nick) { }

	/** Called when defcon level changes
	 * @param level The level
	 */
	virtual void OnDefconLevel(int level) { }

	/** Called before an akill is added
	 * @param u The user adding the akill
	 * @param ak The akill
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnAddAkill(User *u, Akill *ak) { return EVENT_CONTINUE; }

	/** Called before an akill is deleted
	 * @param u The user removing the akill
	 * @param ak The akill, can be NULL for all akills!
	 */
	virtual void OnDelAkill(User *u, Akill *ak) { }

	/** Called after an exception has been added
	 * @param u The user who added it
	 * @param ex The exception
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnExceptionAdd(User *u, Exception *ex) { return EVENT_CONTINUE; }

	/** Called before an exception is deleted
	 * @param u The user who is deleting it
	 * @param ex The exceotion
	 */
	virtual void OnExceptionDel(User *u, Exception *ex) { }

	/** Called before a SXLine is added
	 * @param u The user adding the SXLine
	 * @param sx The SXLine
	 * @param Type The type of SXLine this is
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnAddSXLine(User *u, SXLine *sx, SXLineType Type) { return EVENT_CONTINUE; }

	/** Called before a SXLine is deleted
	 * @param u The user deleting the SXLine
	 * @param sx The SXLine, can be NULL for all SXLines
	 * @param Type The type of SXLine this is
	 */
	virtual void OnDelSXLine(User *u, SXLine *sx, SXLineType Type) { }

	/** Called when a server quits
	 * @param server The server
	 */
	virtual void OnServerQuit(Server *server) { }

	/** Called on a QUIT
	 * @param u The user
	 * @param msg The quit message
	 */
	virtual void OnUserQuit(User *u, const std::string &msg) { }

	/** Called when a user disconnects
	 * @param u The user
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
	 * @param nc The user who was deleted
	 */
	virtual void OnAccessDel(ChannelInfo *ci, User *u, NickCore *nc) { }

	/** Called when access is changed
	 * @param ci The channel
	 * @param u The user who changed the access
	 * @param na The nick whos access was changed
	 * @param level The level of the new access
	 */
	virtual void OnAccessChange(ChannelInfo *ci, User *u, NickAlias *na, int level) { }

	/** Called when access is added
	 * @param ci The channel
	 * @param u The user who added the access
	 * @para na The nick who was added to access
	 * @param level The level they were added at
	 */
	virtual void OnAccessAdd(ChannelInfo *ci, User *u, NickAlias *na, int level) { }

	/** Called when the access list is cleared
	 * @param ci The channel
	 * @param u The user who cleared the access
	 */
	virtual void OnAccessClear(ChannelInfo *ci, User *u) { }

	/** Called when a level for a channel is changed
	 * @param u The user changing the level
	 * @param ci The channel the level was changed on
	 * @param pos The level position, can be -1 for resetting levels
	 * @param what The new level
	 */
	virtual void OnLevelChange(User *u, ChannelInfo *ci, int pos, int what) { }

	/** Called when a channel is dropped
	 * @param chname The channel name
	 */
	virtual void OnChanDrop(const std::string &chname) { }

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

	/** Called when a new channel is created
	 * @param c The channel
	 */
	virtual void OnChannelCreate(Channel *c) { }

	/** Called when a channel is deleted
	 * @param c The channel
	 */
	virtual void OnChannelDelete(Channel *c) { }

	/** Called after adding an akick to a channel
	 * @param ci The channel
	 * @param ak The akick
	 */
	virtual void OnAkickAdd(ChannelInfo *ci, AutoKick *ak) { }

	/** Called before removing an akick from a channel
	 * @param ci The channel
	 * @param ak The akick
	 */
	virtual void OnAkickDel(ChannelInfo *ci, AutoKick *ak) { }

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
	 * @param The nick
	 */
	virtual void OnNickRegister(NickAlias *na) { }

	/** Called when a nick is suspended
	 * @param na The nick alias
	 */
	virtual void OnNickSuspend(NickAlias *na) { }

	/** Called when a nick is unsuspneded
	 * @param na The nick alias
	 */
	virtual void OnNickUnsuspended(NickAlias *na) { }

	/** Called on delnick()
	 * @ param na pointer to the nickalias
	 */
	virtual void OnDelNick(NickAlias *na) { }

	/** Called on delcore()
	 * @param nc pointer to the NickCore
	 */
	virtual void OnDelCore(NickCore *nc) { }

	/** Called on change_core_display()
	 * @param nc pointer to the NickCore
	 * @param newdisplay the new display
	 */
	virtual void OnChangeCoreDisplay(NickCore *nc, const std::string &newdisplay) { }

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

	/** Called when a user adds an entry to their access list
	 * @param nc The nick
	 * @param entry The entry
	 */
	virtual void OnNickAddAccess(NickCore *nc, const std::string &entry) { }

	/** called from NickCore::EraseAccess()
	 * @param nc pointer to the NickCore
	 * @param entry The access mask
	 */
	virtual void OnNickEraseAccess(NickCore *nc, const std::string &entry) { }

	/** called when a vhost is deleted
	 * @param na The nickalias of the vhost
	 */
	virtual void OnDeleteVhost(NickAlias *na) { }

	/** Called when a vhost is set
	 * @param na The nickalias of the vhost
	 */
	virtual void OnSetVhost(NickAlias *na) { }

	/** Called when a memo is sent
	 * @param u The user sending the memo
	 * @param nc The nickcore of who the memo was sent to
	 * @param m The memo
	 */
	virtual void OnMemoSend(User *u, NickCore *nc, Memo *m) { }

	/** Called when a memo is sent
	 * @param u The user sending the memo
	 * @param ci The channel the memo was sent to
	 * @param m The memo
	 */
	virtual void OnMemoSend(User *u, ChannelInfo *ci, Memo *m) { }

	/** Called when a memo is deleted
	 * @param nc The nickcore of the memo being deleted
	 * @param mi The memo info
	 * @param number What memo number is being deleted, can be 0 for all memos
	 */
	virtual void OnMemoDel(NickCore *nc, MemoInfo *mi, int number) { }

	/** Called when a memo is deleted
	 * @param ci The channel of the memo being deleted
	 * @param mi The memo info
	 * @param number What memo number is being deleted, can be 0 for all memos
	 */
	virtual void OnMemoDel(ChannelInfo *ci, MemoInfo *mi, int number) { }

	/** Called when a mode is set on a channel
	 * @param c The channel
	 * @param Name The mode name
	 * @param param The mode param, if there is one
	 * @return EVENT_STOP to make mlock/secureops etc checks not happen
	 */
	virtual EventReturn OnChannelModeSet(Channel *c, ChannelModeName Name, const std::string &param) { return EVENT_CONTINUE; }

	/** Called when a mode is unset on a channel
	 * @param c The channel
	 * @param Name The mode name
	 * @param param The mode param, if there is one
	 * @return EVENT_STOP to make mlock/secureops etc checks not happen
	 */
	virtual EventReturn OnChannelModeUnset(Channel *c, ChannelModeName Name, const std::string &param) { return EVENT_CONTINUE; }

	/** Called when a mode is set on a user
	 * @param u The user
	 * @param Name The mode name
	 */
	virtual void OnUserModeSet(User *u, UserModeName Name) { }

	/** Called when a mode is unset from a user
	 * @param u The user
	 * @param Name The mode name
	 */
	virtual void OnUserModeUnset(User *u, UserModeName Name) { }

	/** Called when a channel mode is introducted into Anope
	 * @param cm The mode
	 */
	virtual void OnChannelModeAdd(ChannelMode *cm) { }

	/** Called when a user mode is introducted into Anope
	 * @param um The mode
	 */
	virtual void OnUserModeAdd(UserMode *um) { }

	/** Called when a mode is about to be mlocked
	 * @param Name The mode being mlocked
	 * @param status true if its being mlocked +, false for -
	 * @param param The param, if there is one and if status is true
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to deny the mlock.
	 */
	virtual EventReturn OnMLock(ChannelModeName Name, bool status, const std::string &param) { return EVENT_CONTINUE; }

	/** Called when a mode is about to be unlocked
	 * @param Name The mode being mlocked
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to deny the mlock.
	 */
	virtual EventReturn OnUnMLock(ChannelModeName Name) { return EVENT_CONTINUE; }

	/** Called after a module is loaded
	 * @param u The user loading the module, can be NULL
	 * @param m The module
	 */
	virtual void OnModuleLoad(User *u, Module *m) { }

	/** Called before a module is unloaded
	 * @param u The user, can be NULL
	 * @param m The module
	 */
	virtual void OnModuleUnload(User *u, Module *m) { }

	/** Called when a server is synced
	 * @param s The server, can be our uplink server
	 */
	virtual void OnServerSync(Server *s) { }

	/** Called when we sync with our uplink
	 */
	virtual void OnUplinkSync() { }

};


/** Implementation-specific flags which may be set in ModuleManager::Attach()
 */
enum Implementation
{
	I_BEGIN,
		/* NickServ */
		I_OnNickServHelp, I_OnPreNickExpire, I_OnNickExpire, I_OnNickForbidden, I_OnNickGroup, I_OnNickLogout, I_OnNickIdentify, I_OnNickDrop,
		I_OnNickRegister, I_OnNickSuspended, I_OnNickUnsuspended,
		I_OnDelNick, I_OnDelCore, I_OnChangeCoreDisplay,
		I_OnDelNickRequest, I_OnMakeNickRequest, I_OnNickClearAccess, I_OnNickAddAccess, I_OnNickEraseAccess,

		/* ChanServ */
		I_OnChanServHelp, I_OnChanForbidden, I_OnChanSuspend, I_OnChanDrop, I_OnPreChanExpire, I_OnChanExpire, I_OnAccessAdd, I_OnAccessChange,
		I_OnAccessDel, I_OnAccessClear, I_OnLevelChange, I_OnChanRegistered, I_OnChanUnsuspend, I_OnDelChan, I_OnChannelCreate,
		I_OnChannelDelete, I_OnAkickAdd, I_OnAkickDel,

		/* BotServ */
		I_OnBotServHelp, I_OnBotJoin, I_OnBotKick, I_OnBotCreate, I_OnBotChange, I_OnBotDelete, I_OnBotAssign, I_OnBotUnAssign,
		I_OnUserKicked, I_OnBotFantasy, I_OnBotNoFantasyAccess, I_OnBotBan, I_OnBadWordAdd, I_OnBadWordDel,

		/* HostServ */
		I_OnHostServHelp, I_OnSetVhost, I_OnDeleteVhost,

		/* MemoServ */
		I_OnMemoServHelp, I_OnMemoSend, I_OnMemoDel,

		/* Users */
		I_OnPreUserConnect, I_OnUserConnect, I_OnUserNickChange, I_OnUserQuit, I_OnUserLogoff, I_OnPreJoinChannel,
		I_OnJoinChannel, I_OnPrePartChannel, I_OnPartChannel,

		/* OperServ */
		I_OnOperServHelp, I_OnDefconLevel, I_OnAddAkill, I_OnDelAkill, I_OnExceptionAdd, I_OnExceptionDel,
		I_OnAddSXLine, I_OnDelSXLine,

		/* Database */
		I_OnPostLoadDatabases, I_OnSaveDatabase, I_OnLoadDatabase,
		I_OnDatabaseExpire,
		I_OnDatabaseWrite, I_OnDatabaseRead, I_OnDatabaseReadMetadata, I_OnDatabaseWriteMetadata,

		/* Modules */
		I_OnModuleLoad, I_OnModuleUnload,

		/* Other */
		I_OnReload, I_OnPreServerConnect, I_OnNewServer, I_OnServerConnect, I_OnFinishSync, I_OnServerDisconnect, I_OnPreCommandRun,
		I_OnPreCommand, I_OnPostCommand, I_OnPreDatabaseExpire, I_OnPreRestart, I_OnRestart, I_OnPreShutdown, I_OnShutdown, I_OnSignal,
		I_OnServerQuit, I_OnTopicUpdated,
		I_OnEncrypt, I_OnEncryptInPlace, I_OnEncryptCheckLen, I_OnDecrypt, I_OnCheckPassword,
		I_OnChannelModeSet, I_OnChannelModeUnset, I_OnUserModeSet, I_OnUserModeUnset, I_OnChannelModeAdd, I_OnUserModeAdd,
		I_OnMLock, I_OnUnMLock, I_OnServerSync, I_OnUplinkSync,
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
	 * @param module_list The list of modules to load
	 **/
	static void LoadModuleList(std::list<std::string> &ModList);

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

	/** Delete all callbacks attached to a module
	 * @param m The module
	 */
	static void ClearCallBacks(Module *m);

private:
	/** Call the module_delete function to safely delete the module
	 * @param m the module to delete
	 */
	static void DeleteModule(Module *m);
};

/** Class used for callbacks within modules
 */
class CallBack : public Timer
{
 private:
	Module *m;
 public:
	CallBack(Module *mod, long time_from_now, time_t now = time(NULL), bool repeating = false) : Timer(time_from_now, now, repeating),  m(mod)
	{
		m->CallBacks.push_back(this);
	}

	virtual ~CallBack()
	{
		std::list<CallBack *>::iterator it = std::find(m->CallBacks.begin(), m->CallBacks.end(), this);
		if (it != m->CallBacks.end())
		{
			m->CallBacks.erase(it);
		}
	}
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

int protocol_module_init();	/* Load the IRCD Protocol Module up*/

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

/************************************************************************/

#endif
/* EOF */
