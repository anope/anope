/*
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

enum
{
	RPL_STATSLINKINFO = 211,
	RPL_ENDOFSTATS    = 219,
	RPL_STATSUPTIME   = 242,
	RPL_STATSOLINE    = 243,
	RPL_STATSCONN     = 250,
	RPL_WHOISREGNICK  = 307,
	RPL_WHOISUSER     = 311,
	RPL_WHOISSERVER   = 312,
	RPL_WHOISOPERATOR = 313,
	RPL_WHOISIDLE     = 317,
	RPL_ENDOFWHOIS    = 318,
	RPL_VERSION       = 351,
	RPL_MOTD          = 372,
	RPL_MOTDSTART     = 375,
	RPL_ENDOFMOTD     = 376,
	RPL_YOUREOPER     = 381,
	RPL_TIME          = 391,
	ERR_NOSUCHNICK    = 401,
	ERR_NOMOTD        = 422,
};
