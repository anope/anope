/* Registered channel functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"
#include "language.h"

/** Default constructor
 * @param chname The channel name
 */
ChannelInfo::ChannelInfo(const Anope::string &chname)
{
	if (chname.empty())
		throw CoreException("Empty channel passed to ChannelInfo constructor");

	this->founder = this->successor = NULL;
	this->last_topic_time = 0;
	this->levels = NULL;
	this->c = NULL;
	this->capsmin = this->capspercent = 0;
	this->floodlines = this->floodsecs = 0;
	this->repeattimes = 0;
	this->bi = NULL;

	this->name = chname;

	this->mlock_on = DefMLockOn;
	this->mlock_off = DefMLockOff;
	this->Params = DefMLockParams;

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
	this->last_used = this->time_registered = time(NULL);

	this->ttb = new int16[2 * TTB_SIZE];
	for (int i = 0; i < TTB_SIZE; ++i)
		this->ttb[i] = 0;

	reset_levels(this);

	RegisteredChannelList[this->name] = this;
}

/** Default destructor, cleans up the channel complete and removes it from the internal list
 */
ChannelInfo::~ChannelInfo()
{
	unsigned i, end;

	FOREACH_MOD(I_OnDelChan, OnDelChan(this));

	Alog(LOG_DEBUG) << "Deleting channel " << this->name;

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

/** Add an entry to the channel access list
 *
 * @param nc The NickCore of the user that the access entry should be tied to
 * @param level The channel access level the user has on the channel
 * @param creator The user who added the access
 * @param last_seen When the user was last seen within the channel
 *
 * Creates a new access list entry and inserts it into the access list.
 */

void ChannelInfo::AddAccess(NickCore *nc, int16 level, const Anope::string &creator, int32 last_seen)
{
	ChanAccess *new_access = new ChanAccess();
	new_access->nc = nc;
	new_access->level = level;
	new_access->last_seen = last_seen;
	if (!creator.empty())
		new_access->creator = creator;
	else
		new_access->creator = "Unknown";

	this->access.push_back(new_access);
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
 * @param nc The NickCore to find within the access list vector
 * @param level Optional channel access level to compare the access entries to
 * @return A ChanAccess struct corresponding to the NickCore, or NULL if not found
 *
 * Retrieves an entry from the access list that matches the given NickCore, optionally also matching a certain level.
 */

ChanAccess *ChannelInfo::GetAccess(const NickCore *nc, int16 level)
{
	if (this->access.empty())
		return NULL;

	for (unsigned i = 0, end = this->access.size(); i < end; ++i)
		if (this->access[i]->nc == nc && (level ? this->access[i]->level == level : true))
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

/** Clear the entire channel access list
 *
 * Clears the entire access list by deleting every item and then clearing the vector.
 */
void ChannelInfo::ClearAccess()
{
	while (!this->access.empty())
		EraseAccess(0);
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
	std::vector<Anope::string> modenames;

	if (this->GetExtRegular("db_mlock_modes_on", modenames))
	{
		for (std::vector<Anope::string>::iterator it = modenames.begin(), it_end = modenames.end(); it != it_end; ++it)
		{
			for (std::list<Mode *>::iterator mit = ModeManager::Modes.begin(), mit_end = ModeManager::Modes.end(); mit != mit_end; ++mit)
			{
				if ((*mit)->Class == MC_CHANNEL)
				{
					ChannelMode *cm = debug_cast<ChannelMode *>(*mit);

					if (cm->NameAsString.equals_ci(*it))
						this->SetMLock(cm->Name, true);
				}
			}
		}

		this->Shrink("db_mlock_modes_on");
	}

	if (this->GetExtRegular("db_mlock_modes_off", modenames))
	{
		for (std::vector<Anope::string>::iterator it = modenames.begin(), it_end = modenames.end(); it != it_end; ++it)
		{
			for (std::list<Mode *>::iterator mit = ModeManager::Modes.begin(), mit_end = ModeManager::Modes.end(); mit != mit_end; ++mit)
			{
				if ((*mit)->Class == MC_CHANNEL)
				{
					ChannelMode *cm = debug_cast<ChannelMode *>(*mit);

					if (cm->NameAsString.equals_ci(*it))
						this->SetMLock(cm->Name, false);
				}
			}
		}

		this->Shrink("db_mlock_modes_off");
	}

	std::vector<std::pair<Anope::string, Anope::string> > params;

	if (this->GetExtRegular("db_mlp", params))
	{
		for (std::vector<std::pair<Anope::string, Anope::string> >::iterator it = params.begin(), it_end = params.end(); it != it_end; ++it)
		{
			for (std::list<Mode *>::iterator mit = ModeManager::Modes.begin(), mit_end = ModeManager::Modes.end(); mit != mit_end; ++mit)
			{
				if ((*mit)->Class == MC_CHANNEL)
				{
					ChannelMode *cm = debug_cast<ChannelMode *>(*mit);

					if (cm->NameAsString.equals_ci(it->first))
						this->SetMLock(cm->Name, true, it->second);
				}
			}
		}

		this->Shrink("db_mlp");
	}
}

/** Check if a mode is mlocked
 * @param Name The mode
 * @param status True to check mlock on, false for mlock off
 * @return true on success, false on fail
 */
bool ChannelInfo::HasMLock(ChannelModeName Name, bool status) const
{
	if (status)
		return this->mlock_on.HasFlag(Name);
	else
		return this->mlock_off.HasFlag(Name);
}

/** Set a mlock
 * @param Name The mode
 * @param status True for mlock on, false for mlock off
 * @param param The param to use for this mode, if required
 * @return true on success, false on failure (module blocking)
 */
bool ChannelInfo::SetMLock(ChannelModeName Name, bool status, const Anope::string &param)
{
	if (!status && !param.empty())
		throw CoreException("Was told to mlock a mode negatively with a param?");

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnMLock, OnMLock(Name, status, param));
	if (MOD_RESULT == EVENT_STOP)
		return false;

	/* First, remove this everywhere */
	this->mlock_on.UnsetFlag(Name);
	this->mlock_off.UnsetFlag(Name);

	std::map<ChannelModeName, Anope::string>::iterator it = Params.find(Name);
	if (it != Params.end())
		Params.erase(it);

	if (status)
		this->mlock_on.SetFlag(Name);
	else
		this->mlock_off.SetFlag(Name);

	if (status && !param.empty())
		this->Params.insert(std::make_pair(Name, param));

	return true;
}

/** Remove a mlock
 * @param Name The mode
 * @return true on success, false on failure (module blocking)
 */
bool ChannelInfo::RemoveMLock(ChannelModeName Name)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnUnMLock, OnUnMLock(Name));
	if (MOD_RESULT == EVENT_STOP)
		return false;

	this->mlock_on.UnsetFlag(Name);
	this->mlock_off.UnsetFlag(Name);

	std::map<ChannelModeName, Anope::string>::iterator it = Params.find(Name);
	if (it != Params.end())
		this->Params.erase(it);

	return true;
}

/** Clear all mlocks on the channel
 */
void ChannelInfo::ClearMLock()
{
	this->mlock_on.ClearFlags();
	this->mlock_off.ClearFlags();
}

/** Get the number of mlocked modes for this channel
 * @param status true for mlock on, false for mlock off
 * @return The number of mlocked modes
 */
size_t ChannelInfo::GetMLockCount(bool status) const
{
	if (status)
		return this->mlock_on.FlagCount();
	else
		return this->mlock_off.FlagCount();
}

/** Get a param from the channel
 * @param Name The mode
 * @param Target a string to put the param into
 * @return true on success
 */
bool ChannelInfo::GetParam(ChannelModeName Name, Anope::string &Target) const
{
	std::map<ChannelModeName, Anope::string>::const_iterator it = this->Params.find(Name);

	Target.clear();

	if (it != this->Params.end())
	{
		Target = it->second;
		return true;
	}

	return false;
}

/** Check if a mode is set and has a param
 * @param Name The mode
 */
bool ChannelInfo::HasParam(ChannelModeName Name) const
{
	std::map<ChannelModeName, Anope::string>::const_iterator it = this->Params.find(Name);

	if (it != this->Params.end())
		return true;

	return false;
}

/** Clear all the params from the channel
 */
void ChannelInfo::ClearParams()
{
	Params.clear();
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
	if (!is_oper(user) && (this->HasFlag(CI_SUSPENDED) || this->HasFlag(CI_FORBIDDEN)))
	{
		get_idealban(this, user, mask);
		reason = this->forbidreason.empty() ? getstring(user, CHAN_MAY_NOT_BE_USED) : this->forbidreason;
		set_modes = true;
		do_kick = true;
	}

	if (!do_kick && ModeManager::FindChannelModeByName(CMODE_EXCEPT) && is_excepted(this, user) == 1)
		return false;

	const NickCore *nc = user->Account() || user->IsRecognized() ? user->Account() : NULL;

	if (!do_kick)
	{
		for (unsigned j = 0, end = this->GetAkickCount(); j < end; ++j)
		{
			AutoKick *autokick = this->GetAkick(j);

			if ((autokick->HasFlag(AK_ISNICK) && autokick->nc == nc) || (!autokick->HasFlag(AK_ISNICK) && match_usermask(autokick->mask, user)))
			{
				Alog(LOG_DEBUG_2) << user->nick << " matched akick " << (autokick->HasFlag(AK_ISNICK) ? autokick->nc->display : autokick->mask);
				autokick->last_used = time(NULL);
				if (autokick->HasFlag(AK_ISNICK))
					get_idealban(this, user, mask);
				else
					mask = autokick->mask;
				reason = autokick->reason.empty() ? Config->CSAutokickReason : autokick->reason;
				do_kick = true;
				break;
			}
		}
	}

	if (!do_kick && check_access(user, this, CA_NOJOIN))
	{
		get_idealban(this, user, mask);
		reason = getstring(user, CHAN_NOT_ALLOWED_TO_JOIN);
		do_kick = true;
	}

	if (!do_kick)
		return false;

	Alog(LOG_DEBUG) << "Autokicking "<< user->GetMask() <<  " from " << this->name;

	/* If the channel isn't syncing and doesn't have any users, join ChanServ
	 * Note that the user AND POSSIBLY the botserv bot exist here
	 * ChanServ always enforces channels like this to keep people from deleting bots etc
	 * that are holding channels.
	 */
	if (this->c->users.size() == (this->bi ? 2 : 1) && !this->HasFlag(CI_INHABIT) && !this->c->HasFlag(CH_SYNCING))
	{
		/* If channel was forbidden, etc, set it +si to prevent rejoin */
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
