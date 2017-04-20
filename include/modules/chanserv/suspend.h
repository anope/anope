/*
 * Anope IRC Services
 *
 * Copyright (C) 2014-2017 Anope Team <team@anope.org>
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

class CSSuspendInfo : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;

 public:
	static constexpr const char *const NAME = "cssuspendinfo";
	 
	virtual ChanServ::Channel *GetChannel() anope_abstract;
	virtual void SetChannel(ChanServ::Channel *) anope_abstract;

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
	struct CoreExport ChanSuspend : Events
	{
		static constexpr const char *NAME = "chansuspend";

		using Events::Events;
		
		/** Called when a channel is suspended
		 * @param ci The channel
		 */
		virtual void OnChanSuspend(ChanServ::Channel *ci) anope_abstract;
	};
	struct CoreExport ChanUnsuspend : Events
	{
		static constexpr const char *NAME = "chanunsuspend";

		using Events::Events;
		
		/** Called when a channel is unsuspended
		 * @param ci The channel
		 */
		virtual void OnChanUnsuspend(ChanServ::Channel *ci) anope_abstract;
	};
}

