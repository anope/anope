/*
 * Copyright (C) 2002-2009 InspIRCd Development Team
 * Copyright (C) 2009 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * These classes have been copied from InspIRCd and modified
 * for use in Anope.
 *
 * $Id$
 *
 */

#ifndef _HASHCOMP_H_
#define _HASHCOMP_H_

#include <string>

/** sepstream allows for splitting token seperated lists.
 * Each successive call to sepstream::GetToken() returns
 * the next token, until none remain, at which point the method returns
 * an empty string.
 */
class sepstream
{
 private:
	/** Original string.
	 */
	std::string tokens;
	/** Last position of a seperator token
	 */
	std::string::iterator last_starting_position;
	/** Current string position
	 */
	std::string::iterator n;
	/** Seperator value
	 */
	char sep;
 public:
	/** Create a sepstream and fill it with the provided data
	 */
	sepstream(const std::string &source, char seperator);
	virtual ~sepstream() { }

	/** Fetch the next token from the stream
	 * @param token The next token from the stream is placed here
	 * @return True if tokens still remain, false if there are none left
	 */
	virtual bool GetToken(std::string &token);

	/** Fetch the entire remaining stream, without tokenizing
	 * @return The remaining part of the stream
	 */
	virtual const std::string GetRemaining();

	/** Returns true if the end of the stream has been reached
	 * @return True if the end of the stream has been reached, otherwise false
	 */
	virtual bool StreamEnd();
};

/** A derived form of sepstream, which seperates on commas
 */
class commasepstream : public sepstream
{
 public:
	/** Initialize with comma seperator
	 */
	commasepstream(const std::string &source) : sepstream(source, ',') { }
};

/** A derived form of sepstream, which seperates on spaces
 */
class spacesepstream : public sepstream
{
 public:
	/** Initialize with space seperator
	 */
	spacesepstream(const std::string &source) : sepstream(source, ' ') { }
};

#endif
