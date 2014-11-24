#include "module.h"
#include "ignoretype.h"

IgnoreType::IgnoreType(Module *me) : Serialize::Type<IgnoreImpl>(me, "MemoIgnore")
	, mi(this, "mi", true)
	, mask(this, "mask")
{

}

