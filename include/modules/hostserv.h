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

#include "serialize.h"

namespace HostServ
{
	class VHost : public Serialize::Object
	{
	 protected:
		using Serialize::Object::Object;

	 public:
		static constexpr const char *const NAME = "vhost";

		virtual NickServ::Account *GetAccount() anope_abstract;
		virtual void SetAccount(NickServ::Account *) anope_abstract;

		virtual Anope::string GetIdent() anope_abstract;
		virtual void SetIdent(const Anope::string &) anope_abstract;

		virtual Anope::string GetHost() anope_abstract;
		virtual void SetHost(const Anope::string &) anope_abstract;

		virtual Anope::string GetCreator() anope_abstract;
		virtual void SetCreator(const Anope::string &) anope_abstract;

		virtual time_t GetCreated() anope_abstract;
		virtual void SetCreated(time_t) anope_abstract;

		virtual bool IsDefault() anope_abstract;
		virtual void SetDefault(bool) anope_abstract;

		inline Anope::string Mask()
		{
			Anope::string ident = GetIdent(), host = GetHost();
			if (!ident.empty())
				return ident + "@" + host;
			else
				return host;
		}
	};

	/** Find the default vhost for an object, or the first
	 * if there is no default.
	 * @return The object's vhost
	 */
	inline VHost *FindVHost(Serialize::Object *obj)
	{
		VHost *first = nullptr;

		for (VHost *v : obj->GetRefs<VHost *>())
		{
			if (first == nullptr)
				first = v;

			if (v->IsDefault())
			{
				first = v;
				break;
			}
		}

		return first;
	}

	inline VHost *FindVHost(Serialize::Object *obj, const Anope::string &v)
	{
		for (VHost *vhost : obj->GetRefs<VHost *>())
		{
			if (vhost->Mask().equals_ci(v))
			{
				return vhost;
			}
		}

		return nullptr;
	}
}

