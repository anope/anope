/*
 *
 * (C) 2003-2012 Anope Team
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

Privilege::Privilege(const Anope::string &n, const Anope::string &d, int r) : name(n), desc(d), rank(r)
{
}

bool Privilege::operator==(const Privilege &other) const
{
	return this->name == other.name;
}

std::vector<Privilege> PrivilegeManager::privs;

void PrivilegeManager::AddPrivilege(Privilege p)
{
	unsigned i;
	for (i = 0; i < privs.size(); ++i)
	{
		Privilege &priv = privs[i];

		if (priv.rank > p.rank)
			break;
	}
	
	privs.insert(privs.begin() + i, p);
}

void PrivilegeManager::RemovePrivilege(Privilege &p)
{
	std::vector<Privilege>::iterator it = std::find(privs.begin(), privs.end(), p);
	if (it != privs.end())
		privs.erase(it);

	for (registered_channel_map::const_iterator cit = RegisteredChannelList->begin(), cit_end = RegisteredChannelList->end(); cit != cit_end; ++cit)
	{
		cit->second->QueueUpdate();
		cit->second->RemoveLevel(p.name);
	}
}

Privilege *PrivilegeManager::FindPrivilege(const Anope::string &name)
{
	for (unsigned i = privs.size(); i > 0; --i)
		if (privs[i - 1].name.equals_ci(name))
			return &privs[i - 1];
	return NULL;
}

std::vector<Privilege> &PrivilegeManager::GetPrivileges()
{
	return privs;
}

void PrivilegeManager::ClearPrivileges()
{
	privs.clear();
}

AccessProvider::AccessProvider(Module *o, const Anope::string &n) : Service(o, "AccessProvider", n)
{
}

AccessProvider::~AccessProvider()
{
}

ChanAccess::ChanAccess(AccessProvider *p) : provider(p)
{
}

ChanAccess::~ChanAccess()
{
}

const Anope::string ChanAccess::serialize_name() const
{
	return "ChanAccess";
}

Serialize::Data ChanAccess::serialize() const
{
	Serialize::Data data;

	data["provider"] << this->provider->name;
	data["ci"] << this->ci->name;
	data["mask"] << this->mask;
	data["creator"] << this->creator;
	data["last_seen"].setType(Serialize::DT_INT) << this->last_seen;
	data["created"].setType(Serialize::DT_INT) << this->created;
	data["data"] << this->Serialize();

	return data;
}

Serializable* ChanAccess::unserialize(Serializable *obj, Serialize::Data &data)
{
	service_reference<AccessProvider> aprovider("AccessProvider", data["provider"].astr());
	ChannelInfo *ci = cs_findchan(data["ci"].astr());
	if (!aprovider || !ci)
		return NULL;

	ChanAccess *access;
	if (obj)
		access = anope_dynamic_static_cast<ChanAccess *>(obj);
	else
		access = aprovider->Create();
	access->ci = ci;
	data["mask"] >> access->mask;
	data["creator"] >> access->creator;
	data["last_seen"] >> access->last_seen;
	data["created"] >> access->created;
	access->Unserialize(data["data"].astr());

	if (!obj)
		ci->AddAccess(access);
	return access;
}

bool ChanAccess::Matches(const User *u, const NickCore *nc) const
{
	bool is_mask = this->mask.find_first_of("!@?*") != Anope::string::npos;
	if (u && is_mask && Anope::Match(u->nick, this->mask))
		return true;
	else if (u && Anope::Match(u->GetDisplayedMask(), this->mask))
		return true;
	else if (nc)
		for (std::list<serialize_obj<NickAlias> >::const_iterator it = nc->aliases.begin(); it != nc->aliases.end();)
		{
			const NickAlias *na = *it++;
			if (na && Anope::Match(na->nick, this->mask))
				return true;
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
		else if (this_p && !other_p)
			return true;
		else
			return false;
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
		else if (!this_p && other_p)
			return true;
		else
			return false;
	}	

	return false;
}

bool ChanAccess::operator>=(const ChanAccess &other) const
{
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other.HasPriv(privs[i - 1].name);

		if (!this_p && !other_p)
			continue;
		else if (!this_p && other_p)
			return false;
		else
			return true;
	}

	return true;
}

bool ChanAccess::operator<=(const ChanAccess &other) const
{
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other.HasPriv(privs[i - 1].name);

		if (!this_p && !other_p)
			continue;
		else if (this_p && !other_p)
			return false;
		else
			return true;
	}

	return true;
}

AccessGroup::AccessGroup() : std::vector<ChanAccess *>()
{
	this->ci = NULL;
	this->nc = NULL;
	this->SuperAdmin = this->Founder = false;
}

bool AccessGroup::HasPriv(const Anope::string &name) const
{
	if (this->SuperAdmin)
		return true;
	else if (ci->GetLevel(name) == ACCESS_INVALID)
		return false;
	else if (this->Founder)
		return true;
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnGroupCheckPriv, OnGroupCheckPriv(this, name));
	if (MOD_RESULT != EVENT_CONTINUE)
		return MOD_RESULT == EVENT_ALLOW;
	for (unsigned i = this->size(); i > 0; --i)
	{
		ChanAccess *access = this->at(i - 1);
		FOREACH_RESULT(I_OnCheckPriv, OnCheckPriv(access, name));
		if (MOD_RESULT == EVENT_ALLOW || access->HasPriv(name))
			return true;
	}
	return false;
}

const ChanAccess *AccessGroup::Highest() const
{
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
		for (unsigned j = this->size(); j > 0; --j)
			if (this->at(j - 1)->HasPriv(privs[i - 1].name))
				return this->at(j - 1);
	return NULL;
}

bool AccessGroup::operator>(const AccessGroup &other) const
{
	if (this->SuperAdmin)
		return true;
	else if (other.SuperAdmin)
		return false;
	else if (this->Founder && !other.Founder)
		return true;
	else if (!this->Founder && other.Founder)
		return false;
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other.HasPriv(privs[i - 1].name);

		if (!this_p && !other_p)
			continue;
		else if (this_p && !other_p)
			return true;
		else
			return false;
	}

	return false;
}

bool AccessGroup::operator<(const AccessGroup &other) const
{
	if (other.SuperAdmin)
		return true;
	else if (this->SuperAdmin)
		return false;
	else if (other.Founder && !this->Founder)
		return true;
	else if (this->Founder && !other.Founder)
		return false;
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other.HasPriv(privs[i - 1].name);

		if (!this_p && !other_p)
			continue;
		else if (!this_p && other_p)
			return true;
		else
			return false;
	}

	return false;
}

bool AccessGroup::operator>=(const AccessGroup &other) const
{
	if (this->SuperAdmin)
		return true;
	else if (other.SuperAdmin)
		return false;
	else if (this->Founder)
		return true;
	else if (other.Founder)
		return false;
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other.HasPriv(privs[i - 1].name);

		if (!this_p && !other_p)
			continue;
		else if (other_p && !this_p)
			return false;
		else
			return true;
	}

	return true;
}

bool AccessGroup::operator<=(const AccessGroup &other) const
{
	if (other.SuperAdmin)
		return true;
	else if (this->SuperAdmin)
		return false;
	else if (other.Founder)
		return true;
	else if (this->Founder)
		return false;
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other.HasPriv(privs[i - 1].name);

		if (!this_p && !other_p)
			continue;
		else if (this_p && !other_p)
			return false;
		else
			return true;
	}

	return true;
}

