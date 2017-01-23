/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2016 Anope Team <team@anope.org>
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

#include "anope.h"
#include "extensible.h"
#include "modes.h"
#include "serialize.h"

using channel_map = Anope::locale_hash_map<Channel *>;

extern CoreExport channel_map ChannelList;

/* A user container, there is one of these per user per channel. */
struct ChanUserContainer : public Extensible
{
	User *user;
	Channel *chan;
	/* Status the user has in the channel */
	ChannelStatus status;

	ChanUserContainer(User *u, Channel *c) : user(u), chan(c) { }
};

class CoreExport Channel : public Base, public Extensible
{
	static std::vector<Channel *> deleting;

 public:
	typedef std::multimap<Anope::string, Anope::string> ModeList;
 private:
	/** A map of channel modes with their parameters set on this channel
	 */
	ModeList modes;

 public:
 	/* Channel name */
	Anope::string name;
	/* Set if this channel is registered. ci->c == this. Contains information relevant to the registered channel */
	Serialize::Reference<ChanServ::Channel> ci;
	/* When the channel was created */
	time_t creation_time;
	/* If the channel has just been created in a netjoin */
	bool syncing = false;
	/* Is configured in the conf as a channel bots should be in */
	bool botchannel = false;

	/* Users in the channel */
	typedef std::map<User *, ChanUserContainer *> ChanUserList;
	ChanUserList users;

	/* Current topic of the channel */
	Anope::string topic;
	/* Who set the topic */
	Anope::string topic_setter;
	/* The timestamp associated with the topic. Not necessarily anywhere close to Anope::CurTime.
	 * This is the time the topic was *originally set*. When we restore the topic we want to change the TS back
	 * to this, but we can only do this on certain IRCds.
	 */
	time_t topic_ts = 0;
	/* The actual time the topic was set, probably close to Anope::CurTime */
	time_t topic_time = 0;

	time_t server_modetime = 0;	/* Time of last server MODE */
	time_t chanserv_modetime = 0;	/* Time of last check_modes() */
	int16_t server_modecount = 0;	/* Number of server MODEs this second */
	int16_t chanserv_modecount = 0;	/* Number of check_mode()'s this sec */
	int16_t bouncy_modes = 0;		/* Did we fail to set modes here? */

	Logger logger;

 private:
	/** Constructor
	 * @param name The channel name
	 * @param ts The time the channel was created
	 */
	Channel(const Anope::string &nname, time_t ts = Anope::CurTime);

 public:
	/** Destructor
	 */
	~Channel();

	/** Gets the channels name
	 * @return the channel name
	 */
	const Anope::string &GetName() const;

	/** Call if we need to unset all modes and clear all user status (internally).
	 * Only useful if we get a SJOIN with a TS older than what we have here
	 */
	void Reset();

	/** Restore the channel topic, set mlock (key), set stickied bans, etc
	 */
	void Sync();

	/** Check if a channels modes are correct.
	 */
	void CheckModes();

	/** Check if this channel should be deleted
	 */
	bool CheckDelete();

	/** Join a user internally to the channel
	 * @param u The user
	 * @param status The status to give the user, if any
	 * @return The UserContainer for the user
	 */
	ChanUserContainer* JoinUser(User *u, const ChannelStatus *status);

	/** Remove a user internally from the channel
	 * @param u The user
	 */
	void DeleteUser(User *u);

	/** Check if the user is on the channel
	 * @param u The user
	 * @return A user container if found, else NULL
	 */
	ChanUserContainer *FindUser(User *u) const;

	/** Check if a user has a status on a channel
	 * @param u The user
	 * @param cms The status mode, or NULL to represent no status
	 * @return true or false
	 */
	bool HasUserStatus(User *u, ChannelModeStatus *cms);

	/** Check if a user has a status on a channel
	 * Use the overloaded function for ChannelModeStatus* to check for no status
	 * @param u The user
	 * @param name The mode name, eg CMODE_OP, CMODE_VOICE
	 * @return true or false
	 */
	bool HasUserStatus(User *u, const Anope::string &name);

	/** See if a channel has a mode
	 * @param name The mode name
	 * @return The number of modes set
 	 * @param param The optional mode param
	 */
	size_t HasMode(const Anope::string &name, const Anope::string &param = "");

	/** Set a mode internally on a channel, this is not sent out to the IRCd
	 * @param setter The setter
	 * @param cm The mode
	 * @param param The param
	 * @param enforce_mlock true if mlocks should be enforced, false to override mlock
	 */
	void SetModeInternal(const MessageSource &source, ChannelMode *cm, const Anope::string &param = "", bool enforce_mlock = true);

	/** Remove a mode internally on a channel, this is not sent out to the IRCd
	 * @param setter The Setter
	 * @param cm The mode
	 * @param param The param
	 * @param enforce_mlock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveModeInternal(const MessageSource &source, ChannelMode *cm, const Anope::string &param = "", bool enforce_mlock = true);

	/** Set a mode on a channel
	 * @param bi The client setting the modes
	 * @param cm The mode
	 * @param param Optional param arg for the mode
	 * @param enforce_mlock true if mlocks should be enforced, false to override mlock
	 */
	void SetMode(User *bi, ChannelMode *cm, const Anope::string &param = "", bool enforce_mlock = true);

	/**
	 * Set a mode on a channel
	 * @param bi The client setting the modes
	 * @param name The mode name
	 * @param param Optional param arg for the mode
	 * @param enforce_mlock true if mlocks should be enforced, false to override mlock
	 */
	void SetMode(User *bi, const Anope::string &name, const Anope::string &param = "", bool enforce_mlock = true);

	/** Remove a mode from a channel
	 * @param bi The client setting the modes
	 * @param cm The mode
	 * @param param Optional param arg for the mode
	 * @param enforce_mlock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveMode(User *bi, ChannelMode *cm, const Anope::string &param = "", bool enforce_mlock = true);

	/**
	 * Remove a mode from a channel
	 * @param bi The client setting the modes
	 * @param name The mode name
	 * @param param Optional param arg for the mode
	 * @param enforce_mlock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveMode(User *bi, const Anope::string &name, const Anope::string &param = "", bool enforce_mlock = true);

	/** Get a modes parameter for the channel
	 * @param name The mode
	 * @param target a string to put the param into
	 * @return true if the parameter was fetched, false if on error (mode not set) etc.
	 */
	bool GetParam(const Anope::string &name, Anope::string &target) const;

	/** Set a string of modes on the channel
	 * @param bi The client setting the modes
	 * @param enforce_mlock Should mlock be enforced on this mode change
	 * @param cmodes The modes to set
	 */
	void SetModes(User *bi, bool enforce_mlock, const char *cmodes, ...);

	/** Set a string of modes internally on a channel
	 * @param source The setter
	 * @param mode the modes
	 * @param enforce_mlock true to enforce mlock
	 */
	void SetModesInternal(MessageSource &source, const Anope::string &mode, time_t ts = 0, bool enforce_mlock = true);

	/** Does the given user match the given list? (CMODE_BAN, CMODE_EXCEPT, etc, a list mode)
	 * @param u The user
	 * @param list The mode of the list to check (eg CMODE_BAN)
	 * @return true if the user matches the list
	 */
	bool MatchesList(User *u, const Anope::string &list);

	/** Kick a user from a channel internally
	 * @param source The sender of the kick
	 * @param nick The nick being kicked
	 * @param reason The reason for the kick
	 * @return true if the kick was scucessful, false if a module blocked the kick or was otherwise unsuccessful
	 */
	bool KickInternal(const MessageSource &source, const Anope::string &nick, const Anope::string &reason);

	/** Kick a user from the channel
	 * @param bi The sender, can be NULL for the service bot for this channel
	 * @param u The user being kicked
	 * @param reason The reason for the kick
	 * @return true if the kick was scucessful, false if a module blocked the kick
	 */
	bool Kick(User *bi, User *u, const Anope::string &reason);

	template<typename... Args> bool Kick(User *bi, User *u, const Anope::string &reason, Args&&... args)
	{
		return Kick(bi, u, Anope::Format(reason, std::forward<Args>(args)...));
	}

	/** Get all modes set on this channel, excluding status modes.
	 * @return a map of modes and their optional parameters.
	 */
	const ModeList &GetModes() const;

	/** Get a list of modes on a channel
	 * @param name A mode name to get the list of
	 * @return a vector of the list mode entries
	 */
	std::vector<Anope::string> GetModeList(const Anope::string &name);

	/** Get a string of the modes set on this channel
	 * @param complete Include mode parameters
	 * @param plus If set to false (with complete), mode parameters will not be given for modes requring no parameters to be unset
	 * @return A mode string
	 */
	Anope::string GetModes(bool complete, bool plus);

	/** Update the topic of the channel internally, and reset it if topiclock etc says to
	 * @param user The user setting the new topic
	 * @param newtopic The new topic
	 * @param ts The time the new topic is being set
	 */
	void ChangeTopicInternal(User *u, const Anope::string &user, const Anope::string &newtopic, time_t ts = Anope::CurTime);

	/** Update the topic of the channel, and reset it if topiclock etc says to
	 * @param user The user setting the topic
	 * @param newtopic The new topic
	 * @param ts The time when the new topic is being set
	 */
	void ChangeTopic(const Anope::string &user, const Anope::string &newtopic, time_t ts = Anope::CurTime);

	/** Set the correct modes, or remove the ones granted without permission,
	 * for the specified user.
	 * @param user The user to give/remove modes to/from
	 * @param give_modes if true modes may be given to the user
	 */
	void SetCorrectModes(User *u, bool give_modes);

	/** Unbans a user from this channel.
	 * @param u The user to unban
	 * @param mode The mode to unban
	 * @param full Whether or not to match using the user's real host and IP
	 * @return whether or not a ban was removed
	 */
	bool Unban(User *u, const Anope::string &mode, bool full = false);

	/** Check whether a user is permitted to be on this channel
	 * @param u The user
	 * @return true if they are allowed, false if they aren't and were kicked
	 */
	bool CheckKick(User *user);

	/** Finds a channel
	 * @param name The channel to find
	 * @return The channel, if found
	 */
	static Channel* Find(const Anope::string &name);

	/** Finds or creates a channel
	 * @param name The channel name
	 * @param created Set to true if the channel was just created
	 * @param ts The time the channel was created
	 */
	static Channel *FindOrCreate(const Anope::string &name, bool &created, time_t ts = Anope::CurTime);

	void QueueForDeletion();

	static void DeleteChannels();
};

