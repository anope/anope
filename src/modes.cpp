/* Mode support
 *
 * (C) 2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * $Id$
 *
 */

#include "services.h"

std::map<char, UserMode *> ModeManager::UserModesByChar;
std::map<UserModeName, UserMode *> ModeManager::UserModesByName;
/* Channel modes */
std::map<char, ChannelMode *> ModeManager::ChannelModesByChar;
std::map<ChannelModeName, ChannelMode *> ModeManager::ChannelModesByName;
/* Although there are two different maps for UserModes and ChannelModes
 * the pointers in each are the same. This is used to increase
 * efficiency.
 */

/** Add a user mode to Anope
 * @param Mode The mode
 * @param um A UserMode or UserMode derived class
 * @return true on success, false on error
 */
bool ModeManager::AddUserMode(char Mode, UserMode *um)
{
	bool ret = ModeManager::UserModesByChar.insert(std::make_pair(Mode, um)).second;
	if (ret)
		ret = ModeManager::UserModesByName.insert(std::make_pair(um->Name, um)).second;

	return ret;
}

/** Add a channel mode to Anope
 * @param Mode The mode
 * @param cm A ChannelMode or ChannelMode derived class
 * @return true on success, false on error
 */
bool ModeManager::AddChannelMode(char Mode, ChannelMode *cm)
{
	bool ret = ModeManager::ChannelModesByChar.insert(std::make_pair(Mode, cm)).second;
	if (ret)
		ret = ModeManager::ChannelModesByName.insert(std::make_pair(cm->Name, cm)).second;

	return ret;
}

/** Find a channel mode
 * @param Mode The mode
 * @return The mode class
 */
ChannelMode *ModeManager::FindChannelModeByChar(char Mode)
{
	std::map<char, ChannelMode *>::iterator it = ModeManager::ChannelModesByChar.find(Mode);

	if (it != ModeManager::ChannelModesByChar.end())
	{
		return it->second;
	}

	return NULL;
}

/** Find a user mode
 * @param Mode The mode
 * @return The mode class
 */
UserMode *ModeManager::FindUserModeByChar(char Mode)
{
	std::map<char, UserMode *>::iterator it = ModeManager::UserModesByChar.find(Mode);

	if (it != ModeManager::UserModesByChar.end())
	{
		return it->second;
	}

	return NULL;
}

/** Find a channel mode
 * @param Mode The modename
 * @return The mode class
 */
ChannelMode *ModeManager::FindChannelModeByName(ChannelModeName Name)
{
	std::map<ChannelModeName, ChannelMode *>::iterator it = ModeManager::ChannelModesByName.find(Name);

	if (it != ModeManager::ChannelModesByName.end())
	{
		return it->second;
	}

	return NULL;
}

/** Find a user mode
 * @param Mode The modename
 * @return The mode class
 */
UserMode *ModeManager::FindUserModeByName(UserModeName Name)
{
	std::map<UserModeName, UserMode *>::iterator it = ModeManager::UserModesByName.find(Name);

	if (it != ModeManager::UserModesByName.end())
	{
		return it->second;
	}

	return NULL;
}

/** Gets the channel mode char for a symbol (eg + returns v)
 * @param Value The symbol
 * @return The char
 */
char ModeManager::GetStatusChar(char Value)
{
	std::map<char, ChannelMode *>::iterator it;
	ChannelMode *cm;
	ChannelModeStatus *cms;

	for (it = ModeManager::ChannelModesByChar.begin(); it != ModeManager::ChannelModesByChar.end(); ++it)
	{
		cm = it->second;

		if (cm->Type == MODE_STATUS)
		{
			cms = static_cast<ChannelModeStatus *>(cm);

			if (Value == cms->Symbol)
			{
				return it->first;
			}
		}
	}

	return 0;
}

/** Is the key valid
 * @param value The key
 * @return true or false
 */
bool ChannelModeKey::IsValid(const char *value)
{
	if (value && *value != ':' && !strchr(value, ','))
		return true;

	return false;
}

/** Can admin only mode be set by the user
 * @param u The user - can be NULL if defcon is checking
 * @return true or false
 */
bool ChannelModeAdmin::CanSet(User *u)
{
	if (u && is_oper(u))
		return true;

	return false;
}

/** Can oper only mode be set by the user
 * @param u The user - can be NULL if defcon is checking
 * @return true or false
 */
bool ChannelModeOper::CanSet(User *u)
{
	if (u && is_oper(u))
		return true;

	return false;
}

/** Can the user set the registerd mode?
 * No.
 * @return false
 */
bool ChannelModeRegistered::CanSet(User *u)
{
	return false;
}

/** Add a ban to the channel
 * @param chan The channel
 * @param mask The ban
 */
void ChannelModeBan::AddMask(Channel *chan, const char *mask)
{
	Entry *ban;

	/* check for NULL values otherwise we will segfault */
	if (!chan || !mask)
	{
		if (debug)
			alog("debug: add_ban called with NULL values");
		return;
	}

	/* Check if the list already exists, if not create it.
	 * Create a new ban and add it to the list.. ~ Viper */
	if (!chan->bans)
		chan->bans = list_create();
	ban = entry_add(chan->bans, mask);

	if (!ban)
		fatal("Creating new ban entry failed");

	/* Check whether it matches a botserv bot after adding internally
	 * and parsing it through cidr support. ~ Viper */
	if (s_BotServ && BSSmartJoin && chan->ci && chan->ci->bi
		&& chan->usercount >= BSMinUsers)
	{
		BotInfo *bi = chan->ci->bi;

		if (entry_match(ban, bi->nick, bi->user, bi->host, 0))
		{
			ircdproto->SendMode(bi, chan->name, "-b %s", mask);
			entry_delete(chan->bans, ban);
			return;
		}
	}

	if (debug)
		alog("debug: Added ban %s to channel %s", mask, chan->name);
}

/** Remove a ban from the channel
 * @param chan The channel
 * @param mask The ban
 */
void ChannelModeBan::DelMask(Channel *chan, const char *mask)
{
	AutoKick *akick;
	Entry *ban;

	/* Sanity check as it seems some IRCD will just send -b without a mask */
	if (!mask || !chan->bans || (chan->bans->count == 0))
		return;

	ban = elist_find_mask(chan->bans, mask);

	if (ban)
	{
		entry_delete(chan->bans, ban);

		if (debug)
			alog("debug: Deleted ban %s from channel %s", mask,
				 chan->name);
	}

	if (chan->ci && (akick = is_stuck(chan->ci, mask)))
		stick_mask(chan->ci, akick);

}

/** Add an except to the channel
 * @param chan The channel
 * @param mask The except
 */
void ChannelModeExcept::AddMask(Channel *chan, const char *mask)
{
	Entry *exception;

	if (!chan || !mask)
	{
		if (debug)
			alog("debug: add_exception called with NULL values");
		return;
	}

	/* Check if the list already exists, if not create it.
	 * Create a new exception and add it to the list.. ~ Viper */
	if (!chan->excepts)
		chan->excepts = list_create();
	exception = entry_add(chan->excepts, mask);

	if (!exception)
		fatal("Creating new exception entry failed");

	if (debug)
		alog("debug: Added except %s to channel %s", mask, chan->name);
}

/** Remove an except from the channel
 * @param chan The channel
 * @param mask The except
 */
void ChannelModeExcept::DelMask(Channel *chan, const char *mask)
{
	Entry *exception;

	/* Sanity check as it seems some IRCD will just send -e without a mask */
	if (!mask || !chan->excepts || (chan->excepts->count == 0))
		return;

	exception = elist_find_mask(chan->excepts, mask);

	if (exception)
	{
		entry_delete(chan->excepts, exception);

		if (debug)
			alog("debug: Deleted except %s to channel %s", mask, chan->name);
	}
}

/** Add an invex to the channel
 * @param chan The channel
 * @param mask The invex
 */
void ChannelModeInvite::AddMask(Channel *chan, const char *mask)
{
	Entry *invite;

	if (!chan || !mask)
	{
		if (debug)
			alog("debug: add_invite called with NULL values");
		return;
	}

	/* Check if the list already exists, if not create it.
	 * Create a new invite and add it to the list.. ~ Viper */
	if (!chan->invites)
		chan->invites = list_create();
	invite = entry_add(chan->invites, mask);

	if (!invite)
		fatal("Creating new exception entry failed");

	if (debug)
		alog("debug: Added invite %s to channel %s", mask, chan->name);

}

/** Remove an invex from the channel
 * @param chan The channel
 * @param mask The index
 */
void ChannelModeInvite::DelMask(Channel *chan, const char *mask)
{
	Entry *invite;

	/* Sanity check as it seems some IRCD will just send -I without a mask */
	if (!mask || !chan->invites || (chan->invites->count == 0))
		return;

	invite = elist_find_mask(chan->invites, mask);

	if (invite)
	{
		entry_delete(chan->invites, invite);

		if (debug)
			alog("debug: Deleted invite %s to channel %s", mask,
				 chan->name);
	}
}
