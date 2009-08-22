#include "services.h"
#include "pseudo.h"

NickCore::NickCore()
{
	next = prev = NULL;
	display = email = greet = url = NULL;
	ot = NULL;
	pass[0] = '\0';
	icq = flags = 0;
	language = channelcount = 0;
	lastmail = 0;
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
