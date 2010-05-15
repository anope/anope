/* HostServ functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#include "services.h"
#include "modules.h"
#include "language.h"

E int do_hs_sync(NickCore * nc, char *vIdent, char *hostmask,
				 char *creator, time_t time);

E void moduleAddHostServCmds();

/*************************************************************************/

void moduleAddHostServCmds()
{
	ModuleManager::LoadModuleList(Config.HostServCoreModules);
}

/*************************************************************************/

/**
 * Return information on memory use.
 * Assumes pointers are valid.
 **/

void get_hostserv_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;

	for (nickalias_map::const_iterator it = NickAliasList.begin(); it != NickAliasList.end(); ++it)
	{
		NickAlias *na = it->second;

		if (!na->hostinfo.HasVhost())
			continue;

		if (!na->hostinfo.GetIdent().empty())
			mem += na->hostinfo.GetIdent().size();
		if (!na->hostinfo.GetHost().empty())
			mem += na->hostinfo.GetHost().size();
		if (!na->hostinfo.GetCreator().empty())
			mem += na->hostinfo.GetCreator().size();
		++count;
	}

	*nrec = count;
	*memuse = mem;
}

/*************************************************************************/

/**
 * HostServ initialization.
 * @return void
 */
void hostserv_init()
{
	if (Config.s_HostServ) {
		moduleAddHostServCmds();
	}
}

/*************************************************************************/

/**
 * Main HostServ routine.
 * @param u User Struct
 * @param buf Buffer holding the message
 * @return void
 */
void hostserv(User * u, char *buf)
{
	const char *cmd, *s;

	cmd = strtok(buf, " ");

	if (!cmd) {
		return;
	} else if (stricmp(cmd, "\1PING") == 0) {
		if (!(s = strtok(NULL, ""))) {
			s = "";
		}
		ircdproto->SendCTCP(HostServ, u->nick.c_str(), "PING %s", s);
	} else {
		if (ircd->vhost) {
			mod_run_cmd(HostServ, u, cmd);
		} else {
			notice_lang(Config.s_HostServ, u, SERVICE_OFFLINE, Config.s_HostServ);
		}
	}
}

/** Set a vhost for the user
 * @param ident The ident
 * @param host The host
 * @param creator Who created the vhost
 * @param time When the vhost was craated
 */
void HostInfo::SetVhost(const std::string &ident, const std::string &host, const std::string &creator, time_t created)
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
const std::string &HostInfo::GetIdent() const
{
	return Ident;
}

/** Retrieve the vhost host
 * @return the host
 */
const std::string &HostInfo::GetHost() const
{
	return Host;
}

/** Retrieve the vhost creator
 * @return the creator
 */
const std::string &HostInfo::GetCreator() const
{
	return Creator;
}

/** Retrieve when the vhost was crated
 * @return the time it was created
 */
const time_t HostInfo::GetTime() const
{
	return Time;
}

/*************************************************************************/
/*	Start of Generic Functions					 */
/*************************************************************************/

/** Sync all vhosts in a group to the same thing
 * @param na The nick with the vhost wanting to by synced
 */
void HostServSyncVhosts(NickAlias *na)
{
	if (!na || !na->hostinfo.HasVhost())
		return;

	for (int i = 0; i < na->nc->aliases.count; i++)
	{
		NickAlias *nick = static_cast<NickAlias *>(na->nc->aliases.list[i]);
		nick->hostinfo.SetVhost(na->hostinfo.GetIdent(), na->hostinfo.GetHost(), na->hostinfo.GetCreator());
	}
}

/*************************************************************************/

void do_on_id(User *u)
{
	if (!u)
		return;
	NickAlias *na = findnick(u->nick);
	if (!na || !na->hostinfo.HasVhost())
		return;

	if (!u->vhost || u->vhost != na->hostinfo.GetHost() || (!na->hostinfo.GetIdent().empty() && u->GetVIdent() != na->hostinfo.GetIdent()))
	{
		ircdproto->SendVhost(u, na->hostinfo.GetIdent(), na->hostinfo.GetHost());
		if (ircd->vhost)
		{
			if (u->vhost)
				delete [] u->vhost;
			u->vhost = sstrdup(na->hostinfo.GetHost().c_str());
		}
		if (ircd->vident && !na->hostinfo.GetIdent().empty())
		{
			u->SetVIdent(na->hostinfo.GetIdent());
		}
		u->UpdateHost();

		if (!na->hostinfo.GetIdent().empty())
			notice_lang(Config.s_HostServ, u, HOST_IDENT_ACTIVATED, na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str());
		else
			notice_lang(Config.s_HostServ, u, HOST_ACTIVATED, na->hostinfo.GetHost().c_str());
	}
}


