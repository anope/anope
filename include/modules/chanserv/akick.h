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

#include "modules/nickserv.h"

 /* AutoKick data. */
class CoreExport AutoKick : public Serialize::Object
{
 protected:
	using Serialize::Object::Object;
 public:
	static constexpr const char *const NAME = "akick";
	 
	virtual ~AutoKick() = default;

	virtual ChanServ::Channel *GetChannel() anope_abstract;
	virtual void SetChannel(ChanServ::Channel *) anope_abstract;

	virtual Anope::string GetMask() anope_abstract;
	virtual void SetMask(const Anope::string &) anope_abstract;

	virtual NickServ::Account *GetAccount() anope_abstract;
	virtual void SetAccount(NickServ::Account *) anope_abstract;

	virtual Anope::string GetReason() anope_abstract;
	virtual void SetReason(const Anope::string &) anope_abstract;

	virtual Anope::string GetCreator() anope_abstract;
	virtual void SetCreator(const Anope::string &) anope_abstract;

	virtual time_t GetAddTime() anope_abstract;
	virtual void SetAddTime(const time_t &) anope_abstract;

	virtual time_t GetLastUsed() anope_abstract;
	virtual void SetLastUsed(const time_t &) anope_abstract;
};

namespace Event
{
	struct CoreExport Akick : Events
	{
		static constexpr const char *NAME = "akick";

		using Events::Events;

		/** Called after adding an akick to a channel
		 * @param source The source of the command
		 * @param ci The channel
		 * @param ak The akick
		 */
		virtual void OnAkickAdd(CommandSource &source, ChanServ::Channel *ci, const AutoKick *ak) anope_abstract;

		/** Called before removing an akick from a channel
		 * @param source The source of the command
		 * @param ci The channel
		 * @param ak The akick
		 */
		virtual void OnAkickDel(CommandSource &source, ChanServ::Channel *ci, const AutoKick *ak) anope_abstract;
	};
}

