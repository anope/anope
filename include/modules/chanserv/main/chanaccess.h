/*
 * Anope IRC Services
 *
 * Copyright (C) 2016 Anope Team <team@anope.org>
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

class ChanAccessImpl : public ChanServ::ChanAccess
{
 public:
	ChanAccessImpl(Serialize::TypeBase *type) : ChanServ::ChanAccess(type) { }
	ChanAccessImpl(Serialize::TypeBase *type, Serialize::ID id) : ChanServ::ChanAccess(type, id) { }

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *ci) override;

	Anope::string GetCreator() override;
	void SetCreator(const Anope::string &c) override;

	time_t GetLastSeen() override;
	void SetLastSeen(const time_t &t) override;

	time_t GetCreated() override;
	void SetCreated(const time_t &t) override;

	Anope::string GetMask() override;
	void SetMask(const Anope::string &) override;

	Serialize::Object *GetObj() override;
	void SetObj(Serialize::Object *) override;

	Anope::string Mask() override;
	NickServ::Account *GetAccount() override;

	bool Matches(const User *u, NickServ::Account *acc) override;
};
