/* Registered channel functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#include "services.h"
#include "modules.h"
#include "regchannel.h"
#include "account.h"
#include "access.h"
#include "channels.h"
#include "config.h"
#include "bots.h"
#include "language.h"
#include "servers.h"
#include "chanserv.h"

Serialize::Checker<registered_channel_map> RegisteredChannelList("ChannelInfo");

static const Anope::string ChannelInfoFlagStrings[] = {
	"BEGIN", "KEEPTOPIC", "SECUREOPS", "PRIVATE", "TOPICLOCK", "RESTRICTED",
	"PEACE", "SECURE", "NO_EXPIRE", "MEMO_HARDMAX", "SECUREFOUNDER",
	"SIGNKICK", "SIGNKICK_LEVEL", "SUSPENDED", "PERSIST", "STATS", "NOAUTOOP", ""
};
template<> const Anope::string* Flags<ChannelInfoFlag>::flags_strings = ChannelInfoFlagStrings;

static const Anope::string AutoKickFlagString[] = { "AK_ISNICK", "" };
template<> const Anope::string* Flags<AutoKickFlag>::flags_strings = AutoKickFlagString;

Serialize::Data BadWord::Serialize() const
{
	Serialize::Data data;

	data["ci"].SetMax(64)/*XXX*/ << this->ci->name;
	data["word"].SetMax(512) << this->word;
	data["type"].SetType(Serialize::DT_INT) << this->type;

	return data;
}

Serializable* BadWord::Unserialize(Serializable *obj, Serialize::Data &data)
{
	ChannelInfo *ci = ChannelInfo::Find(data["ci"].astr());
	if (!ci)
		return NULL;

	unsigned int n;
	data["type"] >> n;
	
	BadWord *bw;
	if (obj)
	{
		bw = anope_dynamic_static_cast<BadWord *>(obj);
		data["word"] >> bw->word;
		bw->type = static_cast<BadWordType>(n);
	}
	else
		bw = ci->AddBadWord(data["word"].astr(), static_cast<BadWordType>(n));
	
	return bw;
}

AutoKick::AutoKick() : Serializable("AutoKick")
{
}

Serialize::Data AutoKick::Serialize() const
{
	Serialize::Data data;

	data["ci"].SetMax(64)/*XXX*/ << this->ci->name;
	if (this->HasFlag(AK_ISNICK) && this->nc)
		data["nc"].SetMax(Config->NickLen) << this->nc->display;
	else
		data["mask"].SetMax(Config->NickLen) << this->mask;
	data["reason"] << this->reason;
	data["creator"] << this->creator;
	data["addtime"].SetType(Serialize::DT_INT) << this->addtime;
	data["last_used"].SetType(Serialize::DT_INT) << this->last_used;
	data["flags"] << this->ToString();

	return data;
}

Serializable* AutoKick::Unserialize(Serializable *obj, Serialize::Data &data)
{
	ChannelInfo *ci = ChannelInfo::Find(data["ci"].astr());
	if (!ci)
		return NULL;
	
	AutoKick *ak;
	NickCore *nc = NickCore::Find(data["nc"].astr());
	if (obj)
	{
		ak = anope_dynamic_static_cast<AutoKick *>(obj);
		data["creator"] >> ak->creator;
		data["reason"] >> ak->reason;
		ak->nc = NickCore::Find(data["nc"].astr());
		data["mask"] >> ak->mask;
		data["addtime"] >> ak->addtime;
		data["last_used"] >> ak->last_used;
	}
	else
	{
		time_t addtime, lastused;
		data["addtime"] >> addtime;
		data["last_used"] >> lastused;

		if (nc)	
			ak = ci->AddAkick(data["creator"].astr(), nc, data["reason"].astr(), addtime, lastused);
		else
			ak = ci->AddAkick(data["creator"].astr(), data["mask"].astr(), data["reason"].astr(), addtime, lastused);
	}

	return ak;
}

ModeLock::ModeLock(ChannelInfo *ch, bool s, ChannelModeName n, const Anope::string &p, const Anope::string &se, time_t c) : Serializable("ModeLock"), ci(ch), set(s), name(n), param(p), setter(se), created(c)
{
}

Serialize::Data ModeLock::Serialize() const
{
	Serialize::Data data;

	if (!this->ci)
		return data;

	const Anope::string* ChannelModeNameStrings = Flags<ChannelModeName>::GetFlagStrings();
	data["ci"].SetMax(64)/*XXX*/ << this->ci->name;
	data["set"].SetMax(5) << this->set;
	data["name"].SetMax(64) << ChannelModeNameStrings[this->name];
	data["param"].SetMax(512) << this->param;
	data["setter"] << this->setter;
	data["created"].SetType(Serialize::DT_INT) << this->created;

	return data;
}

Serializable* ModeLock::Unserialize(Serializable *obj, Serialize::Data &data)
{
	ChannelInfo *ci = ChannelInfo::Find(data["ci"].astr());
	if (!ci)
		return NULL;

	ChannelModeName name = CMODE_END;

	const Anope::string* ChannelModeNameStrings = Flags<ChannelModeName>::GetFlagStrings();
	for (unsigned i = 0; !ChannelModeNameStrings[i].empty(); ++i)
		if (ChannelModeNameStrings[i] == data["name"].astr())
		{
			name = static_cast<ChannelModeName>(i);
			break;
		}
	if (name == CMODE_END)
		return NULL;
	
	ModeLock *ml;
	if (obj)
	{
		ml = anope_dynamic_static_cast<ModeLock *>(obj);

		data["set"] >> ml->set;
		ml->name = name;
		data["param"] >> ml->param;
		data["setter"] >> ml->setter;
		data["created"] >> ml->created;
		return ml;
	}
	else
	{
		bool set;
		data["set"] >> set;

		time_t created;
		data["created"] >> created;

		ml = new ModeLock(ci, set, name, "", data["setter"].astr(), created);
		data["param"] >> ml->param;

		ci->mode_locks->insert(std::make_pair(ml->name, ml));
		return ml;
	}
}

Serialize::Data LogSetting::Serialize() const
{
	Serialize::Data data;

	if (!ci)
		return data;

	data["ci"] << ci->name;
	data["service_name"] << service_name;
	data["command_service"] << command_service;
	data["command_name"] << command_name;
	data["method"] << method;
	data["extra"] << extra;
	data["creator"] << creator;
	data["created"].SetType(Serialize::DT_INT) << created;

	return data;
}

Serializable* LogSetting::Unserialize(Serializable *obj, Serialize::Data &data)
{
	ChannelInfo *ci = ChannelInfo::Find(data["ci"].astr());
	if (ci == NULL)
		return NULL;
	
	LogSetting *ls;
	if (obj)
		ls = anope_dynamic_static_cast<LogSetting *>(obj);
	else
	{
		ls = new LogSetting();
		ci->log_settings->push_back(ls);
	}

	ls->ci = ci;
	data["service_name"] >> ls->service_name;
	data["command_service"] >> ls->command_service;
	data["command_name"] >> ls->command_name;
	data["method"] >> ls->method;
	data["extra"] >> ls->extra;
	data["creator"] >> ls->creator;
	data["created"] >> ls->created;

	return ls;
}

ChannelInfo::ChannelInfo(const Anope::string &chname) : Serializable("ChannelInfo"),
	access("ChanAccess"), akick("AutoKick"),
	badwords("BadWord"), mode_locks("ModeLock"), log_settings("LogSetting")
{
	if (chname.empty())
		throw CoreException("Empty channel passed to ChannelInfo constructor");

	this->founder = NULL;
	this->successor = NULL;
	this->c = Channel::Find(chname);
	if (this->c)
		this->c->ci = this;
	this->capsmin = this->capspercent = 0;
	this->floodlines = this->floodsecs = 0;
	this->repeattimes = 0;
	this->bi = NULL;

	this->last_topic_setter = Config->ChanServ;
	this->last_topic_time = Anope::CurTime;

	this->name = chname;

	size_t t;
	/* Set default channel flags */
	for (t = CI_BEGIN + 1; t != CI_END; ++t)
		if (Config->CSDefFlags.HasFlag(static_cast<ChannelInfoFlag>(t)))
			this->SetFlag(static_cast<ChannelInfoFlag>(t));

	/* Set default bot flags */
	for (t = BS_BEGIN + 1; t != BS_END; ++t)
		if (Config->BSDefFlags.HasFlag(static_cast<BotServFlag>(t)))
			this->botflags.SetFlag(static_cast<BotServFlag>(t));

	this->bantype = Config->CSDefBantype;
	this->memos.memomax = Config->MSMaxMemos;
	this->last_used = this->time_registered = Anope::CurTime;

	for (int i = 0; i < TTB_SIZE; ++i)
		this->ttb[i] = 0;

	size_t old = RegisteredChannelList->size();
	(*RegisteredChannelList)[this->name] = this;
	if (old == RegisteredChannelList->size())
		Log(LOG_DEBUG) << "Duplicate channel " << this->name << " in registered channel table?";

	FOREACH_MOD(I_OnCreateChan, OnCreateChan(this));
}

ChannelInfo::ChannelInfo(const ChannelInfo &ci) : Serializable("ChannelInfo"),
	access("ChanAccess"), akick("AutoKick"),
	badwords("BadWord"), mode_locks("ModeLock"), log_settings("LogSetting")
{
	*this = ci;

	if (this->founder)
		--this->founder->channelcount;

	this->access->clear();
	this->akick->clear();
	this->badwords->clear();

	for (int i = 0; i < TTB_SIZE; ++i)
		this->ttb[i] = ci.ttb[i];

	for (unsigned i = 0; i < ci.GetAccessCount(); ++i)
	{
		const ChanAccess *taccess = ci.GetAccess(i);
		AccessProvider *provider = taccess->provider;

		ChanAccess *newaccess = provider->Create();
		newaccess->ci = this;
		newaccess->mask = taccess->mask;
		newaccess->creator = taccess->creator;
		newaccess->last_seen = taccess->last_seen;
		newaccess->created = taccess->created;
		newaccess->AccessUnserialize(taccess->AccessSerialize());

		this->AddAccess(newaccess);
	}

	for (unsigned i = 0; i < ci.GetAkickCount(); ++i)
	{
		const AutoKick *takick = ci.GetAkick(i);
		if (takick->HasFlag(AK_ISNICK))
			this->AddAkick(takick->creator, takick->nc, takick->reason, takick->addtime, takick->last_used);
		else
			this->AddAkick(takick->creator, takick->mask, takick->reason, takick->addtime, takick->last_used);
	}
	for (unsigned i = 0; i < ci.GetBadWordCount(); ++i)
	{
		const BadWord *bw = ci.GetBadWord(i);
		this->AddBadWord(bw->word, bw->type);
	}

	for (unsigned i = 0; i < ci.log_settings->size(); ++i)
	{
		LogSetting *l = new LogSetting(*ci.log_settings->at(i));
		l->ci = this;
		this->log_settings->push_back(l);
	}
}

ChannelInfo::~ChannelInfo()
{
	FOREACH_MOD(I_OnDelChan, OnDelChan(this));

	Log(LOG_DEBUG) << "Deleting channel " << this->name;

	if (this->c)
	{
		if (this->bi && this->c->FindUser(this->bi))
			this->bi->Part(this->c);
		this->c->ci = NULL;
	}

	RegisteredChannelList->erase(this->name);

	this->ClearAccess();
	this->ClearAkick();
	this->ClearBadWords();

	for (unsigned i = 0; i < this->log_settings->size(); ++i)
		this->log_settings->at(i)->Destroy();
	this->log_settings->clear();

	for (ChannelInfo::ModeList::iterator it = this->mode_locks->begin(), it_end = this->mode_locks->end(); it != it_end; ++it)
		it->second->Destroy();
	this->mode_locks->clear();

	if (!this->memos.memos->empty())
	{
		for (unsigned i = 0, end = this->memos.memos->size(); i < end; ++i)
			this->memos.GetMemo(i)->Destroy();
		this->memos.memos->clear();
	}

	if (this->founder)
		--this->founder->channelcount;
}

Serialize::Data ChannelInfo::Serialize() const
{
	Serialize::Data data;

	data["name"].SetMax(255) << this->name;
	if (this->founder)
		data["founder"] << this->founder->display;
	if (this->successor)
		data["successor"] << this->successor->display;
	data["description"] << this->desc;
	data["time_registered"].SetType(Serialize::DT_INT) << this->time_registered;
	data["last_used"].SetType(Serialize::DT_INT) << this->last_used;
	data["last_topic"] << this->last_topic;
	data["last_topic_setter"] << this->last_topic_setter;
	data["last_topic_time"].SetType(Serialize::DT_INT) << this->last_topic_time;
	data["bantype"].SetType(Serialize::DT_INT) << this->bantype;
	data["flags"] << this->ToString();
	data["botflags"] << this->botflags.ToString();
	{
		Anope::string levels_buffer;
		for (std::map<Anope::string, int16_t>::const_iterator it = this->levels.begin(), it_end = this->levels.end(); it != it_end; ++it)
			levels_buffer += it->first + " " + stringify(it->second) + " ";
		data["levels"] << levels_buffer;
	}
	if (this->bi)
		data["bi"] << this->bi->nick;
	for (int i = 0; i < TTB_SIZE; ++i)
		data["ttb"] << this->ttb[i] << " ";
	data["capsmin"].SetType(Serialize::DT_INT) << this->capsmin;
	data["capspercent"].SetType(Serialize::DT_INT) << this->capspercent;
	data["floodlines"].SetType(Serialize::DT_INT) << this->floodlines;
	data["floodsecs"].SetType(Serialize::DT_INT) << this->floodsecs;
	data["repeattimes"].SetType(Serialize::DT_INT) << this->repeattimes;
	data["memomax"] << this->memos.memomax;
	for (unsigned i = 0; i < this->memos.ignores.size(); ++i)
		data["memoignores"] << this->memos.ignores[i] << " ";

	return data;
}

Serializable* ChannelInfo::Unserialize(Serializable *obj, Serialize::Data &data)
{
	ChannelInfo *ci;
	if (obj)
		ci = anope_dynamic_static_cast<ChannelInfo *>(obj);
	else
		ci = new ChannelInfo(data["name"].astr());

	if (data.count("founder") > 0)
	{
		if (ci->founder)
			--ci->founder->channelcount;
		ci->founder = NickCore::Find(data["founder"].astr());
		if (ci->founder)
			++ci->founder->channelcount;
	}
	if (data.count("successor") > 0)
	{
		ci->successor = NickCore::Find(data["successor"].astr());
		if (ci->founder && *ci->founder == *ci->successor)
			ci->successor = NULL;
	}
	data["description"] >> ci->desc;
	data["time_registered"] >> ci->time_registered;
	data["last_used"] >> ci->last_used;
	data["last_topic"] >> ci->last_topic;
	data["last_topic_setter"] >> ci->last_topic_setter;
	data["last_topic_time"] >> ci->last_topic_time;
	data["bantype"] >> ci->bantype;
	ci->FromString(data["flags"].astr());
	ci->botflags.FromString(data["botflags"].astr());
	{
		std::vector<Anope::string> v;
		spacesepstream(data["levels"].astr()).GetTokens(v);
		for (unsigned i = 0; i + 1 < v.size(); i += 2)
			ci->levels[v[i]] = convertTo<int16_t>(v[i + 1]);
	}
	BotInfo *bi = BotInfo::Find(data["bi"].astr());
	if (*ci->bi != bi)
	{
		if (ci->bi)
			ci->bi->UnAssign(NULL, ci);
		ci->bi = bi;
		if (ci->bi)
			ci->bi->Assign(NULL, ci);
	}
	data["capsmin"] >> ci->capsmin;
	data["capspercent"] >> ci->capspercent;
	data["floodlines"] >> ci->floodlines;
	data["floodsecs"] >> ci->floodsecs;
	data["repeattimes"] >> ci->repeattimes;
	data["memomax"] >> ci->memos.memomax;
	{
		Anope::string buf;
		data["memoignores"] >> buf;
		spacesepstream sep(buf);
		ci->memos.ignores.clear();
		while (sep.GetToken(buf))
			ci->memos.ignores.push_back(buf);
	}

	return ci;
}


void ChannelInfo::SetFounder(NickCore *nc)
{
	if (this->founder)
		--this->founder->channelcount;
	this->founder = nc;
	if (this->founder)
		++this->founder->channelcount;
	if (*this->founder == *this->successor)
		this->successor = NULL;
}

NickCore *ChannelInfo::GetFounder() const
{
	return this->founder;
}

BotInfo *ChannelInfo::WhoSends() const
{
	if (this && this->bi)
		return this->bi;
	else if (ChanServ)
		return ChanServ;
	else if (!BotListByNick->empty())
		return BotListByNick->begin()->second;
	return NULL;
}

void ChannelInfo::AddAccess(ChanAccess *taccess)
{
	this->access->push_back(taccess);
}

ChanAccess *ChannelInfo::GetAccess(unsigned index) const
{
	if (this->access->empty() || index >= this->access->size())
		return NULL;

	ChanAccess *acc = (*this->access)[index];
	acc->QueueUpdate();
	return acc;
}

AccessGroup ChannelInfo::AccessFor(const User *u)
{
	AccessGroup group;

	if (u == NULL)
		return group;

	const NickCore *nc = u->Account();
	if (nc == NULL && u->IsRecognized())
	{
		const NickAlias *na = NickAlias::Find(u->nick);
		if (na != NULL)
			nc = na->nc;
	}

	group.super_admin = u->super_admin;
	group.founder = IsFounder(u, this);
	group.ci = this;
	group.nc = nc;

	for (unsigned i = 0, end = this->GetAccessCount(); i < end; ++i)
		if (this->GetAccess(i)->Matches(u, nc))
			group.push_back(this->GetAccess(i));

	if (group.founder || !group.empty())
	{
		this->last_used = Anope::CurTime;

		for (unsigned i = 0; i < group.size(); ++i)
			group[i]->last_seen = Anope::CurTime;
	}

	return group;
}

AccessGroup ChannelInfo::AccessFor(const NickCore *nc)
{
	AccessGroup group;

	group.founder = (this->founder && this->founder == nc);
	group.ci = this;
	group.nc = nc;

	for (unsigned i = 0, end = this->GetAccessCount(); i < end; ++i)
		if (this->GetAccess(i)->Matches(NULL, nc))
			group.push_back(this->GetAccess(i));
	
	if (group.founder || !group.empty())
	{
		this->last_used = Anope::CurTime;

		for (unsigned i = 0; i < group.size(); ++i)
			group[i]->last_seen = Anope::CurTime;
	}

	return group;
}

unsigned ChannelInfo::GetAccessCount() const
{
	return this->access->size();
}

void ChannelInfo::EraseAccess(unsigned index)
{
	if (this->access->empty() || index >= this->access->size())
		return;

	this->access->at(index)->Destroy();
	this->access->erase(this->access->begin() + index);
}

void ChannelInfo::EraseAccess(const ChanAccess *taccess)
{
	for (unsigned i = 0, end = this->access->size(); i < end; ++i)
	{
		if (this->GetAccess(i) == taccess)
		{
			this->EraseAccess(i);
			break;
		}
	}
}

void ChannelInfo::ClearAccess()
{
	for (unsigned i = this->access->size(); i > 0; --i)
		this->GetAccess(i - 1)->Destroy();
	this->access->clear();
}

AutoKick *ChannelInfo::AddAkick(const Anope::string &user, NickCore *akicknc, const Anope::string &reason, time_t t, time_t lu)
{
	AutoKick *autokick = new AutoKick();
	autokick->ci = this;
	autokick->SetFlag(AK_ISNICK);
	autokick->nc = akicknc;
	autokick->reason = reason;
	autokick->creator = user;
	autokick->addtime = t;
	autokick->last_used = lu;

	this->akick->push_back(autokick);

	return autokick;
}

AutoKick *ChannelInfo::AddAkick(const Anope::string &user, const Anope::string &mask, const Anope::string &reason, time_t t, time_t lu)
{
	AutoKick *autokick = new AutoKick();
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

AutoKick *ChannelInfo::GetAkick(unsigned index) const
{
	if (this->akick->empty() || index >= this->akick->size())
		return NULL;

	AutoKick *ak = (*this->akick)[index];
	ak->QueueUpdate();
	return ak;
}

unsigned ChannelInfo::GetAkickCount() const
{
	return this->akick->size();
}

void ChannelInfo::EraseAkick(unsigned index)
{
	if (this->akick->empty() || index >= this->akick->size())
		return;
	
	this->GetAkick(index)->Destroy();
	this->akick->erase(this->akick->begin() + index);
}

void ChannelInfo::ClearAkick()
{
	while (!this->akick->empty())
		EraseAkick(0);
}

BadWord* ChannelInfo::AddBadWord(const Anope::string &word, BadWordType type)
{
	BadWord *bw = new BadWord();
	bw->ci = this;
	bw->word = word;
	bw->type = type;

	this->badwords->push_back(bw);

	FOREACH_MOD(I_OnBadWordAdd, OnBadWordAdd(this, bw));

	return bw;
}

BadWord* ChannelInfo::GetBadWord(unsigned index) const
{
	if (this->badwords->empty() || index >= this->badwords->size())
		return NULL;

	BadWord *bw = (*this->badwords)[index];
	bw->QueueUpdate();
	return bw;
}

unsigned ChannelInfo::GetBadWordCount() const
{
	return this->badwords->size();
}

void ChannelInfo::EraseBadWord(unsigned index)
{
	if (this->badwords->empty() || index >= this->badwords->size())
		return;
	
	FOREACH_MOD(I_OnBadWordDel, OnBadWordDel(this, (*this->badwords)[index]));

	delete (*this->badwords)[index];
	this->badwords->erase(this->badwords->begin() + index);
}

void ChannelInfo::ClearBadWords()
{
	while (!this->badwords->empty())
		EraseBadWord(0);
}

bool ChannelInfo::HasMLock(ChannelMode *mode, const Anope::string &param, bool status) const
{
	std::multimap<ChannelModeName, ModeLock *>::const_iterator it = this->mode_locks->find(mode->name);

	if (it != this->mode_locks->end())
	{
		if (mode->type != MODE_REGULAR)
		{
			std::multimap<ChannelModeName, ModeLock *>::const_iterator it_end = this->mode_locks->upper_bound(mode->name);

			for (; it != it_end; ++it)
			{
				const ModeLock *ml = it->second;
				if (ml->param == param)
					return true;
			}
		}
		else
			return it->second->set == status;
	}
	return false;
}

bool ChannelInfo::SetMLock(ChannelMode *mode, bool status, const Anope::string &param, Anope::string setter, time_t created)
{
	if (setter.empty())
		setter = this->founder ? this->founder->display : "Unknown";
	std::pair<ChannelModeName, ModeLock *> ml = std::make_pair(mode->name, new ModeLock(this, status, mode->name, param, setter, created));

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnMLock, OnMLock(this, ml.second));
	if (MOD_RESULT == EVENT_STOP)
		return false;

	/* First, remove this */
	if (mode->type == MODE_REGULAR || mode->type == MODE_PARAM)
	{
		ChannelInfo::ModeList::const_iterator it = this->mode_locks->find(mode->name);
		if (it != this->mode_locks->end())
		{
			ChannelInfo::ModeList::const_iterator it_end = this->mode_locks->upper_bound(mode->name);
			for (; it != it_end; ++it)
				it->second->Destroy();
		}
		this->mode_locks->erase(mode->name);
	}
	else
	{
		// For list or status modes, we must check the parameter
		ChannelInfo::ModeList::iterator it = this->mode_locks->find(mode->name);
		if (it != this->mode_locks->end())
		{
			ChannelInfo::ModeList::iterator it_end = this->mode_locks->upper_bound(mode->name);
			for (; it != it_end; ++it)
			{
				const ModeLock *modelock = it->second;
				if (modelock->param == param)
				{
					it->second->Destroy();
					this->mode_locks->erase(it);
					break;
				}
			}
		}
	}

	this->mode_locks->insert(ml);

	return true;
}

bool ChannelInfo::RemoveMLock(ChannelMode *mode, bool status, const Anope::string &param)
{
	if (mode->type == MODE_REGULAR || mode->type == MODE_PARAM)
	{
		ChannelInfo::ModeList::iterator it = this->mode_locks->find(mode->name), it_end = this->mode_locks->upper_bound(mode->name), it_next = it;
		if (it != this->mode_locks->end())
			for (; it != it_end; it = it_next)
			{
				const ModeLock *ml = it->second;
				++it_next;

				if (status != ml->set)
					continue;

				EventReturn MOD_RESULT;
				FOREACH_RESULT(I_OnUnMLock, OnUnMLock(this, it->second));
				if (MOD_RESULT != EVENT_STOP)
				{
					it->second->Destroy();
					this->mode_locks->erase(it);
					return true;
				}
			}
		return false;
	}
	else
	{
		// For list or status modes, we must check the parameter
		ChannelInfo::ModeList::iterator it = this->mode_locks->find(mode->name);
		if (it != this->mode_locks->end())
		{
			ChannelInfo::ModeList::iterator it_end = this->mode_locks->upper_bound(mode->name);
			for (; it != it_end; ++it)
			{
				const ModeLock *ml = it->second;
				if (ml->set == status && ml->param == param)
				{
					EventReturn MOD_RESULT;
					FOREACH_RESULT(I_OnUnMLock, OnUnMLock(this, it->second));
					if (MOD_RESULT == EVENT_STOP)
						return false;
					it->second->Destroy();
					this->mode_locks->erase(it);
					return true;
				}
			}
		}

		return false;
	}
}

void ChannelInfo::ClearMLock()
{
	this->mode_locks->clear();
}

const ChannelInfo::ModeList &ChannelInfo::GetMLock() const
{
	return this->mode_locks;
}

std::pair<ChannelInfo::ModeList::iterator, ChannelInfo::ModeList::iterator> ChannelInfo::GetModeList(ChannelModeName Name)
{
	ChannelInfo::ModeList::iterator it = this->mode_locks->find(Name), it_end = it;
	if (it != this->mode_locks->end())
		it_end = this->mode_locks->upper_bound(Name);
	return std::make_pair(it, it_end);
}

const ModeLock *ChannelInfo::GetMLock(ChannelModeName mname, const Anope::string &param)
{
	ChannelInfo::ModeList::iterator it = this->mode_locks->find(mname);
	if (it != this->mode_locks->end())
	{
		if (param.empty())
			return it->second;
		else
		{
			ChannelInfo::ModeList::iterator it_end = this->mode_locks->upper_bound(mname);
			for (; it != it_end; ++it)
			{
				if (Anope::Match(param, it->second->param))
					return it->second;
			}
		}
	}

	return NULL;
}

Anope::string ChannelInfo::GetMLockAsString(bool complete) const
{
	Anope::string pos = "+", neg = "-", params;

	for (ChannelInfo::ModeList::const_iterator it = this->GetMLock().begin(), it_end = this->GetMLock().end(); it != it_end; ++it)
	{
		const ModeLock *ml = it->second;
		ChannelMode *cm = ModeManager::FindChannelModeByName(ml->name);
		if (!cm || cm->type == MODE_LIST || cm->type == MODE_STATUS)
			continue;

		if (ml->set)
			pos += cm->mchar;
		else
			neg += cm->mchar;

		if (complete && !ml->param.empty() && cm->type == MODE_PARAM)
			params += " " + ml->param;
	}

	if (pos.length() == 1)
		pos.clear();
	if (neg.length() == 1)
		neg.clear();

	return pos + neg + params;
}

bool ChannelInfo::CheckKick(User *user)
{
	if (!user || !this->c)
		return false;

	if (user->super_admin)
		return false;

	/* We don't enforce services restrictions on clients on ulined services
	 * as this will likely lead to kick/rejoin floods. ~ Viper */
	if (user->server->IsULined())
		return false;

	if (user->IsProtected())
		return false;

	bool set_modes = false, do_kick = false;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnCheckKick, OnCheckKick(user, this, do_kick));
	if (MOD_RESULT == EVENT_ALLOW)
		return false;

	Anope::string mask, reason;
	if (!user->HasMode(UMODE_OPER) && this->HasFlag(CI_SUSPENDED))
	{
		mask = this->GetIdealBan(user);
		reason = Language::Translate(user, _("This channel may not be used."));
		set_modes = true;
		do_kick = true;
	}

	if (!do_kick && !this->c->MatchesList(user, CMODE_EXCEPT))
		return false;

	const NickCore *nc = user->Account() || user->IsRecognized() ? user->Account() : NULL;

	if (!do_kick)
	{
		for (unsigned j = 0, end = this->GetAkickCount(); j < end; ++j)
		{
			AutoKick *autokick = this->GetAkick(j);

			if (autokick->HasFlag(AK_ISNICK))
			{
				if (autokick->nc == nc)
					do_kick = true;
			}
			else
			{
				Entry akick_mask(CMODE_BEGIN, autokick->mask);
				if (akick_mask.Matches(user))
					do_kick = true;
			}
			if (do_kick)
			{
				Log(LOG_DEBUG_2) << user->nick << " matched akick " << (autokick->HasFlag(AK_ISNICK) ? autokick->nc->display : autokick->mask);
				autokick->last_used = Anope::CurTime;
				if (autokick->HasFlag(AK_ISNICK))
					mask = this->GetIdealBan(user);
				else
					mask = autokick->mask;
				reason = autokick->reason.empty() ? Config->CSAutokickReason : autokick->reason;
				break;
			}
		}
	}

	if (!do_kick && this->HasFlag(CI_RESTRICTED) && this->AccessFor(user).empty() && (!this->founder || user->Account() != this->founder))
	{
		do_kick = true;
		mask = this->GetIdealBan(user);
		reason = Language::Translate(user->Account(), CHAN_NOT_ALLOWED_TO_JOIN);
	}

	if (!do_kick)
		return false;

	Log(LOG_DEBUG) << "Autokicking "<< user->GetMask() <<  " from " << this->name;

	/* If the channel isn't syncing and doesn't have any users, join ChanServ
	 * Note that the user AND POSSIBLY the botserv bot exist here
	 * ChanServ always enforces channels like this to keep people from deleting bots etc
	 * that are holding channels.
	 */
	if (this->c->users.size() == (this->bi && this->c->FindUser(this->bi) ? 2 : 1) && !this->c->HasFlag(CH_INHABIT) && !this->c->HasFlag(CH_SYNCING))
	{
		/* Set +si to prevent rejoin */
		if (set_modes)
		{
			c->SetMode(NULL, CMODE_NOEXTERNAL);
			c->SetMode(NULL, CMODE_TOPIC);
			c->SetMode(NULL, CMODE_SECRET);
			c->SetMode(NULL, CMODE_INVITE);
		}

		/* Join ChanServ and set a timer for this channel to part ChanServ later */
		this->c->Hold();
	}

	this->c->SetMode(NULL, CMODE_BAN, mask);
	this->c->Kick(NULL, user, "%s", reason.c_str());

	return true;
}

void ChannelInfo::CheckTopic()
{
	if (!this->c)
		return;

	/* We only compare the topics here, not the time or setter. This is because some (old) IRCds do not
	 * allow us to set the topic as someone else, meaning we have to bump the TS and change the setter to us.
	 * This desyncs what is really set with what we have stored, and we end up resetting the topic often when
	 * it is not required
	 */
	if (this->HasFlag(CI_TOPICLOCK) && this->last_topic != this->c->topic)
	{
		this->c->ChangeTopic(this->last_topic_setter, this->last_topic, this->last_topic_time);
	}
	else
	{
		this->last_topic = this->c->topic;
		this->last_topic_setter = this->c->topic_setter;
		this->last_topic_time = this->c->topic_ts;
	}
}

void ChannelInfo::RestoreTopic()
{
	if (!this->c)
		return;

	if ((this->HasFlag(CI_KEEPTOPIC) || this->HasFlag(CI_TOPICLOCK)) && this->last_topic != this->c->topic)
	{
		this->c->ChangeTopic(!this->last_topic_setter.empty() ? this->last_topic_setter : this->WhoSends()->nick, this->last_topic, this->last_topic_time ? this->last_topic_time : Anope::CurTime);
	}
}

int16_t ChannelInfo::GetLevel(const Anope::string &priv) const
{
	if (PrivilegeManager::FindPrivilege(priv) == NULL)
	{
		Log(LOG_DEBUG) << "Unknown privilege " + priv;
		return ACCESS_INVALID;
	}

	std::map<Anope::string, int16_t>::const_iterator it = this->levels.find(priv);
	if (it == this->levels.end())
		return 0;
	return it->second;
}

void ChannelInfo::SetLevel(const Anope::string &priv, int16_t level)
{
	this->levels[priv] = level;
}

void ChannelInfo::RemoveLevel(const Anope::string &priv)
{
	this->levels.erase(priv);
}

void ChannelInfo::ClearLevels()
{
	this->levels.clear();
}

Anope::string ChannelInfo::GetIdealBan(User *u) const
{
	switch (this->bantype)
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

ChannelInfo* ChannelInfo::Find(const Anope::string &name)
{
	registered_channel_map::const_iterator it = RegisteredChannelList->find(name);
	if (it != RegisteredChannelList->end())
	{
		it->second->QueueUpdate();
		return it->second;
	}

	return NULL;
}

bool IsFounder(const User *user, const ChannelInfo *ci)
{
	if (!user || !ci)
		return false;

	if (user->super_admin)
		return true;

	if (user->Account() && user->Account() == ci->GetFounder())
		return true;

	return false;
}

