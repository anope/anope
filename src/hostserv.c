/* HostServ functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */

/*************************************************************************/
#include "services.h"
#include "pseudo.h"

#define HASH(nick)	((tolower((nick)[0])&31)<<5 | (tolower((nick)[1])&31))

HostCore *head = NULL;		  /* head of the HostCore list */

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
	HostCore *hc;

	for (hc = head; hc; hc = hc->next) {
		count++;
		mem += sizeof(*hc);
		if (hc->nick)
			mem += strlen(hc->nick) + 1;
		if (hc->vIdent)
			mem += strlen(hc->vIdent) + 1;
		if (hc->vHost)
			mem += strlen(hc->vHost) + 1;
		if (hc->creator)
			mem += strlen(hc->creator) + 1;
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
		ircdproto->SendCTCP(findbot(Config.s_HostServ), u->nick, "PING %s", s);
	} else {
		if (ircd->vhost) {
			mod_run_cmd(Config.s_HostServ, u, HOSTSERV, cmd);
		} else {
			notice_lang(Config.s_HostServ, u, SERVICE_OFFLINE, Config.s_HostServ);
		}
	}
}

/*************************************************************************/
/*	Start of Linked List routines										 */
/*************************************************************************/

HostCore *hostCoreListHead()
{
	return head;
}

/**
 * Create HostCore list member
 * @param next HostCore next slot
 * @param nick Nick to add
 * @param vIdent Virtual Ident
 * @param vHost  Virtual Host
 * @param creator Person whom set the vhost
 * @param time Time the vhost was Set
 * @return HostCore
 */
HostCore *createHostCorelist(HostCore * next, const char *nick, char *vIdent,
							 char *vHost, const char *creator, int32 tmp_time)
{

	next = new HostCore;
	if (next == NULL) {
		ircdproto->SendGlobops(Config.s_HostServ,
						 "Unable to allocate memory to create the vHost LL, problems i sense..");
	} else {
		next->nick = new char[strlen(nick) + 1];
		next->vHost = new char[strlen(vHost) + 1];
		next->creator = new char[strlen(creator) + 1];
		if (vIdent)
			next->vIdent = new char[strlen(vIdent) + 1];
		if ((next->nick == NULL) || (next->vHost == NULL)
			|| (next->creator == NULL)) {
			ircdproto->SendGlobops(Config.s_HostServ,
							 "Unable to allocate memory to create the vHost LL, problems i sense..");
			return NULL;
		}
		strlcpy(next->nick, nick, strlen(nick) + 1);
		strlcpy(next->vHost, vHost, strlen(vHost) + 1);
		strlcpy(next->creator, creator, strlen(creator) + 1);
		if (vIdent) {
			if ((next->vIdent == NULL)) {
				ircdproto->SendGlobops(Config.s_HostServ,
								 "Unable to allocate memory to create the vHost LL, problems i sense..");
				return NULL;
			}
			strlcpy(next->vIdent, vIdent, strlen(vIdent) + 1);
		} else {
			next->vIdent = NULL;
		}
		next->time = tmp_time;
		next->next = NULL;
		return next;
	}
	return NULL;
}

/*************************************************************************/
/**
 * Returns either NULL for the head, or the location of the *PREVIOUS*
 * record, this is where we need to insert etc..
 * @param head HostCore head
 * @param nick Nick to find
 * @param found If found
 * @return HostCore
 */
HostCore *findHostCore(HostCore * phead, const char *nick, bool* found)
{
	HostCore *previous, *current;

	*found = false;
	current = phead;
	previous = current;

	if (!nick) {
		return NULL;
	}
	FOREACH_MOD(I_OnFindHostCore, OnFindHostCore(nick));
	while (current != NULL) {
		if (stricmp(nick, current->nick) == 0) {
			*found = true;
			break;
		} else if (stricmp(nick, current->nick) < 0) {
			/* we know were not gonna find it now.... */
			break;
		} else {
			previous = current;
			current = current->next;
		}
	}
	if (current == phead) {
		return NULL;
	} else {
		return previous;
	}
}

/*************************************************************************/
HostCore *insertHostCore(HostCore * phead, HostCore * prev, const char *nick,
						 char *vIdent, char *vHost, const char *creator,
						 int32 tmp_time)
{

	HostCore *newCore, *tmp;

	if (!nick || !vHost || !creator) {
		return NULL;
	}

	newCore = new HostCore;
	if (newCore == NULL) {
		ircdproto->SendGlobops(Config.s_HostServ,
						 "Unable to allocate memory to insert into the vHost LL, problems i sense..");
		return NULL;
	} else {
		newCore->nick = new char[strlen(nick) + 1];
		newCore->vHost = new char[strlen(vHost) + 1];
		newCore->creator = new char[strlen(creator) + 1];
		if (vIdent)
			newCore->vIdent = new char[strlen(vIdent) + 1];
		if ((newCore->nick == NULL) || (newCore->vHost == NULL)
			|| (newCore->creator == NULL)) {
			ircdproto->SendGlobops(Config.s_HostServ,
							 "Unable to allocate memory to create the vHost LL, problems i sense..");
			return NULL;
		}
		strlcpy(newCore->nick, nick, strlen(nick) + 1);
		strlcpy(newCore->vHost, vHost, strlen(vHost) + 1);
		strlcpy(newCore->creator, creator, strlen(creator) + 1);
		if (vIdent) {
			if ((newCore->vIdent == NULL)) {
				ircdproto->SendGlobops(Config.s_HostServ,
								 "Unable to allocate memory to create the vHost LL, problems i sense..");
				return NULL;
			}
			strlcpy(newCore->vIdent, vIdent, strlen(vIdent) + 1);
		} else {
			newCore->vIdent = NULL;
		}
		newCore->time = tmp_time;
		if (prev == NULL) {
			tmp = phead;
			phead = newCore;
			newCore->next = tmp;
		} else {
			tmp = prev->next;
			prev->next = newCore;
			newCore->next = tmp;
		}
	}
	FOREACH_MOD(I_OnInsertHostCore, OnInsertHostCore(newCore));
	return phead;
}

/*************************************************************************/
HostCore *deleteHostCore(HostCore * phead, HostCore * prev)
{

	HostCore *tmp;

	if (prev == NULL) {
		tmp = phead;
		phead = phead->next;
	} else {
		tmp = prev->next;
		prev->next = tmp->next;
	}
	FOREACH_MOD(I_OnDeleteHostCore, OnDeleteHostCore(tmp));
	delete [] tmp->vHost;
	delete [] tmp->nick;
	delete [] tmp->creator;
	if (tmp->vIdent) {
		delete [] tmp->vIdent;
	}
	delete tmp;
	return phead;
}

/*************************************************************************/
void addHostCore(const char *nick, char *vIdent, char *vhost, const char *creator,
				 int32 tmp_time)
{
	HostCore *tmp;
	bool found = false;

	if (head == NULL) {
		head =
			createHostCorelist(head, nick, vIdent, vhost, creator,
							   tmp_time);
	} else {
		tmp = findHostCore(head, nick, &found);
		if (!found) {
			head =
				insertHostCore(head, tmp, nick, vIdent, vhost, creator,
							   tmp_time);
		} else {
			head = deleteHostCore(head, tmp);   /* delete the old entry */
			addHostCore(nick, vIdent, vhost, creator, tmp_time);		/* recursive call to add new entry */
		}
	}
}

/*************************************************************************/
char *getvHost(char *nick)
{
	HostCore *tmp;
	bool found = false;
	tmp = findHostCore(head, nick, &found);
	if (found) {
		if (tmp == NULL)
			return head->vHost;
		else
			return tmp->next->vHost;
	}
	return NULL;
}

/*************************************************************************/
char *getvIdent(char *nick)
{
	HostCore *tmp;
	bool found = false;
	tmp = findHostCore(head, nick, &found);
	if (found) {
		if (tmp == NULL)
			return head->vIdent;
		else
			return tmp->next->vIdent;
	}
	return NULL;
}

/*************************************************************************/
void delHostCore(const char *nick)
{
	HostCore *tmp;
	bool found = false;
	tmp = findHostCore(head, nick, &found);
	if (found) {
		head = deleteHostCore(head, tmp);
	}

}

/*************************************************************************/
/*	End of Linked List routines					 */
/*************************************************************************/

/*************************************************************************/
/*	Start of Generic Functions					 */
/*************************************************************************/

int do_hs_sync(NickCore * nc, char *vIdent, char *hostmask, char *creator,
			   time_t time)
{
	int i;
	NickAlias *na;

	for (i = 0; i < nc->aliases.count; i++) {
		na = static_cast<NickAlias *>(nc->aliases.list[i]);
		addHostCore(na->nick, vIdent, hostmask, creator, time);
	}
	return MOD_CONT;
}

/*************************************************************************/
int do_on_id(User * u)
{							   /* we've assumed that the user exists etc.. */
	char *vHost;				/* as were only gonna call this from nsid routine */
	char *vIdent;
	vHost = getvHost(u->nick);
	vIdent = getvIdent(u->nick);
	if (vHost != NULL) {
		if (vIdent) {
			notice_lang(Config.s_HostServ, u, HOST_IDENT_ACTIVATED, vIdent,
						vHost);
		} else {
			notice_lang(Config.s_HostServ, u, HOST_ACTIVATED, vHost);
		}
		ircdproto->SendVhost(u->nick, vIdent, vHost);
		if (ircd->vhost)
		{
			if (u->vhost)
				delete [] u->vhost;
			u->vhost = sstrdup(vHost);
		}
		if (ircd->vident) {
			if (vIdent)
				u->SetVIdent(vIdent);
		}
		set_lastmask(u);
	}
	return MOD_CONT;
}

/*************************************************************************/

/*
 * Sets the last_usermak properly. Using virtual ident and/or host
 */
void set_lastmask(User * u)
{
	NickAlias *na = findnick(u->nick);
	if (na->last_usermask)
		delete [] na->last_usermask;

	std::string last_usermask = u->GetIdent() + "@" + u->GetDisplayedHost();
	na->last_usermask = sstrdup(last_usermask.c_str());
}
