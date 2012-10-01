/* Mode support
 *
 * Copyright (C) 2008-2011 Adam <Adam@anope.org>
 * Copyright (C) 2008-2012 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"
#include "modules.h"
#include "extern.h"
#include "config.h"
#include "sockets.h"
#include "protocol.h"
#include "channels.h"

/* List of pairs of user/channels and their stacker info */
std::map<User *, StackerInfo *> ModeManager::UserStackerObjects;
std::map<Channel *, StackerInfo *> ModeManager::ChannelStackerObjects;

/* List of all modes Anope knows about */
std::vector<ChannelMode *> ModeManager::ChannelModes;
std::vector<UserMode *> ModeManager::UserModes;

/* Number of generic modes we support */
unsigned GenericChannelModes = 0, GenericUserModes = 0;
/* Default mlocked modes */
ChannelInfo::ModeList def_mode_locks;

/** Parse the mode string from the config file and set the default mlocked modes
 */
void SetDefaultMLock(ServerConfig *config)
{
	for (ChannelInfo::ModeList::iterator it = def_mode_locks.begin(), it_end = def_mode_locks.end(); it != it_end; ++it)
		delete it->second;
	def_mode_locks.clear();

	Anope::string modes;
	spacesepstream sep(config->MLock);
	sep.GetToken(modes);

	int adding = -1;
	for (unsigned i = 0, end_mode = modes.length(); i < end_mode; ++i)
	{
		if (modes[i] == '+')
			adding = 1;
		else if (modes[i] == '-')
			adding = 0;
		else if (adding != -1)
		{
			ChannelMode *cm = ModeManager::FindChannelModeByChar(modes[i]);

			if (cm && cm->Type != MODE_STATUS)
			{
				Anope::string param;
				if (adding == 1 && cm->Type != MODE_REGULAR && !sep.GetToken(param)) // MODE_LIST OR MODE_PARAM
				{
					Log() << "Warning: Got default mlock mode " << cm->ModeChar << " with no param?";
					continue;
				}

				if (cm->Type != MODE_LIST) // Only MODE_LIST can have duplicates
				{
					ChannelInfo::ModeList::iterator it = def_mode_locks.find(cm->Name);
					if (it != def_mode_locks.end())
					{
						delete it->second;
						def_mode_locks.erase(it);
					}
				}
				def_mode_locks.insert(std::make_pair(cm->Name, new ModeLock(NULL, adding == 1, cm->Name, param)));
			}
		}
	}

	/* Set Bot Modes */
	config->BotModeList.ClearFlags();
	for (unsigned i = 0; i < config->BotModes.length(); ++i)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByChar(config->BotModes[i]);

		if (cm && cm->Type == MODE_STATUS)
			config->BotModeList.SetFlag(cm->Name);
	}
}

ChannelStatus::ChannelStatus() : Flags<ChannelModeName, CMODE_END * 2>(ChannelModeNameStrings)
{
}

Anope::string ChannelStatus::BuildCharPrefixList() const
{
	Anope::string ret;

	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];

		if (this->HasFlag(cm->Name))
			ret += cm->ModeChar;
	}

	return ret;
}

Anope::string ChannelStatus::BuildModePrefixList() const
{
	Anope::string ret;

	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];

		if (this->HasFlag(cm->Name))
		{
			ChannelModeStatus *cms = anope_dynamic_static_cast<ChannelModeStatus *>(cm);
			ret += cms->Symbol;
		}
	}

	return ret;
}

/** Default constructor
 * @param mClass The type of mode this is
 * @param modeChar The mode char
 * @param modeType The mode type
 */
Mode::Mode(ModeClass mClass, char modeChar, ModeType modeType) : Class(mClass), ModeChar(modeChar), Type(modeType)
{
}

/** Default destructor
 */
Mode::~Mode()
{
}

/** Default constructor
 * @param mName The mode name
 * @param modeChar The mode char
 */
UserMode::UserMode(UserModeName mName, char modeChar) : Mode(MC_USER, modeChar, MODE_REGULAR), Name(mName)
{
}

/** Default destructor
 */
UserMode::~UserMode()
{
}

/** Returns the mode name as a string
 */
const Anope::string UserMode::NameAsString()
{
	if (this->Name > UMODE_END)
		return this->ModeChar;
	return UserModeNameStrings[this->Name];
}

/** Default constructor
 * @param mName The mode name
 * @param modeChar The mode char
 */
UserModeParam::UserModeParam(UserModeName mName, char modeChar) : UserMode(mName, modeChar)
{
	this->Type = MODE_PARAM;
}

/** Default constrcutor
 * @param mName The mode name
 * @param modeChar The mode char
 */
ChannelMode::ChannelMode(ChannelModeName mName, char modeChar) : Mode(MC_CHANNEL, modeChar, MODE_REGULAR), Name(mName)
{
}

/** Default destructor
 */
ChannelMode::~ChannelMode()
{
}

/** Can a user set this mode, used for mlock
 * NOTE: User CAN be NULL, this is for checking if it can be locked with defcon
 * @param u The user, or NULL
 */
bool ChannelMode::CanSet(User *u) const
{
	if (Config->NoMLock.find(this->ModeChar) != Anope::string::npos || Config->CSRequire.find(this->ModeChar) != Anope::string::npos)
		return false;
	return true;
}

/** Returns the mode name as a string
 */
const Anope::string ChannelMode::NameAsString()
{
	if (this->Name > CMODE_END)
		return this->ModeChar;
	return ChannelModeNameStrings[this->Name];
}

/** Default constructor
 * @param mName The mode name
 * @param modeChar The mode char
 */
ChannelModeList::ChannelModeList(ChannelModeName mName, char modeChar) : ChannelMode(mName, modeChar)
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
 * @param modeChar The mode char
 * @param MinusArg true if the mode sends no arg when unsetting
 */
ChannelModeParam::ChannelModeParam(ChannelModeName mName, char modeChar, bool MinusArg) : ChannelMode(mName, modeChar), MinusNoArg(MinusArg)
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
 * @param modeChar The mode char
 * @param mSymbol The symbol for the mode, eg @ % +
 * @param mLevel A level for the mode, which is usually determined by the PREFIX capab
 */
ChannelModeStatus::ChannelModeStatus(ChannelModeName mName, char modeChar, char mSymbol, unsigned short mLevel) : ChannelMode(mName, modeChar), Symbol(mSymbol), Level(mLevel)
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
	if (!value.empty() && value.find(':') == Anope::string::npos && value.find(',') == Anope::string::npos)
		return true;

	return false;
}

/** Can admin only mode be set by the user
 * @param u The user - can be NULL if defcon is checking
 * @return true or false
 */
bool ChannelModeAdmin::CanSet(User *u) const
{
	if (u && u->HasMode(UMODE_OPER))
		return true;

	return false;
}

/** Can oper only mode be set by the user
 * @param u The user - can be NULL if defcon is checking
 * @return true or false
 */
bool ChannelModeOper::CanSet(User *u) const
{
	if (u && u->HasMode(UMODE_OPER))
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

void StackerInfo::AddMode(Mode *mode, bool Set, const Anope::string &Param)
{
	bool IsParam = mode->Type == MODE_PARAM;

	std::list<std::pair<Mode *, Anope::string> > *list, *otherlist;
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
	std::list<std::pair<Mode *, Anope::string > >::iterator it, it_end;
	for (it = list->begin(), it_end = list->end(); it != it_end; ++it)
	{
		/* The param must match too (can have multiple status or list modes), but
		 * if it is a param mode it can match no matter what the param is
		 */
		if (it->first == mode && (Param.equals_cs(it->second) || IsParam))
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
		if (it->first == mode && (Param.equals_cs(it->second) || IsParam))
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
	list->push_back(std::make_pair(mode, Param));
}

static class ModePipe : public Pipe
{
 public:
	void OnNotify()
	{
		ModeManager::ProcessModes();
	}
} *modePipe;

/** Get the stacker info for an item, if one doesnt exist it is created
 * @param Item The user/channel etc
 * @return The stacker info
 */
template<typename List, typename Object>
static StackerInfo *GetInfo(List &l, Object *o)
{
	typename List::const_iterator it = l.find(o);
	if (it != l.end())
		return it->second;

	StackerInfo *s = new StackerInfo();
	l[o] = s;
	return s;
}

/** Build a list of mode strings to send to the IRCd from the mode stacker
 * @param info The stacker info for a channel or user
 * @return a list of strings
 */
std::list<Anope::string> ModeManager::BuildModeStrings(StackerInfo *info)
{
	std::list<Anope::string> ret;
	std::list<std::pair<Mode *, Anope::string> >::iterator it, it_end;
	Anope::string buf = "+", parambuf;
	unsigned NModes = 0;

	for (it = info->AddModes.begin(), it_end = info->AddModes.end(); it != it_end; ++it)
	{
		if (++NModes > ircdproto->MaxModes)
		{
			ret.push_back(buf + parambuf);
			buf = "+";
			parambuf.clear();
			NModes = 1;
		}

		buf += it->first->ModeChar;

		if (!it->second.empty())
			parambuf += " " + it->second;
	}

	if (buf[buf.length() - 1] == '+')
		buf.erase(buf.length() - 1);

	buf += "-";
	for (it = info->DelModes.begin(), it_end = info->DelModes.end(); it != it_end; ++it)
	{
		if (++NModes > ircdproto->MaxModes)
		{
			ret.push_back(buf + parambuf);
			buf = "-";
			parambuf.clear();
			NModes = 1;
		}

		buf += it->first->ModeChar;

		if (!it->second.empty())
			parambuf += " " + it->second;
	}

	if (buf[buf.length() - 1] == '-')
		buf.erase(buf.length() - 1);

	if (!buf.empty())
		ret.push_back(buf + parambuf);

	return ret;
}

/** Add a user mode to Anope
 * @param um A UserMode or UserMode derived class
 * @return true on success, false on error
 */
bool ModeManager::AddUserMode(UserMode *um)
{
	if (ModeManager::FindUserModeByChar(um->ModeChar) != NULL)
		return false;
	
	if (um->Name == UMODE_END)
	{
		um->Name = static_cast<UserModeName>(UMODE_END + ++GenericUserModes);
		Log() << "ModeManager: Added generic support for user mode " << um->ModeChar;
	}
	ModeManager::UserModes.push_back(um);

	FOREACH_MOD(I_OnUserModeAdd, OnUserModeAdd(um));

	return true;
}

/** Add a channel mode to Anope
 * @param cm A ChannelMode or ChannelMode derived class
 * @return true on success, false on error
 */
bool ModeManager::AddChannelMode(ChannelMode *cm)
{
	if (ModeManager::FindChannelModeByChar(cm->ModeChar) != NULL)
		return false;

	if (cm->Name == CMODE_END)
	{
		cm->Name = static_cast<ChannelModeName>(CMODE_END + ++GenericChannelModes);
		Log() << "ModeManager: Added generic support for channel mode " << cm->ModeChar;
	}
	ModeManager::ChannelModes.push_back(cm);

	/* Apply this mode to the new default mlock if its used */
	SetDefaultMLock(Config);

	FOREACH_MOD(I_OnChannelModeAdd, OnChannelModeAdd(cm));

	return true;
}
/** Find a channel mode
 * @param Mode The mode
 * @return The mode class
 */
ChannelMode *ModeManager::FindChannelModeByChar(char Mode)
{
	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];
		if (cm->ModeChar == Mode)
			return cm;
	}

	return NULL;
}

/** Find a user mode
 * @param Mode The mode
 * @return The mode class
 */
UserMode *ModeManager::FindUserModeByChar(char Mode)
{
	for (unsigned i = 0; i < ModeManager::UserModes.size(); ++i)
	{
		UserMode *um = ModeManager::UserModes[i];
		if (um->ModeChar == Mode)
			return um;
	}

	return NULL;
}

/** Find a channel mode
 * @param Mode The modename
 * @return The mode class
 */
ChannelMode *ModeManager::FindChannelModeByName(ChannelModeName Name)
{
	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];
		if (cm->Name == Name)
			return cm;
	}

	return NULL;
}

/** Find a user mode
 * @param Mode The modename
 * @return The mode class
 */
UserMode *ModeManager::FindUserModeByName(UserModeName Name)
{
	for (unsigned i = 0; i < ModeManager::UserModes.size(); ++i)
	{
		UserMode *um = ModeManager::UserModes[i];
		if (um->Name == Name)
			return um;
	}

	return NULL;
}

/** Find channel mode by string
 * @param name The mode name
 * @return The mode
 */
ChannelMode *ModeManager::FindChannelModeByString(const Anope::string &name)
{
	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];
		if (cm->NameAsString() == name || Anope::string(cm->ModeChar) == name)
			return cm;
	}

	return NULL;
}

/** Find user mode by string
 * @param name The mode name
 * @return The mode
 */
UserMode *ModeManager::FindUserModeByString(const Anope::string &name)
{
	for (size_t i = UMODE_BEGIN + 1; i != UMODE_END; ++i)
	{
		UserMode *um = ModeManager::FindUserModeByName(static_cast<UserModeName>(i));
		if (um != NULL && (um->NameAsString() == name || Anope::string(um->ModeChar) == name))
			return um;
	}

	return NULL;
}


/** Gets the channel mode char for a symbol (eg + returns v)
 * @param Value The symbol
 * @return The char
 */
char ModeManager::GetStatusChar(char Value)
{
	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];
		if (cm->Type == MODE_STATUS)
		{
			ChannelModeStatus *cms = anope_dynamic_static_cast<ChannelModeStatus *>(cm);

			if (Value == cms->Symbol)
				return cms->ModeChar;
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
void ModeManager::StackerAdd(const BotInfo *bi, Channel *c, ChannelMode *cm, bool Set, const Anope::string &Param)
{
	StackerInfo *s = GetInfo(ChannelStackerObjects, c);
	s->AddMode(cm, Set, Param);
	if (bi)
		s->bi = bi;
	else
		s->bi = c->ci->WhoSends();

	if (!modePipe)
		modePipe = new ModePipe();
	modePipe->Notify();
}

/** Add a mode to the stacker to be set on a user
 * @param bi The client to set the modes from
 * @param u The user
 * @param um The user mode
 * @param Set true for setting, false for removing
 * @param param The param, if there is one
 */
void ModeManager::StackerAdd(const BotInfo *bi, User *u, UserMode *um, bool Set, const Anope::string &Param)
{
	StackerInfo *s = GetInfo(UserStackerObjects, u);
	s->AddMode(um, Set, Param);
	if (bi)
		s->bi = bi;
	else
		s->bi = NULL;

	if (!modePipe)
		modePipe = new ModePipe();
	modePipe->Notify();
}

/** Process all of the modes in the stacker and send them to the IRCd to be set on channels/users
 */
void ModeManager::ProcessModes()
{
	if (!UserStackerObjects.empty())
	{
		for (std::map<User *, StackerInfo *>::const_iterator it = UserStackerObjects.begin(), it_end = UserStackerObjects.end(); it != it_end; ++it)
		{
			User *u = it->first;
			StackerInfo *s = it->second;

			std::list<Anope::string> ModeStrings = BuildModeStrings(s);
			for (std::list<Anope::string>::iterator lit = ModeStrings.begin(), lit_end = ModeStrings.end(); lit != lit_end; ++lit)
				ircdproto->SendMode(s->bi, u, lit->c_str());
			delete it->second;
		}
		UserStackerObjects.clear();
	}

	if (!ChannelStackerObjects.empty())
	{
		for (std::map<Channel *, StackerInfo *>::const_iterator it = ChannelStackerObjects.begin(), it_end = ChannelStackerObjects.end(); it != it_end; ++it)
		{
			Channel *c = it->first;
			StackerInfo *s = it->second;

			std::list<Anope::string> ModeStrings = BuildModeStrings(s);
			for (std::list<Anope::string>::iterator lit = ModeStrings.begin(), lit_end = ModeStrings.end(); lit != lit_end; ++lit)
				ircdproto->SendMode(s->bi, c, lit->c_str());
			delete it->second;
		}
		ChannelStackerObjects.clear();
	}
}

/** Delete a user or channel from the stacker
 */
void ModeManager::StackerDel(User *u)
{
	UserStackerObjects.erase(u);
}

void ModeManager::StackerDel(Channel *c)
{
	ChannelStackerObjects.erase(c);
}

