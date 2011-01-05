/* Modular support
 *
 * (C) 2008-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#ifndef REGCHANNEL_H
#define REGCHANNEL_H

typedef unordered_map_namespace::unordered_map<Anope::string, ChannelInfo *, ci::hash, std::equal_to<ci::string> > registered_channel_map;
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

struct ModeLock
{
	bool set;
	ChannelModeName name;
	Anope::string param;
	Anope::string setter;
	time_t created;

	ModeLock(bool s, ChannelModeName n, const Anope::string &p, const Anope::string &se = "", time_t c = Anope::CurTime) : set(s), name(n), param(p), setter(se), created(c) { }
};

class CoreExport ChannelInfo : public Extensible, public Flags<ChannelInfoFlag, CI_END>
{
 private:
	typedef std::multimap<ChannelModeName, ModeLock> ModeList;
 private:
	std::vector<ChanAccess *> access;					/* List of authorized users */
	std::vector<AutoKick *> akick;						/* List of users to kickban */
	std::vector<BadWord *> badwords;					/* List of badwords */
	ModeList mode_locks;

 public:
 	/** Default constructor
	 * @param chname The channel name
	 */
	ChannelInfo(const Anope::string &chname);

	/** Copy constructor
	 * @param ci The ChannelInfo to copy settings to
	 */
	ChannelInfo(ChannelInfo *ci);

	/** Default destructor
	 */
	~ChannelInfo();

	Anope::string name; /* Channel name */
	NickCore *founder;
	NickCore *successor; /* Who gets the channel if the founder nick is dropped or expires */
	Anope::string desc;

	time_t time_registered;
	time_t last_used;

	Anope::string last_topic;		/* The last topic that was set on this channel */
	Anope::string last_topic_setter;	/* Setter */
	time_t last_topic_time;			/* Time */

	Anope::string forbidby;
	Anope::string forbidreason;

	int16 bantype;
	int16 *levels; /* Access levels for commands */

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
	 * @param mask The mask of the access entry
	 * @param level The channel access level the user has on the channel
	 * @param creator The user who added the access
	 * @param last_seen When the user was last seen within the channel
	 * @return The new access class
	 *
	 * Creates a new access list entry and inserts it into the access list.
	 */
	ChanAccess *AddAccess(const Anope::string &mask, int16 level, const Anope::string &creator, int32 last_seen = 0);

	/** Get an entry from the channel access list by index
	 *
	 * @param index The index in the access list vector
	 * @return A ChanAccess struct corresponding to the index given, or NULL if outside the bounds
	 *
	 * Retrieves an entry from the access list that matches the given index.
	 */
	ChanAccess *GetAccess(unsigned index);

	/** Get an entry from the channel access list by User
	 *
	 * @param u The User to find within the access list vector
	 * @param level Optional channel access level to compare the access entries to
	 * @return A ChanAccess struct corresponding to the User, or NULL if not found
	 *
	 * Retrieves an entry from the access list that matches the given User, optionally also matching a certain level.
	 */
	ChanAccess *GetAccess(User *u, int16 level = 0);

	/** Get an entry from the channel access list by NickCore
	 *
	 * @param u The NickCore to find within the access list vector
	 * @param level Optional channel access level to compare the access entries to
	 * @return A ChanAccess struct corresponding to the NickCore, or NULL if not found
	 *
	 * Retrieves an entry from the access list that matches the given NickCore, optionally also matching a certain level.
	 */
	ChanAccess *GetAccess(NickCore *nc, int16 level = 0);
	
	/** Get an entry from the channel access list by mask
	 *
	 * @param u The mask to find within the access list vector
	 * @param level Optional channel access level to compare the access entries to
	 * @param wildcard True to match using wildcards
	 * @return A ChanAccess struct corresponding to the mask, or NULL if not found
	 *
	 * Retrieves an entry from the access list that matches the given mask, optionally also matching a certain level.
	 */
	ChanAccess *GetAccess(const Anope::string &mask, int16 level = 0, bool wildcard = true);

	/** Get the size of the accss vector for this channel
	 * @return The access vector size
	 */
	unsigned GetAccessCount() const;

	/** Erase an entry from the channel access list
	 *
	 * @param index The index in the access list vector
	 *
	 * Clears the memory used by the given access entry and removes it from the vector.
	 */
	void EraseAccess(unsigned index);

	/** Erase an entry from the channel access list
	 *
	 * @param access The access to remove
	 *
	 * Clears the memory used by the given access entry and removes it from the vector.
	 */
	void EraseAccess(ChanAccess *access);

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
	AutoKick *AddAkick(const Anope::string &user, NickCore *akicknc, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0);

	/** Add an akick entry to the channel by reason
	 * @param user The user who added the akick
	 * @param mask The mask of the akick
	 * @param reason The reason for the akick
	 * @param t The time the akick was added, defaults to now
	 * @param lu The time the akick was last used, defaults to never
	 */
	AutoKick *AddAkick(const Anope::string &user, const Anope::string &mask, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0);

	/** Get an entry from the channel akick list
	 * @param index The index in the akick vector
	 * @return The akick structure, or NULL if not found
	 */
	AutoKick *GetAkick(unsigned index);

	/** Get the size of the akick vector for this channel
	 * @return The akick vector size
	 */
	unsigned GetAkickCount() const;

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
	BadWord *AddBadWord(const Anope::string &word, BadWordType type);

	/** Get a badword structure by index
	 * @param index The index
	 * @return The badword
	 */
	BadWord *GetBadWord(unsigned index);

	/** Get how many badwords are on this channel
	 * @return The number of badwords in the vector
	 */
	unsigned GetBadWordCount() const;

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
	 * @param mode The mode
	 * @param An optional param
	 * @param status True to check mlock on, false for mlock off
	 * @return true on success, false on fail
	 */
	bool HasMLock(ChannelMode *mode, const Anope::string &param, bool status) const;

	/** Set a mlock
	 * @param mode The mode
	 * @param status True for mlock on, false for mlock off
	 * @param param An optional param arg for + mlocked modes
	 * @param setter Who is setting the mlock
	 * @param created When the mlock was created
	 * @return true on success, false on failure (module blocking)
	 */
	bool SetMLock(ChannelMode *mode, bool status, const Anope::string &param = "", const Anope::string &setter = "", time_t created = Anope::CurTime);

	/** Remove a mlock
	 * @param mode The mode
	 * @param param The param of the mode, required if it is a list or status mode
	 * @return true on success, false on failure
	 */
	bool RemoveMLock(ChannelMode *mode, const Anope::string &param = "");

	/** Clear all mlocks on the channel
	 */
	void ClearMLock();

	/** Get all of the mlocks for this channel
	 * @return The mlocks
	 */
	const std::multimap<ChannelModeName, ModeLock> &GetMLock() const;

	/** Get a list of modes on a channel
	 * @param Name The mode name to get a list of
	 * @return a pair of iterators for the beginning and end of the list
	 */
	std::pair<ModeList::iterator, ModeList::iterator> GetModeList(ChannelModeName Name);

	/** Get details for a specific mlock
	 * @param name The mode name
 	 * @param An optional param to match with
	 * @return The MLock, if any
	 */
	ModeLock *GetMLock(ChannelModeName name, const Anope::string &param = "");

	/** Check whether a user is permitted to be on this channel
	 * @param u The user
	 * @return true if they are allowed, false if they aren't and were kicked
	 */
	bool CheckKick(User *user);

	/** Check the channel topic
	 * If topic lock is enabled will change the topic back, else it records
	 * the new topic in the ChannelInfo
	 */
	void CheckTopic();
	
	/** Restore the channel topic, used on channel creation when not syncing with the uplink
	 * and after uplink sync
	 */
	void RestoreTopic();
};

/** A timer used to keep the BotServ bot/ChanServ in the channel
 * after kicking the last user in a channel
 */
class ChanServTimer : public Timer
{
 private:
	dynamic_reference<Channel> c;

 public:
 	/** Default constructor
	 * @param chan The channel
	 */
	ChanServTimer(Channel *chan);

	/** Called when the delay is up
	 * @param The current time
	 */
	void Tick(time_t);
};


#endif // REGCHANNEL_H
