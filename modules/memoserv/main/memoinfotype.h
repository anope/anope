#include "memoinfo.h"

class MemoInfoType : public Serialize::Type<MemoInfoImpl>
{
 public:
	Serialize::ObjectField<MemoInfoImpl, Serialize::Object *> owner;
	Serialize::Field<MemoInfoImpl, int16_t> memomax;

	MemoInfoType(Module *);
};
