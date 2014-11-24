#include "module.h"
#include "memotype.h"

MemoType::MemoType(Module *me) : Serialize::Type<MemoImpl>(me, "Memo")
	, mi(this, "mi", true)
	, time(this, "time")
	, sender(this, "sender")
	, text(this, "text")
	, unread(this, "unread")
	, receipt(this, "receipt")
{

}

