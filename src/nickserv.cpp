/* NickServ functions.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"

nickalias_map NickAliasList;
nickcore_map NickCoreList;

typedef std::map<Anope::string, NickServCollide *> nickservcollides_map;
typedef std::map<Anope::string, NickServHeld *> nickservheld_map;
typedef std::map<Anope::string, NickServRelease *> nickservreleases_map;

static nickservcollides_map NickServCollides;
static nickservheld_map NickServHelds;
static nickservreleases_map NickServReleases;

NickServCollide::NickServCollide(User *user, time_t delay) : Timer(delay), u(user), nick(u->nick)
{
	/* Erase the current collide and use the new one */
	nickservcollides_map::iterator nit = NickServCollides.find(user->nick);
	if (nit != NickServCollides.end())
		delete nit->second;

	NickServCollides.insert(std::make_pair(nick, this));
}

NickServCollide::~NickServCollide()
{
	NickServCollides.erase(this->nick);
}

void NickServCollide::Tick(time_t ctime)
{
	if (!u)
		return;
	/* If they identified or don't exist anymore, don't kill them. */
	NickAlias *na = findnick(u->nick);
	if (!na || u->Account() == na->nc || u->my_signon > this->GetSetTime())
		return;

	u->Collide(na);
}

NickServHeld::NickServHeld(NickAlias *n, long l) : Timer(l), na(n), nick(na->nick)
{
	nickservheld_map::iterator nit = NickServHelds.find(na->nick);
	if (nit != NickServHelds.end())
		delete nit->second;

	NickServHelds[na->nick] = this;
}

NickServHeld::~NickServHeld()
{
	NickServHelds.erase(this->nick);
}

void NickServHeld::Tick(time_t)
{
	if (na)
		na->UnsetFlag(NS_HELD);
}

NickServRelease::NickServRelease(NickAlias *na, time_t delay) : User(na->nick, Config->NSEnforcerUser, Config->NSEnforcerHost, ts6_uid_retrieve()), Timer(delay), nick(na->nick)
{
	this->realname = "Services Enforcer";
	this->server = Me;

	/* Erase the current release timer and use the new one */
	nickservreleases_map::iterator nit = NickServReleases.find(this->nick);
	if (nit != NickServReleases.end())
		delete nit->second;

	NickServReleases.insert(std::make_pair(this->nick, this));

	ircdproto->SendClientIntroduction(this);
}

NickServRelease::~NickServRelease()
{
	NickServReleases.erase(this->nick);

	ircdproto->SendQuit(this, "");
}

void NickServRelease::Tick(time_t)
{
	/* Do not do anything here,
	 * The timer manager will delete this timer which will do the necessary cleanup
	 */
}

NickAlias *findnick(const Anope::string &nick)
{
	FOREACH_MOD(I_OnFindNick, OnFindNick(nick));

	nickalias_map::const_iterator it = NickAliasList.find(nick);
	if (it != NickAliasList.end())
		return it->second;

	return NULL;
}

/*************************************************************************/

NickCore *findcore(const Anope::string &nick)
{
	FOREACH_MOD(I_OnFindCore, OnFindCore(nick));

	nickcore_map::const_iterator it = NickCoreList.find(nick);
	if (it != NickCoreList.end())
		return it->second;

	return NULL;
}

/** Is the user's address on the nickcores access list?
 * @param u The user
 * @param nc The nickcore
 * @return true or false
 */
bool is_on_access(const User *u, const NickCore *nc)
{
	if (!u || !nc || nc->access.empty())
		return false;

	Anope::string buf = u->GetIdent() + "@" + u->host, buf2, buf3;
	if (ircd->vhost)
	{
		if (!u->vhost.empty())
			buf2 = u->GetIdent() + "@" + u->vhost;
		if (!u->GetCloakedHost().empty())
			buf3 = u->GetIdent() + "@" + u->GetCloakedHost();
	}

	for (unsigned i = 0, end = nc->access.size(); i < end; ++i)
	{
		Anope::string access = nc->GetAccess(i);
		if (Anope::Match(buf, access) || (!buf2.empty() && Anope::Match(buf2, access)) || (!buf3.empty() && Anope::Match(buf3, access)))
			return true;
	}
	return false;
}

/*************************************************************************/

/* Sets nc->display to newdisplay. If newdisplay is NULL, it will change
 * it to the first alias in the list.
 */

void change_core_display(NickCore *nc, const Anope::string &newdisplay)
{
	FOREACH_MOD(I_OnChangeCoreDisplay, OnChangeCoreDisplay(nc, newdisplay));

	/* Remove the core from the list */
	NickCoreList.erase(nc->display);

	nc->display = newdisplay;

	NickCoreList[nc->display] = nc;
}

void change_core_display(NickCore *nc)
{
	NickAlias *na;
	if (nc->aliases.empty())
		return;
	na = nc->aliases.front();
	change_core_display(nc, na->nick);
}

