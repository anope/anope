/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "event.h"
#include "service.h"
#include "serialize.h"

namespace NickServ
{
	class Nick;
	class Account;
	class IdentifyRequestListener;

	using nickalias_map = Anope::locale_hash_map<Nick *>;
	using nickcore_map = Anope::locale_hash_map<Account *>;

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

		virtual std::vector<Nick *> GetNickList() anope_abstract;
		virtual nickalias_map& GetNickMap() anope_abstract;

		virtual std::vector<Account *> GetAccountList() anope_abstract;
		virtual nickcore_map& GetAccountMap() anope_abstract;

		virtual Nick *FindNick(const Anope::string &nick) anope_abstract;
		virtual Account *FindAccount(const Anope::string &acc) anope_abstract;
	};
	
	extern NickServService *service;

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
			static constexpr const char *NAME = "prenickexpire";

			using Events::Events;
			
			/** Called before a nick expires
			 * @param na The nick
			 * @param expire Set to true to allow the nick to expire
			 */
			virtual void OnPreNickExpire(Nick *na, bool &expire) anope_abstract;
		};

		struct CoreExport NickExpire : Events
		{
			static constexpr const char *NAME = "nickexpire";

			using Events::Events;
			
			/** Called when a nick drops
			 * @param na The nick
			 */
			virtual void OnNickExpire(Nick *na) anope_abstract;
		};

		struct CoreExport NickRegister : Events
		{
			static constexpr const char *NAME = "nickregister";

			using Events::Events;
			
			/** Called when a nick is registered
			 * @param user The user registering the nick, of any
			 * @param The nick
			 * @param password The password of the nick
			 */
			virtual void OnNickRegister(User *user, Nick *na, const Anope::string &password) anope_abstract;
		};

		struct CoreExport NickConfirm : Events
		{
			static constexpr const char *NAME = "nickconfirm";

			using Events::Events;
			
			virtual void OnNickConfirm(User *, Account *) anope_abstract;
		};

		struct CoreExport NickValidate : Events
		{
			static constexpr const char *NAME = "nickvalidate";

			using Events::Events;
			
			/** Called when a nick is validated. That is, to determine if a user is permissted
			 * to be on the given nick.
			 * @param u The user
			 * @param na The nick they are on
			 * @return EVENT_STOP to force the user off of the nick
			 */
			virtual EventReturn OnNickValidate(User *u, Nick *na) anope_abstract;
		};
	}

	/* A registered nickname.
	 * It matters that Base is here before Extensible (it is inherited by Serializable)
	 */
	class CoreExport Nick : public Serialize::Object
	{
	 protected:
		using Serialize::Object::Object;

	 public:
		static constexpr const char *const NAME = "nick";

		virtual Anope::string GetNick() anope_abstract;
		virtual void SetNick(const Anope::string &) anope_abstract;

		virtual Anope::string GetLastQuit() anope_abstract;
		virtual void SetLastQuit(const Anope::string &) anope_abstract;

		virtual Anope::string GetLastRealname() anope_abstract;
		virtual void SetLastRealname(const Anope::string &) anope_abstract;

		virtual Anope::string GetLastUsermask() anope_abstract;
		virtual void SetLastUsermask(const Anope::string &) anope_abstract;

		virtual Anope::string GetLastRealhost() anope_abstract;
		virtual void SetLastRealhost(const Anope::string &) anope_abstract;

		virtual time_t GetTimeRegistered() anope_abstract;
		virtual void SetTimeRegistered(const time_t &) anope_abstract;

		virtual time_t GetLastSeen() anope_abstract;
		virtual void SetLastSeen(const time_t &) anope_abstract;

		virtual Account *GetAccount() anope_abstract;
		virtual void SetAccount(Account *acc) anope_abstract;

		/** Set a vhost for the user
		 * @param ident The ident
		 * @param host The host
		 * @param creator Who created the vhost
		 * @param time When the vhost was craated
		 */
		virtual void SetVhost(const Anope::string &ident, const Anope::string &host, const Anope::string &creator, time_t created = Anope::CurTime) anope_abstract;
		virtual void RemoveVhost() anope_abstract;
		virtual bool HasVhost() anope_abstract;

		virtual Anope::string GetVhostIdent() anope_abstract;
		virtual void SetVhostIdent(const Anope::string &) anope_abstract;

		virtual Anope::string GetVhostHost() anope_abstract;
		virtual void SetVhostHost(const Anope::string &) anope_abstract;

		virtual Anope::string GetVhostCreator() anope_abstract;
		virtual void SetVhostCreator(const Anope::string &) anope_abstract;

		virtual time_t GetVhostCreated() anope_abstract;
		virtual void SetVhostCreated(const time_t &) anope_abstract;
	};

	/* A registered account. Each account must have a Nick with the same nick as the
	 * account's display.
	 * It matters that Base is here before Extensible (it is inherited by Serializable)
	 */
	class CoreExport Account : public Serialize::Object
	{
	 public:
		static constexpr const char *const NAME = "account";
		
		/* Set if this user is a services operattor. o->ot must exist. */
		Serialize::Reference<Oper> o;

		/* Unsaved data */

		/* Last time an email was sent to this user */
		time_t lastmail = 0;
		/* Users online now logged into this account */
		std::vector<User *> users;

	 protected:
		using Serialize::Object::Object;

	 public:
		virtual Anope::string GetDisplay() anope_abstract;
		virtual void SetDisplay(const Anope::string &) anope_abstract;

		virtual Anope::string GetPassword() anope_abstract;
		virtual void SetPassword(const Anope::string &) anope_abstract;

		virtual Anope::string GetEmail() anope_abstract;
		virtual void SetEmail(const Anope::string &) anope_abstract;

		virtual Anope::string GetLanguage() anope_abstract;
		virtual void SetLanguage(const Anope::string &) anope_abstract;

		/** Changes the display for this account
		 * @param na The new display, must be grouped to this account.
		 */
		virtual void SetDisplay(Nick *na) anope_abstract;

		/** Checks whether this account is a services oper or not.
		 * @return True if this account is a services oper, false otherwise.
		 */
		virtual bool IsServicesOper() const anope_abstract;

		/** Is the given user on this accounts access list?
		 *
		 * @param u The user
		 *
		 * @return true if the user is on the access list
		 */
		virtual bool IsOnAccess(User *u) anope_abstract;

		virtual MemoServ::MemoInfo *GetMemos() anope_abstract;

		virtual unsigned int GetChannelCount() anope_abstract;
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

	class Mode : public Serialize::Object
	{
	 public:
		static constexpr const char *const NAME = "mode";
		 
		using Serialize::Object::Object;

		virtual Account *GetAccount() anope_abstract;
		virtual void SetAccount(Account *) anope_abstract;

		virtual Anope::string GetMode() anope_abstract;
		virtual void SetMode(const Anope::string &) anope_abstract;
	};

}
