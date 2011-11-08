#include "services.h"
#include "modules.h"
#include "oper.h"

std::vector<Anope::string> SerializeType::type_order;
Anope::map<SerializeType *> SerializeType::types;
std::list<Serializable *> Serializable::serizliable_items;

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

