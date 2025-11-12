// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include "anope.h"
#include "serialize.h"

class CoreExport Memo final
	: public Serializable
{
public:
	struct Type final
		: public Serialize::Type
	{
		Type();
		void Serialize(Serializable *obj, Serialize::Data &data) const override;
		Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override;
	};

	MemoInfo *mi;
	bool unread;
	bool receipt;
	Memo();
	~Memo();

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
	std::set<Anope::string, ci::less> ignores;

	MemoInfo();
	Memo *GetMemo(unsigned index) const;
	unsigned GetIndex(Memo *m) const;
	void Del(unsigned index);
	bool HasIgnore(User *u);

	static MemoInfo *GetMemoInfo(const Anope::string &targ, bool &is_chan);
};
