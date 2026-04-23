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
#include "account.h"
#include "modules.h"
#include "opertype.h"
#include "protocol.h"
#include "users.h"
#include "servers.h"
#include "config.h"

Serialize::Checker<nickalias_map> NickAliasList(NICKALIAS_TYPE);

NickAlias::NickAlias(const Anope::string &nickname, NickCore *nickcore)
	: Serializable(NICKALIAS_TYPE)
	, nick(nickname)
	, nc(nickcore)
{
	if (nickname.empty())
		throw CoreException("Empty nick passed to NickAlias constructor");
	else if (!nickcore)
		throw CoreException("Empty nickcore passed to NickAlias constructor");

	nickcore->aliases->push_back(this);
	if (this->nick.equals_ci(nickcore->display))
		nickcore->na = this;

	if (!this->InsertUnique(*NickAliasList, this->nick))
		Log(LOG_DEBUG) << "Duplicate nick " << this->nick << " in NickAlias table";

	if (this->nc->o == NULL)
	{
		Oper *o = Oper::Find(this->nick);
		if (o == NULL)
			o = Oper::Find(this->nc->display);
		nickcore->o = o;
		if (this->nc->o != NULL)
			Log() << "Tied oper " << this->nc->display << " to type " << this->nc->o->ot->GetName();
	}
}

NickAlias::~NickAlias()
{
	FOREACH_MOD(OnDelNick, (this));

	UnsetExtensibles();

	/* Accept nicks that have no core, because of database load functions */
	if (this->nc)
	{
		/* Next: see if our core is still useful. */
		auto it = std::find(this->nc->aliases->begin(), this->nc->aliases->end(), this);
		if (it != this->nc->aliases->end())
			this->nc->aliases->erase(it);
		if (this->nc->aliases->empty())
		{
			delete this->nc;
			this->nc = NULL;
		}
		else
		{
			/* Display updating stuff */
			if (this->nc->na == this)
				this->nc->SetDisplay(nullptr);
		}
	}

	/* Remove us from the aliases list */
	NickAliasList->erase(this->nick);
}

void NickAlias::SetVHost(const Anope::string &ident, const Anope::string &host, const Anope::string &creator, time_t created)
{
	this->vhost_ident = ident;
	this->vhost_host = host;
	this->vhost_creator = creator;
	this->vhost_created = created;
}

void NickAlias::RemoveVHost()
{
	this->vhost_ident.clear();
	this->vhost_host.clear();
	this->vhost_creator.clear();
	this->vhost_created = 0;
}

bool NickAlias::HasVHost() const
{
	return !this->vhost_host.empty();
}

const Anope::string &NickAlias::GetVHostIdent() const
{
	return this->vhost_ident;
}

const Anope::string &NickAlias::GetVHostHost() const
{
	return this->vhost_host;
}

Anope::string NickAlias::GetVHostMask() const
{
	if (this->GetVHostIdent().empty())
		return this->GetVHostHost();

	return this->GetVHostIdent() + "@" + this->GetVHostHost();
}

const Anope::string &NickAlias::GetVHostCreator() const
{
	return this->vhost_creator;
}

time_t NickAlias::GetVHostCreated() const
{
	return this->vhost_created;
}

void NickAlias::UpdateSeen(User *u)
{
	this->last_seen = Anope::CurTime;
	this->last_userhost = u->GetIdent() + "@" + u->GetDisplayedHost();
	this->last_userhost_real = u->GetIdent() + "@" + u->host;
}

NickAlias *NickAlias::Find(const Anope::string &nick)
{
	auto it = NickAliasList->find(nick);
	if (it != NickAliasList->end())
	{
		it->second->QueueUpdate();
		return it->second;
	}

	return NULL;
}

NickAlias *NickAlias::FindId(uint64_t uid)
{
	const auto *nc = NickCore::FindId(uid);
	return nc ? nc->na : nullptr;
}

NickAlias::Type::Type()
	: Serialize::Type(NICKALIAS_TYPE)
{
}

void NickAlias::Type::Serialize(Serializable *obj, Serialize::Data &data) const
{
	const auto *na = static_cast<const NickAlias *>(obj);
	data.Store("nick", na->nick);
	data.Store("last_quit", na->last_quit);
	data.Store("last_userhost", na->last_userhost);
	data.Store("last_userhost_real", na->last_userhost_real);
	data.Store<time_t>("registered", na->registered);
	data.Store<time_t>("last_seen", na->last_seen);
	data.Store("ncid", na->nc->GetId());

	if (na->HasVHost())
	{
		data.Store("vhost_ident", na->GetVHostIdent());
		data.Store("vhost_host", na->GetVHostHost());
		data.Store("vhost_creator", na->GetVHostCreator());
		data.Store("vhost_time", na->GetVHostCreated());
	}

	Extensible::ExtensibleSerialize(na, na, data);
}

Serializable *NickAlias::Type::Unserialize(Serializable *obj, Serialize::Data &data) const
{
	const auto sncid = data.Load<uint64_t>("ncid");

	auto *core = sncid ? NickCore::FindId(sncid) : NickCore::Find(data.Load("nc"));
	if (core == NULL)
		return NULL;

	NickAlias *na;
	if (obj)
		na = anope_dynamic_static_cast<NickAlias *>(obj);
	else
		na = new NickAlias(data.Load("nick"), core);

	if (na->nc != core)
	{
		auto it = std::find(na->nc->aliases->begin(), na->nc->aliases->end(), na);
		if (it != na->nc->aliases->end())
			na->nc->aliases->erase(it);

		if (na->nc->aliases->empty())
			delete na->nc;
		if (na->nc->na == na)
			na->nc->SetDisplay(nullptr);

		na->nc = core;
		core->aliases->push_back(na);
	}

	na->last_quit = data.Load("last_quit");
	na->last_userhost = data.Load("last_userhost", data.Load("last_usermask"));
	na->last_userhost_real = data.Load("last_userhost_real", data.Load("last_realhost"));
	na->registered = data.Load<time_t>("registered", data.Load<time_t>("time_registered"));
	na->last_seen = data.Load<time_t>("last_seen");

	na->SetVHost(data.Load("vhost_ident"), data.Load("vhost_host"), data.Load("vhost_creator"),
		data.Load<time_t>("vhost_time"));

	Extensible::ExtensibleUnserialize(na, na, data);

	// Begin 1.9 compatibility.
	if (data.Load<bool>("extensible:NO_EXPIRE"))
		na->Extend<bool>("NS_NO_EXPIRE");
	// End 1.9 compatibility.

	// Begin 2.0 compatibility.
	if (na->registered < na->nc->registered)
		na->nc->registered = na->registered;
	// End 2.0 compatibility.

	return na;
}
