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

class LevelImpl : public ChanServ::Level
{
	friend class LevelType;

	Serialize::Storage<ChanServ::Channel *> channel;
	Serialize::Storage<Anope::string> name;
	Serialize::Storage<int> level;

 public:
	using ChanServ::Level::Level;

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *) override;

	Anope::string GetName() override;
	void SetName(const Anope::string &) override;

	int GetLevel() override;
	void SetLevel(const int &) override;
};
