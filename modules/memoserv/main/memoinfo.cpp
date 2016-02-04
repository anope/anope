#include "memoinfotype.h"

MemoServ::Memo *MemoInfoImpl::GetMemo(unsigned index)
{
	auto memos = GetMemos();
	return index >= memos.size() ? nullptr : memos[index];
}

unsigned MemoInfoImpl::GetIndex(MemoServ::Memo *m)
{
	auto memos = GetMemos();
	for (unsigned i = 0; i < memos.size(); ++i)
		if (this->GetMemo(i) == m)
			return i;
	return -1; // XXX wtf?
}

void MemoInfoImpl::Del(unsigned index)
{
	delete GetMemo(index);
}

bool MemoInfoImpl::HasIgnore(User *u)
{
	for (MemoServ::Ignore *ign : GetIgnores())
	{
		const Anope::string &mask = ign->GetMask();
		if (u->nick.equals_ci(mask) || (u->Account() && u->Account()->GetDisplay().equals_ci(mask)) || Anope::Match(u->GetMask(), mask))
			return true;
	}
	return false;
}

Serialize::Object *MemoInfoImpl::GetOwner()
{
	return Get(&MemoInfoType::owner);
}

void MemoInfoImpl::SetOwner(Serialize::Object *o)
{
	Set(&MemoInfoType::owner, o);
}

int16_t MemoInfoImpl::GetMemoMax()
{
	return Get(&MemoInfoType::memomax);
}

void MemoInfoImpl::SetMemoMax(const int16_t &i)
{
	Set(&MemoInfoType::memomax, i);
}

std::vector<MemoServ::Memo *> MemoInfoImpl::GetMemos()
{
	return GetRefs<MemoServ::Memo *>(MemoServ::memo);
}

std::vector<MemoServ::Ignore *> MemoInfoImpl::GetIgnores()
{
	return GetRefs<MemoServ::Ignore *>(MemoServ::ignore);
}

