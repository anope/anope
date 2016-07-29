#include "modules/chanserv.h"
#include "accessgroup.h"

using namespace ChanServ;

bool AccessGroup::HasPriv(const Anope::string &priv)
{
	if (this->super_admin)
		return true;
	else if (!ci || ci->GetLevel(priv) == ACCESS_INVALID)
		return false;

	/* Privileges prefixed with auto are understood to be given
	 * automatically. Sometimes founders want to not automatically
	 * obtain privileges, so we will let them */
	bool auto_mode = !priv.find("AUTO");

	/* Only grant founder privilege if this isn't an auto mode or if they don't match any entries in this group */
	if ((!auto_mode || this->empty()) && this->founder)
		return true;

	EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&::Event::GroupCheckPriv::OnGroupCheckPriv, this, priv);
	if (MOD_RESULT != EVENT_CONTINUE)
		return MOD_RESULT == EVENT_ALLOW;

	for (unsigned i = this->size(); i > 0; --i)
	{
		ChanAccess *access = this->at(i - 1);

		if (access->HasPriv(priv))
			return true;
	}

	return false;
}

ChanAccess *AccessGroup::Highest()
{
	ChanAccess *highest = NULL;
	for (unsigned i = 0; i < this->size(); ++i)
		if (highest == NULL || *this->at(i) > *highest)
			highest = this->at(i);
	return highest;
}

bool AccessGroup::operator>(AccessGroup &other)
{
	if (other.super_admin)
		return false;
	else if (this->super_admin)
		return true;
	else if (other.founder)
		return false;
	else if (this->founder)
		return true;

	const std::vector<Privilege> &privs = service->GetPrivileges();
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

bool AccessGroup::operator<(AccessGroup &other)
{
	if (this->super_admin)
		return false;
	else if (other.super_admin)
		return true;
	else if (this->founder)
		return false;
	else if (other.founder)
		return true;

	const std::vector<Privilege> &privs = service->GetPrivileges();
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

bool AccessGroup::operator>=(AccessGroup &other)
{
	return !(*this < other);
}

bool AccessGroup::operator<=(AccessGroup &other)
{
	return !(*this > other);
}