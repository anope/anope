/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#pragma once

#include "event.h"
#include "service.h"

namespace NickServ
{
	class Nick;
	class Account;
	class IdentifyRequestListener;

	typedef Anope::hash_map<Nick *> nickalias_map;
	typedef Anope::hash_map<Account *> nickcore_map;

	class NickServService : public Service
	{
	 public:
		NickServService(Module *m) : Service(m, "NickServService", "NickServ")
		{
		}

		virtual void Validate(User *u) anope_abstract;
		virtual void Collide(User *u, Nick *na) anope_abstract;
		virtual void Release(Nick *na) anope_abstract;

		virtual IdentifyRequest *CreateIdentifyRequest(IdentifyRequestListener *l, Module *owner, const Anope::string &acc, const Anope::string &pass) anope_abstract;
		virtual std::set<IdentifyRequest *>& GetIdentifyRequests() anope_abstract;

		virtual nickalias_map& GetNickList() anope_abstract;
		virtual nickcore_map& GetAccountList() anope_abstract;

		virtual Nick *CreateNick(const Anope::string &nick, Account *acc) anope_abstract;
		virtual Account *CreateAccount(const Anope::string &acc) anope_abstract;

		virtual Nick *FindNick(const Anope::string &nick) anope_abstract;
		virtual Account *FindAccount(const Anope::string &acc) anope_abstract;
	};
	static ServiceReference<NickServService> service("NickServService", "NickServ");

	inline Nick *FindNick(const Anope::string &nick)
	{
		return service ? service->FindNick(nick) : nullptr;
	}

	inline Account *FindAccount(const Anope::string &account)
	{
		return service ? service->FindAccount(account) : nullptr;
	}

	namespace Event
	{
		struct CoreExport PreNickExpire : Events
		{
			/** Called before a nick expires
			 * @param na The nick
			 * @param expire Set to true to allow the nick to expire
			 */
			virtual void OnPreNickExpire(Nick *na, bool &expire) anope_abstract;
		};
		static EventHandlersReference<PreNickExpire> OnPreNickExpire("OnPreNickExpire");

		struct CoreExport NickExpire : Events
		{
			/** Called when a nick drops
			 * @param na The nick
			 */
			virtual void OnNickExpire(Nick *na) anope_abstract;
		};
		static EventHandlersReference<NickExpire> OnNickExpire("OnNickExpire");

		struct CoreExport NickRegister : Events
		{
			/** Called when a nick is registered
			 * @param user The user registering the nick, of any
			 * @param The nick
			 */
			virtual void OnNickRegister(User *user, Nick *na) anope_abstract;
		};
		static EventHandlersReference<NickRegister> OnNickRegister("OnNickRegister");

		struct CoreExport NickValidate : Events
		{
			/** Called when a nick is validated. That is, to determine if a user is permissted
			 * to be on the given nick.
			 * @param u The user
			 * @param na The nick they are on
			 * @return EVENT_STOP to force the user off of the nick
			 */
			virtual EventReturn OnNickValidate(User *u, Nick *na) anope_abstract;
		};
		static EventHandlersReference<NickValidate> OnNickValidate("OnNickValidate");
	}

	/* A registered nickname.
	 * It matters that Base is here before Extensible (it is inherited by Serializable)
	 */
	class CoreExport Nick : public Serializable, public Extensible
	{
	 protected:
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
		Serialize::Reference<Account> nc;

	 protected:
		Nick() : Serializable("NickAlias") { }

	 public:
		virtual ~Nick() { }

		/** Set a vhost for the user
		 * @param ident The ident
		 * @param host The host
		 * @param creator Who created the vhost
		 * @param time When the vhost was craated
		 */
		virtual void SetVhost(const Anope::string &ident, const Anope::string &host, const Anope::string &creator, time_t created = Anope::CurTime) anope_abstract;

		/** Remove a users vhost
		 **/
		virtual void RemoveVhost() anope_abstract;

		/** Check if the user has a vhost
		 * @return true or false
		 */
		virtual bool HasVhost() const anope_abstract;

		/** Retrieve the vhost ident
		 * @return the ident
		 */
		virtual const Anope::string &GetVhostIdent() const anope_abstract;

		/** Retrieve the vhost host
		 * @return the host
		 */
		virtual const Anope::string &GetVhostHost() const anope_abstract;

		/** Retrieve the vhost creator
		 * @return the creator
		 */
		virtual const Anope::string &GetVhostCreator() const anope_abstract;

		/** Retrieve when the vhost was created
		 * @return the time it was created
		 */
		virtual time_t GetVhostCreated() const anope_abstract;
	};

	/* A registered account. Each account must have a Nick with the same nick as the
	 * account's display.
	 * It matters that Base is here before Extensible (it is inherited by Serializable)
	 */
	class CoreExport Account : public Serializable, public Extensible
	{
	 protected:
		/* Channels which reference this core in some way (this is on their access list, akick list, is founder, successor, etc) */
		Serialize::Checker<std::map<ChanServ::Channel *, int> > chanaccess;
	 public:
		/* Name of the account. Find(display)->nc == this. */
		Anope::string display;
		/* User password in form of hashm:data */
		Anope::string pass;
		Anope::string email;
		/* Locale name of the language of the user. Empty means default language */
		Anope::string language;
		/* Access list, contains user@host masks of users who get certain privileges based
		 * on if NI_SECURE is set and what (if any) kill protection is enabled. */
		std::vector<Anope::string> access;
		MemoServ::MemoInfo *memos = nullptr;
		std::map<Anope::string, Anope::string> last_modes;

		/* Nicknames registered that are grouped to this account.
		 * for n in aliases, n->nc == this.
		 */
		Serialize::Checker<std::vector<Nick *> > aliases;

		/* Set if this user is a services operattor. o->ot must exist. */
		Oper *o;

		/* Unsaved data */

		/* Number of channels registered by this account */
		uint16_t channelcount;
		/* Last time an email was sent to this user */
		time_t lastmail;
		/* Users online now logged into this account */
		std::list<User *> users;

	 protected:
		/** Constructor
		 * @param display The display nick
		 */
		Account() : Serializable("NickCore"), chanaccess("ChannelInfo"), aliases("NickAlias") { }

	 public:
		virtual ~Account() { }

		/** Changes the display for this account
		 * @param na The new display, must be grouped to this account.
		 */
		virtual void SetDisplay(const Nick *na) anope_abstract;

		/** Checks whether this account is a services oper or not.
		 * @return True if this account is a services oper, false otherwise.
		 */
		virtual bool IsServicesOper() const anope_abstract;

		/** Add an entry to the nick's access list
		 *
		 * @param entry The nick!ident@host entry to add to the access list
		 *
		 * Adds a new entry into the access list.
		 */
		virtual void AddAccess(const Anope::string &entry) anope_abstract;

		/** Get an entry from the nick's access list by index
		 *
		 * @param entry Index in the access list vector to retrieve
		 * @return The access list entry of the given index if within bounds, an empty string if the vector is empty or the index is out of bounds
		 *
		 * Retrieves an entry from the access list corresponding to the given index.
		 */
		virtual Anope::string GetAccess(unsigned entry) const anope_abstract;

		/** Get the number of entries on the access list for this account.
		 */
		virtual unsigned GetAccessCount() const anope_abstract;

		/** Find an entry in the nick's access list
		 *
		 * @param entry The nick!ident@host entry to search for
		 * @return True if the entry is found in the access list, false otherwise
		 *
		 * Search for an entry within the access list.
		 */
		virtual bool FindAccess(const Anope::string &entry) anope_abstract;

		/** Erase an entry from the nick's access list
		 *
		 * @param entry The nick!ident@host entry to remove
		 *
		 * Removes the specified access list entry from the access list.
		 */
		virtual void EraseAccess(const Anope::string &entry) anope_abstract;

		/** Clears the entire nick's access list
		 *
		 * Deletes all the memory allocated in the access list vector and then clears the vector.
		 */
		virtual void ClearAccess() anope_abstract;

		/** Is the given user on this accounts access list?
		 *
		 * @param u The user
		 *
		 * @return true if the user is on the access list
		 */
		virtual bool IsOnAccess(const User *u) const anope_abstract;

		virtual void AddChannelReference(ChanServ::Channel *ci) anope_abstract;
		virtual void RemoveChannelReference(ChanServ::Channel *ci) anope_abstract;
		virtual void GetChannelReferences(std::deque<ChanServ::Channel *> &queue) anope_abstract;
	};

	/* A request to check if an account/password is valid. These can exist for
	 * extended periods due to the time some authentication modules take.
	 */
	class CoreExport IdentifyRequest
	{
	 protected:
		/* Owner of this request, used to cleanup requests if a module is unloaded
		 * while a request us pending */
		Module *owner;
		IdentifyRequestListener *l;
		Anope::string account;
		Anope::string password;

		std::set<Module *> holds;
		bool dispatched = false;
		bool success = false;

		IdentifyRequest(IdentifyRequestListener *li, Module *o, const Anope::string &acc, const Anope::string &pass) : owner(o), l(li), account(acc), password(pass) { }
	 public:
		virtual ~IdentifyRequest() { }

		const Anope::string &GetAccount() const { return account; }
		const Anope::string &GetPassword() const { return password; }
		Module *GetOwner() const { return owner; }

		/* Holds this request. When a request is held it must be Released later
		 * for the request to complete. Multiple modules may hold a request at any time,
		 * but the request is not complete until every module has released it. If you do not
		 * require holding this (eg, your password check is done in this thread and immediately)
		 * then you don't need to hold the request before Successing it.
		 * @param m The module holding this request
		 */
		virtual void Hold(Module *m) anope_abstract;

		/** Releases a held request
		 * @param m The module releaseing the hold
		 */
		virtual void Release(Module *m) anope_abstract;

		/** Called by modules when this IdentifyRequest has successeded successfully.
		 * If this request is behind held it must still be Released after calling this.
		 * @param m The module confirming authentication
		 */
		virtual void Success(Module *m) anope_abstract;

		/** Used to either finalize this request or marks
		 * it as dispatched and begins waiting for the module(s)
		 * that have holds to finish.
		 */
		virtual void Dispatch() anope_abstract;
	};

	class IdentifyRequestListener
	{
	 public:
		virtual ~IdentifyRequestListener() { }
		virtual void OnSuccess(IdentifyRequest *) anope_abstract;
		virtual void OnFail(IdentifyRequest *) anope_abstract;
	};
}
