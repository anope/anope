// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

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
