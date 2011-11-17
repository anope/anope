/* MemoServ functions.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "memoserv.h"

Memo::Memo() : Flags<MemoFlag>(MemoFlagStrings) { }

Anope::string Memo::serialize_name() const
{
	return "Memo";
}

Serializable::serialized_data Memo::serialize()
{
	serialized_data data;	

	data["owner"] << this->owner;
	data["time"].setType(Serialize::DT_INT) << this->time;
	data["sender"] << this->sender;
	data["text"] << this->text;
	data["flags"] << this->ToString();

	return data;
}

void Memo::unserialize(serialized_data &data)
{
	if (!memoserv)
		return;
	
	bool ischan;
	MemoInfo *mi = memoserv->GetMemoInfo(data["owner"].astr(), ischan);
	if (!mi)
		return;

	Memo *m = new Memo();
	data["owner"] >> m->owner;
	data["time"] >> m->time;
	data["sender"] >> m->sender;
	data["text"] >> m->text;
	m->FromString(data["flags"].astr());

	mi->memos.push_back(m);
}

unsigned MemoInfo::GetIndex(Memo *m) const
{
	for (unsigned i = 0; i < this->memos.size(); ++i)
		if (this->memos[i] == m)
			return i;
	return -1;
}

void MemoInfo::Del(unsigned index)
{
	if (index >= this->memos.size())
		return;
	delete this->memos[index];
	this->memos.erase(this->memos.begin() + index);
}

void MemoInfo::Del(Memo *memo)
{
	std::vector<Memo *>::iterator it = std::find(this->memos.begin(), this->memos.end(), memo);

	if (it != this->memos.end())
	{
		delete memo;
		this->memos.erase(it);
	}
}

bool MemoInfo::HasIgnore(User *u)
{
	for (unsigned i = 0; i < this->ignores.size(); ++i)
		if (u->nick.equals_ci(this->ignores[i]) || (u->Account() && u->Account()->display.equals_ci(this->ignores[i])) || Anope::Match(u->GetMask(), Anope::string(this->ignores[i])))
			return true;
	return false;
}

