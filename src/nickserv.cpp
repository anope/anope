/* NickServ functions.
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
#include "account.h"
#include "modules.h"
#include "users.h"
#include "protocol.h"

NickAlias* findnick(const Anope::string &nick)
{
	nickalias_map::const_iterator it = NickAliasList->find(nick);
	if (it != NickAliasList->end())
	{
		it->second->QueueUpdate();
		return it->second;
	}

	return NULL;
}

/*************************************************************************/

NickCore* findcore(const Anope::string &nick)
{
	nickcore_map::const_iterator it = NickCoreList->find(nick);
	if (it != NickCoreList->end())
	{
		it->second->QueueUpdate();
		return it->second;
	}

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
	if (!u->vhost.empty())
		buf2 = u->GetIdent() + "@" + u->vhost;
	if (!u->GetCloakedHost().empty())
		buf3 = u->GetIdent() + "@" + u->GetCloakedHost();

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
	NickCoreList->erase(nc->display);

	nc->display = newdisplay;

	(*NickCoreList)[nc->display] = nc;
}

void change_core_display(NickCore *nc)
{
	const NickAlias *na;
	if (nc->aliases.empty())
		return;
	na = nc->aliases.front();
	change_core_display(nc, na->nick);
}

std::set<IdentifyRequest *> IdentifyRequest::requests;

IdentifyRequest::IdentifyRequest(Module *o, const Anope::string &acc, const Anope::string &pass) : owner(o), account(acc), password(pass), dispatched(false), success(false)
{
	requests.insert(this);
}

IdentifyRequest::~IdentifyRequest()
{
	requests.erase(this);
}

void IdentifyRequest::Hold(Module *m)
{
	holds.insert(m);
}

void IdentifyRequest::Release(Module *m)
{
	holds.erase(m);
	if (holds.empty() && dispatched)
	{
		if (!success)
			this->OnFail();
		delete this;
	}
}

void IdentifyRequest::Success(Module *m)
{
	if (!success)
	{
		this->OnSuccess();
		success = true;
	}
}

void IdentifyRequest::Dispatch()
{
	if (holds.empty())
	{
		if (!success)
			this->OnFail();
		delete this;
	}
	else
		dispatched = true;
}

void IdentifyRequest::ModuleUnload(Module *m)
{
	for (std::set<IdentifyRequest *>::iterator it = requests.begin(), it_end = requests.end(); it != it_end;)
	{
		IdentifyRequest *ir = *it;
		++it;

		ir->Release(m);
		if (ir->owner == m)
			delete ir;
	}
}
