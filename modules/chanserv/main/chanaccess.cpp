#include "module.h"
#include "chanaccess.h"
#include "chanaccesstype.h"

ChanServ::Channel *ChanAccessImpl::GetChannel()
{
	return Get(&ChanAccessType::ci);
}

void ChanAccessImpl::SetChannel(ChanServ::Channel *ci)
{
	Object::Set(&ChanAccessType::ci, ci);
}

Anope::string ChanAccessImpl::GetCreator()
{
	return Get(&ChanAccessType::creator);
}

void ChanAccessImpl::SetCreator(const Anope::string &c)
{
	Object::Set(&ChanAccessType::creator, c);
}

time_t ChanAccessImpl::GetLastSeen()
{
	return Get(&ChanAccessType::last_seen);
}

void ChanAccessImpl::SetLastSeen(const time_t &t)
{
	Object::Set(&ChanAccessType::last_seen, t);
}

time_t ChanAccessImpl::GetCreated()
{
	return Get(&ChanAccessType::created);
}

void ChanAccessImpl::SetCreated(const time_t &t)
{
	Object::Set(&ChanAccessType::created, t);
}

Anope::string ChanAccessImpl::GetMask()
{
	return Get(&ChanAccessType::mask);
}

void ChanAccessImpl::SetMask(const Anope::string &n)
{
	Object::Set(&ChanAccessType::mask, n);
}

Serialize::Object *ChanAccessImpl::GetObj()
{
	return Get(&ChanAccessType::obj);
}

void ChanAccessImpl::SetObj(Serialize::Object *o)
{
	Object::Set(&ChanAccessType::obj, o);
}

Anope::string ChanAccessImpl::Mask()
{
	if (NickServ::Account *acc = GetAccount())
		return acc->GetDisplay();

	return GetMask();
}

NickServ::Account *ChanAccessImpl::GetAccount()
{
	if (!GetObj() || GetObj()->GetSerializableType() != NickServ::account)
		return nullptr;

	return anope_dynamic_static_cast<NickServ::Account *>(GetObj());
}

bool ChanAccessImpl::Matches(const User *u, NickServ::Account *acc, Path &p)
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
		for (NickServ::Nick *na : acc->GetRefs<NickServ::Nick *>(NickServ::nick))
			if (Anope::Match(na->GetNick(), this->Mask()))
				return true;

	if (IRCD->IsChannelValid(this->Mask()))
	{
		ChanServ::Channel *tci = ChanServ::Find(this->Mask());
		if (tci)
		{
			for (unsigned i = 0; i < tci->GetAccessCount(); ++i)
			{
				ChanServ::ChanAccess *a = tci->GetAccess(i);
				std::pair<ChanServ::ChanAccess *, ChanServ::ChanAccess *> pair = std::make_pair(this, a);

				std::pair<Set::iterator, Set::iterator> range = p.first.equal_range(this);
				for (; range.first != range.second; ++range.first)
					if (range.first->first == pair.first && range.first->second == pair.second)
						goto cont;

				p.first.insert(pair);
				if (a->Matches(u, acc, p))
					p.second.insert(pair);

				cont:;
			}

			return p.second.count(this) > 0;
		}
	}

	return false;
}
