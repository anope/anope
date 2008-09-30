/*
 * Copyright (C) 2008 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008 Anope Team <info@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 *
 * $Id$
 *
 */


/* Bot info structures. Note that since there won't be many bots,
 * they're not in a hash list.
 *	--lara
 */

class BotInfo
{
 public:
	BotInfo *next, *prev;

	char *nick;    			/* Nickname of the bot */
	char *user;    			/* Its user name */
	char *host;    			/* Its hostname */
	char *real;     		/* Its real name */
	int16 flags;			/* Bot flags -- see BI_* below */
	time_t created; 		/* Birth date ;) */
	int16 chancount;		/* Number of channels that use the bot. */
	/* Dynamic data */
	time_t lastmsg;			/* Last time we said something */
};

