/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
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

#pragma once

namespace NickServ
{

/* A registered account. Each account must have a Nick with the same nick as the
 * account's display.
 * It matters that Base is here before Extensible (it is inherited by Serializable)
 */
class CoreExport Account : public Serialize::Object
{
 public:
	static constexpr const char *const NAME = "account";

	/* Set if this user is a services operattor. o->ot must exist. */
	Serialize::Reference<Oper> o;

	/* Unsaved data */

	/* Last time an email was sent to this user */
	time_t lastmail = 0;
	/* Users online now logged into this account */
	std::vector<User *> users;

 protected:
	using Serialize::Object::Object;

 public:
	virtual Anope::string GetDisplay() anope_abstract;
	virtual void SetDisplay(const Anope::string &) anope_abstract;

	virtual Anope::string GetPassword() anope_abstract;
	virtual void SetPassword(const Anope::string &) anope_abstract;

	virtual Anope::string GetEmail() anope_abstract;
	virtual void SetEmail(const Anope::string &) anope_abstract;

	virtual Anope::string GetLanguage() anope_abstract;
	virtual void SetLanguage(const Anope::string &) anope_abstract;

	/** Changes the display for this account
	 * @param na The new display, must be grouped to this account.
	 */
	virtual void SetDisplay(Nick *na) anope_abstract;

	/** Checks whether this account is a services oper or not.
	 * @return True if this account is a services oper, false otherwise.
	 */
	virtual bool IsServicesOper() const anope_abstract;

	/** Is the given user on this accounts access list?
	 *
	 * @param u The user
	 *
	 * @return true if the user is on the access list
	 */
	virtual bool IsOnAccess(User *u) anope_abstract;

	virtual MemoServ::MemoInfo *GetMemos() anope_abstract;

	virtual unsigned int GetChannelCount() anope_abstract;
};

} // namespace NickServ