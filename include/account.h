
/* NickServ nickname structures. */

/** Flags set on NickAliases
 */
enum NickNameFlag
{
	NS_BEGIN,

	/* Nick may not be registered or used */
	NS_FORBIDDEN,
	/* Nick never expires */
	NS_NO_EXPIRE,
	/* Nick is being held after a kill */
	NS_KILL_HELD,
	/* SVSNICK has been sent but nick has not yet changed.
	 * An enforcer will be introduced when it does change. */
	NS_GUESTED,

	NS_END
};

/** Flags set on NickCores
 */
enum NickCoreFlag
{
	NI_BEGIN,

	/* Kill others who take this nick */
	NI_KILLPROTECT,
	/* Dont recognize unless IDENTIFIED */
	NI_SECURE,
	/* Use PRIVMSG instead of NOTICE */
	NI_MSG,
	/* Don't allow user to change memo limit */
	NI_MEMO_HARDMAX,
	/* Notify of memos at signon and un-away */
	NI_MEMO_SIGNON,
	/* Notify of new memos when sent */
	NI_MEMO_RECEIVE,
	/* Don't show in LIST to non-servadmins */
	NI_PRIVATE,
	/* Don't show email in INFO */
	NI_HIDE_EMAIL,
	/* Don't show last seen address in INFO */
	NI_HIDE_MASK,
	/* Don't show last quit message in INFO */
	NI_HIDE_QUIT,
	/* Kill in 20 seconds instead of in 60 */
	NI_KILL_QUICK,
	/* Kill immediatly */
	NI_KILL_IMMED,
	/* User gets email on memo */
	NI_MEMO_MAIL,
	/* Don't show services access status */
	NI_HIDE_STATUS,
	/* Nickname is suspended */
	NI_SUSPENDED,
	/* Autoop nickname in channels */
	NI_AUTOOP,
	/* This nickcore is forbidden, which means the nickalias for it is aswell */
	NI_FORBIDDEN,

	NI_END
};

/** XXX: this really needs to die with fire and be merged with metadata into NickCore or something.
 */
class CoreExport NickRequest
{
 public:
	NickRequest(const std::string &nickname);

	~NickRequest();

	NickRequest *next, *prev;
	char *nick;
	char *passcode;
	char password[PASSMAX];
	char *email;
	time_t requested;
	time_t lastmail;			/* Unsaved */
};

class NickCore;

class CoreExport NickAlias : public Extensible, public Flags<NickNameFlag>
{
 public:
 	/** Default constructor
	 * @param nickname The nick
	 * @param nickcore The nickcofe for this nick
	 */
	NickAlias(const std::string &nickname, NickCore *nickcore);

	/** Default destructor
	 */
	~NickAlias();

	NickAlias *next, *prev;
	char *nick;				/* Nickname */
	char *last_quit;			/* Last quit message */
	char *last_realname;			/* Last realname */
	char *last_usermask;			/* Last usermask */
	time_t time_registered;			/* When the nick was registered */
	time_t last_seen;			/* When it was seen online for the last time */
	NickCore *nc;				/* I'm an alias of this */
};

class CoreExport NickCore : public Extensible, public Flags<NickCoreFlag>
{
 public:
 	/** Default constructor
	 * @param display The display nick
	 */
	NickCore(const std::string &nickdisplay);

	/** Default destructor
	 */
	~NickCore();

	NickCore *next, *prev;

	char *display;				/* How the nick is displayed */
	char pass[PASSMAX];			/* Password of the nicks */
	char *email;				/* E-mail associated to the nick */
	char *greet;				/* Greet associated to the nick */
	uint32 icq;				/* ICQ # associated to the nick */
	char *url;				/* URL associated to the nick */
	uint16 language;			/* Language selected by nickname owner (LANG_*) */
	std::vector<std::string> access; 	/* Access list, vector of strings */
	MemoInfo memos;
	uint16 channelcount;			/* Number of channels currently registered */

	OperType *ot;

	/* Unsaved data */
	time_t lastmail;			/* Last time this nick record got a mail */
	SList aliases;				/* List of aliases */

	/** Check whether this opertype has access to run the given command string.
	  * @param cmdstr The string to check, e.g. botserv/set/private.
	  * @return True if this opertype may run the specified command, false otherwise.
	  */
	bool HasCommand(const std::string &cmdstr) const;

	/** Checks whether this account is a services oper or not.
	 * @return True if this account is a services oper, false otherwise.
	 */
	bool IsServicesOper() const;

	/** Check whether this opertype has access to the given special permission.
	  * @param privstr The priv to check for, e.g. users/auspex.
	  * @return True if this opertype has the specified priv, false otherwise.
	  */
	bool HasPriv(const std::string &privstr) const;

	/** Add an entry to the nick's access list
	 *
	 * @param entry The nick!ident@host entry to add to the access list
	 *
	 * Adds a new entry into the access list.
	 */
	void AddAccess(const std::string &entry);

	/** Get an entry from the nick's access list by index
	 *
	 * @param entry Index in the access list vector to retrieve
	 * @return The access list entry of the given index if within bounds, an empty string if the vector is empty or the index is out of bounds
	 *
	 * Retrieves an entry from the access list corresponding to the given index.
	 */
	std::string GetAccess(unsigned entry);

	/** Find an entry in the nick's access list
	 *
	 * @param entry The nick!ident@host entry to search for
	 * @return True if the entry is found in the access list, false otherwise
	 *
	 * Search for an entry within the access list.
	 */
	bool FindAccess(const std::string &entry);

	/** Erase an entry from the nick's access list
	 *
	 * @param entry The nick!ident@host entry to remove
	 *
	 * Removes the specified access list entry from the access list.
	 */
	void EraseAccess(const std::string &entry);

	/** Clears the entire nick's access list
	 *
	 * Deletes all the memory allocated in the access list vector and then clears the vector.
	 */
	void ClearAccess();
};

