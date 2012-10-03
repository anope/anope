/* Channel-handling routines.
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
#include "channels.h"
#include "regchannel.h"
#include "logger.h"
#include "modules.h"
#include "users.h"
#include "bots.h"
#include "servers.h"
#include "protocol.h"
#include "users.h"
#include "config.h"
#include "access.h"
#include "extern.h"
#include "sockets.h"

channel_map ChannelList;

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

		ChannelStatus flags = *uc->Status;
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

	this->CheckModes();

	for (CUserList::const_iterator it = this->users.begin(), it_end = this->users.end(); it != it_end; ++it)
		chan_set_correct_modes((*it)->user, this, 1, false);
	
	if (this->ci && Me && Me->IsSynced())
		this->ci->RestoreTopic();
}

void Channel::Sync()
{
	if (!this->HasMode(CMODE_PERM) && (this->users.empty() || (this->users.size() == 1 && this->ci && this->ci->bi && *this->ci->bi == this->users.front()->user)))
	{
		this->Hold();
	}
	if (this->ci)
	{
		this->CheckModes();

		if (Me && Me->IsSynced())
			this->ci->RestoreTopic();
	}
}

void Channel::CheckModes()
{
	if (this->bouncy_modes)
		return;

	/* Check for mode bouncing */
	if (this->server_modecount >= 3 && this->chanserv_modecount >= 3)
	{
		Log() << "Warning: unable to set modes on channel " << this->name << ". Are your servers' U:lines configured correctly?";
		this->bouncy_modes = 1;
		return;
	}

	if (this->chanserv_modetime != Anope::CurTime)
	{
		this->chanserv_modecount = 0;
		this->chanserv_modetime = Anope::CurTime;
	}
	this->chanserv_modecount++;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnCheckModes, OnCheckModes(this));
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (this->ci)
		for (ChannelInfo::ModeList::const_iterator it = this->ci->GetMLock().begin(), it_end = this->ci->GetMLock().end(); it != it_end; ++it)
		{
			const ModeLock *ml = it->second;
			ChannelMode *cm = ModeManager::FindChannelModeByName(ml->name);
			if (!cm)
				continue;

			if (cm->Type == MODE_REGULAR)
			{
				if (!this->HasMode(cm->Name) && ml->set)
					this->SetMode(NULL, cm);
				else if (this->HasMode(cm->Name) && !ml->set)
					this->RemoveMode(NULL, cm);
			}
			else if (cm->Type == MODE_PARAM)
			{
				/* If the channel doesnt have the mode, or it does and it isn't set correctly */
				if (ml->set)
				{
					Anope::string param;
					this->GetParam(cm->Name, param);

					if (!this->HasMode(cm->Name) || (!param.empty() && !ml->param.empty() && !param.equals_cs(ml->param)))
						this->SetMode(NULL, cm, ml->param);
				}
				else
				{
					if (this->HasMode(cm->Name))
						this->RemoveMode(NULL, cm);
				}
	
			}
			else if (cm->Type == MODE_LIST)
			{
				if (ml->set)
					this->SetMode(NULL, cm, ml->param);
				else
					this->RemoveMode(NULL, cm, ml->param);
			}
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
}

/** Remove a user internally from the channel
 * @param u The user
 */
void Channel::DeleteUser(User *user)
{
	if (this->ci)
		update_cs_lastseen(user, this->ci);

	Log(user, this, "leaves");
	FOREACH_MOD(I_OnLeaveChannel, OnLeaveChannel(user, this));

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
		Log(LOG_DEBUG) << "Channel::DeleteUser() tried to delete nonexistant channel " << this->name << " from " << user->nick << "'s channel list";
	else
	{
		delete *uit;
		user->chans.erase(uit);
	}

	/* Channel is persistent, it shouldn't be deleted and the service bot should stay */
	if (this->HasFlag(CH_PERSIST) || (this->ci && this->ci->HasFlag(CI_PERSIST)))
		return;

	/* Channel is syncing from a netburst, don't destroy it as more users are probably wanting to join immediatly
	 * We also don't part the bot here either, if necessary we will part it after the sync
	 */
	if (this->HasFlag(CH_SYNCING))
		return;

	/* Additionally, do not delete this channel if ChanServ/a BotServ bot is inhabiting it */
	if (this->HasFlag(CH_INHABIT))
		return;

	if (this->users.empty())
		delete this;
}

/** Check if the user is on the channel
 * @param u The user
 * @return A user container if found, else NULL
 */
UserContainer *Channel::FindUser(const User *u) const
{
	for (CUserList::const_iterator it = this->users.begin(), it_end = this->users.end(); it != it_end; ++it)
		if ((*it)->user == u)
			return *it;
	return NULL;
}

/** Check if a user has a status on a channel
 * @param u The user
 * @param cms The status mode, or NULL to represent no status
 * @return true or false
 */
bool Channel::HasUserStatus(const User *u, ChannelModeStatus *cms) const
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
bool Channel::HasUserStatus(const User *u, ChannelModeName Name) const
{
	return HasUserStatus(u, anope_dynamic_static_cast<ChannelModeStatus *>(ModeManager::FindChannelModeByName(Name)));
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
 * @param setter The user who is setting the mode
 * @param cm The mode
 * @param param The param
 * @param EnforeMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::SetModeInternal(MessageSource &setter, ChannelMode *cm, const Anope::string &param, bool EnforceMLock)
{
	if (!cm)
		return;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnChannelModeSet, OnChannelModeSet(this, setter, cm->Name, param));

	/* Setting v/h/o/a/q etc */
	if (cm->Type == MODE_STATUS)
	{
		if (param.empty())
		{
			Log() << "Channel::SetModeInternal() mode " << cm->ModeChar << " with no parameter for channel " << this->name;
			return;
		}

		User *u = finduser(param);

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
			chan_set_correct_modes(u, this, 0, false);
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
		ChannelModeList *cml = anope_dynamic_static_cast<ChannelModeList *>(cm);
		cml->OnAdd(this, param);
	}

	/* Channel mode +P or so was set, mark this channel as persistent */
	if (cm->Name == CMODE_PERM)
	{
		this->SetFlag(CH_PERSIST);
		if (this->ci)
			this->ci->SetFlag(CI_PERSIST);
	}

	/* Check if we should enforce mlock */
	if (!EnforceMLock || MOD_RESULT == EVENT_STOP)
		return;

	this->CheckModes();
}

/** Remove a mode internally on a channel, this is not sent out to the IRCd
 * @param setter The user who is unsetting the mode
 * @param cm The mode
 * @param param The param
 * @param EnforceMLock true if mlocks should be enforced, false to override mlock
 */
void Channel::RemoveModeInternal(MessageSource &setter, ChannelMode *cm, const Anope::string &param, bool EnforceMLock)
{
	if (!cm)
		return;

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnChannelModeUnset, OnChannelModeUnset(this, setter, cm->Name, param));

	/* Setting v/h/o/a/q etc */
	if (cm->Type == MODE_STATUS)
	{
		if (param.empty())
		{
			Log() << "Channel::RemoveModeInternal() mode " << cm->ModeChar << " with no parameter for channel " << this->name;
			return;
		}

		BotInfo *bi = findbot(param);
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
			if (this->ci && this->ci->bi && this->ci->bi == bi)
			{
				if (Config->BotModeList.HasFlag(cm->Name))
					this->SetMode(bi, cm, bi->nick);
			}

			chan_set_correct_modes(u, this, 0, false);
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
		ChannelModeList *cml = anope_dynamic_static_cast<ChannelModeList *>(cm);
		cml->OnDel(this, param);
	}

	if (cm->Name == CMODE_PERM)
	{
		this->UnsetFlag(CH_PERSIST);

		if (this->ci)
			this->ci->UnsetFlag(CI_PERSIST);

		if (this->users.empty())
		{
			delete this;
			return;
		}
	}

	/* Check for mlock */

	if (!EnforceMLock || MOD_RESULT == EVENT_STOP)
		return;

	this->CheckModes();
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
		ChannelModeParam *cmp = anope_dynamic_static_cast<ChannelModeParam *>(cm);
		if (!cmp->IsValid(param))
			return;

		Anope::string cparam;
		if (GetParam(cm->Name, cparam) && cparam.equals_cs(param))
			return;
	}
	else if (cm->Type == MODE_STATUS)
	{
		User *u = finduser(param);
		if (!u || HasUserStatus(u, anope_dynamic_static_cast<ChannelModeStatus *>(cm)))
			return;
	}
	else if (cm->Type == MODE_LIST)
	{
		ChannelModeList *cml = anope_dynamic_static_cast<ChannelModeList *>(cm);
		if (this->HasMode(cm->Name, param) || !cml->IsValid(param))
			return;
	}

	ModeManager::StackerAdd(bi, this, cm, true, param);
	MessageSource ms(bi);
	SetModeInternal(ms, cm, param, EnforceMLock);
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
		if (!u || !HasUserStatus(u, anope_dynamic_static_cast<ChannelModeStatus *>(cm)))
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
		ChannelModeParam *cmp = anope_dynamic_static_cast<ChannelModeParam *>(cm);
		if (!cmp->MinusNoArg)
			this->GetParam(cmp->Name, realparam);
	}

	ModeManager::StackerAdd(bi, this, cm, false, realparam);
	MessageSource ms(bi);
	RemoveModeInternal(ms, cm, realparam, EnforceMLock);
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

void Channel::SetModesInternal(MessageSource &source, const Anope::string &mode, time_t ts, bool EnforceMLock)
{
	if (source.GetServer())
	{
		if (Anope::CurTime != this->server_modetime)
		{
			this->server_modecount = 0;
			this->server_modetime = Anope::CurTime;
		}

		++this->server_modecount;
	}

	if (ts > this->creation_time)
		return;
	else if (ts < this->creation_time)
	{
		Log(LOG_DEBUG) << "Changing TS of " << this->name << " from " << this->creation_time << " to " << ts;
		this->creation_time = ts;
		this->Reset();
	}

	User *setter = source.GetUser();
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
				this->SetModeInternal(source, cm, "", EnforceMLock);
			else
				this->RemoveModeInternal(source, cm, "", EnforceMLock);
			continue;
		}
		else if (cm->Type == MODE_PARAM)
		{
			ChannelModeParam *cmp = anope_dynamic_static_cast<ChannelModeParam *>(cm);

			if (!add && cmp->MinusNoArg)
			{
				this->RemoveModeInternal(source, cm, "", EnforceMLock);
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
				this->SetModeInternal(source, cm, token, EnforceMLock);
			else
				this->RemoveModeInternal(source, cm, token, EnforceMLock);
		}
		else
			Log() << "warning: Channel::SetModesInternal() recieved more modes requiring params than params, modes: " << mode;
	}

	if (!this_reference)
		return;

	if (setter)
		Log(setter, this, "mode") << modestring << paramstring;
	else
		Log(LOG_DEBUG) << source.GetName() << " is setting " << this->name << " to " << modestring << paramstring;
}

/** Kick a user from a channel internally
 * @param source The sender of the kick
 * @param nick The nick being kicked
 * @param reason The reason for the kick
 */
void Channel::KickInternal(MessageSource &source, const Anope::string &nick, const Anope::string &reason)
{
	User *sender = source.GetUser();
	User *target = finduser(nick);
	if (!target)
	{
		Log() << "Channel::KickInternal got a nonexistent user " << nick << " on " << this->name << ": " << reason;
		return;
	}

	BotInfo *bi = NULL;
	if (target->server == Me)
		bi = findbot(nick);

	if (sender)
		Log(sender, this, "kick") << "kicked " << target->nick << " (" << reason << ")";
	else
		Log(target, this, "kick") << "was kicked by " << source.GetSource() << " (" << reason << ")";

	Anope::string chname = this->name;

	if (target->FindChannel(this))
	{
		FOREACH_MOD(I_OnUserKicked, OnUserKicked(this, target, source, reason));
		if (bi)
			this->SetFlag(CH_INHABIT);
		this->DeleteUser(target);
	}
	else
		Log() << "Channel::KickInternal got kick for user " << target->nick << " from " << source.GetSource() << " who isn't on channel " << this->name;

	/* Bots get rejoined */
	if (bi)
	{
		bi->Join(this, &Config->BotModeList);
		this->UnsetFlag(CH_INHABIT);
	}
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
	
	if (bi == NULL)
		bi = this->ci->WhoSends();

	EventReturn MOD_RESULT;
	FOREACH_RESULT(I_OnBotKick, OnBotKick(bi, this, u, buf));
	if (MOD_RESULT == EVENT_STOP)
		return false;
	ircdproto->SendKick(bi, this, u, "%s", buf);
	MessageSource ms(bi);
	this->KickInternal(ms, u->nick, buf);
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
			ChannelModeParam *cmp = anope_dynamic_static_cast<ChannelModeParam *>(cm);

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

	Log(LOG_DEBUG) << "Topic of " << this->name << " changed by " << (u ? u->nick : user) << " to " << newtopic;

	FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(this, user, this->topic));

	if (this->ci)
		this->ci->CheckTopic();
}

void Channel::ChangeTopic(const Anope::string &user, const Anope::string &newtopic, time_t ts)
{
	User *u = finduser(user);

	ircdproto->SendTopic(this->ci->WhoSends(), this, newtopic, u ? u->nick : user, ts);

	this->topic = newtopic;
	this->topic_setter = u ? u->nick : user;
	this->topic_time = ts;

	FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(this, user, this->topic));

	if (this->ci)
		this->ci->CheckTopic();
}

/** A timer used to keep the BotServ bot/ChanServ in the channel
 * after kicking the last user in a channel
 */
class CoreExport ChanServTimer : public Timer
{
 private:
	dynamic_reference<Channel> c;

 public:
 	/** Default constructor
	 * @param chan The channel
	 */
	ChanServTimer(Channel *chan) : Timer(Config->CSInhabit), c(chan)
	{
		BotInfo *bi = findbot(Config->ChanServ);
		if (!bi || !c)
			return;
		c->SetFlag(CH_INHABIT);
		if (!c->ci || !c->ci->bi)
			bi->Join(c);
		else if (!c->FindUser(c->ci->bi))
			c->ci->bi->Join(c);
	}

	/** Called when the delay is up
	 * @param The current time
	 */
	void Tick(time_t)
	{
		if (!c)
			return;

		c->UnsetFlag(CH_INHABIT);

		if (!c->ci || !c->ci->bi)
		{
			BotInfo *bi = findbot(Config->ChanServ);
			if (bi)
				bi->Part(c);
		}
		else if (c->users.size() == 1 || c->users.size() < Config->BSMinUsers)
			c->ci->bi->Part(c);
	}
};

void Channel::Hold()
{
	new ChanServTimer(this);
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

/**
 * Set the correct modes, or remove the ones granted without permission,
 * for the specified user on ths specified channel. This doesn't give
 * modes to ignored users, but does remove them if needed.
 * @param user The user to give/remove modes to/from
 * @param c The channel to give/remove modes on
 * @param give_modes Set to 1 to give modes, 0 to not give modes
 * @return void
 **/
void chan_set_correct_modes(const User *user, Channel *c, int give_modes, bool check_noop)
{
	ChannelMode *owner = ModeManager::FindChannelModeByName(CMODE_OWNER),
		*admin = ModeManager::FindChannelModeByName(CMODE_PROTECT),
		*op = ModeManager::FindChannelModeByName(CMODE_OP),
		*halfop = ModeManager::FindChannelModeByName(CMODE_HALFOP),
		*voice = ModeManager::FindChannelModeByName(CMODE_VOICE);

	if (user == NULL || c == NULL)
		return;
	
	ChannelInfo *ci = c->ci;

	if (ci == NULL)
		return;

	Log(LOG_DEBUG) << "Setting correct user modes for " << user->nick << " on " << c->name << " (" << (give_modes ? "" : "not ") << "giving modes)";

	AccessGroup u_access = ci->AccessFor(user);

	if (give_modes && (!user->Account() || user->Account()->HasFlag(NI_AUTOOP)) && (!check_noop || !ci->HasFlag(CI_NOAUTOOP)))
	{
		if (owner && u_access.HasPriv("AUTOOWNER"))
			c->SetMode(NULL, CMODE_OWNER, user->nick);
		else if (admin && u_access.HasPriv("AUTOPROTECT"))
			c->SetMode(NULL, CMODE_PROTECT, user->nick);

		if (op && u_access.HasPriv("AUTOOP"))
			c->SetMode(NULL, CMODE_OP, user->nick);
		else if (halfop && u_access.HasPriv("AUTOHALFOP"))
			c->SetMode(NULL, CMODE_HALFOP, user->nick);
		else if (voice && u_access.HasPriv("AUTOVOICE"))
			c->SetMode(NULL, CMODE_VOICE, user->nick);
	}
	/* If this channel has secureops or the channel is syncing and they are not ulined, check to remove modes */
	if ((ci->HasFlag(CI_SECUREOPS) || (c->HasFlag(CH_SYNCING) && user->server->IsSynced())) && !user->server->IsULined())
	{
		if (owner && !u_access.HasPriv("AUTOOWNER") && !u_access.HasPriv("OWNERME"))
			c->RemoveMode(NULL, CMODE_OWNER, user->nick);

		if (admin && !u_access.HasPriv("AUTOPROTECT") && !u_access.HasPriv("PROTECTME"))
			c->RemoveMode(NULL, CMODE_PROTECT, user->nick);

		if (op && c->HasUserStatus(user, CMODE_OP) && !u_access.HasPriv("AUTOOP") && !u_access.HasPriv("OPDEOPME"))
			c->RemoveMode(NULL, CMODE_OP, user->nick);

		if (halfop && !u_access.HasPriv("AUTOHALFOP") && !u_access.HasPriv("HALFOPME"))
			c->RemoveMode(NULL, CMODE_HALFOP, user->nick);
	}

	// Check mlock
	for (ChannelInfo::ModeList::const_iterator it = ci->GetMLock().begin(), it_end = ci->GetMLock().end(); it != it_end; ++it)
	{
		const ModeLock *ml = it->second;
		ChannelMode *cm = ModeManager::FindChannelModeByName(ml->name);
		if (!cm || cm->Type != MODE_STATUS)
			continue;

		if (Anope::Match(user->nick, ml->param) || Anope::Match(user->GetDisplayedMask(), ml->param))
		{
			if ((ml->set && !c->HasUserStatus(user, ml->name)) || (!ml->set && c->HasUserStatus(user, ml->name)))
			{
				if (ml->set)
					c->SetMode(NULL, cm, user->nick, false);
				else if (!ml->set)
					c->RemoveMode(NULL, cm, user->nick, false);
			}
		}
	}
}

/*************************************************************************/

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
				sockaddrs addr(_realhost.substr(0, sl));
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
bool Entry::Matches(const User *u, bool full) const
{
	bool ret = true;
	
	if (this->HasFlag(ENTRYTYPE_CIDR))
	{
		try
		{
			if (full)
			{
				cidr cidr_mask(this->host, this->cidr_len);
				sockaddrs addr(u->ip);
				if (!cidr_mask.match(addr))
					ret = false;
			}
			/* If we're not matching fully and their displayed host isnt their IP */
			else if (u->ip != u->GetDisplayedHost())
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
		!this->host.equals_ci(u->ip))))
		ret = false;
	if (this->HasFlag(ENTRYTYPE_NICK_WILD) && !Anope::Match(u->nick, this->nick))
		ret = false;
	if (this->HasFlag(ENTRYTYPE_USER_WILD) && !Anope::Match(u->GetVIdent(), this->user) && (!full ||
		!Anope::Match(u->GetIdent(), this->user)))
		ret = false;
	if (this->HasFlag(ENTRYTYPE_HOST_WILD) && !Anope::Match(u->GetDisplayedHost(), this->host) && (!full ||
		(!Anope::Match(u->host, this->host) && !Anope::Match(u->chost, this->host) &&
		!Anope::Match(u->vhost, this->host) && !Anope::Match(u->ip, this->host))))
		ret = false;
	
	ChannelMode *cm = ModeManager::FindChannelModeByName(this->modename);
	if (cm != NULL && cm->Type == MODE_LIST)
	{
		ChannelModeList *cml = anope_dynamic_static_cast<ChannelModeList *>(cm);
		if (cml->Matches(u, this))
			ret = true;
	}

	return ret;
}

