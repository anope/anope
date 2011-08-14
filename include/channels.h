/* Channel support
 *
 * (C) 2008-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#ifndef CHANNELS_H
#define CHANNELS_H

typedef Anope::insensitive_map<Channel *> channel_map;
extern CoreExport channel_map ChannelList;

struct UserContainer : public Extensible
{
	User *user;
	ChannelStatus *Status;

	UserContainer(User *u) : user(u) { }
	virtual ~UserContainer() { }
};

typedef std::list<UserContainer *> CUserList;

enum ChannelFlag
{
	/* ChanServ is currently holding the channel */
	CH_INHABIT,
	/* Channel still exists when emptied */
	CH_PERSIST,
	/* If set the channel is syncing users (channel was just created) and it should not be deleted */
	CH_SYNCING
};

const Anope::string ChannelFlagString[] = { "CH_INABIT", "CH_PERSIST", "CH_SYNCING", "CH_LOGCHAN", "" };

class CoreExport Channel : public Extensible, public Flags<ChannelFlag, 3>
{
 public:
	typedef std::multimap<ChannelModeName, Anope::string> ModeList;
 private:
	/** A map of channel modes with their parameters set on this channel
	 */
	ModeList modes;
	
 public:
	/** Default constructor
	 * @param name The channel name
	 * @param ts The time the channel was created
	 */
	Channel(const Anope::string &nname, time_t ts = Anope::CurTime);

	/** Default destructor
	 */
	~Channel();

	Anope::string name;		/* Channel name */
	ChannelInfo *ci;		/* Corresponding ChannelInfo */
	time_t creation_time;	/* When channel was created */

	/* List of users in the channel */
	CUserList users;

	Anope::string topic;		/* Current topic of the channel */
	Anope::string topic_setter;	/* Who set the topic */
	time_t topic_time;		/* When the topic was set*/

	time_t server_modetime;		/* Time of last server MODE */
	time_t chanserv_modetime;	/* Time of last check_modes() */
	int16 server_modecount;		/* Number of server MODEs this second */
	int16 chanserv_modecount;	/* Number of check_mode()'s this sec */
	int16 bouncy_modes;			/* Did we fail to set modes here? */

	/** Call if we need to unset all modes and clear all user status (internally).
	 * Only useful if we get a SJOIN with a TS older than what we have here
	 */
	void Reset();

	/** Restore the channel topic, set mlock (key), set stickied bans, etc
	 */
	void Sync();

	/** Join a user internally to the channel
	 * @param u The user
	 */
	void JoinUser(User *u);

	/** Remove a user internally from the channel
	 * @param u The user
	 */
	void DeleteUser(User *u);

	/** Check if the user is on the channel
	 * @param u The user
	 * @return A user container if found, else NULL
	 */
	UserContainer *FindUser(User *u);

	/** Check if a user has a status on a channel
	 * @param u The user
	 * @param cms The status mode, or NULL to represent no status
	 * @return true or false
	 */
	bool HasUserStatus(User *u, ChannelModeStatus *cms) const;

	/** Check if a user has a status on a channel
	 * Use the overloaded function for ChannelModeStatus* to check for no status
	 * @param u The user
	 * @param Name The Mode name, eg CMODE_OP, CMODE_VOICE
	 * @return true or false
	 */
	bool HasUserStatus(User *u, ChannelModeName Name) const;

	/** See if a channel has a mode
	 * @param Name The mode name
	 * @return The number of modes set
 	 * @param param The optional mode param
	 */
	size_t HasMode(ChannelModeName Name, const Anope::string &param = "");

	/** Get a list of modes on a channel
	 * @param Name A mode name to get the list of
	 * @return a pair of iterators for the beginning and end of the list
	 */
	std::pair<ModeList::iterator, ModeList::iterator> GetModeList(ChannelModeName Name);

	/** Set a mode internally on a channel, this is not sent out to the IRCd
	 * @param cm The mode
	 * @param param The param
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void SetModeInternal(ChannelMode *cm, const Anope::string &param = "", bool EnforceMLock = true);

	/** Remove a mode internally on a channel, this is not sent out to the IRCd
	 * @param cm The mode
	 * @param param The param
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveModeInternal(ChannelMode *cm, const Anope::string &param = "", bool EnforceMLock = true);

	/** Set a mode on a channel
	 * @param bi The client setting the modes
	 * @param cm The mode
	 * @param param Optional param arg for the mode
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void SetMode(BotInfo *bi, ChannelMode *cm, const Anope::string &param = "", bool EnforceMLock = true);

	/**
	 * Set a mode on a channel
	 * @param bi The client setting the modes
	 * @param Name The mode name
	 * @param param Optional param arg for the mode
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void SetMode(BotInfo *bi, ChannelModeName Name, const Anope::string &param = "", bool EnforceMLock = true);

	/** Remove a mode from a channel
	 * @param bi The client setting the modes
	 * @param cm The mode
	 * @param param Optional param arg for the mode
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveMode(BotInfo *bi, ChannelMode *cm, const Anope::string &param = "", bool EnforceMLock = true);

	/**
	 * Remove a mode from a channel
	 * @param bi The client setting the modes
	 * @param Name The mode name
	 * @param param Optional param arg for the mode
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveMode(BotInfo *bi, ChannelModeName Name, const Anope::string &param = "", bool EnforceMLock = true);

	/** Get a param from the channel
	 * @param Name The mode
	 * @param Target a string to put the param into
	 * @return true on success
	 */
	bool GetParam(ChannelModeName Name, Anope::string &Target) const;

	/** Set a string of modes on the channel
	 * @param bi The client setting the modes
	 * @param EnforceMLock Should mlock be enforced on this mode change
	 * @param cmodes The modes to set
	 */
	void SetModes(BotInfo *bi, bool EnforceMLock, const char *cmodes, ...);

	/** Set a string of modes internally on a channel
	 * @param setter the setter (if it is a user)
	 * @param mode the modes
	 * @param EnforceMLock true to enforce mlock
	 */
	void SetModesInternal(User *setter, const Anope::string &mode, bool EnforceMLock = true);

	/** Kick a user from a channel internally
	 * @param source The sender of the kick
	 * @param nick The nick being kicked
	 * @param reason The reason for the kick
	 */
	void KickInternal(const Anope::string &source, const Anope::string &nick, const Anope::string &reason);

	/** Kick a user from the channel
	 * @param bi The sender, can be NULL for the service bot for this channel
	 * @param u The user being kicked
	 * @param reason The reason for the kick
	 * @return true if the kick was scucessful, false if a module blocked the kick
	 */
	bool Kick(BotInfo *bi, User *u, const char *reason = NULL, ...);

	/** Get a string of the modes set on this channel
	 * @param complete Include mode parameters
	 * @param plus If set to false (with complete), mode parameters will not be given for modes requring no parameters to be unset
	 * @return A mode string
	 */
	Anope::string GetModes(bool complete, bool plus);

	/** Update the topic of the channel internally, and reset it if topiclock etc says to
	 * @param user THe user setting the new topic
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
};

#endif // CHANNELS_H
