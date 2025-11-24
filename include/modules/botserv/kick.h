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

#define BOTSERV_KICKER_DATA_EXT "kickerdata"

namespace BotServ
{
	class KickerData;

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
}


class BotServ::KickerData
{
protected:
	KickerData() = default;

public:
	bool amsgs, badwords, bolds, caps, colors, flood, italics, repeat, reverses, underlines;
	int16_t ttb[BotServ::TTB_SIZE];                    /* Times to ban for each kicker */
	int16_t capsmin, capspercent;	          /* For CAPS kicker */
	int16_t floodlines, floodsecs;            /* For FLOOD kicker */
	int16_t repeattimes;                      /* For REPEAT kicker */

	bool dontkickops, dontkickvoices;

	virtual ~KickerData() = default;
	virtual void Check(ChannelInfo *ci) = 0;
};
