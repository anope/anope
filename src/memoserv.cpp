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

Memo::Memo() : Flags<MemoFlag>(MemoFlagStrings) { }

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

