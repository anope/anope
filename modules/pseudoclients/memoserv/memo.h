#include "modules/memoserv.h"

class MemoImpl : public MemoServ::Memo
{
 public:
	MemoImpl();
	~MemoImpl();

	void Serialize(Serialize::Data &data) const override;
	static Serializable* Unserialize(Serializable *obj, Serialize::Data &);
};
