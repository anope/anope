// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
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

#include "services.h"
#include "modules.h"
#include "service.h"
#include "memo.h"
#include "users.h"
#include "account.h"
#include "regchannel.h"

Memo::Memo()
	: Serializable(MEMO_TYPE)
{
	mi = NULL;
	unread = receipt = false;
}

Memo::~Memo()
{
	if (mi)
	{
		auto it = std::find(mi->memos->begin(), mi->memos->end(), this);

		if (it != mi->memos->end())
			mi->memos->erase(it);
	}
}

Memo::Type::Type()
	: Serialize::Type(MEMO_TYPE)
{
}

void Memo::Type::Serialize(Serializable *obj, Serialize::Data &data) const
{
	const auto *m = static_cast<const Memo *>(obj);
	data.Store("owner", m->owner);
	data.Store("time", m->time);
	data.Store("sender", m->sender);
	data.Store("text", m->text);
	data.Store("unread", m->unread);
	data.Store("receipt", m->receipt);
}

Serializable *Memo::Type::Unserialize(Serializable *obj, Serialize::Data &data) const
{
	const auto owner = data.Load("owner");

	bool ischan;
	MemoInfo *mi = MemoInfo::GetMemoInfo(owner, ischan);
	if (!mi)
		return NULL;

	Memo *m;
	if (obj)
		m = anope_dynamic_static_cast<Memo *>(obj);
	else
	{
		m = new Memo();
		m->mi = mi;
	}

	m->owner = owner;
	m->time = data.Load<time_t>("time");
	m->sender = data.Load("sender");
	m->text = data.Load("text");
	m->unread = data.Load<bool>("unread");
	m->receipt = data.Load<bool>("receipt");

	if (obj == NULL)
		mi->memos->push_back(m);
	return m;
}

MemoInfo::MemoInfo()
	: memos(MEMO_TYPE)
{
}

Memo *MemoInfo::GetMemo(unsigned index) const
{
	if (index >= this->memos->size())
		return NULL;
	Memo *m = (*memos)[index];
	m->QueueUpdate();
	return m;
}

unsigned MemoInfo::GetIndex(Memo *m) const
{
	for (unsigned i = 0; i < this->memos->size(); ++i)
		if (this->GetMemo(i) == m)
			return i;
	return -1;
}

void MemoInfo::Del(unsigned index)
{
	if (index >= this->memos->size())
		return;

	Memo *m = this->GetMemo(index);

	auto it = std::find(memos->begin(), memos->end(), m);
	if (it != memos->end())
		memos->erase(it);

	delete m;
}

bool MemoInfo::HasIgnore(User *u)
{
	for (const auto &ignore : this->ignores)
	{
		if (u->nick.equals_ci(ignore) || (u->IsIdentified() && u->Account()->display.equals_ci(ignore)) || Anope::Match(u->GetMask(), Anope::string(ignore)))
			return true;
	}
	return false;
}

MemoInfo *MemoInfo::GetMemoInfo(const Anope::string &target, bool &ischan)
{
	if (!target.empty() && target[0] == '#')
	{
		ischan = true;
		ChannelInfo *ci = ChannelInfo::Find(target);
		if (ci != NULL)
			return &ci->memos;
	}
	else
	{
		ischan = false;
		NickAlias *na = NickAlias::Find(target);
		if (na != NULL)
			return &na->nc->memos;
	}

	return NULL;
}
