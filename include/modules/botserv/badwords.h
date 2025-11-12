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

/** Flags for badwords
 */
enum BadWordType
{
	/* Always kicks if the word is said */
	BW_ANY,
	/* User must way the entire word */
	BW_SINGLE,
	/* The word has to start with the badword */
	BW_START,
	/* The word has to end with the badword */
	BW_END
};

/* Structure used to contain bad words. */
struct BadWord
{
	Anope::string chan;
	Anope::string word;
	BadWordType type;

	virtual ~BadWord() = default;
protected:
	BadWord() = default;
};

struct BadWords
{
	virtual ~BadWords() = default;

	/** Add a badword to the badword list
	 * @param word The badword
	 * @param type The type (SINGLE START END)
	 * @return The badword
	 */
	virtual BadWord *AddBadWord(const Anope::string &word, BadWordType type) = 0;

	/** Get a badword structure by index
	 * @param index The index
	 * @return The badword
	 */
	virtual BadWord *GetBadWord(unsigned index) const = 0;

	/** Get how many badwords are on this channel
	 * @return The number of badwords in the vector
	 */
	virtual unsigned GetBadWordCount() const = 0;

	/** Remove a badword
	 * @param index The index of the badword
	 */
	virtual void EraseBadWord(unsigned index) = 0;

	/** Clear all badwords from the channel
	 */
	virtual void ClearBadWords() = 0;

	virtual void Check() = 0;
};
