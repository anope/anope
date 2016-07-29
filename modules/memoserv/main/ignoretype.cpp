#include "module.h"
#include "ignoretype.h"

IgnoreType::IgnoreType(Module *me) : Serialize::Type<IgnoreImpl>(me)
	, mi(this, "mi", &IgnoreImpl::memoinfo, true)
	, mask(this, "mask", &IgnoreImpl::mask)
{

}

