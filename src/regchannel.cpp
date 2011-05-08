/* Registered channel functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "chanserv.h"
#include "nickserv.h"

ChanAccess::ChanAccess(const Anope::string &umask)
{
	NickAlias *na = findnick(umask);
	if (na != NULL)
		this->nc = na->nc;
	else
	{
		this->nc = NULL;
		this->mask = umask;
		if (ircdproto->IsNickValid(this->mask))
			this->mask += "!*@*";
	}
}

const Anope::string &ChanAccess::GetMask()
{
	if (this->nc != NULL)
		return this->nc->display;
	return this->mask;
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
	this->levels = NULL;
	this->c = findchan(chname);
	if (this->c)
		this->c->ci = this;
	this->capsmin = this->capspercent = 0;
	this->floodlines = this->floodsecs = 0;
	this->repeattimes = 0;
	this->bi = NULL;

	this->name = chname;

	this->mode_locks = def_mode_locks;

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

	this->ttb = new int16[TTB_SIZE];
	for (int i = 0; i < TTB_SIZE; ++i)
		this->ttb[i] = 0;

	reset_levels(this);

	RegisteredChannelList[this->name] = this;
}

/** Copy constructor
 * @param ci The ChannelInfo to copy settings to
 */
ChannelInfo::ChannelInfo(ChannelInfo *ci) : Flags<ChannelInfoFlag, CI_END>(ChannelInfoFlagStrings), botflags(BotServFlagStrings)
{
	*this = *ci;

	if (this->founder)
		++this->founder->channelcount;

	this->access.clear();
	this->akick.clear();
	this->badwords.clear();

	if (this->bi)
		++this->bi->chancount;

	this->ttb = new int16[TTB_SIZE];
	for (int i = 0; i < TTB_SIZE; ++i)
		this->ttb[i] = ci->ttb[i];

	this->levels = new int16[CA_SIZE];
	for (int i = 0; i < CA_SIZE; ++i)
		this->levels[i] = ci->levels[i];

	for (unsigned i = 0; i < ci->GetAccessCount(); ++i)
	{
		ChanAccess *taccess = ci->GetAccess(i);
		this->AddAccess(taccess->GetMask(), taccess->level, taccess->creator, taccess->last_seen);
	}
	for (unsigned i = 0; i < ci->GetAkickCount(); ++i)
	{
		AutoKick *takick = ci->GetAkick(i);
		if (takick->HasFlag(AK_ISNICK))
			this->AddAkick(takick->creator, takick->nc, takick->reason, takick->addtime, takick->last_used);
		else
			this->AddAkick(takick->creator, takick->mask, takick->reason, takick->addtime, takick->last_used);
	}
	for (unsigned i = 0; i < ci->GetBadWordCount(); ++i)
	{
		BadWord *bw = ci->GetBadWord(i);
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
	if (this->levels)
		delete [] this->levels;

	if (!this->memos.memos.empty())
	{
		for (i = 0, end = this->memos.memos.size(); i < end; ++i)
			delete this->memos.memos[i];
		this->memos.memos.clear();
	}

	if (this->ttb)
		delete [] this->ttb;

	if (this->founder)
		--this->founder->channelcount;
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
	if (!this || !this->bi || !this->c || !this->botflags.HasFlag(BS_SYMBIOSIS) || !this->c->FindUser(this->bi))
		return chanserv ? chanserv->Bot() : (nickserv ? nickserv->Bot() : NULL);
	return this->bi;
}

/** Add an entry to the channel access list
 *
 * @param mask The mask of the access entry
 * @param level The channel access level the user has on the channel
 * @param creator The user who added the access
 * @param last_seen When the user was last seen within the channel
 * @return The new access class
 *
 * Creates a new access list entry and inserts it into the access list.
 */

ChanAccess *ChannelInfo::AddAccess(const Anope::string &mask, int16 level, const Anope::string &creator, int32 last_seen)
{
	ChanAccess *new_access = new ChanAccess(mask);
	new_access->level = level;
	new_access->last_seen = last_seen;
	if (!creator.empty())
		new_access->creator = creator;
	else
		new_access->creator = "Unknown";

	this->access.push_back(new_access);

	return new_access;
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

/** Get an entry from the channel access list by NickCore
 *
 * @param u The User to find within the access list vector
 * @param level Optional channel access level to compare the access entries to
 * @return A ChanAccess struct corresponding to the NickCore, or NULL if not found
 *
 * Retrieves an entry from the access list that matches the given NickCore, optionally also matching a certain level.
 */

ChanAccess *ChannelInfo::GetAccess(User *u, int16 level)
{
	if (!u)
		return NULL;

	if (u->isSuperAdmin || IsFounder(u, this))
	{
		static ChanAccess dummy_access("");
		new(&dummy_access) ChanAccess(u->nick + "!*@*");
		dummy_access.level = u->isSuperAdmin ? ACCESS_SUPERADMIN : ACCESS_FOUNDER;
		dummy_access.last_seen = Anope::CurTime;
		return &dummy_access;
	}
	
	if (this->access.empty())
		return NULL;
	
	ChanAccess *highest = NULL;
	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
	{
		ChanAccess *taccess = this->access[i];

		if (level && level != taccess->level)
			continue;
		/* Access entry is a mask and we match it */
		else if (!taccess->nc && (Anope::Match(u->nick, taccess->GetMask()) || Anope::Match(u->GetDisplayedMask(), taccess->GetMask())))
			;
		/* Access entry is a nick core and we are identified for that account */
		else if (taccess->nc && (u->IsIdentified() || (u->IsRecognized() && !this->HasFlag(CI_SECURE))) && u->Account() == taccess->nc) 
			;
		else
			continue;

		/* Use the highest level access available */
		if (!highest || taccess->level > highest->level)
			highest = taccess;
	}

	return highest;
}

/** Get an entry from the channel access list by NickCore
 *
 * @param u The NickCore to find within the access list vector
 * @param level Optional channel access level to compare the access entries to
 * @return A ChanAccess struct corresponding to the NickCore, or NULL if not found
 *
 * Retrieves an entry from the access list that matches the given NickCore, optionally also matching a certain level.
 */
ChanAccess *ChannelInfo::GetAccess(NickCore *nc, int16 level)
{
	if (nc == this->founder)
	{
		static ChanAccess dummy_access("");
		new(&dummy_access) ChanAccess(nc->display);
		dummy_access.level = ACCESS_FOUNDER;
		dummy_access.last_seen = Anope::CurTime;
		return &dummy_access;
	}
	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
	{
		if (level && this->access[i]->level != level)
			continue;
		if (this->access[i]->nc && this->access[i]->nc == nc)
			return this->access[i];
	}
	return NULL;
}

/** Get an entry from the channel access list by mask
 *
 * @param u The mask to find within the access list vector
 * @param level Optional channel access level to compare the access entries to
 * @param wildcard True to match using wildcards
 * @return A ChanAccess struct corresponding to the mask, or NULL if not found
 *
 * Retrieves an entry from the access list that matches the given mask, optionally also matching a certain level.
 */
ChanAccess *ChannelInfo::GetAccess(const Anope::string &mask, int16 level, bool wildcard)
{
	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
		if (this->access[i]->nc != NULL)
		{
			if (wildcard ? Anope::Match(this->access[i]->nc->display, mask) : this->access[i]->nc->display.equals_ci(mask))
				return this->access[i];
		}
		else if (wildcard ? Anope::Match(this->access[i]->GetMask(), mask) : this->access[i]->GetMask().equals_ci(mask))
			return this->access[i];
	return NULL;
}

/** Get the size of the accss vector for this channel
 * @return The access vector size
 */
unsigned ChannelInfo::GetAccessCount() const
{
	return this->access.empty() ? 0 : this->access.size();
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
	autokick->mask = mask;
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

/** Loads MLocked modes from extensible. This is used from database loading because Anope doesn't know what modes exist
 * until after it connects to the IRCd.
 */
void ChannelInfo::LoadMLock()
{
	this->ClearMLock();

	// Force +r
	ChannelMode *chm = ModeManager::FindChannelModeByName(CMODE_REGISTERED);
	if (chm)
		this->SetMLock(chm, true);

	std::vector<Anope::string> mlock;
	if (this->GetExtRegular("db_mlock", mlock))
	{
		for (unsigned i = 0; i < mlock.size(); ++i)
		{
			std::vector<Anope::string> mlockv = BuildStringVector(mlock[i]);

			bool set = mlockv[0] == "1";
			ChannelMode *cm = ModeManager::FindChannelModeByString(mlockv[1]);
			const Anope::string &setter = mlockv[2];
			time_t created = Anope::CurTime;
			try
			{
				created = convertTo<time_t>(mlockv[3]);
			}
			catch (const ConvertException &) { }
			const Anope::string &param = mlockv.size() > 4 ? mlockv[4] : "";

			if (cm != NULL)
				this->SetMLock(cm, set, param, setter, created);
		}

		this->Shrink("db_mlock");
	}

	/* Create perm channel */
	if (this->HasFlag(CI_PERSIST) && !this->c)
	{
		this->c = new Channel(this->name, this->time_registered);
		if (ModeManager::FindChannelModeByName(CMODE_PERM) != NULL)
		{
			/* At this point, CMODE_PERM *must* be locked on the channel, so this is fine */
			ircdproto->SendChannel(this->c);
			this->c->Reset();
		}
		else
		{
			if (!this->bi)
				this->WhoSends()->Assign(NULL, this);
			if (this->c->FindUser(this->bi) == NULL)
				this->bi->Join(this->c);

			check_modes(this->c);
			this->RestoreTopic();
		}
	}
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
	std::pair<ChannelModeName, ModeLock> ml = std::make_pair(mode->Name, ModeLock(status, mode->Name, param, setter, created));

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
 * @param param The param of the mode, required if it is a list or status mode
 * @return true on success, false on failure
 */
bool ChannelInfo::RemoveMLock(ChannelMode *mode, const Anope::string &param)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnUnMLock, OnUnMLock(this, mode, param));
	if (MOD_RESULT == EVENT_STOP)
		return false;

	if (mode->Type == MODE_REGULAR || mode->Type == MODE_PARAM)
		return this->mode_locks.erase(mode->Name) > 0;
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
				if (ml.param == param)
				{
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

	if (user->isSuperAdmin)
		return false;

	/* We don't enforce services restrictions on clients on ulined services
	 * as this will likely lead to kick/rejoin floods. ~ Viper */
	if (user->server->IsULined())
		return false;

	if (user->IsProtected())
		return false;

	bool set_modes = false, do_kick = false;
	if (ircd->chansqline && SQLineManager::Check(this->c))
		do_kick = true;

	Anope::string mask, reason;
	if (!user->HasMode(UMODE_OPER) && this->HasFlag(CI_SUSPENDED))
	{
		get_idealban(this, user, mask);
		reason = GetString(user->Account(), _("This channel may not be used."));
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

	if (!do_kick && check_access(user, this, CA_NOJOIN))
	{
		get_idealban(this, user, mask);
		reason = GetString(user->Account(), _(CHAN_NOT_ALLOWED_TO_JOIN));
		do_kick = true;
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
		new ChanServTimer(this->c);
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

