// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#define CHANSERV_AUTO_KICK_TYPE "AutoKick"
#define CHANSERV_AUTO_KICK_SERVICE "ChanServ::AutoKickService"

namespace ChanServ
{
	class AutoKick;
	class AutoKickService;

	ServiceReference<AutoKickService> akick_service(CHANSERV_AUTO_KICK_SERVICE, CHANSERV_AUTO_KICK_SERVICE);
}

class ChanServ::AutoKickService
	: public Service
{
public:
	AutoKickService(Module *m)
		: Service(m, CHANSERV_AUTO_KICK_SERVICE, CHANSERV_AUTO_KICK_SERVICE)
	{
	}

	/** Add an akick entry to the channel by NickCore
	 * @param user The user who added the akick
	 * @param akicknc The nickcore being akicked
	 * @param reason The reason for the akick
	 * @param t The time the akick was added, defaults to now
	 * @param lu The time the akick was last used, defaults to never
	 */
	virtual AutoKick *AddAKick(ChannelInfo *ci, const Anope::string &user, NickCore *akicknc, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0) = 0;

	/** Add an akick entry to the channel by reason
	 * @param user The user who added the akick
	 * @param mask The mask of the akick
	 * @param reason The reason for the akick
	 * @param t The time the akick was added, defaults to now
	 * @param lu The time the akick was last used, defaults to never
	 */
	virtual AutoKick *AddAKick(ChannelInfo *ci, const Anope::string &user, const Anope::string &mask, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0)  = 0;

	/** Get an entry from the channel akick list
	 * @param index The index in the akick vector
	 * @return The akick structure, or NULL if not found
	 */
	virtual AutoKick *GetAKick(ChannelInfo *ci, unsigned index) = 0;

	/** Get the size of the akick vector for this channel
	 * @return The akick vector size
	 */
	virtual unsigned GetAKickCount(ChannelInfo *ci) = 0;

	/** Erase an entry from the channel akick list
	 * @param index The index of the akick
	 */
	virtual void EraseAKick(ChannelInfo *ci, unsigned index) = 0;
	virtual void EraseAKick(ChannelInfo *ci, AutoKick *akick) = 0;

	/** Clear the whole akick list
	 */
	virtual void ClearAKick(ChannelInfo *ci) = 0;
};

class ChanServ::AutoKick final
	: public Serializable
{
public:
	/* Channel this autokick is on */
	Serialize::Reference<ChannelInfo> ci;

	Anope::string mask;
	Serialize::Reference<NickCore> nc;

	Anope::string reason;
	Anope::string creator;
	time_t addtime;
	time_t last_used;

	AutoKick()
		: Serializable(CHANSERV_AUTO_KICK_TYPE)
	{
	}

	~AutoKick()
	{
		if (!this->ci)
			return;

		if (ChanServ::akick_service)
			ChanServ::akick_service->EraseAKick(this->ci, this);

		if (this->nc)
			this->nc->RemoveChannelReference(this->ci);
	}
};
