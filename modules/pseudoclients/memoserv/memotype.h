#include "memo.h"

class MemoType : public Serialize::Type<MemoImpl>
{
 public:
	Serialize::ObjectField<MemoImpl, MemoServ::MemoInfo *> mi;
	Serialize::Field<MemoImpl, time_t> time;
	Serialize::Field<MemoImpl, Anope::string> sender;
	Serialize::Field<MemoImpl, Anope::string> text;
	Serialize::Field<MemoImpl, bool> unread, receipt;

 	MemoType(Module *);
};
