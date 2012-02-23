/* Registered channel functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "regchannel.h"
#include "account.h"
#include "access.h"
#include "channels.h"
#include "config.h"
#include "bots.h"
#include "extern.h"
#include "language.h"
#include "servers.h"

Anope::string BadWord::serialize_name() const
{
	return "BadWord";
}

Serializable::serialized_data BadWord::serialize()
{
	serialized_data data;

	data["ci"] << this->ci->name;
	data["word"] << this->word;
	data["type"].setType(Serialize::DT_INT) << this->type;

	return data;
}

void BadWord::unserialize(serialized_data &data)
{
	ChannelInfo *ci = cs_findchan(data["ci"].astr());
	if (!ci)
		return;
	
	unsigned int n;
	data["type"] >> n;

	ci->AddBadWord(data["word"].astr(), static_cast<BadWordType>(n));
}

AutoKick::AutoKick() : Flags<AutoKickFlag>(AutoKickFlagString)
{
}

Anope::string AutoKick::serialize_name() const
{
	return "AutoKick";
}

Serializable::serialized_data AutoKick::serialize()
{
	serialized_data data;

	data["ci"] << this->ci->name;
	if (this->HasFlag(AK_ISNICK) && this->nc)
		data["nc"] << this->nc->display;
	else
		data["mask"] << this->mask;
	data["reason"] << this->reason;
	data["creator"] << this->creator;
	data["addtime"].setType(Serialize::DT_INT) << this->addtime;
	data["last_used"].setType(Serialize::DT_INT) << this->last_used;
	data["flags"] << this->ToString();

	return data;
}

void AutoKick::unserialize(serialized_data &data)
{
	ChannelInfo *ci = cs_findchan(data["ci"].astr());
	if (ci == NULL)
		return;
	
	time_t addtime, lastused;
	data["addtime"] >> addtime;
	data["last_used"] >> lastused;
	
	NickCore *nc = findcore(data["nc"].astr());
	if (nc)	
		ci->AddAkick(data["creator"].astr(), nc, data["reason"].astr(), addtime, lastused);
	else
		ci->AddAkick(data["creator"].astr(), data["mask"].astr(), data["reason"].astr(), addtime, lastused);
}

ModeLock::ModeLock(ChannelInfo *ch, bool s, ChannelModeName n, const Anope::string &p, const Anope::string &se, time_t c) : ci(ch), set(s), name(n), param(p), setter(se), created(c)
{
}

Anope::string ModeLock::serialize_name() const
{
	return "ModeLock";
}

Serializable::serialized_data ModeLock::serialize()
{
	serialized_data data;

	if (this->ci == NULL)
		return data;

	data["ci"] << this->ci->name;
	data["set"] << this->set;
	data["name"] << ChannelModeNameStrings[this->name];
	data["param"] << this->param;
	data["setter"] << this->setter;
	data["created"].setType(Serialize::DT_INT) << this->created;

	return data;
}

void ModeLock::unserialize(serialized_data &data)
{
	ChannelInfo *ci = cs_findchan(data["ci"].astr());
	if (ci == NULL)
		return;

	ChannelModeName name = CMODE_END;

	for (unsigned i = 0; !ChannelModeNameStrings[i].empty(); ++i)
		if (ChannelModeNameStrings[i] == data["name"].astr())
		{
			name = static_cast<ChannelModeName>(i);
			break;
		}
	if (name == CMODE_END)
		return;

	bool set;
	data["set"] >> set;

	time_t created;
	data["created"] >> created;

	ModeLock ml(ci, set, name, "", data["setter"].astr(), created);
	data["param"] >> ml.param;

	ci->mode_locks.insert(std::make_pair(ml.name, ml));
}

Anope::string LogSetting::serialize_name() const
{
	return "LogSetting";
}

Serializable::serialized_data LogSetting::serialize()
{
	serialized_data data;

	data["ci"] << ci->name;
	data["service_name"] << service_name;
	data["command_service"] << command_service;
	data["command_name"] << command_name;
	data["method"] << method;
	data["extra"] << extra;
	data["creator"] << creator;
	data["created"].setType(Serialize::DT_INT) << created;

	return data;
}

void LogSetting::unserialize(serialized_data &data)
{
	LogSetting ls;

	ChannelInfo *ci = cs_findchan(data["ci"].astr());
	if (ci == NULL)
		return;

	ls.ci = ci;
	data["service_name"] >> ls.service_name;
	data["command_service"] >> ls.command_service;
	data["command_name"] >> ls.command_name;
	data["method"] >> ls.method;
	data["extra"] >> ls.extra;
	data["creator"] >> ls.creator;
	data["created"] >> ls.created;

	ci->log_settings.push_back(ls);
}

/** Default constructor
 * @param chname The channel name
 */
ChannelInfo::ChannelInfo(const Anope::string &chname) : Flags<ChannelInfoFlag, CI_END>(ChannelInfoFlagStrings), botflags(BotServFlagStrings)
{
	if (chname.empty())
		throw CoreException("Empty channel passed to ChannelInfo constructor");

	this->founder = this->successor = NULL;
	this->last_topic_time = 0;
	this->c = findchan(chname);
	if (this->c)
		this->c->ci = this;
	this->capsmin = this->capspercent = 0;
	this->floodlines = this->floodsecs = 0;
	this->repeattimes = 0;
	this->bi = NULL;

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

	RegisteredChannelList[this->name] = this;

	FOREACH_MOD(I_OnCreateChan, OnCreateChan(this));
}

/** Copy constructor
 * @param ci The ChannelInfo to copy settings to
 */
ChannelInfo::ChannelInfo(ChannelInfo &ci) : Flags<ChannelInfoFlag, CI_END>(ChannelInfoFlagStrings), botflags(BotServFlagStrings)
{
	*this = ci;

	if (this->founder)
		++this->founder->channelcount;

	this->access.clear();
	this->akick.clear();
	this->badwords.clear();

	if (this->bi)
		++this->bi->chancount;

	for (int i = 0; i < TTB_SIZE; ++i)
		this->ttb[i] = ci.ttb[i];

	for (unsigned i = 0; i < ci.GetAccessCount(); ++i)
	{
		ChanAccess *taccess = ci.GetAccess(i);
		AccessProvider *provider = taccess->provider;

		ChanAccess *newaccess = provider->Create();
		newaccess->ci = this;
		newaccess->mask = taccess->mask;
		newaccess->creator = taccess->creator;
		newaccess->last_seen = taccess->last_seen;
		newaccess->created = taccess->created;
		newaccess->Unserialize(taccess->Serialize());

		this->AddAccess(newaccess);
	}

	for (unsigned i = 0; i < ci.GetAkickCount(); ++i)
	{
		AutoKick *takick = ci.GetAkick(i);
		if (takick->HasFlag(AK_ISNICK))
			this->AddAkick(takick->creator, takick->nc, takick->reason, takick->addtime, takick->last_used);
		else
			this->AddAkick(takick->creator, takick->mask, takick->reason, takick->addtime, takick->last_used);
	}
	for (unsigned i = 0; i < ci.GetBadWordCount(); ++i)
	{
		BadWord *bw = ci.GetBadWord(i);
		this->AddBadWord(bw->word, bw->type);
	}
}

/** Default destructor, cleans up the channel complete and removes it from the internal list
 */
ChannelInfo::~ChannelInfo()
{
	unsigned i, end;

	FOREACH_MOD(I_OnDelChan, OnDelChan(this));

	Log(LOG_DEBUG) << "Deleting channel " << this->name;

	if (this->c)
	{
		if (this->bi && this->c->FindUser(this->bi))
			this->bi->Part(this->c);
		this->c->ci = NULL;
	}

	RegisteredChannelList.erase(this->name);

	this->ClearAccess();
	this->ClearAkick();
	this->ClearBadWords();

	if (!this->memos.memos.empty())
	{
		for (i = 0, end = this->memos.memos.size(); i < end; ++i)
			delete this->memos.memos[i];
		this->memos.memos.clear();
	}

	if (this->founder)
		--this->founder->channelcount;
}

Anope::string ChannelInfo::serialize_name() const
{
	return "ChannelInfo";
}

Serializable::serialized_data ChannelInfo::serialize()
{
	serialized_data data;

	data["name"].setKey().setMax(255) << this->name;
	if (this->founder)
		data["founder"] << this->founder->display;
	if (this->successor)
		data["successor"] << this->successor->display;
	data["description"] << this->desc;
	data["time_registered"].setType(Serialize::DT_INT) << this->time_registered;
	data["last_used"].setType(Serialize::DT_INT) << this->last_used;
	data["last_topic"] << this->last_topic;
	data["last_topic_setter"] << this->last_topic_setter;
	data["last_topic_time"].setType(Serialize::DT_INT) << this->last_topic_time;
	data["bantype"].setType(Serialize::DT_INT) << this->bantype;
	data["flags"] << this->ToString();
	data["botflags"] << this->botflags.ToString();
	{
		Anope::string levels_buffer;
		for (std::map<Anope::string, int16_t>::iterator it = this->levels.begin(), it_end = this->levels.end(); it != it_end; ++it)
			levels_buffer += it->first + " " + stringify(it->second) + " ";
		data["levels"] << levels_buffer;
	}
	if (this->bi)
		data["bi"] << this->bi->nick;
	for (int i = 0; i < TTB_SIZE; ++i)
		data["ttb"] << this->ttb[i] << " ";
	data["capsmin"].setType(Serialize::DT_INT) << this->capsmin;
	data["capspercent"].setType(Serialize::DT_INT) << this->capspercent;
	data["floodlines"].setType(Serialize::DT_INT) << this->floodlines;
	data["floodsecs"].setType(Serialize::DT_INT) << this->floodsecs;
	data["repeattimes"].setType(Serialize::DT_INT) << this->repeattimes;
	data["memomax"] << this->memos.memomax;
	for (unsigned i = 0; i < this->memos.ignores.size(); ++i)
		data["memoignores"] << this->memos.ignores[i] << " ";

	return data;
}

void ChannelInfo::unserialize(serialized_data &data)
{
	ChannelInfo *ci = cs_findchan(data["name"].astr());
	if (ci == NULL)
		ci = new ChannelInfo(data["name"].astr());

	if (data.count("founder") > 0)
	{
		if (ci->founder)
			ci->founder->channelcount--;
		ci->founder = findcore(data["founder"].astr());
		if (ci->founder)
			ci->founder->channelcount++;
	}
	if (data.count("successor") > 0)
	{
		ci->successor = findcore(data["successor"].astr());
		if (ci->founder && ci->founder == ci->successor)
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
		std::vector<Anope::string> v = BuildStringVector(data["levels"].astr());
		for (unsigned i = 0; i + 1 < v.size(); i += 2)
			ci->levels[v[i]] = convertTo<int16_t>(v[i + 1]);
	}
	if (data.count("bi") > 0)
	{
		if (ci->bi)
			ci->bi->chancount--;
		ci->bi = findbot(data["bi"].astr());
		if (ci->bi)
			ci->bi->chancount++;
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
}


/** Change the founder of the channek
 * @params nc The new founder
 */
void ChannelInfo::SetFounder(NickCore *nc)
{
	if (this->founder)
		--this->founder->channelcount;
	this->founder = nc;
	if (this->founder)
		++this->founder->channelcount;
	if (this->founder == this->successor)
		this->successor = NULL;
}

/** Get the founder of the channel
 * @return The founder
 */
NickCore *ChannelInfo::GetFounder() const
{
	return this->founder;
}

/** Find which bot should send mode/topic/etc changes for this channel
 * @return The bot
 */
BotInfo *ChannelInfo::WhoSends()
{
	if (this && this->bi)
		return this->bi;
	BotInfo *tbi = findbot(Config->ChanServ);
	if (tbi)
		return tbi;
	else if (!BotListByNick.empty())
		return BotListByNick.begin()->second;
	return NULL;
}

/** Add an entry to the channel access list
 * @param taccess The entry
 */
void ChannelInfo::AddAccess(ChanAccess *taccess)
{
	this->access.push_back(taccess);
}

/** Get an entry from the channel access list by index
 *
 * @param index The index in the access list vector
 * @return A ChanAccess struct corresponding to the index given, or NULL if outside the bounds
 *
 * Retrieves an entry from the access list that matches the given index.
 */
ChanAccess *ChannelInfo::GetAccess(unsigned index)
{
	if (this->access.empty() || index >= this->access.size())
		return NULL;

	return this->access[index];
}

AccessGroup ChannelInfo::AccessFor(User *u)
{
	AccessGroup group;

	if (u == NULL)
		return group;

	NickCore *nc = u->Account();
	if (nc == NULL && u->IsRecognized())
	{
		NickAlias *na = findnick(u->nick);
		if (na != NULL)
			nc = na->nc;
	}

	group.SuperAdmin = u->SuperAdmin;
	group.Founder = IsFounder(u, this);
	group.ci = this;
	group.nc = nc;

	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
		if (this->access[i]->Matches(u, nc))
			group.push_back(this->access[i]);

	if (group.Founder || !group.empty())
		this->last_used = Anope::CurTime;

	return group;
}

AccessGroup ChannelInfo::AccessFor(NickCore *nc)
{
	AccessGroup group;

	group.Founder = (this->founder && this->founder == nc);
	group.ci = this;
	group.nc = nc;

	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
		if (this->access[i]->Matches(NULL, nc))
			group.push_back(this->access[i]);
	
	if (group.Founder || !group.empty())
		this->last_used = Anope::CurTime;

	return group;
}

/** Get the size of the accss vector for this channel
 * @return The access vector size
 */
unsigned ChannelInfo::GetAccessCount() const
{
	return this->access.size();
}

/** Erase an entry from the channel access list
 *
 * @param index The index in the access list vector
 *
 * Clears the memory used by the given access entry and removes it from the vector.
 */
void ChannelInfo::EraseAccess(unsigned index)
{
	if (this->access.empty() || index >= this->access.size())
		return;

	delete this->access[index];
	this->access.erase(this->access.begin() + index);
}

/** Erase an entry from the channel access list
 *
 * @param taccess The access to remove
 *
 * Clears the memory used by the given access entry and removes it from the vector.
 */
void ChannelInfo::EraseAccess(ChanAccess *taccess)
{
	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
	{
		if (this->access[i] == taccess)
		{
			delete this->access[i];
			this->access.erase(this->access.begin() + i);
			break;
		}
	}
}

/** Clear the entire channel access list
 *
 * Clears the entire access list by deleting every item and then clearing the vector.
 */
void ChannelInfo::ClearAccess()
{
	for (unsigned i = this->access.size(); i > 0; --i)
		delete this->access[i - 1];
	this->access.clear();
}

/** Add an akick entry to the channel by NickCore
 * @param user The user who added the akick
 * @param akicknc The nickcore being akicked
 * @param reason The reason for the akick
 * @param t The time the akick was added, defaults to now
 * @param lu The time the akick was last used, defaults to never
 * @return The AutoKick structure
 */
AutoKick *ChannelInfo::AddAkick(const Anope::string &user, NickCore *akicknc, const Anope::string &reason, time_t t, time_t lu)
{
	if (!akicknc)
		return NULL;

	AutoKick *autokick = new AutoKick();
	autokick->ci = this;
	autokick->SetFlag(AK_ISNICK);
	autokick->nc = akicknc;
	autokick->reason = reason;
	autokick->creator = user;
	autokick->addtime = t;
	autokick->last_used = lu;

	this->akick.push_back(autokick);

	return autokick;
}

/** Add an akick entry to the channel by reason
 * @param user The user who added the akick
 * @param mask The mask of the akick
 * @param reason The reason for the akick
 * @param t The time the akick was added, defaults to now
 * @param lu The time the akick was last used, defaults to never
 * @return The AutoKick structure
 */
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

	this->akick.push_back(autokick);

	return autokick;
}

/** Get an entry from the channel akick list
 * @param index The index in the akick vector
 * @return The akick structure, or NULL if not found
 */
AutoKick *ChannelInfo::GetAkick(unsigned index)
{
	if (this->akick.empty() || index >= this->akick.size())
		return NULL;

	return this->akick[index];
}

/** Get the size of the akick vector for this channel
 * @return The akick vector size
 */
unsigned ChannelInfo::GetAkickCount() const
{
	return this->akick.empty() ? 0 : this->akick.size();
}

/** Erase an entry from the channel akick list
 * @param index The index of the akick
 */
void ChannelInfo::EraseAkick(unsigned index)
{
	if (this->akick.empty() || index >= this->akick.size())
		return;
	
	delete this->akick[index];
	this->akick.erase(this->akick.begin() + index);
}

/** Clear the whole akick list
 */
void ChannelInfo::ClearAkick()
{
	while (!this->akick.empty())
		EraseAkick(0);
}

/** Add a badword to the badword list
 * @param word The badword
 * @param type The type (SINGLE START END)
 * @return The badword
 */
BadWord *ChannelInfo::AddBadWord(const Anope::string &word, BadWordType type)
{
	BadWord *bw = new BadWord;
	bw->ci = this;
	bw->word = word;
	bw->type = type;

	this->badwords.push_back(bw);

	FOREACH_MOD(I_OnBadWordAdd, OnBadWordAdd(this, bw));

	return bw;
}

/** Get a badword structure by index
 * @param index The index
 * @return The badword
 */
BadWord *ChannelInfo::GetBadWord(unsigned index)
{
	if (this->badwords.empty() || index >= this->badwords.size())
		return NULL;

	return this->badwords[index];
}

/** Get how many badwords are on this channel
 * @return The number of badwords in the vector
 */
unsigned ChannelInfo::GetBadWordCount() const
{
	return this->badwords.empty() ? 0 : this->badwords.size();
}

/** Remove a badword
 * @param index The index of the badword
 */
void ChannelInfo::EraseBadWord(unsigned index)
{
	if (this->badwords.empty() || index >= this->badwords.size())
		return;
	
	FOREACH_MOD(I_OnBadWordDel, OnBadWordDel(this, this->badwords[index]));

	delete this->badwords[index];
	this->badwords.erase(this->badwords.begin() + index);
}

/** Clear all badwords from the channel
 */
void ChannelInfo::ClearBadWords()
{
	while (!this->badwords.empty())
		EraseBadWord(0);
}

/** Check if a mode is mlocked
 * @param mode The mode
 * @param status True to check mlock on, false for mlock off
 * @return true on success, false on fail
 */
bool ChannelInfo::HasMLock(ChannelMode *mode, const Anope::string &param, bool status) const
{
	std::multimap<ChannelModeName, ModeLock>::const_iterator it = this->mode_locks.find(mode->Name);

	if (it != this->mode_locks.end())
	{
		if (mode->Type != MODE_REGULAR)
		{
			std::multimap<ChannelModeName, ModeLock>::const_iterator it_end = this->mode_locks.upper_bound(mode->Name);

			for (; it != it_end; ++it)
			{
				const ModeLock &ml = it->second;
				if (ml.param == param)
					return true;
			}
		}
		else
			return it->second.set == status;
	}
	return false;
}

/** Set a mlock
 * @param mode The mode
 * @param status True for mlock on, false for mlock off
 * @param param The param to use for this mode, if required
 * @return true on success, false on failure (module blocking)
 */
bool ChannelInfo::SetMLock(ChannelMode *mode, bool status, const Anope::string &param, Anope::string setter, time_t created)
{
	if (setter.empty())
		setter = this->founder ? this->founder->display : "Unknown";
	std::pair<ChannelModeName, ModeLock> ml = std::make_pair(mode->Name, ModeLock(this, status, mode->Name, param, setter, created));

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnMLock, OnMLock(this, &ml.second));
	if (MOD_RESULT == EVENT_STOP)
		return false;

	/* First, remove this */
	if (mode->Type == MODE_REGULAR || mode->Type == MODE_PARAM)
		this->mode_locks.erase(mode->Name);
	else
	{
		// For list or status modes, we must check the parameter
		std::multimap<ChannelModeName, ModeLock>::iterator it = this->mode_locks.find(mode->Name);
		if (it != this->mode_locks.end())
		{
			std::multimap<ChannelModeName, ModeLock>::iterator it_end = this->mode_locks.upper_bound(mode->Name);
			for (; it != it_end; ++it)
			{
				const ModeLock &modelock = it->second;
				if (modelock.param == param)
				{
					this->mode_locks.erase(it);
					break;
				}
			}
		}
	}

	this->mode_locks.insert(ml);

	return true;
}

/** Remove a mlock
 * @param mode The mode
 * @param status True for mlock on, false for mlock off
 * @param param The param of the mode, required if it is a list or status mode
 * @return true on success, false on failure
 */
bool ChannelInfo::RemoveMLock(ChannelMode *mode, bool status, const Anope::string &param)
{
	if (mode->Type == MODE_REGULAR || mode->Type == MODE_PARAM)
	{
		std::multimap<ChannelModeName, ModeLock>::iterator it = this->mode_locks.find(mode->Name), it_end = this->mode_locks.upper_bound(mode->Name), it_next = it;
		if (it != this->mode_locks.end())
			for (; it != it_end; it = it_next)
			{
				const ModeLock &ml = it->second;
				++it_next;

				if (status != ml.set)
					continue;

				EventReturn MOD_RESULT;
				FOREACH_RESULT(I_OnUnMLock, OnUnMLock(this, &it->second));
				if (MOD_RESULT != EVENT_STOP)
				{
					this->mode_locks.erase(it);
					return true;
				}
			}
		return false;
	}
	else
	{
		// For list or status modes, we must check the parameter
		std::multimap<ChannelModeName, ModeLock>::iterator it = this->mode_locks.find(mode->Name);
		if (it != this->mode_locks.end())
		{
			std::multimap<ChannelModeName, ModeLock>::iterator it_end = this->mode_locks.upper_bound(mode->Name);
			for (; it != it_end; ++it)
			{
				const ModeLock &ml = it->second;
				if (ml.set == status && ml.param == param)
				{
					EventReturn MOD_RESULT;
					FOREACH_RESULT(I_OnUnMLock, OnUnMLock(this, &it->second));
					if (MOD_RESULT == EVENT_STOP)
						return false;
					this->mode_locks.erase(it);
					return true;
				}
			}
		}

		return false;
	}
}

/** Clear all mlocks on the channel
 */
void ChannelInfo::ClearMLock()
{
	this->mode_locks.clear();
}

/** Get all of the mlocks for this channel
 * @return The mlocks
 */
const std::multimap<ChannelModeName, ModeLock> &ChannelInfo::GetMLock() const
{
	return this->mode_locks;
}

/** Get a list of modes on a channel
 * @param Name The mode name to get a list of
 * @return a pair of iterators for the beginning and end of the list
 */
std::pair<ChannelInfo::ModeList::iterator, ChannelInfo::ModeList::iterator> ChannelInfo::GetModeList(ChannelModeName Name)
{
	std::multimap<ChannelModeName, ModeLock>::iterator it = this->mode_locks.find(Name), it_end = it;
	if (it != this->mode_locks.end())
		it_end = this->mode_locks.upper_bound(Name);
	return std::make_pair(it, it_end);
}

/** Get details for a specific mlock
 * @param mname The mode name
 * @param param An optional param to match with
 * @return The MLock, if any
 */
ModeLock *ChannelInfo::GetMLock(ChannelModeName mname, const Anope::string &param)
{
	std::multimap<ChannelModeName, ModeLock>::iterator it = this->mode_locks.find(mname);
	if (it != this->mode_locks.end())
	{
		if (param.empty())
			return &it->second;
		else
		{
			std::multimap<ChannelModeName, ModeLock>::iterator it_end = this->mode_locks.upper_bound(mname);
			for (; it != it_end; ++it)
			{
				if (Anope::Match(param, it->second.param))
					return &it->second;
			}
		}
	}

	return NULL;
}

Anope::string ChannelInfo::GetMLockAsString(bool complete) const
{
	Anope::string pos = "+", neg = "-", params;

	for (std::multimap<ChannelModeName, ModeLock>::const_iterator it = this->GetMLock().begin(), it_end = this->GetMLock().end(); it != it_end; ++it)
	{
		const ModeLock &ml = it->second;
		ChannelMode *cm = ModeManager::FindChannelModeByName(ml.name);
		if (!cm || cm->Type == MODE_LIST || cm->Type == MODE_STATUS)
			continue;

		if (ml.set)
			pos += cm->ModeChar;
		else
			neg += cm->ModeChar;

		if (complete && !ml.param.empty() && cm->Type == MODE_PARAM)
			params += " " + ml.param;
	}

	if (pos.length() == 1)
		pos.clear();
	if (neg.length() == 1)
		neg.clear();

	return pos + neg + params;
}

/** Check whether a user is permitted to be on this channel
 * @param u The user
 * @return true if they were banned, false if they are allowed
 */
bool ChannelInfo::CheckKick(User *user)
{
	if (!user || !this->c)
		return false;

	if (user->SuperAdmin)
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
		get_idealban(this, user, mask);
		reason = translate(user, _("This channel may not be used."));
		set_modes = true;
		do_kick = true;
	}

	if (!do_kick && matches_list(this->c, user, CMODE_EXCEPT))
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
					get_idealban(this, user, mask);
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
		get_idealban(this, user, mask);
		reason = translate(user->Account(), CHAN_NOT_ALLOWED_TO_JOIN);
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
		this->last_topic_time = this->c->topic_time;
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

int16_t ChannelInfo::GetLevel(const Anope::string &priv)
{
	if (PrivilegeManager::FindPrivilege(priv) == NULL)
	{
		Log(LOG_DEBUG) << "Unknown privilege " + priv;
		return ACCESS_INVALID;
	}

	if (this->levels.count(priv) == 0)
		this->levels[priv] = 0;
	return this->levels[priv];
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

