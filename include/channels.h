/* Channel support
 *
 * (C) 2008-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * $Id$
 *
 */

enum ChannelFlags
{
	/* Channel still exists when emptied */
	CH_PERSIST,
	/* If set the channel is syncing users (channel was just created) and it should not be deleted */
	CH_SYNCING
};

class CoreExport Channel : public Extensible, public Flags<ChannelFlags>
{
 private:
	/** A map of channel modes with their parameters set on this channel
	 */
	std::map<ChannelModeName, std::string> Params;

 public:
	/** Default constructor
	 * @param name The channel name
	 * @param ts The time the channel was created
	 */
	Channel(const std::string &name, time_t ts = time(NULL));

	/** Default destructor
	 */
	~Channel();

	Channel *next, *prev;
	std::string name;	       /* Channel name */
	ChannelInfo *ci;			/* Corresponding ChannelInfo */
	time_t creation_time;	   /* When channel was created */
	char *topic;
	std::string topic_setter;
	time_t topic_time;		      /* When topic was set */
	std::bitset<128> modes;

	EList *bans;
	EList *excepts;
	EList *invites;

	struct c_userlist *users;
	uint16 usercount;

	BanData *bd;
				     
	time_t server_modetime;	 /* Time of last server MODE */
	time_t chanserv_modetime;       /* Time of last check_modes() */
	int16 server_modecount;	 /* Number of server MODEs this second */
	int16 chanserv_modecount;       /* Number of check_mode()'s this sec */
	int16 bouncy_modes;		     /* Did we fail to set modes here? */
	int16 topic_sync;		  /* Is the topic in sync? */

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

	/**
	 * See if a channel has a mode
	 * @param Name The mode name
	 * @return true or false
	 */
	bool HasMode(ChannelModeName Name);

	/** Set a mode internally on a channel, this is not sent out to the IRCd
	 * @param cm The mode
	 * @param param The param
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void SetModeInternal(ChannelMode *cm, const std::string &param = "", bool EnforceMLock = true);

	/** Remove a mode internally on a channel, this is not sent out to the IRCd
	 * @param cm The mode
	 * @param param The param
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveModeInternal(ChannelMode *cm, const std::string &param = "", bool EnforceMLock = true);

	/** Set a mode on a channel
	 * @param bi The client setting the modes
	 * @param cm The mode
	 * @param param Optional param arg for the mode
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void SetMode(BotInfo *bi, ChannelMode *cm, const std::string &param = "", bool EnforceMLock = true);

	/**
	 * Set a mode on a channel
	 * @param bi The client setting the modes
	 * @param Name The mode name
	 * @param param Optional param arg for the mode
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void SetMode(BotInfo *bi, ChannelModeName Name, const std::string &param = "", bool EnforceMLock = true);

	/**
	 * Set a mode on a channel
	 * @param bi The client setting the modes
	 * @param Mode The mode
	 * @param param Optional param arg for the mode
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void SetMode(BotInfo *bi, char Mode, const std::string &param = "", bool EnforceMLock = true);

	/** Remove a mode from a channel
	 * @param bi The client setting the modes
	 * @param cm The mode
	 * @param param Optional param arg for the mode
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveMode(BotInfo *bi, ChannelMode *cm, const std::string &param = "", bool EnforceMLock = true);

	/**
	 * Remove a mode from a channel
	 * @param bi The client setting the modes
	 * @param Name The mode name
	 * @param param Optional param arg for the mode
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveMode(BotInfo *bi, ChannelModeName Name, const std::string &param = "", bool EnforceMLock = true);
	/**
	 * Remove a mode from a channel
	 * @param bi The client setting the modes
	 * @param Mode The mode
	 * @param param Optional param arg for the mode
	 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
	 */
	void RemoveMode(BotInfo *bi, char Mode, const std::string &param = "", bool EnforceMLock = true);

	/** Clear all the modes from the channel
	 * @param bi The client unsetting the modes
	 */
	void ClearModes(BotInfo *bi = NULL);

	/** Clear all the bans from the channel
	 * @param bi The client unsetting the modes
	 */
	void ClearBans(BotInfo *bi = NULL);

	/** Clear all the excepts from the channel
	 * @param bi The client unsetting the modes
	 */
	void ClearExcepts(BotInfo *bi = NULL);

	/** Clear all the invites from the channel
	 * @param bi The client unsetting the modes
	 */
	void ClearInvites(BotInfo *bi = NULL);

	/** Get a param from the channel
	 * @param Name The mode
	 * @param Target a string to put the param into
	 * @return true on success
	 */
	const bool GetParam(ChannelModeName Name, std::string *Target);

	/** Check if a mode is set and has a param
	 * @param Name The mode
	 */
	const bool HasParam(ChannelModeName Name);
	/** Set a string of modes on the channel
	 * @param bi The client setting the modes
	 * @param EnforceMLock Should mlock be enforced on this mode change
	 * @param cmodes The modes to set
	 */
	void SetModes(BotInfo *bi, bool EnforceMLock, const char *cmodes, ...);

	/** Kick a user from a channel internally
	 * @param source The sender of the kick
	 * @param nick The nick being kicked
	 * @param reason The reason for the kick
	 */
	void KickInternal(const std::string &source, const std::string &nick, const std::string &reason);

	/** Kick a user from the channel
	 * @param bi The sender, can be NULL for the service bot for this channel
	 * @param u The user being kicked
	 * @param reason The reason for the kick
	 * @return true if the kick was scucessful, false if a module blocked the kick
	 */
	bool Kick(BotInfo *bi, User *u, const char *reason = NULL, ...);
};

