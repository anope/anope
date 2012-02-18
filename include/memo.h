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

#ifndef MEMO_H
#define MEMO_H

#include "anope.h"
#include "serialize.h"

/** Memo Flags
 */
enum MemoFlag
{
	/* Memo is unread */
	MF_UNREAD,
	/* Sender requests a receipt */
	MF_RECEIPT
};

const Anope::string MemoFlagStrings[] = {
	"MF_UNREAD", "MF_RECEIPT", ""
};

/* Memo info structures.  Since both nicknames and channels can have memos,
 * we encapsulate memo data in a MemoList to make it easier to handle. */
class CoreExport Memo : public Flags<MemoFlag>, public Serializable
{
 public:
 	Memo();

	Anope::string serialize_name() const anope_override;
	serialized_data serialize() anope_override;
	static void unserialize(serialized_data &);

	Anope::string owner;
	time_t time;	/* When it was sent */
	Anope::string sender;
	Anope::string text;
};

struct CoreExport MemoInfo
{
	int16_t memomax;
	std::vector<Memo *> memos;
	std::vector<Anope::string> ignores;

	unsigned GetIndex(Memo *m) const;
	void Del(unsigned index);
	void Del(Memo *m);
	bool HasIgnore(User *u);
};

#endif // MEMO_H
