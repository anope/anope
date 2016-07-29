#include "modules/chanserv.h"

#pragma once

namespace ChanServ
{

/* A group of access entries. This is used commonly, for example with ChanServ::Channel::AccessFor,
 * to show what access a user has on a channel because users can match multiple access entries.
 */
class CoreExport AccessGroup : public std::vector<ChanAccess *>
{
 public:
	/* Channel these access entries are on */
	ChanServ::Channel *ci = nullptr;
	/* Account these entries affect, if any */
	const NickServ::Account *nc = nullptr;
	/* super_admin always gets all privs. founder is a special case where ci->founder == nc */
	bool super_admin = false, founder = false;

	/** Check if this access group has a certain privilege. Eg, it
	 * will check every ChanAccess entry of this group for any that
	 * has the given privilege.
	 * @param priv The privilege
	 * @return true if any entry has the given privilege
	 */
	bool HasPriv(const Anope::string &priv);

	/** Get the "highest" access entry from this group of entries.
	 * The highest entry is determined by the entry that has the privilege
	 * with the highest rank (see Privilege::rank).
	 * @return The "highest" entry
	 */
	ChanAccess *Highest();

	/* Comparison operators to other AccessGroups */
	bool operator>(AccessGroup &other);
	bool operator<(AccessGroup &other);
	bool operator>=(AccessGroup &other);
	bool operator<=(AccessGroup &other);
};

} // namespace ChanServ