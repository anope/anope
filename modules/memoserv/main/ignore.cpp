#include "memo.h"
#include "ignoretype.h"

IgnoreImpl::~IgnoreImpl()
{
}

MemoServ::MemoInfo *IgnoreImpl::GetMemoInfo()
{
	return Get(&IgnoreType::mi);
}

void IgnoreImpl::SetMemoInfo(MemoServ::MemoInfo *m)
{
	Set(&IgnoreType::mi, m);
}

Anope::string IgnoreImpl::GetMask()
{
	return Get(&IgnoreType::mask);
}

void IgnoreImpl::SetMask(const Anope::string &mask)
{
	Set(&IgnoreType::mask, mask);
}

