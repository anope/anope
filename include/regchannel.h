/* Modular support
 *
 * (C) 2008-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#ifndef REGCHANNEL_H
#define REGCHANNEL_H

typedef unordered_map_namespace::unordered_map<ci::string, ChannelInfo *, hash_compare_ci_string> registered_channel_map;
extern CoreExport registered_channel_map RegisteredChannelList;

/** Flags used for the ChannelInfo class
 */
enum ChannelInfoFlag
{
	CI_BEGIN,

	/* ChanServ is currently holding the channel */
	CI_INHABIT,
	/* Retain the topic even after the channel is emptied */
	CI_KEEPTOPIC,
	/* Don't allow non-authorized users to be opped */
	CI_SECUREOPS,
	/* Hide channel from ChanServ LIST command */
	CI_PRIVATE,
	/* Topic can only be changed by SET TOPIC */
	CI_TOPICLOCK,
	/* Only users on the access list may join */
	CI_RESTRICTED,
	/* Don't allow ChanServ and BotServ commands to do bad things to users with higher access levels */
	CI_PEACE,
	/* Don't allow any privileges unless a user is IDENTIFIED with NickServ */
	CI_SECURE,
	/* Don't allow the channel to be registered or used */
	CI_FORBIDDEN,
	/* Channel does not expire */
	CI_NO_EXPIRE,
	/* Channel memo limit may not be changed */
	CI_MEMO_HARDMAX,
	/* Send notice to channel on use of OP/DEOP */
	CI_OPNOTICE,
	/* Stricter control of channel founder status */
	CI_SECUREFOUNDER,
	/* Sign kicks with the user who did the kick */
	CI_SIGNKICK,
	/* Sign kicks if level is < than the one defined by the SIGNKIGK level */
	CI_SIGNKICK_LEVEL,
	/* Uses XOP */
	CI_XOP,
	/* Channel is suspended */
	CI_SUSPENDED,
	/* Channel still exists when emptied, this can be caused by setting a perm
	 * channel mode (+P on InspIRCd) or /cs set #chan persist on.
	 * This keeps the service bot in the channel regardless if a +P type mode
	 * is set or not
	 */
	CI_PERSIST,

	CI_END
};

class CoreExport ChannelInfo : public Extensible, public Flags<ChannelInfoFlag, CI_END>
{
 private:
	std::map<ChannelModeName, std::string> Params;	/* Map of parameters by mode name for mlock */
	std::vector<ChanAccess *> access;				/* List of authorized users */
	std::vector<AutoKick *> akick;					/* List of users to kickban */
	std::vector<BadWord *> badwords;				/* List of badwords */
	Flags<ChannelModeName, CMODE_END> mlock_on;				/* Modes mlocked on */
	Flags<ChannelModeName, CMODE_END> mlock_off;				/* Modes mlocked off */

 public:
 	/** Default constructor
	 * @param chname The channel name
	 */
	ChannelInfo(const std::string &chname);

	/** Default destructor
	 */
	~ChannelInfo();

	std::string name; /* Channel name */
	NickCore *founder;
	NickCore *successor; /* Who gets the channel if the founder nick is dropped or expires */
	char *desc;
	char *url;
	char *email;

	time_t time_registered;
	time_t last_used;
	char *last_topic;				/* Last topic on the channel */
	std::string last_topic_setter;	/* Who set the last topic */
	time_t last_topic_time;			/* When the last topic was set */

	char *forbidby;
	char *forbidreason;

	int16 bantype;
	int16 *levels; /* Access levels for commands */

	char *entry_message; /* Notice sent on entering channel */

	MemoInfo memos;

	Channel *c; /* Pointer to channel record (if channel is currently in use) */

	/* For BotServ */

	BotInfo *bi;					/* Bot used on this channel */
	Flags<BotServFlag> botflags;
	int16 *ttb;						/* Times to ban for each kicker */

	int16 capsmin, capspercent;		/* For CAPS kicker */
	int16 floodlines, floodsecs;	/* For FLOOD kicker */
	int16 repeattimes;				/* For REPEAT kicker */

	/** Add an entry to the channel access list
	 *
	 * @param nc The NickCore of the user that the access entry should be tied to
	 * @param level The channel access level the user has on the channel
	 * @param creator The user who added the access
	 * @param last_seen When the user was last seen within the channel
	 *
	 * Creates a new access list entry and inserts it into the access list.
	 */
	void AddAccess(NickCore *nc, int16 level, const std::string &creator, int32 last_seen = 0);

	/** Get an entry from the channel access list by index
	 *
	 * @param index The index in the access list vector
	 * @return A ChanAccess struct corresponding to the index given, or NULL if outside the bounds
	 *
	 * Retrieves an entry from the access list that matches the given index.
	 */
	ChanAccess *GetAccess(unsigned index);

	/** Get an entry from the channel access list by NickCore
	 *
	 * @param nc The NickCore to find within the access list vector
	 * @param level Optional channel access level to compare the access entries to
	 * @return A ChanAccess struct corresponding to the NickCore, or NULL if not found
	 *
	 * Retrieves an entry from the access list that matches the given NickCore, optionally also matching a certain level.
	 */
	ChanAccess *GetAccess(NickCore *nc, int16 level = 0);

	/** Get the size of the accss vector for this channel
	 * @return The access vector size
	 */
	const unsigned GetAccessCount() const;

	/** Erase an entry from the channel access list
	 *
	 * @param index The index in the access list vector
	 *
	 * Clears the memory used by the given access entry and removes it from the vector.
	 */
	void EraseAccess(unsigned index);

	/** Clear the entire channel access list
	 *
	 * Clears the entire access list by deleting every item and then clearing the vector.
	 */
	void ClearAccess();

	/** Add an akick entry to the channel by NickCore
	 * @param user The user who added the akick
	 * @param akicknc The nickcore being akicked
	 * @param reason The reason for the akick
	 * @param t The time the akick was added, defaults to now
	 * @param lu The time the akick was last used, defaults to never
	 */
	AutoKick *AddAkick(const std::string &user, NickCore *akicknc, const std::string &reason, time_t t = time(NULL), time_t lu = 0);

	/** Add an akick entry to the channel by reason
	 * @param user The user who added the akick
	 * @param mask The mask of the akick
	 * @param reason The reason for the akick
	 * @param t The time the akick was added, defaults to now
	 * @param lu The time the akick was last used, defaults to never
	 */
	AutoKick *AddAkick(const std::string &user, const std::string &mask, const std::string &reason, time_t t = time(NULL), time_t lu = 0);

	/** Get an entry from the channel akick list
	 * @param index The index in the akick vector
	 * @return The akick structure, or NULL if not found
	 */
	AutoKick *GetAkick(unsigned index);

	/** Get the size of the akick vector for this channel
	 * @return The akick vector size
	 */
	const unsigned GetAkickCount() const;

	/** Erase an entry from the channel akick list
	 * @param index The index of the akick
	 */
	void EraseAkick(unsigned index);

	/** Clear the whole akick list
	 */
	void ClearAkick();

	/** Add a badword to the badword list
	 * @param word The badword
	 * @param type The type (SINGLE START END)
	 * @return The badword
	 */
	BadWord *AddBadWord(const std::string &word, BadWordType type);

	/** Get a badword structure by index
	 * @param index The index
	 * @return The badword
	 */
	BadWord *GetBadWord(unsigned index);

	/** Get how many badwords are on this channel
	 * @return The number of badwords in the vector
	 */
	const unsigned GetBadWordCount() const;

	/** Remove a badword
	 * @param index The index of the badword
	 */
	void EraseBadWord(unsigned index);

	/** Clear all badwords from the channel
	 */
	void ClearBadWords();

	/** Loads MLocked modes from extensible. This is used from database loading because Anope doesn't know what modes exist
	 * until after it connects to the IRCd.
	 */
	void LoadMLock();

	/** Check if a mode is mlocked
	 * @param Name The mode
	 * @param status True to check mlock on, false for mlock off
	 * @return true on success, false on fail
	 */
	const bool HasMLock(ChannelModeName Name, bool status);

	/** Set a mlock
	 * @param Name The mode
	 * @param status True for mlock on, false for mlock off
	 * @param param An optional param arg for + mlocked modes
	 * @return true on success, false on failure (module blocking)
	 */
	bool SetMLock(ChannelModeName Name, bool status, const std::string param = "");

	/** Remove a mlock
	 * @param Name The mode
	 * @return true on success, false on failure (module blcoking)
	 */
	bool RemoveMLock(ChannelModeName Name);

	/** Clear all mlocks on the channel
	 */
	void ClearMLock();

	/** Get the number of mlocked modes for this channel
	 * @param status true for mlock on, false for mlock off
	 * @return The number of mlocked modes
	 */
	const size_t GetMLockCount(bool status) const;

	/** Get a param from the channel
	 * @param Name The mode
	 * @param Target a string to put the param into
	 * @return true on success
	 */
	const bool GetParam(ChannelModeName Name, std::string &Target);

	/** Check if a mode is set and has a param
	 * @param Name The mode
	 */
	const bool HasParam(ChannelModeName Name);

	/** Clear all the params from the channel
	 */
	void ClearParams();

	/** Check whether a user is permitted to be on this channel
	 * @param u The user
	 * @return true if they are allowed, false if they aren't and were kicked
	 */
	bool CheckKick(User *user);
};

#endif // REGCHANNEL_H
