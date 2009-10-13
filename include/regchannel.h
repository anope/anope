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
	ChannelInfo()
	{
		next = prev = NULL;
		founderpass[0] = name[0] = last_topic_setter[0] = '\0';
		founder = successor = NULL;
		desc = url = email = last_topic = forbidby = forbidreason = NULL;
		time_registered = last_used = last_topic_time = 0;
		flags = 0;
		bantype = akickcount = 0;
		levels = NULL;
		akick = NULL;
		entry_message = NULL;
		c = NULL;
		bi = NULL;
		botflags = 0;
		ttb = NULL;
		bwcount = 0;
		badwords = NULL;
		capsmin = capspercent = 0;
		floodlines = floodsecs = 0;
		repeattimes = 0;
	}

	ChannelInfo *next, *prev;
	char name[CHANMAX];
	NickCore *founder;
	NickCore *successor;		/* Who gets the channel if the founder
					 			 * nick is dropped or expires */
	char founderpass[PASSMAX];
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
	uint16 akickcount;
	AutoKick *akick;			/* List of users to kickban */

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
	 * @param last_seen When the user was last seen within the channel
	 *
	 * Creates a new access list entry and inserts it into the access list.
	 */
	void AddAccess(NickCore *nc, int16 level, int32 last_seen = 0)
	{
		ChanAccess *new_access = new ChanAccess;
		new_access->in_use = 1;
		new_access->nc = nc;
		new_access->level = level;
		new_access->last_seen = last_seen;
		access.push_back(new_access);
	}

	/** Get an entry from the channel access list by index
	 *
	 * @param index The index in the access list vector
	 * @return A ChanAccess struct corresponding to the index given, or NULL if outside the bounds
	 *
	 * Retrieves an entry from the access list that matches the given index.
	 */
	ChanAccess *GetAccess(unsigned index)
	{
		if (access.empty() || index >= access.size())
			return NULL;

		return access[index];
	}

	/** Get an entry from the channel access list by NickCore
	 *
	 * @param nc The NickCore to find within the access list vector
	 * @param level Optional channel access level to compare the access entries to
	 * @return A ChanAccess struct corresponding to the NickCore, or NULL if not found
	 *
	 * Retrieves an entry from the access list that matches the given NickCore, optionally also matching a certain level.
	 */
	ChanAccess *GetAccess(NickCore *nc, int16 level = 0)
	{
		if (access.empty())
			return NULL;

		for (unsigned i = 0; i < access.size(); i++)
			if (access[i]->in_use && access[i]->nc == nc && (level ? access[i]->level == level : true))
				return access[i];

		return NULL;
	}

	/** Erase an entry from the channel access list
	 *
	 * @param index The index in the access list vector
	 *
	 * Clears the memory used by the given access entry and removes it from the vector.
	 */
	void EraseAccess(unsigned index)
	{
		if (access.empty() || index >= access.size())
			return;
		delete access[index];
		access.erase(access.begin() + index);
	}

	/** Cleans the channel access list
	 *
	 * Cleans up the access list so it no longer contains entries no longer in use.
	 */
	void CleanAccess()
	{
		for (unsigned j = access.size(); j > 0; --j)
			if (!access[j - 1]->in_use)
				EraseAccess(j - 1);
	}

	/** Clear the entire channel access list
	 *
	 * Clears the entire access list by deleting every item and then clearing the vector.
	 */
	void ClearAccess()
	{
		while (access.begin() != access.end())
			EraseAccess(0);
	}

	/** Check if a mode is mlocked
	 * @param Name The mode
	 * @param status True to check mlock on, false for mlock off
	 * @return true on success, false on fail
	 */
	const bool HasMLock(ChannelModeName Name, bool status)
	{
		if (status)
			return mlock_on[(size_t)Name];
		else
			return mlock_off[(size_t)Name];
	}

	/** Set a mlock
	 * @param Name The mode
	 * @param status True for mlock on, false for mlock off
	 */
	void SetMLock(ChannelModeName Name, bool status)
	{
		size_t value = (size_t)Name;

		if (status)
			mlock_on[value] = true;
		else
			mlock_off[value] = true;
	}

	/** Remove a mlock
	 * @param Name The mode
	 * @param status True for mlock on, false for mlock off
	 */
	void RemoveMLock(ChannelModeName Name, bool status)
	{
		size_t value = (size_t)Name;

		if (status)
			mlock_on[value] = false;
		else
			mlock_off[value] = false;
	}

	/** Clear all mlocks on the channel
	 */
	void ClearMLock()
	{
		mlock_on.reset();
		mlock_off.reset();
	}

	/** Set a channel mode param on the channel
	 * @param Name The mode
	 * @param param The param
	 * @param true on success
	 */
	bool SetParam(ChannelModeName Name, std::string Value)
	{
		return Params.insert(std::make_pair(Name, Value)).second;
	}

	/** Unset a param from the channel
	 * @param Name The mode
	 */
	void UnsetParam(ChannelModeName Name)
	{
		std::map<ChannelModeName, std::string>::iterator it = Params.find(Name);

		if (it != Params.end())
		{
			Params.erase(it);
		}
	}

	/** Get a param from the channel
	 * @param Name The mode
	 * @param Target a string to put the param into
	 * @return true on success
	 */
	const bool GetParam(ChannelModeName Name, std::string *Target)
	{
		std::map<ChannelModeName, std::string>::iterator it = Params.find(Name);

		(*Target).clear();

		if (it != Params.end())
		{
			*Target = it->second;
			return true;
		}

		return false;
	}

	/** Check if a mode is set and has a param
	 * @param Name The mode
	 */
	const bool HasParam(ChannelModeName Name)
	{
		std::map<ChannelModeName, std::string>::iterator it = Params.find(Name);

		if (it != Params.end())
		{
			return true;
		}

		return false;
	}

	/** Clear all the params from the channel
	 */
	void ClearParams()
	{
		Params.clear();
	}
};
