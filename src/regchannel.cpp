/* Registered channel functions
 *
 * (C) 2003-2013 Anope Team
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

Serialize::Checker<registered_channel_map> RegisteredChannelList("ChannelInfo");

BadWord::BadWord() : Serializable("BadWord")
{
}

BadWord::~BadWord()
{
	if (this->ci)
	{
		std::vector<BadWord *>::iterator it = std::find(this->ci->badwords->begin(), this->ci->badwords->end(), this);
		if (it != this->ci->badwords->end())
			this->ci->badwords->erase(it);
	}
}

void BadWord::Serialize(Serialize::Data &data) const
{
	data["ci"] << this->ci->name;
	data["word"] << this->word;
	data.SetType("type", Serialize::Data::DT_INT); data["type"] << this->type;
}

Serializable* BadWord::Unserialize(Serializable *obj, Serialize::Data &data)
{
	Anope::string sci, sword;

	data["ci"] >> sci;
	data["word"] >> sword;

	ChannelInfo *ci = ChannelInfo::Find(sci);
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
		bw = ci->AddBadWord(sword, static_cast<BadWordType>(n));
	
	return bw;
}

AutoKick::AutoKick() : Serializable("AutoKick")
{
}

AutoKick::~AutoKick()
{
	if (this->ci)
	{
		std::vector<AutoKick *>::iterator it = std::find(this->ci->akick->begin(), this->ci->akick->end(), this);
		if (it != this->ci->akick->end())
			this->ci->akick->erase(it);

		const NickAlias *na = NickAlias::Find(this->mask);
		if (na != NULL)
			na->nc->RemoveChannelReference(this->ci);
	}
}

void AutoKick::Serialize(Serialize::Data &data) const
{
	data["ci"] << this->ci->name;
	if (this->nc)
		data["nc"] << this->nc->display;
	else
		data["mask"] << this->mask;
	data["reason"] << this->reason;
	data["creator"] << this->creator;
	data.SetType("addtime", Serialize::Data::DT_INT); data["addtime"] << this->addtime;
	data.SetType("last_used", Serialize::Data::DT_INT); data["last_used"] << this->last_used;
}

Serializable* AutoKick::Unserialize(Serializable *obj, Serialize::Data &data)
{
	Anope::string sci, snc;

	data["ci"] >> sci;
	data["nc"] >> snc;

	ChannelInfo *ci = ChannelInfo::Find(sci);
	if (!ci)
		return NULL;
	
	AutoKick *ak;
	NickCore *nc = NickCore::Find(snc);
	if (obj)
	{
		ak = anope_dynamic_static_cast<AutoKick *>(obj);
		data["creator"] >> ak->creator;
		data["reason"] >> ak->reason;
		ak->nc = NickCore::Find(snc);
		data["mask"] >> ak->mask;
		data["addtime"] >> ak->addtime;
		data["last_used"] >> ak->last_used;
	}
	else
	{
		time_t addtime, lastused;
		data["addtime"] >> addtime;
		data["last_used"] >> lastused;

		Anope::string screator, sreason, smask;

		data["creator"] >> screator;
		data["reason"] >> sreason;
		data["mask"] >> smask;

		if (nc)	
			ak = ci->AddAkick(screator, nc, sreason, addtime, lastused);
		else
			ak = ci->AddAkick(screator, smask, sreason, addtime, lastused);
	}

	return ak;
}

ModeLock::ModeLock(ChannelInfo *ch, bool s, const Anope::string &n, const Anope::string &p, const Anope::string &se, time_t c) : Serializable("ModeLock"), ci(ch), set(s), name(n), param(p), setter(se), created(c)
{
}

ModeLock::~ModeLock()
{
	if (this->ci)
		this->ci->RemoveMLock(this);
}

void ModeLock::Serialize(Serialize::Data &data) const
{
	if (!this->ci)
		return;

	data["ci"] << this->ci->name;
	data["set"] << this->set;
	data["name"] << this->name;
	data["param"] << this->param;
	data["setter"] << this->setter;
	data.SetType("created", Serialize::Data::DT_INT); data["created"] << this->created;
}

Serializable* ModeLock::Unserialize(Serializable *obj, Serialize::Data &data)
{
	Anope::string sci;
	
	data["ci"] >> sci;

	ChannelInfo *ci = ChannelInfo::Find(sci);
	if (!ci)
		return NULL;
	
	ModeLock *ml;
	if (obj)
	{
		ml = anope_dynamic_static_cast<ModeLock *>(obj);

		data["set"] >> ml->set;
		data["name"] >> ml->name;
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

		Anope::string setter;
		data["setter"] >> setter;

		Anope::string sname;
		data["name"] >> sname;

		ml = new ModeLock(ci, set, sname, "", setter, created);
		data["param"] >> ml->param;

		ci->mode_locks->insert(std::make_pair(ml->name, ml));
		return ml;
	}
}

void LogSetting::Serialize(Serialize::Data &data) const
{
	if (!ci)
		return;

	data["ci"] << ci->name;
	data["service_name"] << service_name;
	data["command_service"] << command_service;
	data["command_name"] << command_name;
	data["method"] << method;
	data["extra"] << extra;
	data["creator"] << creator;
	data.SetType("created", Serialize::Data::DT_INT); data["created"] << created;
}

Serializable* LogSetting::Unserialize(Serializable *obj, Serialize::Data &data)
{
	Anope::string sci;
	
	data["ci"] >> sci;

	ChannelInfo *ci = ChannelInfo::Find(sci);
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
	this->banexpire = 0;
	this->bi = NULL;

	this->last_topic_setter = Config->ChanServ;
	this->last_topic_time = Anope::CurTime;

	this->name = chname;

	/* Set default channel flags */
	for (std::set<Anope::string>::const_iterator it = Config->CSDefFlags.begin(), it_end = Config->CSDefFlags.end(); it != it_end; ++it)
		this->ExtendMetadata(*it);

	/* Set default bot flags */
	for (std::set<Anope::string>::const_iterator it = Config->BSDefFlags.begin(), it_end = Config->BSDefFlags.end(); it != it_end; ++it)
		this->ExtendMetadata(*it);

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
		if (takick->nc)
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
		/* Parting the service bot can cause the channel to go away */
		if (this->c)
			this->c->ci = NULL;
	}

	RegisteredChannelList->erase(this->name);

	this->SetFounder(NULL);
	this->SetSuccessor(NULL);

	this->ClearAccess();
	this->ClearAkick();
	this->ClearBadWords();

	for (unsigned i = 0; i < this->log_settings->size(); ++i)
		delete this->log_settings->at(i);
	this->log_settings->clear();

	while (!this->mode_locks->empty())
		delete this->mode_locks->begin()->second;

	if (!this->memos.memos->empty())
	{
		for (unsigned i = 0, end = this->memos.memos->size(); i < end; ++i)
			delete this->memos.GetMemo(i);
		this->memos.memos->clear();
	}

	if (this->founder)
		--this->founder->channelcount;
}

void ChannelInfo::Serialize(Serialize::Data &data) const
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
	this->ExtensibleSerialize(data);
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
	data.SetType("capsmin", Serialize::Data::DT_INT); data["capsmin"] << this->capsmin;
	data.SetType("capspercent", Serialize::Data::DT_INT); data["capspercent"] << this->capspercent;
	data.SetType("floodlines", Serialize::Data::DT_INT); data["floodlines"] << this->floodlines;
	data.SetType("floodsecs", Serialize::Data::DT_INT); data["floodsecs"] << this->floodsecs;
	data.SetType("repeattimes", Serialize::Data::DT_INT); data["repeattimes"] << this->repeattimes;
	data.SetType("banexpire", Serialize::Data::DT_INT); data["banexpire"] << this->banexpire;
	data["memomax"] << this->memos.memomax;
	for (unsigned i = 0; i < this->memos.ignores.size(); ++i)
		data["memoignores"] << this->memos.ignores[i] << " ";
}

Serializable* ChannelInfo::Unserialize(Serializable *obj, Serialize::Data &data)
{
	Anope::string sname, sfounder, ssuccessor, slevels, sbi;

	data["name"] >> sname;
	data["founder"] >> sfounder;
	data["successor"] >> ssuccessor;
	data["levels"] >> slevels;
	data["bi"] >> sbi;

	ChannelInfo *ci;
	if (obj)
		ci = anope_dynamic_static_cast<ChannelInfo *>(obj);
	else
		ci = new ChannelInfo(sname);

	ci->ExtensibleUnserialize(data);

	ci->SetFounder(NickCore::Find(sfounder));
	ci->SetSuccessor(NickCore::Find(ssuccessor));

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
			ci->levels[v[i]] = convertTo<int16_t>(v[i + 1]);
	}
	BotInfo *bi = BotInfo::Find(sbi);
	if (*ci->bi != bi)
	{
		if (ci->bi)
			ci->bi->UnAssign(NULL, ci);
		ci->bi = bi;
		if (ci->bi)
			ci->bi->Assign(NULL, ci);
	}
	{
		Anope::string ttb, tok;
		data["ttb"] >> ttb;
		spacesepstream sep(ttb);
		for (int i = 0; sep.GetToken(tok) && i < TTB_SIZE; ++i)
			try
			{
				ci->ttb[i] = convertTo<int16_t>(tok);
			}
			catch (const ConvertException &) { }

	}
	data["capsmin"] >> ci->capsmin;
	data["capspercent"] >> ci->capspercent;
	data["floodlines"] >> ci->floodlines;
	data["floodsecs"] >> ci->floodsecs;
	data["repeattimes"] >> ci->repeattimes;
	data["banexpire"] >> ci->banexpire;
	data["memomax"] >> ci->memos.memomax;
	{
		Anope::string buf;
		data["memoignores"] >> buf;
		spacesepstream sep(buf);
		ci->memos.ignores.clear();
		while (sep.GetToken(buf))
			ci->memos.ignores.push_back(buf);
	}

	/* Compat */
	Anope::string sflags, sbotflags;
	data["flags"] >> sflags;
	data["botflags"] >> sbotflags;
	spacesepstream sep(sflags);
	Anope::string tok;
	while (sep.GetToken(tok))
		ci->ExtendMetadata(tok);
	spacesepstream sep2(sbotflags);
	while (sep2.GetToken(tok))
		ci->ExtendMetadata("BS_" + tok);
	/* End compat */

	return ci;
}


void ChannelInfo::SetFounder(NickCore *nc)
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

NickCore *ChannelInfo::GetFounder() const
{
	return this->founder;
}

void ChannelInfo::SetSuccessor(NickCore *nc)
{
	if (this->successor)
		this->successor->RemoveChannelReference(this);
	this->successor = nc;
	if (this->successor)
		this->successor->AddChannelReference(this);
}

NickCore *ChannelInfo::GetSuccessor() const
{
	return this->successor;
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

	const NickAlias *na = NickAlias::Find(taccess->mask);
	if (na != NULL)
	{
		na->nc->AddChannelReference(this);
		taccess->nc = na->nc;
	}
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
	{
		ChanAccess *a = this->GetAccess(i);

		if (a->nc)
		{
			if (a->nc == nc)
				group.push_back(a);
		}
		else if (a->Matches(u, nc))
		{
			group.push_back(a);
		}
	}

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
	{
		ChanAccess *a = this->GetAccess(i);

		if (a->nc)
		{
			if (a->nc == nc)
				group.push_back(a);
		}
		else if (this->GetAccess(i)->Matches(NULL, nc))
		{
			group.push_back(this->GetAccess(i));
		}
	}
	
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

	delete this->access->at(index);
}

void ChannelInfo::ClearAccess()
{
	for (unsigned i = this->access->size(); i > 0; --i)
		delete this->GetAccess(i - 1);
}

AutoKick *ChannelInfo::AddAkick(const Anope::string &user, NickCore *akicknc, const Anope::string &reason, time_t t, time_t lu)
{
	AutoKick *autokick = new AutoKick();
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
	
	delete this->GetAkick(index);
}

void ChannelInfo::ClearAkick()
{
	while (!this->akick->empty())
		delete this->akick->back();
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

	delete this->badwords->at(index);
}

void ChannelInfo::ClearBadWords()
{
	while (!this->badwords->empty())
		delete this->badwords->back();
}

bool ChannelInfo::HasMLock(ChannelMode *mode, const Anope::string &param, bool status) const
{
	if (!mode)
		return false;

	std::multimap<Anope::string, ModeLock *>::const_iterator it = this->mode_locks->find(mode->name);

	if (it != this->mode_locks->end())
	{
		if (mode->type != MODE_REGULAR)
		{
			std::multimap<Anope::string, ModeLock *>::const_iterator it_end = this->mode_locks->upper_bound(mode->name);

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
	if (!mode)
		return false;

	if (setter.empty())
		setter = this->founder ? this->founder->display : "Unknown";
	std::pair<Anope::string, ModeLock *> ml = std::make_pair(mode->name, new ModeLock(this, status, mode->name, param, setter, created));

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnMLock, OnMLock(this, ml.second));
	if (MOD_RESULT == EVENT_STOP)
		return false;

	/* First, remove this */
	if (mode->type == MODE_REGULAR || mode->type == MODE_PARAM)
	{
		for (ChannelInfo::ModeList::const_iterator it; (it = this->mode_locks->find(mode->name)) != this->mode_locks->end();)
			delete it->second;
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
					delete it->second;
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
	if (!mode)
		return false;

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
					delete it->second;
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
					delete it->second;
					return true;
				}
			}
		}

		return false;
	}
}

void ChannelInfo::RemoveMLock(ModeLock *mlock)
{
	ChannelInfo::ModeList::iterator it = this->mode_locks->find(mlock->name);
	if (it != this->mode_locks->end())
		for (; it != this->mode_locks->upper_bound(mlock->name); ++it)
			if (it->second == mlock)
			{
				this->mode_locks->erase(it);
				break;
			}
}

void ChannelInfo::ClearMLock()
{
	while (!this->mode_locks->empty())
		delete this->mode_locks->begin()->second;
	this->mode_locks->clear();
}

const ChannelInfo::ModeList &ChannelInfo::GetMLock() const
{
	return this->mode_locks;
}

std::pair<ChannelInfo::ModeList::iterator, ChannelInfo::ModeList::iterator> ChannelInfo::GetModeList(const Anope::string &mname)
{
	ChannelInfo::ModeList::iterator it = this->mode_locks->find(mname), it_end = it;
	if (it != this->mode_locks->end())
		it_end = this->mode_locks->upper_bound(mname);
	return std::make_pair(it, it_end);
}

const ModeLock *ChannelInfo::GetMLock(const Anope::string &mname, const Anope::string &param)
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

	Anope::string mask, reason;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnCheckKick, OnCheckKick(user, this, mask, reason));
	if (MOD_RESULT != EVENT_STOP)
		return false;
	
	if (mask.empty())
		mask = this->GetIdealBan(user);
	if (reason.empty())
		reason = Language::Translate(user->Account(), CHAN_NOT_ALLOWED_TO_JOIN);

	Log(LOG_DEBUG) << "Autokicking " << user->nick << " (" << mask << ") from " << this->name;

	/* If the channel isn't syncing and doesn't have any users, join ChanServ
	 * Note that the user AND POSSIBLY the botserv bot exist here
	 * ChanServ always enforces channels like this to keep people from deleting bots etc
	 * that are holding channels.
	 */
	if (this->c->users.size() == (this->bi && this->c->FindUser(this->bi) ? 2 : 1) && !this->c->HasExt("INHABIT") && !this->c->HasExt("SYNCING"))
	{
		/* Set +ntsi to prevent rejoin */
		c->SetMode(NULL, "NOEXTERNAL");
		c->SetMode(NULL, "TOPIC");
		c->SetMode(NULL, "SECRET");
		c->SetMode(NULL, "INVITE");

		/* Join ChanServ and set a timer for this channel to part ChanServ later */
		this->c->Hold();
	}

	this->c->SetMode(NULL, "BAN", mask);
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
	if (this->HasExt("TOPICLOCK") && this->last_topic != this->c->topic)
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

	if ((this->HasExt("KEEPTOPIC") || this->HasExt("TOPICLOCK")) && this->last_topic != this->c->topic)
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

