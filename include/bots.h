/*
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2012 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#ifndef BOTS_H
#define BOTS_H

#include "users.h"
#include "anope.h"
#include "serialize.h"
#include "commands.h"


typedef Anope::insensitive_map<BotInfo *> botinfo_map;
typedef Anope::map<BotInfo *> botinfouid_map;

extern CoreExport serialize_checker<botinfo_map> BotListByNick;
extern CoreExport serialize_checker<botinfouid_map> BotListByUID;

/** Flags settable on a bot
 */
enum BotFlag
{
	BI_BEGIN,

	/* This bot is a core bot. NickServ, ChanServ, etc */
	BI_CORE,
	/* This bot can only be assigned by IRCops */
	BI_PRIVATE,
	/* This bot is defined in the config */
	BI_CONF,

	BI_END
};

static const Anope::string BotFlagString[] = { "BEGIN", "CORE", "PRIVATE", "CONF", "" };

class CoreExport BotInfo : public User, public Flags<BotFlag, BI_END>, public Serializable
{
 public:
	time_t created;			/* Birth date ;) */
	time_t lastmsg;			/* Last time we said something */
	typedef Anope::insensitive_map<CommandInfo> command_map;
	command_map commands; /* Commands, actual name to service name */
	Anope::string botmodes;		/* Modes the bot should have as configured in service:modes */
	std::vector<Anope::string> botchannels;	/* Channels the bot should be in as configured in service:channels */
	bool introduced;		/* Whether or not this bot is introduced */

	/** Create a new bot.
	 * @param nick The nickname to assign to the bot.
	 * @param user The ident to give the bot.
	 * @param host The hostname to give the bot.
	 * @param real The realname to give the bot.
	 * @param bmodes The modes to give the bot.
	 */
	BotInfo(const Anope::string &nick, const Anope::string &user = "", const Anope::string &host = "", const Anope::string &real = "", const Anope::string &bmodes = "");

	/** Destroy a bot, clearing up appropriately.
	 */
	virtual ~BotInfo();

	const Anope::string serialize_name() const;
	Serialize::Data serialize() const;
	static Serializable* unserialize(Serializable *obj, Serialize::Data &);

	void GenerateUID();

	/** Change the nickname for the bot.
	 * @param newnick The nick to change to
	 */
	void SetNewNick(const Anope::string &newnick);

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

	/** Get the number of channels this bot is assigned to
	 */
	unsigned GetChannelCount() const;

	/** Join this bot to a channel
	 * @param c The channel
	 * @param status The status the bot should have on the channel
	 */
	void Join(Channel *c, ChannelStatus *status = NULL);

	/** Join this bot to a channel
	 * @param chname The channel name
	 * @param status The status the bot should have on the channel
	 */
	void Join(const Anope::string &chname, ChannelStatus *status = NULL);

	/** Part this bot from a channel
	 * @param c The channel
	 * @param reason The reason we're parting
	 */
	void Part(Channel *c, const Anope::string &reason = "");

	/** Called when a user messages this bot
	 * @param u The user
	 * @param message The users' message
	 */
	virtual void OnMessage(User *u, const Anope::string &message);

	/** Link a command name to a command in services
	 * @param cname The command name
	 * @param sname The service name
	 * @param permission Permission required to execute the command, if any
	 */
	void SetCommand(const Anope::string &cname, const Anope::string &sname, const Anope::string &permission = "");

	/** Get command info for a command
	 * @param cname The command name
	 * @return A struct containing service name and permission
	 */
	CommandInfo *GetCommand(const Anope::string &cname);
};

extern CoreExport BotInfo *findbot(const Anope::string &nick);

extern CoreExport void bot_raw_ban(User *requester, ChannelInfo *ci, const Anope::string &nick, const Anope::string &reason);
extern CoreExport void bot_raw_kick(User *requester, ChannelInfo *ci, const Anope::string &nick, const Anope::string &reason);

#endif // BOTS_H
