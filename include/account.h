/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

#include "extensible.h"
#include "serialize.h"
#include "anope.h"
#include "memo.h"
#include "base.h"

typedef Anope::unordered_map<NickAlias *> nickalias_map;
typedef Anope::unordered_map<NickCore *> nickcore_map;
typedef std::unordered_map<uint64_t, NickCore *> nickcoreid_map;

extern CoreExport Serialize::Checker<nickalias_map> NickAliasList;
extern CoreExport Serialize::Checker<nickcore_map> NickCoreList;
extern CoreExport Serialize::Checker<nickcoreid_map> NickCoreIdList;

/* A registered nickname.
 * It matters that Base is here before Extensible (it is inherited by Serializable)
 */
class CoreExport NickAlias final
	: public Serializable
	, public Extensible
{
public:
	struct Type final
		: public Serialize::Type
	{
		Type();
		void Serialize(Serializable *obj, Serialize::Data &data) const override;
		Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override;
	};

private:
	Anope::string vhost_ident, vhost_host, vhost_creator;
	time_t vhost_created = 0;

public:
	Anope::string nick;
	Anope::string last_quit;
	Anope::string last_realname;
	/* Last usermask this nick was seen on, eg user@host */
	Anope::string last_usermask;
	/* Last uncloaked usermask, requires nickserv/auspex to see */
	Anope::string last_realhost;
	time_t registered = Anope::CurTime;
	time_t last_seen = Anope::CurTime;

	/* Account this nick is tied to. Multiple nicks can be tied to a single account. */
	Serialize::Reference<NickCore> nc;

	/** Constructor
	 * @param nickname The nick
	 * @param nickcore The nickcore for this nick
	 */
	NickAlias(const Anope::string &nickname, NickCore *nickcore);
	~NickAlias();

	/** Set a vhost for the user
	 * @param ident The ident
	 * @param host The host
	 * @param creator Who created the vhost
	 * @param time When the vhost was created
	 */
	void SetVHost(const Anope::string &ident, const Anope::string &host, const Anope::string &creator, time_t created = Anope::CurTime);

	/** Remove a users vhost
	 **/
	void RemoveVHost();

	/** Check if the user has a vhost
	 * @return true or false
	 */
	bool HasVHost() const;

	/** Retrieve the vhost ident
	 * @return the ident
	 */
	const Anope::string &GetVHostIdent() const;

	/** Retrieve the vhost host
	 * @return the host
	 */
	const Anope::string &GetVHostHost() const;

	/** Retrieve the vhost mask
	 * @param the mask
	 */
	Anope::string GetVHostMask() const;

	/** Retrieve the vhost creator
	 * @return the creator
	 */
	const Anope::string &GetVHostCreator() const;

	/** Retrieve when the vhost was created
	 * @return the time it was created
	 */
	time_t GetVHostCreated() const;

	/** Finds a registered nick
	 * @param nick The nick to lookup
	 * @return the nick, if found
	 */
	static NickAlias *Find(const Anope::string &nick);
	static NickAlias *FindId(uint64_t uid);
};

/* A registered account. Each account must have a NickAlias with the same nick as the
 * account's display.
 * It matters that Base is here before Extensible (it is inherited by Serializable)
 */
class CoreExport NickCore final
	: public Serializable
	, public Extensible
{
public:
	struct Type final
		: public Serialize::Type
	{
		Type();
		void Serialize(Serializable *obj, Serialize::Data &data) const override;
		Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override;
	};

private:
	/* Channels which reference this core in some way (this is on their access list, akick list, is founder, successor, etc) */
	Serialize::Checker<std::map<ChannelInfo *, int> > chanaccess;
	/* Unique identifier for the account. */
	uint64_t uniqueid;
public:
	/* Name of the account. Find(display)->nc == this. */
	Anope::string display;
	/* User password in form of hashm:data */
	Anope::string pass;
	Anope::string email;
	/* Locale name of the language of the user. Empty means default language */
	Anope::string language;
	/* Last time an email was sent to this user */
	time_t lastmail = 0;
	/* The time this account was registered */
	time_t registered = Anope::CurTime;
	MemoInfo memos;
	std::map<Anope::string, ModeData> last_modes;

	/* Nicknames registered that are grouped to this account.
	 * for n in aliases, n->nc == this.
	 */
	Serialize::Checker<std::vector<NickAlias *> > aliases;

	/* Set if this user is a services operator. o->ot must exist. */
	Oper *o = nullptr;

	/* Unsaved data */

	/** The display nick for this account. */
	NickAlias *na = nullptr;
	/* Number of channels registered by this account */
	uint16_t channelcount = 0;
	/* Users online now logged into this account */
	std::list<User *> users;

	/** Constructor
	 * @param display The display nick
	 * @param id The account id
	 */
	NickCore(const Anope::string &nickdisplay, uint64_t nickid = 0);
	~NickCore();

	/** Changes the display for this account
	 * @param na The new display, must be grouped to this account.
	 */
	void SetDisplay(NickAlias *na);

	/** Checks whether this account is a services oper or not.
	 * @return True if this account is a services oper, false otherwise.
	 */
	virtual bool IsServicesOper() const;

	/** Retrieves the account id for this user */
	uint64_t GetId();

	/** Finds an account
	 * @param nick The account name to find
	 * @return The account, if it exists
	 */
	static NickCore *Find(const Anope::string &nick);
	static NickCore *FindId(uint64_t uid);

	void AddChannelReference(ChannelInfo *ci);
	void RemoveChannelReference(ChannelInfo *ci);
	void GetChannelReferences(std::deque<ChannelInfo *> &queue);
};

/* A request to check if an account/password is valid. These can exist for
 * extended periods due to the time some authentication modules take.
 */
class CoreExport IdentifyRequest
{
	/* Owner of this request, used to cleanup requests if a module is unloaded
	 * while a request us pending */
	Module *owner;
	Anope::string account;
	Anope::string password;
	Anope::string address;

	std::set<Module *> holds;
	bool dispatched = false;
	bool success = false;

	static std::set<IdentifyRequest *> Requests;

protected:
	IdentifyRequest(Module *o, const Anope::string &acc, const Anope::string &pass, const Anope::string &ip);
	virtual ~IdentifyRequest();

public:
	/* One of these is called when the request goes through */
	virtual void OnSuccess() = 0;
	virtual void OnFail() = 0;

	Module *GetOwner() const { return owner; }
	const Anope::string &GetAccount() const { return account; }
	const Anope::string &GetPassword() const { return password; }
	const Anope::string &GetAddress() const { return address; }

	/* Holds this request. When a request is held it must be Released later
	 * for the request to complete. Multiple modules may hold a request at any time,
	 * but the request is not complete until every module has released it. If you do not
	 * require holding this (eg, your password check is done in this thread and immediately)
	 * then you don't need to hold the request before calling `Success()`.
	 * @param m The module holding this request
	 */
	void Hold(Module *m);

	/** Releases a held request
	 * @param m The module releasing the hold
	 */
	void Release(Module *m);

	/** Called by modules when this IdentifyRequest has succeeded.
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
