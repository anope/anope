/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "account.h"
#include "config.h"
#include <climits>

Serialize::Checker<nickcore_map> NickCoreList("NickCore");
Serialize::Checker<nickcoreid_map> NickCoreIdList("NickCore");

NickCore::NickCore(const Anope::string &coredisplay, uint64_t coreid)
	: Serializable("NickCore")
	, chanaccess("ChannelInfo")
	, id(coreid)
	, display(coredisplay)
	, aliases("NickAlias")
{
	if (coredisplay.empty())
		throw CoreException("Empty display passed to NickCore constructor");

	if (!NickCoreList->insert_or_assign(this->display, this).second)
		Log(LOG_DEBUG) << "Duplicate account " << this->display << " in NickCore table";

	// Upgrading users may not have an account identifier.
	if (this->id && !NickCoreIdList->insert_or_assign(this->id, this).second)
		Log(LOG_DEBUG) << "Duplicate account id " << this->id << " in NickCore table";

	FOREACH_MOD(OnNickCoreCreate, (this));
}

NickCore::~NickCore()
{
	FOREACH_MOD(OnDelCore, (this));

	UnsetExtensibles();

	if (!this->chanaccess->empty())
		Log(LOG_DEBUG) << "Non-empty chanaccess list in destructor!";

	for (std::list<User *>::iterator it = this->users.begin(); it != this->users.end();)
	{
		User *user = *it++;
		user->Logout();
	}
	this->users.clear();

	NickCoreList->erase(this->display);
	if (this->id)
		NickCoreIdList->erase(this->id);

	if (!this->memos.memos->empty())
	{
		for (unsigned i = 0, end = this->memos.memos->size(); i < end; ++i)
			delete this->memos.GetMemo(i);
		this->memos.memos->clear();
	}
}

void NickCore::Serialize(Serialize::Data &data) const
{
	data.Store("display", this->display);
	data.Store("uniqueid", this->id);
	data.Store("pass", this->pass);
	data.Store("email", this->email);
	data.Store("language", this->language);
	data.Store("lastmail", this->lastmail);
	data.Store("time_registered", this->time_registered);
	data.Store("memomax", this->memos.memomax);

	std::ostringstream oss;
	for (const auto &ignore : this->memos.ignores)
		oss << ignore << " ";
	data.Store("memoignores", oss.str());

	Extensible::ExtensibleSerialize(this, this, data);
}

Serializable *NickCore::Unserialize(Serializable *obj, Serialize::Data &data)
{
	NickCore *nc;

	Anope::string sdisplay;
	data["display"] >> sdisplay;

	uint64_t sid = 0;
	data["uniqueid"] >> sid;

	if (obj)
		nc = anope_dynamic_static_cast<NickCore *>(obj);
	else
		nc = new NickCore(sdisplay, sid);

	data["pass"] >> nc->pass;
	data["email"] >> nc->email;
	data["language"] >> nc->language;
	data["lastmail"] >> nc->lastmail;
	data["time_registered"] >> nc->time_registered;
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
		nc->Extend<bool>("PROTECT");

	b = false;
	data["KILLPROTECT"] >> b;
	if (b)
	{
		nc->Extend<bool>("PROTECT");
		nc->Extend("PROTECT_AFTER", Config->GetModule("nickserv").Get<time_t>("kill", "60s"));
	}
	b = false;
	data["KILL_QUICK"] >> b;
	if (b)
	{
		nc->Extend<bool>("PROTECT");
		nc->Extend("PROTECT_AFTER", Config->GetModule("nickserv").Get<time_t>("killquick", "20s"));
	}
	b = false;
	data["KILL_IMMED"] >> b;
	if (b)
	{
		nc->Extend<bool>("PROTECT");
		nc->Extend("PROTECT_AFTER", 0);
	}
	/* end compat */

	return nc;
}

void NickCore::SetDisplay(NickAlias *na)
{
	if (na->nc != this || na->nick == this->display)
		return;

	FOREACH_MOD(OnChangeCoreDisplay, (this, na->nick));

	/* this affects the serialized aliases */
	for (auto *alias : *aliases)
		alias->QueueUpdate();

	/* Remove the core from the list */
	NickCoreList->erase(this->display);

	this->display = na->nick;
	this->na = na;

	(*NickCoreList)[this->display] = this;
}

bool NickCore::IsServicesOper() const
{
	return this->o != NULL;
}

void NickCore::AddChannelReference(ChannelInfo *ci)
{
	++(*this->chanaccess)[ci];
}

void NickCore::RemoveChannelReference(ChannelInfo *ci)
{
	int &i = (*this->chanaccess)[ci];
	if (--i <= 0)
		this->chanaccess->erase(ci);
}

void NickCore::GetChannelReferences(std::deque<ChannelInfo *> &queue)
{
	queue.clear();
	for (const auto &[ci, _] : *this->chanaccess)
		queue.push_back(ci);
}

NickCore *NickCore::Find(const Anope::string &nick)
{
	nickcore_map::const_iterator it = NickCoreList->find(nick);
	if (it != NickCoreList->end())
	{
		it->second->QueueUpdate();
		return it->second;
	}

	return NULL;
}

uint64_t NickCore::GetId()
{
	if (this->id)
		return this->id;

	// We base the account identifier on the account display at registration and
	// when the account was first registered. This should be unique enough that
	// it never collides. In the extremely rare case that it does generate a
	// duplicate id we try with a new suffix.
	uint64_t attempt = 0;
	while (!this->id)
	{
		const auto newidstr = this->display + "\0" + Anope::ToString(this->time_registered) + "\0" + Anope::ToString(attempt++);
		const auto newid = Anope::hash_cs()(newidstr);
		if (NickCoreIdList->emplace(newid, this).second)
		{
			this->id = newid;
			this->QueueUpdate();
			break;
		}
	}

	return this->id;
}
