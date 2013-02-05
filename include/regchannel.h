/*
 *
 * (C) 2008-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#ifndef REGCHANNEL_H
#define REGCHANNEL_H

#include "memo.h"
#include "modes.h"
#include "extensible.h"
#include "logger.h"
#include "modules.h"
#include "serialize.h"
#include "bots.h"

typedef Anope::hash_map<ChannelInfo *> registered_channel_map;

extern CoreExport Serialize::Checker<registered_channel_map> RegisteredChannelList;

/* Indices for TTB (Times To Ban) */
enum
{
	TTB_BOLDS,
	TTB_COLORS,
	TTB_REVERSES,
	TTB_UNDERLINES,
	TTB_BADWORDS,
	TTB_CAPS,
	TTB_FLOOD,
	TTB_REPEAT,
	TTB_ITALICS,
	TTB_AMSGS,
	TTB_SIZE
};

/** Flags for badwords
 */
enum BadWordType
{
	/* Always kicks if the word is said */
	BW_ANY,
	/* User must way the entire word */
	BW_SINGLE,
	/* The word has to start with the badword */
	BW_START,
	/* The word has to end with the badword */
	BW_END
};

/* Structure used to contain bad words. */
struct CoreExport BadWord : Serializable
{
	Serialize::Reference<ChannelInfo> ci;
	Anope::string word;
	BadWordType type;

	BadWord();
	~BadWord();
	void Serialize(Serialize::Data &data) const anope_override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &);
};

/* AutoKick data. */
class CoreExport AutoKick : public Serializable
{
 public:
	/* Channel this autokick is on */
	Serialize::Reference<ChannelInfo> ci;

	Anope::string mask;
	Serialize::Reference<NickCore> nc;

	Anope::string reason;
	Anope::string creator;
	time_t addtime;
	time_t last_used;

 	AutoKick();
	~AutoKick();
	void Serialize(Serialize::Data &data) const anope_override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &);
};

struct CoreExport ModeLock : Serializable
{
 public:
	Serialize::Reference<ChannelInfo> ci;
	bool set;
	Anope::string name;
	Anope::string param;
	Anope::string setter;
	time_t created;

	ModeLock(ChannelInfo *ch, bool s, const Anope::string &n, const Anope::string &p, const Anope::string &se = "", time_t c = Anope::CurTime);
	~ModeLock();

	void Serialize(Serialize::Data &data) const anope_override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &);
};

struct CoreExport LogSetting : Serializable
{
	Serialize::Reference<ChannelInfo> ci;
	/* Our service name of the command */
	Anope::string service_name;
	/* The name of the client the command is on */
	Anope::string command_service;
	/* Name of the command to the user, can have spaces */
	Anope::string command_name;
	Anope::string method, extra;
	Anope::string creator;
	time_t created;

	LogSetting() : Serializable("LogSetting") { }
	void Serialize(Serialize::Data &data) const anope_override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &);
};

/* It matters that Base is here before Extensible (it is inherited by Serializable)
 *
 * Possible flags:
 *     TOPICLOCK      - Topic can only be changed by TOPIC SET
 *     RESTRICTED     - Only users on the access list may join
 *     PEACE          - Don't allow ChanServ and BotServ commands to do bad things to users with higher access levels
 *     SECURE         - Don't allow any privileges
 *     NO_EXPIRE      - Channel does not expire
 *     MEMO_HARDMAX   - Channel memo limit may not be changed
 *     SECUREFOUNDER  - Stricter control of channel founder status
 *     SIGNKICK       - Sign kicks with the user who did the kick
 *     SIGNKICK_LEVEL - Sign kicks if level is < than the one defined by the SIGNKICK level
 *     SUSPENDED      - Channel is suepended
 *     PERSIST        - Channel still exists when empited (perm channel mode or leaving a botserv bot).
 *     STATS          - Chanstats are enabled
 *     NOAUTOOP       - If set users are not auto given any status on join
 *
 * BotServ flags:
 *     BS_DONTKICKOPS     - BotServ won't kick ops
 *     BS_DONTKICKVOICES  - BotServ won't kick voices
 *     BS_FANTASY         - BotServ bot accepts fantasy commands
 *     BS_GREET           - BotServ should show greets
 *     BS_NOBOT           - BotServ bots are not allowed to be in this channel
 *     BS_KICK_BOLDS      - BotServ kicks for bolds
 *     BS_KICK_COLORS     - BotServ kicks for colors
 *     BS_KICK_REVERSES   - BotServ kicks for reverses
 *     BS_KICK_UNDERLINES - BotServ kicks for underlines
 *     BS_KICK_BADWORD    - BotServ kicks for badwords
 *     BS_KICK_CAPS       - BotServ kicks for caps
 *     BS_KICK_FLOOD      - BotServ kicks for flood
 *     BS_KICK_REPEAT     - BotServ kicks for repeating
 *     BS_KICK_ITALICS    - BotServ kicks for italics
 *     BS_KICK_AMSGS      - BotServ kicks for amsgs
 */
class CoreExport ChannelInfo : public Serializable, public Extensible
{
 private:
	Serialize::Reference<NickCore> founder;					/* Channel founder */
	Serialize::Checker<std::vector<ChanAccess *> > access;			/* List of authorized users */
	Serialize::Checker<std::vector<AutoKick *> > akick;			/* List of users to kickban */
	Serialize::Checker<std::vector<BadWord *> > badwords;			/* List of badwords */
	std::map<Anope::string, int16_t> levels;

 public:
 	friend class ChanAccess;
	friend class AutoKick;
	friend class BadWord;

	typedef std::multimap<Anope::string, ModeLock *> ModeList;
	Serialize::Checker<ModeList> mode_locks;
	Serialize::Checker<std::vector<LogSetting *> > log_settings;

	Anope::string name;                       /* Channel name */
	Serialize::Reference<NickCore> successor; /* Who gets the channel if the founder nick is dropped or expires */
	Anope::string desc;

	time_t time_registered;
	time_t last_used;

	Anope::string last_topic;                 /* The last topic that was set on this channel */
	Anope::string last_topic_setter;          /* Setter */
	time_t last_topic_time;	                  /* Time */

	int16_t bantype;

	MemoInfo memos;

	Channel *c;                               /* Pointer to channel, if the channel exists */

	/* For BotServ */
	Serialize::Reference<BotInfo> bi;         /* Bot used on this channel */
	int16_t ttb[TTB_SIZE];                    /* Times to ban for each kicker */

	int16_t capsmin, capspercent;	          /* For CAPS kicker */
	int16_t floodlines, floodsecs;            /* For FLOOD kicker */
	int16_t repeattimes;                      /* For REPEAT kicker */

 	/** Constructor
	 * @param chname The channel name
	 */
	ChannelInfo(const Anope::string &chname);

	/** Copy constructor
	 * @param ci The ChannelInfo to copy settings to
	 */
	ChannelInfo(const ChannelInfo &ci);

	~ChannelInfo();

	void Serialize(Serialize::Data &data) const anope_override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &);

	/** Change the founder of the channek
	 * @params nc The new founder
	 */
	void SetFounder(NickCore *nc);

	/** Get the founder of the channel
	 * @return The founder
	 */
	NickCore *GetFounder() const;

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
	AccessGroup AccessFor(const User *u);
	AccessGroup AccessFor(const NickCore *nc);

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
	AutoKick* AddAkick(const Anope::string &user, NickCore *akicknc, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0);

	/** Add an akick entry to the channel by reason
	 * @param user The user who added the akick
	 * @param mask The mask of the akick
	 * @param reason The reason for the akick
	 * @param t The time the akick was added, defaults to now
	 * @param lu The time the akick was last used, defaults to never
	 */
	AutoKick* AddAkick(const Anope::string &user, const Anope::string &mask, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0);

	/** Get an entry from the channel akick list
	 * @param index The index in the akick vector
	 * @return The akick structure, or NULL if not found
	 */
	AutoKick* GetAkick(unsigned index) const;

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
	BadWord* AddBadWord(const Anope::string &word, BadWordType type);

	/** Get a badword structure by index
	 * @param index The index
	 * @return The badword
	 */
	BadWord* GetBadWord(unsigned index) const;

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
	bool SetMLock(ChannelMode *mode, bool status, const Anope::string &param = "", Anope::string setter = "", time_t created = Anope::CurTime);

	/** Remove a mlock
	 * @param mode The mode
	 * @param status True for mlock on, false for mlock off
	 * @param param The param of the mode, required if it is a list or status mode
	 * @return true on success, false on failure
	 */
	bool RemoveMLock(ChannelMode *mode, bool status, const Anope::string &param = "");

	void RemoveMLock(ModeLock *mlock);

	/** Clear all mlocks on the channel
	 */
	void ClearMLock();

	/** Get all of the mlocks for this channel
	 * @return The mlocks
	 */
	const ModeList &GetMLock() const;

	/** Get a list of modes on a channel
	 * @param Name The mode name to get a list of
	 * @return a pair of iterators for the beginning and end of the list
	 */
	std::pair<ModeList::iterator, ModeList::iterator> GetModeList(const Anope::string &name);

	/** Get details for a specific mlock
	 * @param mname The mode name
 	 * @param An optional param to match with
	 * @return The MLock, if any
	 */
	const ModeLock *GetMLock(const Anope::string &mname, const Anope::string &param = "");

	/** Get the current mode locks as a string
	 * @param complete True to show mlock parameters aswell
	 * @return A string of mode locks, eg: +nrt
	 */
	Anope::string GetMLockAsString(bool complete) const;

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
	static ChannelInfo* Find(const Anope::string &name);
};

/** Is the user the real founder?
 * @param user The user
 * @param ci The channel
 * @return true or false
 */
extern CoreExport bool IsFounder(const User *user, const ChannelInfo *ci);

#endif // REGCHANNEL_H
