/* Channel-handling routines.
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

channel_map ChannelList;

static const Anope::string ChannelFlagString[] = { "CH_PERSIST", "CH_SYNCING", "CH_LOGCHAN", "" };

/** Default constructor
 * @param name The channel name
 * @param ts The time the channel was created
 */
Channel::Channel(const Anope::string &nname, time_t ts) : Flags<ChannelFlag, 3>(ChannelFlagString)
{
	if (nname.empty())
		throw CoreException("A channel without a name ?");

	this->name = nname;

	ChannelList[this->name] = this;

	this->creation_time = ts;
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

	ChannelList.erase(this->name);
}

void Channel::Reset()
{
	this->modes.clear();

	for (CUserList::const_iterator it = this->users.begin(), it_end = this->users.end(); it != it_end; ++it)
	{
		UserContainer *uc = *it;

		Flags<ChannelModeName, CMODE_END * 2> flags = *debug_cast<Flags<ChannelModeName, CMODE_END * 2> *>(uc->Status);
		uc->Status->ClearFlags();

		if (findbot(uc->user->nick))
		{
			for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
			{
				ChannelMode *cm = ModeManager::ChannelModes[i];

				if (flags.HasFlag(cm->Name))
					this->SetMode(NULL, cm, uc->user->nick, false);
			}
		}
	}

	check_modes(this);
	for (CUserList::const_iterator it = this->users.begin(), it_end = this->users.end(); it != it_end; ++it)
		chan_set_correct_modes((*it)->user, this, 1);
	
	if (this->ci)
		this->ci->RestoreTopic();
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

	if (this->ci && this->ci->HasFlag(CI_PERSIST) && this->creation_time > this->ci->time_registered)
	{
		Log(LOG_DEBUG) << "Changing TS of " << this->name << " from " << this->creation_time << " to " << this->ci->time_registered;
		this->creation_time = this->ci->time_registered;
		ircdproto->SendChannel(this);
		this->Reset();
	}

	if (this->ci && check_access(user, this->ci, CA_MEMO) && this->ci->memos.memos.size() > 0)
	{
		if (this->ci->memos.memos.size() == 1)
			user->SendMessage(MemoServ, _("There is \002%d\002 memo on channel %s."), this->ci->memos.memos.size(), this->ci->name.c_str());
		else
			user->SendMessage(MemoServ, _("There are \002%d\002 memos on channel %s."), this->ci->memos.memos.size(), this->ci->name.c_str());
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
			this->ci->bi->Join(this, &Config->BotModeList);
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
 * @param param The optional mode param
 * @return The number of modes set
 */
size_t Channel::HasMode(ChannelModeName Name, const Anope::string &param)
{
	if (param.empty())
		return modes.count(Name);
	std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> its = this->GetModeList(Name);
	for (; its.first != its.second; ++its.first)
		if (its.first->second == param)
			return 1;
	return 0;
}

/** Get a list of modes on a channel
 * @param Name A mode name to get the list of
 * @return a pair of iterators for the beginning and end of the list
 */
std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> Channel::GetModeList(ChannelModeName Name)
{
	Channel::ModeList::iterator it = this->modes.find(Name), it_end = it;
	if (it != this->modes.end())
		it_end = this->modes.upper_bound(Name);
	return std::make_pair(it, it_end);
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

	if (cm->Type != MODE_LIST)
		this->modes.erase(cm->Name);
	this->modes.insert(std::make_pair(cm->Name, param));

	if (param.empty() && cm->Type != MODE_REGULAR)
	{
		Log() << "Channel::SetModeInternal() mode " << cm->ModeChar << " for " << this->name << " with a paramater, but its not a param mode";
		return;
	}

	if (cm->Type == MODE_LIST)
	{
		ChannelModeList *cml = debug_cast<ChannelModeList *>(cm);
		cml->OnAdd(this, param);
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

	ModeLock *ml = ci->GetMLock(cm->Name, param);
	if (ml)
	{
		if (ml->set && cm->Type == MODE_PARAM)
		{
			Anope::string cparam;
			this->GetParam(cm->Name, cparam);

			/* We have the wrong param set */
			if (cparam.empty() || ml->param.empty() || !cparam.equals_cs(ml->param))
				/* Reset the mode with the correct param */
				this->SetMode(NULL, cm, ml->param);
		}
		else if (!ml->set)
		{
			if (cm->Type == MODE_REGULAR)
				this->RemoveMode(NULL, cm);
			else if (cm->Type == MODE_PARAM)
			{
				Anope::string cparam;
				this->GetParam(cm->Name, cparam);
				this->RemoveMode(NULL, cm, cparam);
			}
			else if (cm->Type == MODE_LIST)
			{
				this->RemoveMode(NULL, cm, param);
			}
		}
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

		if (EnforceMLock)
		{
			/* Reset modes on bots if we're supposed to */
			if (bi)
			{
				if (Config->BotModeList.HasFlag(cm->Name))
					this->SetMode(bi, cm, bi->nick);
			}

			chan_set_correct_modes(u, this, 0);
		}
		return;
	}

	if (cm->Type == MODE_LIST && !param.empty())
	{
		std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> its = this->GetModeList(cm->Name);
		for (; its.first != its.second; ++its.first)
			if (Anope::Match(param, its.first->second))
			{
				this->modes.erase(its.first);
				break;
			}
	}
	else
		this->modes.erase(cm->Name);
	
	if (cm->Type == MODE_LIST)
	{
		ChannelModeList *cml = debug_cast<ChannelModeList *>(cm);
		cml->OnDel(this, param);
	}

	if (cm->Name == CMODE_PERM)
	{
		this->UnsetFlag(CH_PERSIST);

		if (ci)
		{
			ci->UnsetFlag(CI_PERSIST);
			if (!Config->s_BotServ.empty() && ci->bi && this->FindUser(ci->bi) && Config->BSMinUsers && this->users.size() <= Config->BSMinUsers)
			{
				bool empty = this->users.size() == 1;
				this->ci->bi->Part(this);
				if (empty)
					return;
			}
		}

		if (this->users.empty())
		{
			delete this;
			return;
		}
	}

	/* Check for mlock */

	/* Non registered channel, no mlock */
	if (!ci || !EnforceMLock || MOD_RESULT == EVENT_STOP)
		return;

	ModeLock *ml = ci->GetMLock(cm->Name, param);
	/* This channel has this the mode locked on */
	if (ml && ml->set)
	{
		if (cm->Type == MODE_REGULAR)
			this->SetMode(NULL, cm);
		else if (cm->Type == MODE_PARAM || cm->Type == MODE_LIST)
			this->SetMode(NULL, cm, ml->param);
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
	else if (cm->Type == MODE_PARAM)
	{
		ChannelModeParam *cmp = debug_cast<ChannelModeParam *>(cm);
		if (!cmp->IsValid(param))
			return;

		Anope::string cparam;
		if (GetParam(cm->Name, cparam) && cparam.equals_cs(param))
			return;
	}
	else if (cm->Type == MODE_STATUS)
	{
		User *u = finduser(param);
		if (!u || HasUserStatus(u, debug_cast<ChannelModeStatus *>(cm)))
			return;
	}
	else if (cm->Type == MODE_LIST)
	{
		ChannelModeList *cml = debug_cast<ChannelModeList *>(cm);
		if (this->HasMode(cm->Name, param) || !cml->IsValid(param))
			return;
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
		if (!u || !HasUserStatus(u, debug_cast<ChannelModeStatus *>(cm)))
			return;
	}
	else if (cm->Type == MODE_LIST)
	{
		if (!this->HasMode(cm->Name, param))
			return;
	}

	/* Get the param to send, if we need it */
	Anope::string realparam = param;
	if (cm->Type == MODE_PARAM)
	{
		realparam.clear();
		ChannelModeParam *cmp = debug_cast<ChannelModeParam *>(cm);
		if (!cmp->MinusNoArg)
			this->GetParam(cmp->Name, realparam);
	}

	ModeManager::StackerAdd(bi, this, cm, false, realparam);
	RemoveModeInternal(cm, realparam, EnforceMLock);
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
	std::multimap<ChannelModeName, Anope::string>::const_iterator it = this->modes.find(Name);

	Target.clear();

	if (it != this->modes.end())
	{
		Target = it->second;
		return true;
	}

	return false;
}

/*************************************************************************/

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
	/* Removing channel modes *may* delete this channel */
	dynamic_reference<Channel> this_reference(this);

	spacesepstream sep_modes(mode);
	Anope::string m;

	sep_modes.GetToken(m);

	Anope::string modestring;
	Anope::string paramstring;
	int add = -1;
	for (unsigned int i = 0, end = m.length(); i < end && this_reference; ++i)
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
				{
					Log(LOG_DEBUG) << "Channel::SetModeInternal: Unknown mode char " << m[i];
					continue;
				}
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

	if (!this_reference)
		return;

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
		bi->Join(this, &Config->BotModeList);
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
	Anope::string res, params;

	for (std::multimap<ChannelModeName, Anope::string>::const_iterator it = this->modes.begin(), it_end = this->modes.end(); it != it_end; ++it)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(it->first);
		if (!cm || cm->Type == MODE_LIST)
			continue;

		res += cm->ModeChar;

		if (complete && !it->second.empty())
		{
			ChannelModeParam *cmp = debug_cast<ChannelModeParam *>(cm);

			if (plus || !cmp->MinusNoArg)
				params += " " + it->second;
		}
	}

	return res + params;
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

	if (give_modes && (!user->Account() || user->Account()->HasFlag(NI_AUTOOP)))
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
	/* If this channel has secureops or the channel is syncing and they are not ulined, check to remove modes */
	if ((ci->HasFlag(CI_SECUREOPS) || c->HasFlag(CH_SYNCING)) && !user->server->IsULined())
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

	// Check mlock
	for (std::multimap<ChannelModeName, ModeLock>::const_iterator it = ci->GetMLock().begin(), it_end = ci->GetMLock().end(); it != it_end; ++it)
	{
		const ModeLock &ml = it->second;
		ChannelMode *cm = ModeManager::FindChannelModeByName(ml.name);
		if (!cm || cm->Type != MODE_STATUS)
			continue;

		if (Anope::Match(user->nick, ml.param) || Anope::Match(user->GetDisplayedMask(), ml.param))
		{
			if ((ml.set && !c->HasUserStatus(user, ml.name)) || (!ml.set && c->HasUserStatus(user, ml.name)))
			{
				if (ml.set)
					c->SetMode(NULL, cm, user->nick, false);
				else if (!ml.set)
					c->RemoveMode(NULL, cm, user->nick, false);
			}
		}
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

static const Anope::string EntryFlagString[] = { "ENTRYTYPE_NONE", "ENTRYTYPE_CIDR", "ENTRYTYPE_NICK_WILD", "ENTRYTYPE_NICK", "ENTRYTYPE_USER_WILD", "ENTRYTYPE_USER", "ENTRYTYPE_HOST_WILD", "ENTRYTYPE_HOST", "" };

/** Constructor
 * @param mode What mode this host is for - can be CMODE_BEGIN for unknown/no mode
 * @param _host A full nick!ident@host/cidr mask
 */
Entry::Entry(ChannelModeName mode, const Anope::string &_host) : Flags<EntryType>(EntryFlagString), modename(mode)
{
	this->SetFlag(ENTRYTYPE_NONE);
	this->cidr_len = 0;
	this->mask = _host;

	Anope::string _nick, _user, _realhost;
	size_t at = _host.find('@');
	if (at != Anope::string::npos)
	{
		_realhost = _host.substr(at + 1);
		Anope::string _nickident = _host.substr(0, at);

		size_t ex = _nickident.find('!');
		if (ex != Anope::string::npos)
		{
			_user = _nickident.substr(ex + 1);
			_nick = _nickident.substr(0, ex);
		}
		else
			_user = _nickident;
	}
	else
		_realhost = _host;
	
	if (!_nick.empty() && !str_is_pure_wildcard(_nick))
	{
		this->nick = _nick;
		if (str_is_wildcard(_nick))
			this->SetFlag(ENTRYTYPE_NICK_WILD);
		else
			this->SetFlag(ENTRYTYPE_NICK);
	}

	if (!_user.empty() && !str_is_pure_wildcard(_user))
	{
		this->user = _user;
		if (str_is_wildcard(_user))
			this->SetFlag(ENTRYTYPE_USER_WILD);
		else
			this->SetFlag(ENTRYTYPE_USER);
	}

	if (!_realhost.empty() && !str_is_pure_wildcard(_realhost))
	{
		size_t sl = _realhost.find_last_of('/');
		if (sl != Anope::string::npos)
		{
			try
			{
				sockaddrs addr;
				bool ipv6 = _realhost.substr(0, sl).find(':') != Anope::string::npos;
				addr.pton(ipv6 ? AF_INET6 : AF_INET, _realhost.substr(0, sl));
				/* If we got here, _realhost is a valid IP */

				Anope::string cidr_range = _realhost.substr(sl + 1);
				if (cidr_range.is_pos_number_only())
				{
					_realhost = _realhost.substr(0, sl);
					this->cidr_len = convertTo<unsigned int>(cidr_range);
					this->SetFlag(ENTRYTYPE_CIDR);
					Log(LOG_DEBUG) << "Ban " << _realhost << " has cidr " << static_cast<unsigned int>(this->cidr_len);
				}
			}
			catch (const SocketException &) { }
		}

		this->host = _realhost;

		if (!this->HasFlag(ENTRYTYPE_CIDR))
		{
			if (str_is_wildcard(_realhost))
				this->SetFlag(ENTRYTYPE_HOST_WILD);
			else
				this->SetFlag(ENTRYTYPE_HOST);
		}
	}
}

/** Get the banned mask for this entry
 * @return The mask
 */
const Anope::string Entry::GetMask()
{
	return this->mask;
}

/** Check if this entry matches a user
 * @param u The user
 * @param full True to match against a users real host and IP
 * @return true on match
 */
bool Entry::Matches(User *u, bool full) const
{
	bool ret = true;
	
	if (this->HasFlag(ENTRYTYPE_CIDR))
	{
		try
		{
			cidr cidr_mask(this->host, this->cidr_len);
			if (!u->ip() || !cidr_mask.match(u->ip))
				ret = false;
			/* If we're not matching fully and their displayed host isnt their IP */
			else if (!full && u->ip.addr() != u->GetDisplayedHost())
				ret = false;
		}
		catch (const SocketException &)
		{
			ret = false;
		}
	}
	if (this->HasFlag(ENTRYTYPE_NICK) && !this->nick.equals_ci(u->nick))
		ret = false;
	if (this->HasFlag(ENTRYTYPE_USER) && !this->user.equals_ci(u->GetVIdent()) && (!full ||
		!this->user.equals_ci(u->GetIdent())))
		ret = false;
	if (this->HasFlag(ENTRYTYPE_HOST) && !this->host.equals_ci(u->GetDisplayedHost()) && (!full ||
		(!this->host.equals_ci(u->host) && !this->host.equals_ci(u->chost) && !this->host.equals_ci(u->vhost) &&
		(!u->ip() || !this->host.equals_ci(u->ip.addr())))))
		ret = false;
	if (this->HasFlag(ENTRYTYPE_NICK_WILD) && !Anope::Match(u->nick, this->nick))
		ret = false;
	if (this->HasFlag(ENTRYTYPE_USER_WILD) && !Anope::Match(u->GetVIdent(), this->user) && (!full ||
		!Anope::Match(u->GetIdent(), this->user)))
		ret = false;
	if (this->HasFlag(ENTRYTYPE_HOST_WILD) && !Anope::Match(u->GetDisplayedHost(), this->host) && (!full ||
		(!Anope::Match(u->host, this->host) && !Anope::Match(u->chost, this->host) &&
		!Anope::Match(u->vhost, this->host) && (!u->ip() || !Anope::Match(u->ip.addr(), this->host)))))
		ret = false;
	
	ChannelMode *cm = ModeManager::FindChannelModeByName(this->modename);
	if (cm != NULL && cm->Type == MODE_LIST)
	{
		ChannelModeList *cml = debug_cast<ChannelModeList *>(cm);
		if (cml->Matches(u, this))
			ret = true;
	}

	return ret;
}

