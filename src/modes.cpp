/* Mode support
 *
 * Copyright (C) 2008-2011 Adam <Adam@anope.org>
 * Copyright (C) 2008-2012 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 */

#include "services.h"
#include "modules.h"
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
unsigned ModeManager::GenericChannelModes = 0, ModeManager::GenericUserModes = 0;

/* Default channel mode lock */
ChannelInfo::ModeList ModeManager::DefaultModeLocks;

/* Default modes bots have on channels */
ChannelStatus ModeManager::DefaultBotModes;

static const Anope::string UserModeNameStrings[] = {
	"UMODE_BEGIN",

	"UMODE_SERV_ADMIN", "UMODE_BOT", "UMODE_CO_ADMIN", "UMODE_FILTER", "UMODE_HIDEOPER", "UMODE_NETADMIN",
	"UMODE_REGPRIV", "UMODE_PROTECTED", "UMODE_NOCTCP", "UMODE_WEBTV", "UMODE_WEBIRC", "UMODE_WHOIS", "UMODE_ADMIN", "UMODE_DEAF",
	"UMODE_GLOBOPS", "UMODE_HELPOP", "UMODE_INVIS", "UMODE_OPER", "UMODE_PRIV", "UMODE_GOD", "UMODE_REGISTERED",
	"UMODE_SNOMASK", "UMODE_VHOST", "UMODE_WALLOPS", "UMODE_CLOAK", "UMODE_SSL", "UMODE_SOFTCALLERID", "UMODE_CALLERID",
	"UMODE_COMMONCHANS", "UMODE_HIDDEN", "UMODE_STRIPCOLOR", "UMODE_INVISIBLE_OPER", "UMODE_RESTRICTED", "UMODE_HIDEIDLE",

	""
};
template<> const Anope::string* Flags<UserModeName>::flags_strings = UserModeNameStrings;

static const Anope::string ChannelModeNameStrings[] = {
	"CMODE_BEGIN",

	/* Channel modes */
	"CMODE_BLOCKCOLOR", "CMODE_FLOOD", "CMODE_INVITE", "CMODE_KEY", "CMODE_LIMIT", "CMODE_MODERATED", "CMODE_NOEXTERNAL",
	"CMODE_PRIVATE", "CMODE_REGISTERED", "CMODE_SECRET", "CMODE_TOPIC", "CMODE_AUDITORIUM", "CMODE_SSL", "CMODE_ADMINONLY",
	"CMODE_NOCTCP", "CMODE_FILTER", "CMODE_NOKNOCK", "CMODE_REDIRECT", "CMODE_REGMODERATED", "CMODE_NONICK", "CMODE_OPERONLY",
	"CMODE_NOKICK", "CMODE_REGISTEREDONLY", "CMODE_STRIPCOLOR", "CMODE_NONOTICE", "CMODE_NOINVITE", "CMODE_ALLINVITE",
	"CMODE_BLOCKCAPS", "CMODE_PERM", "CMODE_NICKFLOOD", "CMODE_JOINFLOOD", "CMODE_DELAYEDJOIN", "CMODE_NOREJOIN",
	"CMODE_BANDWIDTH",

	/* b/e/I */
	"CMODE_BAN", "CMODE_EXCEPT",
	"CMODE_INVITEOVERRIDE",

	/* v/h/o/a/q */
	"CMODE_VOICE", "CMODE_HALFOP", "CMODE_OP",
	"CMODE_PROTECT", "CMODE_OWNER",

	""
};
template<> const Anope::string* Flags<ChannelModeName>::flags_strings = ChannelModeNameStrings;

static const Anope::string EntryFlagString[] = { "ENTRYTYPE_NONE", "ENTRYTYPE_CIDR", "ENTRYTYPE_NICK_WILD", "ENTRYTYPE_NICK", "ENTRYTYPE_USER_WILD", "ENTRYTYPE_USER", "ENTRYTYPE_HOST_WILD", "ENTRYTYPE_HOST", "" };
template<> const Anope::string* Flags<EntryType>::flags_strings = EntryFlagString;

Anope::string ChannelStatus::BuildCharPrefixList() const
{
	Anope::string ret;

	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];

		if (this->HasFlag(cm->name))
			ret += cm->mchar;
	}

	return ret;
}

Anope::string ChannelStatus::BuildModePrefixList() const
{
	Anope::string ret;

	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];

		if (this->HasFlag(cm->name))
		{
			ChannelModeStatus *cms = anope_dynamic_static_cast<ChannelModeStatus *>(cm);
			ret += cms->Symbol;
		}
	}

	return ret;
}

Mode::Mode(ModeClass mcl, char mch, ModeType mt) : mclass(mcl), mchar(mch), type(mt)
{
}

Mode::~Mode()
{
}

UserMode::UserMode(UserModeName un, char mch) : Mode(MC_USER, mch, MODE_REGULAR), name(un)
{
}

UserMode::~UserMode()
{
}

const Anope::string UserMode::NameAsString()
{
	if (this->name >= UMODE_END)
		return this->mchar;
	return UserModeNameStrings[this->name];
}

UserModeParam::UserModeParam(UserModeName un, char mch) : UserMode(un, mch)
{
	this->type = MODE_PARAM;
}

ChannelMode::ChannelMode(ChannelModeName cm, char mch) : Mode(MC_CHANNEL, mch, MODE_REGULAR), name(cm)
{
}

ChannelMode::~ChannelMode()
{
}

bool ChannelMode::CanSet(User *u) const
{
	if (Config->NoMLock.find(this->mchar) != Anope::string::npos || Config->CSRequire.find(this->mchar) != Anope::string::npos)
		return false;
	return true;
}

const Anope::string ChannelMode::NameAsString()
{
	if (this->name >= CMODE_END)
		return this->mchar;
	return ChannelModeNameStrings[this->name];
}

ChannelModeList::ChannelModeList(ChannelModeName cm, char mch) : ChannelMode(cm, mch)
{
	this->type = MODE_LIST;
}

ChannelModeList::~ChannelModeList()
{
}

ChannelModeParam::ChannelModeParam(ChannelModeName cm, char mch, bool ma) : ChannelMode(cm, mch), minus_no_arg(ma)
{
	this->type = MODE_PARAM;
}

ChannelModeParam::~ChannelModeParam()
{
}

ChannelModeStatus::ChannelModeStatus(ChannelModeName mName, char modeChar, char mSymbol, unsigned short mLevel) : ChannelMode(mName, modeChar), Symbol(mSymbol), Level(mLevel)
{
	this->type = MODE_STATUS;
}

ChannelModeStatus::~ChannelModeStatus()
{
}

bool ChannelModeKey::IsValid(const Anope::string &value) const
{
	if (!value.empty() && value.find(':') == Anope::string::npos && value.find(',') == Anope::string::npos)
		return true;

	return false;
}

bool ChannelModeAdmin::CanSet(User *u) const
{
	if (u && u->HasMode(UMODE_OPER))
		return true;

	return false;
}

bool ChannelModeOper::CanSet(User *u) const
{
	if (u && u->HasMode(UMODE_OPER))
		return true;

	return false;
}

bool ChannelModeRegistered::CanSet(User *u) const
{
	return false;
}

void StackerInfo::AddMode(Mode *mode, bool set, const Anope::string &param)
{
	bool is_param = mode->type == MODE_PARAM;

	std::list<std::pair<Mode *, Anope::string> > *list, *otherlist;
	if (set)
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
		if (it->first == mode && (is_param || param.equals_cs(it->second)))
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
		if (it->first == mode && (is_param || param.equals_cs(it->second)))
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
	list->push_back(std::make_pair(mode, param));
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

std::list<Anope::string> ModeManager::BuildModeStrings(StackerInfo *info)
{
	std::list<Anope::string> ret;
	std::list<std::pair<Mode *, Anope::string> >::iterator it, it_end;
	Anope::string buf = "+", parambuf;
	unsigned NModes = 0;

	for (it = info->AddModes.begin(), it_end = info->AddModes.end(); it != it_end; ++it)
	{
		if (++NModes > IRCD->MaxModes)
		{
			ret.push_back(buf + parambuf);
			buf = "+";
			parambuf.clear();
			NModes = 1;
		}

		buf += it->first->mchar;

		if (!it->second.empty())
			parambuf += " " + it->second;
	}

	if (buf[buf.length() - 1] == '+')
		buf.erase(buf.length() - 1);

	buf += "-";
	for (it = info->DelModes.begin(), it_end = info->DelModes.end(); it != it_end; ++it)
	{
		if (++NModes > IRCD->MaxModes)
		{
			ret.push_back(buf + parambuf);
			buf = "-";
			parambuf.clear();
			NModes = 1;
		}

		buf += it->first->mchar;

		if (!it->second.empty())
			parambuf += " " + it->second;
	}

	if (buf[buf.length() - 1] == '-')
		buf.erase(buf.length() - 1);

	if (!buf.empty())
		ret.push_back(buf + parambuf);

	return ret;
}

bool ModeManager::AddUserMode(UserMode *um)
{
	if (ModeManager::FindUserModeByChar(um->mchar) != NULL)
		return false;
	
	if (um->name == UMODE_END)
	{
		um->name = static_cast<UserModeName>(UMODE_END + ++GenericUserModes);
		Log() << "ModeManager: Added generic support for user mode " << um->mchar;
	}
	ModeManager::UserModes.push_back(um);

	FOREACH_MOD(I_OnUserModeAdd, OnUserModeAdd(um));

	return true;
}

bool ModeManager::AddChannelMode(ChannelMode *cm)
{
	if (ModeManager::FindChannelModeByChar(cm->mchar) != NULL)
		return false;

	if (cm->name == CMODE_END)
	{
		cm->name = static_cast<ChannelModeName>(CMODE_END + ++GenericChannelModes);
		Log() << "ModeManager: Added generic support for channel mode " << cm->mchar;
	}
	ModeManager::ChannelModes.push_back(cm);

	/* Apply this mode to the new default mlock if its used */
	UpdateDefaultMLock(Config);

	FOREACH_MOD(I_OnChannelModeAdd, OnChannelModeAdd(cm));

	return true;
}

void ModeManager::RemoveUserMode(UserMode *um)
{
	for (unsigned i = 0; i < ModeManager::UserModes.size(); ++i)
	{
		UserMode *mode = ModeManager::UserModes[i];
		if (um == mode)
		{
			ModeManager::UserModes.erase(ModeManager::UserModes.begin() + i);
			break;
		}
	}

	StackerDel(um);
}

void ModeManager::RemoveChannelMode(ChannelMode *cm)
{
	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *mode = ModeManager::ChannelModes[i];
		if (cm == mode)
		{
			ModeManager::ChannelModes.erase(ModeManager::ChannelModes.begin() + i);
			break;
		}
	}

	StackerDel(cm);
}

ChannelMode *ModeManager::FindChannelModeByChar(char Mode)
{
	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];
		if (cm->mchar == Mode)
			return cm;
	}

	return NULL;
}

UserMode *ModeManager::FindUserModeByChar(char Mode)
{
	for (unsigned i = 0; i < ModeManager::UserModes.size(); ++i)
	{
		UserMode *um = ModeManager::UserModes[i];
		if (um->mchar == Mode)
			return um;
	}

	return NULL;
}

ChannelMode *ModeManager::FindChannelModeByName(ChannelModeName Name)
{
	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];
		if (cm->name == Name)
			return cm;
	}

	return NULL;
}

UserMode *ModeManager::FindUserModeByName(UserModeName Name)
{
	for (unsigned i = 0; i < ModeManager::UserModes.size(); ++i)
	{
		UserMode *um = ModeManager::UserModes[i];
		if (um->name == Name)
			return um;
	}

	return NULL;
}

ChannelMode *ModeManager::FindChannelModeByString(const Anope::string &name)
{
	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];
		if (cm->NameAsString() == name || Anope::string(cm->mchar) == name)
			return cm;
	}

	return NULL;
}

UserMode *ModeManager::FindUserModeByString(const Anope::string &name)
{
	for (size_t i = UMODE_BEGIN + 1; i != UMODE_END; ++i)
	{
		UserMode *um = ModeManager::FindUserModeByName(static_cast<UserModeName>(i));
		if (um != NULL && (um->NameAsString() == name || Anope::string(um->mchar) == name))
			return um;
	}

	return NULL;
}

char ModeManager::GetStatusChar(char Value)
{
	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];
		if (cm->type == MODE_STATUS)
		{
			ChannelModeStatus *cms = anope_dynamic_static_cast<ChannelModeStatus *>(cm);

			if (Value == cms->Symbol)
				return cms->mchar;
		}
	}

	return 0;
}

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
				IRCD->SendMode(s->bi, u, lit->c_str());
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
				IRCD->SendMode(s->bi, c, lit->c_str());
			delete it->second;
		}
		ChannelStackerObjects.clear();
	}
}

void ModeManager::StackerDel(User *u)
{
	UserStackerObjects.erase(u);
}

void ModeManager::StackerDel(Channel *c)
{
	ChannelStackerObjects.erase(c);
}

void ModeManager::StackerDel(Mode *m)
{
	for (std::map<User *, StackerInfo *>::const_iterator it = UserStackerObjects.begin(), it_end = UserStackerObjects.end(); it != it_end;)
	{
		StackerInfo *si = it->second;
		++it;

		for (std::list<std::pair<Mode *, Anope::string> >::iterator it2 = si->AddModes.begin(), it2_end = si->AddModes.end(); it2 != it2_end;)
		{
			if (it2->first == m)
				it2 = si->AddModes.erase(it2);
			else
				++it2;
		}

		for (std::list<std::pair<Mode *, Anope::string> >::iterator it2 = si->DelModes.begin(), it2_end = si->DelModes.end(); it2 != it2_end;)
		{
			if (it2->first == m)
				it2 = si->DelModes.erase(it2);
			else
				++it2;
		}
	}

	for (std::map<Channel *, StackerInfo *>::const_iterator it = ChannelStackerObjects.begin(), it_end = ChannelStackerObjects.end(); it != it_end;)
	{
		StackerInfo *si = it->second;
		++it;

		for (std::list<std::pair<Mode *, Anope::string> >::iterator it2 = si->AddModes.begin(), it2_end = si->AddModes.end(); it2 != it2_end;)
		{
			if (it2->first == m)
				it2 = si->AddModes.erase(it2);
			else
				++it2;
		}

		for (std::list<std::pair<Mode *, Anope::string> >::iterator it2 = si->DelModes.begin(), it2_end = si->DelModes.end(); it2 != it2_end;)
		{
			if (it2->first == m)
				it2 = si->DelModes.erase(it2);
			else
				++it2;
		}
	}
}

void ModeManager::UpdateDefaultMLock(ServerConfig *config)
{
	for (ChannelInfo::ModeList::iterator it = DefaultModeLocks.begin(), it_end = DefaultModeLocks.end(); it != it_end; ++it)
		delete it->second;
	DefaultModeLocks.clear();

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

			if (cm && cm->type != MODE_STATUS)
			{
				Anope::string param;
				if (adding == 1 && cm->type != MODE_REGULAR && !sep.GetToken(param)) // MODE_LIST OR MODE_PARAM
				{
					Log() << "Warning: Got default mlock mode " << cm->mchar << " with no param?";
					continue;
				}

				if (cm->type != MODE_LIST) // Only MODE_LIST can have duplicates
				{
					ChannelInfo::ModeList::iterator it = DefaultModeLocks.find(cm->name);
					if (it != DefaultModeLocks.end())
					{
						delete it->second;
						DefaultModeLocks.erase(it);
					}
				}
				DefaultModeLocks.insert(std::make_pair(cm->name, new ModeLock(NULL, adding == 1, cm->name, param)));
			}
		}
	}

	/* Set Bot Modes */
	DefaultBotModes.ClearFlags();
	for (unsigned i = 0; i < config->BotModes.length(); ++i)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByChar(config->BotModes[i]);

		if (cm && cm->type == MODE_STATUS)
			DefaultBotModes.SetFlag(cm->name);
	}
}

Entry::Entry(ChannelModeName mode, const Anope::string &_host) : modename(mode)
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
	
	if (!_nick.empty() && _nick.find_first_not_of("*") != Anope::string::npos)
	{
		this->nick = _nick;
		if (_nick.find_first_of("*?") != Anope::string::npos)
			this->SetFlag(ENTRYTYPE_NICK_WILD);
		else
			this->SetFlag(ENTRYTYPE_NICK);
	}

	if (!_user.empty() && _user.find_first_not_of("*") != Anope::string::npos)
	{
		this->user = _user;
		if (_user.find_first_of("*?") != Anope::string::npos)
			this->SetFlag(ENTRYTYPE_USER_WILD);
		else
			this->SetFlag(ENTRYTYPE_USER);
	}

	if (!_realhost.empty() && _realhost.find_first_not_of("*") != Anope::string::npos)
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
			if (_realhost.find_first_of("*?") != Anope::string::npos)
				this->SetFlag(ENTRYTYPE_HOST_WILD);
			else
				this->SetFlag(ENTRYTYPE_HOST);
		}
	}
}

const Anope::string Entry::GetMask()
{
	return this->mask;
}

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
	if (cm != NULL && cm->type == MODE_LIST)
	{
		ChannelModeList *cml = anope_dynamic_static_cast<ChannelModeList *>(cm);
		if (cml->Matches(u, this))
			ret = true;
	}

	return ret;
}

