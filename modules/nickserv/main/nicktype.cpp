#include "module.h"
#include "nicktype.h"

NickType::NickType(Module *me) : Serialize::Type<NickImpl>(me, "NickAlias")
	, nick(this, "nick")
	, last_quit(this, "last_quit")
	, last_realname(this, "last_realname")
	, last_usermask(this, "last_usermask")
	, last_realhost(this, "last_realhost")
	, time_registered(this, "time_registered")
	, last_seen(this, "last_seen")
	, vhost_ident(this, "vhost_ident")
	, vhost_host(this, "vhost_host")
	, vhost_creator(this, "vhost_creator")
	, vhost_created(this, "vhost_created")
	, nc(this, "nc")
{

}

void NickType::Nick::SetField(NickImpl *na, const Anope::string &value)
{
	/* Remove us from the aliases list */
	NickServ::nickalias_map &map = NickServ::service->GetNickMap();
	map.erase(GetField(na));

	Serialize::Field<NickImpl, Anope::string>::SetField(na, value);

	map[value] = na;
}

NickServ::Nick *NickType::FindNick(const Anope::string &n)
{
	Serialize::ID id;
	EventReturn result = Event::OnSerialize(&Event::SerializeEvents::OnSerializeFind, this, &this->nick, n, id);
	if (result == EVENT_ALLOW)
		return RequireID(id);

	NickServ::nickalias_map &map = NickServ::service->GetNickMap();
	auto it = map.find(n);
	if (it != map.end())
		return it->second;
	return nullptr;
}

