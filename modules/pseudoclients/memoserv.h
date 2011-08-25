#ifndef MEMOSERV_H
#define MEMOSERV_H

class MemoServService : public Service<MemoServService>
{
 public:
	enum MemoResult
	{
		MEMO_SUCCESS,
		MEMO_INVALID_TARGET,
		MEMO_TOO_FAST,
		MEMO_TARGET_FULL
	};

	MemoServService(Module *m) : Service<MemoServService>(m, "MemoServ") { }

	/** Retrieve the memo info for a nick or channel
	 * @param target Target
	 * @param ischan Set to true if target is a channel
	 * @return A memoinfo structure or NULL
	 */
 	virtual MemoInfo *GetMemoInfo(const Anope::string &target, bool &ischan) = 0;

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

static service_reference<MemoServService> memoserv("MemoServ");

#endif // MEMOSERV_H

