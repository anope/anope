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

struct EntryMsg
{
	Anope::string chan;
	Anope::string creator;
	Anope::string message;
	time_t when;

	virtual ~EntryMsg() = default;
protected:
	EntryMsg() = default;
};

struct EntryMessageList
	: Serialize::Checker<std::vector<EntryMsg *> >
{
protected:
	EntryMessageList() : Serialize::Checker<std::vector<EntryMsg *> >("EntryMsg") { }

public:
	virtual ~EntryMessageList()
	{
		for (unsigned i = (*this)->size(); i > 0; --i)
			delete (*this)->at(i - 1);
	}

	virtual EntryMsg *Create() = 0;
};
