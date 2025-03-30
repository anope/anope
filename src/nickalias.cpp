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

	if (!NickAliasList->insert_or_assign(this->nick, this).second)
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
		std::vector<NickAlias *>::iterator it = std::find(this->nc->aliases->begin(), this->nc->aliases->end(), this);
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
			if (this->nick.equals_ci(this->nc->display))
				this->nc->SetDisplay(this->nc->aliases->front());
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

NickAlias *NickAlias::Find(const Anope::string &nick)
{
	nickalias_map::const_iterator it = NickAliasList->find(nick);
	if (it != NickAliasList->end())
	{
		it->second->QueueUpdate();
		return it->second;
	}

	return NULL;
}

NickAlias *NickAlias::FindId(uint64_t id)
{
	const auto *nc = NickCore::FindId(id);
	return nc ? nc->na : nullptr;
}

NickAlias::Type::Type()
	: Serialize::Type(NICKALIAS_TYPE)
{
}

void NickAlias::Type::Serialize(const Serializable *obj, Serialize::Data &data) const
{
	const auto *na = static_cast<const NickAlias *>(obj);
	data.Store("nick", na->nick);
	data.Store("last_quit", na->last_quit);
	data.Store("last_realname", na->last_realname);
	data.Store("last_usermask", na->last_usermask);
	data.Store("last_realhost", na->last_realhost);
	data.Store("time_registered", na->time_registered);
	data.Store("last_seen", na->last_seen);
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
	Anope::string snc, snick;
	uint64_t sncid = 0;

	data["nc"] >> snc; // Deprecated 2.0 field
	data["ncid"] >> sncid;
	data["nick"] >> snick;

	auto *core = sncid ? NickCore::FindId(sncid) : NickCore::Find(snc);
	if (core == NULL)
		return NULL;

	NickAlias *na;
	if (obj)
		na = anope_dynamic_static_cast<NickAlias *>(obj);
	else
		na = new NickAlias(snick, core);

	if (na->nc != core)
	{
		std::vector<NickAlias *>::iterator it = std::find(na->nc->aliases->begin(), na->nc->aliases->end(), na);
		if (it != na->nc->aliases->end())
			na->nc->aliases->erase(it);

		if (na->nc->aliases->empty())
			delete na->nc;
		else if (na->nick.equals_ci(na->nc->display))
			na->nc->SetDisplay(na->nc->aliases->front());

		na->nc = core;
		core->aliases->push_back(na);
	}

	data["last_quit"] >> na->last_quit;
	data["last_realname"] >> na->last_realname;
	data["last_usermask"] >> na->last_usermask;
	data["last_realhost"] >> na->last_realhost;
	data["time_registered"] >> na->time_registered;
	data["last_seen"] >> na->last_seen;

	Anope::string vhost_ident, vhost_host, vhost_creator;
	time_t vhost_time;

	data["vhost_ident"] >> vhost_ident;
	data["vhost_host"] >> vhost_host;
	data["vhost_creator"] >> vhost_creator;
	data["vhost_time"] >> vhost_time;

	na->SetVHost(vhost_ident, vhost_host, vhost_creator, vhost_time);

	Extensible::ExtensibleUnserialize(na, na, data);

	/* compat */
	bool b;
	b = false;
	data["extensible:NO_EXPIRE"] >> b;
	if (b)
		na->Extend<bool>("NS_NO_EXPIRE");

	if (na->time_registered < na->nc->time_registered)
		na->nc->time_registered = na->time_registered;
	/* end compat */

	return na;
}
