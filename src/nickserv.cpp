/* NickServ functions.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "language.h"

nickalias_map NickAliasList;
nickcore_map NickCoreList;
nickrequest_map NickRequestList;

typedef std::map<Anope::string, NickServCollide *> nickservcollides_map;
typedef std::map<Anope::string, NickServRelease *> nickservreleases_map;

static nickservcollides_map NickServCollides;
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

NickServRelease::NickServRelease(NickAlias *na, time_t delay) : User(na->nick, Config->NSEnforcerUser, Config->NSEnforcerHost, ts6_uid_retrieve()), Timer(delay), nick(na->nick)
{
	this->realname = "Services Enforcer";
	this->server = Me;

	/* Erase the current release timer and use the new one */
	nickservreleases_map::iterator nit = NickServReleases.find(this->nick);
	if (nit != NickServReleases.end())
		delete nit->second;

	NickServReleases.insert(std::make_pair(this->nick, this));

	ircdproto->SendClientIntroduction(this, "+");
}

NickServRelease::~NickServRelease()
{
	NickServReleases.erase(this->nick);

	ircdproto->SendQuit(debug_cast<User *>(this), NULL);
}

void NickServRelease::Tick(time_t)
{
	/* Do not do anything here,
	 * The timer manager will delete this timer which will do the necessary cleanup
	 */
}

/*************************************************************************/
/* *INDENT-OFF* */
void moduleAddNickServCmds()
{
	ModuleManager::LoadModuleList(Config->NickServCoreModules);
}
/* *INDENT-ON* */
/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_aliases_stats(long &count, long &mem)
{
	count = mem = 0;

	for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
	{
		NickAlias *na = it->second;

		++count;
		mem += sizeof(*na);
		if (!na->nick.empty())
			mem += na->nick.length() + 1;
		if (!na->last_usermask.empty())
			mem += na->last_usermask.length() + 1;
		if (!na->last_realname.empty())
			mem += na->last_realname.length() + 1;
		if (!na->last_quit.empty())
			mem += na->last_quit.length() + 1;
	}
}

/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_core_stats(long &count, long &mem)
{
	count = mem = 0;
	unsigned j, end;

	for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
	{
		NickCore *nc = it->second;

		++count;
		mem += sizeof(*nc);

		if (!nc->display.empty())
			mem += nc->display.length() + 1;
		if (!nc->pass.empty())
			mem += nc->pass.length() + 1;
		if (!nc->greet.empty())
			mem += nc->greet.length() + 1;

		mem += sizeof(Anope::string) * nc->access.size();

		for (j = 0, end = nc->access.size(); j < end; ++j)
			mem += nc->GetAccess(j).length() + 1;

		mem += nc->memos.memos.size() * sizeof(Memo);
		for (j = 0, end = nc->memos.memos.size(); j < end; ++j)
			if (!nc->memos.memos[j]->text.empty())
				mem += nc->memos.memos[j]->text.length() + 1;

		mem += sizeof(NickAlias *) * nc->aliases.size();
	}
}

/*************************************************************************/
/*************************************************************************/

/* NickServ initialization. */

void ns_init()
{
	moduleAddNickServCmds();
}

/*************************************************************************/

/* Check whether a user is on the access list of the nick they're using If
 * not, send warnings as appropriate.  If so (and not NI_SECURE), update
 * last seen info.
 * Return 1 if the user is valid and recognized, 0 otherwise (note
 * that this means an NI_SECURE nick will return 0 from here).
 * If the user's nick is not registered, 0 is returned.
 */

int validate_user(User *u)
{
	NickRequest *nr = findrequestnick(u->nick);
	if (nr)
	{
		notice_lang(Config->s_NickServ, u, NICK_IS_PREREG);
		return 0;
	}

	NickAlias *na = findnick(u->nick);
	if (!na)
		return 0;

	if (na->HasFlag(NS_FORBIDDEN))
	{
		notice_lang(Config->s_NickServ, u, NICK_MAY_NOT_BE_USED);
		u->Collide(na);
		return 0;
	}

	if (na->nc->HasFlag(NI_SUSPENDED))
	{
		notice_lang(Config->s_NickServ, u, NICK_X_SUSPENDED, u->nick.c_str());
		u->Collide(na);
		return 0;
	}

	if (!na->nc->HasFlag(NI_SECURE) && u->IsRecognized())
	{
		na->last_seen = time(NULL);
		Anope::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
		na->last_usermask = last_usermask;
		na->last_realname = u->realname;

		check_memos(u);

		return 1;
	}

	if (u->IsRecognized() || !na->nc->HasFlag(NI_KILL_IMMED))
	{
		if (na->nc->HasFlag(NI_SECURE))
			notice_lang(Config->s_NickServ, u, NICK_IS_SECURE, Config->s_NickServ.c_str());
		else
			notice_lang(Config->s_NickServ, u, NICK_IS_REGISTERED, Config->s_NickServ.c_str());
	}

	if (na->nc->HasFlag(NI_KILLPROTECT) && !u->IsRecognized())
	{
		if (na->nc->HasFlag(NI_KILL_IMMED))
		{
			notice_lang(Config->s_NickServ, u, FORCENICKCHANGE_NOW);
			u->Collide(na);
		}
		else if (na->nc->HasFlag(NI_KILL_QUICK))
		{
			notice_lang(Config->s_NickServ, u, FORCENICKCHANGE_IN_20_SECONDS);
			new NickServCollide(u, 20);
		}
		else
		{
			notice_lang(Config->s_NickServ, u, FORCENICKCHANGE_IN_1_MINUTE);
			new NickServCollide(u, 60);
		}
	}

	return 0;
}

/*************************************************************************/

/* Remove all nicks which have expired.  Also update last-seen time for all
 * nicks.
 */

void expire_nicks()
{
	time_t now = time(NULL);

	for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; )
	{
		NickAlias *na = it->second;
		++it;

		User *u = finduser(na->nick);
		if (u && (na->nc->HasFlag(NI_SECURE) ? u->IsIdentified() : u->IsRecognized()))
		{
			Log(LOG_DEBUG_2) << "NickServ: updating last seen time for " << na->nick;
			na->last_seen = now;
			continue;
		}

		if (Config->NSExpire && now - na->last_seen >= Config->NSExpire && !na->HasFlag(NS_FORBIDDEN) && !na->HasFlag(NS_NO_EXPIRE) && !na->nc->HasFlag(NI_SUSPENDED))
		{
			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnPreNickExpire, OnPreNickExpire(na));
			if (MOD_RESULT == EVENT_STOP)
				continue;
			Log() << "Expiring nickname " << na->nick << " (group: " << na->nc->display << ") (e-mail: " << (na->nc->email.empty() ? "none" : na->nc->email) << ")";
			FOREACH_MOD(I_OnNickExpire, OnNickExpire(na));
			delete na;
		}
	}
}

void expire_requests()
{
	time_t now = time(NULL);

	for (nickrequest_map::const_iterator it = NickRequestList.begin(), it_end = NickRequestList.end(); it != it_end; )
	{
		NickRequest *nr = it->second;
		++it;

		if (Config->NSRExpire && now - nr->requested >= Config->NSRExpire)
		{
			Log() << "Request for nick " << nr->nick << " expiring";
			delete nr;
		}
	}
}

/*************************************************************************/

NickRequest *findrequestnick(const Anope::string &nick)
{
	nickrequest_map::const_iterator it = NickRequestList.find(nick);

	if (it != NickRequestList.end())
		return it->second;
	return NULL;
}

NickAlias *findnick(const Anope::string &nick)
{
	nickalias_map::const_iterator it = NickAliasList.find(nick);

	if (it != NickAliasList.end())
		return it->second;
	return NULL;
}

/*************************************************************************/

NickCore *findcore(const Anope::string &nick)
{
	nickcore_map::const_iterator it = NickCoreList.find(nick);

	if (it != NickCoreList.end())
		return it->second;
	return NULL;
}

/*************************************************************************/
/*********************** NickServ private routines ***********************/
/*************************************************************************/

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
	/* Log ... */
	FOREACH_MOD(I_OnChangeCoreDisplay, OnChangeCoreDisplay(nc, newdisplay));
	Log() << Config->s_NickServ << ": changing " << nc->display << " nickname group display to " << newdisplay;

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

/*************************************************************************/
/*********************** NickServ command routines ***********************/
/*************************************************************************/

int do_setmodes(User *u)
{
	/* Walk users current channels */
	for (UChannelList::iterator it = u->chans.begin(), it_end = u->chans.end(); it != it_end; ++it)
	{
		ChannelContainer *cc = *it;

		Channel *c = cc->chan;
		if (c)
			chan_set_correct_modes(u, c, 1);
	}
	return MOD_CONT;
}
