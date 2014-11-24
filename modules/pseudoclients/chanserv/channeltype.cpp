#include "module.h"
#include "channeltype.h"

ChannelType::ChannelType(Module *me) : Serialize::Type<ChannelImpl>(me, "ChannelInfo")
	, name(this, "name")
	, desc(this, "desc")
	, time_registered(this, "time_registered")
	, last_used(this, "last_used")
	, last_topic(this, "last_topic")
	, last_topic_setter(this, "last_topic_setter")
	, last_topic_time(this, "last_topic_time")
	, bantype(this, "bantype")
	, banexpire(this, "banexpire")
	, founder(this, "founder")
	, successor(this, "successor")
	, bi(this, "bi")
{

}

void ChannelType::Name::SetField(ChanServ::Channel *c, const Anope::string &value)
{
	ChanServ::registered_channel_map& map = ChanServ::service->GetChannels();
	map.erase(GetField(c));

	Serialize::Field<ChanServ::Channel, Anope::string>::SetField(c, value);

	map[value] = c;
}

ChanServ::Channel *ChannelType::FindChannel(const Anope::string &chan)
{
	Serialize::ID id;
	EventReturn result = Event::OnSerialize(&Event::SerializeEvents::OnSerializeFind, this, &this->name, chan, id);
	if (result == EVENT_ALLOW)
		return RequireID(id);

	// fall back
	ChanServ::registered_channel_map& map = ChanServ::service->GetChannels();
	auto it = map.find(chan);
	if (it != map.end())
		return it->second;
	return nullptr;
}

