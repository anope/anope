/* Mode support
 *
 * Copyright (C) 2008-2011 Adam <Adam@anope.org>
 * Copyright (C) 2008-2013 Anope Team <team@anope.org>
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
std::list<std::pair<Anope::string, Anope::string> > ModeManager::ModeLockOn;
std::list<Anope::string> ModeManager::ModeLockOff;

/* Default modes bots have on channels */
ChannelStatus ModeManager::DefaultBotModes;

Anope::string ChannelStatus::BuildCharPrefixList() const
{
	Anope::string ret;

	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];

		if (this->modes.count(cm->name))
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

		if (this->modes.count(cm->name))
		{
			ChannelModeStatus *cms = anope_dynamic_static_cast<ChannelModeStatus *>(cm);
			ret += cms->Symbol;
		}
	}

	return ret;
}

Mode::Mode(const Anope::string &mname, ModeClass mcl, char mch, ModeType mt) : name(mname), mclass(mcl), mchar(mch), type(mt)
{
}

Mode::~Mode()
{
}

UserMode::UserMode(const Anope::string &un, char mch) : Mode(un, MC_USER, mch, MODE_REGULAR)
{
}

UserMode::~UserMode()
{
}

UserModeParam::UserModeParam(const Anope::string &un, char mch) : UserMode(un, mch)
{
	this->type = MODE_PARAM;
}

ChannelMode::ChannelMode(const Anope::string &cm, char mch) : Mode(cm, MC_CHANNEL, mch, MODE_REGULAR)
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

ChannelModeList::ChannelModeList(const Anope::string &cm, char mch) : ChannelMode(cm, mch)
{
	this->type = MODE_LIST;
}

ChannelModeList::~ChannelModeList()
{
}

ChannelModeParam::ChannelModeParam(const Anope::string &cm, char mch, bool ma) : ChannelMode(cm, mch), minus_no_arg(ma)
{
	this->type = MODE_PARAM;
}

ChannelModeParam::~ChannelModeParam()
{
}

ChannelModeStatus::ChannelModeStatus(const Anope::string &mname, char modeChar, char mSymbol, unsigned short mLevel) : ChannelMode(mname, modeChar), Symbol(mSymbol), Level(mLevel)
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
	if (u && u->HasMode("OPER"))
		return true;

	return false;
}

bool ChannelModeOper::CanSet(User *u) const
{
	if (u && u->HasMode("OPER"))
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
	
	if (um->name.empty())
	{
		um->name = stringify(++GenericUserModes);
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

	if (cm->name.empty())
	{
		cm->name = stringify(++GenericChannelModes);
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

ChannelMode *ModeManager::FindChannelModeByName(const Anope::string &name)
{
	for (unsigned i = 0; i < ModeManager::ChannelModes.size(); ++i)
	{
		ChannelMode *cm = ModeManager::ChannelModes[i];
		if (cm->name == name)
			return cm;
	}

	return NULL;
}

UserMode *ModeManager::FindUserModeByName(const Anope::string &name)
{
	for (unsigned i = 0; i < ModeManager::UserModes.size(); ++i)
	{
		UserMode *um = ModeManager::UserModes[i];
		if (um->name == name)
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
	ModeLockOn.clear();
	ModeLockOff.clear();

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
					for (std::list<std::pair<Anope::string, Anope::string> >::iterator it = ModeLockOn.begin(), it_end = ModeLockOn.end(); it != it_end; ++it)
						if (it->first == cm->name)
						{
							ModeLockOn.erase(it);
							break;
						}

					for (std::list<Anope::string>::iterator it = ModeLockOff.begin(), it_end = ModeLockOff.end(); it != it_end; ++it)
						if (*it == cm->name)
						{
							ModeLockOff.erase(it);
							break;
						}
				}

				if (adding)
					ModeLockOn.push_back(std::make_pair(cm->name, param));
				else
					ModeLockOff.push_back(cm->name);
			}
		}
	}

	/* Set Bot Modes */
	DefaultBotModes.modes.clear();
	for (unsigned i = 0; i < config->BotModes.length(); ++i)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByChar(config->BotModes[i]);

		if (cm && cm->type == MODE_STATUS)
			DefaultBotModes.modes.insert(cm->name);
	}
}

Entry::Entry(const Anope::string &m, const Anope::string &_host) : name(m)
{
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
			this->types.insert(ENTRYTYPE_NICK_WILD);
		else
			this->types.insert(ENTRYTYPE_NICK);
	}

	if (!_user.empty() && _user.find_first_not_of("*") != Anope::string::npos)
	{
		this->user = _user;
		if (_user.find_first_of("*?") != Anope::string::npos)
			this->types.insert(ENTRYTYPE_USER_WILD);
		else
			this->types.insert(ENTRYTYPE_USER);
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
					this->types.insert(ENTRYTYPE_CIDR);
					Log(LOG_DEBUG) << "Ban " << _realhost << " has cidr " << static_cast<unsigned int>(this->cidr_len);
				}
			}
			catch (const SocketException &) { }
			catch (const ConvertException &) { }
		}

		this->host = _realhost;

		if (!this->types.count(ENTRYTYPE_CIDR))
		{
			if (_realhost.find_first_of("*?") != Anope::string::npos)
				this->types.insert(ENTRYTYPE_HOST_WILD);
			else
				this->types.insert(ENTRYTYPE_HOST);
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
	
	if (this->types.count(ENTRYTYPE_CIDR))
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
	if (this->types.count(ENTRYTYPE_NICK) && !this->nick.equals_ci(u->nick))
		ret = false;
	if (this->types.count(ENTRYTYPE_USER) && !this->user.equals_ci(u->GetVIdent()) && (!full ||
		!this->user.equals_ci(u->GetIdent())))
		ret = false;
	if (this->types.count(ENTRYTYPE_HOST) && !this->host.equals_ci(u->GetDisplayedHost()) && (!full ||
		(!this->host.equals_ci(u->host) && !this->host.equals_ci(u->chost) && !this->host.equals_ci(u->vhost) &&
		!this->host.equals_ci(u->ip))))
		ret = false;
	if (this->types.count(ENTRYTYPE_NICK_WILD) && !Anope::Match(u->nick, this->nick))
		ret = false;
	if (this->types.count(ENTRYTYPE_USER_WILD) && !Anope::Match(u->GetVIdent(), this->user) && (!full ||
		!Anope::Match(u->GetIdent(), this->user)))
		ret = false;
	if (this->types.count(ENTRYTYPE_HOST_WILD) && !Anope::Match(u->GetDisplayedHost(), this->host) && (!full ||
		(!Anope::Match(u->host, this->host) && !Anope::Match(u->chost, this->host) &&
		!Anope::Match(u->vhost, this->host) && !Anope::Match(u->ip, this->host))))
		ret = false;
	
	ChannelMode *cm = ModeManager::FindChannelModeByName(this->name);
	if (cm != NULL && cm->type == MODE_LIST)
	{
		ChannelModeList *cml = anope_dynamic_static_cast<ChannelModeList *>(cm);
		if (cml->Matches(u, this))
			ret = true;
	}

	return ret;
}

