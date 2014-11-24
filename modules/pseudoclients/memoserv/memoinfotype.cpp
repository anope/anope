#include "module.h"
#include "memoinfotype.h"

MemoInfoType::MemoInfoType(Module *me) : Serialize::Type<MemoInfoImpl>(me, "MemoInfo")
	, owner(this, "owner", true)
	, memomax(this, "memomax")
{

}

