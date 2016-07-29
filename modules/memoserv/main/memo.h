#include "modules/memoserv.h"

class MemoImpl : public MemoServ::Memo
{
	friend class MemoType;

	MemoServ::MemoInfo *memoinfo = nullptr;
	Anope::string text, sender;
	time_t time = 0;
	bool unread = false, receipt = false;

 public:
	MemoImpl(Serialize::TypeBase *type) : MemoServ::Memo(type) { }
	MemoImpl(Serialize::TypeBase *type, Serialize::ID id) : MemoServ::Memo(type, id) { }
	~MemoImpl();

	MemoServ::MemoInfo *GetMemoInfo() override;
	void SetMemoInfo(MemoServ::MemoInfo *) override;

	time_t GetTime() override;
	void SetTime(const time_t &) override;

	Anope::string GetSender() override;
	void SetSender(const Anope::string &) override;

	Anope::string GetText() override;
	void SetText(const Anope::string &) override;

	bool GetUnread() override;
	void SetUnread(const bool &) override;

	bool GetReceipt() override;
	void SetReceipt(const bool &) override;
};
