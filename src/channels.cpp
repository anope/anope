/* Channel-handling routines.
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

channel_map ChannelList;

/** Default constructor
 * @param name The channel name
 * @param ts The time the channel was created
 */
Channel::Channel(const Anope::string &nname, time_t ts)
{
	if (nname.empty())
		throw CoreException("A channel without a name ?");

	this->name = nname;

	ChannelList[this->name] = this;

	this->creation_time = ts;
	this->bans = this->excepts = this->invites = NULL;
	this->server_modetime = this->chanserv_modetime = 0;
	this->server_modecount = this->chanserv_modecount = this->bouncy_modes = this->topic_time = 0;

	this->ci = cs_findchan(this->name);
	if (this->ci)
		this->ci->c = this;

	Log(NULL, this, "create");

	FOREACH_MOD(I_OnChannelCreate, OnChannelCreate(this));
}

/** Default destructor
 */
Channel::~Channel()
{
	FOREACH_MOD(I_OnChannelDelete, OnChannelDelete(this));

	ModeManager::StackerDel(this);

	Log(NULL, this, "destroy");

	for (std::list<BanData *>::iterator it = this->bd.begin(), it_end = this->bd.end(); it != it_end; ++it)
		delete *it;

	if (this->ci)
		this->ci->c = NULL;

	if (this->bans && this->bans->count)
		while (this->bans->entries)
			entry_delete(this->bans, this->bans->entries);

	if (ModeManager::FindChannelModeByName(CMODE_EXCEPT) && this->excepts && this->excepts->count)
		while (this->excepts->entries)
			entry_delete(this->excepts, this->excepts->entries);

	if (ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE) && this->invites && this->invites->count)
		while (this->invites->entries)
			entry_delete(this->invites, this->invites->entries);

	ChannelList.erase(this->name);
}

void Channel::Reset()
{
	this->ClearModes(NULL, false);
	this->ClearBans(NULL, false);
	this->ClearExcepts(NULL, false);
	this->ClearInvites(NULL, false);

	for (CUserList::const_iterator it = this->users.begin(), it_end = this->users.end(); it != it_end; ++it)
	{
		UserContainer *uc = *it;

		Flags<ChannelModeName, CMODE_END * 2> flags = *debug_cast<Flags<ChannelModeName, CMODE_END * 2> *>(uc->Status);
		uc->Status->ClearFlags();

		if (findbot(uc->user->nick))
		{
			for (std::map<char, ChannelMode *>::iterator mit = ModeManager::ChannelModesByChar.begin(), mit_end = ModeManager::ChannelModesByChar.end(); mit != mit_end; ++mit)
			{
				ChannelMode *cm = mit->second;

				if (flags.HasFlag(cm->Name))
				{
					this->SetMode(NULL, cm, uc->user->nick, false);
				}
			}
		}
	}

	check_modes(this);
	for (CUserList::const_iterator it = this->users.begin(), it_end = this->users.end(); it != it_end; ++it)
		chan_set_correct_modes((*it)->user, this, 1);
}

void Channel::Sync()
{
	if (!this->HasMode(CMODE_PERM) && (this->users.empty() || (this->users.size() == 1 && this->ci && this->ci->bi == this->users.front()->user)))
	{
		new ChanServTimer(this);
	}
	if (this->ci)
	{
		check_modes(this);

		if (Me && Me->IsSynced())
			this->ci->RestoreTopic();
	}
}

void Channel::JoinUser(User *user)
{
	Log(user, this, "join");

	ChannelStatus *Status = new ChannelStatus();
	ChannelContainer *cc = new ChannelContainer(this);
	cc->Status = Status;
	user->chans.push_back(cc);

	UserContainer *uc = new UserContainer(user);
	uc->Status = Status;
	this->users.push_back(uc);

	bool update_ts = false;
	if (this->ci && this->ci->HasFlag(CI_PERSIST) && this->creation_time > this->ci->time_registered)
	{
		Log(LOG_DEBUG) << "Changing TS of " << this->name << " from " << this->creation_time << " to " << this->ci->time_registered;
		this->creation_time = this->ci->time_registered;
		update_ts = true;
	}

	if (!get_ignore(user->nick))
	{
		if (this->ci && check_access(user, this->ci, CA_MEMO) && this->ci->memos.memos.size() > 0)
		{
			if (this->ci->memos.memos.size() == 1)
				user->SendMessage(MemoServ, MEMO_X_ONE_NOTICE, this->ci->memos.memos.size(), this->ci->name.c_str());
			else
				user->SendMessage(MemoServ, MEMO_X_MANY_NOTICE, this->ci->memos.memos.size(), this->ci->name.c_str());
		}
		/* Added channelname to entrymsg - 30.03.2004, Certus */
		/* Also, don't send the entrymsg when bursting -GD */
		if (this->ci && !this->ci->entry_message.empty() && user->server->IsSynced())
			user->SendMessage(whosends(this->ci)->nick, "[%s] %s", this->name.c_str(), this->ci->entry_message.c_str());
	}

	if (!Config->s_BotServ.empty() && this->ci && this->ci->bi)
	{
		/**
		 * We let the bot join even if it was an ignored user, as if we don't,
		 * and the ignored user doesnt just leave, the bot will never
		 * make it into the channel, leaving the channel botless even for
		 * legit users - Rob
		 **/
		if (this->users.size() >= Config->BSMinUsers && !this->FindUser(this->ci->bi))
			this->ci->bi->Join(this, update_ts);
		/* Only display the greet if the main uplink we're connected
		 * to has synced, or we'll get greet-floods when the net
		 * recovers from a netsplit. -GD
		 */
		if (this->FindUser(this->ci->bi) && this->ci->botflags.HasFlag(BS_GREET) && user->Account() && !user->Account()->greet.empty() && check_access(user, this->ci, CA_GREET) && user->server->IsSynced())
		{
			ircdproto->SendPrivmsg(this->ci->bi, this->name, "[%s] %s", user->Account()->display.c_str(), user->Account()->greet.c_str());
			this->ci->bi->lastmsg = Anope::CurTime;
		}
	}

	/* Update the TS, unless I'm joining a bot already */
	if (update_ts && user->server != Me)
	{
		/* Send the updated TS */
		if (!this->FindUser(whosends(this->ci)))
		{
			whosends(this->ci)->Join(this, update_ts);
			whosends(this->ci)->Part(this);
		}
	}
}

/** Remove a user internally from the channel
 * @param u The user
 */
void Channel::DeleteUser(User *user)
{
	if (this->ci)
		update_cs_lastseen(user, this->ci);

	Log(user, this, "leaves");

	CUserList::iterator cit, cit_end = this->users.end();
	for (cit = this->users.begin(); (*cit)->user != user && cit != cit_end; ++cit);
	if (cit == cit_end)
	{
		Log(LOG_DEBUG) << "Channel::DeleteUser() tried to delete nonexistant user " << user->nick << " from channel " << this->name;
		return;
	}

	delete (*cit)->Status;
	delete *cit;
	this->users.erase(cit);

	UChannelList::iterator uit, uit_end = user->chans.end();
	for (uit = user->chans.begin(); (*uit)->chan != this && uit != uit_end; ++uit);
	if (uit == uit_end)
	{
		Log(LOG_DEBUG) << "Channel::DeleteUser() tried to delete nonexistant channel " << this->name << " from " << user->nick << "'s channel list";
		return;
	}

	delete *uit;
	user->chans.erase(uit);

	/* Channel is persistant, it shouldn't be deleted and the service bot should stay */
	if (this->HasFlag(CH_PERSIST) || (this->ci && this->ci->HasFlag(CI_PERSIST)))
		return;

	/* Channel is syncing from a netburst, don't destroy it as more users are probably wanting to join immediatly
	 * We also don't part the bot here either, if necessary we will part it after the sync
	 */
	if (this->HasFlag(CH_SYNCING))
		return;

	/* Additionally, do not delete this channel if ChanServ/a BotServ bot is inhabiting it */
	if (this->ci && this->ci->HasFlag(CI_INHABIT))
		return;

	/* check for BSMinUsers and part the BotServ bot from the channel
	 * Use <= because the bot is included in this->users.size()
	 */
	if (!Config->s_BotServ.empty() && this->ci && this->ci->bi && this->users.size() <= Config->BSMinUsers && this->FindUser(this->ci->bi))
		this->ci->bi->Part(this->ci->c);
	else if (this->users.empty())
		delete this;
}

/** Check if the user is on the channel
 * @param u The user
 * @return A user container if found, else NULL
 */
UserContainer *Channel::FindUser(User *u)
{
	for (CUserList::iterator it = this->users.begin(), it_end = this->users.end(); it != it_end; ++it)
		if ((*it)->user == u)
			return *it;
	return NULL;
}

/** Check if a user has a status on a channel
 * @param u The user
 * @param cms The status mode, or NULL to represent no status
 * @return true or false
 */
bool Channel::HasUserStatus(User *u, ChannelModeStatus *cms) const
{
	if (!u || (cms && cms->Type != MODE_STATUS))
		throw CoreException("Channel::HasUserStatus got bad mode");

	/* Usually its more efficient to search the users channels than the channels users */
	ChannelContainer *cc = u->FindChannel(this);
	if (cc)
	{
		if (cms)
			return cc->Status->HasFlag(cms->Name);
		else
			return !cc->Status->FlagCount();
	}

	return false;
}

/** Check if a user has a status on a channel
 * Use the overloaded function for ChannelModeStatus* to check for no status
 * @param u The user
 * @param Name The Mode name, eg CMODE_OP, CMODE_VOICE
 * @return true or false
 */
bool Channel::HasUserStatus(User *u, ChannelModeName Name) const
{
	return HasUserStatus(u, debug_cast<ChannelModeStatus *>(ModeManager::FindChannelModeByName(Name)));
}

/**
 * See if a channel has a mode
 * @param Name The mode name
 * @return true or false
 */
bool Channel::HasMode(ChannelModeName Name) const
{
	return modes.HasFlag(Name);
}

/** Set a mode internally on a channel, this is not sent out to the IRCd
 * @param cm The mode
 * @param param The param
 * @param EnforeMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::SetModeInternal(ChannelMode *cm, const Anope::string &param, bool EnforceMLock)
{
	if (!cm)
		return;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnChannelModeSet, OnChannelModeSet(this, cm->Name, param));

	/* Setting v/h/o/a/q etc */
	if (cm->Type == MODE_STATUS)
	{
		if (param.empty())
		{
			Log() << "Channel::SetModeInternal() mode " << cm->ModeChar << " with no parameter for channel " << this->name;
			return;
		}

		BotInfo *bi = NULL;
		if (!Config->s_BotServ.empty())
			bi = findbot(param);
		User *u = bi ? bi : finduser(param);

		if (!u)
		{
			Log() << "MODE " << this->name << " +" << cm->ModeChar << " for nonexistant user " << param;
			return;
		}

		Log(LOG_DEBUG) << "Setting +" << cm->ModeChar << " on " << this->name << " for " << u->nick;

		/* Set the status on the user */
		ChannelContainer *cc = u->FindChannel(this);
		if (cc)
			cc->Status->SetFlag(cm->Name);

		/* Enforce secureops, etc */
		if (EnforceMLock)
			chan_set_correct_modes(u, this, 0);
		return;
	}
	/* Setting b/e/I etc */
	else if (cm->Type == MODE_LIST)
	{
		if (param.empty())
		{
			Log() << "Channel::SetModeInternal() mode " << cm->ModeChar << " with no parameter for channel " << this->name;
			return;
		}

		ChannelModeList *cml = debug_cast<ChannelModeList *>(cm);
		cml->AddMask(this, param);
		return;
	}

	modes.SetFlag(cm->Name);

	if (!param.empty())
	{
		if (cm->Type != MODE_PARAM)
		{
			Log() << "Channel::SetModeInternal() mode " << cm->ModeChar << " for " << this->name << " with a paramater, but its not a param mode";
			return;
		}

		/* They could be resetting the mode to change its params */
		std::map<ChannelModeName, Anope::string>::iterator it = Params.find(cm->Name);
		if (it != Params.end())
			Params.erase(it);

		Params.insert(std::make_pair(cm->Name, param));
	}

	/* Channel mode +P or so was set, mark this channel as persistant */
	if (cm->Name == CMODE_PERM)
	{
		this->SetFlag(CH_PERSIST);
		if (ci)
			ci->SetFlag(CI_PERSIST);
	}

	/* Check if we should enforce mlock */
	if (!EnforceMLock || MOD_RESULT == EVENT_STOP)
		return;

	/* Non registered channels can not be +r */
	if (!ci && HasMode(CMODE_REGISTERED))
		RemoveMode(NULL, CMODE_REGISTERED);

	/* Non registered channel has no mlock */
	if (!ci)
		return;

	/* If this channel has this mode locked negative */
	if (ci->HasMLock(cm->Name, false))
	{
		/* Remove the mode */
		if (cm->Type == MODE_PARAM)
		{
			Anope::string cparam;
			GetParam(cm->Name, cparam);
			RemoveMode(NULL, cm, cparam);
		}
		else if (cm->Type == MODE_REGULAR)
			RemoveMode(NULL, cm);
	}
	/* If this is a param mode and its mlocked +, check to ensure someone didn't reset it with the wrong param */
	else if (cm->Type == MODE_PARAM && ci->HasMLock(cm->Name, true))
	{
		ChannelModeParam *cmp = debug_cast<ChannelModeParam *>(cm);
		Anope::string cparam, ciparam;
		/* Get the param currently set on this channel */
		GetParam(cmp->Name, cparam);
		/* Get the param set in mlock */
		ci->GetParam(cmp->Name, ciparam);

		/* We have the wrong param set */
		if (cparam.empty() || ciparam.empty() || !cparam.equals_cs(ciparam))
			/* Reset the mode with the correct param */
			SetMode(NULL, cm, ciparam);
	}
}

/** Remove a mode internally on a channel, this is not sent out to the IRCd
 * @param cm The mode
 * @param param The param
 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::RemoveModeInternal(ChannelMode *cm, const Anope::string &param, bool EnforceMLock)
{
	if (!cm)
		return;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnChannelModeUnset, OnChannelModeUnset(this, cm->Name, param));

	/* Setting v/h/o/a/q etc */
	if (cm->Type == MODE_STATUS)
	{
		if (param.empty())
		{
			Log() << "Channel::RemoveModeInternal() mode " << cm->ModeChar << " with no parameter for channel " << this->name;
			return;
		}

		BotInfo *bi = NULL;
		if (!Config->s_BotServ.empty())
			bi = findbot(param);
		User *u = bi ? bi : finduser(param);

		if (!u)
		{
			Log() << "Channel::RemoveModeInternal() MODE " << this->name << "-" << cm->ModeChar << " for nonexistant user " << param;
			return;
		}

		Log(LOG_DEBUG) << "Setting -" << cm->ModeChar << " on " << this->name << " for " << u->nick;

		/* Remove the status on the user */
		ChannelContainer *cc = u->FindChannel(this);
		if (cc)
			cc->Status->UnsetFlag(cm->Name);

		/* Reset modes on bots if we're supposed to */
		if (bi)
		{
			if (std::find(Config->BotModeList.begin(), Config->BotModeList.end(), cm) != Config->BotModeList.end())
				this->SetMode(bi, cm, bi->nick);
		}

		return;
	}
	/* Setting b/e/I etc */
	else if (cm->Type == MODE_LIST)
	{
		if (param.empty())
		{
			Log() << "Channel::RemoveModeInternal() mode " << cm->ModeChar << " with no parameter for channel " << this->name;
			return;
		}

		ChannelModeList *cml = debug_cast<ChannelModeList *>(cm);
		cml->DelMask(this, param);
		return;
	}

	modes.UnsetFlag(cm->Name);

	if (cm->Type == MODE_PARAM)
	{
		std::map<ChannelModeName, Anope::string>::iterator it = Params.find(cm->Name);
		if (it != Params.end())
			Params.erase(it);
	}

	if (cm->Name == CMODE_PERM)
	{
		this->UnsetFlag(CH_PERSIST);

		if (ci)
		{
			ci->UnsetFlag(CI_PERSIST);
			if (!Config->s_BotServ.empty() && ci->bi && this->FindUser(ci->bi) && Config->BSMinUsers && this->users.size() <= Config->BSMinUsers)
				this->ci->bi->Part(this);
		}
	}

	/* We set -P in an empty channel, delete the channel */
	if (cm->Name == CMODE_PERM && users.empty())
	{
		delete this;
		return;
	}

	/* Check for mlock */

	/* Non registered channel, no mlock */
	if (!ci || !EnforceMLock || MOD_RESULT == EVENT_STOP)
		return;

	/* This channel has this the mode locked on */
	if (ci->HasMLock(cm->Name, true))
	{
		if (cm->Type == MODE_REGULAR)
		{
			/* Set the mode */
			SetMode(NULL, cm);
		}
		/* This is a param mode */
		else if (cm->Type == MODE_PARAM)
		{
			Anope::string cparam;
			/* Get the param stored in mlock for this mode */
			if (ci->GetParam(cm->Name, cparam))
				SetMode(NULL, cm, cparam);
		}
	}
}

/** Set a mode on a channel
 * @param bi The client setting the modes
 * @param cm The mode
 * @param param Optional param arg for the mode
 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::SetMode(BotInfo *bi, ChannelMode *cm, const Anope::string &param, bool EnforceMLock)
{
	if (!cm)
		return;
	/* Don't set modes already set */
	if (cm->Type == MODE_REGULAR && HasMode(cm->Name))
		return;
	else if (cm->Type == MODE_PARAM && HasMode(cm->Name))
	{
		Anope::string cparam;
		if (GetParam(cm->Name, cparam) && cparam.equals_cs(param))
			return;
	}
	else if (cm->Type == MODE_STATUS)
	{
		User *u = finduser(param);
		if (u && HasUserStatus(u, debug_cast<ChannelModeStatus *>(cm)))
			return;
	}
	else if (cm->Type == MODE_LIST)
	{
		// XXX this needs rewritten
	}

	ModeManager::StackerAdd(bi, this, cm, true, param);
	SetModeInternal(cm, param, EnforceMLock);
}

/**
 * Set a mode on a channel
 * @param bi The client setting the modes
 * @param Name The mode name
 * @param param Optional param arg for the mode
 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::SetMode(BotInfo *bi, ChannelModeName Name, const Anope::string &param, bool EnforceMLock)
{
	SetMode(bi, ModeManager::FindChannelModeByName(Name), param, EnforceMLock);
}

/** Remove a mode from a channel
 * @param bi The client setting the modes
 * @param cm The mode
 * @param param Optional param arg for the mode
 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::RemoveMode(BotInfo *bi, ChannelMode *cm, const Anope::string &param, bool EnforceMLock)
{
	if (!cm)
		return;
	/* Don't unset modes that arent set */
	if ((cm->Type == MODE_REGULAR || cm->Type == MODE_PARAM) && !HasMode(cm->Name))
		return;
	/* Don't unset status that aren't set */
	else if (cm->Type == MODE_STATUS)
	{
		User *u = finduser(param);
		if (u && !HasUserStatus(u, debug_cast<ChannelModeStatus *>(cm)))
			return;
	}
	else if (cm->Type == MODE_LIST)
	{
		// XXX this needs to be rewritten sometime
	}

	/* If this mode needs no param when being unset, empty the param */
	bool SendParam = true;
	if (cm->Type == MODE_PARAM)
	{
		ChannelModeParam *cmp = debug_cast<ChannelModeParam *>(cm);
		if (cmp->MinusNoArg)
			SendParam = false;
	}

	ModeManager::StackerAdd(bi, this, cm, false, param);
	RemoveModeInternal(cm, SendParam ? param : "", EnforceMLock);
}

/**
 * Remove a mode from a channel
 * @param bi The client setting the modes
 * @param Name The mode name
 * @param param Optional param arg for the mode
 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::RemoveMode(BotInfo *bi, ChannelModeName Name, const Anope::string &param, bool EnforceMLock)
{
	RemoveMode(bi, ModeManager::FindChannelModeByName(Name), param, EnforceMLock);
}

/** Get a param from the channel
 * @param Name The mode
 * @param Target a string to put the param into
 * @return true on success
 */
bool Channel::GetParam(ChannelModeName Name, Anope::string &Target) const
{
	std::map<ChannelModeName, Anope::string>::const_iterator it = Params.find(Name);

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
bool Channel::HasParam(ChannelModeName Name) const
{
	std::map<ChannelModeName, Anope::string>::const_iterator it = Params.find(Name);

	if (it != Params.end())
		return true;

	return false;
}

/*************************************************************************/

/** Clear all the modes from the channel
 * @param bi The client setting the modes
 * @param internal Only remove the modes internally
 */
void Channel::ClearModes(BotInfo *bi, bool internal)
{
	for (size_t n = CMODE_BEGIN + 1; n != CMODE_END; ++n)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(static_cast<ChannelModeName>(n));

		if (cm && this->HasMode(cm->Name))
		{
			if (cm->Type == MODE_REGULAR)
			{
				if (!internal)
					this->RemoveMode(NULL, cm);
				else
					this->RemoveModeInternal(cm);
			}
			else if (cm->Type == MODE_PARAM)
			{
				Anope::string param;
				this->GetParam(cm->Name, param);
				if (!internal)
					this->RemoveMode(NULL, cm, param);
				else
					this->RemoveModeInternal(cm, param);
			}
		}
	}

	modes.ClearFlags();
}

/** Clear all the bans from the channel
 * @param bi The client setting the modes
 * @param internal Only remove the modes internally
 */
void Channel::ClearBans(BotInfo *bi, bool internal)
{
	Entry *entry, *nexte;

	ChannelModeList *cml = debug_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_BAN));

	if (cml && this->bans && this->bans->count)
		for (entry = this->bans->entries; entry; entry = nexte)
		{
			nexte = entry->next;

			if (!internal)
				this->RemoveMode(bi, cml, entry->mask);
			else
				this->RemoveModeInternal(cml, entry->mask);
		}
}

/** Clear all the excepts from the channel
 * @param bi The client setting the modes
 * @param internal Only remove the modes internally
 */
void Channel::ClearExcepts(BotInfo *bi, bool internal)
{
	Entry *entry, *nexte;

	ChannelModeList *cml = debug_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_EXCEPT));

	if (cml && this->excepts && this->excepts->count)
		for (entry = this->excepts->entries; entry; entry = nexte)
		{
			nexte = entry->next;

			if (!internal)
				this->RemoveMode(bi, cml, entry->mask);
			else
				this->RemoveModeInternal(cml, entry->mask);
		}
}

/** Clear all the invites from the channel
 * @param bi The client setting the modes
 * @param internal Only remove the modes internally
 */
void Channel::ClearInvites(BotInfo *bi, bool internal)
{
	Entry *entry, *nexte;

	ChannelModeList *cml = debug_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE));

	if (cml && this->invites && this->invites->count)
		for (entry = this->invites->entries; entry; entry = nexte)
		{
			nexte = entry->next;

			if (!internal)
				this->RemoveMode(bi, cml, entry->mask);
			else
				this->RemoveModeInternal(cml, entry->mask);
		}
}

/** Set a string of modes on the channel
 * @param bi The client setting the modes
 * @param EnforceMLock Should mlock be enforced on this mode change
 * @param cmodes The modes to set
 */
void Channel::SetModes(BotInfo *bi, bool EnforceMLock, const char *cmodes, ...)
{
	char buf[BUFSIZE] = "";
	va_list args;
	Anope::string modebuf, sbuf;
	int add = -1;
	va_start(args, cmodes);
	vsnprintf(buf, BUFSIZE - 1, cmodes, args);
	va_end(args);

	spacesepstream sep(buf);
	sep.GetToken(modebuf);
	for (unsigned i = 0, end = modebuf.length(); i < end; ++i)
	{
		ChannelMode *cm;

		switch (modebuf[i])
		{
			case '+':
				add = 1;
				continue;
			case '-':
				add = 0;
				continue;
			default:
				if (add == -1)
					continue;
				cm = ModeManager::FindChannelModeByChar(modebuf[i]);
				if (!cm)
					continue;
		}

		if (add)
		{
			if (cm->Type != MODE_REGULAR && sep.GetToken(sbuf))
				this->SetMode(bi, cm, sbuf, EnforceMLock);
			else
				this->SetMode(bi, cm, "", EnforceMLock);
		}
		else if (!add)
		{
			if (cm->Type != MODE_REGULAR && sep.GetToken(sbuf))
				this->RemoveMode(bi, cm, sbuf, EnforceMLock);
			else
				this->RemoveMode(bi, cm, "", EnforceMLock);
		}
	}
}

/** Set a string of modes internally on a channel
 * @param setter The setter, if it is a user
 * @param mode the modes
 * @param EnforceMLock true to enforce mlock
 */
void Channel::SetModesInternal(User *setter, const Anope::string &mode, bool EnforceMLock)
{
	spacesepstream sep_modes(mode);
	Anope::string m;

	sep_modes.GetToken(m);

	Anope::string modestring;
	Anope::string paramstring;
	int add = -1;
	for (unsigned int i = 0, end = m.length(); i < end; ++i)
	{
		ChannelMode *cm;

		switch (m[i])
		{
			case '+':
				modestring += '+';
				add = 1;
				continue;
			case '-':
				modestring += '-';
				add = 0;
				continue;
			default:
				if (add == -1)
					continue;
				cm = ModeManager::FindChannelModeByChar(m[i]);
				if (!cm)
					continue;
				modestring += cm->ModeChar;
		}

		if (cm->Type == MODE_REGULAR)
		{
			if (add)
				this->SetModeInternal(cm, "", EnforceMLock);
			else
				this->RemoveModeInternal(cm, "", EnforceMLock);
			continue;
		}
		else if (cm->Type == MODE_PARAM)
		{
			ChannelModeParam *cmp = debug_cast<ChannelModeParam *>(cm);

			if (!add && cmp->MinusNoArg)
			{
				this->RemoveModeInternal(cm, "", EnforceMLock);
				continue;
			}
		}
		Anope::string token;
		if (sep_modes.GetToken(token))
		{
			User *u = NULL;
			if (cm->Type == MODE_STATUS && (u = finduser(token)))
				paramstring += " " + u->nick;
			else
				paramstring += " " + token;

			if (add)
				this->SetModeInternal(cm, token, EnforceMLock);
			else
				this->RemoveModeInternal(cm, token, EnforceMLock);
		}
		else
			Log() << "warning: Channel::SetModesInternal() recieved more modes requiring params than params, modes: " << mode;
	}

	if (setter)
		Log(setter, this, "mode") << modestring << paramstring;
	else
		Log(LOG_DEBUG) << "Setting " << this->name << " to " << modestring << paramstring;
}

/** Kick a user from a channel internally
 * @param source The sender of the kick
 * @param nick The nick being kicked
 * @param reason The reason for the kick
 */
void Channel::KickInternal(const Anope::string &source, const Anope::string &nick, const Anope::string &reason)
{
	User *sender = finduser(source);
	BotInfo *bi = NULL;
	if (!Config->s_BotServ.empty() && this->ci)
		bi = findbot(nick);
	User *target = bi ? bi : finduser(nick);
	if (!target)
	{
		Log() << "Channel::KickInternal got a nonexistent user " << nick << " on " << this->name << ": " << reason;
		return;
	}

	if (sender)
		Log(sender, this, "kick") << "kicked " << target->nick << " (" << reason << ")";
	else
		Log(target, this, "kick") << "was kicked by " << source << " (" << reason << ")";

	Anope::string chname = this->name;

	if (target->FindChannel(this))
	{
		FOREACH_MOD(I_OnUserKicked, OnUserKicked(this, target, source, reason));
		this->DeleteUser(target);
	}
	else
		Log() << "Channel::KickInternal got kick for user " << target->nick << " from " << (sender ? sender->nick : source) << " who isn't on channel " << this->name;

	/* Bots get rejoined */
	if (bi)
		bi->Join(chname);
}

/** Kick a user from the channel
 * @param bi The sender, can be NULL for the service bot for this channel
 * @param u The user being kicked
 * @param reason The reason for the kick
 * @return true if the kick was scucessful, false if a module blocked the kick
 */
bool Channel::Kick(BotInfo *bi, User *u, const char *reason, ...)
{
	va_list args;
	char buf[BUFSIZE] = "";
	va_start(args, reason);
	vsnprintf(buf, BUFSIZE - 1, reason, args);
	va_end(args);

	/* May not kick ulines */
	if (u->server->IsULined())
		return false;

	/* Do not kick protected clients */
	if (u->IsProtected())
		return false;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnBotKick, OnBotKick(bi, this, u, buf));
	if (MOD_RESULT == EVENT_STOP)
		return false;
	ircdproto->SendKick(bi ? bi : whosends(this->ci), this, u, "%s", buf);
	this->KickInternal(bi ? bi->nick : whosends(this->ci)->nick, u->nick, buf);
	return true;
}

Anope::string Channel::GetModes(bool complete, bool plus)
{
	Anope::string res;

	if (this->HasModes())
	{
		Anope::string params;
		for (std::map<Anope::string, Mode *>::const_iterator it = ModeManager::Modes.begin(), it_end = ModeManager::Modes.end(); it != it_end; ++it)
		{
			if (it->second->Class != MC_CHANNEL)
				continue;

			ChannelMode *cm = debug_cast<ChannelMode *>(it->second);

			if (this->HasMode(cm->Name))
			{
				res += cm->ModeChar;

				if (complete)
				{
					if (cm->Type == MODE_PARAM)
					{
						ChannelModeParam *cmp = debug_cast<ChannelModeParam *>(cm);

						if (plus || !cmp->MinusNoArg)
						{
							Anope::string param;
							this->GetParam(cmp->Name, param);

							if (!param.empty())
								params += " " + param;
						}
					}
				}
			}
		}

		res += params;
	}

	return res;
}

void Channel::ChangeTopicInternal(const Anope::string &user, const Anope::string &newtopic, time_t ts)
{
	User *u = finduser(user);
	this->topic = newtopic;
	this->topic_setter = u ? u->nick : user;
	this->topic_time = ts;

	Log(LOG_DEBUG) << "Topic of " << this->name << " changed by " << user << " to " << newtopic;

	FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(this, this->topic));

	if (this->ci)
	{
		this->ci->CheckTopic();
	}
}

void Channel::ChangeTopic(const Anope::string &user, const Anope::string &newtopic, time_t ts)
{
	User *u = finduser(user);
	this->topic = newtopic;
	this->topic_setter = u ? u->nick : user;
	this->topic_time = ts;

	ircdproto->SendTopic(whosends(this->ci), this);

	FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(this, this->topic));

	if (this->ci)
	{
		this->ci->CheckTopic();
	}
}

/*************************************************************************/

Channel *findchan(const Anope::string &chan)
{
	channel_map::const_iterator it = ChannelList.find(chan);

	if (it != ChannelList.end())
		return it->second;
	return NULL;
}

/*************************************************************************/

/* Return statistics.  Pointers are assumed to be valid. */

void get_channel_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;
	Anope::string buf;

	for (channel_map::const_iterator cit = ChannelList.begin(); cit != ChannelList.end(); ++cit)
	{
		Channel *chan = cit->second;

		++count;
		mem += sizeof(*chan);
		if (!chan->topic.empty())
			mem += chan->topic.length() + 1;
		if (chan->GetParam(CMODE_KEY, buf))
			mem += buf.length() + 1;
		if (chan->GetParam(CMODE_FLOOD, buf))
			mem += buf.length() + 1;
		if (chan->GetParam(CMODE_REDIRECT, buf))
			mem += buf.length() + 1;
		mem += get_memuse(chan->bans);
		if (ModeManager::FindChannelModeByName(CMODE_EXCEPT))
			mem += get_memuse(chan->excepts);
		if (ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE))
			mem += get_memuse(chan->invites);
		for (CUserList::iterator it = chan->users.begin(), it_end = chan->users.end(); it != it_end; ++it)
		{
			mem += sizeof(*it);
			mem += sizeof((*it)->ud);
			if (!(*it)->ud.lastline.empty())
				mem += (*it)->ud.lastline.length() + 1;
		}
		for (std::list<BanData *>::iterator it = chan->bd.begin(), it_end = chan->bd.end(); it != it_end; ++it)
		{
			if (!(*it)->mask.empty())
				mem += (*it)->mask.length() + 1;
			mem += sizeof(*it);
		}
	}
	*nrec = count;
	*memuse = mem;
}

/*************************************************************************/

/* Is the given nick on the given channel?
   This function supports links. */

User *nc_on_chan(Channel *c, const NickCore *nc)
{
	if (!c || !nc)
		return NULL;

	for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; ++it)
	{
		UserContainer *uc = *it;

		if (uc->user->Account() == nc)
			return uc->user;
	}
	return NULL;
}

/*************************************************************************/
/*************************** Message Handling ****************************/
/*************************************************************************/

/** Handle a JOIN command
 * @param source user joining
 * @param channels being joined
 * @param ts TS for the join
 */
void do_join(const Anope::string &source, const Anope::string &channels, const Anope::string &ts)
{
	User *user = finduser(source);
	if (!user)
	{
		Log() << "JOIN from nonexistent user " << source << ": " << channels;
		return;
	}

	commasepstream sep(channels);
	Anope::string buf;
	while (sep.GetToken(buf))
	{
		if (buf[0] == '0')
		{
			for (UChannelList::iterator it = user->chans.begin(), it_end = user->chans.end(); it != it_end; )
			{
				ChannelContainer *cc = *it++;

				Anope::string channame = cc->chan->name;
				FOREACH_MOD(I_OnPrePartChannel, OnPrePartChannel(user, cc->chan));
				cc->chan->DeleteUser(user);
				FOREACH_MOD(I_OnPartChannel, OnPartChannel(user, findchan(channame), channame, ""));
			}
			user->chans.clear();
			continue;
		}

		Channel *chan = findchan(buf);

		/* Channel doesn't exist, create it */
		if (!chan)
			chan = new Channel(buf, Anope::CurTime);

		/* Join came with a TS */
		if (!ts.empty())
		{
			time_t t = Anope::string(ts).is_pos_number_only() ? convertTo<time_t>(ts) : 0;

			/* Their time is older, we lose */
			if (t && chan->creation_time > t)
			{
				Log(LOG_DEBUG) << "Recieved an older TS " << chan->name << " in JOIN, changing from " << chan->creation_time << " to " << ts;
				chan->creation_time = t;

				chan->Reset();
			}
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnPreJoinChannel, OnPreJoinChannel(user, chan));

		/* Join the user to the channel */
		chan->JoinUser(user);
		/* Set the propre modes on the user */
		chan_set_correct_modes(user, chan, 1);

		/* Modules may want to allow this user in the channel, check.
		 * If not, CheckKick will kick/ban them, don't call OnJoinChannel after this as the user will have
		 * been destroyed
		 */
		if (MOD_RESULT != EVENT_STOP && chan && chan->ci && chan->ci->CheckKick(user))
			continue;

		FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(user, chan));
	}
}

/*************************************************************************/

/** Handle a KICK command.
 * @param source The source of the kick
 * @param users the user(s) being kicked
 * @param reason The reason for the kick
 */
void do_kick(const Anope::string &source, const Anope::string &channel, const Anope::string &users, const Anope::string &reason)
{
	Channel *c = findchan(channel);
	if (!c)
	{
		Log() << "Recieved kick for nonexistant channel " << channel;
		return;
	}

	Anope::string buf;
	commasepstream sep(users);
	while (sep.GetToken(buf))
		c->KickInternal(source, buf, reason);
}

/*************************************************************************/

void do_part(const Anope::string &source, const Anope::string &channels, const Anope::string &reason)
{
	User *user = finduser(source);
	if (!user)
	{
		Log() << "PART from nonexistent user " << source << ": " << reason;
		return;
	}

	commasepstream sep(channels);
	Anope::string buf;
	while (sep.GetToken(buf))
	{
		Channel *c = findchan(buf);

		if (!c)
			Log() << "Recieved PART from " << user->nick << " for nonexistant channel " << buf;
		else if (user->FindChannel(c))
		{
			Log(user, c, "part") << "Reason: " << (!reason.empty() ? reason : "No reason");
			FOREACH_MOD(I_OnPrePartChannel, OnPrePartChannel(user, c));
			Anope::string ChannelName = c->name;
			c->DeleteUser(user);
			FOREACH_MOD(I_OnPartChannel, OnPartChannel(user, findchan(ChannelName), ChannelName, !reason.empty() ? reason : ""));
		}
		else
			Log() << "Recieved PART from " << user->nick << " for " << c->name << ", but " << user->nick << " isn't in " << c->name << "?";
	}
}

/*************************************************************************/

/** Process a MODE command from the server, and set the modes on the user/channel
 * it was sent for
 * @param source The source of the command
 * @param channel the channel to change modes on
 * @param modes the mode changes
 * @param ts the timestamp for the modes
 */
void do_cmode(const Anope::string &source, const Anope::string &channel, const Anope::string &modes, const Anope::string &ts)
{
	Channel *c = findchan(channel);
	if (!c)
	{
		Log(LOG_DEBUG) << "MODE " << modes << " for nonexistant channel " << channel;
		return;
	}

	Log(LOG_DEBUG) << "MODE " << channel << " " << modes << " ts: " << ts;

	if (source.find('.') != Anope::string::npos)
	{
		if (Anope::CurTime != c->server_modetime)
		{
			c->server_modecount = 0;
			c->server_modetime = Anope::CurTime;
		}
		++c->server_modecount;
	}

	c->SetModesInternal(finduser(source), modes);
}

/*************************************************************************/

/**
 * Set the correct modes, or remove the ones granted without permission,
 * for the specified user on ths specified channel. This doesn't give
 * modes to ignored users, but does remove them if needed.
 * @param user The user to give/remove modes to/from
 * @param c The channel to give/remove modes on
 * @param give_modes Set to 1 to give modes, 0 to not give modes
 * @return void
 **/
void chan_set_correct_modes(User *user, Channel *c, int give_modes)
{
	ChannelInfo *ci;
	ChannelMode *owner, *admin, *op, *halfop, *voice;

	owner = ModeManager::FindChannelModeByName(CMODE_OWNER);
	admin = ModeManager::FindChannelModeByName(CMODE_PROTECT);
	op = ModeManager::FindChannelModeByName(CMODE_OP);
	halfop = ModeManager::FindChannelModeByName(CMODE_HALFOP);
	voice = ModeManager::FindChannelModeByName(CMODE_VOICE);

	if (!c || !(ci = c->ci))
		return;

	if (ci->HasFlag(CI_FORBIDDEN) || c->name[0] == '+')
		return;

	Log(LOG_DEBUG) << "Setting correct user modes for " << user->nick << " on " << c->name << " (" << (give_modes ? "" : "not ") << "giving modes)";

	if (give_modes && !get_ignore(user->nick) && (!user->Account() || user->Account()->HasFlag(NI_AUTOOP)))
	{
		if (owner && check_access(user, ci, CA_AUTOOWNER))
			c->SetMode(NULL, CMODE_OWNER, user->nick);
		else if (admin && check_access(user, ci, CA_AUTOPROTECT))
			c->SetMode(NULL, CMODE_PROTECT, user->nick);

		if (op && check_access(user, ci, CA_AUTOOP))
			c->SetMode(NULL, CMODE_OP, user->nick);
		else if (halfop && check_access(user, ci, CA_AUTOHALFOP))
			c->SetMode(NULL, CMODE_HALFOP, user->nick);
		else if (voice && check_access(user, ci, CA_AUTOVOICE))
			c->SetMode(NULL, CMODE_VOICE, user->nick);
	}
	/* If this channel has secureops or the user matches autodeop or the channel is syncing and they are not ulined, check to remove modes */
	if ((ci->HasFlag(CI_SECUREOPS) || check_access(user, ci, CA_AUTODEOP) || c->HasFlag(CH_SYNCING)) && !user->server->IsULined())
	{
		if (owner && c->HasUserStatus(user, CMODE_OWNER) && !check_access(user, ci, CA_FOUNDER))
			c->RemoveMode(NULL, CMODE_OWNER, user->nick);

		if (admin && c->HasUserStatus(user, CMODE_PROTECT) && !check_access(user, ci, CA_AUTOPROTECT) && !check_access(user, ci, CA_PROTECTME))
			c->RemoveMode(NULL, CMODE_PROTECT, user->nick);

		if (op && c->HasUserStatus(user, CMODE_OP) && !check_access(user, ci, CA_AUTOOP) && !check_access(user, ci, CA_OPDEOPME))
			c->RemoveMode(NULL, CMODE_OP, user->nick);

		if (halfop && c->HasUserStatus(user, CMODE_HALFOP) && !check_access(user, ci, CA_AUTOHALFOP) && !check_access(user, ci, CA_HALFOPME))
			c->RemoveMode(NULL, CMODE_HALFOP, user->nick);
	}
}

/*************************************************************************/

/** Set modes on every channel
 * This overrides mlock on channels
 * @param bi The bot to send the modes from
 * @param modes The modes
 */
void MassChannelModes(BotInfo *bi, const Anope::string &modes)
{
	for (channel_map::const_iterator it = ChannelList.begin(), it_end = ChannelList.end(); it != it_end; ++it)
	{
		Channel *c = it->second;

		if (c->bouncy_modes)
			return;
		c->SetModes(bi, false, "%s", modes.c_str());
	}
}

/*************************************************************************/

/**
 * This handles creating a new Entry.
 * This function destroys and free's the given mask as a side effect.
 * @param mask Host/IP/CIDR mask to convert to an entry
 * @return Entry struct for the given mask, NULL if creation failed
 */
Entry *entry_create(const Anope::string &mask)
{
	Entry *entry;
	Anope::string cidrhost;
	uint32 ip, cidr;

	entry = new Entry;
	entry->SetFlag(ENTRYTYPE_NONE);
	entry->prev = NULL;
	entry->next = NULL;
	entry->mask = mask;

	Anope::string newmask = mask, host, nick, user;

	size_t at = newmask.find('@');
	if (at != Anope::string::npos)
	{
		host = newmask.substr(at + 1);
		newmask = newmask.substr(0, at);
		/* If the user is purely a wildcard, ignore it */
		if (!str_is_pure_wildcard(newmask))
		{
			/* There might be a nick too  */
			//user = strchr(mask, '!');
			size_t ex = newmask.find('!');
			if (ex != Anope::string::npos)
			{
				user = newmask.substr(ex + 1);
				newmask = newmask.substr(0, ex);
				/* If the nick is purely a wildcard, ignore it */
				if (!str_is_pure_wildcard(newmask))
					nick = newmask;
			}
			else
				user = newmask;
		}
	}
	else
		/* It is possibly an extended ban/invite mask, but we do
		 * not support these at this point.. ~ Viper */
		/* If there's no user in the mask, assume a pure wildcard */
		host = newmask;

	if (!nick.empty())
	{
		entry->nick = nick;
		/* Check if we have a wildcard user */
		if (str_is_wildcard(nick))
			entry->SetFlag(ENTRYTYPE_NICK_WILD);
		else
			entry->SetFlag(ENTRYTYPE_NICK);
	}

	if (!user.empty())
	{
		entry->user = user;
		/* Check if we have a wildcard user */
		if (str_is_wildcard(user))
			entry->SetFlag(ENTRYTYPE_USER_WILD);
		else
			entry->SetFlag(ENTRYTYPE_USER);
	}

	/* Only check the host if it's not a pure wildcard */
	if (!host.empty() && !str_is_pure_wildcard(host))
	{
		if (ircd->cidrchanbei && str_is_cidr(host, ip, cidr, cidrhost))
		{
			entry->cidr_ip = ip;
			entry->cidr_mask = cidr;
			entry->SetFlag(ENTRYTYPE_CIDR4);
			host = cidrhost;
		}
		else if (ircd->cidrchanbei && host.find('/') != Anope::string::npos)
		{
			/* Most IRCd's don't enforce sane bans therefore it is not
			 * so unlikely we will encounter this.
			 * Currently we only support strict CIDR without taking into
			 * account quirks of every single ircd (nef) that ignore everything
			 * after the first /cidr. To add this, sanitaze before sending to
			 * str_is_cidr() as this expects a standard cidr.
			 * Add it to the internal list (so it is included in for example clear)
			 * but do not use if during matching.. ~ Viper */
			entry->ClearFlags();
			entry->SetFlag(ENTRYTYPE_NONE);
		}
		else
		{
			entry->host = host;
			if (str_is_wildcard(host))
				entry->SetFlag(ENTRYTYPE_HOST_WILD);
			else
				entry->SetFlag(ENTRYTYPE_HOST);
		}
	}

	return entry;
}

/**
 * Create an entry and add it at the beginning of given list.
 * @param list The List the mask should be added to
 * @param mask The mask to parse and add to the list
 * @return Pointer to newly added entry. NULL if it fails.
 */
Entry *entry_add(EList *list, const Anope::string &mask)
{
	Entry *e;

	e = entry_create(mask);

	if (!e)
		return NULL;

	e->next = list->entries;
	e->prev = NULL;

	if (list->entries)
		list->entries->prev = e;
	list->entries = e;
	++list->count;

	return e;
}

/**
 * Delete the given entry from a given list.
 * @param list Linked list from which entry needs to be removed.
 * @param e The entry to be deleted, must be member of list.
 */
void entry_delete(EList *list, Entry *e)
{
	if (!list || !e)
		return;

	if (e->next)
		e->next->prev = e->prev;
	if (e->prev)
		e->prev->next = e->next;

	if (list->entries == e)
		list->entries = e->next;

	delete e;

	--list->count;
}

/**
 * Create and initialize a new entrylist
 * @return Pointer to the created EList object
 **/
EList *list_create()
{
	EList *list;

	list = new EList;
	list->entries = NULL;
	list->count = 0;

	return list;
}

/**
 * Match the given Entry to the given user/host and optional IP addy
 * @param e Entry struct to match against
 * @param nick Nick to match against
 * @param user User to match against
 * @param host Host to match against
 * @param ip IP to match against, set to 0 to not match this
 * @return 1 for a match, 0 for no match
 */
int entry_match(Entry *e, const Anope::string &nick, const Anope::string &user, const Anope::string &host, uint32 ip)
{
	/* If we don't get an entry, or it s an invalid one, no match ~ Viper */
	if (!e || !e->FlagCount())
		return 0;

	if (ircd->cidrchanbei && e->HasFlag(ENTRYTYPE_CIDR4) && (!ip || (ip && (ip & e->cidr_mask) != e->cidr_ip)))
		return 0;
	if (e->HasFlag(ENTRYTYPE_NICK) && (nick.empty() || !e->nick.equals_ci(nick)))
		return 0;
	if (e->HasFlag(ENTRYTYPE_USER) && (user.empty() || !e->user.equals_ci(user)))
		return 0;
	if (e->HasFlag(ENTRYTYPE_HOST) && (host.empty() || !e->host.equals_ci(host)))
		return 0;
	if (e->HasFlag(ENTRYTYPE_NICK_WILD) && !Anope::Match(nick, e->nick))
		return 0;
	if (e->HasFlag(ENTRYTYPE_USER_WILD) && !Anope::Match(user, e->user))
		return 0;
	if (e->HasFlag(ENTRYTYPE_HOST_WILD) && !Anope::Match(host, e->host))
		return 0;

	return 1;
}

/**
 * Match the given Entry to the given hostmask and optional IP addy.
 * @param e Entry struct to match against
 * @param mask Hostmask to match against
 * @param ip IP to match against, set to 0 to not match this
 * @return 1 for a match, 0 for no match
 */
int entry_match_mask(Entry *e, const Anope::string &mask, uint32 ip)
{
	int res;

	Anope::string hostmask = mask, host, user, nick;

	size_t at = hostmask.find('@');
	if (at != Anope::string::npos)
	{
		host = hostmask.substr(at + 1);
		hostmask = hostmask.substr(0, at);
		size_t ex = hostmask.find('!');
		if (ex != Anope::string::npos)
		{
			user = hostmask.substr(ex + 1);
			nick = hostmask.substr(0, ex);
		}
		else
			user = hostmask;
	}
	else
		host = hostmask;

	res = entry_match(e, nick, user, host, ip);

	return res;
}

/**
 * Match a nick, user, host, and ip to a list entry
 * @param e List that should be matched against
 * @param nick The nick to match
 * @param user The user to match
 * @param host The host to match
 * @param ip The ip to match
 * @return Returns the first matching entry, if none, NULL is returned.
 */
Entry *elist_match(EList *list, const Anope::string &nick, const Anope::string &user, const Anope::string &host, uint32 ip)
{
	Entry *e;

	if (!list || !list->entries)
		return NULL;

	for (e = list->entries; e; e = e->next)
		if (entry_match(e, nick, user, host, ip))
			return e;

	/* We matched none */
	return NULL;
}

/**
 * Match a mask and ip to a list.
 * @param list EntryList that should be matched against
 * @param mask The nick!user@host mask to match
 * @param ip The ip to match
 * @return Returns the first matching entry, if none, NULL is returned.
 */
Entry *elist_match_mask(EList *list, const Anope::string &mask, uint32 ip)
{
	Entry *res;

	if (!list || !list->entries || mask.empty())
		return NULL;

	Anope::string hostmask = mask, host, user, nick;

	size_t at = hostmask.find('@');
	if (at != Anope::string::npos)
	{
		host = hostmask.substr(at + 1);
		hostmask = hostmask.substr(0, at);
		size_t ex = hostmask.find('!');
		if (ex != Anope::string::npos)
		{
			user = hostmask.substr(ex + 1);
			nick = hostmask.substr(0, ex);
		}
		else
			user = hostmask;
	}
	else
		host = hostmask;

	res = elist_match(list, nick, user, host, ip);

	return res;
}

/**
 * Check if a user matches an entry on a list.
 * @param list EntryList that should be matched against
 * @param user The user to match against the entries
 * @return Returns the first matching entry, if none, NULL is returned.
 */
Entry *elist_match_user(EList *list, User *u)
{
	Entry *res;
	//uint32 ip = 0;

	if (!list || !list->entries || !u)
		return NULL;

	/* Match what we ve got against the lists.. */
	res = elist_match(list, u->nick, u->GetIdent(), u->host, /*ip XXX*/0);
	if (!res)
		res = elist_match(list, u->nick, u->GetIdent(), u->GetDisplayedHost(), /*ip XXX*/0);
	if (!res && !u->GetCloakedHost().empty() && !u->GetCloakedHost().equals_cs(u->GetDisplayedHost()))
		res = elist_match(list, u->nick, u->GetIdent(), u->GetCloakedHost(), /*ip XXX*/ 0);

	return res;
}

/**
 * Find a entry identical to the given mask..
 * @param list EntryList that should be matched against
 * @param mask The *!*@* mask to match
 * @return Returns the first matching entry, if none, NULL is returned.
 */
Entry *elist_find_mask(EList *list, const Anope::string &mask)
{
	Entry *e;

	if (!list || !list->entries || mask.empty())
		return NULL;

	for (e = list->entries; e; e = e->next)
		if (e->mask.equals_ci(mask))
			return e;

	return NULL;
}

/**
 * Gets the total memory use of an entrylit.
 * @param list The list we should estimate the mem use of.
 * @return Returns the memory useage of the given list.
 */
long get_memuse(EList *list)
{
	Entry *e;
	long mem = 0;

	if (!list)
		return 0;

	mem += sizeof(EList *);
	mem += sizeof(Entry *) * list->count;
	if (list->entries)
	{
		for (e = list->entries; e; e = e->next)
		{
			if (!e->nick.empty())
				mem += e->nick.length() + 1;
			if (!e->user.empty())
				mem += e->user.length() + 1;
			if (!e->host.empty())
				mem += e->host.length() + 1;
			if (!e->mask.empty())
				mem += e->mask.length() + 1;
		}
	}

	return mem;
}

/*************************************************************************/
