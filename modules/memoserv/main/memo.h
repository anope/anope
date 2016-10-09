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

class MemoImpl : public MemoServ::Memo
{
	friend class MemoType;

	MemoServ::MemoInfo *memoinfo = nullptr;
	Anope::string text, sender;
	time_t time = 0;
	bool unread = false, receipt = false;

 public:
	MemoImpl(Serialize::TypeBase *type) : MemoServ::Memo(type) { }
	MemoImpl(Serialize::TypeBase *type, Serialize::ID id) : MemoServ::Memo(type, id) { }

	MemoServ::MemoInfo *GetMemoInfo() override;
	void SetMemoInfo(MemoServ::MemoInfo *) override;

	time_t GetTime() override;
	void SetTime(const time_t &) override;

	Anope::string GetSender() override;
	void SetSender(const Anope::string &) override;

	Anope::string GetText() override;
	void SetText(const Anope::string &) override;

	bool GetUnread() override;
	void SetUnread(const bool &) override;

	bool GetReceipt() override;
	void SetReceipt(const bool &) override;
};
