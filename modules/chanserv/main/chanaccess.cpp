#include "module.h"
#include "modules/chanserv/main/chanaccess.h"
#include "chanaccesstype.h"

ChanServ::Channel *ChanAccessImpl::GetChannel()
{
	return Get(&ChanAccessType<ChanServ::ChanAccess>::ci);
}

void ChanAccessImpl::SetChannel(ChanServ::Channel *ci)
{
	Object::Set(&ChanAccessType<ChanServ::ChanAccess>::ci, ci);
}

Anope::string ChanAccessImpl::GetCreator()
{
	return Get(&ChanAccessType<ChanServ::ChanAccess>::creator);
}

void ChanAccessImpl::SetCreator(const Anope::string &c)
{
	Object::Set(&ChanAccessType<ChanServ::ChanAccess>::creator, c);
}

time_t ChanAccessImpl::GetLastSeen()
{
	return Get(&ChanAccessType<ChanServ::ChanAccess>::last_seen);
}

void ChanAccessImpl::SetLastSeen(const time_t &t)
{
	Object::Set(&ChanAccessType<ChanServ::ChanAccess>::last_seen, t);
}

time_t ChanAccessImpl::GetCreated()
{
	return Get(&ChanAccessType<ChanServ::ChanAccess>::created);
}

void ChanAccessImpl::SetCreated(const time_t &t)
{
	Object::Set(&ChanAccessType<ChanServ::ChanAccess>::created, t);
}

Anope::string ChanAccessImpl::GetMask()
{
	return Get(&ChanAccessType<ChanServ::ChanAccess>::mask);
}

void ChanAccessImpl::SetMask(const Anope::string &n)
{
	Object::Set(&ChanAccessType<ChanServ::ChanAccess>::mask, n);
}

Serialize::Object *ChanAccessImpl::GetObj()
{
	return Get(&ChanAccessType<ChanServ::ChanAccess>::obj);
}

void ChanAccessImpl::SetObj(Serialize::Object *o)
{
	Object::Set(&ChanAccessType<ChanServ::ChanAccess>::obj, o);
}

Anope::string ChanAccessImpl::Mask()
{
	if (NickServ::Account *acc = GetAccount())
		return acc->GetDisplay();

	return GetMask();
}

NickServ::Account *ChanAccessImpl::GetAccount()
{
	if (!GetObj() || GetObj()->GetSerializableType()->GetName() != NickServ::Account::NAME)
		return nullptr;

	return anope_dynamic_static_cast<NickServ::Account *>(GetObj());
}

bool ChanAccessImpl::Matches(const User *u, NickServ::Account *acc)
{
	if (this->GetAccount())
		return this->GetAccount() == acc;

	if (u)
	{
		bool is_mask = this->Mask().find_first_of("!@?*") != Anope::string::npos;
		if (is_mask && Anope::Match(u->nick, this->Mask()))
			return true;
		else if (Anope::Match(u->GetDisplayedMask(), this->Mask()))
			return true;
	}

	if (acc)
		for (NickServ::Nick *na : acc->GetRefs<NickServ::Nick *>())
			if (Anope::Match(na->GetNick(), this->Mask()))
				return true;

	return false;
}
