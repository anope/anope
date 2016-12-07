/*
 * Anope IRC Services
 *
 * Copyright (C) 2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
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
	RPL_WHOISIDLE     = 317,
	RPL_ENDOFWHOIS    = 318,
	RPL_VERSION       = 351,
	RPL_MOTD          = 372,
	RPL_MOTDSTART     = 375,
	RPL_ENDOFMOTD     = 376,
	RPL_YOUREOPER     = 381,
	RPL_TIME          = 391,
	ERR_NOSUCHNICK    = 401,
	ERR_NOMOTD        = 422
};
