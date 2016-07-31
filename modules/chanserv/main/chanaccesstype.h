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

template<typename T>
class ChanAccessType : public Serialize::Type<T>
{
	static_assert(std::is_base_of<ChanServ::ChanAccess, T>::value, "");

 public:
	Serialize::ObjectField<ChanServ::ChanAccess, ChanServ::Channel *> ci;
	Serialize::Field<ChanServ::ChanAccess, Anope::string> mask;
	Serialize::ObjectField<ChanServ::ChanAccess, Serialize::Object *> obj;
	Serialize::Field<ChanServ::ChanAccess, Anope::string> creator;
	Serialize::Field<ChanServ::ChanAccess, time_t> last_seen;
	Serialize::Field<ChanServ::ChanAccess, time_t> created;

	ChanAccessType(Module *me) : Serialize::Type<T>(me)
		, ci(this, "ci", &ChanServ::ChanAccess::channel, true)
		, mask(this, "mask", &ChanServ::ChanAccess::mask)
		, obj(this, "obj", &ChanServ::ChanAccess::object, true)
		, creator(this, "creator", &ChanServ::ChanAccess::creator)
		, last_seen(this, "last_seen", &ChanServ::ChanAccess::last_seen)
		, created(this, "created", &ChanServ::ChanAccess::created)
	{
	}
};
