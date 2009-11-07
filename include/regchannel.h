/* Modular support
 *
 * (C) 2008-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * $Id$
 *
 */

class CoreExport ChannelInfo : public Extensible
{
 private:
	std::map<ChannelModeName, std::string> Params;

 public:
	ChannelInfo();

	ChannelInfo *next, *prev;
	char name[CHANMAX];
	NickCore *founder;
	NickCore *successor;		/* Who gets the channel if the founder
					 			 * nick is dropped or expires */
	char *desc;
	char *url;
	char *email;

	time_t time_registered;
	time_t last_used;
	char *last_topic;			/* Last topic on the channel */
	char last_topic_setter[NICKMAX];	/* Who set the last topic */
	time_t last_topic_time;		/* When the last topic was set */

	uint32 flags;				/* See below */
	char *forbidby;
	char *forbidreason;

	int16 bantype;
	int16 *levels;				/* Access levels for commands */

	std::vector<ChanAccess *> access;			/* List of authorized users */
	std::vector<AutoKick *> akick;				/* List of users to kickban */

	std::bitset<128> mlock_on;
	std::bitset<128> mlock_off;

	char *entry_message;		/* Notice sent on entering channel */

	MemoInfo memos;

	Channel *c;			/* Pointer to channel record (if   *
					 			 *	channel is currently in use) */

	/* For BotServ */

	BotInfo *bi;					/* Bot used on this channel */
	uint32 botflags;				/* BS_* below */
	int16 *ttb;						/* Times to ban for each kicker */

	uint16 bwcount;
	BadWord *badwords;				/* For BADWORDS kicker */
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

	/** Erase an entry from the channel access list
	 *
	 * @param index The index in the access list vector
	 *
	 * Clears the memory used by the given access entry and removes it from the vector.
	 */
	void EraseAccess(unsigned index);

	/** Cleans the channel access list
	 *
	 * Cleans up the access list so it no longer contains entries no longer in use.
	 */
	void CleanAccess();

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
	 */
	AutoKick *AddAkick(const std::string &user, NickCore *akicknc, const std::string &reason, time_t t = time(NULL));

	/** Add an akick entry to the channel by reason
	 * @param user The user who added the akick
	 * @param mask The mask of the akick
	 * @param reason The reason for the akick
	 * @param t The time the akick was added, defaults to now
	 */
	AutoKick *AddAkick(const std::string &user, const std::string &mask, const std::string &reason, time_t t = time(NULL));

	/** Erase an entry from the channel akick list
	 * @param akick The akick
	 */
	void EraseAkick(AutoKick *akick);

	/** Clear the whole akick list
	 */
	void ClearAkick();

	/** Check if a mode is mlocked
	 * @param Name The mode
	 * @param status True to check mlock on, false for mlock off
	 * @return true on success, false on fail
	 */
	const bool HasMLock(ChannelModeName Name, bool status);

	/** Set a mlock
	 * @param Name The mode
	 * @param status True for mlock on, false for mlock off
	 */
	void SetMLock(ChannelModeName Name, bool status);

	/** Remove a mlock
	 * @param Name The mode
	 * @param status True for mlock on, false for mlock off
	 */
	void RemoveMLock(ChannelModeName Name, bool status);

	/** Clear all mlocks on the channel
	 */
	void ClearMLock();

	/** Set a channel mode param on the channel
	 * @param Name The mode
	 * @param param The param
	 * @param true on success
	 */
	bool SetParam(ChannelModeName Name, std::string Value);

	/** Unset a param from the channel
	 * @param Name The mode
	 */
	void UnsetParam(ChannelModeName Name);

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

	/** Clear all the params from the channel
	 */
	void ClearParams();
};
