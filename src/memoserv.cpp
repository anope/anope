/* MemoServ functions.
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */


#include "services.h"
#include "modules.h"
#include "service.h"
#include "memoserv.h"
#include "memo.h"
#include "users.h"
#include "account.h"

Memo::Memo() : Flags<MemoFlag>(MemoFlagStrings), Serializable("Memo") { }

Serialize::Data Memo::serialize() const
{
	Serialize::Data data;	

	data["owner"] << this->owner;
	data["time"].setType(Serialize::DT_INT) << this->time;
	data["sender"] << this->sender;
	data["text"] << this->text;
	data["flags"] << this->ToString();

	return data;
}

Serializable* Memo::unserialize(Serializable *obj, Serialize::Data &data)
{
	if (!memoserv)
		return NULL;
	
	bool ischan;
	MemoInfo *mi = memoserv->GetMemoInfo(data["owner"].astr(), ischan);
	if (!mi)
		return NULL;

	Memo *m;
	if (obj)
		m = anope_dynamic_static_cast<Memo *>(obj);
	else
		m = new Memo();
	data["owner"] >> m->owner;
	data["time"] >> m->time;
	data["sender"] >> m->sender;
	data["text"] >> m->text;
	m->FromString(data["flags"].astr());

	if (obj == NULL)
		mi->memos->push_back(m);
	return m;
}

MemoInfo::MemoInfo() : memos("Memo")
{
}

Memo *MemoInfo::GetMemo(unsigned index) const
{
	if (index >= this->memos->size())
		return NULL;
	Memo *m = (*memos)[index];
	m->QueueUpdate();
	return m;
}

unsigned MemoInfo::GetIndex(Memo *m) const
{
	for (unsigned i = 0; i < this->memos->size(); ++i)
		if (this->GetMemo(i) == m)
			return i;
	return -1;
}

void MemoInfo::Del(unsigned index)
{
	if (index >= this->memos->size())
		return;
	this->GetMemo(index)->destroy();
	this->memos->erase(this->memos->begin() + index);
}

void MemoInfo::Del(Memo *memo)
{
	std::vector<Memo *>::iterator it = std::find(this->memos->begin(), this->memos->end(), memo);

	if (it != this->memos->end())
	{
		memo->destroy();
		this->memos->erase(it);
	}
}

bool MemoInfo::HasIgnore(User *u)
{
	for (unsigned i = 0; i < this->ignores.size(); ++i)
		if (u->nick.equals_ci(this->ignores[i]) || (u->Account() && u->Account()->display.equals_ci(this->ignores[i])) || Anope::Match(u->GetMask(), Anope::string(this->ignores[i])))
			return true;
	return false;
}

