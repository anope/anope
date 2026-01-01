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

#define CHANSERV_ENTRY_MESSAGE_EXT "entrymsg"
#define CHANSERV_ENTRY_MESSAGE_TYPE "EntryMsg"

namespace ChanServ
{
	class EntryMessage;
	class EntryMessageList;
}

class ChanServ::EntryMessage
{
protected:
	EntryMessage() = default;

public:
	Anope::string chan;
	Anope::string creator;
	Anope::string message;
	time_t when = 0;

	virtual ~EntryMessage() = default;
};

class ChanServ::EntryMessageList
	: public Serialize::Checker<std::vector<ChanServ::EntryMessage *>>
{
protected:
	EntryMessageList()
		: Serialize::Checker<std::vector<ChanServ::EntryMessage *>>(CHANSERV_ENTRY_MESSAGE_TYPE)
	{
	}

public:
	virtual ~EntryMessageList()
	{
		for (auto i = (*this)->size(); i > 0; --i)
			delete (*this)->at(i - 1);
	}

	virtual ChanServ::EntryMessage *Create() = 0;
};
