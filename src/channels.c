/* Channel-handling routines.
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
#include "language.h"
#include "modules.h"

Channel *chanlist[1024];

#define HASH(chan)	((chan)[1] ? ((chan)[1]&31)<<5 | ((chan)[2]&31) : 0)

/*************************************************************************/

/** Default constructor
 * @param name The channel name
 * @param ts The time the channel was created
 */
Channel::Channel(const std::string &name, time_t ts)
{
	Channel **list;

	if (name.empty())
		throw CoreException("A channel without a name ?");

	this->name = name;
	list = &chanlist[HASH(this->name)];
	this->prev = NULL;
	this->next = *list;
	if (*list)
		(*list)->prev = this;
	*list = this;

	this->creation_time = ts;
	this->topic = NULL;
	this->bans = this->excepts = this->invites = NULL;
	this->bd = NULL;
	this->server_modetime = this->chanserv_modetime = 0;
	this->server_modecount = this->chanserv_modecount = this->bouncy_modes = this->topic_sync = 0;

	this->ci = cs_findchan(this->name);
	if (this->ci)
		this->ci->c = this;

	FOREACH_MOD(I_OnChannelCreate, OnChannelCreate(this));
}

/** Default destructor
 */
Channel::~Channel()
{
	BanData *bd, *next;

	FOREACH_MOD(I_OnChannelDelete, OnChannelDelete(this));

	Alog(LOG_DEBUG) << "Deleting channel " << this->name;

	for (bd = this->bd; bd; bd = next)
	{
		if (bd->mask)
			delete [] bd->mask;
		next = bd->next;
		delete bd;
	}

	if (this->ci)
		this->ci->c = NULL;

	if (this->topic)
		delete [] this->topic;

	if (this->bans && this->bans->count)
	{
		while (this->bans->entries)
			entry_delete(this->bans, this->bans->entries);
	}

	if (ModeManager::FindChannelModeByName(CMODE_EXCEPT))
	{
		if (this->excepts && this->excepts->count)
		{
			while (this->excepts->entries)
				entry_delete(this->excepts, this->excepts->entries);
		}
	}

	if (ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE))
	{
		if (this->invites && this->invites->count)
		{
			while (this->invites->entries)
				entry_delete(this->invites, this->invites->entries);
		}
	}

	if (this->next)
		this->next->prev = this->prev;
	if (this->prev)
		this->prev->next = this->next;
	else
		chanlist[HASH(this->name)] = this->next;
}

void Channel::Sync()
{
	if (this->ci)
	{
		check_modes(this);
		stick_all(this->ci);
	}

	if (serv_uplink && is_sync(serv_uplink) && !this->topic_sync)
		restore_topic(name.c_str());
}

void Channel::JoinUser(User *user)
{
	Alog(LOG_DEBUG) << user->nick << " joins " << this->name;

	Flags<ChannelModeName> *Status = new Flags<ChannelModeName>;
	ChannelContainer *cc = new ChannelContainer(this);
	cc->Status = Status;
	user->chans.push_back(cc);

	UserContainer *uc = new UserContainer(user);
	uc->Status = Status;
	this->users.push_back(uc);

	if (get_ignore(user->nick.c_str()) == NULL)
	{
		if (this->ci && (check_access(user, this->ci, CA_MEMO)) && (this->ci->memos.memos.size() > 0))
		{
			if (this->ci->memos.memos.size() == 1)
				notice_lang(Config.s_MemoServ, user, MEMO_X_ONE_NOTICE, this->ci->memos.memos.size(), this->ci->name.c_str());
			else
				notice_lang(Config.s_MemoServ, user, MEMO_X_MANY_NOTICE, this->ci->memos.memos.size(), this->ci->name.c_str());
		}
		/* Added channelname to entrymsg - 30.03.2004, Certus */
		/* Also, don't send the entrymsg when bursting -GD */
		if (this->ci && this->ci->entry_message && is_sync(user->server))
			user->SendMessage(whosends(this->ci)->nick, "[%s] %s", this->name.c_str(), this->ci->entry_message);
	}

	/**
	 * We let the bot join even if it was an ignored user, as if we don't,
	 * and the ignored user doesnt just leave, the bot will never
	 * make it into the channel, leaving the channel botless even for
	 * legit users - Rob
	 * But don't join the bot if the channel is persistant - Adam
	 **/
	if (Config.s_BotServ && this->ci && this->ci->bi && !this->ci->HasFlag(CI_PERSIST))
	{
		if (this->users.size() == Config.BSMinUsers)
			bot_join(this->ci);
	}
	if (Config.s_BotServ && this->ci && this->ci->bi)
	{
		if (this->users.size() >= Config.BSMinUsers && (this->ci->botflags.HasFlag(BS_GREET))
		&& user->nc && user->nc->greet && check_access(user, this->ci, CA_GREET))
		{
			/* Only display the greet if the main uplink we're connected
			 * to has synced, or we'll get greet-floods when the net
			 * recovers from a netsplit. -GD
			 */
			if (is_sync(user->server))
			{
				ircdproto->SendPrivmsg(this->ci->bi, this->name.c_str(), "[%s] %s", user->nc->display, user->nc->greet);
				this->ci->bi->lastmsg = time(NULL);
			}
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

	CUserList::iterator cit;
	for (cit = this->users.begin(); (*cit)->user != user && cit != this->users.end(); ++cit);
	if (cit == this->users.end())
	{
		Alog(LOG_DEBUG) << "Channel::DeleteUser() tried to delete nonexistant user " << user->nick << " from channel " << this->name;
		return;
	}

	delete (*cit)->Status;
	delete *cit;
	this->users.erase(cit);

	UChannelList::iterator uit;
	for (uit = user->chans.begin(); (*uit)->chan != this && uit != user->chans.end(); ++uit);
	if (uit == user->chans.end())
	{
		Alog(LOG_DEBUG) << "Channel::DeleteUser() tried to delete nonexistant channel " << this->name << " from " << user->nick << "'s channel list";
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

	if (Config.s_BotServ && this->ci && this->ci->bi && this->users.size() <= Config.BSMinUsers - 1)
		ircdproto->SendPart(this->ci->bi, this, NULL);
	
	if (this->users.empty())
		delete this;
}

/** Check if the user is on the channel
 * @param u The user
 * @return A user container if found, else NULL
 */
UserContainer *Channel::FindUser(User *u)
{
	for (CUserList::iterator it = this->users.begin(); it != this->users.end(); ++it)
		if ((*it)->user == u)
			return *it;
	return NULL;
}

/** Check if a user has a status on a channel
 * @param u The user
 * @param cms The status mode, or NULL to represent no status
 * @return true or false
 */
bool Channel::HasUserStatus(User *u, ChannelModeStatus *cms)
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
bool Channel::HasUserStatus(User *u, ChannelModeName Name)
{
	return HasUserStatus(u, dynamic_cast<ChannelModeStatus *>(ModeManager::FindChannelModeByName(Name)));
}

/**
 * See if a channel has a mode
 * @param Name The mode name
 * @return true or false
 */
bool Channel::HasMode(ChannelModeName Name)
{
	return modes[Name];
}

/** Set a mode internally on a channel, this is not sent out to the IRCd
 * @param cm The mode
 * @param param The param
 * @param EnforeMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::SetModeInternal(ChannelMode *cm, const std::string &param, bool EnforceMLock)
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
			Alog() << "Channel::SetModeInternal() mode " << cm->ModeChar << " with no parameter for channel " << this->name;
			return;
		}

		/* We don't track bots */
		if (findbot(param))
			return;

		User *u = finduser(param);
		if (!u)
		{
			Alog(LOG_DEBUG) << "MODE " << this->name << " +" << cm->ModeChar << " for nonexistant user " << param;
			return;
		}

		Alog(LOG_DEBUG) << "Setting +" << cm->ModeChar << " on " << this->name << " for " << u->nick;

		/* Set the status on the user */
		ChannelContainer *cc = u->FindChannel(this);
		if (cc)
		{
			cc->Status->SetFlag(cm->Name);
		}

		/* Enforce secureops, etc */
		chan_set_correct_modes(u, this, 0);
		return;
	}
	/* Setting b/e/I etc */
	else if (cm->Type == MODE_LIST)
	{
		if (param.empty())
		{
			Alog() << "Channel::SetModeInternal() mode " << cm->ModeChar << " with no parameter for channel " << this->name;
			return;
		}

		ChannelModeList *cml = dynamic_cast<ChannelModeList *>(cm);
		cml->AddMask(this, param.c_str());
		return;
	}

	modes[cm->Name] = true;

	if (!param.empty())
	{
		if (cm->Type != MODE_PARAM)
		{
			Alog() << "Channel::SetModeInternal() mode " << cm->ModeChar << " for " << this->name << " with a paramater, but its not a param mode";
			return;
		}

		/* They could be resetting the mode to change its params */
		std::map<ChannelModeName, std::string>::iterator it = Params.find(cm->Name);
		if (it != Params.end())
		{
			Params.erase(it);
		}

		Params.insert(std::make_pair(cm->Name, param));
	}

	/* Channel mode +P or so was set, mark this channel as persistant */
	if (cm->Name == CMODE_PERM)
	{
		this->SetFlag(CH_PERSIST);
		if (ci)
			ci->SetFlag(CI_PERSIST);
	}

	/* Check for mlock */

	/* Non registered channel, no mlock */
	if (!ci || !EnforceMLock || MOD_RESULT == EVENT_STOP)
		return;

	/* If this channel has this mode locked negative */
	if (ci->HasMLock(cm->Name, false))
	{
		/* Remove the mode */
		if (cm->Type == MODE_PARAM)
		{
			ChannelModeParam *cmp = dynamic_cast<ChannelModeParam *>(cmp);

			if (!cmp->MinusNoArg)
			{
				/* Get the current param set on the channel and send it with the mode */
				std::string cparam;
				if (GetParam(cmp->Name, &cparam))
					RemoveMode(NULL, cm, cparam);
			}
			else
				RemoveMode(NULL, cm);
		}
		else if (cm->Type == MODE_REGULAR)
			RemoveMode(NULL, cm);
	}
	/* If this is a param mode and its mlocked +, check to ensure someone didn't reset it with the wrong param */
	else if (cm->Type == MODE_PARAM && ci->HasMLock(cm->Name, true))
	{
		ChannelModeParam *cmp = dynamic_cast<ChannelModeParam *>(cm);
		std::string cparam, ciparam;
		/* Get the param currently set on this channel */
		GetParam(cmp->Name, &cparam);
		/* Get the param set in mlock */
		ci->GetParam(cmp->Name, &ciparam);

		/* We have the wrong param set */
		if (cparam.empty() || ciparam.empty() || cparam != ciparam)
		{
			/* Reset the mode with the correct param */
			SetMode(NULL, cm, ciparam);
		}
	}
}

/** Remove a mode internally on a channel, this is not sent out to the IRCd
 * @param cm The mode
 * @param param The param
 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::RemoveModeInternal(ChannelMode *cm, const std::string &param, bool EnforceMLock)
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
			Alog() << "Channel::RemoveModeInternal() mode " << cm->ModeChar << " with no parameter for channel " << this->name;
			return;
		}

		/* We don't track bots */
		if (findbot(param))
			return;

		User *u = finduser(param);
		if (!u)
		{
			Alog() << "Channel::RemoveModeInternal() MODE " << this->name << "-" << cm->ModeChar << " for nonexistant user " << param;
			return;
		}

		Alog(LOG_DEBUG) << "Setting -" << cm->ModeChar << " on " << this->name << " for " << u->nick;

		/* Remove the status on the user */
		ChannelContainer *cc = u->FindChannel(this);
		if (cc)
		{
			cc->Status->UnsetFlag(cm->Name);
		}

		return;
	}
	/* Setting b/e/I etc */
	else if (cm->Type == MODE_LIST)
	{
		if (param.empty())
		{
			Alog() << "Channel::RemoveModeInternal() mode " << cm->ModeChar << " with no parameter for channel " << this->name;
			return;
		}

		ChannelModeList *cml = dynamic_cast<ChannelModeList *>(cm);
		cml->DelMask(this, param.c_str());
		return;
	}

	modes[cm->Name] = false;

	if (cm->Type == MODE_PARAM)
	{
		std::map<ChannelModeName, std::string>::iterator it = Params.find(cm->Name);
		if (it != Params.end())
		{
			Params.erase(it);
		}
	}

	if (cm->Name == CMODE_PERM)
	{
		this->UnsetFlag(CH_PERSIST);

		if (ci)
		{
			ci->UnsetFlag(CI_PERSIST);
			if (Config.s_BotServ && ci->bi && users.size() == Config.BSMinUsers - 1)
				ircdproto->SendPart(ci->bi, this, NULL);
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
			std::string cparam;
			/* Get the param stored in mlock for this mode */
			if (ci->GetParam(cm->Name, &cparam))
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
void Channel::SetMode(BotInfo *bi, ChannelMode *cm, const std::string &param, bool EnforceMLock)
{
	if (!cm)
		return;
	/* Don't set modes already set */
	if ((cm->Type == MODE_REGULAR || cm->Type == MODE_PARAM) && HasMode(cm->Name))
		return;

	else if (cm->Type == MODE_STATUS)
	{
		User *u = finduser(param);
		if (u && HasUserStatus(u, dynamic_cast<ChannelModeStatus *>(cm)))
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
void Channel::SetMode(BotInfo *bi, ChannelModeName Name, const std::string &param, bool EnforceMLock)
{
	SetMode(bi, ModeManager::FindChannelModeByName(Name), param, EnforceMLock);
}

/**
 * Set a mode on a channel
 * @param bi The client setting the modes
 * @param Mode The mode
 * @param param Optional param arg for the mode
 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::SetMode(BotInfo *bi, char Mode, const std::string &param, bool EnforceMLock)
{
	SetMode(bi, ModeManager::FindChannelModeByChar(Mode), param, EnforceMLock);
}

/** Remove a mode from a channel
 * @param bi The client setting the modes
 * @param cm The mode
 * @param param Optional param arg for the mode
 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::RemoveMode(BotInfo *bi, ChannelMode *cm, const std::string &param, bool EnforceMLock)
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
		if (u && !HasUserStatus(u, dynamic_cast<ChannelModeStatus *>(cm)))
			return;
	}

	ModeManager::StackerAdd(bi, this, cm, false, param);
	RemoveModeInternal(cm, param, EnforceMLock);
}

/**
 * Remove a mode from a channel
 * @param bi The client setting the modes
 * @param Name The mode name
 * @param param Optional param arg for the mode
 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::RemoveMode(BotInfo *bi, ChannelModeName Name, const std::string &param, bool EnforceMLock)
{
	RemoveMode(bi, ModeManager::FindChannelModeByName(Name), param, EnforceMLock);
}

/**
 * Remove a mode from a channel
 * @param bi The client setting the modes
 * @param Mode The mode
 * @param param Optional param arg for the mode
 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::RemoveMode(BotInfo *bi, char Mode, const std::string &param, bool EnforceMLock)
{
	RemoveMode(bi, ModeManager::FindChannelModeByChar(Mode), param, EnforceMLock);
}

/** Get a param from the channel
 * @param Name The mode
 * @param Target a string to put the param into
 * @return true on success
 */
const bool Channel::GetParam(ChannelModeName Name, std::string *Target)
{
	std::map<ChannelModeName, std::string>::iterator it = Params.find(Name);

	(*Target).clear();

	if (it != Params.end())
	{
		*Target = it->second;
		return true;
	}

	return false;
}

/** Check if a mode is set and has a param
 * @param Name The mode
 */
const bool Channel::HasParam(ChannelModeName Name)
{
	std::map<ChannelModeName, std::string>::iterator it = Params.find(Name);

	if (it != Params.end())
	{
		return true;
	}

	return false;
}

/*************************************************************************/

/** Clear all the modes from the channel
 * @param bi The client setting the modes
 */
void Channel::ClearModes(BotInfo *bi)
{
	ChannelMode *cm;
	ChannelModeParam *cmp;

	for (size_t n = CMODE_BEGIN + 1; n != CMODE_END; ++n)
	{
		cm = ModeManager::FindChannelModeByName(static_cast<ChannelModeName>(n));

		if (cm && this->HasMode(cm->Name))
		{
			if (cm->Type == MODE_REGULAR)
			{
				this->RemoveMode(NULL, cm);
			}
			else if (cm->Type == MODE_PARAM)
			{
				cmp = dynamic_cast<ChannelModeParam *>(cm);

				if (!cmp->MinusNoArg)
				{
					std::string buf;
					if (this->GetParam(cmp->Name, &buf))
						this->RemoveMode(NULL, cm, buf);
				}
				else
				{
					this->RemoveMode(NULL, cm);
				}
			}
		}
	}

	modes.reset();
}

/** Clear all the bans from the channel
 * @param bi The client setting the modes
 */
void Channel::ClearBans(BotInfo *bi)
{
	Entry *entry, *nexte;
	ChannelModeList *cml;

	cml = dynamic_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_BAN));

	if (cml && this->bans && this->bans->count)
	{
		for (entry = this->bans->entries; entry; entry = nexte)
		{
			nexte = entry->next;

			this->RemoveMode(bi, CMODE_BAN, entry->mask);
		}
	}
}

/** Clear all the excepts from the channel
 * @param bi The client setting the modes
 */
void Channel::ClearExcepts(BotInfo *bi)
{
	Entry *entry, *nexte;
	ChannelModeList *cml;

	cml = dynamic_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_EXCEPT));

	if (cml && this->excepts && this->excepts->count)
	{
		for (entry = this->excepts->entries; entry; entry = nexte)
		{
			nexte = entry->next;

			this->RemoveMode(bi, CMODE_EXCEPT, entry->mask);
		}
	}
}

/** Clear all the invites from the channel
 * @param bi The client setting the modes
 */
void Channel::ClearInvites(BotInfo *bi)
{
	Entry *entry, *nexte;
	ChannelModeList *cml;

	cml = dynamic_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE));

	if (cml && this->invites && this->invites->count)
	{
		for (entry = this->invites->entries; entry; entry = nexte)
		{
			nexte = entry->next;

			this->RemoveMode(bi, CMODE_INVITEOVERRIDE, entry->mask);
		}
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
	std::string modebuf, sbuf;
	int add = -1;
	va_start(args, cmodes);
	vsnprintf(buf, BUFSIZE - 1, cmodes, args);
	va_end(args);

	spacesepstream sep(buf);
	sep.GetToken(modebuf);
	for (unsigned i = 0; i < modebuf.size(); ++i)
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
			if (cm->Type == MODE_PARAM && sep.GetToken(sbuf))
				this->SetMode(bi, cm, sbuf, EnforceMLock);
			else
				this->SetMode(bi, cm, "", EnforceMLock);
		}
		else if (add == 0)
		{
			if (cm->Type == MODE_PARAM && sep.GetToken(sbuf))
				this->RemoveMode(bi, cm, sbuf, EnforceMLock);
			else
				this->RemoveMode(bi, cm, "", EnforceMLock);
		}
	}
}

/*************************************************************************/

/** Set modes internally on the channel
 * @param c The channel
 * @param ac Number of args
 * @param av args
 */
void ChanSetInternalModes(Channel *c, int ac, const char **av)
{
	if (!ac)
		return;

	int k = 0, j = 0, add = -1;
	for (unsigned int i = 0; i < strlen(av[0]); ++i)
	{
		ChannelMode *cm;

		switch (av[0][i])
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
				cm = ModeManager::FindChannelModeByChar(av[0][i]);
				if (!cm)
					continue;
		}

		if (cm->Type == MODE_REGULAR)
		{
			if (add)
				c->SetModeInternal(cm);
			else
				c->RemoveModeInternal(cm);
			continue;
		}
		else if (cm->Type == MODE_PARAM)
		{
			ChannelModeParam *cmp = dynamic_cast<ChannelModeParam *>(cm);

			if (!add && cmp->MinusNoArg)
			{
				c->RemoveModeInternal(cm);
				++k;
				continue;
			}
		}
		if (++j < ac)
		{
			if (add)
				c->SetModeInternal(cm, av[j]);
			else
				c->RemoveModeInternal(cm, av[j]);
		}
		else
		{
			Alog() << "warning: ChanSetInternalModes() recieved more modes requiring params than params, modes: " << merge_args(ac, av) << ", ac: " << ac << ", j: " << j;
		}
	}

	if (j + k + 1 < ac)
	{
		Alog() << "warning: ChanSetInternalModes() recieved more params than modes requiring them, modes: " << merge_args(ac, av) << ", ac: " << ac << ", j: " << j << " k: " << k;
	}
}

/** Kick a user from a channel internally
 * @param source The sender of the kick
 * @param nick The nick being kicked
 * @param reason The reason for the kick
 */
void Channel::KickInternal(const std::string &source, const std::string &nick, const std::string &reason)
{
	/* If it is the bot that is being kicked, we make it rejoin the
	 * channel and stop immediately.
	 *        --lara
	 */
	if (Config.s_BotServ && this->ci && findbot(nick))
	{
		bot_join(this->ci);
		return;
	}

	User *user = finduser(nick);
	if (!user)
	{
		Alog(LOG_DEBUG) << "Channel::KickInternal got a nonexistent user " << nick << " on " << this->name << ": " << reason;
		return;
	}

	Alog(LOG_DEBUG) << "Channel::KickInternal kicking " << user->nick << " from " << this->name;
	
	if (user->FindChannel(this))
	{
		FOREACH_MOD(I_OnUserKicked, OnUserKicked(this, user, source, reason));
		this->DeleteUser(user);
	}
	else
		Alog(LOG_DEBUG) << "Channel::KickInternal got kick for user " << user->nick << " who isn't on channel " << this->name << " ?";
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
	if (is_ulined(u->server->name))
		return false;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnBotKick, OnBotKick(bi, this, u, buf));
	if (MOD_RESULT == EVENT_STOP)
		return false;
	ircdproto->SendKick(bi ? bi : whosends(this->ci), this, u, "%s", buf);
	this->KickInternal(bi ? bi->nick : whosends(this->ci)->nick, u->nick, buf);
	return true;
}

/* Returns a fully featured binary modes string. If complete is 0, the
 * eventual parameters won't be added to the string.
 */

char *chan_get_modes(Channel * chan, int complete, int plus)
{
	static char res[BUFSIZE];
	char params[BUFSIZE];
	char *end = res, *value, *pend = params, *pend2 = params;
	std::string param;
	ChannelMode *cm;
	ChannelModeParam *cmp;
	std::map<char, ChannelMode *>::iterator it;

	if (chan->HasModes())
	{
		for (it = ModeManager::ChannelModesByChar.begin(); it != ModeManager::ChannelModesByChar.end(); ++it)
		{
			cm = it->second;

			if (chan->HasMode(cm->Name))
			{
				*end++ = it->first;

				if (complete)
				{
					if (cm->Type == MODE_PARAM)
					{
						cmp = dynamic_cast<ChannelModeParam *>(cm);

						if (plus || !cmp->MinusNoArg)
						{
							chan->GetParam(cmp->Name, &param);

							if (!param.empty())
							{
								value = const_cast<char *>(param.c_str());

								*pend++ = ' ';

								while (*value)
									*pend++ = *value++;
							}
						}
					}
				}
			}
		}

		while (*pend2)
			*end++ = *pend2++;
	}

	*end = 0;

	return res;
}

/*************************************************************************/

/* Return the Channel structure corresponding to the named channel, or NULL
 * if the channel was not found.  chan is assumed to be non-NULL and valid
 * (i.e. pointing to a channel name of 2 or more characters). */

Channel *findchan(const char *chan)
{
	Channel *c;

	if (!chan || !*chan)
	{
		Alog(LOG_DEBUG) << "findchan() called with NULL values";
		return NULL;
	}

	c = chanlist[HASH(chan)];
	while (c)
	{
		if (stricmp(c->name.c_str(), chan) == 0)
		{
			Alog(LOG_DEBUG_3) << "findchan(" << chan << ") -> " << static_cast<void *>(c);
			return c;
		}
		c = c->next;
	}
	return NULL;
}

/*************************************************************************/

/* Iterate over all channels in the channel list.  Return NULL at end of
 * list.
 */

static Channel *current;
static int next_index;

Channel *firstchan()
{
	next_index = 0;
	while (next_index < 1024 && current == NULL)
		current = chanlist[next_index++];
	Alog(LOG_DEBUG_3) << "firstchan() returning " << (current ? current->name : "NULL (end of list)");
	return current;
}

Channel *nextchan()
{
	if (current)
		current = current->next;
	if (!current && next_index < 1024) {
		while (next_index < 1024 && current == NULL)
			current = chanlist[next_index++];
	}
	Alog(LOG_DEBUG_3) << "nextchan() returning " << (current ? current->name : "NULL (end of list)");
	return current;
}

/*************************************************************************/

/* Return statistics.  Pointers are assumed to be valid. */

void get_channel_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;
	Channel *chan;
	BanData *bd;
	int i;
	std::string buf;

	for (i = 0; i < 1024; i++) {
		for (chan = chanlist[i]; chan; chan = chan->next) {
			count++;
			mem += sizeof(*chan);
			if (chan->topic)
				mem += strlen(chan->topic) + 1;
			if (chan->GetParam(CMODE_KEY, &buf))
				mem += buf.length() + 1;
			if (chan->GetParam(CMODE_FLOOD, &buf))
				mem += buf.length() + 1;
			if (chan->GetParam(CMODE_REDIRECT, &buf))
				mem += buf.length() + 1;
			mem += get_memuse(chan->bans);
			if (ModeManager::FindChannelModeByName(CMODE_EXCEPT))
				mem += get_memuse(chan->excepts);
			if (ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE))
				mem += get_memuse(chan->invites);
			for (CUserList::iterator it = chan->users.begin(); it != chan->users.end(); ++it)
			{
				mem += sizeof(*it);
				mem += sizeof((*it)->ud);
				if ((*it)->ud->lastline)
					mem += strlen((*it)->ud->lastline) + 1;
			}
			for (bd = chan->bd; bd; bd = bd->next) {
				if (bd->mask)
					mem += strlen(bd->mask) + 1;
				mem += sizeof(*bd);
			}
		}
	}
	*nrec = count;
	*memuse = mem;
}

/*************************************************************************/

/* Is the given nick on the given channel?
   This function supports links. */

User *nc_on_chan(Channel * c, NickCore * nc)
{
	if (!c || !nc)
		return NULL;

	for (CUserList::iterator it = c->users.begin(); it != c->users.end(); ++it)
	{
		UserContainer *uc = *it;

		if (uc->user->nc == nc)
			return uc->user;
	}
	return NULL;
}

/*************************************************************************/
/*************************** Message Handling ****************************/
/*************************************************************************/

/* Handle a JOIN command.
 *	av[0] = channels to join
 */

void do_join(const char *source, int ac, const char **av)
{
	User *user;
	Channel *chan;
	time_t ctime = time(NULL);

	user = finduser(source);
	if (!user)
	{
		Alog(LOG_DEBUG) << "JOIN from nonexistent user " << source << ": " << merge_args(ac, av);
		return;
	}

	commasepstream sep(av[0]);
	ci::string buf;
	while (sep.GetToken(buf))
	{
		if (buf[0] == '0')
		{
			for (UChannelList::iterator it = user->chans.begin(); it != user->chans.end();)
			{
				ChannelContainer *cc = *it++;

				std::string channame = cc->chan->name;
				FOREACH_MOD(I_OnPrePartChannel, OnPrePartChannel(user, cc->chan));
				cc->chan->DeleteUser(user);
				FOREACH_MOD(I_OnPartChannel, OnPartChannel(user, findchan(channame.c_str()), channame, ""));
			}
			user->chans.clear();
			continue;
		}

		chan = findchan(buf.c_str());

		/* Channel doesn't exist, create it */
		if (!chan)
		{
			chan = new Channel(av[0], ctime);
		}

		/* Join came with a TS */
		if (ac == 2)
		{
			time_t ts = atol(av[1]);

			/* Their time is older, we lose */
			if (chan->creation_time > ts)
			{
				Alog(LOG_DEBUG) << "recieved a new TS for JOIN: " << ts;

				if (chan->ci)
				{
					/* Cycle the bot to fix ts */
					if (chan->ci->bi)
					{
						ircdproto->SendPart(chan->ci->bi, chan, "TS reop");
						bot_join(chan->ci);
					}
					/* Be sure to set mlock again, else we could be -r etc.. */
					check_modes(chan);
				}
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
 * @param ac number of args
 * @param av The channel, nick(s) being kicked, and reason
 */
void do_kick(const std::string &source, int ac, const char **av)
{
	Channel *c = findchan(av[0]);
	if (!c)
	{
		Alog(LOG_DEBUG) << "Recieved kick for nonexistant channel " << av[0];
		return;
	}

	std::string buf;
	commasepstream sep(av[1]);
	while (sep.GetToken(buf))
	{
		c->KickInternal(source, buf, av[2]);
	}
}

/*************************************************************************/

/* Handle a PART command.
 *	av[0] = channels to leave
 *	av[1] = reason (optional)
 */

void do_part(const char *source, int ac, const char **av)
{
	User *user = finduser(source);
	if (!user) 
	{
		Alog(LOG_DEBUG) << "PART from nonexistent user " << source << ": " << merge_args(ac, av);
		return;
	}

	commasepstream sep(av[0]);
	ci::string buf;
	while (sep.GetToken(buf))
	{
		Channel *c = findchan(buf.c_str());
		
		if (!c)
		{
			Alog(LOG_DEBUG) << "Recieved PART from " << user->nick << " for nonexistant channel " << buf;
		}

		Alog(LOG_DEBUG) << source << " leaves " << buf;

		if (user->FindChannel(c))
		{
			FOREACH_MOD(I_OnPrePartChannel, OnPrePartChannel(user, c));
			std::string ChannelName = c->name;
			c->DeleteUser(user);
			FOREACH_MOD(I_OnPartChannel, OnPartChannel(user, findchan(ChannelName.c_str()), ChannelName, av[1] ? av[1] : ""));
		}
		else
			Alog(LOG_DEBUG) << "Recieved PART from " << user->nick << " for " << c->name << ", but " << user->nick << " isn't in " << c->name << "?";
	}
}

/*************************************************************************/

/** Process a MODE command from the server, and set the modes on the user/channel
 * it was sent for
 * @param source The source of the command
 * @param ac Number of args in array..
 * @param av Array of args
 */
void do_cmode(const char *source, int ac, const char **av)
{
	Channel *c;
	ChannelInfo *ci;
	unsigned int i;
	const char *t;

	if (ircdcap->tsmode)
	{
		if (uplink_capab & ircdcap->tsmode || UseTSMODE)
		{
			for (i = 0; i < strlen(av[1]); i++)
				if (!isdigit(av[1][i]))
					break;
			if (av[1][i] == '\0')
			{
				t = av[0];
				av[0] = av[1];
				av[1] = t;
				ac--;
				av++;
			}
			else
				Alog() << "TSMODE enabled but MODE has no valid TS";
		}
	}

	/* :42XAAAAAO TMODE 1106409026 #ircops +b *!*@*.aol.com */
	if (ircd->ts6)
	{
		if (isdigit(av[0][0]))
		{
			ac--;
			av++;
		}
	}

	c = findchan(av[0]);
	if (!c)
	{
		if (debug)
		{
			ci = cs_findchan(av[0]);
			if (!ci || ci->HasFlag(CI_FORBIDDEN))
				Alog(LOG_DEBUG) << "MODE " << merge_args(ac - 1, av + 1) << " for nonexistant channel " << av[0];
		}
		return;
	}

	if (strchr(source, '.') && !av[1][strcspn(av[1], "bovahq")])
	{
		if (time(NULL) != c->server_modetime)
		{
			c->server_modecount = 0;
			c->server_modetime = time(NULL);
		}
		c->server_modecount++;
	}

	ac--;
	av++;
	ChanSetInternalModes(c, ac, av);
}

/*************************************************************************/

/* Handle a TOPIC command. */

void do_topic(const char *source, int ac, const char **av)
{
	Channel *c = findchan(av[0]);
	ChannelInfo *ci;
	int ts;
	time_t topic_time;
	char *topicsetter;

	if (ircd->sjb64) {
		ts = base64dects(av[2]);
		Alog(LOG_DEBUG) << "encoded TOPIC TS " << av[2] << " converted to " << ts;
	} else {
		ts = strtoul(av[2], NULL, 10);
	}

	topic_time = ts;

	if (!c) 
	{
		Alog(LOG_DEBUG) << "TOPIC " << merge_args(ac - 1, av + 1) << " for nonexistent channel " << av[0];
		return;
	}

	/* We can be sure that the topic will be in sync here -GD */
	c->topic_sync = 1;

	ci = c->ci;

	/* For Unreal, cut off the ! and any futher part of the topic setter.
	 * This way, nick!ident@host setters will only show the nick. -GD
	 */
	topicsetter = myStrGetToken(av[1], '!', 0);

	/* If the current topic we have matches the last known topic for this
	 * channel exactly, there's no need to update anything and we can as
	 * well just return silently without updating anything. -GD
	 */
	if ((ac > 3) && *av[3] && ci && ci->last_topic
		&& (strcmp(av[3], ci->last_topic) == 0)
		&& (strcmp(topicsetter, ci->last_topic_setter.c_str()) == 0)) {
		delete [] topicsetter;
		return;
	}

	if (check_topiclock(c, topic_time)) {
		delete [] topicsetter;
		return;
	}

	if (c->topic) {
		delete [] c->topic;
		c->topic = NULL;
	}
	if (ac > 3 && *av[3]) {
		c->topic = sstrdup(av[3]);
	}

	c->topic_setter = topicsetter;
	c->topic_time = topic_time;
	delete [] topicsetter;

	record_topic(av[0]);

	FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(c, av[0]));
}

/*************************************************************************/
/**************************** Internal Calls *****************************/
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
void chan_set_correct_modes(User * user, Channel * c, int give_modes)
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

	if ((ci->HasFlag(CI_FORBIDDEN)) || (*(c->name.c_str()) == '+'))
		return;

	Alog(LOG_DEBUG) << "Setting correct user modes for " << user->nick << " on " << c->name << " (" << (give_modes ? "" : "not ") << "giving modes)";

	if (give_modes && !get_ignore(user->nick.c_str()) && (!user->nc || !user->nc->HasFlag(NI_AUTOOP)))
	{
		if (owner && check_access(user, ci, CA_AUTOOWNER))
			c->SetMode(NULL, CMODE_OWNER, user->nick);

		if (admin && check_access(user, ci, CA_AUTOPROTECT))
			c->SetMode(NULL, CMODE_PROTECT, user->nick);

		if (op && check_access(user, ci, CA_AUTOOP))
			c->SetMode(NULL, CMODE_OP, user->nick);

		if (halfop && check_access(user, ci, CA_AUTOHALFOP))
			c->SetMode(NULL, CMODE_HALFOP, user->nick);

		if (voice && check_access(user, ci, CA_AUTOVOICE))
			c->SetMode(NULL, CMODE_VOICE, user->nick);
	}
	if ((ci->HasFlag(CI_SECUREOPS) || check_access(user, ci, CA_AUTODEOP)) && !is_ulined(user->server->name))
	{
		if (owner && !IsFounder(user, ci))
			c->RemoveMode(NULL, CMODE_OWNER, user->nick);

		if (admin && !check_access(user, ci, CA_AUTOPROTECT) && !check_access(user, ci, CA_PROTECTME))
			c->RemoveMode(NULL, CMODE_PROTECT, user->nick);

		if (op && !check_access(user, ci, CA_AUTOOP) && !check_access(user, ci, CA_OPDEOPME))
			c->RemoveMode(NULL, CMODE_OP, user->nick);

		if (halfop && !check_access(user, ci, CA_AUTOHALFOP) && !check_access(user, ci, CA_HALFOPME))
			c->RemoveMode(NULL, CMODE_HALFOP, user->nick);
	}
}

/*************************************************************************/

/** Set modes on every channel
 * This overrides mlock on channels
 * @param bi The bot to send the modes from
 * @param modes The modes
 */
void MassChannelModes(BotInfo *bi, const std::string &modes)
{
	Channel *c;

	for (c = firstchan(); c; c = nextchan())
	{
		if (c->bouncy_modes)
			return;
		c->SetModes(bi, false, modes.c_str());
	}
}

/*************************************************************************/

void restore_unsynced_topics()
{
	Channel *c;

	for (c = firstchan(); c; c = nextchan()) {
		if (!(c->topic_sync))
			restore_topic(c->name.c_str());
	}
}

/*************************************************************************/

/**
 * This handles creating a new Entry.
 * This function destroys and free's the given mask as a side effect.
 * @param mask Host/IP/CIDR mask to convert to an entry
 * @return Entry struct for the given mask, NULL if creation failed
 */
Entry *entry_create(char *mask)
{
	Entry *entry;
	char *nick = NULL, *user, *host, *cidrhost;
	uint32 ip, cidr;

	entry = new Entry;
	entry->SetFlag(ENTRYTYPE_NONE);
	entry->prev = NULL;
	entry->next = NULL;
	entry->nick = NULL;
	entry->user = NULL;
	entry->host = NULL;
	entry->mask = sstrdup(mask);

	host = strchr(mask, '@');
	if (host) {
		*host++ = '\0';
		/* If the user is purely a wildcard, ignore it */
		if (str_is_pure_wildcard(mask))
			user = NULL;
		else {

			/* There might be a nick too  */
			user = strchr(mask, '!');
			if (user) {
				*user++ = '\0';
				/* If the nick is purely a wildcard, ignore it */
				if (str_is_pure_wildcard(mask))
					nick = NULL;
				else
					nick = mask;
			} else {
				nick = NULL;
				user = mask;
			}
		}
	} else {
		/* It is possibly an extended ban/invite mask, but we do
		 * not support these at this point.. ~ Viper */
		/* If there's no user in the mask, assume a pure wildcard */
		user = NULL;
		host = mask;
	}

	if (nick) {
		entry->nick = sstrdup(nick);
		/* Check if we have a wildcard user */
		if (str_is_wildcard(nick))
			entry->SetFlag(ENTRYTYPE_NICK_WILD);
		else
			entry->SetFlag(ENTRYTYPE_NICK);
	}

	if (user) {
		entry->user = sstrdup(user);
		/* Check if we have a wildcard user */
		if (str_is_wildcard(user))
			entry->SetFlag(ENTRYTYPE_USER_WILD);
		else
			entry->SetFlag(ENTRYTYPE_USER);
	}

	/* Only check the host if it's not a pure wildcard */
	if (*host && !str_is_pure_wildcard(host)) {
		if (ircd->cidrchanbei && str_is_cidr(host, &ip, &cidr, &cidrhost)) {
			entry->cidr_ip = ip;
			entry->cidr_mask = cidr;
			entry->SetFlag(ENTRYTYPE_CIDR4);
			host = cidrhost;
		} else if (ircd->cidrchanbei && strchr(host, '/')) {
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
		} else {
			entry->host = sstrdup(host);
			if (str_is_wildcard(host))
				entry->SetFlag(ENTRYTYPE_HOST_WILD);
			else
				entry->SetFlag(ENTRYTYPE_HOST);
		}
	}
	delete [] mask;

	return entry;
}


/**
 * Create an entry and add it at the beginning of given list.
 * @param list The List the mask should be added to
 * @param mask The mask to parse and add to the list
 * @return Pointer to newly added entry. NULL if it fails.
 */
Entry *entry_add(EList * list, const char *mask)
{
	Entry *e;
	char *hostmask;

	hostmask = sstrdup(mask);
	e = entry_create(hostmask);

	if (!e)
		return NULL;

	e->next = list->entries;
	e->prev = NULL;

	if (list->entries)
		list->entries->prev = e;
	list->entries = e;
	list->count++;

	return e;
}


/**
 * Delete the given entry from a given list.
 * @param list Linked list from which entry needs to be removed.
 * @param e The entry to be deleted, must be member of list.
 */
void entry_delete(EList * list, Entry * e)
{
	if (!list || !e)
		return;

	if (e->next)
		e->next->prev = e->prev;
	if (e->prev)
		e->prev->next = e->next;

	if (list->entries == e)
		list->entries = e->next;

	if (e->nick)
		delete [] e->nick;
	if (e->user)
		delete [] e->user;
	if (e->host)
		delete [] e->host;
	delete [] e->mask;
	delete e;

	list->count--;
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
int entry_match(Entry *e, const ci::string &nick, const ci::string &user, const ci::string &host, uint32 ip)
{
	/* If we don't get an entry, or it s an invalid one, no match ~ Viper */
	if (!e || !e->FlagCount())
		return 0;

	ci::string ci_nick(nick.c_str()), ci_user(user.c_str()), ci_host(host.c_str());
	if (ircd->cidrchanbei && e->HasFlag(ENTRYTYPE_CIDR4) && (!ip || (ip && (ip & e->cidr_mask) != e->cidr_ip)))
		return 0;
	if (e->HasFlag(ENTRYTYPE_NICK) && (nick.empty() || nick != e->nick))
		return 0;
	if (e->HasFlag(ENTRYTYPE_USER) && (user.empty() || user != e->user))
		return 0;
	if (e->HasFlag(ENTRYTYPE_HOST) && (host.empty() || host != e->host))
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
int entry_match_mask(Entry * e, const char *mask, uint32 ip)
{
	char *hostmask, *nick, *user, *host;
	int res;

	hostmask = sstrdup(mask);

	host = strchr(hostmask, '@');
	if (host) {
		*host++ = '\0';
		user = strchr(hostmask, '!');
		if (user) {
			*user++ = '\0';
			nick = hostmask;
		} else {
			nick = NULL;
			user = hostmask;
		}
	} else {
		nick = NULL;
		user = NULL;
		host = hostmask;
	}

	res = entry_match(e, nick ? nick : "", user ? user : "", host ? host : "", ip);

	/* Free the destroyed mask. */
	delete [] hostmask;

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
Entry *elist_match(EList * list, const char *nick, const char *user, const char *host,
				   uint32 ip)
{
	Entry *e;

	if (!list || !list->entries)
		return NULL;

	for (e = list->entries; e; e = e->next) {
		if (entry_match(e, nick ? nick : "", user ? user : "", host ? host : "", ip))
			return e;
	}

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
Entry *elist_match_mask(EList * list, const char *mask, uint32 ip)
{
	char *hostmask, *nick, *user, *host;
	Entry *res;

	if (!list || !list->entries || !mask)
		return NULL;

	hostmask = sstrdup(mask);

	host = strchr(hostmask, '@');
	if (host) {
		*host++ = '\0';
		user = strchr(hostmask, '!');
		if (user) {
			*user++ = '\0';
			nick = hostmask;
		} else {
			nick = NULL;
			user = hostmask;
		}
	} else {
		nick = NULL;
		user = NULL;
		host = hostmask;
	}

	res = elist_match(list, nick, user, host, ip);

	/* Free the destroyed mask. */
	delete [] hostmask;

	return res;
}

/**
 * Check if a user matches an entry on a list.
 * @param list EntryList that should be matched against
 * @param user The user to match against the entries
 * @return Returns the first matching entry, if none, NULL is returned.
 */
Entry *elist_match_user(EList * list, User * u)
{
	Entry *res;
	char *host;
	uint32 ip = 0;

	if (!list || !list->entries || !u)
		return NULL;

	if (u->hostip == NULL) {
		host = host_resolve(u->host);
		/* we store the just resolved hostname so we don't
		 * need to do this again */
		if (host) {
			u->hostip = sstrdup(host);
		}
	} else {
		host = sstrdup(u->hostip);
	}

	/* Convert the host to an IP.. */
	if (host)
		ip = str_is_ip(host);

	/* Match what we ve got against the lists.. */
	res = elist_match(list, u->nick.c_str(), u->GetIdent().c_str(), u->host, ip);
	if (!res)
		res = elist_match(list, u->nick.c_str(), u->GetIdent().c_str(), u->GetDisplayedHost().c_str(), ip);
	if (!res && !u->GetCloakedHost().empty() && u->GetCloakedHost() != u->GetDisplayedHost())
		res = elist_match(list, u->nick.c_str(), u->GetIdent().c_str(), u->GetCloakedHost().c_str(), ip);

	if (host)
		delete [] host;

	return res;
}

/**
 * Find a entry identical to the given mask..
 * @param list EntryList that should be matched against
 * @param mask The *!*@* mask to match
 * @return Returns the first matching entry, if none, NULL is returned.
 */
Entry *elist_find_mask(EList * list, const char *mask)
{
	Entry *e;

	if (!list || !list->entries || !mask)
		return NULL;

	for (e = list->entries; e; e = e->next) {
		if (!stricmp(e->mask, mask))
			return e;
	}

	return NULL;
}

/**
 * Gets the total memory use of an entrylit.
 * @param list The list we should estimate the mem use of.
 * @return Returns the memory useage of the given list.
 */
long get_memuse(EList * list)
{
	Entry *e;
	long mem = 0;

	if (!list)
		return 0;

	mem += sizeof(EList *);
	mem += sizeof(Entry *) * list->count;
	if (list->entries) {
		for (e = list->entries; e; e = e->next) {
			if (e->nick)
				mem += strlen(e->nick) + 1;
			if (e->user)
				mem += strlen(e->user) + 1;
			if (e->host)
				mem += strlen(e->host) + 1;
			if (e->mask)
				mem += strlen(e->mask) + 1;
		}
	}

	return mem;
}

/*************************************************************************/
