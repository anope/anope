/*
 * Anope IRC Services
 *
 * Copyright (C) 2015-2016 Anope Team <team@anope.org>
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

#include "memoinfotype.h"

MemoServ::Memo *MemoInfoImpl::GetMemo(unsigned index)
{
	auto memos = GetMemos();
	return index >= memos.size() ? nullptr : memos[index];
}

unsigned MemoInfoImpl::GetIndex(MemoServ::Memo *m)
{
	auto memos = GetMemos();
	for (unsigned i = 0; i < memos.size(); ++i)
		if (this->GetMemo(i) == m)
			return i;
#warning "-1 unsigned?"
	return -1; // XXX wtf?
}

void MemoInfoImpl::Del(unsigned index)
{
	delete GetMemo(index);
}

bool MemoInfoImpl::HasIgnore(User *u)
{
	for (MemoServ::Ignore *ign : GetIgnores())
	{
		const Anope::string &mask = ign->GetMask();
		if (u->nick.equals_ci(mask) || (u->Account() && u->Account()->GetDisplay().equals_ci(mask)) || Anope::Match(u->GetMask(), mask))
			return true;
	}
	return false;
}

Serialize::Object *MemoInfoImpl::GetOwner()
{
	return Get(&MemoInfoType::owner);
}

void MemoInfoImpl::SetOwner(Serialize::Object *o)
{
	Set(&MemoInfoType::owner, o);
}

int16_t MemoInfoImpl::GetMemoMax()
{
	return Get(&MemoInfoType::memomax);
}

void MemoInfoImpl::SetMemoMax(const int16_t &i)
{
	Set(&MemoInfoType::memomax, i);
}

bool MemoInfoImpl::IsHardMax()
{
	return Get(&MemoInfoType::hardmax);
}

void MemoInfoImpl::SetHardMax(bool hardmax)
{
	Set(&MemoInfoType::hardmax, hardmax);
}

std::vector<MemoServ::Memo *> MemoInfoImpl::GetMemos()
{
	return GetRefs<MemoServ::Memo *>();
}

std::vector<MemoServ::Ignore *> MemoInfoImpl::GetIgnores()
{
	return GetRefs<MemoServ::Ignore *>();
}

