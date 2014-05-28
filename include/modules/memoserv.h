/*
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#pragma once

#include "module.h"

namespace MemoServ
{
	class MemoServService : public Service
	{
	 public:
		enum MemoResult
		{
			MEMO_SUCCESS,
			MEMO_INVALID_TARGET,
			MEMO_TOO_FAST,
			MEMO_TARGET_FULL
		};

		MemoServService(Module *m) : Service(m, "MemoServService", "MemoServ")
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

		virtual Memo *CreateMemo() anope_abstract;
		virtual MemoInfo *GetMemoInfo(const Anope::string &targ, bool &is_registered, bool &is_chan, bool create) anope_abstract;
	};
	static ServiceReference<MemoServService> service("MemoServService", "MemoServ");

	namespace Event
	{
		struct CoreExport MemoSend : Events
		{
			/** Called when a memo is sent
			 * @param source The source of the memo
			 * @param target The target of the memo
			 * @param mi Memo info for target
			 * @param m The memo
			 */
			virtual void OnMemoSend(const Anope::string &source, const Anope::string &target, MemoInfo *mi, Memo *m) anope_abstract;
		};
		static EventHandlersReference<MemoSend> OnMemoSend("OnMemoSend");

		struct CoreExport MemoDel : Events
		{
			/** Called when a memo is deleted
			 * @param target The target the memo is being deleted from (nick or channel)
			 * @param mi The memo info
			 * @param m The memo
			 */
			virtual void OnMemoDel(const Anope::string &target, MemoInfo *mi, const Memo *m) anope_abstract;
		};
		static EventHandlersReference<MemoDel> OnMemoDel("OnMemoDel");
	}

	class Memo : public Serializable
	{
	 public:
		bool unread;
		bool receipt;
	 protected:
		Memo() : Serializable("Memo") { }
	 public:

		Anope::string owner;
		/* When it was sent */
		time_t time;
		Anope::string sender;
		Anope::string text;
	};

	/* Memo info structures.  Since both nicknames and channels can have memos,
	 * we encapsulate memo data in a MemoInfo to make it easier to handle.
	 */
	struct CoreExport MemoInfo
	{
		int16_t memomax = 0;
		Serialize::Checker<std::vector<Memo *> > memos;
		std::vector<Anope::string> ignores;

		MemoInfo() : memos("Memo") { }
		virtual ~MemoInfo() { }

		virtual Memo *GetMemo(unsigned index) const anope_abstract;

		virtual unsigned GetIndex(Memo *m) const anope_abstract;

		virtual void Del(unsigned index) anope_abstract;

		virtual bool HasIgnore(User *u) anope_abstract;
	};
}
