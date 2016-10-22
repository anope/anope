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

class CoreExport Channel : public Serialize::Object
{
 public:
	::Channel *c = nullptr;                               /* Pointer to channel, if the channel exists */

 protected:
	using Serialize::Object::Object;

 public:
	static constexpr const char *const NAME = "channel";

	virtual Anope::string GetName() anope_abstract;
	virtual void SetName(const Anope::string &) anope_abstract;

	virtual Anope::string GetDesc() anope_abstract;
	virtual void SetDesc(const Anope::string &) anope_abstract;

	virtual time_t GetTimeRegistered() anope_abstract;
	virtual void SetTimeRegistered(const time_t &) anope_abstract;

	virtual time_t GetLastUsed() anope_abstract;
	virtual void SetLastUsed(const time_t &) anope_abstract;

	virtual Anope::string GetLastTopic() anope_abstract;
	virtual void SetLastTopic(const Anope::string &) anope_abstract;

	virtual Anope::string GetLastTopicSetter() anope_abstract;
	virtual void SetLastTopicSetter(const Anope::string &) anope_abstract;

	virtual time_t GetLastTopicTime() anope_abstract;
	virtual void SetLastTopicTime(const time_t &) anope_abstract;

	virtual int16_t GetBanType() anope_abstract;
	virtual void SetBanType(const int16_t &) anope_abstract;

	virtual time_t GetBanExpire() anope_abstract;
	virtual void SetBanExpire(const time_t &) anope_abstract;

	virtual BotInfo *GetBI() anope_abstract;
	virtual void SetBI(BotInfo *) anope_abstract;

	virtual ServiceBot *GetBot() anope_abstract;
	virtual void SetBot(ServiceBot *) anope_abstract;

	/** Is the user the real founder?
	 * @param user The user
	 * @return true or false
	 */
	virtual bool IsFounder(const User *user) anope_abstract;

	/** Change the founder of the channel
	 * @params nc The new founder
	 */
	virtual void SetFounder(NickServ::Account *nc) anope_abstract;

	/** Get the founder of the channel
	 * @return The founder
	 */
	virtual NickServ::Account *GetFounder() anope_abstract;

	virtual void SetSuccessor(NickServ::Account *nc) anope_abstract;
	virtual NickServ::Account *GetSuccessor() anope_abstract;

	/** Find which bot should send mode/topic/etc changes for this channel
	 * @return The bot
	 */
	ServiceBot *WhoSends()
	{
		if (this)
			if (ServiceBot *bi = GetBot())
				return bi;

		ServiceBot *ChanServ = Config->GetClient("ChanServ");
		if (ChanServ)
			return ChanServ;

#warning "if(this)"
		//XXX
//			if (!BotListByNick->empty())
//				return BotListByNick->begin()->second;

		return NULL;
	}

	/** Get an entry from the channel access list by index
	 *
	 * @param index The index in the access list vector
	 * @return A ChanAccess struct corresponding to the index given, or NULL if outside the bounds
	 *
	 * Retrieves an entry from the access list that matches the given index.
	 */
	virtual ChanAccess *GetAccess(unsigned index) /*const*/ anope_abstract;

	/** Retrieve the access for a user or group in the form of a vector of access entries
	 * (as multiple entries can affect a single user).
	 */
	virtual AccessGroup AccessFor(const User *u, bool updateLastUsed = true) anope_abstract;
	virtual AccessGroup AccessFor(NickServ::Account *nc, bool updateLastUsed = true) anope_abstract;

	/** Get the size of the access vector for this channel
	 * @return The access vector size
	 */
	virtual unsigned GetAccessCount() anope_abstract;

	/** Clear the entire channel access list
	 *
	 * Clears the entire access list by deleting every item and then clearing the vector.
	 */
	virtual void ClearAccess() anope_abstract;

	/** Add an akick entry to the channel by NickServ::Account
	 * @param user The user who added the akick
	 * @param akicknc The nickcore being akicked
	 * @param reason The reason for the akick
	 * @param t The time the akick was added, defaults to now
	 * @param lu The time the akick was last used, defaults to never
	 */
	virtual AutoKick* AddAkick(const Anope::string &user, NickServ::Account *akicknc, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0) anope_abstract;

	/** Add an akick entry to the channel by reason
	 * @param user The user who added the akick
	 * @param mask The mask of the akick
	 * @param reason The reason for the akick
	 * @param t The time the akick was added, defaults to now
	 * @param lu The time the akick was last used, defaults to never
	 */
	virtual AutoKick* AddAkick(const Anope::string &user, const Anope::string &mask, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0) anope_abstract;

	/** Get an entry from the channel akick list
	 * @param index The index in the akick vector
	 * @return The akick structure, or NULL if not found
	 */
	virtual AutoKick* GetAkick(unsigned index) anope_abstract;

	/** Get the size of the akick vector for this channel
	 * @return The akick vector size
	 */
	virtual unsigned GetAkickCount() anope_abstract;

	/** Clear the whole akick list
	 */
	virtual void ClearAkick() anope_abstract;

	/** Get the level for a privilege
	 * @param priv The privilege name
	 * @return the level
	 * @throws CoreException if priv is not a valid privilege
	 */
	virtual int16_t GetLevel(const Anope::string &priv) anope_abstract;

	/** Set the level for a privilege
	 * @param priv The privilege priv
	 * @param level The new level
	 */
	virtual void SetLevel(const Anope::string &priv, int16_t level) anope_abstract;

	/** Remove a privilege from the channel
	 * @param priv The privilege
	 */
	virtual void RemoveLevel(const Anope::string &priv) anope_abstract;

	/** Clear all privileges from the channel
	 */
	virtual void ClearLevels() anope_abstract;

	/** Gets a ban mask for the given user based on the bantype
	 * of the channel.
	 * @param u The user
	 * @return A ban mask that affects the user
	 */
	virtual Anope::string GetIdealBan(User *u) anope_abstract;

	virtual MemoServ::MemoInfo *GetMemos() anope_abstract;
};


} // namespace ChanServ