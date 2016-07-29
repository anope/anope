#include "module.h"
#include "memoinfotype.h"

MemoInfoType::MemoInfoType(Module *me) : Serialize::Type<MemoInfoImpl>(me)
	, owner(this, "owner", &MemoInfoImpl::owner, true)
	, memomax(this, "memomax", &MemoInfoImpl::memomax)
{

}

