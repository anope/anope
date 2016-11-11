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

#include "modules/nickserv.h"

class AccountImpl : public NickServ::Account
{
	friend class AccountType;

	Anope::string display, password, email, language;
	Oper *oper = nullptr;

 public:
	using NickServ::Account::Account;
	~AccountImpl();
	void Delete() override;

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

	MemoServ::MemoInfo *GetMemos() override;

	void SetDisplay(NickServ::Nick *na) override;
	bool IsOnAccess(User *u) override;
	unsigned int GetChannelCount() override;
};
