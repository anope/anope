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

class LevelImpl : public ChanServ::Level
{
	friend class LevelType;

	ChanServ::Channel *channel = nullptr;
	Anope::string name;
	int level = 0;

 public:
	LevelImpl(Serialize::TypeBase *type) : ChanServ::Level(type) { }
	LevelImpl(Serialize::TypeBase *type, Serialize::ID id) : ChanServ::Level(type, id) { }

	ChanServ::Channel *GetChannel() override;
	void SetChannel(ChanServ::Channel *) override;

	Anope::string GetName() override;
	void SetName(const Anope::string &) override;

	int GetLevel() override;
	void SetLevel(const int &) override;
};
