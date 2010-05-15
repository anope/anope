#include "services.h"
#include "pseudo.h"

/** Default constructor
 * @param display The display nick
 */
NickCore::NickCore(const std::string &coredisplay)
{
	if (coredisplay.empty())
		throw CoreException("Empty display passed to NickCore constructor");

	display = email = greet = url = NULL;
	ot = NULL;
	icq = 0;
	language = channelcount = 0;
	lastmail = 0;

	this->display = sstrdup(coredisplay.c_str());
	slist_init(&this->aliases);

	/* Set default nick core flags */
	for (size_t t = NI_BEGIN + 1; t != NI_END; ++t)
		if (Config.NSDefFlags.HasFlag(static_cast<NickCoreFlag>(t)))
			SetFlag(static_cast<NickCoreFlag>(t));
	
	NickCoreList[this->display] = this;
}

/** Default destructor
 */
NickCore::~NickCore()
{
	FOREACH_MOD(I_OnDelCore, OnDelCore(this));

	/* Clean up this nick core from any users online using it
	 * (ones that /nick but remain unidentified)
	 */
	for (std::list<User *>::iterator it = this->Users.begin(); it != this->Users.end(); ++it)
	{
		User *user = *it;
		ircdproto->SendAccountLogout(user, user->Account());
		ircdproto->SendUnregisteredNick(user);
		user->Logout();
		FOREACH_MOD(I_OnNickLogout, OnNickLogout(user));
	}
	this->Users.clear();

	/* (Hopefully complete) cleanup */
	cs_remove_nick(this);

	/* Remove the core from the list */
	NickCoreList.erase(this->display);

	/* Log .. */
	Alog() << Config.s_NickServ << ": deleting nickname group " << this->display;

	/* Clear access before deleting display name, we want to be able to use the display name in the clear access event */
	this->ClearAccess();

	/* Now we can safely free it. */
	delete [] this->display;

	if (this->email)
		delete [] this->email;
	if (this->greet)
		delete [] this->greet;
	if (this->url)
	       delete [] this->url;

	if (!this->memos.memos.empty())
	{
		for (unsigned i = 0; i < this->memos.memos.size(); ++i)
		{
			if (this->memos.memos[i]->text)
				delete [] this->memos.memos[i]->text;
			delete this->memos.memos[i];
		}
		this->memos.memos.clear();
	}
}

bool NickCore::HasCommand(const std::string &cmdstr) const
{
	if (!this->ot)
	{
		// No opertype.
		return false;
	}

	return this->ot->HasCommand(cmdstr);
}

bool NickCore::IsServicesOper() const
{
	if (this->ot)
		return true;

	return false;
}

bool NickCore::HasPriv(const std::string &privstr) const
{
	if (!this->ot)
	{
		// No opertype.
		return false;
	}

	return this->ot->HasPriv(privstr);
}

void NickCore::AddAccess(const std::string &entry)
{
	access.push_back(entry);
	FOREACH_MOD(I_OnNickAddAccess, OnNickAddAccess(this, entry));
}

std::string NickCore::GetAccess(unsigned entry)
{
	if (access.empty() || entry >= access.size())
		return "";
	return access[entry];
}

bool NickCore::FindAccess(const std::string &entry)
{
	for (unsigned i = 0; i < access.size(); ++i)
		if (access[i] == entry)
			return true;

	return false;
}

void NickCore::EraseAccess(const std::string &entry)
{
	for (unsigned i = 0; i < access.size(); ++i)
		if (access[i] == entry)
		{
			FOREACH_MOD(I_OnNickEraseAccess, OnNickEraseAccess(this, entry));
			access.erase(access.begin() + i);
			break;
		}
}

void NickCore::ClearAccess()
{
	FOREACH_MOD(I_OnNickClearAccess, OnNickClearAccess(this));
	access.clear();
}
