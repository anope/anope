/*
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

#include "anope.h"
#include "serialize.h"

class CoreExport Memo final
	: public Serializable
{
public:
	MemoInfo *mi;
	bool unread;
	bool receipt;
	Memo();
	~Memo();

	void Serialize(Serialize::Data &data) const override;
	static Serializable *Unserialize(Serializable *obj, Serialize::Data &);

	Anope::string owner;
	/* When it was sent */
	time_t time;
	Anope::string sender;
	Anope::string text;
};

/* Memo info structures.  Since both nicknames and channels can have memos,
 * we encapsulate memo data in a MemoInfo to make it easier to handle.
 */
struct CoreExport MemoInfo final
{
	int16_t memomax = 0;
	Serialize::Checker<std::vector<Memo *> > memos;
	std::vector<Anope::string> ignores;

	MemoInfo();
	Memo *GetMemo(unsigned index) const;
	unsigned GetIndex(Memo *m) const;
	void Del(unsigned index);
	bool HasIgnore(User *u);

	static MemoInfo *GetMemoInfo(const Anope::string &targ, bool &is_chan);
};
