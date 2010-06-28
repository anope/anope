/*
 * Copyright (C) 2008-2010 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#ifndef BOTS_H
#define BOTS_H

#include "commands.h"

class BotInfo;

typedef unordered_map_namespace::unordered_map<ci::string, BotInfo *, hash_compare_ci_string> botinfo_map;
typedef unordered_map_namespace::unordered_map<std::string, BotInfo *, hash_compare_std_string> botinfo_uid_map;
extern CoreExport botinfo_map BotListByNick;
extern CoreExport botinfo_uid_map BotListByUID;

/** Flags settable on a bot
 */
enum BotFlag
{
	BI_BEGIN,

	/* This bot can only be assigned by IRCops */
	BI_PRIVATE,

	BI_END
};

class CoreExport BotInfo : public User, public Flags<BotFlag, BI_END>
{
 public:
	time_t created; 	/* Birth date ;) */
	time_t lastmsg;		/* Last time we said something */
	CommandMap Commands;	/* Commands on this bot */

	/** Create a new bot.
	 * @param nick The nickname to assign to the bot.
	 * @param user The ident to give the bot.
	 * @param host The hostname to give the bot.
	 * @param real The realname to give the bot.
	 */
	BotInfo(const std::string &nick, const std::string &user = "", const std::string &host = "", const std::string &real = "");

	/** Destroy a bot, clearing up appropriately.
	 */
	virtual ~BotInfo();

	/** Change the nickname for the bot.
	 * @param newnick The nick to change to
	 */
	void SetNewNick(const std::string &newnick);

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

	void Join(Channel *c);
	void Join(const std::string &chname);
	void Part(Channel *c, const std::string &reason = "");
};

#endif // BOTS_H
