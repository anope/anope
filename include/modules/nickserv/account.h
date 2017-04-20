/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2017 Anope Team <team@anope.org>
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
 */
class CoreExport Account : public Serialize::Object
{
 public:
	static constexpr const char *const NAME = "account";

	/* Users online now logged into this account */
	std::vector<User *> users;

	using Serialize::Object::Object;

	virtual Anope::string GetDisplay() anope_abstract;
	virtual void SetDisplay(const Anope::string &) anope_abstract;

	virtual Anope::string GetPassword() anope_abstract;
	virtual void SetPassword(const Anope::string &) anope_abstract;

	virtual Anope::string GetEmail() anope_abstract;
	virtual void SetEmail(const Anope::string &) anope_abstract;

	virtual Anope::string GetLanguage() anope_abstract;
	virtual void SetLanguage(const Anope::string &) anope_abstract;

	virtual Oper *GetOper() anope_abstract;
	virtual void SetOper(Oper *) anope_abstract;

	virtual Anope::string GetGreet() anope_abstract;
	virtual void SetGreet(const Anope::string &) anope_abstract;

	virtual bool IsUnconfirmed() anope_abstract;
	virtual void SetUnconfirmed(bool) anope_abstract;

	virtual bool IsPrivate() anope_abstract;
	virtual void SetPrivate(bool) anope_abstract;

	virtual bool IsAutoOp() anope_abstract;
	virtual void SetAutoOp(bool) anope_abstract;

	virtual bool IsKeepModes() anope_abstract;
	virtual void SetKeepModes(bool) anope_abstract;

	virtual bool IsKillProtect() anope_abstract;
	virtual void SetKillProtect(bool) anope_abstract;

	virtual bool IsKillQuick() anope_abstract;
	virtual void SetKillQuick(bool) anope_abstract;

	virtual bool IsKillImmed() anope_abstract;
	virtual void SetKillImmed(bool) anope_abstract;

	virtual bool IsMsg() anope_abstract;
	virtual void SetMsg(bool) anope_abstract;

	virtual bool IsSecure() anope_abstract;
	virtual void SetSecure(bool) anope_abstract;

	virtual bool IsMemoSignon() anope_abstract;
	virtual void SetMemoSignon(bool) anope_abstract;

	virtual bool IsMemoReceive() anope_abstract;
	virtual void SetMemoReceive(bool) anope_abstract;

	virtual bool IsMemoMail() anope_abstract;
	virtual void SetMemoMail(bool) anope_abstract;

	virtual bool IsHideEmail() anope_abstract;
	virtual void SetHideEmail(bool) anope_abstract;

	virtual bool IsHideMask() anope_abstract;
	virtual void SetHideMask(bool) anope_abstract;

	virtual bool IsHideStatus() anope_abstract;
	virtual void SetHideStatus(bool) anope_abstract;

	virtual bool IsHideQuit() anope_abstract;
	virtual void SetHideQuit(bool) anope_abstract;

	/** Changes the display for this account
	 * @param na The new display, must be grouped to this account.
	 */
	virtual void SetDisplay(Nick *na) anope_abstract;

	/** Is the given user on this accounts access list?
	 *
	 * @param u The user
	 *
	 * @return true if the user is on the access list
	 */
	virtual bool IsOnAccess(User *u) anope_abstract;

	virtual MemoServ::MemoInfo *GetMemos() anope_abstract;

	virtual unsigned int GetChannelCount() anope_abstract;

	virtual time_t GetLastMail() anope_abstract;
	virtual void SetLastMail(time_t) anope_abstract;
};

} // namespace NickServ
