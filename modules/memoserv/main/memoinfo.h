#include "modules/memoserv.h"

class MemoInfoImpl : public MemoServ::MemoInfo
{
 public:
	MemoInfoImpl(Serialize::TypeBase *type) : MemoServ::MemoInfo(type) { }
	MemoInfoImpl(Serialize::TypeBase *type, Serialize::ID id) : MemoServ::MemoInfo(type, id) { }

	MemoServ::Memo *GetMemo(unsigned index) override;
	unsigned GetIndex(MemoServ::Memo *m) override;
	void Del(unsigned index) override;
	bool HasIgnore(User *u) override;

	Serialize::Object *GetOwner() override;
	void SetOwner(Serialize::Object *) override;

	int16_t GetMemoMax() override;
	void SetMemoMax(const int16_t &) override;

	std::vector<MemoServ::Memo *> GetMemos() override;
	std::vector<MemoServ::Ignore *> GetIgnores() override;
};

