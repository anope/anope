/*
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

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
		virtual MemoResult Send(const Anope::string &source, const Anope::string &target, const Anope::string &message, bool force = false) = 0;
	
		/** Check for new memos and notify the user if there are any
		 * @param u The user
		 */
		virtual void Check(User *u) = 0;
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
}
