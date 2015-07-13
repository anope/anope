/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#include "services.h"
#include "modules.h"
#include "account.h"
#include "config.h"

Serialize::Checker<nickcore_map> NickCoreList("NickCore");

NickCore::NickCore(const Anope::string &coredisplay) : Serializable("NickCore"), chanaccess("ChannelInfo"), aliases("NickAlias")
{
	if (coredisplay.empty())
		throw CoreException("Empty display passed to NickCore constructor");

	this->o = NULL;
	this->channelcount = 0;
	this->lastmail = 0;

	this->display = coredisplay;

	size_t old = NickCoreList->size();
	(*NickCoreList)[this->display] = this;
	if (old == NickCoreList->size())
		Log(LOG_DEBUG) << "Duplicate account " << coredisplay << " in nickcore table?";
	
	FOREACH_MOD(OnNickCoreCreate, (this));
}

NickCore::~NickCore()
{
	UnsetExtensibles();

	FOREACH_MOD(OnDelCore, (this));

	if (!this->chanaccess->empty())
		Log(LOG_DEBUG) << "Non-empty chanaccess list in destructor!";

	for (std::list<User *>::iterator it = this->users.begin(); it != this->users.end();)
	{
		User *user = *it++;
		user->Logout();
	}
	this->users.clear();

	NickCoreList->erase(this->display);

	this->ClearAccess();

	if (!this->memos.memos->empty())
	{
		for (unsigned i = 0, end = this->memos.memos->size(); i < end; ++i)
			delete this->memos.GetMemo(i);
		this->memos.memos->clear();
	}
}

void NickCore::Serialize(Serialize::Data &data) const
{
	data["display"] << this->display;
	data["pass"] << this->pass;
	data["email"] << this->email;
	data["language"] << this->language;
	for (unsigned i = 0; i < this->access.size(); ++i)
		data["access"] << this->access[i] << " ";
	data["memomax"] << this->memos.memomax;
	for (unsigned i = 0; i < this->memos.ignores.size(); ++i)
		data["memoignores"] << this->memos.ignores[i] << " ";
	Extensible::ExtensibleSerialize(this, this, data);
}

Serializable* NickCore::Unserialize(Serializable *obj, Serialize::Data &data)
{
	NickCore *nc;

	Anope::string sdisplay;

	data["display"] >> sdisplay;

	if (obj)
		nc = anope_dynamic_static_cast<NickCore *>(obj);
	else
		nc = new NickCore(sdisplay);

	data["pass"] >> nc->pass;
	data["email"] >> nc->email;
	data["language"] >> nc->language;
	{
		Anope::string buf;
		data["access"] >> buf;
		spacesepstream sep(buf);
		nc->access.clear();
		while (sep.GetToken(buf))
			nc->access.push_back(buf);
	}
	data["memomax"] >> nc->memos.memomax;
	{
		Anope::string buf;
		data["memoignores"] >> buf;
		spacesepstream sep(buf);
		nc->memos.ignores.clear();
		while (sep.GetToken(buf))
			nc->memos.ignores.push_back(buf);
	}

	Extensible::ExtensibleUnserialize(nc, nc, data);

	/* compat */
	bool b;
	b = false;
	data["extensible:SECURE"] >> b;
	if (b)
		nc->Extend<bool>("NS_SECURE");
	b = false;
	data["extensible:PRIVATE"] >> b;
	if (b)
		nc->Extend<bool>("NS_PRIVATE");
	b = false;
	data["extensible:AUTOOP"] >> b;
	if (b)
		nc->Extend<bool>("AUTOOP");
	b = false;
	data["extensible:HIDE_EMAIL"] >> b;
	if (b)
		nc->Extend<bool>("HIDE_EMAIL");
	b = false;
	data["extensible:HIDE_QUIT"] >> b;
	if (b)
		nc->Extend<bool>("HIDE_QUIT");
	b = false;
	data["extensible:MEMO_RECEIVE"] >> b;
	if (b)
		nc->Extend<bool>("MEMO_RECEIVE");
	b = false;
	data["extensible:MEMO_SIGNON"] >> b;
	if (b)
		nc->Extend<bool>("MEMO_SIGNON");
	b = false;
	data["extensible:KILLPROTECT"] >> b;
	if (b)
		nc->Extend<bool>("KILLPROTECT");
	/* end compat */

	return nc;
}

void NickCore::SetDisplay(const NickAlias *na)
{
	if (na->nc != this || na->nick == this->display)
		return;

	FOREACH_MOD(OnChangeCoreDisplay, (this, na->nick));

	/* this affects the serialized aliases */
	for (unsigned i = 0; i < aliases->size(); ++i)
		aliases->at(i)->QueueUpdate();

	/* Remove the core from the list */
	NickCoreList->erase(this->display);

	this->display = na->nick;

	(*NickCoreList)[this->display] = this;
}

bool NickCore::IsServicesOper() const
{
	return this->o != NULL;
}

void NickCore::AddAccess(const Anope::string &entry)
{
	this->access.push_back(entry);
	FOREACH_MOD(OnNickAddAccess, (this, entry));
}

Anope::string NickCore::GetAccess(unsigned entry) const
{
	if (this->access.empty() || entry >= this->access.size())
		return "";
	return this->access[entry];
}

unsigned NickCore::GetAccessCount() const
{
	return this->access.size();
}

bool NickCore::FindAccess(const Anope::string &entry)
{
	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
		if (this->access[i] == entry)
			return true;

	return false;
}

void NickCore::EraseAccess(const Anope::string &entry)
{
	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
		if (this->access[i] == entry)
		{
			FOREACH_MOD(OnNickEraseAccess, (this, entry));
			this->access.erase(this->access.begin() + i);
			break;
		}
}

void NickCore::ClearAccess()
{
	FOREACH_MOD(OnNickClearAccess, (this));
	this->access.clear();
}

bool NickCore::IsOnAccess(const User *u) const
{
	Anope::string buf = u->GetIdent() + "@" + u->host, buf2, buf3;
	if (!u->vhost.empty())
		buf2 = u->GetIdent() + "@" + u->vhost;
	if (!u->GetCloakedHost().empty())
		buf3 = u->GetIdent() + "@" + u->GetCloakedHost();

	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
	{
		Anope::string a = this->GetAccess(i);
		if (Anope::Match(buf, a) || (!buf2.empty() && Anope::Match(buf2, a)) || (!buf3.empty() && Anope::Match(buf3, a)))
			return true;
	}
	return false;
}

void NickCore::AddChannelReference(ChannelInfo *ci)
{
	++(*this->chanaccess)[ci];
}

void NickCore::RemoveChannelReference(ChannelInfo *ci)
{
	int& i = (*this->chanaccess)[ci];
	if (--i <= 0)
		this->chanaccess->erase(ci);
}

void NickCore::GetChannelReferences(std::deque<ChannelInfo *> &queue)
{
	queue.clear();
	for (std::map<ChannelInfo *, int>::iterator it = this->chanaccess->begin(), it_end = this->chanaccess->end(); it != it_end; ++it)
		queue.push_back(it->first);
}

NickCore* NickCore::Find(const Anope::string &nick)
{
	nickcore_map::const_iterator it = NickCoreList->find(nick);
	if (it != NickCoreList->end())
	{
		it->second->QueueUpdate();
		return it->second;
	}

	return NULL;
}

