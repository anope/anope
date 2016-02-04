#include "modules/chanserv.h"
#include "leveltype.h"

ChanServ::Channel *LevelImpl::GetChannel()
{
	return Get(&LevelType::channel);
}

void LevelImpl::SetChannel(ChanServ::Channel *c)
{
	Set(&LevelType::channel, c);
}

Anope::string LevelImpl::GetName()
{
	return Get(&LevelType::name);
}

void LevelImpl::SetName(const Anope::string &n)
{
	Set(&LevelType::name, n);
}

int LevelImpl::GetLevel()
{
	return Get(&LevelType::level);
}

void LevelImpl::SetLevel(const int &i)
{
	Set(&LevelType::level, i);
}

