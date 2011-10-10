#include "services.h"
#include "modules.h"

std::vector<SerializableBase *> serialized_types;
std::list<SerializableBase *> *serialized_items;

void RegisterTypes()
{
	Serializable<NickCore>::Alloc.Register("NickCore");
	Serializable<NickAlias>::Alloc.Register("NickAlias");
	Serializable<BotInfo>::Alloc.Register("BotInfo");
	Serializable<ChannelInfo>::Alloc.Register("ChannelInfo");
	Serializable<LogSetting>::Alloc.Register("LogSetting");
	Serializable<ModeLock>::Alloc.Register("ModeLock");
	Serializable<AutoKick>::Alloc.Register("AutoKick");
	Serializable<BadWord>::Alloc.Register("BadWord");
	Serializable<Memo>::Alloc.Register("Memo");
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

