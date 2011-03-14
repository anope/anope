#include "services.h"
#include "modules.h"

/** Default constructor
 * @param display The display nick
 */
NickCore::NickCore(const Anope::string &coredisplay) : Flags<NickCoreFlag, NI_END>(NickCoreFlagStrings)
{
	if (coredisplay.empty())
		throw CoreException("Empty display passed to NickCore constructor");

	this->o = NULL;
	this->channelcount = 0;
	this->lastmail = 0;
	this->memos.memomax = Config->MSMaxMemos;
	this->language = Config->NSDefLanguage;

	this->display = coredisplay;

	/* Set default nick core flags */
	for (size_t t = NI_BEGIN + 1; t != NI_END; ++t)
		if (Config->NSDefFlags.HasFlag(static_cast<NickCoreFlag>(t)))
			this->SetFlag(static_cast<NickCoreFlag>(t));

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
	for (std::list<User *>::iterator it = this->Users.begin(); it != this->Users.end();)
	{
		User *user = *it++;
		ircdproto->SendAccountLogout(user, user->Account());
		user->RemoveMode(NickServ, UMODE_REGISTERED);
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
	Log(NickServ, "nick") << "deleting nickname group " << this->display;

	/* Clear access before deleting display name, we want to be able to use the display name in the clear access event */
	this->ClearAccess();

	if (!this->memos.memos.empty())
	{
		for (unsigned i = 0, end = this->memos.memos.size(); i < end; ++i)
			delete this->memos.memos[i];
		this->memos.memos.clear();
	}
}

bool NickCore::IsServicesOper() const
{
	return this->o != NULL;
}

void NickCore::AddAccess(const Anope::string &entry)
{
	this->access.push_back(entry);
	FOREACH_MOD(I_OnNickAddAccess, OnNickAddAccess(this, entry));
}

Anope::string NickCore::GetAccess(unsigned entry) const
{
	if (this->access.empty() || entry >= this->access.size())
		return "";
	return this->access[entry];
}

bool NickCore::FindAccess(const Anope::string &entry)
{
	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
		if (this->access[i] == entry)
			return true;

	return false;
}

void NickCore::EraseAccess(const Anope::string &entry)
{
	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
		if (this->access[i] == entry)
		{
			FOREACH_MOD(I_OnNickEraseAccess, OnNickEraseAccess(this, entry));
			this->access.erase(this->access.begin() + i);
			break;
		}
}

void NickCore::ClearAccess()
{
	FOREACH_MOD(I_OnNickClearAccess, OnNickClearAccess(this));
	this->access.clear();
}

void NickCore::AddCert(const Anope::string &entry)
{
	this->cert.push_back(entry);
	FOREACH_MOD(I_OnNickAddCert, OnNickAddCert(this, entry));
}

Anope::string NickCore::GetCert(unsigned entry) const
{
	if (this->cert.empty() || entry >= this->cert.size())
		return "";
	return this->cert[entry];
}

bool NickCore::FindCert(const Anope::string &entry)
{
	for (unsigned i = 0, end = this->cert.size(); i < end; ++i)
		if (this->cert[i] == entry)
			return true;

	return false;
}

void NickCore::EraseCert(const Anope::string &entry)
{
	for (unsigned i = 0, end = this->cert.size(); i < end; ++i)
		if (this->cert[i] == entry)
		{
			FOREACH_MOD(I_OnNickEraseCert, OnNickEraseCert(this, entry));
			this->cert.erase(this->cert.begin() + i);
			break;
		}
}

void NickCore::ClearCert()
{
	FOREACH_MOD(I_OnNickClearCert, OnNickClearCert(this));
	this->cert.clear();
}
