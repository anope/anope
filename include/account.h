/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "extensible.h"
#include "serialize.h"
#include "anope.h"
#include "memo.h"
#include "base.h"

typedef Anope::insensitive_map<NickAlias *> nickalias_map;
typedef Anope::insensitive_map<NickCore *> nickcore_map;

extern CoreExport serialize_checker<nickalias_map> NickAliasList;
extern CoreExport serialize_checker<nickcore_map> NickCoreList;

/* NickServ nickname structures. */

/** Flags set on NickAliases
 */
enum NickNameFlag
{
	NS_BEGIN,

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
	"BEGIN", "NO_EXPIRE", "HELD", "COLLIDED", ""
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
	/* Chanstats are enabled for this user */
	NI_STATS,

	NI_END
};

const Anope::string NickCoreFlagStrings[] = {
	"BEGIN", "KILLPROTECT", "SECURE", "MSG", "MEMO_HARDMAX", "MEMO_SIGNON", "MEMO_RECEIVE",
	"PRIVATE", "HIDE_EMAIL", "HIDE_MASK", "HIDE_QUIT", "KILL_QUICK", "KILL_IMMED",
	"MEMO_MAIL", "HIDE_STATUS", "SUSPENDED", "AUTOOP", "FORBIDDEN", "UNCONFIRMED", "STATS", ""
};

/* It matters that Base is here before Extensible (it is inherited by Serializable) */
class CoreExport NickAlias : public Serializable, public Extensible, public Flags<NickNameFlag, NS_END>
{
	Anope::string vhost_ident, vhost_host, vhost_creator;
	time_t vhost_created;

 public:
 	/** Default constructor
	 * @param nickname The nick
	 * @param nickcore The nickcore for this nick
	 */
	NickAlias(const Anope::string &nickname, NickCore *nickcore);

	/** Default destructor
	 */
	~NickAlias();

	Anope::string nick;				/* Nickname */
	Anope::string last_quit;		/* Last quit message */
	Anope::string last_realname;	/* Last realname */
	Anope::string last_usermask;	/* Last usermask */
	Anope::string last_realhost;	/* Last uncloaked usermask, requires nickserv/auspex to see */
	time_t time_registered;			/* When the nick was registered */
	time_t last_seen;				/* When it was seen online for the last time */
	serialize_obj<NickCore> nc;					/* I'm an alias of this */

	Serialize::Data serialize() const anope_override;
	static Serializable* unserialize(Serializable *obj, Serialize::Data &);

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

 	/** Set a vhost for the user
	 * @param ident The ident
	 * @param host The host
	 * @param creator Who created the vhost
	 * @param time When the vhost was craated
	 */
	void SetVhost(const Anope::string &ident, const Anope::string &host, const Anope::string &creator, time_t created = Anope::CurTime);

	/** Remove a users vhost
	 **/
	void RemoveVhost();

	/** Check if the user has a vhost
	 * @return true or false
	 */
	bool HasVhost() const;

	/** Retrieve the vhost ident
	 * @return the ident
	 */
	const Anope::string &GetVhostIdent() const;

	/** Retrieve the vhost host
	 * @return the host
	 */
	const Anope::string &GetVhostHost() const;

	/** Retrieve the vhost creator
	 * @return the creator
	 */
	const Anope::string &GetVhostCreator() const;

	/** Retrieve when the vhost was created
	 * @return the time it was created
	 */
	time_t GetVhostCreated() const;
};

/* It matters that Base is here before Extensible (it is inherited by Serializable) */
class CoreExport NickCore : public Serializable, public Extensible, public Flags<NickCoreFlag, NI_END>
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

	Oper *o;

	/* Unsaved data */
	uint16_t channelcount; /* Number of channels currently registered */
	time_t lastmail;				/* Last time this nick record got a mail */
	std::list<serialize_obj<NickAlias> > aliases;	/* List of aliases */

	Serialize::Data serialize() const anope_override;
	static Serializable* unserialize(Serializable *obj, Serialize::Data &);

	/** Checks whether this account is a services oper or not.
	 * @return True if this account is a services oper, false otherwise.
	 */
	virtual bool IsServicesOper() const;

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
	bool FindCert(const Anope::string &entry) const;

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

class IdentifyRequest
{
	Module *owner;
	Anope::string account;
	Anope::string password;

	std::set<Module *> holds;
	bool dispatched;
	bool success;
	
	static std::set<IdentifyRequest *> requests;

 protected:
	IdentifyRequest(Module *o, const Anope::string &acc, const Anope::string &pass);
	virtual ~IdentifyRequest();

 public:
	virtual void OnSuccess() = 0;
	virtual void OnFail() = 0;

	const Anope::string &GetAccount() const { return account; }
	const Anope::string &GetPassword() const { return password; }

	/* Hold this request. Once held it must be Release()d later on */
	void Hold(Module *m);
	void Release(Module *m);

	void Success(Module *m);

	void Dispatch();

	static void ModuleUnload(Module *m);
};

extern CoreExport void change_core_display(NickCore *nc);
extern CoreExport void change_core_display(NickCore *nc, const Anope::string &newdisplay);

extern CoreExport NickAlias *findnick(const Anope::string &nick);
extern CoreExport NickCore *findcore(const Anope::string &nick);
extern CoreExport bool is_on_access(const User *u, const NickCore *nc);

#endif // ACCOUNT_H
