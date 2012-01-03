/* HostServ functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"


/** Set a vhost for the user
 * @param ident The ident
 * @param host The host
 * @param creator Who created the vhost
 * @param time When the vhost was craated
 */
void HostInfo::SetVhost(const Anope::string &ident, const Anope::string &host, const Anope::string &creator, time_t created)
{
	Ident = ident;
	Host = host;
	Creator = creator;
	Time = created;
}

/** Remove a users vhost
 **/
void HostInfo::RemoveVhost()
{
	Ident.clear();
	Host.clear();
	Creator.clear();
	Time = 0;
}

/** Check if the user has a vhost
 * @return true or false
 */
bool HostInfo::HasVhost() const
{
	return !Host.empty();
}

/** Retrieve the vhost ident
 * @return the ident
 */
const Anope::string &HostInfo::GetIdent() const
{
	return Ident;
}

/** Retrieve the vhost host
 * @return the host
 */
const Anope::string &HostInfo::GetHost() const
{
	return Host;
}

/** Retrieve the vhost creator
 * @return the creator
 */
const Anope::string &HostInfo::GetCreator() const
{
	return Creator;
}

/** Retrieve when the vhost was crated
 * @return the time it was created
 */
time_t HostInfo::GetTime() const
{
	return Time;
}

