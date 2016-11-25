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

/* A registered nickname.
 */
class CoreExport Nick : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	static constexpr const char *const NAME = "nick";

	virtual Anope::string GetNick() anope_abstract;
	virtual void SetNick(const Anope::string &) anope_abstract;

	virtual Anope::string GetLastQuit() anope_abstract;
	virtual void SetLastQuit(const Anope::string &) anope_abstract;

	virtual Anope::string GetLastRealname() anope_abstract;
	virtual void SetLastRealname(const Anope::string &) anope_abstract;

	virtual Anope::string GetLastUsermask() anope_abstract;
	virtual void SetLastUsermask(const Anope::string &) anope_abstract;

	virtual Anope::string GetLastRealhost() anope_abstract;
	virtual void SetLastRealhost(const Anope::string &) anope_abstract;

	virtual time_t GetTimeRegistered() anope_abstract;
	virtual void SetTimeRegistered(const time_t &) anope_abstract;

	virtual time_t GetLastSeen() anope_abstract;
	virtual void SetLastSeen(const time_t &) anope_abstract;

	virtual Account *GetAccount() anope_abstract;
	virtual void SetAccount(Account *acc) anope_abstract;

	virtual bool IsNoExpire() anope_abstract;
	virtual void SetNoExpire(bool) anope_abstract;
};

} // namespace NickServ
