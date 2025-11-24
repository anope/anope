// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
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

#include "memo.h"
#include "modes.h"
#include "extensible.h"
#include "logger.h"
#include "modules.h"
#include "serialize.h"
#include "bots.h"

typedef Anope::unordered_map<ChannelInfo *> registered_channel_map;

extern CoreExport Serialize::Checker<registered_channel_map> RegisteredChannelList;

/* It matters that Base is here before Extensible (it is inherited by Serializable)
 */
class CoreExport ChannelInfo final
	: public Serializable
	, public Extensible
{
public:
	struct Type final
		: public Serialize::Type
	{
		Type();
		void Serialize(Serializable *obj, Serialize::Data &data) const override;
		Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override;
	};

private:
	/* channels who reference this one */
	Anope::map<int> references;
	Serialize::Reference<NickCore> founder;					/* Channel founder */
	Serialize::Reference<NickCore> successor;                               /* Who gets the channel if the founder nick is dropped or expires */
	Serialize::Checker<std::vector<ChanAccess *> > access;			/* List of authorized users */
	Anope::map<int16_t> levels;

public:
	friend class ChanAccess;
	friend class AutoKick;

	Anope::string name;                       /* Channel name */
	Anope::string desc;

	time_t registered;
	time_t last_used;

	Anope::string last_topic;                 /* The last topic that was set on this channel */
	Anope::string last_topic_setter;          /* Setter */
	time_t last_topic_time = 0;               /* Time */

	Channel::ModeList last_modes;             /* The last modes set on this channel */

	int16_t bantype = 2;

	MemoInfo memos;

	Channel *c;                               /* Pointer to channel, if the channel exists */

	/* For BotServ */
	Serialize::Reference<BotInfo> bi;         /* Bot used on this channel */

	time_t banexpire = 0;                     /* Time bans expire in */

	/** Constructor
	 * @param chname The channel name
	 */
	ChannelInfo(const Anope::string &chname);

	/** Copy constructor
	 * @param ci The ChannelInfo to copy settings from
	 */
	ChannelInfo(const ChannelInfo &ci);

	~ChannelInfo();
	ChannelInfo &operator=(const ChannelInfo &) = default;

	/** Change the founder of the channel
	 * @params nc The new founder
	 */
	void SetFounder(NickCore *nc);

	/** Get the founder of the channel
	 * @return The founder
	 */
	NickCore *GetFounder() const;

	void SetSuccessor(NickCore *nc);
	NickCore *GetSuccessor() const;

	/** Find which bot should send mode/topic/etc changes for this channel
	 * @return The bot
	 */
	BotInfo *WhoSends() const;

	/** Add an entry to the channel access list
	 * @param access The entry
	 */
	void AddAccess(ChanAccess *access);

	/** Get an entry from the channel access list by index
	 *
	 * @param index The index in the access list vector
	 * @return A ChanAccess struct corresponding to the index given, or NULL if outside the bounds
	 *
	 * Retrieves an entry from the access list that matches the given index.
	 */
	ChanAccess *GetAccess(unsigned index) const;

	/** Retrieve the access for a user or group in the form of a vector of access entries
	 * (as multiple entries can affect a single user).
	 */
	AccessGroup AccessFor(const User *u, bool updateLastUsed = true);
	AccessGroup AccessFor(const NickCore *nc, bool updateLastUsed = true);

	/** Get the size of the access vector for this channel
	 * @return The access vector size
	 */
	unsigned GetAccessCount() const;

	/** Get the number of access entries for this channel,
	 * including those that are on other channels.
	 */
	unsigned GetDeepAccessCount() const;

	/** Erase an entry from the channel access list
	 *
	 * @param index The index in the access list vector
	 *
	 * @return The erased entry
	 */
	ChanAccess *EraseAccess(unsigned index);

	/** Clear the entire channel access list
	 *
	 * Clears the entire access list by deleting every item and then clearing the vector.
	 */
	void ClearAccess();

	/** Get the level entries for the channel.
	 * @return The levels for the channel.
	 */
	const Anope::map<int16_t> &GetLevelEntries();

	/** Get the level for a privilege
	 * @param priv The privilege name
	 * @return the level
	 * @throws CoreException if priv is not a valid privilege
	 */
	int16_t GetLevel(const Anope::string &priv) const;

	/** Set the level for a privilege
	 * @param priv The privilege priv
	 * @param level The new level
	 */
	void SetLevel(const Anope::string &priv, int16_t level);

	/** Remove a privilege from the channel
	 * @param priv The privilege
	 */
	void RemoveLevel(const Anope::string &priv);

	/** Clear all privileges from the channel
	 */
	void ClearLevels();

	/** Gets a ban mask for the given user based on the bantype
	 * of the channel.
	 * @param u The user
	 * @return A ban mask that affects the user
	 */
	Anope::string GetIdealBan(User *u) const;

	/** Finds a ChannelInfo
	 * @param name channel name to lookup
	 * @return the ChannelInfo associated with the channel
	 */
	static ChannelInfo *Find(const Anope::string &name);

	void AddChannelReference(const Anope::string &what);
	void RemoveChannelReference(const Anope::string &what);
	void GetChannelReferences(std::deque<Anope::string> &chans);
};

/** Is the user the real founder?
 * @param user The user
 * @param ci The channel
 * @return true or false
 */
extern CoreExport bool IsFounder(const User *user, const ChannelInfo *ci);
