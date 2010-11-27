/* Modular support
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef MODULES_H
#define MODULES_H

#include <time.h>
#include "services.h"
#include <stdio.h>
#include "timers.h"
#include "hashcomp.h"
#include "commands.h"

/* Cross OS compatibility macros */
#ifdef _WIN32
	typedef HMODULE ano_module_t;

# define dlopen(file, unused) LoadLibrary(file)
# define dlsym(file, symbol) (HMODULE)GetProcAddress(file, symbol)
# define dlclose(file) FreeLibrary(file) ? 0 : 1
# define ano_modclearerr() SetLastError(0)
# define ano_moderr() Anope::LastError().c_str()
#else
	typedef void * ano_module_t;

/* We call dlerror() here because it clears the module error after being
 * called. This previously read 'errno = 0', but that didn't work on
 * all POSIX-compliant architectures. This way the error is guaranteed
 * to be cleared, POSIX-wise. -GD
 */
# define ano_modclearerr() dlerror()
# define ano_moderr() dlerror()
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
#define FOREACH_MOD(y, x) \
if (true) \
{ \
	std::vector<Module*>::iterator safei; \
	for (std::vector<Module*>::iterator _i = ModuleManager::EventHandlers[y].begin(); _i != ModuleManager::EventHandlers[y].end(); ) \
	{ \
		safei = _i; \
		++safei; \
		try \
		{ \
			(*_i)->x ; \
		} \
		catch (const ModuleException &modexcept) \
		{ \
			Log() << "Exception caught: " << modexcept.GetReason(); \
		} \
		_i = safei; \
	} \
} \
else \
	static_cast<void>(0)

/**
 * This define is similar to the one above but returns a result in MOD_RESULT.
 * The first module to return a nonzero result is the value to be accepted,
 * and any modules after are ignored.
 */
#define FOREACH_RESULT(y, x) \
if (true) \
{ \
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
		catch (const ModuleException &modexcept) \
		{ \
			Log() << "Exception caught: " << modexcept.GetReason(); \
		} \
		_i = safei; \
	} \
} \
else \
	static_cast<void>(0)

#ifndef _WIN32
# include <dlfcn.h>
	/* Define these for systems without them */
# ifndef RTLD_NOW
#  define RTLD_NOW 0
# endif
# ifndef RTLD_LAZY
#  define RTLD_LAZY RTLD_NOW
# endif
# ifndef RTLD_GLOBAL
#  define RTLD_GLOBAL 0
# endif
# ifndef RTLD_LOCAL
#  define RTLD_LOCAL 0
# endif
#endif

class Message;

extern CoreExport Module *FindModule(const Anope::string &name);
int protocol_module_init();
extern CoreExport bool moduleMinVersion(int major, int minor, int patch, int build);

enum ModuleReturn
{
	MOD_ERR_OK,
	MOD_ERR_MEMORY,
	MOD_ERR_PARAMS,
	MOD_ERR_EXISTS,
	MOD_ERR_NOEXIST,
	MOD_ERR_NOUSER,
	MOD_ERR_NOLOAD,
	MOD_ERR_NOUNLOAD,
	MOD_ERR_SYNTAX,
	MOD_ERR_NODELETE,
	MOD_ERR_UNKNOWN,
	MOD_ERR_FILE_IO,
	MOD_ERR_NOSERVICE,
	MOD_ERR_NO_MOD_NAME,
	MOD_ERR_EXCEPTION,
	MOD_ERR_VERSION
};

/** Priority types which can be returned from Module::Prioritize()
 */
enum Priority { PRIORITY_FIRST, PRIORITY_DONTCARE, PRIORITY_LAST, PRIORITY_BEFORE, PRIORITY_AFTER };
/* Module types, in the order in which they are unloaded. The order these are in is IMPORTANT */
enum MODType { MT_BEGIN, THIRD, QATESTED, SUPPORTED, CORE, DATABASE, ENCRYPTION, PROTOCOL, SOCKETENGINE, MT_END };

typedef std::multimap<Anope::string, Message *> message_map;
extern CoreExport message_map MessageMap;
class Module;
extern CoreExport std::list<Module *> Modules;

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
	unsigned GetMajor() const;

	/** Get the minor version of Anope this was built against
	 * @return The minor version
	 */
	unsigned GetMinor() const;

	/** Get the build version this was built against
	 * @return The build version
	 */
	unsigned GetBuild() const;
};

/* Forward declaration of CallBack class for the Module class */
class CallBack;

/** Every module in Anope is actually a class.
 */
class CoreExport Module : public Extensible
{
 private:
	bool permanent;
 public:
	/** The module name (e.g. os_modload)
	 */
	Anope::string name;

	/** The temporary path/filename
	 */
	Anope::string filename;

	/** Callbacks used in this module
	 */
	std::list<CallBack *> CallBacks;

	/** Handle for this module, obtained from dlopen()
	 */
	ano_module_t handle;

	/** Time this module was created
	 */
	time_t created;

	/** Version of this module
	 */
	Anope::string version;

	/** Author of the module
	 */
	Anope::string author;

	/** What type this module is
	 */
	MODType type;

	/** Creates and initialises a new module.
	 * @param loadernick The nickname of the user loading the module.
	 */
	Module(const Anope::string &modname, const Anope::string &loadernick);

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
	bool GetPermanent() const;

	/** Set the modules version info.
	 * @param version the version of the module
	 */
	void SetVersion(const Anope::string &version);

	/** Set the modules author info
	 * @param author the author of the module
	 */
	void SetAuthor(const Anope::string &author);

	/** Get the version of Anope this module was
	 * compiled against
	 * @return The version
	 */
	Version GetVersion() const;

	/** Send a message to a user in their language, if a translation is available
	 * @param source The source of the message
	 * @param fmt The message
	 */
	void SendMessage(CommandSource &source, const char *fmt, ...);

	/**
	 * Add a module provided command to the given service.
	 * @param bi The service to add the command to
	 * @param c The command to add
	 * @return MOD_ERR_OK on successfully adding the command
	 */
	int AddCommand(BotInfo *bi, Command *c);

	/**
	 * Delete a command from the service given.
	 * @param bi The service to remove the command from
	 * @param c Thec command to delete
	 * @return returns MOD_ERR_OK on success
	 */
	int DelCommand(BotInfo *bi, Command *c);

	/** Called when the ircd notifies that a user has been kicked from a channel.
	 * @param c The channel the user has been kicked from.
	 * @param target The user that has been kicked.
	 * @param source The nick of the sender.
	 * @param kickmsg The reason for the kick.
	 */
	virtual void OnUserKicked(Channel *c, User *target, const Anope::string &source, const Anope::string &kickmsg) { }

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
	virtual void OnUserNickChange(User *u, const Anope::string &oldnick) { }

	/** Called immediatly when a user tries to run a command
	 * @param u The user
	 * @param bi The bot the command is being run from
	 * @param command The command
	 * @param message The parameters used for the command
	 * @param ci If a tanasy command, the channel the comman was used on
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnPreCommandRun(User *u, BotInfo *bi, Anope::string &command, Anope::string &message, ChannelInfo *ci) { return EVENT_CONTINUE; }

	/** Called before a command is due to be executed.
	 * @param source The source of the command
	 * @param command The command the user is executing
	 * @param params The parameters the user is sending
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnPreCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params) { return EVENT_CONTINUE; }

	/** Called after a command has been executed.
	 * @param source The source of the command
	 * @param command The command the user executed
	 * @param params The parameters the user sent
	 */
	virtual void OnPostCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params) { }

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
	virtual EventReturn OnEncrypt(const Anope::string &src, Anope::string &dest) { return EVENT_CONTINUE; }
	virtual EventReturn OnDecrypt(const Anope::string &hashm, const Anope::string &src, Anope::string &dest) { return EVENT_CONTINUE; }
	virtual EventReturn OnCheckPassword(const Anope::string &hashm, Anope::string &plaintext, Anope::string &password) { return EVENT_CONTINUE; }

	/** Called on fantasy command
	 * @param command The command
	 * @param u The user using the command
	 * @param ci The channel it's being used in
	 * @param params The params
	 */
	virtual void OnBotFantasy(const Anope::string &command, User *u, ChannelInfo *ci, const Anope::string &params) { }

	/** Called on fantasy command without access
	 * @param command The command
	 * @param u The user using the command
	 * @param ci The channel it's being used in
	 * @param params The params
	 */
	virtual void OnBotNoFantasyAccess(const Anope::string &command, User *u, ChannelInfo *ci, const Anope::string &params) { }

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
	virtual void OnBotBan(User *u, ChannelInfo *ci, const Anope::string &mask) { }

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
	virtual EventReturn OnBotKick(BotInfo *bi, Channel *c, User *u, const Anope::string &reason) { return EVENT_CONTINUE; }

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
	virtual void OnPartChannel(User *u, Channel *c, const Anope::string &channel, const Anope::string &msg) { }

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
	virtual void OnTopicUpdated(Channel *c, const Anope::string &topic) { }

	/** Called before a channel expires
	 * @param ci The channel
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnPreChanExpire(ChannelInfo *ci) { return EVENT_CONTINUE; }

	/** Called before a channel expires
	 * @param ci The channel
	 */
	virtual void OnChanExpire(ChannelInfo *ci) { }

	/** Called before Anope connects to its uplink
	 * @param u The uplink we're going to connect to
	 * @param Number What number the uplink is
	 * @return Other than EVENT_CONTINUE to stop attempting to connect
	 */
	virtual EventReturn OnPreServerConnect(Uplink *u, int Number) { return EVENT_CONTINUE; }

	/** Called when Anope connects to its uplink
	 */
	virtual void OnServerConnect() { }

	/** Called when we are almost done synching with the uplink, just before we send the EOB
	 */
	virtual void OnPreUplinkSync(Server *serv) { }

	/** Called when Anope disconnects from its uplink, before it tries to reconnect
	 */
	virtual void OnServerDisconnect() { }

	/** Called before the database expire routines are called
	* Note: Code that is in seperate expiry routines should just be done
	* when we save the DB, theres no need to have both
	*/
	virtual void OnPreDatabaseExpire() { }

	/** Called when the database expire routines are called
	 */
	virtual void OnDatabaseExpire() { }

	/** Called when the flatfile dbs are being written
	 * @param Write A callback to the function used to insert a line into the database
	 */
	virtual void OnDatabaseWrite(void (*Write)(const Anope::string &)) { }

	/** Called when a line is read from the database
	 * @param params The params from the database
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to stop processing
	 */
	virtual EventReturn OnDatabaseRead(const std::vector<Anope::string> &params) { return EVENT_CONTINUE; }

	/** Called when nickcore metadata is read from the database
	 * @param nc The nickcore
	 * @param key The metadata key
	 * @param params The params from the database
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to stop processing
	 */
	virtual EventReturn OnDatabaseReadMetadata(NickCore *nc, const Anope::string &key, const std::vector<Anope::string> &params) { return EVENT_CONTINUE; }

	/** Called when nickcore metadata is read from the database
	 * @param na The nickalias
	 * @param key The metadata key
	 * @param params The params from the database
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to stop processing
	 */
	virtual EventReturn OnDatabaseReadMetadata(NickAlias *na, const Anope::string &key, const std::vector<Anope::string> &params) { return EVENT_CONTINUE; }

	/** Called when nickrequest metadata is read from the database
	 * @param nr The nickrequest
	 * @parm key The metadata key
	 * @param params The params from the database
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to stop processing
	 */
	virtual EventReturn OnDatabaseReadMetadata(NickRequest *nr, const Anope::string &key, const std::vector<Anope::string> &params) { return EVENT_CONTINUE; }

	/** Called when botinfo metadata is read from the database
	 * @param bi The botinfo
	 * @param key The metadata key
	 * @param params The params from the database
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to stop processing
	 */
	virtual EventReturn OnDatabaseReadMetadata(BotInfo *bi, const Anope::string &key, const std::vector<Anope::string> &params) { return EVENT_CONTINUE; }

	/** Called when chaninfo metadata is read from the database
	 * @param ci The chaninfo
	 * @param key The metadata key
	 * @param params The params from the database
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to stop processing
	 */
	virtual EventReturn OnDatabaseReadMetadata(ChannelInfo *ci, const Anope::string &key, const std::vector<Anope::string> &params) { return EVENT_CONTINUE; }

	/** Called when we are writing metadata for a nickcore
	 * @param WriteMetata A callback function used to insert the metadata
	 * @param nc The nickcore
	 */
	virtual void OnDatabaseWriteMetadata(void (*WriteMetadata)(const Anope::string &, const Anope::string &), NickCore *nc) { }

	/** Called when we are wrting metadata for a nickalias
	 * @param WriteMetata A callback function used to insert the metadata
	 * @param na The nick alias
	 */
	virtual void OnDatabaseWriteMetadata(void (*WriteMetadata)(const Anope::string &, const Anope::string &), NickAlias *na) { }

	/** Called when we are wrting metadata for a nickrequest
	 * @param WriteMetata A callback function used to insert the metadata
	 * @param nr The nick request
	 */
	virtual void OnDatabaseWriteMetadata(void (*WriteMetadata)(const Anope::string &, const Anope::string &), NickRequest *nr) { }

	/** Called when we are writing metadata for a botinfo
	 * @param WriteMetata A callback function used to insert the metadata
	 * @param bi The botinfo
	 */
	virtual void OnDatabaseWriteMetadata(void (*WriteMetadata)(const Anope::string &, const Anope::string &), BotInfo *bi) { }

	/** Called when are are writing metadata for a channelinfo
	 * @param WriteMetata A callback function used to insert the metadata
	 * @param bi The channelinfo
	 */
	virtual void OnDatabaseWriteMetadata(void (*WriteMetadata)(const Anope::string &, const Anope::string &), ChannelInfo *ci) { }

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
	 * @param signum The signum
	 * @param msg The quitmsg
	 */
	virtual void OnSignal(int signum, const Anope::string &msg) { }

	/** Called before a nick expires
	 * @param na The nick
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnPreNickExpire(NickAlias *na) { return EVENT_CONTINUE; }

	/** Called when a nick drops
	 * @param na The nick
	 */
	virtual void OnNickExpire(NickAlias *na) { }

	/** Called when defcon level changes
	 * @param level The level
	 */
	virtual void OnDefconLevel(int level) { }

	/** Called before an akill is added
	 * @param u The user adding the akill
	 * @param ak The akill
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnAddAkill(User *u, XLine *ak) { return EVENT_CONTINUE; }

	/** Called before an akill is deleted
	 * @param u The user removing the akill
	 * @param ak The akill, can be NULL for all akills!
	 */
	virtual void OnDelAkill(User *u, XLine *ak) { }

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

	/** Called before a XLine is added
	 * @param u The user adding the XLine
	 * @param sx The XLine
	 * @param Type The type of XLine this is
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
	 */
	virtual EventReturn OnAddXLine(User *u, XLine *x, XLineType Type) { return EVENT_CONTINUE; }

	/** Called before a XLine is deleted
	 * @param u The user deleting the XLine
	 * @param sx The XLine, can be NULL for all XLines
	 * @param Type The type of XLine this is
	 */
	virtual void OnDelXLine(User *u, XLine *x, XLineType Type) { }

	/** Called when a server quits
	 * @param server The server
	 */
	virtual void OnServerQuit(Server *server) { }

	/** Called on a QUIT
	 * @param u The user
	 * @param msg The quit message
	 */
	virtual void OnUserQuit(User *u, const Anope::string &msg) { }

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
	 * @param nc The nick whos access was changed
	 * @param level The level of the new access
	 */
	virtual void OnAccessChange(ChannelInfo *ci, User *u, NickCore *nc, int level) { }

	/** Called when access is added
	 * @param ci The channel
	 * @param u The user who added the access
	 * @para nc The nick who was added to access
	 * @param level The level they were added at
	 */
	virtual void OnAccessAdd(ChannelInfo *ci, User *u, NickCore *nc, int level) { }

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
	virtual void OnChanDrop(const Anope::string &chname) { }

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
	 * @param u The user adding the akick
	 * @param ci The channel
	 * @param ak The akick
	 */
	virtual void OnAkickAdd(User *u, ChannelInfo *ci, AutoKick *ak) { }

	/** Called before removing an akick from a channel
	 * @param u The user removing the akick
	 * @param ci The channel
	 * @param ak The akick
	 */
	virtual void OnAkickDel(User *u, ChannelInfo *ci, AutoKick *ak) { }

	/** Called when a user requests info for a channel
	 * @param u The user requesting info
	 * @param ci The channel the user is requesting info for
	 * @param ShowHidden true if we should show the user everything
	 */
	virtual void OnChanInfo(User *u, ChannelInfo *ci, bool ShowHidden) { }

	/** Called when a nick is dropped
	 * @param nick The nick
	 */
	virtual void OnNickDrop(const Anope::string &nick) { }

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
	virtual void OnChangeCoreDisplay(NickCore *nc, const Anope::string &newdisplay) { }

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
	virtual void OnNickAddAccess(NickCore *nc, const Anope::string &entry) { }

	/** Called from NickCore::EraseAccess()
	 * @param nc pointer to the NickCore
	 * @param entry The access mask
	 */
	virtual void OnNickEraseAccess(NickCore *nc, const Anope::string &entry) { }

	/** Called when a user requests info for a nick
	 * @param u The user requesting info
	 * @param na The nick the user is requesting info from
	 * @param ShowHidden true if we should show the user everything
	 */
	virtual void OnNickInfo(User *u, NickAlias *na, bool ShowHidden) { }

	/** Called when a vhost is deleted
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
	 * @param m The memo
	 * @param number What memo number is being deleted, can be 0 for all memos
	 */
	virtual void OnMemoDel(const NickCore *nc, MemoInfo *mi, Memo *m) { }

	/** Called when a memo is deleted
	 * @param ci The channel of the memo being deleted
	 * @param mi The memo info
	 * @param m The memo
	 * @param number What memo number is being deleted, can be 0 for all memos
	 */
	virtual void OnMemoDel(ChannelInfo *ci, MemoInfo *mi, Memo *m) { }

	/** Called when a mode is set on a channel
	 * @param c The channel
	 * @param Name The mode name
	 * @param param The mode param, if there is one
	 * @return EVENT_STOP to make mlock/secureops etc checks not happen
	 */
	virtual EventReturn OnChannelModeSet(Channel *c, ChannelModeName Name, const Anope::string &param) { return EVENT_CONTINUE; }

	/** Called when a mode is unset on a channel
	 * @param c The channel
	 * @param Name The mode name
	 * @param param The mode param, if there is one
	 * @return EVENT_STOP to make mlock/secureops etc checks not happen
	 */
	virtual EventReturn OnChannelModeUnset(Channel *c, ChannelModeName Name, const Anope::string &param) { return EVENT_CONTINUE; }

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
	 * @param ci The channel the mode is being locked on
	 * @param lock The mode lock
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to deny the mlock.
	 */
	virtual EventReturn OnMLock(ChannelInfo *ci, ModeLock *lock) { return EVENT_CONTINUE; }

	/** Called when a mode is about to be unlocked
	 * @param ci The channel the mode is being unlocked from
	 * @param mode The mode being unlocked
	 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to deny the mlock.
	 */
	virtual EventReturn OnUnMLock(ChannelInfo *ci, ChannelMode *mode, const Anope::string &param) { return EVENT_CONTINUE; }

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
	 * @param s Our uplink
	 */
	virtual void OnUplinkSync(Server *s) { }
};

/** Implementation-specific flags which may be set in ModuleManager::Attach()
 */
enum Implementation
{
	I_BEGIN,
		/* NickServ */
		I_OnPreNickExpire, I_OnNickExpire, I_OnNickForbidden, I_OnNickGroup, I_OnNickLogout, I_OnNickIdentify, I_OnNickDrop,
		I_OnNickRegister, I_OnNickSuspended, I_OnNickUnsuspended,
		I_OnDelNick, I_OnDelCore, I_OnChangeCoreDisplay,
		I_OnDelNickRequest, I_OnMakeNickRequest, I_OnNickClearAccess, I_OnNickAddAccess, I_OnNickEraseAccess,
		I_OnNickInfo,

		/* ChanServ */
		I_OnChanForbidden, I_OnChanSuspend, I_OnChanDrop, I_OnPreChanExpire, I_OnChanExpire, I_OnAccessAdd, I_OnAccessChange,
		I_OnAccessDel, I_OnAccessClear, I_OnLevelChange, I_OnChanRegistered, I_OnChanUnsuspend, I_OnDelChan, I_OnChannelCreate,
		I_OnChannelDelete, I_OnAkickAdd, I_OnAkickDel,
		I_OnChanInfo,

		/* BotServ */
		I_OnBotJoin, I_OnBotKick, I_OnBotCreate, I_OnBotChange, I_OnBotDelete, I_OnBotAssign, I_OnBotUnAssign,
		I_OnUserKicked, I_OnBotFantasy, I_OnBotNoFantasyAccess, I_OnBotBan, I_OnBadWordAdd, I_OnBadWordDel,

		/* HostServ */
		I_OnSetVhost, I_OnDeleteVhost,

		/* MemoServ */
		I_OnMemoSend, I_OnMemoDel,

		/* Users */
		I_OnPreUserConnect, I_OnUserConnect, I_OnUserNickChange, I_OnUserQuit, I_OnUserLogoff, I_OnPreJoinChannel,
		I_OnJoinChannel, I_OnPrePartChannel, I_OnPartChannel,

		/* OperServ */
		I_OnDefconLevel, I_OnAddAkill, I_OnDelAkill, I_OnExceptionAdd, I_OnExceptionDel,
		I_OnAddXLine, I_OnDelXLine,

		/* Database */
		I_OnPostLoadDatabases, I_OnSaveDatabase, I_OnLoadDatabase,
		I_OnDatabaseExpire,
		I_OnDatabaseWrite, I_OnDatabaseRead, I_OnDatabaseReadMetadata, I_OnDatabaseWriteMetadata,

		/* Modules */
		I_OnModuleLoad, I_OnModuleUnload,

		/* Other */
		I_OnReload, I_OnPreServerConnect, I_OnNewServer, I_OnServerConnect, I_OnPreUplinkSync, I_OnServerDisconnect, I_OnPreCommandRun,
		I_OnPreCommand, I_OnPostCommand, I_OnPreDatabaseExpire, I_OnPreRestart, I_OnRestart, I_OnPreShutdown, I_OnShutdown, I_OnSignal,
		I_OnServerQuit, I_OnTopicUpdated,
		I_OnEncrypt, I_OnEncryptCheckLen, I_OnDecrypt, I_OnCheckPassword,
		I_OnChannelModeSet, I_OnChannelModeUnset, I_OnUserModeSet, I_OnUserModeUnset, I_OnChannelModeAdd, I_OnUserModeAdd,
		I_OnMLock, I_OnUnMLock, I_OnServerSync, I_OnUplinkSync,
	I_END
};

class Service;

/** Used to manage modules.
 */
class CoreExport ModuleManager
{
 private:
	/** A map of service providers
	 */
	static std::map<Anope::string, Service *> ServiceProviders;
 public:
	/** Event handler hooks.
	 * This needs to be public to be used by FOREACH_MOD and friends.
	 */
	static std::vector<Module *> EventHandlers[I_END];

	/** Load up a list of modules.
	 * @param module_list The list of modules to load
	 **/
	static void LoadModuleList(std::list<Anope::string> &ModList);

	/** Loads a given module.
	 * @param m the module to load
	 * @param u the user who loaded it, NULL for auto-load
	 * @return MOD_ERR_OK on success, anything else on fail
	 */
	static ModuleReturn LoadModule(const Anope::string &modname, User *u);

	/** Unload the given module.
	 * @param m the module to unload
	 * @param u the user who unloaded it
	 * @return MOD_ERR_OK on success, anything else on fail
	 */
	static ModuleReturn UnloadModule(Module *m, User * u);

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
	static bool SetPriority(Module *mod, Implementation i, Priority s, Module **modules = NULL, size_t sz = 1);

	/** Change the priority of all events in a module.
	 * @param mod The module to set the priority of
	 * @param s The priority of all events in the module.
	 * Note that with this method, it is not possible to effectively use PRIO_BEFORE or PRIO_AFTER, you should use the more fine tuned
	 * SetPriority method for this, where you may specify other modules to be prioritized against.
	 */
	static bool SetPriority(Module *mod, Priority s);

	/** Attach an event to a module.
	 * You may later detatch the event with ModuleManager::Detach(). If your module is unloaded, all events are automatically detatched.
	 * @param i Event type to attach
	 * @param mod Module to attach event to
	 * @return True if the event was attached
	 */
	static bool Attach(Implementation i, Module *mod);

	/** Detatch an event from a module.
	 * This is not required when your module unloads, as the core will automatically detatch your module from all events it is attached to.
	 * @param i Event type to detach
	 * @param mod Module to detach event from
	 * @param Detach true if the event was detached
	 */
	static bool Detach(Implementation i, Module *mod);

	/** Detach all events from a module (used on unload)
	 * @param mod Module to detach from
	 */
	static void DetachAll(Module *mod);

	/** Attach an array of events to a module
	 * @param i Event types (array) to attach
	 * @param mod Module to attach events to
	 */
	static void Attach(Implementation *i, Module *mod, size_t sz);

	/** Delete all callbacks attached to a module
	 * @param m The module
	 */
	static void ClearCallBacks(Module *m);

	/** Unloading all modules, NEVER call this when Anope isn't shutting down.
	 * Ever.
	 */
	static void UnloadAll();

	/** Register a service
	 * @param s The service
	 * @return true if it was successfully registeed, else false (service name colision)
	 */
	static bool RegisterService(Service *s);

	/** Unregister a service
	 * @param s The service
	 * @return true if it was unregistered successfully
	 */
	static bool UnregisterService(Service *s);

	/** Get a service
	 * @param name The service name
	 * @return The services, or NULL
	 */
	static Service *GetService(const Anope::string &name);

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
	CallBack(Module *mod, long time_from_now, time_t now = Anope::CurTime, bool repeating = false) : Timer(time_from_now, now, repeating),  m(mod)
	{
		m->CallBacks.push_back(this);
	}

	virtual ~CallBack()
	{
		std::list<CallBack *>::iterator it = std::find(m->CallBacks.begin(), m->CallBacks.end(), this);
		if (it != m->CallBacks.end())
			m->CallBacks.erase(it);
	}
};

class CoreExport Service : public Base
{
 public:
	Module *owner;
	Anope::string name;

	Service(Module *o, const Anope::string &n);

	virtual ~Service();
};

template<typename T>
class service_reference : public dynamic_reference<T>
{
	Anope::string name;

 public:
	service_reference(const Anope::string &n) : dynamic_reference<T>(static_cast<T *>(ModuleManager::GetService(n))), name(n)
	{
	}

	virtual ~service_reference()
	{
	}

	operator bool()
	{
		if (this->invalid)
		{
			this->invalid = false;
			this->ref = NULL;
		}
		if (!this->ref)
		{
			this->ref = static_cast<T *>(ModuleManager::GetService(this->name));
			if (this->ref)
				this->ref->AddReference(this);
		}
		return this->ref;
	}
};

class CoreExport Message
{
public:
	Anope::string name;
	bool (*func)(const Anope::string &source, const std::vector<Anope::string> &params);

	Message(const Anope::string &n, bool (*f)(const Anope::string &, const std::vector<Anope::string> &));
	~Message();
};

#endif // MODULES_H
