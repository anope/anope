#include "ignore.h"

class IgnoreType : public Serialize::Type<IgnoreImpl>
{
 public:
	Serialize::ObjectField<IgnoreImpl, MemoServ::MemoInfo *> mi;
	Serialize::Field<IgnoreImpl, Anope::string> mask;

 	IgnoreType(Module *);
};
