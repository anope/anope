/*
 * Anope IRC Services
 *
 * Copyright (C) 2015-2017 Anope Team <team@anope.org>
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

#include "modules/nickserv.h"

class AccountImpl : public NickServ::Account
{
	friend class AccountType;

	Serialize::Storage<Anope::string> display, password, email, language;
	Serialize::Storage<Oper *> oper;
	Serialize::Storage<Anope::string> greet;
	Serialize::Storage<bool> unconfirmed;
	Serialize::Storage<bool> _private;
	Serialize::Storage<bool> autoop;
	Serialize::Storage<bool> keepmodes;
	Serialize::Storage<bool> killprotect;
	Serialize::Storage<bool> killquick;
	Serialize::Storage<bool> killimmed;
	Serialize::Storage<bool> msg;
	Serialize::Storage<bool> memosignon, memoreceive, memomail;
	Serialize::Storage<bool> hideemail, hidemask, hidestatus, hidequit;
	Serialize::Storage<time_t> lastmail;

 public:
	using NickServ::Account::Account;
	~AccountImpl();
	void Delete() override;

	bool CanGC() override;

	Anope::string GetDisplay() override;
	void SetDisplay(const Anope::string &) override;

	Anope::string GetPassword() override;
	void SetPassword(const Anope::string &) override;

	Anope::string GetEmail() override;
	void SetEmail(const Anope::string &) override;

	Anope::string GetLanguage() override;
	void SetLanguage(const Anope::string &) override;

	Oper *GetOper() override;
	void SetOper(Oper *) override;

	Anope::string GetGreet() override;
	void SetGreet(const Anope::string &) override;

	bool IsUnconfirmed() override;
	void SetUnconfirmed(bool) override;

	bool IsPrivate() override;
	void SetPrivate(bool) override;

	bool IsAutoOp() override;
	void SetAutoOp(bool) override;

	bool IsKeepModes() override;
	void SetKeepModes(bool) override;

	bool IsKillProtect() override;
	void SetKillProtect(bool) override;

	bool IsKillQuick() override;
	void SetKillQuick(bool) override;

	bool IsKillImmed() override;
	void SetKillImmed(bool) override;

	bool IsMsg() override;
	void SetMsg(bool) override;

	bool IsMemoSignon() override;
	void SetMemoSignon(bool) override;

	bool IsMemoReceive() override;
	void SetMemoReceive(bool) override;

	bool IsMemoMail() override;
	void SetMemoMail(bool) override;

	bool IsHideEmail() override;
	void SetHideEmail(bool) override;

	bool IsHideMask() override;
	void SetHideMask(bool) override;

	bool IsHideStatus() override;
	void SetHideStatus(bool) override;

	bool IsHideQuit() override;
	void SetHideQuit(bool) override;

	MemoServ::MemoInfo *GetMemos() override;

	void SetDisplay(NickServ::Nick *na) override;
	unsigned int GetChannelCount() override;

	time_t GetLastMail() override;
	void SetLastMail(time_t) override;
};
