/* Mode support
 *
 * Copyright (C) 2008-2010 Adam <Adam@anope.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 *
 *
 */

/** All of the valid user mode names
 */
enum UserModeName
{
	UMODE_BEGIN,
	
	UMODE_SERV_ADMIN, UMODE_BOT, UMODE_CO_ADMIN, UMODE_FILTER, UMODE_HIDEOPER, UMODE_NETADMIN,
	UMODE_REGPRIV, UMODE_PROTECTED, UMODE_NO_CTCP, UMODE_WEBTV, UMODE_WHOIS, UMODE_ADMIN, UMODE_DEAF,
	UMODE_GLOBOPS, UMODE_HELPOP, UMODE_INVIS, UMODE_OPER, UMODE_PRIV, UMODE_GOD, UMODE_REGISTERED,
	UMODE_SNOMASK, UMODE_VHOST, UMODE_WALLOPS, UMODE_CLOAK, UMODE_SSL, UMODE_CALLERID, UMODE_COMMONCHANS,
	UMODE_HIDDEN, UMODE_STRIPCOLOR,

	UMODE_END
};

/** All of the valid channel mode names
 */
enum ChannelModeName
{
	CMODE_BEGIN,

	/* Channel modes */
	CMODE_BLOCKCOLOR, CMODE_FLOOD, CMODE_INVITE, CMODE_KEY, CMODE_LIMIT, CMODE_MODERATED, CMODE_NOEXTERNAL,
	CMODE_PRIVATE, CMODE_REGISTERED, CMODE_SECRET, CMODE_TOPIC, CMODE_AUDITORIUM, CMODE_SSL, CMODE_ADMINONLY,
	CMODE_NOCTCP, CMODE_FILTER, CMODE_NOKNOCK, CMODE_REDIRECT, CMODE_REGMODERATED, CMODE_NONICK, CMODE_OPERONLY,
	CMODE_NOKICK, CMODE_REGISTEREDONLY, CMODE_STRIPCOLOR, CMODE_NONOTICE, CMODE_NOINVITE, CMODE_ALLINVITE,
	CMODE_BLOCKCAPS, CMODE_PERM, CMODE_NICKFLOOD, CMODE_JOINFLOOD, CMODE_DELAYEDJOIN, CMODE_NOREJOIN,

	/* b/e/I */
	CMODE_BAN, CMODE_EXCEPT,
	CMODE_INVITEOVERRIDE,

	/* v/h/o/a/q */
	CMODE_VOICE, CMODE_HALFOP, CMODE_OP,
	CMODE_PROTECT, CMODE_OWNER,

	CMODE_END
};

/** The different types of modes
*/
enum ModeType
{
	/* Regular mode */
	MODE_REGULAR,
	/* b/e/I */
	MODE_LIST,
	/* k/l etc */
	MODE_PARAM,
	/* v/h/o/a/q */
	MODE_STATUS
};

/* Classes of modes, Channel modes and User modes
 */
enum ModeClass
{
	/* Channel mode */
	MC_CHANNEL,
	/* User mode */
	MC_USER
};

/** This class is the basis of all modes in Anope
 */
class CoreExport Mode
{
 public:
  	/* Class of mode this is */
	ModeClass Class;
	/* The mode name, as a string */
	const std::string NameAsString;
	/* Mode char for this */
	char ModeChar;
	/* Type of mode this is */
	ModeType Type;

	/** Default constructor
	 * @param mClass The type of mode this is
	 * @param mNameAsString The mode name as a string
	 * @param modeChar The mode char
	 * @param type The mode type
	 */
	Mode(ModeClass mClass, const std::string &mNameAsString, char modeChar, ModeType type);

	/** Default destructor
	 */
	virtual ~Mode();
};

/** This class is a user mode, all user modes use this/inherit from this
 */
class CoreExport UserMode : public Mode
{
 public:
	/* Mode name */
	UserModeName Name;

	/** Default constructor
	 * @param nName The mode name
	 * @param mNameAsString The mode name as a string
	 * @param modeChar The mode char
	 */
	UserMode(UserModeName mName, const std::string &mNameAsString, char modeChar);

	/** Default destructor
	 */
	virtual ~UserMode();
};

class UserModeParam : public UserMode
{
 public:
 	/** Default constructor
	 * @param mName The mode name
	 * @param mNameAsString The mode name as a string
	 * @param modeChar The mode char
	 */
	UserModeParam(UserModeName mName, const std::string &mNameAsString, char modeChar);

	/** Check if the param is valid
	 * @param value The param
	 * @return true or false
	 */
	virtual bool IsValid(const std::string &value) { return true; }
};

/** This class is a channel mode, all channel modes use this/inherit from this
 */
class CoreExport ChannelMode : public Mode
{
 public:
	/* Mode name */
	ChannelModeName Name;

	/** Default constructor
	 * @param mName The mode name
	 * @param mNameAsString The mode name as a string
	 * @param modeChar The mode char
	 */
	ChannelMode(ChannelModeName mName, const std::string &mNameAsString, char modeChar);

	/** Default destructor
	 */
	virtual ~ChannelMode();

	/** Can a user set this mode, used for mlock
	 * NOTE: User CAN be NULL, this is for checking if it can be locked with defcon
	 * @param u The user, or NULL
	 */
	virtual bool CanSet(User *u) { return true; }
};


/** This is a mode for lists, eg b/e/I. These modes should inherit from this
 */
class CoreExport ChannelModeList : public ChannelMode
{
 public:
	/** Default constructor
	 * @param mName The mode name
	 * @param mNameAsString The mode name as a string
	 * @param modeChar The mode char
	 */
	ChannelModeList(ChannelModeName mName, const std::string &mNameAsString, char modeChar);

	/** Default destructor
	 */
	virtual ~ChannelModeList();

	/** Is the mask valid
	 * @param mask The mask
	 * @return true for yes, false for no
	 */
	virtual bool IsValid(const std::string &mask) { return true; }

	/** Add the mask to the channel, this should be overridden
	 * @param chan The channel
	 * @param mask The mask
	 */
	virtual void AddMask(Channel *chan, const char *mask) { }

	/** Delete the mask from the channel, this should be overridden
	 * @param chan The channel
	 * @param mask The mask
	 */
	virtual void DelMask(Channel *chan, const char *mask) { }

};

/** This is a mode with a paramater, eg +k/l. These modes should use/inherit from this
*/
class CoreExport ChannelModeParam : public ChannelMode
{
 public:
	/** Default constructor
	 * @param mName The mode name
	 * @param mNameAsString The mode name as a string
	 * @param modeChar The mode char
	 * @param MinusArg true if this mode sends no arg when unsetting
	 */
	ChannelModeParam(ChannelModeName mName, const std::string &mNameAsString, char modeChar, bool MinusArg = false);

	/** Default destructor
	 */
	virtual ~ChannelModeParam();

	/* Should we send an arg when unsetting this mode? */
	bool MinusNoArg;

	/** Is the param valid
	 * @param value The param
	 * @return true for yes, false for no
	 */
	virtual bool IsValid(const std::string &value) { return true; }
};

/** This is a mode that is a channel status, eg +v/h/o/a/q.
*/
class CoreExport ChannelModeStatus : public ChannelMode
{
 public:
	/* The symbol, eg @ % + */
	char Symbol;

	/** Default constructor
	 * @param mName The mode name
	 * @param mNameAsString The mode name as a string
	 * @param modeChar The mode char
	 * @param mSymbol The symbol for the mode, eg @ % +
	 */
	ChannelModeStatus(ChannelModeName mName, const std::string &mNameAsString, char modeChar, char mSymbol);

	/** Default destructor
	 */
	virtual ~ChannelModeStatus();
};

/** Channel mode +b
 */
class CoreExport ChannelModeBan : public ChannelModeList
{
 public:
	ChannelModeBan(char modeChar) : ChannelModeList(CMODE_BAN, "CMODE_BAN", modeChar) { }

	void AddMask(Channel *chan, const char *mask);

	void DelMask(Channel *chan, const char *mask);
};

/** Channel mode +e
 */
class CoreExport ChannelModeExcept : public ChannelModeList
{
 public:
	ChannelModeExcept(char modeChar) : ChannelModeList(CMODE_EXCEPT, "CMODE_EXCEPT", modeChar) { }

	void AddMask(Channel *chan, const char *mask);

	void DelMask(Channel *chan, const char *mask);
};

/** Channel mode +I
 */
class CoreExport ChannelModeInvite : public ChannelModeList
{
 public:
	ChannelModeInvite(char modeChar) : ChannelModeList(CMODE_INVITEOVERRIDE, "CMODE_INVITEOVERRIDE", modeChar) { }

	void AddMask(Channel *chan, const char *mask);

	void DelMask(Channel *chan, const char *mask);
};


/** Channel mode +k (key)
 */
class CoreExport ChannelModeKey : public ChannelModeParam
{
 public:
	ChannelModeKey(char modeChar) : ChannelModeParam(CMODE_KEY, "CMODE_KEY", modeChar) { }

	bool IsValid(const std::string &value);
};

/** Channel mode +f (flood)
 */
class ChannelModeFlood : public ChannelModeParam
{
 public:
	ChannelModeFlood(char modeChar, bool minusNoArg = false) : ChannelModeParam(CMODE_FLOOD, "CMODE_FLOOD", modeChar, minusNoArg) { }

	bool IsValid(const std::string &value);
};

/** This class is used for channel mode +A (Admin only)
 * Only opers can mlock it
 */
class CoreExport ChannelModeAdmin : public ChannelMode
{
 public:
	ChannelModeAdmin(char modeChar) : ChannelMode(CMODE_ADMINONLY, "CMODE_ADMINONLY", modeChar) { }

	/* Opers only */
	bool CanSet(User *u);
};

/** This class is used for channel mode +O (Opers only)
 * Only opers can mlock it
 */
class CoreExport ChannelModeOper : public ChannelMode
{
 public:
	ChannelModeOper(char modeChar) : ChannelMode(CMODE_OPERONLY, "CMODE_OPERONLY", modeChar) { }

	/* Opers only */
	bool CanSet(User *u);
};
			       
/** This class is used for channel mode +r (registered channel)
 * No one may mlock r
 */
class CoreExport ChannelModeRegistered : public ChannelMode
{
 public:
	ChannelModeRegistered(char modeChar) : ChannelMode(CMODE_REGISTERED, "CMODE_REGISTERED", modeChar) { }

	/* No one mlocks +r */
	bool CanSet(User *u);
};

enum StackerType
{
	ST_CHANNEL,
	ST_USER
};

class StackerInfo
{
 public:
 	/* Modes to be added */
	std::list<std::pair<void *, std::string> > AddModes;
	/* Modes to be deleted */
	std::list<std::pair<void *, std::string> > DelModes;
	/* The type of object this stacker info is for */
	StackerType Type;
	/* Bot this is sent from */
	BotInfo *bi;

	/** Add a mode to this object
	 * @param Mode The mode
	 * @param Set true if setting, false if unsetting
	 * @param Param The param for the mode
	 */
	void AddMode(void *Mode, bool Set, const std::string &Param);
};

/** This is mode manager
 * It contains functions for adding modes to Anope so Anope can track them
 * and do things such as MLOCK.
 * This also contains a mode stacker that will combine multiple modes and set
 * them on a channel all at once
 */
class CoreExport ModeManager
{
 protected:
	/* List of pairs of user/channels and their stacker info */
	static std::list<std::pair<void *, StackerInfo *> > StackerObjects;

	/** Get the stacker info for an item, if one doesnt exist it is created
	 * @param Item The user/channel etc
	 * @return The stacker info
	 */
	static StackerInfo *GetInfo(void *Item);

	/** Build a list of mode strings to send to the IRCd from the mode stacker
	 * @param info The stacker info for a channel or user
	 * @return a list of strings
	 */
	static std::list<std::string> BuildModeStrings(StackerInfo *info);

	/** Add a mode to the stacker, internal use only
	 * @param bi The client to set the modes from
	 * @param u The user
	 * @param um The user mode
	 * @param Set Adding or removing?
	 * @param Param A param, if there is one
	 */
	static void StackerAddInternal(BotInfo *bi, User *u, UserMode *um, bool Set, const std::string &Param);

	/** Add a mode to the stacker, internal use only
	 * @param bi The client to set the modes from
	 * @param c The channel
	 * @param cm The channel mode
	 * @param Set Adding or removing?
	 * @param Param A param, if there is one
	 */
	static void StackerAddInternal(BotInfo *bi, Channel *c, ChannelMode *cm, bool Set, const std::string &Param);

	/** Really add a mode to the stacker, internal use only
	 * @param bi The client to set the modes from
	 * @param Object The object, user/channel
	 * @param Mode The mode
	 * @param Set Adding or removing?
	 * @param Param A param, if there is one
	 * @param Type The type this is, user or channel
	 */
	static void StackerAddInternal(BotInfo *bi, void *Object, void *Mode, bool Set, const std::string &Param, StackerType Type);

 public:
	/* List of all modes Anope knows about */
	static std::list<Mode *> Modes;

        /* User modes */
        static std::map<char, UserMode *> UserModesByChar;
        static std::map<UserModeName, UserMode *> UserModesByName;
        /* Channel modes */
        static std::map<char, ChannelMode *> ChannelModesByChar;
        static std::map<ChannelModeName, ChannelMode *> ChannelModesByName;
        /* Although there are two different maps for UserModes and ChannelModes
         * the pointers in each are the same. This is used to increase efficiency.
	 */

        /** Add a user mode to Anope
         * @param um A UserMode or UserMode derived class
         * @return true on success, false on error
         */
        static bool AddUserMode(UserMode *um);

        /** Add a channel mode to Anope
         * @param cm A ChannelMode or ChannelMode derived class
         * @return true on success, false on error
         */
        static bool AddChannelMode(ChannelMode *cm);

        /** Find a channel mode
         * @param Mode The mode
         * @return The mode class
         */
        static ChannelMode *FindChannelModeByChar(char Mode);

        /** Find a user mode
         * @param Mode The mode
         * @return The mode class
         */
        static UserMode *FindUserModeByChar(char Mode);

        /** Find a channel mode
         * @param Mode The modename
         * @return The mode class
         */
        static ChannelMode *FindChannelModeByName(ChannelModeName Name);

        /** Find a user mode
         * @param Mode The modename
         * @return The mode class
         */
        static UserMode *FindUserModeByName(UserModeName Name);

        /** Gets the channel mode char for a symbol (eg + returns v)
         * @param Value The symbol
         * @return The char
         */
        static char GetStatusChar(char Value);

	/** Add a mode to the stacker to be set on a channel
	 * @param bi The client to set the modes from
	 * @param c The channel
	 * @param cm The channel mode
	 * @param Set true for setting, false for removing
	 * @param Param The param, if there is one
	 */
        static void StackerAdd(BotInfo *bi, Channel *c, ChannelMode *cm, bool Set, const std::string &Param = "");

	/** Add a mode to the stacker to be set on a channel
	 * @param bi The client to set the modes from
	 * @param c The channel
	 * @param Name The channel mode name
	 * @param Set true for setting, false for removing
	 * @param Param The param, if there is one
	 */
        static void StackerAdd(BotInfo *bi, Channel *c, ChannelModeName Name, bool Set, const std::string &Param = "");

	/** Add a mode to the stacker to be set on a channel
	 * @param bi The client to set the modes from
	 * @param c The channel
	 * @param Mode The mode char
	 * @param Set true for setting, false for removing
	 * @param Param The param, if there is one
	 */
        static void StackerAdd(BotInfo *bi, Channel *c, const char Mode, bool Set, const std::string &Param = "");

	/** Add a mode to the stacker to be set on a user
	 * @param bi The client to set the modes from
	 * @param u The user
	 * @param um The user mode
	 * @param Set true for setting, false for removing
	 * @param param The param, if there is one
	 */
	static void StackerAdd(BotInfo *bi, User *u, UserMode *um, bool Set, const std::string &Param = "");

	/** Add a mode to the stacker to be set on a user
	 * @param bi The client to set the modes from
	 * @param u The user
	 * @param Name The user mode name
	 * @param Set true for setting, false for removing
	 * @param Param The param, if there is one
	 */
	static void StackerAdd(BotInfo *bi, User *u, UserModeName Name, bool Set, const std::string &Param = "");

	/** Add a mode to the stacker to be set on a user
	 * @param bi The client to set the modes from
	 * @param u The user
	 * @param Mode The mode to be set
	 * @param Set true for setting, false for removing
	 * @param Param The param, if there is one
	 */
	static void StackerAdd(BotInfo *bi, User *u, const char Mode, bool Set, const std::string &Param = "");

	/** Process all of the modes in the stacker and send them to the IRCd to be set on channels/users
	 */
	static void ProcessModes();
};

