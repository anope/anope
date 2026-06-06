// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
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

#define NICKSERV_AJOIN_LIST_EXT "ajoinlist"

struct AJoinEntry;

struct AJoinList
	: Serialize::Checker<std::vector<AJoinEntry *> >
{
	AJoinList(Extensible *) : Serialize::Checker<std::vector<AJoinEntry *> >("AJoinEntry") { }
	virtual ~AJoinList() = default;
};

struct AJoinEntry final
	: Serializable
{
	Serialize::Reference<NickCore> owner;
	Anope::string channel;
	Anope::string key;

	AJoinEntry(Extensible *) : Serializable("AJoinEntry") { }

	~AJoinEntry() override
	{
		auto *channels = owner->GetExt<AJoinList>(NICKSERV_AJOIN_LIST_EXT);
		if (channels)
		{
			auto it = std::find((*channels)->begin(), (*channels)->end(), this);
			if (it != (*channels)->end())
				(*channels)->erase(it);
		}
	}
};
