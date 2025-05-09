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

#include "service.h"
#include "access.h"
#include "regchannel.h"
#include "users.h"
#include "account.h"
#include "protocol.h"

Privilege::Privilege(const Anope::string &n, const Anope::string &d, int r) : name(n), desc(d), rank(r)
{
}

bool Privilege::operator==(const Privilege &other) const
{
	return this->name.equals_ci(other.name);
}

std::vector<Privilege> PrivilegeManager::Privileges;

void PrivilegeManager::AddPrivilege(Privilege p)
{
	unsigned i;
	for (i = 0; i < Privileges.size(); ++i)
	{
		Privilege &priv = Privileges[i];

		if (priv.rank > p.rank)
			break;
	}

	Privileges.insert(Privileges.begin() + i, p);
}

void PrivilegeManager::RemovePrivilege(Privilege &p)
{
	std::vector<Privilege>::iterator it = std::find(Privileges.begin(), Privileges.end(), p);
	if (it != Privileges.end())
		Privileges.erase(it);

	for (const auto &[_, ci] : *RegisteredChannelList)
	{
		ci->QueueUpdate();
		ci->RemoveLevel(p.name);
	}
}

Privilege *PrivilegeManager::FindPrivilege(const Anope::string &name)
{
	for (unsigned i = Privileges.size(); i > 0; --i)
		if (Privileges[i - 1].name.equals_ci(name))
			return &Privileges[i - 1];
	return NULL;
}

std::vector<Privilege> &PrivilegeManager::GetPrivileges()
{
	return Privileges;
}

void PrivilegeManager::ClearPrivileges()
{
	Privileges.clear();
}

AccessProvider::AccessProvider(Module *o, const Anope::string &n) : Service(o, "AccessProvider", n)
{
	Providers.push_back(this);
}

AccessProvider::~AccessProvider()
{
	std::list<AccessProvider *>::iterator it = std::find(Providers.begin(), Providers.end(), this);
	if (it != Providers.end())
		Providers.erase(it);
}

std::list<AccessProvider *> AccessProvider::Providers;

const std::list<AccessProvider *>& AccessProvider::GetProviders()
{
	return Providers;
}

ChanAccess::ChanAccess(AccessProvider *p)
	: Serializable(CHANACCESS_TYPE)
	, provider(p)
{
}

ChanAccess::~ChanAccess()
{
	if (this->ci)
	{
		std::vector<ChanAccess *>::iterator it = std::find(this->ci->access->begin(), this->ci->access->end(), this);
		if (it != this->ci->access->end())
			this->ci->access->erase(it);

		if (*nc != NULL)
			nc->RemoveChannelReference(this->ci);
		else
		{
			ChannelInfo *c = ChannelInfo::Find(this->mask);
			if (c)
				c->RemoveChannelReference(this->ci->name);
		}
	}
}

void ChanAccess::SetMask(const Anope::string &m, ChannelInfo *c)
{
	if (*nc != NULL)
		nc->RemoveChannelReference(this->ci);
	else if (!this->mask.empty())
	{
		ChannelInfo *targc = ChannelInfo::Find(this->mask);
		if (targc)
			targc->RemoveChannelReference(this->ci->name);
	}

	ci = c;
	mask.clear();
	nc = NULL;

	const NickAlias *na = NickAlias::Find(m);
	if (na != NULL)
	{
		nc = na->nc;
		nc->AddChannelReference(ci);
	}
	else
	{
		mask = m;

		ChannelInfo *targci = ChannelInfo::Find(mask);
		if (targci != NULL)
			targci->AddChannelReference(ci->name);
	}
}

const Anope::string &ChanAccess::Mask() const
{
	if (nc)
		return nc->display;
	else
		return mask;
}

NickCore *ChanAccess::GetAccount() const
{
	return nc;
}


ChanAccess::Type::Type()
	: Serialize::Type(CHANACCESS_TYPE)
{
}

void ChanAccess::Type::Serialize(const Serializable *obj, Serialize::Data &data) const
{
	const auto *access = static_cast<const ChanAccess *>(obj);
	data.Store("provider", access->provider->name);
	data.Store("ci", access->ci->name);
	data.Store("mask", access->Mask());
	data.Store("creator", access->creator);
	data.Store("description", access->description);
	data.Store("last_seen", access->last_seen);
	data.Store("created", access->created);
	data.Store("data", access->AccessSerialize());
}

Serializable *ChanAccess::Type::Unserialize(Serializable *obj, Serialize::Data &data) const
{
	Anope::string provider, chan;

	data["provider"] >> provider;
	data["ci"] >> chan;

	ServiceReference<AccessProvider> aprovider("AccessProvider", provider);
	ChannelInfo *ci = ChannelInfo::Find(chan);
	if (!aprovider || !ci)
		return NULL;

	ChanAccess *access;
	if (obj)
		access = anope_dynamic_static_cast<ChanAccess *>(obj);
	else
		access = aprovider->Create();
	access->ci = ci;
	Anope::string m;
	data["mask"] >> m;
	access->SetMask(m, ci);
	data["creator"] >> access->creator;
	data["description"] >> access->description;
	data["last_seen"] >> access->last_seen;
	data["created"] >> access->created;

	Anope::string adata;
	data["data"] >> adata;
	access->AccessUnserialize(adata);

	if (!obj)
		ci->AddAccess(access);
	return access;
}

bool ChanAccess::Matches(const User *u, const NickCore *acc, ChannelInfo *&next) const
{
	next = NULL;

	if (this->nc)
		return this->nc == acc;

	if (u)
	{
		bool is_mask = this->mask.find_first_of("!@?*") != Anope::string::npos;
		if (is_mask && Anope::Match(u->nick, this->mask))
			return true;
		else if (Anope::Match(u->GetDisplayedMask(), this->mask))
			return true;
	}

	if (acc)
	{
		for (auto *na : *acc->aliases)
		{
			if (Anope::Match(na->nick, this->mask))
				return true;
		}
	}

	if (IRCD->IsChannelValid(this->mask))
	{
		next = ChannelInfo::Find(this->mask);
	}

	return false;
}

bool ChanAccess::operator>(const ChanAccess &other) const
{
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other.HasPriv(privs[i - 1].name);

		if (!this_p && !other_p)
			continue;

		return this_p && !other_p;
	}

	return false;
}

bool ChanAccess::operator<(const ChanAccess &other) const
{
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other.HasPriv(privs[i - 1].name);

		if (!this_p && !other_p)
			continue;

		return !this_p && other_p;
	}

	return false;
}

bool ChanAccess::operator>=(const ChanAccess &other) const
{
	return !(*this < other);
}

bool ChanAccess::operator<=(const ChanAccess &other) const
{
	return !(*this > other);
}

AccessGroup::AccessGroup()
{
	this->ci = NULL;
	this->nc = NULL;
	this->super_admin = this->founder = false;
}

static bool HasPriv(const ChanAccess::Path &path, const Anope::string &name)
{
	if (path.empty())
		return false;

	for (auto *access : path)
	{
		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnCheckPriv, MOD_RESULT, (access, name));

		if (MOD_RESULT != EVENT_ALLOW && !access->HasPriv(name))
			return false;
	}

	return true;
}

bool AccessGroup::HasPriv(const Anope::string &name) const
{
	if (this->super_admin)
		return true;
	else if (!ci || ci->GetLevel(name) == ACCESS_INVALID)
		return false;

	/* Privileges prefixed with auto are understood to be given
	 * automatically. Sometimes founders want to not automatically
	 * obtain privileges, so we will let them */
	bool auto_mode = !name.find("AUTO");

	/* Only grant founder privilege if this isn't an auto mode or if they don't match any entries in this group */
	if ((!auto_mode || paths.empty()) && this->founder)
		return true;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(OnGroupCheckPriv, MOD_RESULT, (this, name));
	if (MOD_RESULT != EVENT_CONTINUE)
		return MOD_RESULT == EVENT_ALLOW;

	for (unsigned int i = paths.size(); i > 0; --i)
	{
		const ChanAccess::Path &path = paths[i - 1];

		if (::HasPriv(path, name))
			return true;
	}

	return false;
}

static ChanAccess *HighestInPath(const ChanAccess::Path &path)
{
	ChanAccess *highest = NULL;

	for (auto *ca : path)
		if (highest == NULL || *ca > *highest)
			highest = ca;

	return highest;
}

const ChanAccess *AccessGroup::Highest() const
{
	ChanAccess *highest = NULL;

	for (const auto &path : paths)
	{
		ChanAccess *hip = HighestInPath(path);

		if (highest == NULL || *hip > *highest)
			highest = hip;
	}

	return highest;
}

bool AccessGroup::operator>(const AccessGroup &other) const
{
	if (other.super_admin)
		return false;
	else if (this->super_admin)
		return true;
	else if (other.founder)
		return false;
	else if (this->founder)
		return true;

	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other.HasPriv(privs[i - 1].name);

		if (!this_p && !other_p)
			continue;

		return this_p && !other_p;
	}

	return false;
}

bool AccessGroup::operator<(const AccessGroup &other) const
{
	if (this->super_admin)
		return false;
	else if (other.super_admin)
		return true;
	else if (this->founder)
		return false;
	else if (other.founder)
		return true;

	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other.HasPriv(privs[i - 1].name);

		if (!this_p && !other_p)
			continue;

		return !this_p && other_p;
	}

	return false;
}

bool AccessGroup::operator>=(const AccessGroup &other) const
{
	return !(*this < other);
}

bool AccessGroup::operator<=(const AccessGroup &other) const
{
	return !(*this > other);
}
