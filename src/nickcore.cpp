// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "services.h"
#include "modules.h"
#include "account.h"
#include "config.h"
#include <climits>

Serialize::Checker<nickcore_map> NickCoreList(NICKCORE_TYPE);
Serialize::Checker<nickcoreid_map> NickCoreIdList(NICKCORE_TYPE);

NickCore::NickCore(const Anope::string &coredisplay, uint64_t coreid)
	: Serializable(NICKCORE_TYPE)
	, chanaccess(CHANNELINFO_TYPE)
	, uniqueid(coreid)
	, display(coredisplay)
	, aliases(NICKALIAS_TYPE)
{
	if (coredisplay.empty())
		throw CoreException("Empty display passed to NickCore constructor");

	if (!NickCoreList->insert_or_assign(this->display, this).second)
		Log(LOG_DEBUG) << "Duplicate account " << this->display << " in NickCore table";

	// Upgrading users may not have an account identifier.
	if (this->uniqueid && !NickCoreIdList->insert_or_assign(this->uniqueid, this).second)
		Log(LOG_DEBUG) << "Duplicate account id " << this->uniqueid << " in NickCore table";

	FOREACH_MOD(OnNickCoreCreate, (this));
}

NickCore::~NickCore()
{
	FOREACH_MOD(OnDelCore, (this));

	UnsetExtensibles();

	if (!this->chanaccess->empty())
		Log(LOG_DEBUG) << "Non-empty chanaccess list in destructor!";

	for (auto it = this->users.begin(); it != this->users.end();)
	{
		User *user = *it++;
		user->Logout();
	}
	this->users.clear();

	NickCoreList->erase(this->display);
	if (this->uniqueid)
		NickCoreIdList->erase(this->uniqueid);

	if (!this->memos.memos->empty())
	{
		for (unsigned i = 0, end = this->memos.memos->size(); i < end; ++i)
			delete this->memos.GetMemo(i);
		this->memos.memos->clear();
	}
}

NickCore::Type::Type()
	: Serialize::Type(NICKCORE_TYPE)
{
}

void NickCore::Type::Serialize(Serializable *obj, Serialize::Data &data) const
{
	auto *nc = static_cast<NickCore *>(obj);
	data.Store("display", nc->display);
	data.Store("uniqueid", nc->GetId());
	data.Store("pass", nc->pass);
	data.Store("email", nc->email);
	data.Store("language", nc->language);
	data.Store<time_t>("lastmail", nc->lastmail);
	data.Store<time_t>("registered", nc->registered);
	data.Store<int16_t>("memomax", nc->memos.memomax);

	std::ostringstream oss;
	for (const auto &ignore : nc->memos.ignores)
		oss << ignore << " ";
	data.Store("memoignores", oss.str());

	Extensible::ExtensibleSerialize(nc, nc, data);
}

Serializable *NickCore::Type::Unserialize(Serializable *obj, Serialize::Data &data) const
{
	NickCore *nc;
	if (obj)
		nc = anope_dynamic_static_cast<NickCore *>(obj);
	else
		nc = new NickCore(data.Load("display"), data.Load<uint64_t>("uniqueid"));

	nc->pass = data.Load("pass");
	nc->email = data.Load("email");
	nc->language = data.Load("language");
	nc->lastmail = data.Load<time_t>("lastmail");
	nc->registered = data.Load<time_t>("registered", data.Load<time_t>("time_registered"));
	nc->memos.memomax = data.Load<int16_t>("memomax");
	{
		nc->memos.ignores.clear();
		spacesepstream sep(data.Load("memoignores"));
		for (Anope::string buf; sep.GetToken(buf); )
			nc->memos.ignores.insert(buf);
	}

	Extensible::ExtensibleUnserialize(nc, nc, data);

	// Begin 1.9 compatibility.
	if (data.Load<bool>("extensible:PRIVATE"))
		nc->Extend<bool>("NS_PRIVATE");
	if (data.Load<bool>("extensible:AUTOOP"))
		nc->Extend<bool>("AUTOOP");
	if (data.Load<bool>("extensible:HIDE_EMAIL"))
		nc->Extend<bool>("HIDE_EMAIL");
	if (data.Load<bool>("extensible:HIDE_QUIT"))
		nc->Extend<bool>("HIDE_QUIT");
	if (data.Load<bool>("extensible:MEMO_RECEIVE"))
		nc->Extend<bool>("MEMO_RECEIVE");
	if (data.Load<bool>("extensible:MEMO_SIGNON"))
		nc->Extend<bool>("MEMO_SIGNON");
	if (data.Load<bool>("extensible:KILLPROTECT"))
		nc->Extend<bool>("PROTECT");
	// End 1.9 compatibility

	// Begin 2.0 compatibility.
	if (data.Load<bool>("KILLPROTECT"))
	{
		nc->Extend<bool>("PROTECT");
		nc->Extend("PROTECT_AFTER", Config->GetModule("nickserv").Get<time_t>("kill", "60s"));
	}
	if (data.Load<bool>("KILL_QUICK"))
	{
		nc->Extend<bool>("PROTECT");
		nc->Extend("PROTECT_AFTER", Config->GetModule("nickserv").Get<time_t>("killquick", "20s"));
	}
	if (data.Load<bool>("KILL_IMMED"))
	{
		nc->Extend<bool>("PROTECT");
		nc->Extend("PROTECT_AFTER", 0);
	}
	// End 2.0 compatibility.

	return nc;
}

void NickCore::SetDisplay(NickAlias *na)
{
	// If no nick is specified then pick the oldest one.
	if (!na)
	{
		for (auto *alias : *this->aliases)
		{
			if (!na || alias->registered < na->registered)
				na = alias;
		}
	}

	if (!na || na->nc != this || na->nick == this->display)
		return;

	FOREACH_MOD(OnChangeCoreDisplay, (this, na->nick));

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
	auto it = NickCoreList->find(nick);
	if (it != NickCoreList->end())
	{
		it->second->QueueUpdate();
		return it->second;
	}

	return NULL;
}

NickCore *NickCore::FindId(uint64_t uid)
{
	auto it = NickCoreIdList->find(uid);
	if (it != NickCoreIdList->end())
	{
		it->second->QueueUpdate();
		return it->second;
	}
	return nullptr;
}

uint64_t NickCore::GetId()
{
	if (this->uniqueid)
		return this->uniqueid;

	// We base the account identifier on the account display at registration and
	// when the account was first registered. This should be unique enough that
	// it never collides. In the extremely rare case that it does generate a
	// duplicate id we try with a new suffix.
	uint64_t attempt = 0;
	while (!this->uniqueid)
	{
		const auto newidstr = this->display + "\0" + Anope::ToString(this->registered) + "\0" + Anope::ToString(attempt++);
		const auto newid = Anope::hash_cs()(newidstr);
		if (NickCoreIdList->emplace(newid, this).second)
		{
			this->uniqueid = newid;
			this->QueueUpdate();
			break;
		}
	}

	return this->uniqueid;
}
