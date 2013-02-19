/* Channel support
 *
 * (C) 2008-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#ifndef CHANNELS_H
#define CHANNELS_H

#include "anope.h"
#include "extensible.h"
#include "modes.h"
#include "serialize.h"

typedef Anope::hash_map<Channel *> channel_map;

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

enum ChannelFlag
{
	/* ChanServ is currently holding the channel */
	CH_INHABIT,
	/* Channel still exists when emptied */
	CH_PERSIST,
	/* If set the channel is syncing users (channel was just created) and it should not be deleted */
	CH_SYNCING
};

class CoreExport Channel : public Base, public Extensible
{
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
	Serialize::Reference<ChannelInfo> ci;
	/* When the channel was created */
	time_t creation_time;
	std::set<ChannelFlag> flags;

	/* Users in the channel */
	typedef std::list<ChanUserContainer *> ChanUserList;
	ChanUserList users;

	/* Current topic of the channel */
	Anope::string topic;
	/* Who set the topic */
	Anope::string topic_setter;
	/* The timestamp associated with the topic. Not necessarually anywhere close to Anope::CurTime.
	 * This is the time the topic was *originally set*. When we restore the topic we want to change the TS back
	 * to this, but we can only do this on certain IRCds.
	 */
	time_t topic_ts;
	/* The actual time the topic was set, probably close to Anope::CurTime */
	time_t topic_time;

	time_t server_modetime;		/* Time of last server MODE */
	time_t chanserv_modetime;	/* Time of last check_modes() */
	int16_t server_modecount;	/* Number of server MODEs this second */
	int16_t chanserv_modecount;	/* Number of check_mode()'s this sec */
	int16_t bouncy_modes;		/* Did we fail to set modes here? */

	/** Constructor
	 * @param name The channel name
	 * @param ts The time the channel was created
	 */
	Channel(const Anope::string &nname, time_t ts = Anope::CurTime);

	/** Destructor
	 */
	~Channel();

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

	/** Join a user internally to the channel
	 * @param u The user
	 * @return The UserContainer for the user
	 */
	ChanUserContainer* JoinUser(User *u);

	/** Remove a user internally from the channel
	 * @param u The user
	 */
	void DeleteUser(User *u);

	/** Check if the user is on the channel
	 * @param u The user
	 * @return A user container if found, else NULL
	 */
	ChanUserContainer *FindUser(const User *u) const;

	/** Check if a user has a status on a channel
	 * @param u The user
	 * @param cms The status mode, or NULL to represent no status
	 * @return true or false
	 */
	bool HasUserStatus(const User *u, ChannelModeStatus *cms) const;

	/** Check if a user has a status on a channel
	 * Use the overloaded function for ChannelModeStatus* to check for no status
	 * @param u The user
	 * @param name The mode name, eg CMODE_OP, CMODE_VOICE
	 * @return true or false
	 */
	bool HasUserStatus(const User *u, const Anope::string &name) const;

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
	void SetModeInternal(MessageSource &source, ChannelMode *cm, const Anope::string &param = "", bool enforce_mlock = true);

	/** Remove a mode internally on a channel, this is not sent out to the IRCd
	 * @param setter The Setter
	 * @param cm The mode
	 * @param param The param
	 * @param enforce_mlock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveModeInternal(MessageSource &source, ChannelMode *cm, const Anope::string &param = "", bool enforce_mlock = true);

	/** Set a mode on a channel
	 * @param bi The client setting the modes
	 * @param cm The mode
	 * @param param Optional param arg for the mode
	 * @param enforce_mlock true if mlocks should be enforced, false to override mlock
	 */
	void SetMode(BotInfo *bi, ChannelMode *cm, const Anope::string &param = "", bool enforce_mlock = true);

	/**
	 * Set a mode on a channel
	 * @param bi The client setting the modes
	 * @param name The mode name
	 * @param param Optional param arg for the mode
	 * @param enforce_mlock true if mlocks should be enforced, false to override mlock
	 */
	void SetMode(BotInfo *bi, const Anope::string &name, const Anope::string &param = "", bool enforce_mlock = true);

	/** Remove a mode from a channel
	 * @param bi The client setting the modes
	 * @param cm The mode
	 * @param param Optional param arg for the mode
	 * @param enforce_mlock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveMode(BotInfo *bi, ChannelMode *cm, const Anope::string &param = "", bool enforce_mlock = true);

	/**
	 * Remove a mode from a channel
	 * @param bi The client setting the modes
	 * @param name The mode name
	 * @param param Optional param arg for the mode
	 * @param enforce_mlock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveMode(BotInfo *bi, const Anope::string &name, const Anope::string &param = "", bool enforce_mlock = true);

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
	void SetModes(BotInfo *bi, bool enforce_mlock, const char *cmodes, ...);

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
	 */
	void KickInternal(MessageSource &source, const Anope::string &nick, const Anope::string &reason);

	/** Kick a user from the channel
	 * @param bi The sender, can be NULL for the service bot for this channel
	 * @param u The user being kicked
	 * @param reason The reason for the kick
	 * @return true if the kick was scucessful, false if a module blocked the kick
	 */
	bool Kick(BotInfo *bi, User *u, const char *reason = NULL, ...);

	/** Get all modes set on this channel, excluding status modes.
	 * @return a map of modes and their optional parameters.
	 */
	const ModeList &GetModes() const;

	/** Get a list of modes on a channel
	 * @param name A mode name to get the list of
	 * @return a pair of iterators for the beginning and end of the list
	 */
	std::pair<ModeList::iterator, ModeList::iterator> GetModeList(const Anope::string &name);

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
	void ChangeTopicInternal(const Anope::string &user, const Anope::string &newtopic, time_t ts = Anope::CurTime);

	/** Update the topic of the channel, and reset it if topiclock etc says to
	 * @param user The user setting the topic
	 * @param newtopic The new topic
	 * @param ts The time when the new topic is being set
	 */
	void ChangeTopic(const Anope::string &user, const Anope::string &newtopic, time_t ts = Anope::CurTime);

	/** Hold the channel open using ChanServ
	 */
	void Hold();

	/** Set the correct modes, or remove the ones granted without permission,
	 * for the specified user.
	 * @param user The user to give/remove modes to/from
	 * @param give_modes if true modes may be given to the user
	 * @param check_noop if true, CI_NOAUTOOP is checked before giving modes
	 */
	void SetCorrectModes(User *u, bool give_mode, bool check_noop);

	/** Unbans a user from this channel.
	 * @param u The user to unban
	 * @param full Whether or not to match using the user's real host and IP
	 * @return whether or not a ban was removed
	 */
	bool Unban(const User *u, bool full = false);

	/** Finds a channel
	 * @param name The channel to find
	 * @return The channel, if found
	 */
	static Channel* Find(const Anope::string &name);
};

#endif // CHANNELS_H
