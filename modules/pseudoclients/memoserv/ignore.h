#include "modules/memoserv.h"

class IgnoreImpl : public MemoServ::Ignore
{
 public:
	IgnoreImpl(Serialize::TypeBase *type) : MemoServ::Ignore(type) { }
	IgnoreImpl(Serialize::TypeBase *type, Serialize::ID id) : MemoServ::Ignore(type, id) { }
	~IgnoreImpl();

	MemoServ::MemoInfo *GetMemoInfo() override;
	void SetMemoInfo(MemoServ::MemoInfo *) override;

	Anope::string GetMask() override;
	void SetMask(const Anope::string &mask) override;
};
