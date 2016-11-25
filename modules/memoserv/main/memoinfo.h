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

#include "modules/memoserv.h"

class MemoInfoImpl : public MemoServ::MemoInfo
{
	friend class MemoInfoType;

	Serialize::Object *owner = nullptr;
	int16_t memomax = 0;
	bool hardmax = false;

 public:
	MemoInfoImpl(Serialize::TypeBase *type) : MemoServ::MemoInfo(type) { }
	MemoInfoImpl(Serialize::TypeBase *type, Serialize::ID id) : MemoServ::MemoInfo(type, id) { }

	MemoServ::Memo *GetMemo(unsigned index) override;
	unsigned GetIndex(MemoServ::Memo *m) override;
	void Del(unsigned index) override;
	bool HasIgnore(User *u) override;

	Serialize::Object *GetOwner() override;
	void SetOwner(Serialize::Object *) override;

	int16_t GetMemoMax() override;
	void SetMemoMax(const int16_t &) override;

	bool IsHardMax() override;
	void SetHardMax(bool) override;

	std::vector<MemoServ::Memo *> GetMemos() override;
	std::vector<MemoServ::Ignore *> GetIgnores() override;
};

