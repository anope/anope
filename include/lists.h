/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2017 Anope Team <team@anope.org>
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

#include "services.h"
#include "anope.h"

/** A class to process numbered lists (passed to most DEL/LIST/VIEW commands).
 * The function HandleNumber is called for every number in the list. Note that
 * if descending is true it gets called in descending order. This is so deleting
 * the index passed to the function from an array will not cause the other indexes
 * passed to the function to be incorrect. This keeps us from having to have an
 * 'in use' flag on everything.
 */
class CoreExport NumberList
{
	std::function<void(void)> endf;

 public:
	/** Processes a numbered list
	 * @param list The list
	 * @param descending True to call the number handler callback with the numbers in descending order
	 */
	NumberList(const Anope::string &list, bool descending, std::function<void(unsigned int)> nf, std::function<void(void)> ef);

	~NumberList();
};

/** This class handles formatting LIST/VIEW replies.
 */
class CoreExport ListFormatter
{
 public:
 	typedef std::map<Anope::string, Anope::string> ListEntry;
 private:
	NickServ::Account *nc;
	std::vector<Anope::string> columns;
	std::vector<ListEntry> entries;
 public:
	ListFormatter(NickServ::Account *nc);
	ListFormatter &AddColumn(const Anope::string &name);
	void AddEntry(const ListEntry &entry);
	bool IsEmpty() const;
	void Process(std::vector<Anope::string> &);
};

/** This class handles formatting INFO replies
 */
class CoreExport InfoFormatter
{
	NickServ::Account *nc;
	std::vector<std::pair<Anope::string, Anope::string> > replies;
	unsigned longest;
 public:
	InfoFormatter(NickServ::Account *nc);
	void Process(std::vector<Anope::string> &);
	Anope::string &operator[](const Anope::string &key);
	void AddOption(const Anope::string &opt);
};

