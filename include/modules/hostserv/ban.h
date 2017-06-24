/*
 * Anope IRC Services
 *
 * Copyright (C) 2017 Anope Team <team@anope.org>
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

namespace HostServ
{
	class Ban : public Serialize::Object
	{
	 protected:
		using Serialize::Object::Object;

	 public:
		static constexpr const char *const NAME = "hostservban";

		virtual NickServ::Account *GetAccount() anope_abstract;
		virtual void SetAccount(NickServ::Account *) anope_abstract;

		virtual Anope::string GetCreator() anope_abstract;
		virtual void SetCreator(const Anope::string &) anope_abstract;

		virtual time_t GetCreated() anope_abstract;
		virtual void SetCreated(time_t) anope_abstract;

		virtual time_t GetExpires() anope_abstract;
		virtual void SetExpires(time_t) anope_abstract;

		virtual Anope::string GetReason() anope_abstract;
		virtual void SetReason(const Anope::string &) anope_abstract;
	};
}
