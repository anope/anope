/* Mode support
 *
 * Copyright (C) 2008-2010 Adam <Adam@anope.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"
#include "modules.h"

/* List of pairs of user/channels and their stacker info */
std::list<std::pair<Base *, StackerInfo *> > ModeManager::StackerObjects;

/* List of all modes Anope knows about */
std::map<Anope::string, Mode *> ModeManager::Modes;

/* User modes */
std::map<char, UserMode *> ModeManager::UserModesByChar;
std::map<UserModeName, UserMode *> ModeManager::UserModesByName;
/* Channel modes */
std::map<char, ChannelMode *> ModeManager::ChannelModesByChar;
std::map<ChannelModeName, ChannelMode *> ModeManager::ChannelModesByName;
/* Although there are two different maps for UserModes and ChannelModes
 * the pointers in each are the same. This is used to increase efficiency.
 */

/* Number of generic modes we support */
unsigned GenericChannelModes = 0, GenericUserModes = 0;
/* Default mlocked modes on */
Flags<ChannelModeName> DefMLockOn;
/* Default mlocked modes off */
Flags<ChannelModeName> DefMLockOff;
/* Map for default mlocked mode parameters */
std::map<ChannelModeName, Anope::string> DefMLockParams;

/** Parse the mode string from the config file and set the default mlocked modes
 */
void SetDefaultMLock(ServerConfig *config)
{
	DefMLockOn.ClearFlags();
	DefMLockOff.ClearFlags();
	DefMLockParams.clear();
	Flags<ChannelModeName> *ptr = NULL;

	Anope::string modes, param;
	spacesepstream sep(config->MLock);
	sep.GetToken(modes);

	for (unsigned i = 0, end_mode = modes.length(); i < end_mode; ++i)
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
				ptr->SetFlag(cm->Name);

				if (ptr == &DefMLockOn && cm->Type == MODE_PARAM)
				{
					if (sep.GetToken(param))
						DefMLockParams.insert(std::make_pair(cm->Name, param));
					else
					{
						Log() << "Warning: Got default mlock mode " << cm->ModeChar << " with no param?";
						ptr->UnsetFlag(cm->Name);
					}
				}
			}
		}
	}

	/* Set Bot Modes */
	config->BotModeList.clear();
	for (unsigned i = 0; i < config->BotModes.length(); ++i)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByChar(config->BotModes[i]);

		if (cm && cm->Type == MODE_STATUS && std::find(config->BotModeList.begin(), config->BotModeList.end(), cm) == config->BotModeList.end())
			config->BotModeList.push_back(debug_cast<ChannelModeStatus *>(cm));
	}
}

Anope::string ChannelStatus::BuildCharPrefixList() const
{
	Anope::string ret;

	for (std::map<char, ChannelMode *>::const_iterator it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
	{
		if (this->HasFlag(it->second->Name))
		{
			ret += it->second->ModeChar;
		}
	}

	return ret;
}

Anope::string ChannelStatus::BuildModePrefixList() const
{
	Anope::string ret;

	for (std::map<char, ChannelMode *>::const_iterator it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
	{
		if (this->HasFlag(it->second->Name))
		{
			ChannelModeStatus *cm = debug_cast<ChannelModeStatus *>(it->second);
			ret += cm->Symbol;
		}
	}

	return ret;
}

/** Default constructor
 * @param mClass The type of mode this is
 * @param mNameAsString The mode name as a string
 * @param modeChar The mode char
 * @param modeType The mode type
 */
Mode::Mode(ModeClass mClass, const Anope::string &mNameAsString, char modeChar, ModeType modeType) : Class(mClass), NameAsString(mNameAsString), ModeChar(modeChar), Type(modeType)
{
}

/** Default destructor
 */
Mode::~Mode()
{
}

/** Default constructor
 * @param mName The mode name
 * @param mNameAsString The mode name as a string
 * @param modeChar The mode char
 */
UserMode::UserMode(UserModeName mName, const Anope::string &mNameAsString, char modeChar) : Mode(MC_USER, mNameAsString, modeChar, MODE_REGULAR), Name(mName)
{
}

/** Default destructor
 */
UserMode::~UserMode()
{
}

/** Default constructor
 * @param mName The mode name
 * @param mNameAsString The mode name as a string
 * @param modeChar The mode char
 */
UserModeParam::UserModeParam(UserModeName mName, const Anope::string &mNameAsString, char modeChar) : UserMode(mName, mNameAsString, modeChar)
{
	this->Type = MODE_PARAM;
}

/** Default constrcutor
 * @param mName The mode name
 * @param mNameAsString The mode name as a string
 * @param modeChar The mode char
 */
ChannelMode::ChannelMode(ChannelModeName mName, const Anope::string &mNameAsString, char modeChar) : Mode(MC_CHANNEL, mNameAsString, modeChar, MODE_REGULAR), Name(mName)
{
}

/** Default destructor
 */
ChannelMode::~ChannelMode()
{
}

/** Default constructor
 * @param mName The mode name
 * @param mNameAsString The mode name as a string
 * @param modeChar The mode char
 */
ChannelModeList::ChannelModeList(ChannelModeName mName, const Anope::string &mNameAsString, char modeChar) : ChannelMode(mName, mNameAsString, modeChar)
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
 * @param mNameAsString The mode name as a string
 * @param modeChar The mode char
 * @param MinusArg true if the mode sends no arg when unsetting
 */
ChannelModeParam::ChannelModeParam(ChannelModeName mName, const Anope::string &mNameAsString, char modeChar, bool MinusArg) : ChannelMode(mName, mNameAsString, modeChar), MinusNoArg(MinusArg)
{
	this->Type = MODE_PARAM;
}

/** Default destructor
 */
ChannelModeParam::~ChannelModeParam()
{
}

/** Default constructor
 * @param mName The mode name
 * @param mNameAsString The mode name as a string
 * @param modeChar The mode char
 * @param mSymbol The symbol for the mode, eg @ % +
 */
ChannelModeStatus::ChannelModeStatus(ChannelModeName mName, const Anope::string &mNameAsString, char modeChar, char mSymbol) : ChannelMode(mName, mNameAsString, modeChar), Symbol(mSymbol)
{
	this->Type = MODE_STATUS;
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
bool ChannelModeKey::IsValid(const Anope::string &value) const
{
	if (!value.empty() && value.find(':') != Anope::string::npos && value.find(',') != Anope::string::npos)
		return true;

	return false;
}

/** Can admin only mode be set by the user
 * @param u The user - can be NULL if defcon is checking
 * @return true or false
 */
bool ChannelModeAdmin::CanSet(User *u) const
{
	if (u && is_oper(u))
		return true;

	return false;
}

/** Can oper only mode be set by the user
 * @param u The user - can be NULL if defcon is checking
 * @return true or false
 */
bool ChannelModeOper::CanSet(User *u) const
{
	if (u && is_oper(u))
		return true;

	return false;
}

/** Can the user set the registered mode?
 * No.
 * @return false
 */
bool ChannelModeRegistered::CanSet(User *u) const
{
	return false;
}

/** Add a ban to the channel
 * @param chan The channel
 * @param mask The ban
 */
void ChannelModeBan::AddMask(Channel *chan, const Anope::string &mask)
{
	/* check for NULL values otherwise we will segfault */
	if (!chan || mask.empty())
	{
		Log() << "add_ban called with NULL values";
		return;
	}

	/* Check if the list already exists, if not create it.
	 * Create a new ban and add it to the list.. ~ Viper */
	if (!chan->bans)
		chan->bans = list_create();

	Entry *ban = entry_add(chan->bans, mask);
	if (!ban)
		throw CoreException("Creating new ban entry failed");

	/* Check whether it matches a botserv bot after adding internally
	 * and parsing it through cidr support. ~ Viper */
	if (!Config->s_BotServ.empty() && Config->BSSmartJoin && chan->ci && chan->ci->bi && chan->FindUser(chan->ci->bi))
	{
		BotInfo *bi = chan->ci->bi;

		if (entry_match(ban, bi->nick, bi->GetIdent(), bi->host, 0))
		{
			ircdproto->SendMode(bi, chan, "-b %s", mask.c_str());
			entry_delete(chan->bans, ban);
			return;
		}
	}

	Log(LOG_DEBUG) << "Added ban " << mask << " to channel " << chan->name;
}

/** Remove a ban from the channel
 * @param chan The channel
 * @param mask The ban
 */
void ChannelModeBan::DelMask(Channel *chan, const Anope::string &mask)
{
	/* Sanity check as it seems some IRCD will just send -b without a mask */
	if (mask.empty() || !chan->bans || !chan->bans->count)
		return;

	Entry *ban = elist_find_mask(chan->bans, mask);
	if (ban)
	{
		entry_delete(chan->bans, ban);

		Log(LOG_DEBUG) << "Deleted ban " << mask << " from channel " << chan->name;
	}

	AutoKick *akick;
	if (chan->ci && (akick = is_stuck(chan->ci, mask)))
		stick_mask(chan->ci, akick);
}

/** Add an except to the channel
 * @param chan The channel
 * @param mask The except
 */
void ChannelModeExcept::AddMask(Channel *chan, const Anope::string &mask)
{
	if (!chan || mask.empty())
	{
		Log() << "add_exception called with NULL values";
		return;
	}

	/* Check if the list already exists, if not create it.
	 * Create a new exception and add it to the list.. ~ Viper */
	if (!chan->excepts)
		chan->excepts = list_create();

	Entry *exception = entry_add(chan->excepts, mask);
	if (!exception)
		throw CoreException("Creating new exception entry failed");

	Log(LOG_DEBUG) << "Added except " << mask << " to channel " << chan->name;
}

/** Remove an except from the channel
 * @param chan The channel
 * @param mask The except
 */
void ChannelModeExcept::DelMask(Channel *chan, const Anope::string &mask)
{
	/* Sanity check as it seems some IRCD will just send -e without a mask */
	if (mask.empty() || !chan->excepts || !chan->excepts->count)
		return;

	Entry *exception = elist_find_mask(chan->excepts, mask);
	if (exception)
	{
		entry_delete(chan->excepts, exception);
		Log(LOG_DEBUG) << "Deleted except " << mask << " to channel " << chan->name;
	}
}

/** Add an invex to the channel
 * @param chan The channel
 * @param mask The invex
 */
void ChannelModeInvex::AddMask(Channel *chan, const Anope::string &mask)
{
	if (!chan || mask.empty())
	{
		Log() << "add_invite called with NULL values";
		return;
	}

	/* Check if the list already exists, if not create it.
	 * Create a new invite and add it to the list.. ~ Viper */
	if (!chan->invites)
		chan->invites = list_create();

	Entry *invite = entry_add(chan->invites, mask);
	if (!invite)
		throw CoreException("Creating new invex entry failed");

	Log(LOG_DEBUG) << "Added invite " << mask << " to channel " << chan->name;

}

/** Remove an invex from the channel
 * @param chan The channel
 * @param mask The index
 */
void ChannelModeInvex::DelMask(Channel *chan, const Anope::string &mask)
{
	/* Sanity check as it seems some IRCD will just send -I without a mask */
	if (mask.empty() || !chan->invites || !chan->invites->count)
		return;

	Entry *invite = elist_find_mask(chan->invites, mask);
	if (invite)
	{
		entry_delete(chan->invites, invite);
		Log(LOG_DEBUG) << "Deleted invite " << mask << " to channel " << chan->name;
	}
}

void StackerInfo::AddMode(Base *Mode, bool Set, const Anope::string &Param)
{
	ChannelMode *cm = NULL;
	UserMode *um = NULL;
	bool IsParam = false;

	if (Type == ST_CHANNEL)
	{
		cm = debug_cast<ChannelMode *>(Mode);
		if (cm->Type == MODE_PARAM)
			IsParam = true;
	}
	else if (Type == ST_USER)
	{
		um = debug_cast<UserMode *>(Mode);
		if (um->Type == MODE_PARAM)
			IsParam = true;
	}
	std::list<std::pair<Base *, Anope::string> > *list, *otherlist;
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

	/* Loop through the list and find if this mode is already on here */
	std::list<std::pair<Base *, Anope::string > >::iterator it, it_end;
	for (it = list->begin(), it_end = list->end(); it != it_end; ++it)
	{
		/* The param must match too (can have multiple status or list modes), but
		 * if it is a param mode it can match no matter what the param is
		 */
		if (it->first == Mode && (Param.equals_cs(it->second) || IsParam))
		{
			list->erase(it);
			/* It can only be on this list once */
			break;
		}
	}
	/* If the mode is on the other list, remove it from there (eg, we dont want +o-o Adam Adam) */
	for (it = otherlist->begin(), it_end = otherlist->end(); it != it_end; ++it)
	{
		/* The param must match too (can have multiple status or list modes), but
		 * if it is a param mode it can match no matter what the param is
		 */
		if (it->first == Mode && (Param.equals_cs(it->second) || IsParam))
		{
			otherlist->erase(it);
			return;
			/* Note that we return here because this is like setting + and - on the same mode within the same
			 * cycle, no change is made. This causes no problems with something like - + and -, because after the
			 * second mode change the list is empty, and the third mode change starts fresh.
			 */
		}
	}

	/* Add this mode and its param to our list */
	list->push_back(std::make_pair(Mode, Param));
}

/** Get the stacker info for an item, if one doesnt exist it is created
 * @param Item The user/channel etc
 * @return The stacker info
 */
StackerInfo *ModeManager::GetInfo(Base *Item)
{
	for (std::list<std::pair<Base *, StackerInfo *> >::const_iterator it = StackerObjects.begin(), it_end = StackerObjects.end(); it != it_end; ++it)
	{
		const std::pair<Base *, StackerInfo *> &PItem = *it;
		if (PItem.first == Item)
			return PItem.second;
	}

	StackerInfo *s = new StackerInfo();
	StackerObjects.push_back(std::make_pair(Item, s));
	return s;
}

/** Build a list of mode strings to send to the IRCd from the mode stacker
 * @param info The stacker info for a channel or user
 * @return a list of strings
 */
std::list<Anope::string> ModeManager::BuildModeStrings(StackerInfo *info)
{
	std::list<Anope::string> ret;
	std::list<std::pair<Base *, Anope::string> >::iterator it, it_end;
	Anope::string buf = "+", parambuf;
	ChannelMode *cm = NULL;
	UserMode *um = NULL;
	unsigned NModes = 0;

	for (it = info->AddModes.begin(), it_end = info->AddModes.end(); it != it_end; ++it)
	{
		if (++NModes > ircd->maxmodes)
		{
			ret.push_back(buf + parambuf);
			buf = "+";
			parambuf.clear();
			NModes = 1;
		}

		if (info->Type == ST_CHANNEL)
		{
			cm = debug_cast<ChannelMode *>(it->first);
			buf += cm->ModeChar;
		}
		else if (info->Type == ST_USER)
		{
			um = debug_cast<UserMode *>(it->first);
			buf += um->ModeChar;
		}

		if (!it->second.empty())
			parambuf += " " + it->second;
	}

	if (buf[buf.length() - 1] == '+')
		buf.erase(buf.length() - 1);

	buf += "-";
	for (it = info->DelModes.begin(), it_end = info->DelModes.end(); it != it_end; ++it)
	{
		if (++NModes > ircd->maxmodes)
		{
			ret.push_back(buf + parambuf);
			buf = "-";
			parambuf.clear();
			NModes = 1;
		}

		if (info->Type == ST_CHANNEL)
		{
			cm = debug_cast<ChannelMode *>(it->first);
			buf += cm->ModeChar;
		}
		else if (info->Type == ST_USER)
		{
			um = debug_cast<UserMode *>(it->first);
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

/** Really add a mode to the stacker, internal use only
 * @param bi The client to set the modes from
 * @param Object The object, user/channel
 * @param Mode The mode
 * @param Set Adding or removing?
 * @param Param A param, if there is one
 * @param Type The type this is, user or channel
 */
void ModeManager::StackerAddInternal(BotInfo *bi, Base *Object, Base *Mode, bool Set, const Anope::string &Param, StackerType Type)
{
	StackerInfo *s = GetInfo(Object);
	s->Type = Type;
	s->AddMode(Mode, Set, Param);
	if (bi)
		s->bi = bi;
	else if (Type == ST_CHANNEL)
		s->bi = whosends(debug_cast<Channel *>(Object)->ci);
	else if (Type == ST_USER)
		s->bi = NULL;
}

/** Add a user mode to Anope
 * @param um A UserMode or UserMode derived class
 * @return true on success, false on error
 */
bool ModeManager::AddUserMode(UserMode *um)
{
	if (ModeManager::UserModesByChar.insert(std::make_pair(um->ModeChar, um)).second)
	{
		if (um->Name == UMODE_END)
		{
			um->Name = static_cast<UserModeName>(UMODE_END + ++GenericUserModes);
			Log() << "ModeManager: Added generic support for user mode " << um->ModeChar;
		}
		ModeManager::UserModesByName.insert(std::make_pair(um->Name, um));
		ModeManager::Modes.insert(std::make_pair(um->NameAsString, um));

		FOREACH_MOD(I_OnUserModeAdd, OnUserModeAdd(um));

		return true;
	}

	return false;
}

/** Add a channel mode to Anope
 * @param cm A ChannelMode or ChannelMode derived class
 * @return true on success, false on error
 */
bool ModeManager::AddChannelMode(ChannelMode *cm)
{
	if (ModeManager::ChannelModesByChar.insert(std::make_pair(cm->ModeChar, cm)).second)
	{
		if (cm->Name == CMODE_END)
		{
			cm->Name = static_cast<ChannelModeName>(CMODE_END + ++GenericChannelModes);
			Log() << "ModeManager: Added generic support for channel mode " << cm->ModeChar;
		}
		ModeManager::ChannelModesByName.insert(std::make_pair(cm->Name, cm));
		ModeManager::Modes.insert(std::make_pair(cm->NameAsString, cm));

		/* Apply this mode to the new default mlock if its used */
		SetDefaultMLock(Config);

		FOREACH_MOD(I_OnChannelModeAdd, OnChannelModeAdd(cm));

		return true;
	}

	return false;
}
/** Find a channel mode
 * @param Mode The mode
 * @return The mode class
 */
ChannelMode *ModeManager::FindChannelModeByChar(char Mode)
{
	std::map<char, ChannelMode *>::iterator it = ModeManager::ChannelModesByChar.find(Mode);

	if (it != ModeManager::ChannelModesByChar.end())
		return it->second;

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
		return it->second;

	return NULL;
}

/** Find a mode by name
 * @param name The mode name
 * @return The mode
 */
Mode *ModeManager::FindModeByName(const Anope::string &name)
{
	std::map<Anope::string, Mode *>::const_iterator it = ModeManager::Modes.find(name);

	if (it != ModeManager::Modes.end())
		return it->second;
	
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
		return it->second;

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
		return it->second;

	return NULL;
}

/** Gets the channel mode char for a symbol (eg + returns v)
 * @param Value The symbol
 * @return The char
 */
char ModeManager::GetStatusChar(char Value)
{
	std::map<char, ChannelMode *>::iterator it, it_end;
	ChannelMode *cm;
	ChannelModeStatus *cms;

	for (it = ModeManager::ChannelModesByChar.begin(), it_end = ModeManager::ChannelModesByChar.end(); it != it_end; ++it)
	{
		cm = it->second;
		if (cm->Type == MODE_STATUS)
		{
			cms = debug_cast<ChannelModeStatus *>(cm);

			if (Value == cms->Symbol)
				return it->first;
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
void ModeManager::StackerAdd(BotInfo *bi, Channel *c, ChannelMode *cm, bool Set, const Anope::string &Param)
{
	StackerAddInternal(bi, c, cm, Set, Param, ST_CHANNEL);
}

/** Add a mode to the stacker to be set on a user
 * @param bi The client to set the modes from
 * @param u The user
 * @param um The user mode
 * @param Set true for setting, false for removing
 * @param param The param, if there is one
 */
void ModeManager::StackerAdd(BotInfo *bi, User *u, UserMode *um, bool Set, const Anope::string &Param)
{
	StackerAddInternal(bi, u, um, Set, Param, ST_USER);
}

/** Process all of the modes in the stacker and send them to the IRCd to be set on channels/users
 */
void ModeManager::ProcessModes()
{
	if (!StackerObjects.empty())
	{
		for (std::list<std::pair<Base *, StackerInfo *> >::const_iterator it = StackerObjects.begin(), it_end = StackerObjects.end(); it != it_end; ++it)
		{
			StackerInfo *s = it->second;
			User *u = NULL;
			Channel *c = NULL;

			if (s->Type == ST_USER)
				u = debug_cast<User *>(it->first);
			else if (s->Type == ST_CHANNEL)
				c = debug_cast<Channel *>(it->first);
			else
				throw CoreException("ModeManager::ProcessModes got invalid Stacker Info type");

			std::list<Anope::string> ModeStrings = BuildModeStrings(s);
			for (std::list<Anope::string>::iterator lit = ModeStrings.begin(), lit_end = ModeStrings.end(); lit != lit_end; ++lit)
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
