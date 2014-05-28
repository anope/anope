#include "modules/memoserv.h"

struct MemoInfoImpl : MemoServ::MemoInfo
{
	MemoServ::Memo *GetMemo(unsigned index) const override;
	unsigned GetIndex(MemoServ::Memo *m) const override;
	void Del(unsigned index) override;
	bool HasIgnore(User *u) override;
};

