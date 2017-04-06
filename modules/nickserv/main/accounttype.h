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

#include "account.h"

class AccountType : public Serialize::Type<AccountImpl>
{
 public:
	/* Name of the account */
	struct Display : Serialize::Field<AccountImpl, Anope::string>
	{
		using Serialize::Field<AccountImpl, Anope::string>::Field;

		void OnSet(AccountImpl *s, const Anope::string &) override;
	} display;
	/* User password in form of hashm:data */
	Serialize::Field<AccountImpl, Anope::string> pass;
	Serialize::Field<AccountImpl, Anope::string> email;
	/* Locale name of the language of the user. Empty means default language */
	Serialize::Field<AccountImpl, Anope::string> language;
	Serialize::ObjectField<AccountImpl, Oper *> oper;
	Serialize::Field<AccountImpl, Anope::string> greet;
	Serialize::Field<AccountImpl, bool> unconfirmed, _private, autoop, keepmodes,
		killprotect, killquick, killimmed, msg, secure, memosignon, memoreceive,
		memomail, hideemail, hidemask, hidestatus, hidequit;
	Serialize::Field<AccountImpl, time_t> last_mail;

 	AccountType(Module *);

	NickServ::Account *FindAccount(const Anope::string &nick);
};
