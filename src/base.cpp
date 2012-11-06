#include "services.h"
#include "modules.h"
#include "oper.h"
#include "account.h"
#include "regchannel.h"
#include "access.h"
#include "bots.h"

std::map<Anope::string, std::map<Anope::string, Service *> > Service::services;

void RegisterTypes()
{
	static SerializeType nc("NickCore", NickCore::unserialize), na("NickAlias", NickAlias::unserialize), bi("BotInfo", BotInfo::unserialize),
		ci("ChannelInfo", ChannelInfo::unserialize), access("ChanAccess", ChanAccess::unserialize), logsetting("LogSetting", LogSetting::unserialize),
		modelock("ModeLock", ModeLock::unserialize), akick("AutoKick", AutoKick::unserialize), badword("BadWord", BadWord::unserialize),
		memo("Memo", Memo::unserialize), xline("XLine", XLine::unserialize);
}

Base::Base()
{
}

Base::~Base()
{
	for (std::set<dynamic_reference_base *>::iterator it = this->References.begin(), it_end = this->References.end(); it != it_end; ++it)
	{
		(*it)->Invalidate();
	}
}

void Base::AddReference(dynamic_reference_base *r)
{
	this->References.insert(r);
}

void Base::DelReference(dynamic_reference_base *r)
{
	this->References.erase(r);
}

