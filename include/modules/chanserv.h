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

#include "event.h"
#include "channels.h"
#include "modules/nickserv.h"
#include "modules/memoserv.h"
#include "bots.h"

#include "chanserv/channel.h"
#include "chanserv/privilege.h"
#include "chanserv/chanaccess.h"
#include "chanserv/level.h"
#include "chanserv/mode.h"

namespace ChanServ
{
	class Channel;
	using registered_channel_map = Anope::locale_hash_map<Channel *>;

	class ChanServService : public Service
	{
	 public:
		static constexpr const char *NAME = "chanserv";
		
		ChanServService(Module *m) : Service(m, NAME)
		{
		}

		virtual Channel *Find(const Anope::string &name) anope_abstract;
		virtual registered_channel_map& GetChannels() anope_abstract;

		/* Have ChanServ hold the channel, that is, join and set +nsti and wait
		 * for a few minutes so no one can join or rejoin.
		 */
		virtual void Hold(::Channel *c) anope_abstract;

		virtual void AddPrivilege(Privilege p) anope_abstract;
		virtual void RemovePrivilege(Privilege &p) anope_abstract;
		virtual Privilege *FindPrivilege(const Anope::string &name) anope_abstract;
		virtual std::vector<Privilege> &GetPrivileges() anope_abstract;
		virtual void ClearPrivileges() anope_abstract;
	};
	
	extern ChanServService *service;

	inline Channel *Find(const Anope::string name)
	{
		return service ? service->Find(name) : nullptr;
	}

	namespace Event
	{
		struct CoreExport PreChanExpire : Events
		{
			static constexpr const char *NAME = "prechanexpire";

			using Events::Events;
			
			/** Called before a channel expires
			 * @param ci The channel
			 * @param expire Set to true to allow the chan to expire
			 */
			virtual void OnPreChanExpire(Channel *ci, bool &expire) anope_abstract;
		};

		struct CoreExport ChanExpire : Events
		{
			static constexpr const char *NAME = "chanexpire";

			using Events::Events;
			
			/** Called before a channel expires
			 * @param ci The channel
			 */
			virtual void OnChanExpire(Channel *ci) anope_abstract;
		};
	}
}

