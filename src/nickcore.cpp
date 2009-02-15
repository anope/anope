#include "services.h"

NickCore::NickCore()
{
	next = prev = NULL;
	display = email = greet = url = NULL;
	ot = NULL;
	pass[0] = '\0';
	icq = flags = 0;
	language = accesscount = channelcount = 0;
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

bool NickCore::HasPriv(const std::string &privstr) const
{
	if (!this->ot)
	{
		// No opertype.
		return false;
	}

	return this->ot->HasPriv(privstr);
}
