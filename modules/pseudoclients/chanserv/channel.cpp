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
#include "channel.h"
#include "modules/cs_akick.h"

ChannelImpl::ChannelImpl(const Anope::string &chname)
{
	if (chname.empty())
		throw CoreException("Empty channel passed to ChanServ::Channel constructor");

	this->founder = NULL;
	this->successor = NULL;
	this->c = ::Channel::Find(chname);
	if (this->c)
		this->c->ci = this;
	this->banexpire = 0;
	this->bi = NULL;
	this->last_topic_time = 0;

	this->name = chname;

	this->bantype = 2;
	this->last_used = this->time_registered = Anope::CurTime;

	ChanServ::registered_channel_map& map = ChanServ::service->GetChannels();
	Channel* &ci = map[chname];
	if (ci)
		Log(LOG_DEBUG) << "Duplicate channel " << this->name << " in registered channel table?";
	ci = this;

	Event::OnCreateChan(&Event::CreateChan::OnCreateChan, this);
}

ChannelImpl::ChannelImpl(const ChanServ::Channel &ci)
{
	*this = ci;

	if (this->founder)
		--this->founder->channelcount;

	this->access->clear();
	this->akick->clear();

	for (unsigned i = 0; i < ci.GetAccessCount(); ++i)
	{
		const ChanServ::ChanAccess *taccess = ci.GetAccess(i);
		ChanServ::AccessProvider *provider = taccess->provider;

		ChanServ::ChanAccess *newaccess = provider->Create();
		newaccess->SetMask(taccess->Mask(), this);
		newaccess->creator = taccess->creator;
		newaccess->last_seen = taccess->last_seen;
		newaccess->created = taccess->created;
		newaccess->AccessUnserialize(taccess->AccessSerialize());

		this->AddAccess(newaccess);
	}

	for (unsigned i = 0; i < ci.GetAkickCount(); ++i)
	{
		const AutoKick *takick = ci.GetAkick(i);
		if (takick->nc)
			this->AddAkick(takick->creator, takick->nc, takick->reason, takick->addtime, takick->last_used);
		else
			this->AddAkick(takick->creator, takick->mask, takick->reason, takick->addtime, takick->last_used);
	}

	Event::OnCreateChan(&Event::CreateChan::OnCreateChan, this);
}

ChannelImpl::~ChannelImpl()
{
	Event::OnDelChan(&Event::DelChan::OnDelChan, this);

	Log(LOG_DEBUG) << "Deleting channel " << this->name;

	if (this->c)
	{
		if (this->bi && this->c->FindUser(this->bi))
			this->bi->Part(this->c);

		/* Parting the service bot can cause the channel to go away */

		if (this->c)
		{
			if (this->c && this->c->CheckDelete())
				delete this->c;

			this->c = NULL;
		}
	}

	ChanServ::registered_channel_map& map = ChanServ::service->GetChannels();
	map.erase(this->name);

	this->SetFounder(NULL);
	this->SetSuccessor(NULL);

	this->ClearAccess();
	this->ClearAkick();

	if (this->memos)
	{
		for (unsigned i = 0, end = this->memos->memos->size(); i < end; ++i)
			delete this->memos->GetMemo(i);
		this->memos->memos->clear();
		delete memos;
	}

	if (this->founder)
		--this->founder->channelcount;
}

void ChannelImpl::Serialize(Serialize::Data &data) const
{
	data["name"] << this->name;
	if (this->founder)
		data["founder"] << this->founder->display;
	if (this->successor)
		data["successor"] << this->successor->display;
	data["description"] << this->desc;
	data.SetType("time_registered", Serialize::Data::DT_INT); data["time_registered"] << this->time_registered;
	data.SetType("last_used", Serialize::Data::DT_INT); data["last_used"] << this->last_used;
	data["last_topic"] << this->last_topic;
	data["last_topic_setter"] << this->last_topic_setter;
	data.SetType("last_topic_time", Serialize::Data::DT_INT); data["last_topic_time"] << this->last_topic_time;
	data.SetType("bantype", Serialize::Data::DT_INT); data["bantype"] << this->bantype;
	{
		Anope::string levels_buffer;
		for (Anope::map<int16_t>::const_iterator it = this->levels.begin(), it_end = this->levels.end(); it != it_end; ++it)
			levels_buffer += it->first + " " + stringify(it->second) + " ";
		data["levels"] << levels_buffer;
	}
	if (this->bi)
		data["bi"] << this->bi->nick;
	data.SetType("banexpire", Serialize::Data::DT_INT); data["banexpire"] << this->banexpire;
	if (memos)
	{
		data["memomax"] << this->memos->memomax;
		for (unsigned i = 0; i < this->memos->ignores.size(); ++i)
			data["memoignores"] << this->memos->ignores[i] << " ";
	}

	Extensible::ExtensibleSerialize(this, this, data);
}

Serializable* ChannelImpl::Unserialize(Serializable *obj, Serialize::Data &data)
{
	Anope::string sname, sfounder, ssuccessor, slevels, sbi;

	data["name"] >> sname;
	data["founder"] >> sfounder;
	data["successor"] >> ssuccessor;
	data["levels"] >> slevels;
	data["bi"] >> sbi;

	ChannelImpl *ci;
	if (obj)
		ci = anope_dynamic_static_cast<ChannelImpl *>(obj);
	else
		ci = new ChannelImpl(sname);

	ci->SetFounder(NickServ::FindAccount(sfounder));
	ci->SetSuccessor(NickServ::FindAccount(ssuccessor));

	data["description"] >> ci->desc;
	data["time_registered"] >> ci->time_registered;
	data["last_used"] >> ci->last_used;
	data["last_topic"] >> ci->last_topic;
	data["last_topic_setter"] >> ci->last_topic_setter;
	data["last_topic_time"] >> ci->last_topic_time;
	data["bantype"] >> ci->bantype;
	{
		std::vector<Anope::string> v;
		spacesepstream(slevels).GetTokens(v);
		for (unsigned i = 0; i + 1 < v.size(); i += 2)
			try
			{
				ci->levels[v[i]] = convertTo<int16_t>(v[i + 1]);
			}
			catch (const ConvertException &) { }
	}
	BotInfo *bi = BotInfo::Find(sbi, true);
	if (*ci->bi != bi)
	{
		if (bi)
			bi->Assign(NULL, ci);
		else if (ci->bi)
			ci->bi->UnAssign(NULL, ci);
	}
	data["banexpire"] >> ci->banexpire;
	if (ci->memos)
	{
		data["memomax"] >> ci->memos->memomax;
		{
			Anope::string buf;
			data["memoignores"] >> buf;
			spacesepstream sep(buf);
			ci->memos->ignores.clear();
			while (sep.GetToken(buf))
				ci->memos->ignores.push_back(buf);
		}
	}

	Extensible::ExtensibleUnserialize(ci, ci, data);

	/* compat */
	bool b;
	b = false;
	data["extensible:SECURE"] >> b;
	if (b)
		ci->Extend<bool>("CS_SECURE");
	b = false;
	data["extensible:PRIVATE"] >> b;
	if (b)
		ci->Extend<bool>("CS_PRIVATE");
	b = false;
	data["extensible:NO_EXPIRE"] >> b;
	if (b)
		ci->Extend<bool>("CS_NO_EXPIRE");
	b = false;
	data["extensible:FANTASY"] >> b;
	if (b)
		ci->Extend<bool>("BS_FANTASY");
	b = false;
	data["extensible:GREET"] >> b;
	if (b)
		ci->Extend<bool>("BS_GREET");
	b = false;
	data["extensible:PEACE"] >> b;
	if (b)
		ci->Extend<bool>("PEACE");
	b = false;
	data["extensible:SECUREFOUNDER"] >> b;
	if (b)
		ci->Extend<bool>("SECUREFOUNDER");
	b = false;
	data["extensible:RESTRICTED"] >> b;
	if (b)
		ci->Extend<bool>("RESTRICTED");
	b = false;
	data["extensible:KEEPTOPIC"] >> b;
	if (b)
		ci->Extend<bool>("KEEPTOPIC");
	b = false;
	data["extensible:SIGNKICK"] >> b;
	if (b)
		ci->Extend<bool>("SIGNKICK");
	b = false;
	data["extensible:SIGNKICK_LEVEL"] >> b;
	if (b)
		ci->Extend<bool>("SIGNKICK_LEVEL");
	/* end compat */

	return ci;
}


void ChannelImpl::SetFounder(NickServ::Account *nc)
{
	if (this->founder)
	{
		--this->founder->channelcount;
		this->founder->RemoveChannelReference(this);
	}

	this->founder = nc;

	if (this->founder)
	{
		++this->founder->channelcount;
		this->founder->AddChannelReference(this);
	}
}

NickServ::Account *ChannelImpl::GetFounder() const
{
	return this->founder;
}

void ChannelImpl::SetSuccessor(NickServ::Account *nc)
{
	if (this->successor)
		this->successor->RemoveChannelReference(this);
	this->successor = nc;
	if (this->successor)
		this->successor->AddChannelReference(this);
}

NickServ::Account *ChannelImpl::GetSuccessor() const
{
	return this->successor;
}

BotInfo *ChannelImpl::WhoSends() const
{
	if (this && this->bi)
		return this->bi;

	BotInfo *ChanServ = Config->GetClient("ChanServ");
	if (ChanServ)
		return ChanServ;

	if (!BotListByNick->empty())
		return BotListByNick->begin()->second;

	return NULL;
}

void ChannelImpl::AddAccess(ChanServ::ChanAccess *taccess)
{
	this->access->push_back(taccess);
}

ChanServ::ChanAccess *ChannelImpl::GetAccess(unsigned index) const
{
	if (this->access->empty() || index >= this->access->size())
		return NULL;

	ChanServ::ChanAccess *acc = (*this->access)[index];
	acc->QueueUpdate();
	return acc;
}

ChanServ::AccessGroup ChannelImpl::AccessFor(const User *u)
{
	ChanServ::AccessGroup group;

	if (u == NULL)
		return group;

	const NickServ::Account *nc = u->Account();
	if (nc == NULL && !this->HasExt("NS_SECURE") && u->IsRecognized())
	{
		const NickServ::Nick *na = NickServ::FindNick(u->nick);
		if (na != NULL)
			nc = na->nc;
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
		this->last_used = Anope::CurTime;

		for (unsigned i = 0; i < group.size(); ++i)
			group[i]->last_seen = Anope::CurTime;
	}

	return group;
}

ChanServ::AccessGroup ChannelImpl::AccessFor(const NickServ::Account *nc)
{
	ChanServ::AccessGroup group;

	group.founder = (this->founder && this->founder == nc);
	group.ci = this;
	group.nc = nc;

	for (unsigned i = 0, end = this->GetAccessCount(); i < end; ++i)
	{
		ChanServ::ChanAccess *a = this->GetAccess(i);
		if (a->Matches(NULL, nc, group.path))
			group.push_back(a);
	}

	if (group.founder || !group.empty())
		this->last_used = Anope::CurTime;

		/* don't update access last seen here, this isn't the user requesting access */

	return group;
}

unsigned ChannelImpl::GetAccessCount() const
{
	return this->access->size();
}

unsigned ChannelImpl::GetDeepAccessCount() const
{
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
		const ChanServ::Channel *ci = it->first->ci;
		if (!channels.count(ci))
		{
			channels.count(ci);
			count += ci->GetAccessCount();
		}
	}
	return count;
}

ChanServ::ChanAccess *ChannelImpl::EraseAccess(unsigned index)
{
	if (this->access->empty() || index >= this->access->size())
		return NULL;

	ChanServ::ChanAccess *ca = this->access->at(index);
	this->access->erase(this->access->begin() + index);
	return ca;
}

void ChannelImpl::ClearAccess()
{
	for (unsigned i = this->access->size(); i > 0; --i)
		delete this->GetAccess(i - 1);
}

AutoKick *ChannelImpl::AddAkick(const Anope::string &user, NickServ::Account *akicknc, const Anope::string &reason, time_t t, time_t lu)
{
	if (!::akick)
		return nullptr;

	AutoKick *autokick = ::akick->Create();
	autokick->ci = this;
	autokick->nc = akicknc;
	autokick->reason = reason;
	autokick->creator = user;
	autokick->addtime = t;
	autokick->last_used = lu;

	this->akick->push_back(autokick);

	akicknc->AddChannelReference(this);

	return autokick;
}

AutoKick *ChannelImpl::AddAkick(const Anope::string &user, const Anope::string &mask, const Anope::string &reason, time_t t, time_t lu)
{
	if (!::akick)
		return nullptr;

	AutoKick *autokick = ::akick->Create();
	autokick->ci = this;
	autokick->mask = mask;
	autokick->nc = NULL;
	autokick->reason = reason;
	autokick->creator = user;
	autokick->addtime = t;
	autokick->last_used = lu;

	this->akick->push_back(autokick);

	return autokick;
}

AutoKick *ChannelImpl::GetAkick(unsigned index) const
{
	if (this->akick->empty() || index >= this->akick->size())
		return NULL;

	AutoKick *ak = (*this->akick)[index];
	ak->QueueUpdate();
	return ak;
}

unsigned ChannelImpl::GetAkickCount() const
{
	return this->akick->size();
}

void ChannelImpl::EraseAkick(unsigned index)
{
	if (this->akick->empty() || index >= this->akick->size())
		return;

	delete this->GetAkick(index);
}

void ChannelImpl::ClearAkick()
{
	while (!this->akick->empty())
		delete this->akick->back();
}

int16_t ChannelImpl::GetLevel(const Anope::string &priv) const
{
	if (!ChanServ::service || !ChanServ::service->FindPrivilege(priv))
	{
		Log(LOG_DEBUG) << "Unknown privilege " + priv;
		return ChanServ::ACCESS_INVALID;
	}

	Anope::map<int16_t>::const_iterator it = this->levels.find(priv);
	if (it == this->levels.end())
		return 0;
	return it->second;
}

void ChannelImpl::SetLevel(const Anope::string &priv, int16_t level)
{
	this->levels[priv] = level;
}

void ChannelImpl::RemoveLevel(const Anope::string &priv)
{
	this->levels.erase(priv);
}

void ChannelImpl::ClearLevels()
{
	this->levels.clear();
}

Anope::string ChannelImpl::GetIdealBan(User *u) const
{
	int bt = this ? this->bantype : -1;
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


void ChannelImpl::AddChannelReference(const Anope::string &what)
{
	++references[what];
}

void ChannelImpl::RemoveChannelReference(const Anope::string &what)
{
	int &i = references[what];
	if (--i <= 0)
		references.erase(what);
}

void ChannelImpl::GetChannelReferences(std::deque<Anope::string> &chans)
{
	chans.clear();
	for (Anope::map<int>::iterator it = references.begin(); it != references.end(); ++it)
		chans.push_back(it->first);
}
