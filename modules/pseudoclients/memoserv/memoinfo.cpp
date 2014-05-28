#include "memoinfo.h"

MemoServ::Memo *MemoInfoImpl::GetMemo(unsigned index) const
{
	if (index >= this->memos->size())
		return NULL;
	MemoServ::Memo *m = (*memos)[index];
	m->QueueUpdate();
	return m;
}

unsigned MemoInfoImpl::GetIndex(MemoServ::Memo *m) const
{
	for (unsigned i = 0; i < this->memos->size(); ++i)
		if (this->GetMemo(i) == m)
			return i;
	return -1;
}

void MemoInfoImpl::Del(unsigned index)
{
	if (index >= this->memos->size())
		return;
	delete this->GetMemo(index);
}

bool MemoInfoImpl::HasIgnore(User *u)
{
	for (unsigned i = 0; i < this->ignores.size(); ++i)
		if (u->nick.equals_ci(this->ignores[i]) || (u->Account() && u->Account()->display.equals_ci(this->ignores[i])) || Anope::Match(u->GetMask(), Anope::string(this->ignores[i])))
			return true;
	return false;
}
