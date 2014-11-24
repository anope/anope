/* Registered channel functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#include "module.h"
#include "channeltype.h"
#include "modules/cs_akick.h"

ChannelImpl::ChannelImpl(Serialize::TypeBase *type, const Anope::string &chname) : ChanServ::Channel(type)
{
	SetName(chname);
	SetTimeRegistered(Anope::CurTime);
	SetLastUsed(Anope::CurTime);
}

ChannelImpl::~ChannelImpl()
{
	ChanServ::registered_channel_map& map = ChanServ::service->GetChannels();
	map.erase(this->GetName());
}

void ChannelImpl::Delete()
{
	Event::OnDelChan(&Event::DelChan::OnDelChan, this);

	Log(LOG_DEBUG) << "Deleting channel " << this->GetName();

	if (this->c)
	{
		if (this->GetBot() && this->c->FindUser(this->GetBot()))
			this->GetBot()->Part(this->c);

		/* Parting the service bot can cause the channel to go away */

		if (this->c)
		{
			if (this->c && this->c->CheckDelete())
				delete this->c;

			this->c = NULL;
		}
	}

	return Serialize::Object::Delete();
}

Anope::string ChannelImpl::GetName()
{
	return Get<Anope::string>(&ChannelType::name);
}

void ChannelImpl::SetName(const Anope::string &name)
{
	Set(&ChannelType::name, name);
}

Anope::string ChannelImpl::GetDesc()
{
	return Get(&ChannelType::desc);
}

void ChannelImpl::SetDesc(const Anope::string &desc)
{
	Set(&ChannelType::desc, desc);
}

time_t ChannelImpl::GetTimeRegistered()
{
	return Get(&ChannelType::time_registered);
}

void ChannelImpl::SetTimeRegistered(const time_t &t)
{
	Set(&ChannelType::time_registered, t);
}

time_t ChannelImpl::GetLastUsed()
{
	return Get(&ChannelType::last_used);
}

void ChannelImpl::SetLastUsed(const time_t &t)
{
	Set(&ChannelType::last_used, t);
}

Anope::string ChannelImpl::GetLastTopic()
{
	return Get(&ChannelType::last_topic);
}

void ChannelImpl::SetLastTopic(const Anope::string &topic)
{
	Set(&ChannelType::last_topic, topic);
}

Anope::string ChannelImpl::GetLastTopicSetter()
{
	return Get(&ChannelType::last_topic_setter);
}

void ChannelImpl::SetLastTopicSetter(const Anope::string &setter)
{
	Set(&ChannelType::last_topic_setter, setter);
}

time_t ChannelImpl::GetLastTopicTime()
{
	return Get(&ChannelType::last_topic_time);
}

void ChannelImpl::SetLastTopicTime(const time_t &t)
{
	Set(&ChannelType::last_topic_time, t);
}

int16_t ChannelImpl::GetBanType()
{
	return Get(&ChannelType::bantype);
}

void ChannelImpl::SetBanType(const int16_t &i)
{
	Set(&ChannelType::bantype, i);
}

time_t ChannelImpl::GetBanExpire()
{
	return Get(&ChannelType::banexpire);
}

void ChannelImpl::SetBanExpire(const time_t &t)
{
	Set(&ChannelType::banexpire, t);
}

BotInfo *ChannelImpl::GetBI()
{
	return Get(&ChannelType::bi);
}

void ChannelImpl::SetBI(BotInfo *bi)
{
	Set(&ChannelType::bi, bi);
}

ServiceBot *ChannelImpl::GetBot()
{
	BotInfo *bi = GetBI();
	return bi ? bi->bot : nullptr;
}

void ChannelImpl::SetBot(ServiceBot *bi)
{
	SetBI(bi->bi);
}

MemoServ::MemoInfo *ChannelImpl::GetMemos()
{
	return GetRef<MemoServ::MemoInfo *>(MemoServ::memoinfo);
}

void ChannelImpl::SetFounder(NickServ::Account *nc)
{
	Set(&ChannelType::founder, nc);
}

NickServ::Account *ChannelImpl::GetFounder()
{
	return Get(&ChannelType::founder);
}

void ChannelImpl::SetSuccessor(NickServ::Account *nc)
{
	Set(&ChannelType::successor, nc);
}

NickServ::Account *ChannelImpl::GetSuccessor()
{
	return Get(&ChannelType::successor);
}

ChanServ::ChanAccess *ChannelImpl::GetAccess(unsigned index)
{
	std::vector<ChanServ::ChanAccess *> a = GetRefs<ChanServ::ChanAccess *>(ChanServ::chanaccess);
	return a.size() > index ? a[index] : nullptr;
}

ChanServ::AccessGroup ChannelImpl::AccessFor(const User *u)
{
	ChanServ::AccessGroup group;

	if (u == NULL)
		return group;

	const NickServ::Account *nc = u->Account();
	if (nc == NULL && !this->HasFieldS("NS_SECURE") && u->IsRecognized())
	{
		NickServ::Nick *na = NickServ::FindNick(u->nick);
		if (na != NULL)
			nc = na->GetAccount();
	}

	group.super_admin = u->super_admin;
	group.founder = IsFounder(u);
	group.ci = this;
	group.nc = nc;

	for (unsigned i = 0, end = this->GetAccessCount(); i < end; ++i)
	{
		ChanServ::ChanAccess *a = this->GetAccess(i);
		if (a->Matches(u, u->Account(), group.path))
			group.push_back(a);
	}

	if (group.founder || !group.empty())
	{
		this->SetLastUsed(Anope::CurTime);

		for (unsigned i = 0; i < group.size(); ++i)
			group[i]->SetLastSeen(Anope::CurTime);
	}

	return group;
}

ChanServ::AccessGroup ChannelImpl::AccessFor(NickServ::Account *nc)
{
	ChanServ::AccessGroup group;

	group.founder = GetFounder() && GetFounder() == nc;
	group.ci = this;
	group.nc = nc;

	for (unsigned i = 0, end = this->GetAccessCount(); i < end; ++i)
	{
		ChanServ::ChanAccess *a = this->GetAccess(i);
		if (a->Matches(NULL, nc, group.path))
			group.push_back(a);
	}

	if (group.founder || !group.empty())
		this->SetLastUsed(Anope::CurTime);

	/* don't update access last seen here, this isn't the user requesting access */

	return group;
}

unsigned ChannelImpl::GetAccessCount()
{
	return GetRefs<ChanServ::ChanAccess *>(ChanServ::chanaccess).size();
}

unsigned ChannelImpl::GetDeepAccessCount() const
{
	return 0;
#if 0
	ChanServ::ChanAccess::Path path;
	for (unsigned i = 0, end = this->GetAccessCount(); i < end; ++i)
	{
		ChanServ::ChanAccess *a = this->GetAccess(i);
		a->Matches(NULL, NULL, path);
	}

	unsigned count = this->GetAccessCount();
	std::set<const ChanServ::Channel *> channels;
	channels.insert(this);
	for (ChanServ::ChanAccess::Set::iterator it = path.first.begin(); it != path.first.end(); ++it)
	{
		const ChanServ::Channel *ci = it->first->GetChannel();
		if (!channels.count(ci))
		{
			channels.count(ci);
			count += ci->GetAccessCount();
		}
	}
	return count;
#endif
}

void ChannelImpl::ClearAccess()
{
	for (ChanServ::ChanAccess *a : GetRefs<ChanServ::ChanAccess *>(ChanServ::chanaccess))
		a->Delete();
}

AutoKick *ChannelImpl::AddAkick(const Anope::string &user, NickServ::Account *akicknc, const Anope::string &reason, time_t t, time_t lu)
{
	AutoKick *ak = ::autokick.Create();
	ak->SetChannel(this);
	ak->SetAccount(akicknc);
	ak->SetReason(reason);
	ak->SetCreator(user);
	ak->SetAddTime(t);
	ak->SetLastUsed(lu);

	return ak;
}

AutoKick *ChannelImpl::AddAkick(const Anope::string &user, const Anope::string &mask, const Anope::string &reason, time_t t, time_t lu)
{
	AutoKick *ak = ::autokick.Create();
	ak->SetChannel(this);
	ak->SetMask(mask);
	ak->SetReason(reason);
	ak->SetCreator(user);
	ak->SetAddTime(t);
	ak->SetLastUsed(lu);

	return ak;
}

AutoKick *ChannelImpl::GetAkick(unsigned index)
{
	std::vector<AutoKick *> a = GetRefs<AutoKick *>(autokick);
	return a.size() > index ? a[index] : nullptr;
}

unsigned ChannelImpl::GetAkickCount()
{
	std::vector<AutoKick *> t = GetRefs<AutoKick *>(autokick);
	return t.size();
}

void ChannelImpl::ClearAkick()
{
	for (AutoKick *ak : GetRefs<AutoKick *>(autokick))
		ak->Delete();
}

int16_t ChannelImpl::GetLevel(const Anope::string &priv)
{
	for (ChanServ::Level *l : this->GetRefs<ChanServ::Level *>(ChanServ::level))
		if (l->GetName() == priv)
			return l->GetLevel();

	ChanServ::Privilege *p = ChanServ::service ? ChanServ::service->FindPrivilege(priv) : nullptr;
	if (!p)
	{
		Log(LOG_DEBUG) << "Unknown privilege " + priv;
		return ChanServ::ACCESS_INVALID;
	}

	return p->level;
}

void ChannelImpl::SetLevel(const Anope::string &priv, int16_t level)
{
	for (ChanServ::Level *l : this->GetRefs<ChanServ::Level *>(ChanServ::level))
		if (l->GetName() == priv)
		{
			l->SetLevel(level);
			return;
		}

	ChanServ::Level *l = ChanServ::level.Create();
	l->SetChannel(this);
	l->SetName(priv);
	l->SetLevel(level);
}

void ChannelImpl::RemoveLevel(const Anope::string &priv)
{
	for (ChanServ::Level *l : this->GetRefs<ChanServ::Level *>(ChanServ::level))
		if (l->GetName() == priv)
		{
			l->Delete();
			return;
		}
}

void ChannelImpl::ClearLevels()
{
	for (ChanServ::Level *l : this->GetRefs<ChanServ::Level *>(ChanServ::level))
		l->Delete();
}

Anope::string ChannelImpl::GetIdealBan(User *u)
{
	int bt = this->GetBanType();
	switch (bt)
	{
		case 0:
			return "*!" + u->GetVIdent() + "@" + u->GetDisplayedHost();
		case 1:
			if (u->GetVIdent()[0] == '~')
				return "*!*" + u->GetVIdent() + "@" + u->GetDisplayedHost();
			else
				return "*!" + u->GetVIdent() + "@" + u->GetDisplayedHost();
		case 3:
			return "*!" + u->Mask();
		case 2:
		default:
			return "*!*@" + u->GetDisplayedHost();
	}
}

bool ChannelImpl::IsFounder(const User *user)
{
	if (!user)
		return false;

	if (user->super_admin)
		return true;

	if (user->Account() && user->Account() == this->GetFounder())
		return true;

	return false;
}

