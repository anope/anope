#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "anope.h"

class NickAlias;
class NickCore;

typedef unordered_map_namespace::unordered_map<Anope::string, NickAlias *, ci::hash, std::equal_to<ci::string> > nickalias_map;
typedef unordered_map_namespace::unordered_map<Anope::string, NickCore *, ci::hash, std::equal_to<ci::string> > nickcore_map;

extern CoreExport nickalias_map NickAliasList;
extern CoreExport nickcore_map NickCoreList;

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
	/* This nick is being held after a kill by an enforcer client
	 * or is being SVSHeld. Used by ns_release to determin if something
	 * should be allowed to be released
	 */
	NS_HELD,
	/* We are taking over this nick, either by SVSNICK or KILL.
	 * We are waiting for the confirmation of either of these actions to
	 * proceed. This is checked in NickAlias::OnCancel
	 */
	NS_COLLIDED,

	NS_END
};

const Anope::string NickNameFlagStrings[] = {
	"BEGIN", "FORBIDDEN", "NO_EXPIRE", "HELD", "COLLIDED", ""
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
	/* If set means the nick core does not have their email addrses confirmed.
	 */
	NI_UNCONFIRMED,

	NI_END
};

const Anope::string NickCoreFlagStrings[] = {
	"BEGIN", "KILLPROTECT", "SECURE", "MSG", "MEMO_HARDMAX", "MEMO_SIGNON", "MEMO_RECEIVE",
	"PRIVATE", "HIDE_EMAIL", "HIDE_MASK", "HIDE_QUIT", "KILL_QUICK", "KILL_IMMED",
	"MEMO_MAIL", "HIDE_STATUS", "SUSPENDED", "AUTOOP", "FORBIDDEN", "UNCONFIRMED", ""
};

class NickCore;

class CoreExport NickAlias : public Extensible, public Flags<NickNameFlag, NS_END>
{
 public:
 	/** Default constructor
	 * @param nickname The nick
	 * @param nickcore The nickcofe for this nick
	 */
	NickAlias(const Anope::string &nickname, NickCore *nickcore);

	/** Default destructor
	 */
	~NickAlias();

	Anope::string nick;				/* Nickname */
	Anope::string last_quit;		/* Last quit message */
	Anope::string last_realname;	/* Last realname */
	Anope::string last_usermask;	/* Last usermask */
	time_t time_registered;			/* When the nick was registered */
	time_t last_seen;				/* When it was seen online for the last time */
	NickCore *nc;					/* I'm an alias of this */
	HostInfo hostinfo;

	/** Release a nick
	 * See the comment in users.cpp
	 */
	void Release();

	/** This function is called when a user on this nick either disconnects or changes nick.
	 * Note that the user isnt necessarially identified to this nick
	 * See the comment in users.cpp
	 * @param u The user
	 */
	void OnCancel(User *u);
};

class CoreExport NickCore : public Extensible, public Flags<NickCoreFlag, NI_END>
{
 public:
	/** Default constructor
	 * @param display The display nick
	 */
	NickCore(const Anope::string &nickdisplay);

	/** Default destructor
	 */
	~NickCore();

	std::list<User *> Users;

	Anope::string display;	/* How the nick is displayed */
	Anope::string pass;		/* Password of the nicks */
	Anope::string email;	/* E-mail associated to the nick */
	Anope::string greet;	/* Greet associated to the nick */
	Anope::string language;	/* Language name */
	std::vector<Anope::string> access; /* Access list, vector of strings */
	std::vector<Anope::string> cert; /* ssl certificate list, vector of strings */
	MemoInfo memos;
	uint16 channelcount; /* Number of channels currently registered */

	OperType *ot;

	/* Unsaved data */
	time_t lastmail;				/* Last time this nick record got a mail */
	std::list<NickAlias *> aliases;	/* List of aliases */

	/** Check whether this opertype has access to run the given command string.
	  * @param cmdstr The string to check, e.g. botserv/set/private.
	  * @return True if this opertype may run the specified command, false otherwise.
	  */
	virtual bool HasCommand(const Anope::string &cmdstr) const;

	/** Checks whether this account is a services oper or not.
	 * @return True if this account is a services oper, false otherwise.
	 */
	virtual bool IsServicesOper() const;

	/** Check whether this opertype has access to the given special permission.
	  * @param privstr The priv to check for, e.g. users/auspex.
	  * @return True if this opertype has the specified priv, false otherwise.
	  */
	virtual bool HasPriv(const Anope::string &privstr) const;

	/** Add an entry to the nick's access list
	 *
	 * @param entry The nick!ident@host entry to add to the access list
	 *
	 * Adds a new entry into the access list.
	 */
	void AddAccess(const Anope::string &entry);

	/** Get an entry from the nick's access list by index
	 *
	 * @param entry Index in the access list vector to retrieve
	 * @return The access list entry of the given index if within bounds, an empty string if the vector is empty or the index is out of bounds
	 *
	 * Retrieves an entry from the access list corresponding to the given index.
	 */
	Anope::string GetAccess(unsigned entry) const;

	/** Find an entry in the nick's access list
	 *
	 * @param entry The nick!ident@host entry to search for
	 * @return True if the entry is found in the access list, false otherwise
	 *
	 * Search for an entry within the access list.
	 */
	bool FindAccess(const Anope::string &entry);

	/** Erase an entry from the nick's access list
	 *
	 * @param entry The nick!ident@host entry to remove
	 *
	 * Removes the specified access list entry from the access list.
	 */
	void EraseAccess(const Anope::string &entry);

	/** Clears the entire nick's access list
	 *
	 * Deletes all the memory allocated in the access list vector and then clears the vector.
	 */
	void ClearAccess();

	/** Add an entry to the nick's certificate list
	 *
	 * @param entry The fingerprint to add to the cert list
	 *
	 * Adds a new entry into the cert list.
	 */
	void AddCert(const Anope::string &entry);

	/** Get an entry from the nick's cert list by index
	 *
	 * @param entry Index in the certificaate list vector to retrieve
	 * @return The fingerprint entry of the given index if within bounds, an empty string if the vector is empty or the index is out of bounds
	 *
	 * Retrieves an entry from the certificate list corresponding to the given index.
	 */
	Anope::string GetCert(unsigned entry) const;

	/** Find an entry in the nick's cert list
	 *
	 * @param entry The fingerprint to search for
	 * @return True if the fingerprint is found in the cert list, false otherwise
	 *
	 * Search for an fingerprint within the cert list.
	 */
	bool FindCert(const Anope::string &entry);

	/** Erase a fingerprint from the nick's certificate list
	 *
	 * @param entry The fingerprint to remove
	 *
	 * Removes the specified fingerprint from the cert list.
	 */
	void EraseCert(const Anope::string &entry);

	/** Clears the entire nick's cert list
	 *
	 * Deletes all the memory allocated in the certificate list vector and then clears the vector.
	 */
	void ClearCert();

};

/** Timer for colliding nicks to force people off of nicknames
 */
class NickServCollide : public Timer
{
	dynamic_reference<User> u;
	Anope::string nick;

 public:
	/** Default constructor
	 * @param nick The nick we're colliding
	 * @param delay How long to delay before kicking the user off the nick
	 */
	NickServCollide(User *user, time_t delay);

	/** Default destructor
	 */
	virtual ~NickServCollide();

	/** Called when the delay is up
	 * @param t The current time
	 */
	void Tick(time_t t);
};

/** Timers for releasing nicks to be available for use
 */
class NickServRelease : public User, public Timer
{
	Anope::string nick;

 public:
	/** Default constructor
	 * @param na The nick
	 * @param delay The delay before the nick is released
	 */
	NickServRelease(NickAlias *na, time_t delay);

	/** Default destructor
	 */
	virtual ~NickServRelease();

	/** Called when the delay is up
	 * @param t The current time
	 */
	void Tick(time_t t);
};

#endif // ACCOUNT_H
