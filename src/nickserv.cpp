
/* NickServ functions.
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

nickalias_map NickAliasList;
nickcore_map NickCoreList;
nickrequest_map NickRequestList;

static std::map<std::string, NickServCollide *> NickServCollides;
static std::map<std::string, NickServRelease *> NickServReleases;

NickServCollide::NickServCollide(const std::string &_nick, time_t delay) : Timer(delay), nick(_nick)
{
	/* Erase the current collide and use the new one */
	std::map<std::string, NickServCollide *>::iterator nit = NickServCollides.find(nick);
	if (nit != NickServCollides.end())
	{
		delete nit->second;
	}

	NickServCollides.insert(std::make_pair(nick, this));
}

NickServCollide::~NickServCollide()
{
	NickServCollides.erase(nick);
}

void NickServCollide::Tick(time_t ctime)
{
	/* If they identified or don't exist anymore, don't kill them. */
	User *u = finduser(nick);
	NickAlias *na = findnick(nick);
	if (!u || !na || u->Account() == na->nc || u->my_signon > this->GetSetTime())
		return;
	
	u->Collide(na);
}

NickServRelease::NickServRelease(const std::string &_nick, const std::string &_uid, time_t delay) : Timer(delay), nick(_nick), uid(_uid)
{
	/* Erase the current release timer and use the new one */
	std::map<std::string, NickServRelease *>::iterator nit = NickServReleases.find(nick);
	if (nit != NickServReleases.end())
	{
		delete nit->second;
	}

	NickServReleases.insert(std::make_pair(nick, this));
}

NickServRelease::~NickServRelease()
{
	NickServReleases.erase(nick);
}

void NickServRelease::Tick(time_t ctime)
{
	NickAlias *na = findnick(nick);

	if (na)
		na->Release();
}

/*************************************************************************/
/* *INDENT-OFF* */
void moduleAddNickServCmds()
{
	ModuleManager::LoadModuleList(Config.NickServCoreModules);
}
/* *INDENT-ON* */
/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_aliases_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;

	for (nickalias_map::const_iterator it = NickAliasList.begin(); it != NickAliasList.end(); ++it)
	{
		NickAlias *na = it->second;
			
		count++;
		mem += sizeof(*na);
		if (na->nick)
			mem += strlen(na->nick) + 1;
		if (na->last_usermask)
			mem += strlen(na->last_usermask) + 1;
		if (na->last_realname)
			mem += strlen(na->last_realname) + 1;
		if (na->last_quit)
			mem += strlen(na->last_quit) + 1;
	}
	*nrec = count;
	*memuse = mem;
}

/*************************************************************************/

/* Return information on memory use.  Assumes pointers are valid. */

void get_core_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;
	unsigned j;

	for (nickcore_map::const_iterator it = NickCoreList.begin(); it != NickCoreList.end(); ++it)
	{
		NickCore *nc = it->second;

		count++;
		mem += sizeof(*nc);

		if (nc->display)
			mem += strlen(nc->display) + 1;
		if (!nc->pass.empty())
			mem += (nc->pass.capacity() + (2 * sizeof(size_t)) + (2 * sizeof(void*)));
		if (nc->url)
			mem += strlen(nc->url) + 1;
		if (nc->email)
			mem += strlen(nc->email) + 1;
		if (nc->greet)
			mem += strlen(nc->greet) + 1;

		mem += sizeof(std::string) * nc->access.size();
		for (j = 0; j < nc->access.size(); ++j)
			mem += nc->GetAccess(j).length() + 1;

		mem += nc->memos.memos.size() * sizeof(Memo);
		for (j = 0; j < nc->memos.memos.size(); j++)
		{
			if (nc->memos.memos[j]->text)
				mem += strlen(nc->memos.memos[j]->text) + 1;
		}

		mem += sizeof(NickAlias *) * nc->aliases.size();
	}
	*nrec = count;
	*memuse = mem;
}

/*************************************************************************/
/*************************************************************************/

/* NickServ initialization. */

void ns_init()
{
	moduleAddNickServCmds();
}

/*************************************************************************/

/* Main NickServ routine. */

void nickserv(User *u, const std::string &buf)
{
	if (!u || buf.empty())
		return;
	
	if (buf.find("\1PING ", 0, 6) != std::string::npos && buf[buf.length() - 1] == '\1')
	{
		std::string command = buf;
		command.erase(command.begin());
		command.erase(command.end());
		ircdproto->SendCTCP(NickServ, u->nick.c_str(), "%s", command.c_str());
	}
	else
	{
		mod_run_cmd(NickServ, u, buf);
	}

}

/*************************************************************************/

/* Check whether a user is on the access list of the nick they're using If
 * not, send warnings as appropriate.  If so (and not NI_SECURE), update
 * last seen info.
 * Return 1 if the user is valid and recognized, 0 otherwise (note
 * that this means an NI_SECURE nick will return 0 from here).
 * If the user's nick is not registered, 0 is returned.
 */

int validate_user(User * u)
{
	NickAlias *na;
	NickRequest *nr;

	if ((nr = findrequestnick(u->nick)))
	{
		notice_lang(Config.s_NickServ, u, NICK_IS_PREREG);
		return 0;
	}

	if (!(na = findnick(u->nick)))
		return 0;

	if (na->HasFlag(NS_FORBIDDEN))
	{
		notice_lang(Config.s_NickServ, u, NICK_MAY_NOT_BE_USED);
		u->Collide(na);
		return 0;
	}

	if (na->nc->HasFlag(NI_SUSPENDED))
	{
		notice_lang(Config.s_NickServ, u, NICK_X_SUSPENDED, u->nick.c_str());
		u->Collide(na);
		return 0;
	}

	if (!na->nc->HasFlag(NI_SECURE) && u->IsRecognized())
	{
		na->last_seen = time(NULL);
		if (na->last_usermask)
			delete [] na->last_usermask;
		std::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
		na->last_usermask = sstrdup(last_usermask.c_str());
		if (na->last_realname)
			delete [] na->last_realname;
		na->last_realname = sstrdup(u->realname);

		check_memos(u);

		return 1;
	}

	if (u->IsRecognized() || !na->nc->HasFlag(NI_KILL_IMMED))
	{
		if (na->nc->HasFlag(NI_SECURE))
			notice_lang(Config.s_NickServ, u, NICK_IS_SECURE, Config.s_NickServ);
		else
			notice_lang(Config.s_NickServ, u, NICK_IS_REGISTERED, Config.s_NickServ);
	}

	if (na->nc->HasFlag(NI_KILLPROTECT) && !u->IsRecognized())
	{
		if (na->nc->HasFlag(NI_KILL_IMMED))
		{
			notice_lang(Config.s_NickServ, u, FORCENICKCHANGE_NOW);
			u->Collide(na);
		}
		else if (na->nc->HasFlag(NI_KILL_QUICK))
		{
			notice_lang(Config.s_NickServ, u, FORCENICKCHANGE_IN_20_SECONDS);
			new NickServCollide(na->nick, 20);
		}
		else
		{
			notice_lang(Config.s_NickServ, u, FORCENICKCHANGE_IN_1_MINUTE);
			new NickServCollide(na->nick, 60);
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

	for (nickalias_map::const_iterator it = NickAliasList.begin(); it != NickAliasList.end();)
	{
		NickAlias *na = it->second;
		++it;
		
		User *u = finduser(na->nick);
		if (u && (na->nc->HasFlag(NI_SECURE) ? u->IsIdentified() : u->IsRecognized()))
		{
			Alog(LOG_DEBUG_2) << "NickServ: updating last seen time for " << na->nick;
			na->last_seen = now;
			continue;
		}

		if (Config.NSExpire && now - na->last_seen >= Config.NSExpire
		        && !na->HasFlag(NS_FORBIDDEN) && !na->HasFlag(NS_NO_EXPIRE)
		        && !na->nc->HasFlag(NI_SUSPENDED))
		{
			EventReturn MOD_RESULT;
			FOREACH_RESULT(I_OnPreNickExpire, OnPreNickExpire(na));
			if (MOD_RESULT == EVENT_STOP)
				continue;
			Alog() << "Expiring nickname " << na->nick << " (group: " << na->nc->display << ") (e-mail: "
				<< (na->nc->email ? na->nc->email : "none") << ")";
			FOREACH_MOD(I_OnNickExpire, OnNickExpire(na));
			delete na;
		}
	}
}

void expire_requests()
{
	time_t now = time(NULL);

	for (nickrequest_map::const_iterator it = NickRequestList.begin(); it != NickRequestList.end(); ++it)
	{
		NickRequest *nr = it->second;
		
		if (Config.NSRExpire && now - nr->requested >= Config.NSRExpire)
		{
			Alog() << "Request for nick " << nr->nick << " expiring";
			delete nr;
		}
	}
}

/*************************************************************************/

NickRequest *findrequestnick(const char *nick)
{
	return findrequestnick(ci::string(nick));
}

NickRequest *findrequestnick(const std::string &nick)
{
	return findrequestnick(ci::string(nick.c_str()));
}

NickRequest *findrequestnick(const ci::string &nick)
{
	nickrequest_map::const_iterator it = NickRequestList.find(nick);

	if (it != NickRequestList.end())
		return it->second;
	return NULL;
}

NickAlias *findnick(const char *nick)
{
	return findnick(ci::string(nick));
}

NickAlias *findnick(const std::string &nick)
{
	return findnick(ci::string(nick.c_str()));
}

NickAlias *findnick(const ci::string &nick)
{
	nickalias_map::const_iterator it = NickAliasList.find(nick);

	if (it != NickAliasList.end())
		return it->second;
	return NULL;
}

/*************************************************************************/

NickCore *findcore(const char *nick)
{
	return findcore(ci::string(nick));
}

NickCore *findcore(const std::string &nick)
{
	return findcore(ci::string(nick.c_str()));
}

NickCore *findcore(const ci::string &nick)
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
bool is_on_access(User *u, NickCore *nc)
{
	unsigned i;
	char *buf;
	char *buf2 = NULL;
	char *buf3 = NULL;
	std::string tmp_buf;

	if (!u || !nc || nc->access.empty())
		return false;

	tmp_buf = u->GetIdent() + "@" + u->host;
	buf = sstrdup(tmp_buf.c_str());
	if (ircd->vhost)
	{
		if (u->vhost)
		{
			tmp_buf = u->GetIdent() + "@" + u->vhost;
			buf2 = sstrdup(tmp_buf.c_str());
		}
		if (!u->GetCloakedHost().empty())
		{
			tmp_buf = u->GetIdent() + "@" + u->GetCloakedHost();
			buf3 = sstrdup(tmp_buf.c_str());
		}
	}

	for (i = 0; i < nc->access.size(); i++)
	{
		std::string access = nc->GetAccess(i);
		if (Anope::Match(buf, access, false) || (buf2 && Anope::Match(buf2, access, false)) || (buf3 && Anope::Match(buf3, access, false)))
		{
			delete [] buf;
			if (ircd->vhost)
			{
				if (u->vhost)
				{
					delete [] buf2;
				}
				if (!u->GetCloakedHost().empty())
				{
					delete [] buf3;
				}
			}
			return true;
		}
	}
	delete [] buf;
	if (ircd->vhost)
	{
		if (buf2)
			delete [] buf2;
		if (buf3)
			delete [] buf3;
	}
	return false;
}

/*************************************************************************/

/* Sets nc->display to newdisplay. If newdisplay is NULL, it will change
 * it to the first alias in the list.
 */


void change_core_display(NickCore * nc, const char *newdisplay)
{
	/* Log ... */
	FOREACH_MOD(I_OnChangeCoreDisplay, OnChangeCoreDisplay(nc, newdisplay));
	Alog() << Config.s_NickServ << ": changing " << nc->display << " nickname group display to " << newdisplay;

	/* Remove the core from the list */
	NickCoreList.erase(nc->display);

	delete [] nc->display;
	nc->display = sstrdup(newdisplay);

	NickCoreList[nc->display] = nc;
}

void change_core_display(NickCore * nc)
{
	NickAlias *na;
	if (nc->aliases.empty())
		return;
	na = nc->aliases.front();
	change_core_display(nc,na->nick);
}


/*************************************************************************/
/*********************** NickServ command routines ***********************/
/*************************************************************************/

int do_setmodes(User * u)
{
	Channel *c;

	/* Walk users current channels */
	for (UChannelList::iterator it = u->chans.begin(); it != u->chans.end(); ++it)
	{
		ChannelContainer *cc = *it;

		if ((c = cc->chan))
			chan_set_correct_modes(u, c, 1);
	}
	return MOD_CONT;
}

