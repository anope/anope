/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#ifndef BOTSERV_H
#define BOTSERV_H

#include "anope.h"

/* BotServ SET flags */
enum BotServFlag
{
	BS_BEGIN,
	/* BotServ won't kick ops */
	BS_DONTKICKOPS,
	/* BotServ won't kick voices */
	BS_DONTKICKVOICES,
	/* BotServ bot accepts fantasy commands */
	BS_FANTASY,
	/* BotServ should show greets */
	BS_GREET,
	/* BotServ bots are not allowed to be in this channel */
	BS_NOBOT,
	/* BotServ kicks for bolds */
	BS_KICK_BOLDS,
	/* BotServ kicks for colors */
	BS_KICK_COLORS,
	/* BOtServ kicks for reverses */
	BS_KICK_REVERSES,
	/* BotServ kicks for underlines */
	BS_KICK_UNDERLINES,
	/* BotServ kicks for badwords */
	BS_KICK_BADWORDS,
	/* BotServ kicks for caps */
	BS_KICK_CAPS,
	/* BotServ kicks for flood */
	BS_KICK_FLOOD,
	/* BotServ kicks for repeating */
	BS_KICK_REPEAT,
	/* BotServ kicks for italics */
	BS_KICK_ITALICS,
	/* BotServ kicks for amsgs */
	BS_KICK_AMSGS,
	BS_END
};

const Anope::string BotServFlagStrings[] = {
	"BEGIN", "DONTKICKOPS", "DONTKICKVOICES", "FANTASY", "GREET", "NOBOT",
	"KICK_BOLDs", "KICK_COLORS", "KICK_REVERSES", "KICK_UNDERLINES", "KICK_BADWORDS", "KICK_CAPS",
	"KICK_FLOOD", "KICK_REPEAT", "KICK_ITALICS", "KICK_AMSGS", "MSG_PRIVMSG", "MSG_NOTICE",
	"MSG_NOTICEOPS", ""
};

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

#endif // BOTSERV_H
