/*
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"

enum
{
	ACCESS_INVALID = -10000,
	ACCESS_FOUNDER = 10001
};

Privilege::Privilege(const Anope::string &n, const Anope::string &d) : name(n), desc(d)
{
}

bool Privilege::operator==(const Privilege &other)
{
	return this->name == other.name;
}

std::vector<Privilege> PrivilegeManager::privs;

void PrivilegeManager::AddPrivilege(Privilege p, int pos, int def)
{
	if (pos < 0 || static_cast<size_t>(pos) > privs.size())
		pos = privs.size();
	privs.insert(privs.begin() + pos, p);
	for (registered_channel_map::const_iterator cit = RegisteredChannelList.begin(), cit_end = RegisteredChannelList.end(); cit != cit_end; ++cit)
		cit->second->SetLevel(p.name, def);
}

void PrivilegeManager::RemovePrivilege(Privilege &p)
{
	std::vector<Privilege>::iterator it = std::find(privs.begin(), privs.end(), p);
	if (it != privs.end())
		privs.erase(it);
	for (registered_channel_map::const_iterator cit = RegisteredChannelList.begin(), cit_end = RegisteredChannelList.end(); cit != cit_end; ++cit)
		cit->second->RemoveLevel(p.name);
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

AccessProvider::AccessProvider(Module *o, const Anope::string &n) : Service<AccessProvider>(o, n)
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

bool ChanAccess::operator>(ChanAccess &other)
{
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
		if (this->HasPriv(privs[i - 1].name) && !other.HasPriv(privs[i - 1].name))
			return true;
	return false;
}

bool ChanAccess::operator<(ChanAccess &other)
{
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
		if (!this->HasPriv(privs[i - 1].name) && other.HasPriv(privs[i - 1].name))
			return true;
	return false;
}

bool ChanAccess::operator>=(ChanAccess &other)
{
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other.HasPriv(privs[i - 1].name);

		if ((this_p && !other_p) || (this_p && other_p))
			return true;
	}

	return false;
}

bool ChanAccess::operator<=(ChanAccess &other)
{
	const std::vector<Privilege> &privs = PrivilegeManager::GetPrivileges();
	for (unsigned i = privs.size(); i > 0; --i)
	{
		bool this_p = this->HasPriv(privs[i - 1].name),
			other_p = other.HasPriv(privs[i - 1].name);

		if ((!this_p && other_p) || (this_p && other_p))
			return true;
	}

	return false;
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

ChanAccess *AccessGroup::Highest() const
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
		if (this->HasPriv(privs[i - 1].name) && !other.HasPriv(privs[i - 1].name))
			return true;
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
		if (!this->HasPriv(privs[i - 1].name) && other.HasPriv(privs[i - 1].name))
			return true;
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

		if ((this_p && !other_p) || (this_p && other_p))
			return true;
	}

	return false;
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

		if ((!this_p && other_p) || (this_p && other_p))
			return true;
	}

	return false;
}

