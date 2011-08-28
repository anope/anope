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
#include "access.h"

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
		if (privs[i - 1].name == name)
			return &privs[i - 1];
	return NULL;
}

void PrivilegeManager::Init()
{
	AddPrivilege(Privilege("ACCESS_LIST", _("Allowed to view the access list")));
	AddPrivilege(Privilege("NOKICK", _("Prevents users being kicked by Services")));
	AddPrivilege(Privilege("FANTASIA", _("Allowed to use fantasy commands")));
	AddPrivilege(Privilege("FOUNDER", _("Allowed to issue commands restricted to channel founders")));
	AddPrivilege(Privilege("GREET", _("Greet message displayed")));
	AddPrivilege(Privilege("AUTOVOICE", _("Automatic mode +v")));
	AddPrivilege(Privilege("VOICEME", _("Allowed to (de)voice him/herself")));
	AddPrivilege(Privilege("VOICE", _("Allowed to (de)voice users")));
	AddPrivilege(Privilege("INFO", _("Allowed to use INFO command with ALL option")));
	AddPrivilege(Privilege("SAY", _("Allowed to use SAY and ACT commands")));
	AddPrivilege(Privilege("AUTOHALFOP", _("Automatic mode +h")));
	AddPrivilege(Privilege("HALFOPME", _("Allowed to (de)halfop him/herself")));
	AddPrivilege(Privilege("HALFOP", _("Allowed to (de)halfop users")));
	AddPrivilege(Privilege("KICK", _("Allowed to use the KICK command")));
	AddPrivilege(Privilege("SIGNKICK", _("No signed kick when SIGNKICK LEVEL is used")));
	AddPrivilege(Privilege("BAN", _("Allowed to use ban users")));
	AddPrivilege(Privilege("TOPIC", _("Allowed to change channel topics")));
	AddPrivilege(Privilege("MODE", _("Allowed to change channel modes")));
	AddPrivilege(Privilege("GETKEY", _("Allowed to use GETKEY command")));
	AddPrivilege(Privilege("INVITE", _("Allowed to use the INVITE command")));
	AddPrivilege(Privilege("UNBAN", _("Allowed to unban users")));
	AddPrivilege(Privilege("AUTOOP", _("Automatic channel operator status")));
	AddPrivilege(Privilege("AUTOOWNER", _("Automatic mode +q")));
	AddPrivilege(Privilege("OPDEOPME", _("Allowed to (de)op him/herself")));
	AddPrivilege(Privilege("OPDEOP", _("Allowed to (de)op users")));
	AddPrivilege(Privilege("AUTOPROTECT", _("Automatic mode +a")));
	AddPrivilege(Privilege("AKICK", _("Allowed to use AKICK command")));
	AddPrivilege(Privilege("BADWORDS", _("Allowed to modify channel badwords list")));
	AddPrivilege(Privilege("ASSIGN", _("Allowed to assign/unassign a bot")));
	AddPrivilege(Privilege("MEMO", _("Allowed to read channel memos")));
	AddPrivilege(Privilege("ACCESS_CHANGE", _("Allowed to modify the access list")));
	AddPrivilege(Privilege("PROTECTME", _("Allowed to (de)protect him/herself")));
}

std::vector<Privilege> &PrivilegeManager::GetPrivileges()
{
	return privs;
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
			if (this->at(j - 1)->HasPriv(privs[ - 1].name))
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

