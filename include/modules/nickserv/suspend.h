/*
 * Anope IRC Services
 *
 * Copyright (C) 2013-2016 Anope Team <team@anope.org>
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

class NSSuspendInfo : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	static constexpr const char *const NAME = "nssuspendinfo";

	virtual NickServ::Account *GetAccount() anope_abstract;
	virtual void SetAccount(NickServ::Account *) anope_abstract;

	virtual Anope::string GetBy() anope_abstract;
	virtual void SetBy(const Anope::string &) anope_abstract;
	
	virtual Anope::string GetReason() anope_abstract;
	virtual void SetReason(const Anope::string &) anope_abstract;

	virtual time_t GetWhen() anope_abstract;
	virtual void SetWhen(const time_t &) anope_abstract;

	virtual time_t GetExpires() anope_abstract;
	virtual void SetExpires(const time_t &) anope_abstract;
};

namespace Event
{
	struct CoreExport NickSuspend : Events
	{
		static constexpr const char *NAME = "nicksuspend";

		using Events::Events;
		
		/** Called when a nick is suspended
		 * @param na The nick alias
		 */
		virtual void OnNickSuspend(NickServ::Nick *na) anope_abstract;
	};

	struct CoreExport NickUnsuspend : Events
	{
		static constexpr const char *NAME = "nickunsuspend";

		using Events::Events;
		
		/** Called when a nick is unsuspneded
		 * @param na The nick alias
		 */
		virtual void OnNickUnsuspend(NickServ::Nick *na) anope_abstract;
	};
}
