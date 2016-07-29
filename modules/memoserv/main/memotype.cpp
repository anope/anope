#include "module.h"
#include "memotype.h"

MemoType::MemoType(Module *me) : Serialize::Type<MemoImpl>(me)
	, mi(this, "mi", &MemoImpl::memoinfo, true)
	, time(this, "time", &MemoImpl::time)
	, sender(this, "sender", &MemoImpl::sender)
	, text(this, "text", &MemoImpl::text)
	, unread(this, "unread", &MemoImpl::unread)
	, receipt(this, "receipt", &MemoImpl::receipt)
{

}

