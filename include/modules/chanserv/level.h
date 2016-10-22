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


class Level : public Serialize::Object
{
 public:
	static constexpr const char *const NAME = "level";

	using Serialize::Object::Object;

	virtual Channel *GetChannel() anope_abstract;
	virtual void SetChannel(Channel *) anope_abstract;

	virtual Anope::string GetName() anope_abstract;
	virtual void SetName(const Anope::string &) anope_abstract;

	virtual int GetLevel() anope_abstract;
	virtual void SetLevel(const int &) anope_abstract;
};

} // namespace ChanServ