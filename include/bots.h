/*
 * Copyright (C) 2008-2009 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2009 Anope Team <info@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 *
 * $Id$
 *
 */


struct CommandHash;

class CoreExport BotInfo
{
 public:
	BotInfo *next, *prev;

	std::string uid;		/* required for UID supporting servers, as opposed to the shitty struct Uid. */
	char *nick;				/* Nickname of the bot */
	char *user;				/* Its user name */
	char *host;				/* Its hostname */
	char *real;	 		/* Its real name */
	int16 flags;			/* Bot flags -- see BI_* below */
	time_t created; 		/* Birth date ;) */
	int16 chancount;		/* Number of channels that use the bot. */
	/* Dynamic data */
	time_t lastmsg;			/* Last time we said something */
	CommandHash **cmdTable;

	/** Create a new bot.
	 * XXX: Note - this constructor is considered obsolete. Use the four parameter form.
	 * @param nick The nickname to assign to the bot.
	 */
	BotInfo(const char *nick);
	/** Create a new bot.
	 * @param nick The nickname to assign to the bot.
	 * @param user The ident to give the bot.
	 * @param host The hostname to give the bot.
	 * @param real The realname to give the bot.
	 */
	BotInfo(const char *nick, const char *user, const char *host, const char *real);

	/** Destroy a bot, clearing up appropriately.
	 */
	virtual ~BotInfo();

	/** Change the nickname set on a bot.
	 * @param newnick The nick to change to
	 */
	void ChangeNick(const char *newnick);

	/** Rejoins all channels that this bot is assigned to.
	 * Used on /kill, rename, etc.
	 */
	void RejoinAll();

	/** Assign this bot to a given channel, removing the existing assigned bot if one exists.
	 * @param u The user assigning the bot, or NULL
	 * @param ci The channel registration to assign the bot to.
	 */
	void Assign(User *u, ChannelInfo *ci);

	/** Remove this bot from a given channel.
	 * @param u The user requesting the unassign, or NULL.
	 * @param ci The channel registration to remove the bot from.
	 */
	void UnAssign(User *u, ChannelInfo *ci);
};

