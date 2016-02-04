#include "modules/chanserv.h"
#include "modetype.h"

ChanServ::Channel *ModeImpl::GetChannel()
{
	return Get(&CSModeType::channel);
}

void ModeImpl::SetChannel(ChanServ::Channel *c)
{
	Set(&CSModeType::channel, c);
}

Anope::string ModeImpl::GetMode()
{
	return Get(&CSModeType::mode);
}

void ModeImpl::SetMode(const Anope::string &m)
{
	Set(&CSModeType::mode, m);
}

Anope::string ModeImpl::GetParam()
{
	return Get(&CSModeType::param);
}

void ModeImpl::SetParam(const Anope::string &p)
{
	Set(&CSModeType::param, p);
}

