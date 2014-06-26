/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
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

