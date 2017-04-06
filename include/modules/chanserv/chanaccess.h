/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
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

namespace ChanServ
{

enum
{
	ACCESS_INVALID = -10000,
	ACCESS_FOUNDER = 10001
};

/* Represents one entry of an access list on a channel. */
class CoreExport ChanAccess : public Serialize::Object
{
 public:
	static constexpr const char *const NAME = "access";

	Serialize::Storage<Channel *> channel;
	Serialize::Storage<NickServ::Account *> account;
	Serialize::Storage<Anope::string> creator, mask;
	Serialize::Storage<time_t> last_seen, created;

	using Serialize::Object::Object;

	virtual Channel *GetChannel() anope_abstract;
	virtual void SetChannel(Channel *ci) anope_abstract;

	virtual Anope::string GetCreator() anope_abstract;
	virtual void SetCreator(const Anope::string &c) anope_abstract;

	virtual time_t GetLastSeen() anope_abstract;
	virtual void SetLastSeen(const time_t &t) anope_abstract;

	virtual time_t GetCreated() anope_abstract;
	virtual void SetCreated(const time_t &t) anope_abstract;

	virtual Anope::string GetMask() anope_abstract;
	virtual void SetMask(const Anope::string &) anope_abstract;

	virtual NickServ::Account *GetAccount() anope_abstract;
	virtual void SetAccount(NickServ::Account *) anope_abstract;

	virtual Anope::string Mask() anope_abstract;

	/** Check if this access entry matches the given user or account
	 * @param u The user
	 * @param acc The account
	 */
	virtual bool Matches(const User *u, NickServ::Account *acc) anope_abstract;

	/** Check if this access entry has the given privilege.
	 * @param name The privilege name
	 */
	virtual bool HasPriv(const Anope::string &name) anope_abstract;

	/** Serialize the access given by this access entry into a human
	 * readable form. chanserv/access will return a number, chanserv/xop
	 * will be AOP, SOP, etc.
	 */
	virtual Anope::string AccessSerialize() anope_abstract;

	/** Unserialize this access entry from the given data. This data
	 * will be fetched from AccessSerialize.
	 */
	virtual void AccessUnserialize(const Anope::string &data) anope_abstract;

	virtual int Compare(ChanAccess *other) anope_abstract;

	/* Comparison operators to other Access entries */
	bool operator>(ChanAccess &other)
	{
		return Compare(&other) > 0;
	}

	bool operator<(ChanAccess &other)
	{
		return Compare(&other) < 0;
	}

	bool operator>=(ChanAccess &other)
	{
		return !(*this < other);
	}

	bool operator<=(ChanAccess &other)
	{
		return !(*this > other);
	}
};

} // namespace ChanServ
