/* Mode support
 *
 * Copyright (C) 2008-2010 Adam <Adam@anope.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * $Id$
 *
 */

#include "services.h"
#include "modules.h"

/* List of pairs of user/channels and their stacker info */
std::list<std::pair<void *, StackerInfo *> > ModeManager::StackerObjects;

std::map<char, UserMode *> ModeManager::UserModesByChar;
std::map<UserModeName, UserMode *> ModeManager::UserModesByName;
/* Channel modes */
std::map<char, ChannelMode *> ModeManager::ChannelModesByChar;
std::map<ChannelModeName, ChannelMode *> ModeManager::ChannelModesByName;
/* Although there are two different maps for UserModes and ChannelModes
 * the pointers in each are the same. This is used to increase
 * efficiency.
 */

/* Default mlocked modes on */
std::bitset<128> DefMLockOn;
/* Default mlocked modes off */
std::bitset<128> DefMLockOff;
/* Map for default mlocked mode parameters */
std::map<ChannelModeName, std::string> DefMLockParams;

/** Parse the mode string from the config file and set the default mlocked modes
 */
void SetDefaultMLock()
{
	DefMLockOn.reset();
	DefMLockOff.reset();
	DefMLockParams.clear();
	std::bitset<128> *ptr = NULL;

	std::string modes, param;
	spacesepstream sep(Config.MLock);
	sep.GetToken(modes);

	for (unsigned i = 0; i < modes.size(); ++i)
	{
		 if (modes[i] == '+')
			  ptr = &DefMLockOn;
		 else if (modes[i] == '-')
			  ptr = &DefMLockOff;
		 else
		 {
		 	if (!ptr)
				continue;

			ChannelMode *cm = ModeManager::FindChannelModeByChar(modes[i]);

			if (cm && (cm->Type == MODE_REGULAR || cm->Type == MODE_PARAM))
			{
				ptr->set(cm->Name);

				if (*ptr == DefMLockOn && cm->Type == MODE_PARAM)
				{
					if (sep.GetToken(param))
					{
						DefMLockParams.insert(std::make_pair(cm->Name, param));
					}
					else
					{
						alog("Warning: Got default mlock mode %c with no param?", cm->ModeChar);
						ptr->set(cm->Name, false);
					}
				}
			}
		}
	}
}

/** Default constructor
 * @param mName The mode name
 */
UserMode::UserMode(UserModeName mName)
{
	this->Name = mName;
	this->Type = MODE_REGULAR;
}

/** Default destructor
 */
UserMode::~UserMode()
{
}

UserModeParam::UserModeParam(UserModeName mName) : UserMode(mName)
{
	this->Type = MODE_PARAM;
}

/** Default constrcutor
 * @param mName The mode name
 */
ChannelMode::ChannelMode(ChannelModeName mName)
{
	this->Name = mName;
	this->Type = MODE_REGULAR;
}

/** Default destructor
 */
ChannelMode::~ChannelMode()
{
}

/** Default constructor
 * @param mName The mode name
 */
ChannelModeList::ChannelModeList(ChannelModeName mName) : ChannelMode(mName)
{
	this->Type = MODE_LIST;
}

/** Default destructor
 */
ChannelModeList::~ChannelModeList()
{
}

/** Default constructor
 * @param mName The mode name
 * @param MinusArg true if the mode sends no arg when unsetting
 */
ChannelModeParam::ChannelModeParam(ChannelModeName mName, bool MinusArg) : ChannelMode(mName)
{
	this->Type = MODE_PARAM;
	this->MinusNoArg = MinusArg;
}

/** Default destructor
 */
ChannelModeParam::~ChannelModeParam()
{
}

/** Default constructor
 * @param mName The mode name
 * @param mStatus A CUS_ value
 * @param mSymbol The symbol for the mode, eg @ % +
 * @param mProtectBotServ Should botserv clients reset this on themself if it gets unset?
 */
ChannelModeStatus::ChannelModeStatus(ChannelModeName mName, int16 mStatus, char mSymbol, bool mProtectBotServ) : ChannelMode(mName)
{
	this->Type = MODE_STATUS;
	this->Status = mStatus;
	this->Symbol = mSymbol;
	this->ProtectBotServ = mProtectBotServ;
}

/** Default destructor
 */
ChannelModeStatus::~ChannelModeStatus()
{
}

/** Is the key valid
 * @param value The key
 * @return true or false
 */
bool ChannelModeKey::IsValid(const std::string &value)
{
	if (!value.empty() && value.find(':') != std::string::npos && value.find(',') != std::string::npos)
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
	if (Config.s_BotServ && Config.BSSmartJoin && chan->ci && chan->ci->bi
		&& chan->users.size() >= Config.BSMinUsers)
	{
		BotInfo *bi = chan->ci->bi;

		if (entry_match(ban, bi->nick.c_str(), bi->user.c_str(), bi->host.c_str(), 0))
		{
			ircdproto->SendMode(bi, chan, "-b %s", mask);
			entry_delete(chan->bans, ban);
			return;
		}
	}

	if (debug)
		alog("debug: Added ban %s to channel %s", mask, chan->name.c_str());
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
			alog("debug: Deleted ban %s from channel %s", mask, chan->name.c_str());
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
		alog("debug: Added except %s to channel %s", mask, chan->name.c_str());
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
			alog("debug: Deleted except %s to channel %s", mask, chan->name.c_str());
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
		alog("debug: Added invite %s to channel %s", mask, chan->name.c_str());

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
			alog("debug: Deleted invite %s to channel %s", mask, chan->name.c_str());
	}
}

void StackerInfo::AddMode(void *Mode, bool Set, const std::string &Param)
{
	ChannelMode *cm = NULL;
	UserMode *um = NULL;
	std::list<std::pair<void *, std::string> > *list, *otherlist;
	std::list<std::pair<void *, std::string > >::iterator it;

	if (Type == ST_CHANNEL)
		cm = static_cast<ChannelMode *>(Mode);
	else if (Type == ST_USER)
		um = static_cast<UserMode *>(Mode);
	if (Set)
	{
		list = &AddModes;
		otherlist = &DelModes;
	}
	else
	{
		list = &DelModes;
		otherlist = &AddModes;
	}

	/* Don't allow modes to be on the list twice, but only if they are
	 * not a list or status mode
	 */
	if (cm && (cm->Type != MODE_LIST && cm->Type != MODE_STATUS))
	{
		/* Remove this mode from our list if it already exists */
		for (it = list->begin(); it != list->end(); ++it)
		{
			if (it->first == Mode)
			{
				list->erase(it);
				/* It can't be on the list twice */
				break;
			}
		}

		/* Remove the mode from the other list */
		for (it = otherlist->begin(); it != otherlist->end(); ++it)
		{
			if (it->first == Mode)
			{
				otherlist->erase(it);
				/* It can't be on the list twice */
				break;
			}
		}
	}
	/* This is a list mode or a status mode, or a usermode */
	else
	{
		/* If exactly the same thing is being set on the other list, remove it from the other list */
		it = std::find(otherlist->begin(), otherlist->end(), std::make_pair(Mode, Param));
		if (it != otherlist->end())
		{
			otherlist->erase(it);
			/* If it is on the other list then just return, it would alreayd be set proprely */
			return;
		}
	}

	/* Add this mode and its param to our list */
	list->push_back(std::make_pair(Mode, Param));
}

/** Get the stacker info for an item, if one doesnt exist it is created
 * @param Item The user/channel etc
 * @return The stacker info
 */
StackerInfo *ModeManager::GetInfo(void *Item)
{
        for (std::list<std::pair<void *, StackerInfo *> >::const_iterator it = StackerObjects.begin(); it !=
StackerObjects.end(); ++it)
        {
                const std::pair<void *, StackerInfo *> &PItem = *it;
                if (PItem.first == Item)
                        return PItem.second;
        }

        StackerInfo *s = new StackerInfo;
        StackerObjects.push_back(std::make_pair(Item, s));
        return s;
}

/** Build a list of mode strings to send to the IRCd from the mode stacker
 * @param info The stacker info for a channel or user
 * @return a list of strings
 */
std::list<std::string> ModeManager::BuildModeStrings(StackerInfo *info)
{
	std::list<std::string> ret;
	std::list<std::pair<void *, std::string> >::iterator it;
	std::string buf, parambuf;
	ChannelMode *cm = NULL;
	UserMode *um = NULL;
	unsigned Modes = 0;

	buf = "+";
	for (it = info->AddModes.begin(); it != info->AddModes.end(); ++it)
	{
		if (++Modes > 12) //XXX this number needs to be recieved from the ircd
		{
			ret.push_back(buf + parambuf);
			buf = "+";
			parambuf.clear();
			Modes = 1;
		}

		if (info->Type == ST_CHANNEL)
		{
			cm = static_cast<ChannelMode *>(it->first);
			buf += cm->ModeChar;
		}
		else if (info->Type == ST_USER)
		{
			um = static_cast<UserMode *>(it->first);
			buf += um->ModeChar;
		}

		if (!it->second.empty())
			parambuf += " " + it->second;
	}

	if (buf[buf.length() - 1] == '+')
		buf.erase(buf.length() - 1);

	buf += "-";
	for (it = info->DelModes.begin(); it != info->DelModes.end(); ++it)
	{
		if (++Modes > 12) //XXX this number needs to be recieved from the ircd
		{
			ret.push_back(buf + parambuf);
			buf = "-";
			parambuf.clear();
			Modes = 1;
		}

		if (info->Type == ST_CHANNEL)
		{
			cm = static_cast<ChannelMode *>(it->first);
			buf += cm->ModeChar;
		}
		else if (info->Type == ST_USER)
		{
			um = static_cast<UserMode *>(it->first);
			buf += um->ModeChar;
		}

		if (!it->second.empty())
			parambuf += " " + it->second;
	}

	if (buf[buf.length() - 1] == '-')
		buf.erase(buf.length() - 1);

	if (!buf.empty())
		ret.push_back(buf + parambuf);
	
	return ret;
}

/** Add a mode to the stacker, internal use only
 * @param bi The client to set the modes from
 * @param u The user
 * @param um The user mode
 * @param Set Adding or removing?
 * @param Param A param, if there is one
 */
void ModeManager::StackerAddInternal(BotInfo *bi, User *u, UserMode *um, bool Set, const std::string &Param)
{
	StackerAddInternal(bi, u, um, Set, Param, ST_USER);
}

/** Add a mode to the stacker, internal use only
 * @param bi The client to set the modes from
 * @param c The channel
 * @param cm The channel mode
 * @param Set Adding or removing?
 * @param Param A param, if there is one
 */
void ModeManager::StackerAddInternal(BotInfo *bi, Channel *c, ChannelMode *cm, bool Set, const std::string &Param)
{
	StackerAddInternal(bi, c, cm, Set, Param, ST_CHANNEL);
}

/** Really add a mode to the stacker, internal use only
 * @param bi The client to set the modes from
 * @param Object The object, user/channel
 * @param Mode The mode
 * @param Set Adding or removing?
 * @param Param A param, if there is one
 * @param Type The type this is, user or channel
 */
void ModeManager::StackerAddInternal(BotInfo *bi, void *Object, void *Mode, bool Set, const std::string &Param, StackerType Type)
{
	StackerInfo *s = GetInfo(Object);
	s->Type = Type;
	s->AddMode(Mode, Set, Param);
	if (bi)
		s->bi = bi;
	else if (Type == ST_CHANNEL)
		s->bi = whosends(static_cast<Channel *>(Object)->ci);
	else if (Type == ST_USER)
		s->bi = NULL;
}

/** Add a user mode to Anope
 * @param Mode The mode
 * @param um A UserMode or UserMode derived class
 * @return true on success, false on error
 */
bool ModeManager::AddUserMode(char Mode, UserMode *um)
{
	um->ModeChar = Mode;
	bool ret = ModeManager::UserModesByChar.insert(std::make_pair(Mode, um)).second;
	if (ret)
		ret = ModeManager::UserModesByName.insert(std::make_pair(um->Name, um)).second;

	if (ret)
	{
		FOREACH_MOD(I_OnUserModeAdd, OnUserModeAdd(um));
	}

	return ret;
}

/** Add a channel mode to Anope
 * @param Mode The mode
 * @param cm A ChannelMode or ChannelMode derived class
 * @return true on success, false on error
 */
bool ModeManager::AddChannelMode(char Mode, ChannelMode *cm)
{
	cm->ModeChar = Mode;
	bool ret = ModeManager::ChannelModesByChar.insert(std::make_pair(Mode, cm)).second;
	if (ret)
		ret = ModeManager::ChannelModesByName.insert(std::make_pair(cm->Name, cm)).second;

	if (ret)
	{
		/* Apply this mode to the new default mlock if its used */
		SetDefaultMLock();

		FOREACH_MOD(I_OnChannelModeAdd, OnChannelModeAdd(cm));
	}

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
			cms = dynamic_cast<ChannelModeStatus *>(cm);

			if (Value == cms->Symbol)
			{
				return it->first;
			}
		}
	}

	return 0;
}

/** Add a mode to the stacker to be set on a channel
 * @param bi The client to set the modes from
 * @param c The channel
 * @param cm The channel mode
 * @param Set true for setting, false for removing
 * @param Param The param, if there is one
 */
void ModeManager::StackerAdd(BotInfo *bi, Channel *c, ChannelMode *cm, bool Set, const std::string &Param)
{
	StackerAddInternal(bi, c, cm, Set, Param);
}

/** Add a mode to the stacker to be set on a channel
 * @param bi The client to set the modes from
 * @param c The channel
 * @param Name The channel mode name
 * @param Set true for setting, false for removing
 * @param Param The param, if there is one
 */
void ModeManager::StackerAdd(BotInfo *bi, Channel *c, ChannelModeName Name, bool Set, const std::string &Param)
{
	StackerAdd(bi, c, FindChannelModeByName(Name), Set, Param);
}

/** Add a mode to the stacker to be set on a channel
 * @param bi The client to set the modes from
 * @param c The channel
 * @param Mode The mode char
 * @param Set true for setting, false for removing
 * @param Param The param, if there is one
 */
void ModeManager::StackerAdd(BotInfo *bi, Channel *c, const char Mode, bool Set, const std::string &Param)
{
	StackerAdd(bi, c, FindChannelModeByChar(Mode), Set, Param);
}

/** Add a mode to the stacker to be set on a user
 * @param bi The client to set the modes from
 * @param u The user
 * @param um The user mode
 * @param Set true for setting, false for removing
 * @param param The param, if there is one
 */
void ModeManager::StackerAdd(BotInfo *bi, User *u, UserMode *um, bool Set, const std::string &Param)
{
	StackerAddInternal(bi, u, um, Set, Param);
}

/** Add a mode to the stacker to be set on a user
 * @param bi The client to set the modes from
 * @param u The user
 * @param Name The user mode name
 * @param Set true for setting, false for removing
 * @param Param The param, if there is one
 */
void ModeManager::StackerAdd(BotInfo *bi, User *u, UserModeName Name, bool Set, const std::string &Param)
{
	StackerAdd(bi, u, FindUserModeByName(Name), Set, Param);
}

/** Add a mode to the stacker to be set on a user
 * @param bi The client to set the modes from
 * @param u The user
 * @param Mode The mode to be set
 * @param Set true for setting, false for removing
 * @param Param The param, if there is one
 */
void ModeManager::StackerAdd(BotInfo *bi, User *u, const char Mode, bool Set, const std::string &Param)
{
	StackerAdd(bi, u, FindUserModeByChar(Mode), Set, Param);
}

/** Process all of the modes in the stacker and send them to the IRCd to be set on channels/users
 */
void ModeManager::ProcessModes()
{
	if (!StackerObjects.empty())
	{
		for (std::list<std::pair<void *, StackerInfo *> >::const_iterator it = StackerObjects.begin(); it != StackerObjects.end(); ++it)
		{
			StackerInfo *s = it->second;
			User *u = NULL;
			Channel *c = NULL;
			std::list<std::string> ModeStrings = BuildModeStrings(s);

			if (s->Type == ST_USER)
				u = static_cast<User *>(it->first);
			else if (s->Type == ST_CHANNEL)
				c = static_cast<Channel *>(it->first);
			else
				throw CoreException("ModeManager::ProcessModes got invalid Stacker Info type");

			for (std::list<std::string>::iterator lit = ModeStrings.begin(); lit != ModeStrings.end(); ++lit)
			{
				if (c)
					ircdproto->SendMode(s->bi, c, lit->c_str());
				else if (u)
					ircdproto->SendMode(s->bi, u, lit->c_str());
			}
			delete it->second;
		}
		StackerObjects.clear();
	}
}

