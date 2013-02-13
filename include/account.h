/*
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 * 
 */

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "extensible.h"
#include "serialize.h"
#include "anope.h"
#include "memo.h"
#include "base.h"

typedef Anope::hash_map<NickAlias *> nickalias_map;
typedef Anope::hash_map<NickCore *> nickcore_map;

extern CoreExport Serialize::Checker<nickalias_map> NickAliasList;
extern CoreExport Serialize::Checker<nickcore_map> NickCoreList;

/* A registered nickname.
 * It matters that Base is here before Extensible (it is inherited by Serializable) 
 *
 * Possible flags:
 *     NO_EXPIRE - Nick never expires
 *     HELD      - This nick is being held after a kill by an enforcer client
 *                    or is being SVSHeld.
 *     COLLIDED  - We are taking over this nick, either by SVSNICK or KILL
 *                    and are waiting for the confirmation of either of these actions to
 *                    proceed. This is checked in NickAlias::OnCancel
 *
 */
class CoreExport NickAlias : public Serializable, public Extensible
{
	Anope::string vhost_ident, vhost_host, vhost_creator;
	time_t vhost_created;

 public:
	Anope::string nick;
	Anope::string last_quit;
	Anope::string last_realname;
	/* Last usermask this nick was seen on, eg user@host */
	Anope::string last_usermask;
	/* Last uncloaked usermask, requires nickserv/auspex to see */
	Anope::string last_realhost;
	time_t time_registered;
	time_t last_seen;
	/* Account this nick is tied to. Multiple nicks can be tied to a single account. */
	Serialize::Reference<NickCore> nc;

 	/** Constructor
	 * @param nickname The nick
	 * @param nickcore The nickcore for this nick
	 */
	NickAlias(const Anope::string &nickname, NickCore *nickcore);
	~NickAlias();

	void Serialize(Serialize::Data &data) const anope_override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &);

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

	/** Finds a registered nick
	 * @param nick The nick to lookup
	 * @return the nick, if found
	 */
	static NickAlias *Find(const Anope::string &nick);
};

/* A registered account. Each account must have a NickAlias with the same nick as the
 * account's display.
 * It matters that Base is here before Extensible (it is inherited by Serializable)
 *
 * Possible flags:
 *     KILLPROTECT   - Kill other users who try to take this nick
 *     SECURE        - Don't recognize unless identified
 *     MSG           - Use PRIVMSG instead of notice
 *     MEMO_HARDMAX  - Don't allow user to change memo limit
 *     MEMO_SIGNON   - Notify of memos at signon and unaway
 *     MEMO_RECEIEVE - Notify of new memos when sent
 *     PRIVATE       - Don't show in LIST to non-servadmins
 *     HIDE_EMAIL    - Don't show email in INFO
 *     HIDE_MASK     - Don't show last seen address in INFO
 *     HIDE_QUIT     - Don't show last quit message in INFO
 *     KILL_QUICK    - Kill quicker
 *     KILL_IMMED    - Kill immediately
 *     MEMO_MAIL     - User gets email on memo
 *     HIDE_STATUS   - Don't show services access status
 *     SUSPEND       - Nickname is suspended
 *     AUTOOP        - Autoop nickname in channels
 *     UNCONFIRMED   - Account has not had email address confirmed
 *     STATS         - ChanStats is enabled for this user
 */
class CoreExport NickCore : public Serializable, public Extensible
{
 public:
 	/* Name of the account. Find(display)->nc == this. */
	Anope::string display;
	/* User password in form of hashm:data */
	Anope::string pass;
	Anope::string email;
	/* Greet associated with the account, sometimes sent when the user joins a channel */
	Anope::string greet;
	/* Locale name of the language of the user. Empty means default language */
	Anope::string language;
	/* Access list, contains user@host masks of users who get certain privileges based
	 * on if NI_SECURE is set and what (if any) kill protection is enabled. */
	std::vector<Anope::string> access;
	/* SSL certificate list. Users who have a matching certificate may be automatically logged in */
	std::vector<Anope::string> cert;
	MemoInfo memos;

	/* Nicknames registered that are grouped to this account.
	 * for n in aliases, n->nc == this.
	 */
	Serialize::Checker<std::vector<NickAlias *> > aliases;

	/* Set if this user is a services operattor. o->ot must exist. */
	Oper *o;

	/* Unsaved data */

	/* Number of channels registered by this account */
	uint16_t channelcount;
	/* Last time an email was sent to this user */
	time_t lastmail;
	/* Users online now logged into this account */
	std::list<User *> users;

	/** Constructor
	 * @param display The display nick
	 */
	NickCore(const Anope::string &nickdisplay);
	~NickCore();

	void Serialize(Serialize::Data &data) const anope_override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &);

	/** Changes the display for this account
	 * @param na The new display, must be grouped to this account.
	 */
	void SetDisplay(const NickAlias *na);

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

	/** Is the given user on this accounts access list?
	 *
	 * @param u The user
	 *
	 * @return true if the user is on the access list
	 */
	bool IsOnAccess(const User *u) const;

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

	/** Finds an account
	 * @param nick The account name to find
	 * @return The account, if it exists
	 */
	static NickCore* Find(const Anope::string &nick);
};

/* A request to check if an account/password is valid. These can exist for
 * extended periods of time due to some authentication modules take.
 */
class CoreExport IdentifyRequest
{
	/* Owner of this request, used to cleanup requests if a module is unloaded
	 * while a reqyest us pending */
	Module *owner;
	Anope::string account;
	Anope::string password;

	std::set<Module *> holds;
	bool dispatched;
	bool success;
	
	static std::set<IdentifyRequest *> Requests;

 protected:
	IdentifyRequest(Module *o, const Anope::string &acc, const Anope::string &pass);
	virtual ~IdentifyRequest();

 public:
 	/* One of these is called when the request goes through */
	virtual void OnSuccess() = 0;
	virtual void OnFail() = 0;

	const Anope::string &GetAccount() const { return account; }
	const Anope::string &GetPassword() const { return password; }

	/* Holds this request. When a request is held it must be Released later
	 * for the request to complete. Multiple modules may hold a request at any time,
	 * but the request is not complete until every module has released it. If you do not
	 * require holding this (eg, your password check is done in this thread and immediately)
	 * then you don't need to hold the request before Successing it.
	 * @param m The module holding this request
	 */
	void Hold(Module *m);

	/** Releases a held request
	 * @param m The module releaseing the hold
	 */
	void Release(Module *m);

	/** Called by modules when this IdentifyRequest has successeded successfully.
	 * If this request is behind held it must still be Released after calling this.
	 * @param m The module confirming authentication
	 */
	void Success(Module *m);

	/** Used to either finalize this request or marks
	 * it as dispatched and begins waiting for the module(s)
	 * that have holds to finish.
	 */
	void Dispatch();

	static void ModuleUnload(Module *m);
};

#endif // ACCOUNT_H
