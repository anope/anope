/*
 *
 * (C) 2003-2013 Anope Team
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

NickCore::NickCore(const Anope::string &coredisplay) : Serializable("NickCore"), aliases("NickAlias")
{
	if (coredisplay.empty())
		throw CoreException("Empty display passed to NickCore constructor");

	this->o = NULL;
	this->channelcount = 0;
	this->lastmail = 0;
	this->memos.memomax = Config->MSMaxMemos;
	this->language = Config->NSDefLanguage;

	this->display = coredisplay;

	/* Set default nick core flags */
	for (std::set<Anope::string>::const_iterator it = Config->NSDefFlags.begin(), it_end = Config->NSDefFlags.end(); it != it_end; ++it)
		this->ExtendMetadata(*it);

	size_t old = NickCoreList->size();
	(*NickCoreList)[this->display] = this;
	if (old == NickCoreList->size())
		Log(LOG_DEBUG) << "Duplicate account " << coredisplay << " in nickcore table?";
}

NickCore::~NickCore()
{
	FOREACH_MOD(I_OnDelCore, OnDelCore(this));

	for (std::list<User *>::iterator it = this->users.begin(); it != this->users.end();)
	{
		User *user = *it++;
		user->Logout();
	}
	this->users.clear();

	/* Remove the core from the list */
	NickCoreList->erase(this->display);

	/* Clear access before deleting display name, we want to be able to use the display name in the clear access event */
	this->ClearAccess();

	if (!this->memos.memos->empty())
	{
		for (unsigned i = 0, end = this->memos.memos->size(); i < end; ++i)
			this->memos.GetMemo(i)->Destroy();
		this->memos.memos->clear();
	}
}

void NickCore::Serialize(Serialize::Data &data) const
{
	data["display"] << this->display;
	data["pass"] << this->pass;
	data["email"] << this->email;
	data["greet"] << this->greet;
	data["language"] << this->language;
	this->ExtensibleSerialize(data);
	for (unsigned i = 0; i < this->access.size(); ++i)
		data["access"] << this->access[i] << " ";
	for (unsigned i = 0; i < this->cert.size(); ++i)
		data["cert"] << this->cert[i] << " ";
	data["memomax"] << this->memos.memomax;
	for (unsigned i = 0; i < this->memos.ignores.size(); ++i)
		data["memoignores"] << this->memos.ignores[i] << " ";
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
	data["greet"] >> nc->greet;
	data["language"] >> nc->language;
	nc->ExtensibleUnserialize(data);
	{
		Anope::string buf;
		data["access"] >> buf;
		spacesepstream sep(buf);
		nc->access.clear();
		while (sep.GetToken(buf))
			nc->access.push_back(buf);
	}
	{
		Anope::string buf;
		data["cert"] >> buf;
		spacesepstream sep(buf);
		nc->cert.clear();
		while (sep.GetToken(buf))
			nc->cert.push_back(buf);
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

	/* Compat */
	Anope::string sflags;
	data["flags"] >> sflags;
	spacesepstream sep(sflags);
	Anope::string tok;
	while (sep.GetToken(tok))
		nc->ExtendMetadata(tok);
	/* End compat */

	return nc;
}

void NickCore::SetDisplay(const NickAlias *na)
{
	if (na->nc != this || na->nick == this->display)
		return;

	FOREACH_MOD(I_OnChangeCoreDisplay, OnChangeCoreDisplay(this, na->nick));

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
	FOREACH_MOD(I_OnNickAddAccess, OnNickAddAccess(this, entry));
}

Anope::string NickCore::GetAccess(unsigned entry) const
{
	if (this->access.empty() || entry >= this->access.size())
		return "";
	return this->access[entry];
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
			FOREACH_MOD(I_OnNickEraseAccess, OnNickEraseAccess(this, entry));
			this->access.erase(this->access.begin() + i);
			break;
		}
}

void NickCore::ClearAccess()
{
	FOREACH_MOD(I_OnNickClearAccess, OnNickClearAccess(this));
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

void NickCore::AddCert(const Anope::string &entry)
{
	this->cert.push_back(entry);
	FOREACH_MOD(I_OnNickAddCert, OnNickAddCert(this, entry));
}

Anope::string NickCore::GetCert(unsigned entry) const
{
	if (this->cert.empty() || entry >= this->cert.size())
		return "";
	return this->cert[entry];
}

bool NickCore::FindCert(const Anope::string &entry) const
{
	for (unsigned i = 0, end = this->cert.size(); i < end; ++i)
		if (this->cert[i] == entry)
			return true;

	return false;
}

void NickCore::EraseCert(const Anope::string &entry)
{
	for (unsigned i = 0, end = this->cert.size(); i < end; ++i)
		if (this->cert[i] == entry)
		{
			FOREACH_MOD(I_OnNickEraseCert, OnNickEraseCert(this, entry));
			this->cert.erase(this->cert.begin() + i);
			break;
		}
}

void NickCore::ClearCert()
{
	FOREACH_MOD(I_OnNickClearCert, OnNickClearCert(this));
	this->cert.clear();
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

