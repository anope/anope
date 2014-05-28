#include "memo.h"

MemoImpl::MemoImpl()
{
	unread = receipt = false;
}

MemoImpl::~MemoImpl()
{
	bool ischan, isregistered;
	MemoServ::MemoInfo *mi = MemoServ::service->GetMemoInfo(this->owner, ischan, isregistered, false);
	if (mi)
	{
		std::vector<Memo *>::iterator it = std::find(mi->memos->begin(), mi->memos->end(), this);

		if (it != mi->memos->end())
			mi->memos->erase(it);
	}
}

void MemoImpl::Serialize(Serialize::Data &data) const
{
	data["owner"] << this->owner;
	data.SetType("time", Serialize::Data::DT_INT); data["time"] << this->time;
	data["sender"] << this->sender;
	data["text"] << this->text;
	data["unread"] << this->unread;
	data["receipt"] << this->receipt;
}

Serializable* MemoImpl::Unserialize(Serializable *obj, Serialize::Data &data)
{
	Anope::string owner;

	data["owner"] >> owner;

	bool ischan, isregistered;
	MemoServ::MemoInfo *mi = MemoServ::service->GetMemoInfo(owner, ischan, isregistered, true);
	if (!mi)
		return NULL;

	Memo *m;
	if (obj)
		m = anope_dynamic_static_cast<Memo *>(obj);
	else
		m = new MemoImpl();

	m->owner = owner;
	data["time"] >> m->time;
	data["sender"] >> m->sender;
	data["text"] >> m->text;
	data["unread"] >> m->unread;
	data["receipt"] >> m->receipt;

	if (obj == NULL)
		mi->memos->push_back(m);
	return m;
}
