/* Registered channel functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */

#include "services.h"
#include "modules.h"

/** Default constructor
 * @param chname The channel name
 */
ChannelInfo::ChannelInfo(const std::string &chname)
{
	if (chname.empty())
		throw CoreException("Empty channel passed to ChannelInfo constructor");

	next = prev = NULL;
	name[0] = last_topic_setter[0] = '\0';
	founder = successor = NULL;
	desc = url = email = last_topic = forbidby = forbidreason = NULL;
	last_topic_time = 0;
	levels = NULL;
	entry_message = NULL;
	c = NULL;
	capsmin = capspercent = 0;
	floodlines = floodsecs = 0;
	repeattimes = 0;
	bi = NULL;

	this->name = chname;

	mlock_on = DefMLockOn;
	mlock_off = DefMLockOff;
	Params = DefMLockParams;

	size_t t;
	/* Set default channel flags */
	for (t = CI_BEGIN + 1; t != CI_END; ++t)
		if (Config.CSDefFlags.HasFlag(static_cast<ChannelInfoFlag>(t)))
			this->SetFlag(static_cast<ChannelInfoFlag>(t));

	/* Set default bot flags */
	for (t = BI_BEGIN + 1; t != BI_END; ++t)
		if (Config.BSDefFlags.HasFlag(static_cast<BotServFlag>(t)))
			this->botflags.SetFlag(static_cast<BotServFlag>(t));

	bantype = Config.CSDefBantype;
	memos.memomax = Config.MSMaxMemos;
	last_used = time_registered = time(NULL);

	this->ttb = new int16[2 * TTB_SIZE];
	for (int i = 0; i < TTB_SIZE; i++)
		this->ttb[i] = 0;

	reset_levels(this);
	alpha_insert_chan(this);
}

/** Default destructor, cleans up the channel complete and removes it from the internal list
 */
ChannelInfo::~ChannelInfo()
{
	unsigned i;

	FOREACH_MOD(I_OnDelChan, OnDelChan(this));

	if (this->c && this->c->HasMode(CMODE_PERM))
	{
		this->c->RemoveMode(NULL, CMODE_PERM);
		ircdproto->SendMode(whosends(this), this->c, "-%c", ModeManager::FindChannelModeByName(CMODE_PERM)->ModeChar);
	}

	Alog(LOG_DEBUG) << "Deleting channel " << this->name;

	if (this->bi)
		this->bi->chancount--;

	if (this->c)
	{
		if (this->bi && this->c->users.size() >= Config.BSMinUsers)
			ircdproto->SendPart(this->bi, this->c, NULL);
		this->c->ci = NULL;
	}

	if (this->next)
		this->next->prev = this->prev;
	if (this->prev)
		this->prev->next = this->next;
	else
		chanlists[static_cast<unsigned char>(tolower(this->name[1]))] = this->next;
	if (this->desc)
		delete [] this->desc;
	if (this->url)
		delete [] this->url;
	if (this->email)
		delete [] this->email;
	if (this->entry_message)
		delete [] this->entry_message;

	if (this->last_topic)
		delete [] this->last_topic;
	if (this->forbidby)
		delete [] this->forbidby;
	if (this->forbidreason)
		delete [] this->forbidreason;
	this->ClearAkick();
	if (this->levels)
		delete [] this->levels;

	if (!this->memos.memos.empty())
	{
		for (i = 0; i < this->memos.memos.size(); ++i)
		{
			if (this->memos.memos[i]->text)
				delete [] this->memos.memos[i]->text;
			delete this->memos.memos[i];
		}
		this->memos.memos.clear();
	}

	if (this->ttb)
		delete [] this->ttb;

	if (this->founder)
		this->founder->channelcount--;
}

/** Add an entry to the channel access list
 *
 * @param nc The NickCore of the user that the access entry should be tied to
 * @param level The channel access level the user has on the channel
 * @param creator The user who added the access
 * @param last_seen When the user was last seen within the channel
 *
 * Creates a new access list entry and inserts it into the access list.
 */

void ChannelInfo::AddAccess(NickCore *nc, int16 level, const std::string &creator, int32 last_seen)
{
	ChanAccess *new_access = new ChanAccess();
	new_access->in_use = 1;
	new_access->nc = nc;
	new_access->level = level;
	new_access->last_seen = last_seen;
	if (!creator.empty())
		new_access->creator = creator;
	else
		new_access->creator = "Unknown";

	access.push_back(new_access);
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
	if (access.empty() || index >= access.size())
		return NULL;

	return access[index];
}

/** Get an entry from the channel access list by NickCore
 *
 * @param nc The NickCore to find within the access list vector
 * @param level Optional channel access level to compare the access entries to
 * @return A ChanAccess struct corresponding to the NickCore, or NULL if not found
 *
 * Retrieves an entry from the access list that matches the given NickCore, optionally also matching a
certain level.
 */

ChanAccess *ChannelInfo::GetAccess(NickCore *nc, int16 level)
{
	if (access.empty())
		return NULL;

	for (unsigned i = 0; i < access.size(); i++)
		if (access[i]->in_use && access[i]->nc == nc && (level ? access[i]->level == level : true))
			return access[i];

	return NULL;
}

/** Get the size of the accss vector for this channel
 * @return The access vector size
 */
const unsigned ChannelInfo::GetAccessCount() const
{
	return access.empty() ? 0 : access.size();
}


/** Erase an entry from the channel access list
 *
 * @param index The index in the access list vector
 *
 * Clears the memory used by the given access entry and removes it from the vector.
 */
void ChannelInfo::EraseAccess(unsigned index)
{
	if (access.empty() || index >= access.size())
		return;
	delete access[index];
	access.erase(access.begin() + index);
}

/** Cleans the channel access list
 *
 * Cleans up the access list so it no longer contains entries no longer in use.
 */
void ChannelInfo::CleanAccess()
{
	for (unsigned j = access.size(); j > 0; --j)
		if (!access[j - 1]->in_use)
			EraseAccess(j - 1);
}

/** Clear the entire channel access list
 *
 * Clears the entire access list by deleting every item and then clearing the vector.
 */
void ChannelInfo::ClearAccess()
{
	while (access.begin() != access.end())
		EraseAccess(0);
}

/** Add an akick entry to the channel by NickCore
 * @param user The user who added the akick
 * @param akicknc The nickcore being akicked
 * @param reason The reason for the akick
 * @param t The time the akick was added, defaults to now
 * @return The AutoKick structure
 */
AutoKick *ChannelInfo::AddAkick(const std::string &user, NickCore *akicknc, const std::string &reason, time_t t)
{
	if (!akicknc)
		return NULL;

	AutoKick *autokick = new AutoKick();
	autokick->InUse = true;
	autokick->SetFlag(AK_ISNICK);
	autokick->nc = akicknc;
	autokick->reason = reason;
	autokick->creator = user;
	autokick->addtime = t;

	akick.push_back(autokick);
	return autokick;
}

/** Add an akick entry to the channel by reason
 * @param user The user who added the akick
 * @param mask The mask of the akick
 * @param reason The reason for the akick
 * @param t The time the akick was added, defaults to now
 * @return The AutoKick structure
 */
AutoKick *ChannelInfo::AddAkick(const std::string &user, const std::string &mask, const std::string &reason, time_t t)
{
	AutoKick *autokick = new AutoKick();
	autokick->mask = mask;
	autokick->InUse = true;
	autokick->reason = reason;
	autokick->creator = user;
	autokick->addtime = t;

	akick.push_back(autokick);
	return autokick;
}

/** Get an entry from the channel akick list
 * @param index The index in the akick vector
 * @return The akick structure, or NULL if not found
 */
AutoKick *ChannelInfo::GetAkick(unsigned index)
{
	if (akick.empty() || index >= akick.size())
		return NULL;

	return akick[index];
}

/** Get the size of the akick vector for this channel
 * @return The akick vector size
 */
const unsigned ChannelInfo::GetAkickCount() const
{
	return akick.empty() ? 0 : akick.size();
}

/** Erase an entry from the channel akick list
 * @param akick The akick
 */
void ChannelInfo::EraseAkick(AutoKick *autokick)
{
	std::vector<AutoKick *>::iterator it = std::find(akick.begin(), akick.end(), autokick);

	if (it != akick.end())
	{
		delete *it;
		akick.erase(it);
	}
}

/** Clear the whole akick list
 */
void ChannelInfo::ClearAkick()
{
	for (unsigned i = akick.size(); i > 0; --i)
	{
		EraseAkick(akick[i - 1]);
	}
}

/** Clean all of the nonused entries from the akick list
 */
void ChannelInfo::CleanAkick()
{
	for (unsigned i = akick.size(); i > 0; --i)
		if (!akick[i - 1]->InUse)
			EraseAkick(akick[i - 1]);
}

/** Add a badword to the badword list
 * @param word The badword
 * @param type The type (SINGLE START END)
 * @return The badword
 */
BadWord *ChannelInfo::AddBadWord(const std::string &word, BadWordType type)
{
	BadWord *bw = new BadWord;
	bw->InUse = true;
	bw->word = word;
	bw->type = type;

	badwords.push_back(bw);
	return bw;
}

/** Get a badword structure by index
 * @param index The index
 * @return The badword
 */
BadWord *ChannelInfo::GetBadWord(unsigned index)
{
	if (badwords.empty() || index >= badwords.size())
		return NULL;

	return badwords[index];
}

/** Get how many badwords are on this channel
 * @return The number of badwords in the vector
 */
const unsigned ChannelInfo::GetBadWordCount() const
{
	return badwords.empty() ? 0 : badwords.size();
}

/** Remove a badword
 * @param badword The badword
 */
void ChannelInfo::EraseBadWord(BadWord *badword)
{
	std::vector<BadWord *>::iterator it = std::find(badwords.begin(), badwords.end(), badword);

	if (it != badwords.end())
	{
		delete *it;
		badwords.erase(it);
	}
}

/** Clear all badwords from the channel
 */
void ChannelInfo::ClearBadWords()
{
	for (unsigned i = badwords.size(); i > 0; --i)
	{
		EraseBadWord(badwords[i - 1]);
	}
}

/** Clean all of the nonused entries from the badwords list
 */
void ChannelInfo::CleanBadWords()
{
	for (unsigned i = badwords.size(); i > 0; --i)
		if (!badwords[i - 1]->InUse)
			EraseBadWord(badwords[i - 1]);
}

/** Check if a mode is mlocked
 * @param Name The mode
 * @param status True to check mlock on, false for mlock off
 * @return true on success, false on fail
 */
const bool ChannelInfo::HasMLock(ChannelModeName Name, bool status)
{
	if (status)
		return mlock_on[Name];
	else
		return mlock_off[Name];
}

/** Set a mlock
 * @param Name The mode
 * @param status True for mlock on, false for mlock off
 * @param param The param to use for this mode, if required
 * @return true on success, false on failure (module blocking)
 */
bool ChannelInfo::SetMLock(ChannelModeName Name, bool status, const std::string param)
{
	size_t value = Name;

	if (!status && !param.empty())
		throw CoreException("Was told to mlock a mode negatively with a param?");

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnMLock, OnMLock(Name, status, param));
	if (MOD_RESULT == EVENT_STOP)
		return false;

	/* First, remove this everywhere */
	mlock_on[value] = false;
	mlock_off[value] = false;

	std::map<ChannelModeName, std::string>::iterator it = Params.find(Name);
	if (it != Params.end())
	{
		Params.erase(it);
	}

	if (status)
		mlock_on[value] = true;
	else
		mlock_off[value] = true;
	
	if (status && !param.empty())
	{
		Params.insert(std::make_pair(Name, param));
	}

	return true;
}

/** Remove a mlock
 * @param Name The mode
 * @return true on success, false on failure (module blocking)
 */
bool ChannelInfo::RemoveMLock(ChannelModeName Name)
{
	size_t value = Name;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnUnMLock, OnUnMLock(Name));
	if (MOD_RESULT == EVENT_STOP)
		return false;

	mlock_on[value] = false;
	mlock_off[value] = false;
	
	std::map<ChannelModeName, std::string>::iterator it = Params.find(Name);
	if (it != Params.end())
	{
		Params.erase(it);
	}

	return true;
}

/** Clear all mlocks on the channel
 */
void ChannelInfo::ClearMLock()
{
	mlock_on.reset();
	mlock_off.reset();
}

/** Get the number of mlocked modes for this channel
 * @param status true for mlock on, false for mlock off
 * @return The number of mlocked modes
 */
const size_t ChannelInfo::GetMLockCount(bool status) const
{
	if (status)
		return mlock_on.count();
	else
		return mlock_off.count();
}

/** Get a param from the channel
 * @param Name The mode
 * @param Target a string to put the param into
 * @return true on success
 */
const bool ChannelInfo::GetParam(ChannelModeName Name, std::string &Target)
{
	std::map<ChannelModeName, std::string>::iterator it = Params.find(Name);

	Target.clear();

	if (it != Params.end())
	{
		Target = it->second;
		return true;
	}

	return false;
}

/** Check if a mode is set and has a param
 * @param Name The mode
 */
const bool ChannelInfo::HasParam(ChannelModeName Name)
{
	std::map<ChannelModeName, std::string>::iterator it = Params.find(Name);

	if (it != Params.end())
	{
		return true;
	}

	return false;
}

/** Clear all the params from the channel
 */
void ChannelInfo::ClearParams()
{
	Params.clear();
}

