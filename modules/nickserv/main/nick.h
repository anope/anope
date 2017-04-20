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

class NickImpl : public NickServ::Nick
{
	friend class NickType;

	Serialize::Storage<NickServ::Account *> account;
	Serialize::Storage<Anope::string> nick, last_quit, last_realname, last_usermask, last_realhost;
	Serialize::Storage<time_t> time_registered, last_seen;
	Serialize::Storage<bool> noexpire;

 public:
	using NickServ::Nick::Nick;
	~NickImpl();
	void Delete() override;

	Anope::string GetNick() override;
	void SetNick(const Anope::string &) override;

	Anope::string GetLastQuit() override;
	void SetLastQuit(const Anope::string &) override;

	Anope::string GetLastRealname() override;
	void SetLastRealname(const Anope::string &) override;

	Anope::string GetLastUsermask() override;
	void SetLastUsermask(const Anope::string &) override;

	Anope::string GetLastRealhost() override;
	void SetLastRealhost(const Anope::string &) override;

	time_t GetTimeRegistered() override;
	void SetTimeRegistered(const time_t &) override;

	time_t GetLastSeen() override;
	void SetLastSeen(const time_t &) override;

	NickServ::Account *GetAccount() override;
	void SetAccount(NickServ::Account *acc) override;

	bool IsNoExpire() override;
	void SetNoExpire(bool) override;
};
