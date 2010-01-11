/*
 * Copyright (C) 2008-2010 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 *
 * $Id$
 *
 */

#include "hashcomp.h"

class CoreExport OperType
{
 private:
	/** The name of this opertype, e.g. "sra".
	  */
	ci::string name;

	/** Privs that this opertype may use, e.g. 'users/auspex'.
	  * This *must* be std::list, see commands comment for details.
	  */
	std::list<std::string> privs;

	/** Commands this user may execute, e.g:
	  * botserv/set/ *, botserv/set/private, botserv/ *
	  * et cetera.
	  *
	  * This *must* be std::list, not std::map, because
	  * we support full globbing here. This shouldn't be a problem
	  * as we don't invoke it often.
	  */
	std::list<std::string> commands;
 public:
	/** Create a new opertype of the given name.
	 * @param nname The opertype name, e.g. "sra".
	 */
	OperType(const ci::string &nname);

	/** Check whether this opertype has access to run the given command string.
	  * @param cmdstr The string to check, e.g. botserv/set/private.
	  * @return True if this opertype may run the specified command, false otherwise.
	  */
	bool HasCommand(const std::string &cmdstr) const;

	/** Check whether this opertype has access to the given special permission.
	  * @param privstr The priv to check for, e.g. users/auspex.
	  * @return True if this opertype has the specified priv, false otherwise.
	  */
	bool HasPriv(const std::string &privstr) const;

	/** Add the specified command to this opertype.
	  * @param cmdstr The command mask to grant this opertype access to, e.g: nickserv/ *, chanserv/set/ *, botserv/set/private.
	  */
	void AddCommand(const std::string &cmdstr);

	/** Add the specified priv mask to this opertype.
	  * @param privstr The specified mask of privs to grant this opertype access to,  e.g. users/auspex, users/ *, etc.
	  */
	void AddPriv(const std::string &privstr);

	/** Returns the name of this opertype.
	 */
	const ci::string &GetName() const;
};
