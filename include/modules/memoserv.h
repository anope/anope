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

#include "module.h"

namespace MemoServ
{
	class Ignore;

	class MemoServService : public Service
	{
	 public:
		static constexpr const char *NAME = "memoserv";
		 
		enum MemoResult
		{
			MEMO_SUCCESS,
			MEMO_INVALID_TARGET,
			MEMO_TOO_FAST,
			MEMO_TARGET_FULL
		};

		MemoServService(Module *m) : Service(m, "MemoServService", NAME)
		{
		}

		/** Sends a memo.
		 * @param source The source of the memo, can be anythin.
		 * @param target The target of the memo, nick or channel.
		 * @param message Memo text
		 * @param force true to force the memo, restrictions/delays etc are not checked
		 */
		virtual MemoResult Send(const Anope::string &source, const Anope::string &target, const Anope::string &message, bool force = false) anope_abstract;

		/** Check for new memos and notify the user if there are any
		 * @param u The user
		 */
		virtual void Check(User *u) anope_abstract;

		virtual MemoInfo *GetMemoInfo(const Anope::string &targ, bool &is_registered, bool &is_chan, bool create) anope_abstract;
	};
	
	extern MemoServService *service;

	namespace Event
	{
		struct CoreExport MemoSend : Events
		{
			static constexpr const char *NAME = "memosend";

			using Events::Events;
			
			/** Called when a memo is sent
			 * @param source The source of the memo
			 * @param target The target of the memo
			 * @param mi Memo info for target
			 * @param m The memo
			 */
			virtual void OnMemoSend(const Anope::string &source, const Anope::string &target, MemoInfo *mi, Memo *m) anope_abstract;
		};

		struct CoreExport MemoDel : Events
		{
			static constexpr const char *NAME = "memodel";

			using Events::Events;
			
			/** Called when a memo is deleted
			 * @param target The target the memo is being deleted from (nick or channel)
			 * @param mi The memo info
			 * @param m The memo
			 */
			virtual void OnMemoDel(const Anope::string &target, MemoInfo *mi, const Memo *m) anope_abstract;
		};
	}

	class Memo : public Serialize::Object
	{
	 protected:
		using Serialize::Object::Object;

	 public:
		static constexpr const char *const NAME = "memo";

		virtual MemoInfo *GetMemoInfo() anope_abstract;
		virtual void SetMemoInfo(MemoInfo *) anope_abstract;

		virtual time_t GetTime() anope_abstract;
		virtual void SetTime(const time_t &) anope_abstract;

		virtual Anope::string GetSender() anope_abstract;
		virtual void SetSender(const Anope::string &) anope_abstract;

		virtual Anope::string GetText() anope_abstract;
		virtual void SetText(const Anope::string &) anope_abstract;

		virtual bool GetUnread() anope_abstract;
		virtual void SetUnread(const bool &) anope_abstract;

		virtual bool GetReceipt() anope_abstract;
		virtual void SetReceipt(const bool &) anope_abstract;
	};

	/* Memo info structures.  Since both nicknames and channels can have memos,
	 * we encapsulate memo data in a MemoInfo to make it easier to handle.
	 */
	class MemoInfo : public Serialize::Object
	{
	 protected:
		using Serialize::Object::Object;

	 public:
		static constexpr const char *const NAME = "memoinfo";

		virtual Memo *GetMemo(unsigned index) anope_abstract;

		virtual unsigned GetIndex(Memo *m) anope_abstract;

		virtual void Del(unsigned index) anope_abstract;

		virtual bool HasIgnore(User *u) anope_abstract;

		virtual NickServ::Account *GetAccount() anope_abstract;
		virtual void SetAccount(NickServ::Account *) anope_abstract;

		virtual ChanServ::Channel *GetChannel() anope_abstract;
		virtual void SetChannel(ChanServ::Channel *) anope_abstract;

		virtual int16_t GetMemoMax() anope_abstract;
		virtual void SetMemoMax(const int16_t &) anope_abstract;

		virtual bool IsHardMax() anope_abstract;
		virtual void SetHardMax(bool) anope_abstract;

		virtual std::vector<Memo *> GetMemos() anope_abstract;
		virtual std::vector<Ignore *> GetIgnores() anope_abstract;
	};

	class Ignore : public Serialize::Object
	{
	 protected:
		using Serialize::Object::Object;

	 public:
		static constexpr const char *const NAME = "memoignore";

		virtual MemoInfo *GetMemoInfo() anope_abstract;
		virtual void SetMemoInfo(MemoInfo *) anope_abstract;

		virtual Anope::string GetMask() anope_abstract;
		virtual void SetMask(const Anope::string &mask) anope_abstract;
	};
}
