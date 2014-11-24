#include "modules/nickserv.h"
#include "modetype.h"

NickServ::Account *ModeImpl::GetAccount()
{
	return Get(&NSModeType::account);
}

void ModeImpl::SetAccount(NickServ::Account *a)
{
	Set(&NSModeType::account, a);
}

Anope::string ModeImpl::GetMode()
{
	return Get(&NSModeType::mode);
}

void ModeImpl::SetMode(const Anope::string &m)
{
	Set(&NSModeType::mode, m);
}

