/* Channel-handling routines.
 *
 * (C) 2003-2009 Anope Team
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

	strscpy(this->name, name.c_str(), sizeof(this->name));
	list = &chanlist[HASH(this->name)];
	this->prev = NULL;
	this->next = *list;
	if (*list)
		(*list)->prev = this;
	*list = this;

	this->creation_time = ts;
	this->topic = NULL;
	*this->topic_setter = 0;
	this->bans = this->excepts = this->invites = NULL;
	this->users = NULL;
	this->usercount = 0;
	this->bd = NULL;
	this->server_modetime = this->chanserv_modetime = 0;
	this->server_modecount = this->chanserv_modecount = this->bouncy_modes = this->topic_sync = 0;

	this->ci = cs_findchan(this->name);
	if (this->ci)
	{
		this->ci->c = this;

		check_modes(this);
		stick_all(this->ci);
	}

	if (serv_uplink && is_sync(serv_uplink) && (!(this->topic_sync)))
		restore_topic(name.c_str());

	FOREACH_MOD(I_OnChannelCreate, OnChannelCreate(this));
}

/** Default destructor
 */
Channel::~Channel()
{
	BanData *bd, *next;

	FOREACH_MOD(I_OnChannelDelete, OnChannelDelete(this));

	if (debug)
		alog("debug: Deleting channel %s", this->name);

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

	/* Setting v/h/o/a/q etc */
	if (cm->Type == MODE_STATUS)
	{
		if (param.empty())
		{
			alog("Channel::SetModeInternal() mode %c with no parameter for channel %s", cm->ModeChar, this->name);
			return;
		}

		/* We don't track bots */
		if (findbot(param.c_str()))
			return;

		User *u = finduser(param.c_str());
		if (!u)
		{
			if (debug)
				alog("debug: MODE %s +%c for nonexistant user %s", this->name, cm->ModeChar, param.c_str());
			return;
		}

		if (debug)
			alog("debug: Setting +%c on %s for %s", cm->ModeChar, this->name, u->nick);

		ChannelModeStatus *cms = static_cast<ChannelModeStatus *>(cm);
		/* Set the new status on the user */
		chan_set_user_status(this, u, cms->Status);
		/* Enforce secureops, etc */
		chan_set_correct_modes(u, this, 0);
		return;
	}
	/* Setting b/e/I etc */
	else if (cm->Type == MODE_LIST)
	{
		if (param.empty())
		{
			alog("Channel::SetModeInternal() mode %c with no parameter for channel %s", cm->ModeChar, this->name);
			return;
		}

		ChannelModeList *cml = static_cast<ChannelModeList *>(cm);
		cml->AddMask(this, param.c_str());
		return;
	}

	modes[cm->Name] = true;

	/* Channel mode +P or so was set, mark this channel as persistant */
	if (cm->Name == CMODE_PERM && ci)
	{
		ci->SetFlag(CI_PERSIST);
	}

	if (!param.empty())
	{
		if (cm->Type != MODE_PARAM)
		{
			alog("Channel::SetModeInternal() mode %c for %s with a paramater, but its not a param mode", cm->ModeChar, this->name);
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

	FOREACH_MOD(I_OnChannelModeSet, OnChannelModeSet(this, cm->Name));

	/* Check for mlock */

	/* Non registered channel, no mlock */
	if (!ci || !EnforceMLock)
		return;

	/* If this channel has this mode locked negative */
	if (ci->HasMLock(cm->Name, false))
	{
		/* Remove the mode */
		if (cm->Type == MODE_PARAM)
		{
			ChannelModeParam *cmp = static_cast<ChannelModeParam *>(cmp);

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
		ChannelModeParam *cmp = static_cast<ChannelModeParam *>(cm);
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

	/* Setting v/h/o/a/q etc */
	if (cm->Type == MODE_STATUS)
	{
		if (param.empty())
		{
			alog("Channel::RemoveModeInternal() mode %c with no parameter for channel %s", cm->ModeChar, this->name);
			return;
		}

		/* We don't track bots */
		if (findbot(param.c_str()))
			return;

		User *u = finduser(param.c_str());
		if (!u)
		{
			alog("Channel::RemoveModeInternal() MODE %s +%c for nonexistant user %s", this->name, cm->ModeChar, param.c_str());
			return;
		}

		if (debug)
			alog("debug: Setting +%c on %s for %s", cm->ModeChar, this->name, u->nick);

		ChannelModeStatus *cms = static_cast<ChannelModeStatus *>(cm);
		chan_remove_user_status(this, u, cms->Status);
		return;
	}
	/* Setting b/e/I etc */
	else if (cm->Type == MODE_LIST)
	{
		if (param.empty())
		{
			alog("Channel::SetModeInternal() mode %c with no parameter for channel %s", cm->ModeChar, this->name);
			return;
		}

		ChannelModeList *cml = static_cast<ChannelModeList *>(cm);
		cml->DelMask(this, param.c_str());
		return;
	}

	modes[cm->Name] = false;

	if (cm->Name == CMODE_PERM && ci)
	{
		ci->UnsetFlag(CI_PERSIST);
		if (Config.s_BotServ && ci->bi && usercount == Config.BSMinUsers - 1)
			ircdproto->SendPart(ci->bi, this, NULL);
		if (!users)
			delete this;
	}

	if (cm->Type == MODE_PARAM)
	{
		std::map<ChannelModeName, std::string>::iterator it = Params.find(cm->Name);
		if (it != Params.end())
		{
			Params.erase(it);
		}
	}

	FOREACH_MOD(I_OnChannelModeUnset, OnChannelModeUnset(this, cm->Name));

	/* Check for mlock */

	/* Non registered channel, no mlock */
	if (!ci || !EnforceMLock)
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
				cmp = static_cast<ChannelModeParam *>(cm);

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

	cml = static_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_BAN));

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

	cml = static_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_EXCEPT));

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

	cml = static_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE));

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
void Channel::SetModes(BotInfo *bi, bool EnforceMLock, const std::string &cmodes, ...)
{
	char buf[BUFSIZE] = "";
	va_list args;
	std::string modebuf, sbuf;
	int add = -1;
	va_start(args, cmodes.c_str());
	vsnprintf(buf, BUFSIZE - 1, cmodes.c_str(), args);
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
			ChannelModeParam *cmp = static_cast<ChannelModeParam *>(cm);

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
			alog("warning: ChanSetInternalModes() recieved more modes requiring params than params, modes: %s, ac: %d, j: %d", merge_args(ac, av), ac, j);
		}
	}

	if (j + k + 1 < ac)
	{
		alog("warning: ChanSetInternalModes() recieved more params than modes requiring them, modes: %s, ac: %d, j: %d k: %d", merge_args(ac, av), ac, j, k);
	}
}

/*************************************************************************/

void chan_deluser(User * user, Channel * c)
{
	struct c_userlist *u;

	if (c->ci)
		update_cs_lastseen(user, c->ci);

	for (u = c->users; u && u->user != user; u = u->next);
	if (!u)
		return;

	if (u->ud) {
		if (u->ud->lastline)
			delete [] u->ud->lastline;
		delete u->ud;
	}

	if (u->next)
		u->next->prev = u->prev;
	if (u->prev)
		u->prev->next = u->next;
	else
		c->users = u->next;
	delete u;
	c->usercount--;

	/* Channel is persistant, it shouldn't be deleted and the service bot should stay */
	if (c->ci && c->ci->HasFlag(CI_PERSIST))
		return;

	if (Config.s_BotServ && c->ci && c->ci->bi && c->usercount == Config.BSMinUsers - 1)
		ircdproto->SendPart(c->ci->bi, c, NULL);

	if (!c->users)
		delete c;
}

/*************************************************************************/

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

	if (!chan->modes.count())
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
						cmp = static_cast<ChannelModeParam *>(cm);

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

/* Retrieves the status of an user on a channel */

int chan_get_user_status(Channel * chan, User * user)
{
	struct u_chanlist *uc;

	for (uc = user->chans; uc; uc = uc->next)
		if (uc->chan == chan)
			return uc->status;

	return 0;
}

/*************************************************************************/

/* Has the given user the given status on the given channel? :p */

int chan_has_user_status(Channel * chan, User * user, int16 status)
{
	struct u_chanlist *uc;

	for (uc = user->chans; uc; uc = uc->next) {
		if (uc->chan == chan) {
			if (debug) {
				alog("debug: chan_has_user_status wanted %d the user is %d", status, uc->status);
			}
			return (uc->status & status);
		}
	}
	return 0;
}

/*************************************************************************/

/* Remove the status of an user on a channel */

void chan_remove_user_status(Channel * chan, User * user, int16 status)
{
	struct u_chanlist *uc;

	if (debug >= 2)
		alog("debug: removing user status (%d) from %s for %s", status,
			 user->nick, chan->name);

	for (uc = user->chans; uc; uc = uc->next) {
		if (uc->chan == chan) {
			uc->status &= ~status;
			break;
		}
	}
}

/*************************************************************************/

/* Set the status of an user on a channel */

void chan_set_user_status(Channel * chan, User * user, int16 status)
{
	struct u_chanlist *uc;
	UserMode *um;

	if (debug >= 2)
		alog("debug: setting user status (%d) on %s for %s", status,
			 user->nick, chan->name);

	if (Config.HelpChannel && ((um = ModeManager::FindUserModeByName(UMODE_HELPOP))) && (status & CUS_OP)
		&& (stricmp(chan->name, Config.HelpChannel) == 0)
		&& (!chan->ci || check_access(user, chan->ci, CA_AUTOOP))) {
		if (debug) {
			alog("debug: %s being given helpop for having %d status in %s",
				 user->nick, status, chan->name);
		}

		user->SetMode(um);
	}

	for (uc = user->chans; uc; uc = uc->next) {
		if (uc->chan == chan) {
			uc->status |= status;
			break;
		}
	}
}

/*************************************************************************/

/* Return the Channel structure corresponding to the named channel, or NULL
 * if the channel was not found.  chan is assumed to be non-NULL and valid
 * (i.e. pointing to a channel name of 2 or more characters). */

Channel *findchan(const char *chan)
{
	Channel *c;

	if (!chan || !*chan) {
		if (debug) {
			alog("debug: findchan() called with NULL values");
		}
		return NULL;
	}

	if (debug >= 3)
		alog("debug: findchan(%p)", chan);
	c = chanlist[HASH(chan)];
	while (c) {
		if (stricmp(c->name, chan) == 0) {
			if (debug >= 3)
				alog("debug: findchan(%s) -> %p", chan, static_cast<void *>(c));
			return c;
		}
		c = c->next;
	}
	if (debug >= 3)
		alog("debug: findchan(%s) -> %p", chan, static_cast<void *>(c));
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
	if (debug >= 3)
		alog("debug: firstchan() returning %s",
			 current ? current->name : "NULL (end of list)");
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
	if (debug >= 3)
		alog("debug: nextchan() returning %s",
			 current ? current->name : "NULL (end of list)");
	return current;
}

/*************************************************************************/

/* Return statistics.  Pointers are assumed to be valid. */

void get_channel_stats(long *nrec, long *memuse)
{
	long count = 0, mem = 0;
	Channel *chan;
	struct c_userlist *cu;
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
			for (cu = chan->users; cu; cu = cu->next) {
				mem += sizeof(*cu);
				if (cu->ud) {
					mem += sizeof(*cu->ud);
					if (cu->ud->lastline)
						mem += strlen(cu->ud->lastline) + 1;
				}
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

/* Is the given nick on the given channel? */

int is_on_chan(Channel * c, User * u)
{
	struct u_chanlist *uc;

	for (uc = u->chans; uc; uc = uc->next)
		if (uc->chan == c)
			return 1;

	return 0;
}

/*************************************************************************/

/* Is the given nick on the given channel?
   This function supports links. */

User *nc_on_chan(Channel * c, NickCore * nc)
{
	struct c_userlist *u;

	if (!c || !nc)
		return NULL;

	for (u = c->users; u; u = u->next) {
		if (u->user->nc == nc)
			return u->user;
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
	char *s, *t;
	struct u_chanlist *c, *nextc;
	char *channame;
	time_t ts = time(NULL);

	if (ircd->ts6) {
		user = find_byuid(source);
		if (!user)
			user = finduser(source);
	} else {
		user = finduser(source);
	}
	if (!user) {
		if (debug) {
			alog("debug: JOIN from nonexistent user %s: %s", source,
				 merge_args(ac, av));
		}
		return;
	}

	t = const_cast<char *>(av[0]); // XXX Unsafe cast, this needs reviewing -- CyberBotX
	while (*(s = t)) {
		t = s + strcspn(s, ",");
		if (*t)
			*t++ = 0;

		if (*s == '0') {
			c = user->chans;
			while (c) {
				nextc = c->next;
				channame = sstrdup(c->chan->name);
				FOREACH_MOD(I_OnPrePartChannel, OnPrePartChannel(user, c->chan));
				chan_deluser(user, c->chan);
				FOREACH_MOD(I_OnPartChannel, OnPartChannel(user, findchan(channame), channame, ""));
				delete [] channame;
				delete c;
				c = nextc;
			}
			user->chans = NULL;
			continue;
		}

		chan = findchan(s);

		/* how about not triggering the JOIN event on an actual /part :) -certus */
		FOREACH_MOD(I_OnPreJoinChannel, OnPreJoinChannel(user, s));

		/* Make sure check_kick comes before chan_adduser, so banned users
		 * don't get to see things like channel keys. */
		/* If channel already exists, check_kick() will use correct TS.
		 * Otherwise, we lose. */
		if (check_kick(user, s, ts))
			continue;

		if (ac == 2) {
			ts = strtoul(av[1], NULL, 10);
			if (debug) {
				alog("debug: recieved a new TS for JOIN: %ld",
					 static_cast<long>(ts));
			}
		}

		chan = join_user_update(user, chan, s, ts);
		chan_set_correct_modes(user, chan, 1);

		FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(user, chan));
	}
}

/*************************************************************************/

/* Handle a KICK command.
 *	av[0] = channel
 *	av[1] = nick(s) being kicked
 *	av[2] = reason
 */

void do_kick(const char *source, int ac, const char **av)
{
	BotInfo *bi;
	ChannelInfo *ci;
	User *user;
	char *s, *t;
	struct u_chanlist *c;

	t = const_cast<char *>(av[1]); // XXX unsafe cast, this needs reviewing -- w00t
	while (*(s = t)) {
		t = s + strcspn(s, ",");
		if (*t)
			*t++ = 0;

		/* If it is the bot that is being kicked, we make it rejoin the
		 * channel and stop immediately.
		 *	  --lara
		 */
		if (Config.s_BotServ && (bi = findbot(s)) && (ci = cs_findchan(av[0]))) {
			bot_join(ci);
			continue;
		}

		if (ircd->ts6) {
			user = find_byuid(s);
			if (!user) {
				user = finduser(s);
			}
		} else {
			user = finduser(s);
		}
		if (!user) {
			if (debug) {
				alog("debug: KICK for nonexistent user %s on %s: %s", s,
					 av[0], merge_args(ac - 2, av + 2));
			}
			continue;
		}
		if (debug) {
			alog("debug: kicking %s from %s", user->nick, av[0]);
		}
		for (c = user->chans; c && stricmp(av[0], c->chan->name) != 0;
			 c = c->next);
		if (c)
		{
			FOREACH_MOD(I_OnUserKicked, OnUserKicked(c->chan, user, source, merge_args(ac - 2, av + 2)));
			chan_deluser(user, c->chan);
			if (c->next)
				c->next->prev = c->prev;
			if (c->prev)
				c->prev->next = c->next;
			else
				user->chans = c->next;
			delete c;
		}
	}
}

/*************************************************************************/

/* Handle a PART command.
 *	av[0] = channels to leave
 *	av[1] = reason (optional)
 */

void do_part(const char *source, int ac, const char **av)
{
	User *user;
	char *s, *t;
	struct u_chanlist *c;

	if (ircd->ts6) {
		user = find_byuid(source);
		if (!user)
			user = finduser(source);
	} else {
		user = finduser(source);
	}
	if (!user) {
		if (debug) {
			alog("debug: PART from nonexistent user %s: %s", source,
				 merge_args(ac, av));
		}
		return;
	}
	t = const_cast<char *>(av[0]); // XXX Unsafe cast, this needs reviewing -- CyberBotX
	while (*(s = t)) {
		t = s + strcspn(s, ",");
		if (*t)
			*t++ = 0;
		if (debug)
			alog("debug: %s leaves %s", source, s);
		for (c = user->chans; c && stricmp(s, c->chan->name) != 0;
			 c = c->next);
		if (c) {
			if (!c->chan) {
				alog("user: BUG parting %s: channel entry found but c->chan NULL", s);
				return;
			}
			FOREACH_MOD(I_OnPrePartChannel, OnPrePartChannel(user, c->chan));
			std::string ChannelName = c->chan->name;

			chan_deluser(user, c->chan);

			FOREACH_MOD(I_OnPartChannel, OnPartChannel(user, findchan(ChannelName.c_str()), ChannelName, av[1] ? av[1] : ""));

			if (c->next)
				c->next->prev = c->prev;
			if (c->prev)
				c->prev->next = c->next;
			else
				user->chans = c->next;
			delete c;
		}
	}
}

/*************************************************************************/

/* Handle a SJOIN command.

   On channel creation, syntax is:

   av[0] = timestamp
   av[1] = channel name
   av[2|3|4] = modes   \   depends of whether the modes k and l
   av[3|4|5] = users   /   are set or not.

   When a single user joins an (existing) channel, it is:

   av[0] = timestamp
   av[1] = user

   ============================================================

   Unreal SJOIN

   On Services connect there is
   SJOIN !11LkOb #ircops +nt :@Trystan &*!*@*.aol.com "*@*.home.com

   av[0] = time stamp (base64)
   av[1] = channel
   av[2] = modes
   av[3] = users + bans + exceptions

   On Channel Creation or a User joins an existing
   Luna.NomadIrc.Net SJOIN !11LkW9 #akill :@Trystan
   Luna.NomadIrc.Net SJOIN !11LkW9 #akill :Trystan`

   av[0] = time stamp (base64)
   av[1] = channel
   av[2] = users

*/

void do_sjoin(const char *source, int ac, const char **av)
{
	Channel *c;
	User *user;
	Server *serv;
	struct c_userlist *cu;
	const char *s = NULL;
	char *buf, *end, cubuf[7], *end2, value;
	const char *modes[6];
	int is_sqlined = 0;
	int ts = 0;
	int is_created = 0;
	int keep_their_modes = 1;
	ChannelModeList *cml;

	serv = findserver(servlist, source);

	if (ircd->sjb64) {
		ts = base64dects(av[0]);
	} else {
		ts = strtoul(av[0], NULL, 10);
	}
	c = findchan(av[1]);
	if (c != NULL) {
		if (c->creation_time == 0 || ts == 0)
			c->creation_time = 0;
		else if (c->creation_time > ts) {
			c->creation_time = ts;
			for (cu = c->users; cu; cu = cu->next) {
				c->RemoveMode(NULL, CMODE_OP, cu->user->nick);
				c->RemoveMode(NULL, CMODE_VOICE, cu->user->nick);
			}
			if (c->ci)
			{
				if (c->ci->bi)
				{
					/* This is ugly, but it always works */
					ircdproto->SendPart(c->ci->bi, c, "TS reop");
					bot_join(c->ci);
				}
				/* Make sure +r is set */
				if (ModeManager::FindChannelModeByName(CMODE_REGISTERED))
				{
					c->SetMode(NULL, CMODE_REGISTERED);
				}
			}
			/* XXX simple modes and bans */
		} else if (c->creation_time < ts)
			keep_their_modes = 0;
	} else
		is_created = 1;

	/* Double check to avoid unknown modes that need parameters */
	if (ac >= 4) {
		if (ircd->chansqline) {
			if (!c)
				is_sqlined = check_chan_sqline(av[1]);
		}

		cubuf[0] = '+';
		modes[0] = cubuf;

		/* We make all the users join */
		s = av[ac - 1];		 /* Users are always the last element */

		while (*s) {
			end = const_cast<char *>(strchr(s, ' '));
			if (end)
				*end = 0;

			end2 = cubuf + 1;


			if (ircd->sjoinbanchar) {
				if (*s == ircd->sjoinbanchar && keep_their_modes) {
					buf = myStrGetToken(s, ircd->sjoinbanchar, 1);

					cml = static_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_BAN));
					if (cml->IsValid(buf))
						cml->AddMask(c, buf);

					delete [] buf;
					if (!end)
						break;
					s = end + 1;
					continue;
				}
			}
			if (ircd->sjoinexchar) {
				if (*s == ircd->sjoinexchar && keep_their_modes) {
					buf = myStrGetToken(s, ircd->sjoinexchar, 1);

					cml = static_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_EXCEPT));
					if (cml->IsValid(buf))
						cml->AddMask(c, buf);

					delete [] buf;
					if (!end)
						break;
					s = end + 1;
					continue;
				}
			}

			if (ircd->sjoininvchar) {
				if (*s == ircd->sjoininvchar && keep_their_modes) {
					buf = myStrGetToken(s, ircd->sjoininvchar, 1);

					cml = static_cast<ChannelModeList *>(ModeManager::FindChannelModeByName(CMODE_INVITEOVERRIDE));
					if (cml->IsValid(buf))
						cml->AddMask(c, buf);

					delete [] buf;
					if (!end)
						break;
					s = end + 1;
					continue;
				}
			}

			while ((value = ModeManager::GetStatusChar(*s)))
			{
				*end2++ = value;
				*s++;
			}
			*end2 = 0;

			if (ircd->ts6) {
				user = find_byuid(s);
				if (!user)
					user = finduser(s);
			} else {
				user = finduser(s);
			}

			if (!user) {
				if (debug) {
					alog("debug: SJOIN for nonexistent user %s on %s", s,
						 av[1]);
				}
				return;
			}

			if (is_sqlined && !is_oper(user)) {
				ircdproto->SendKick(findbot(Config.s_OperServ), c, user, "Q-Lined");
			} else {
				if (!check_kick(user, av[1], ts)) {
					FOREACH_MOD(I_OnPreJoinChannel, OnPreJoinChannel(user, av[1]));

					/* Make the user join; if the channel does not exist it
					 * will be created there. This ensures that the channel
					 * is not created to be immediately destroyed, and
					 * that the locked key or topic is not shown to anyone
					 * who joins the channel when empty.
					 */
					c = join_user_update(user, c, av[1], ts);

					/* We update user mode on the channel */
					if (end2 - cubuf > 1 && keep_their_modes) {
						int i;

						for (i = 1; i < end2 - cubuf; i++)
							modes[i] = user->nick;

						ChanSetInternalModes(c, 1 + (end2 - cubuf - 1), modes);
					}

					if (c->ci && (!serv || is_sync(serv))
						&& !c->topic_sync)
						restore_topic(c->name);
					chan_set_correct_modes(user, c, 1);

					FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(user, c));
				}
			}

			if (!end)
				break;
			s = end + 1;
		}

		if (c && keep_their_modes) {
			/* We now update the channel mode. */
			ChanSetInternalModes(c, ac - 3, &av[2]);
		}

		/* Unreal just had to be different */
	} else if (ac == 3 && !ircd->ts6) {
		if (ircd->chansqline) {
			if (!c)
				is_sqlined = check_chan_sqline(av[1]);
		}

		cubuf[0] = '+';
		modes[0] = cubuf;

		/* We make all the users join */
		s = av[2];			  /* Users are always the last element */

		while (*s) {
			end = const_cast<char *>(strchr(s, ' '));
			if (end)
				*end = 0;

			end2 = cubuf + 1;

			while ((value = ModeManager::GetStatusChar(*s)))
			{
				*end2++ = value;
				*s++;
			}
			*end2 = 0;

			if (ircd->ts6) {
				user = find_byuid(s);
				if (!user)
					user = finduser(s);
			} else {
				user = finduser(s);
			}

			if (!user) {
				if (debug) {
					alog("debug: SJOIN for nonexistent user %s on %s", s,
						 av[1]);
				}
				return;
			}

			if (is_sqlined && !is_oper(user)) {
				ircdproto->SendKick(findbot(Config.s_OperServ), c, user, "Q-Lined");
			} else {
				if (!check_kick(user, av[1], ts)) {
					FOREACH_MOD(I_OnPreJoinChannel, OnPreJoinChannel(user, av[1]));

					/* Make the user join; if the channel does not exist it
					 * will be created there. This ensures that the channel
					 * is not created to be immediately destroyed, and
					 * that the locked key or topic is not shown to anyone
					 * who joins the channel when empty.
					 */
					c = join_user_update(user, c, av[1], ts);

					/* We update user mode on the channel */
					if (end2 - cubuf > 1 && keep_their_modes) {
						int i;

						for (i = 1; i < end2 - cubuf; i++)
							modes[i] = user->nick;
						ChanSetInternalModes(c, 1 + (end2 - cubuf - 1), modes);
					}

					chan_set_correct_modes(user, c, 1);

					FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(user, c));
				}
			}

			if (!end)
				break;
			s = end + 1;
		}
	} else if (ac == 3 && ircd->ts6) {
		if (ircd->chansqline) {
			if (!c)
				is_sqlined = check_chan_sqline(av[1]);
		}

		cubuf[0] = '+';
		modes[0] = cubuf;

		/* We make all the users join */
		s = sstrdup(source);	/* Users are always the last element */

		while (*s) {
			end = const_cast<char *>(strchr(s, ' '));
			if (end)
				*end = 0;

			end2 = cubuf + 1;

			while ((value = ModeManager::GetStatusChar(*s)))
			{
				*end2++ = value;
				*s++;
			}
			*end2 = 0;

			if (ircd->ts6) {
				user = find_byuid(s);
				if (!user)
					user = finduser(s);
			} else {
				user = finduser(s);
			}
			if (!user) {
				if (debug) {
					alog("debug: SJOIN for nonexistent user %s on %s", s,
						 av[1]);
				}
				delete [] s;
				return;
			}

			if (is_sqlined && !is_oper(user)) {
				ircdproto->SendKick(findbot(Config.s_OperServ), c, user, "Q-Lined");
			} else {
				if (!check_kick(user, av[1], ts)) {
					FOREACH_MOD(I_OnPreJoinChannel, OnPreJoinChannel(user, av[1]));

					/* Make the user join; if the channel does not exist it
					 * will be created there. This ensures that the channel
					 * is not created to be immediately destroyed, and
					 * that the locked key or topic is not shown to anyone
					 * who joins the channel when empty.
					 */
					c = join_user_update(user, c, av[1], ts);

					/* We update user mode on the channel */
					if (end2 - cubuf > 1 && keep_their_modes) {
						int i;

						for (i = 1; i < end2 - cubuf; i++)
							modes[i] = user->nick;
						ChanSetInternalModes(c, 1 + (end2 - cubuf - 1), modes);
					}

					chan_set_correct_modes(user, c, 1);

					FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(user, c));
				}
			}

			if (!end)
				break;
			s = end + 1;
		}
		delete [] s;
	} else if (ac == 2) {
		if (ircd->ts6) {
			user = find_byuid(source);
			if (!user)
				user = finduser(source);
		} else {
			user = finduser(source);
		}
		if (!user) {
			if (debug) {
				alog("debug: SJOIN for nonexistent user %s on %s", source,
					 av[1]);
			}
			return;
		}

		if (check_kick(user, av[1], ts))
			return;

		if (ircd->chansqline) {
			if (!c)
				is_sqlined = check_chan_sqline(av[1]);
		}

		if (is_sqlined && !is_oper(user)) {
			ircdproto->SendKick(findbot(Config.s_OperServ), c, user, "Q-Lined");
		} else {
			FOREACH_MOD(I_OnPreJoinChannel, OnPreJoinChannel(user, av[1]));

			c = join_user_update(user, c, av[1], ts);
			if (is_created && c->ci)
				restore_topic(c->name);
			chan_set_correct_modes(user, c, 1);

			FOREACH_MOD(I_OnJoinChannel, OnJoinChannel(user, c));
		}
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
				alog("TSMODE enabled but MODE has no valid TS");
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
				alog("debug: MODE %s for nonexistant channel %s", merge_args(ac - 1, av + 1), av[0]);
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
		if (debug) {
			alog("debug: encoded TOPIC TS %s converted to %d", av[2], ts);
		}
	} else {
		ts = strtoul(av[2], NULL, 10);
	}

	topic_time = ts;

	if (!c) {
		if (debug) {
			alog("debug: TOPIC %s for nonexistent channel %s",
				 merge_args(ac - 1, av + 1), av[0]);
		}
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
		&& (strcmp(topicsetter, ci->last_topic_setter) == 0)) {
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

	strscpy(c->topic_setter, topicsetter, sizeof(c->topic_setter));
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
	int status;
	int add_modes = 0;
	int rem_modes = 0;
	ChannelInfo *ci;
	ChannelMode *owner, *admin, *op, *halfop, *voice;

	owner = ModeManager::FindChannelModeByName(CMODE_OWNER);
	admin = ModeManager::FindChannelModeByName(CMODE_PROTECT);
	op = ModeManager::FindChannelModeByName(CMODE_OP);
	halfop = ModeManager::FindChannelModeByName(CMODE_HALFOP);
	voice = ModeManager::FindChannelModeByName(CMODE_VOICE);

	if (!c || !(ci = c->ci))
		return;

	if ((ci->HasFlag(CI_FORBIDDEN)) || (*(c->name) == '+'))
		return;

	status = chan_get_user_status(c, user);

	if (debug)
		alog("debug: Setting correct user modes for %s on %s (current status: %d, %sgiving modes)", user->nick, c->name, status, (give_modes ? "" : "not "));

	/* Changed the second line of this if a bit, to make sure unregistered
	 * users can always get modes (IE: they always have autoop enabled). Before
	 * this change, you were required to have a registered nick to be able
	 * to receive modes. I wonder who added that... *looks at Rob* ;) -GD
	 */
	if (give_modes && (get_ignore(user->nick) == NULL)
		&& (!user->nc || !user->nc->HasFlag(NI_AUTOOP))) {
		if (owner && check_access(user, ci, CA_AUTOOWNER))
			add_modes |= CUS_OWNER;
		else if (admin && check_access(user, ci, CA_AUTOPROTECT))
			add_modes |= CUS_PROTECT;
		if (op && check_access(user, ci, CA_AUTOOP))
			add_modes |= CUS_OP;
		else if (halfop && check_access(user, ci, CA_AUTOHALFOP))
			add_modes |= CUS_HALFOP;
		else if (voice && check_access(user, ci, CA_AUTOVOICE))
			add_modes |= CUS_VOICE;
	}

	/* We check if every mode they have is legally acquired here, and remove
	 * the modes that they're not allowed to have. But only if SECUREOPS is
	 * on, because else every mode is legal. -GD
	 * Unless the channel has just been created. -heinz
	 *	 Or the user matches CA_AUTODEOP... -GD
	 */
	if (((ci->HasFlag(CI_SECUREOPS)) || (c->usercount == 1)
		 || check_access(user, ci, CA_AUTODEOP))
		&& !is_ulined(user->server->name)) {
		if (owner && (status & CUS_OWNER) && !IsFounder(user, ci))
			rem_modes |= CUS_OWNER;
		if (admin && (status & CUS_PROTECT)
			&& !check_access(user, ci, CA_AUTOPROTECT)
			&& !check_access(user, ci, CA_PROTECTME))
			rem_modes |= CUS_PROTECT;
		if (op && (status & CUS_OP) && !check_access(user, ci, CA_AUTOOP)
			&& !check_access(user, ci, CA_OPDEOPME))
			rem_modes |= CUS_OP;
		if (halfop && (status & CUS_HALFOP)
			&& !check_access(user, ci, CA_AUTOHALFOP)
			&& !check_access(user, ci, CA_HALFOPME))
			rem_modes |= CUS_HALFOP;
	}

	/* No modes to add or remove, exit function -GD */
	if (!add_modes && !rem_modes)
		return;

	if (add_modes > 0)
	{
		if (owner && (add_modes & CUS_OWNER) && !(status & CUS_OWNER))
			c->SetMode(NULL, CMODE_OWNER, user->nick);
		else
			add_modes &= ~CUS_OWNER;

		if (admin && (add_modes & CUS_PROTECT) && !(status & CUS_PROTECT))
			c->SetMode(NULL, CMODE_PROTECT, user->nick);
		else
			add_modes &= ~CUS_PROTECT;

		if (op && (add_modes & CUS_OP) && !(status & CUS_OP))
			c->SetMode(NULL, CMODE_OP, user->nick);
		else
			add_modes &= ~CUS_OP;

		if (halfop && (add_modes & CUS_HALFOP) && !(status & CUS_HALFOP))
			c->SetMode(NULL, CMODE_HALFOP, user->nick);
		else
			add_modes &= ~CUS_HALFOP;

		if (voice && (add_modes & CUS_VOICE) && !(status & CUS_VOICE))
			c->SetMode(NULL, CMODE_VOICE, user->nick);
		else
			add_modes &= ~CUS_VOICE;
	}
	if (rem_modes > 0)
	{
		if (owner && rem_modes & CUS_OWNER)
		{
			c->RemoveMode(NULL, CMODE_OWNER, user->nick);
		}

		if (admin && rem_modes & CUS_PROTECT)
		{
			c->RemoveMode(NULL, CMODE_PROTECT, user->nick);
		}

		if (op && rem_modes & CUS_OP)
		{
			c->RemoveMode(NULL, CMODE_OP, user->nick);
		}

		if (halfop && rem_modes & CUS_HALFOP)
		{
			c->RemoveMode(NULL, CMODE_HALFOP, user->nick);
		}
	}

	/* Here, both can be empty again due to the "isn't it set already?"
	 * checks above. -GD
	 */
	if (!add_modes && !rem_modes)
		return;

	if (add_modes > 0)
		chan_set_user_status(c, user, add_modes);
	if (rem_modes > 0)
		chan_remove_user_status(c, user, rem_modes);
}

/*************************************************************************/

/* Add/remove a user to/from a channel, creating or deleting the channel as
 * necessary.  If creating the channel, restore mode lock and topic as
 * necessary.  Also check for auto-opping and auto-voicing.
 */

void chan_adduser2(User * user, Channel * c)
{
	struct c_userlist *u;

	u = new c_userlist;
	u->prev = NULL;
	u->next = c->users;
	if (c->users)
		c->users->prev = u;
	c->users = u;
	u->user = user;
	u->ud = NULL;
	c->usercount++;

	if (get_ignore(user->nick) == NULL) {
		if (c->ci && (check_access(user, c->ci, CA_MEMO))
			&& (c->ci->memos.memos.size() > 0)) {
			if (c->ci->memos.memos.size() == 1) {
				notice_lang(Config.s_MemoServ, user, MEMO_X_ONE_NOTICE,
							c->ci->memos.memos.size(), c->ci->name);
			} else {
				notice_lang(Config.s_MemoServ, user, MEMO_X_MANY_NOTICE,
							c->ci->memos.memos.size(), c->ci->name);
			}
		}
		/* Added channelname to entrymsg - 30.03.2004, Certus */
		/* Also, don't send the entrymsg when bursting -GD */
		if (c->ci && c->ci->entry_message && is_sync(user->server))
			user->SendMessage(whosends(c->ci)->nick, "[%s] %s", c->name, c->ci->entry_message);
	}

	/**
	 * We let the bot join even if it was an ignored user, as if we don't,
	 * and the ignored user dosnt just leave, the bot will never
	 * make it into the channel, leaving the channel botless even for
	 * legit users - Rob
	 * But don't join the bot if the channel is persistant - Adam
	 **/
	if (Config.s_BotServ && c->ci && c->ci->bi && !c->ci->HasFlag(CI_PERSIST))
	{
		if (c->usercount == Config.BSMinUsers)
			bot_join(c->ci);
	}
	if (Config.s_BotServ && c->ci && c->ci->bi)
	{
		if (c->usercount >= Config.BSMinUsers && (c->ci->botflags.HasFlag(BS_GREET))
			&& user->nc && user->nc->greet
			&& check_access(user, c->ci, CA_GREET)) {
			/* Only display the greet if the main uplink we're connected
			 * to has synced, or we'll get greet-floods when the net
			 * recovers from a netsplit. -GD
			 */
			if (is_sync(user->server)) {
				ircdproto->SendPrivmsg(c->ci->bi, c->name, "[%s] %s",
								  user->nc->display, user->nc->greet);
				c->ci->bi->lastmsg = time(NULL);
			}
		}
	}
}

/*************************************************************************/

Channel *join_user_update(User * user, Channel * chan, const char *name,
						  time_t chants)
{
	struct u_chanlist *c;

	/* If it's a new channel, so we need to create it first. */
	if (!chan)
		chan = new Channel(name, chants);
	else
	{
		// Check chants against 0, as not every ircd sends JOIN with a TS.
		if (chan->creation_time > chants && chants != 0)
		{
			struct c_userlist *cu;

			chan->creation_time = chants;
			for (cu = chan->users; cu; cu = cu->next)
			{
				chan->RemoveMode(NULL, CMODE_OP, cu->user->nick);
				chan->RemoveMode(NULL, CMODE_VOICE, cu->user->nick);
			}
			if (chan->ci)
			{
				if (chan->ci->bi)
				{
					/* This is ugly, but it always works */
					ircdproto->SendPart(chan->ci->bi, chan, "TS reop");
					bot_join(chan->ci);
				}
				/* Make sure +r is set */
				if (ModeManager::FindChannelModeByName(CMODE_REGISTERED))
				{
					chan->SetMode(NULL, CMODE_REGISTERED);
				}
			}
			/* XXX simple modes and bans */
		}
	}

	if (debug)
		alog("debug: %s joins %s", user->nick, chan->name);

	c = new u_chanlist;
	c->prev = NULL;
	c->next = user->chans;
	if (user->chans)
		user->chans->prev = c;
	user->chans = c;
	c->chan = chan;
	c->status = 0;

	chan_adduser2(user, chan);

	return chan;
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
		c->SetModes(bi, false, modes);
	}
}

/*************************************************************************/

void restore_unsynced_topics()
{
	Channel *c;

	for (c = firstchan(); c; c = nextchan()) {
		if (!(c->topic_sync))
			restore_topic(c->name);
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
int entry_match(Entry * e, const char *nick, const char *user, const char *host, uint32 ip)
{
	/* If we don't get an entry, or it s an invalid one, no match ~ Viper */
	if (!e || !e->FlagCount())
		return 0;

	if (ircd->cidrchanbei && (e->HasFlag(ENTRYTYPE_CIDR4)) &&
		(!ip || (ip && ((ip & e->cidr_mask) != e->cidr_ip))))
		return 0;
	if ((e->HasFlag(ENTRYTYPE_NICK))
		&& (!nick || stricmp(e->nick, nick) != 0))
		return 0;
	if ((e->HasFlag(ENTRYTYPE_USER))
		&& (!user || stricmp(e->user, user) != 0))
		return 0;
	if ((e->HasFlag(ENTRYTYPE_HOST))
		&& (!user || stricmp(e->host, host) != 0))
		return 0;
	if ((e->HasFlag(ENTRYTYPE_NICK_WILD))
		&& !Anope::Match(nick, e->nick, false))
		return 0;
	if ((e->HasFlag(ENTRYTYPE_USER_WILD))
		&& !Anope::Match(user, e->user, false))
		return 0;
	if ((e->HasFlag(ENTRYTYPE_HOST_WILD))
		&& !Anope::Match(host, e->host, false))
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

	res = entry_match(e, nick, user, host, ip);

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
		if (entry_match(e, nick, user, host, ip))
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
	res = elist_match(list, u->nick, u->GetIdent().c_str(), u->host, ip);
	if (!res)
		res = elist_match(list, u->nick, u->GetIdent().c_str(), u->GetDisplayedHost().c_str(), ip);
	if (!res && !u->GetCloakedHost().empty() && u->GetCloakedHost() != u->GetDisplayedHost())
		res = elist_match(list, u->nick, u->GetIdent().c_str(), u->GetCloakedHost().c_str(), ip);

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
