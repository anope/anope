/* Channel-handling routines.
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
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
#include "sockets.h"

channel_map ChannelList;

Channel::Channel(const Anope::string &nname, time_t ts)
{
	if (nname.empty())
		throw CoreException("A channel without a name ?");

	this->name = nname;

	this->creation_time = ts;
	this->server_modetime = this->chanserv_modetime = 0;
	this->server_modecount = this->chanserv_modecount = this->bouncy_modes = this->topic_ts = this->topic_time = 0;

	this->ci = ChannelInfo::Find(this->name);
	if (this->ci)
		this->ci->c = this;

	if (Me && Me->IsSynced())
		Log(NULL, this, "create");

	FOREACH_MOD(I_OnChannelCreate, OnChannelCreate(this));
}

Channel::~Channel()
{
	FOREACH_MOD(I_OnChannelDelete, OnChannelDelete(this));

	ModeManager::StackerDel(this);

	if (Me && Me->IsSynced())
		Log(NULL, this, "destroy");

	if (this->ci)
		this->ci->c = NULL;

	ChannelList.erase(this->name);
}

void Channel::Reset()
{
	this->modes.clear();

	for (ChanUserList::const_iterator it = this->users.begin(), it_end = this->users.end(); it != it_end; ++it)
	{
		ChanUserContainer *uc = it->second;

		ChannelStatus f = uc->status;
		uc->status.Clear();

		if (BotInfo::Find(uc->user->nick))
			for (size_t i = 0; i < f.Modes().length(); ++i)
				this->SetMode(NULL, ModeManager::FindChannelModeByName(f.Modes()[i]), uc->user->GetUID(), false);
	}

	this->CheckModes();

	for (ChanUserList::const_iterator it = this->users.begin(), it_end = this->users.end(); it != it_end; ++it)
		this->SetCorrectModes(it->second->user, true, false);
	
	if (this->ci && Me && Me->IsSynced())
		this->ci->RestoreTopic();
}

void Channel::Sync()
{
	if (!this->HasMode("PERM") && (this->users.empty() || (this->users.size() == 1 && this->ci && this->ci->bi && *this->ci->bi == this->users.begin()->second->user)))
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

			if (cm->type == MODE_REGULAR)
			{
				if (!this->HasMode(cm->name) && ml->set)
					this->SetMode(NULL, cm);
				else if (this->HasMode(cm->name) && !ml->set)
					this->RemoveMode(NULL, cm);
			}
			else if (cm->type == MODE_PARAM)
			{
				/* If the channel doesnt have the mode, or it does and it isn't set correctly */
				if (ml->set)
				{
					Anope::string param;
					this->GetParam(cm->name, param);

					if (!this->HasMode(cm->name) || (!param.empty() && !ml->param.empty() && !param.equals_cs(ml->param)))
						this->SetMode(NULL, cm, ml->param);
				}
				else
				{
					if (this->HasMode(cm->name))
						this->RemoveMode(NULL, cm);
				}
	
			}
			else if (cm->type == MODE_LIST)
			{
				if (ml->set)
					this->SetMode(NULL, cm, ml->param);
				else
					this->RemoveMode(NULL, cm, ml->param);
			}
		}
}

ChanUserContainer* Channel::JoinUser(User *user)
{
	if (user->server && user->server->IsSynced())
		Log(user, this, "join");

	ChanUserContainer *cuc = new ChanUserContainer(user, this);
	user->chans[this] = cuc;
	this->users[user] = cuc;

	if (this->ci && this->ci->HasExt("PERSIST") && this->creation_time > this->ci->time_registered)
	{
		Log(LOG_DEBUG) << "Changing TS of " << this->name << " from " << this->creation_time << " to " << this->ci->time_registered;
		this->creation_time = this->ci->time_registered;
		IRCD->SendChannel(this);
		this->Reset();
	}

	return cuc;
}

void Channel::DeleteUser(User *user)
{
	if (user->server && user->server->IsSynced() && !user->Quitting())
		Log(user, this, "leave");

	FOREACH_MOD(I_OnLeaveChannel, OnLeaveChannel(user, this));

	ChanUserContainer *cu = user->FindChannel(this);
	if (!this->users.erase(user))
		Log(LOG_DEBUG) << "Channel::DeleteUser() tried to delete nonexistant user " << user->nick << " from channel " << this->name;

	if (!user->chans.erase(this))
		Log(LOG_DEBUG) << "Channel::DeleteUser() tried to delete nonexistant channel " << this->name << " from " << user->nick << "'s channel list";
	delete cu;

	/* Channel is persistent, it shouldn't be deleted and the service bot should stay */
	if (this->HasExt("PERSIST") || (this->ci && this->ci->HasExt("PERSIST")))
		return;

	/* Channel is syncing from a netburst, don't destroy it as more users are probably wanting to join immediatly
	 * We also don't part the bot here either, if necessary we will part it after the sync
	 */
	if (this->HasExt("SYNCING"))
		return;

	/* Additionally, do not delete this channel if ChanServ/a BotServ bot is inhabiting it */
	if (this->HasExt("INHABIT"))
		return;

	if (this->users.empty())
		delete this;
}

ChanUserContainer *Channel::FindUser(User *u) const
{
	ChanUserList::const_iterator it = this->users.find(u);
	if (it != this->users.end())
		return it->second;
	return NULL;
}

bool Channel::HasUserStatus(User *u, ChannelModeStatus *cms)
{
	/* Usually its more efficient to search the users channels than the channels users */
	ChanUserContainer *cc = u->FindChannel(this);
	if (cc)
	{
		if (cms)
			return cc->status.HasMode(cms->mchar);
		else
			return cc->status.Empty();
	}

	return false;
}

bool Channel::HasUserStatus(User *u, const Anope::string &mname)
{
	return HasUserStatus(u, anope_dynamic_static_cast<ChannelModeStatus *>(ModeManager::FindChannelModeByName(mname)));
}

size_t Channel::HasMode(const Anope::string &mname, const Anope::string &param)
{
	if (param.empty())
		return modes.count(mname);
	std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> its = this->GetModeList(mname);
	for (; its.first != its.second; ++its.first)
		if (its.first->second == param)
			return 1;
	return 0;
}

Anope::string Channel::GetModes(bool complete, bool plus)
{
	Anope::string res, params;

	for (std::multimap<Anope::string, Anope::string>::const_iterator it = this->modes.begin(), it_end = this->modes.end(); it != it_end; ++it)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(it->first);
		if (!cm || cm->type == MODE_LIST)
			continue;

		res += cm->mchar;

		if (complete && !it->second.empty())
		{
			ChannelModeParam *cmp = NULL;
			if (cm->type == MODE_PARAM)
				cmp = anope_dynamic_static_cast<ChannelModeParam *>(cm);

			if (plus || !cmp || !cmp->minus_no_arg)
				params += " " + it->second;
		}
	}

	return res + params;
}

const Channel::ModeList &Channel::GetModes() const
{
	return this->modes;
}

std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> Channel::GetModeList(const Anope::string &mname)
{
	Channel::ModeList::iterator it = this->modes.find(mname), it_end = it;
	if (it != this->modes.end())
		it_end = this->modes.upper_bound(mname);
	return std::make_pair(it, it_end);
}

void Channel::SetModeInternal(MessageSource &setter, ChannelMode *cm, const Anope::string &param, bool enforce_mlock)
{
	if (!cm)
		return;

	EventReturn MOD_RESULT;

	/* Setting v/h/o/a/q etc */
	if (cm->type == MODE_STATUS)
	{
		if (param.empty())
		{
			Log() << "Channel::SetModeInternal() mode " << cm->mchar << " with no parameter for channel " << this->name;
			return;
		}

		User *u = User::Find(param);

		if (!u)
		{
			Log() << "MODE " << this->name << " +" << cm->mchar << " for nonexistant user " << param;
			return;
		}

		Log(LOG_DEBUG) << "Setting +" << cm->mchar << " on " << this->name << " for " << u->nick;

		/* Set the status on the user */
		ChanUserContainer *cc = u->FindChannel(this);
		if (cc)
			cc->status.AddMode(cm->mchar);

		FOREACH_RESULT(I_OnChannelModeSet, OnChannelModeSet(this, setter, cm->name, param));

		/* Enforce secureops, etc */
		if (enforce_mlock && MOD_RESULT != EVENT_STOP)
			this->SetCorrectModes(u, false, false);
		return;
	}

	if (cm->type != MODE_LIST)
		this->modes.erase(cm->name);
	this->modes.insert(std::make_pair(cm->name, param));

	if (param.empty() && cm->type != MODE_REGULAR)
	{
		Log() << "Channel::SetModeInternal() mode " << cm->mchar << " for " << this->name << " with no paramater, but is a param mode";
		return;
	}

	if (cm->type == MODE_LIST)
	{
		ChannelModeList *cml = anope_dynamic_static_cast<ChannelModeList *>(cm);
		cml->OnAdd(this, param);
	}

	/* Channel mode +P or so was set, mark this channel as persistent */
	if (cm->name == "PERM")
	{
		this->Extend("PERSIST");
		if (this->ci)
			this->ci->ExtendMetadata("PERSIST");
	}

	FOREACH_RESULT(I_OnChannelModeSet, OnChannelModeSet(this, setter, cm->name, param));

	/* Check if we should enforce mlock */
	if (!enforce_mlock || MOD_RESULT == EVENT_STOP)
		return;

	this->CheckModes();
}

void Channel::RemoveModeInternal(MessageSource &setter, ChannelMode *cm, const Anope::string &param, bool enforce_mlock)
{
	if (!cm)
		return;

	EventReturn MOD_RESULT;

	/* Setting v/h/o/a/q etc */
	if (cm->type == MODE_STATUS)
	{
		if (param.empty())
		{
			Log() << "Channel::RemoveModeInternal() mode " << cm->mchar << " with no parameter for channel " << this->name;
			return;
		}

		BotInfo *bi = BotInfo::Find(param);
		User *u = bi ? bi : User::Find(param);

		if (!u)
		{
			Log() << "Channel::RemoveModeInternal() MODE " << this->name << "-" << cm->mchar << " for nonexistant user " << param;
			return;
		}

		Log(LOG_DEBUG) << "Setting -" << cm->mchar << " on " << this->name << " for " << u->nick;

		/* Remove the status on the user */
		ChanUserContainer *cc = u->FindChannel(this);
		if (cc)
			cc->status.DelMode(cm->mchar);

		FOREACH_RESULT(I_OnChannelModeUnset, OnChannelModeUnset(this, setter, cm->name, param));

		if (enforce_mlock && MOD_RESULT != EVENT_STOP)
		{
			/* Reset modes on bots if we're supposed to */
			if (this->ci && this->ci->bi && this->ci->bi == bi)
			{
				if (ModeManager::DefaultBotModes.HasMode(cm->mchar))
					this->SetMode(bi, cm, bi->GetUID());
			}

			this->SetCorrectModes(u, false, false);
		}
		return;
	}

	if (cm->type == MODE_LIST && !param.empty())
	{
		std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> its = this->GetModeList(cm->name);
		for (; its.first != its.second; ++its.first)
			if (Anope::Match(param, its.first->second))
			{
				this->modes.erase(its.first);
				break;
			}
	}
	else
		this->modes.erase(cm->name);
	
	if (cm->type == MODE_LIST)
	{
		ChannelModeList *cml = anope_dynamic_static_cast<ChannelModeList *>(cm);
		cml->OnDel(this, param);
	}

	if (cm->name == "PERM")
	{
		this->Shrink("PERSIST");

		if (this->ci)
			this->ci->Shrink("PERSIST");

		if (this->users.empty() && !this->HasExt("SYNCING") && !this->HasExt("INHABIT"))
		{
			delete this;
			return;
		}
	}

	FOREACH_RESULT(I_OnChannelModeUnset, OnChannelModeUnset(this, setter, cm->name, param));

	/* Check for mlock */
	if (!enforce_mlock || MOD_RESULT == EVENT_STOP)
		return;

	this->CheckModes();
}

void Channel::SetMode(BotInfo *bi, ChannelMode *cm, const Anope::string &param, bool enforce_mlock)
{
	if (!cm)
		return;
	/* Don't set modes already set */
	if (cm->type == MODE_REGULAR && HasMode(cm->name))
		return;
	else if (cm->type == MODE_PARAM)
	{
		ChannelModeParam *cmp = anope_dynamic_static_cast<ChannelModeParam *>(cm);
		if (!cmp->IsValid(param))
			return;

		Anope::string cparam;
		if (GetParam(cm->name, cparam) && cparam.equals_cs(param))
			return;
	}
	else if (cm->type == MODE_STATUS)
	{
		User *u = User::Find(param);
		if (!u || HasUserStatus(u, anope_dynamic_static_cast<ChannelModeStatus *>(cm)))
			return;
	}
	else if (cm->type == MODE_LIST)
	{
		ChannelModeList *cml = anope_dynamic_static_cast<ChannelModeList *>(cm);
		if (this->HasMode(cm->name, param) || !cml->IsValid(param))
			return;
	}

	if (Me->IsSynced())
	{
		if (this->chanserv_modetime != Anope::CurTime)
		{
			this->chanserv_modecount = 0;
			this->chanserv_modetime = Anope::CurTime;
		}

		this->chanserv_modecount++;
	}

	ModeManager::StackerAdd(bi, this, cm, true, param);
	MessageSource ms(bi);
	SetModeInternal(ms, cm, param, enforce_mlock);
}

void Channel::SetMode(BotInfo *bi, const Anope::string &mname, const Anope::string &param, bool enforce_mlock)
{
	SetMode(bi, ModeManager::FindChannelModeByName(mname), param, enforce_mlock);
}

void Channel::RemoveMode(BotInfo *bi, ChannelMode *cm, const Anope::string &param, bool enforce_mlock)
{
	if (!cm)
		return;
	/* Don't unset modes that arent set */
	if ((cm->type == MODE_REGULAR || cm->type == MODE_PARAM) && !HasMode(cm->name))
		return;
	/* Don't unset status that aren't set */
	else if (cm->type == MODE_STATUS)
	{
		User *u = User::Find(param);
		if (!u || !HasUserStatus(u, anope_dynamic_static_cast<ChannelModeStatus *>(cm)))
			return;
	}
	else if (cm->type == MODE_LIST)
	{
		if (!this->HasMode(cm->name, param))
			return;
	}

	/* Get the param to send, if we need it */
	Anope::string realparam = param;
	if (cm->type == MODE_PARAM)
	{
		realparam.clear();
		ChannelModeParam *cmp = anope_dynamic_static_cast<ChannelModeParam *>(cm);
		if (!cmp->minus_no_arg)
			this->GetParam(cmp->name, realparam);
	}

	if (Me->IsSynced())
	{
		if (this->chanserv_modetime != Anope::CurTime)
		{
			this->chanserv_modecount = 0;
			this->chanserv_modetime = Anope::CurTime;
		}

		this->chanserv_modecount++;
	}

	ModeManager::StackerAdd(bi, this, cm, false, realparam);
	MessageSource ms(bi);
	RemoveModeInternal(ms, cm, realparam, enforce_mlock);
}

void Channel::RemoveMode(BotInfo *bi, const Anope::string &mname, const Anope::string &param, bool enforce_mlock)
{
	RemoveMode(bi, ModeManager::FindChannelModeByName(mname), param, enforce_mlock);
}

bool Channel::GetParam(const Anope::string &mname, Anope::string &target) const
{
	std::multimap<Anope::string, Anope::string>::const_iterator it = this->modes.find(mname);

	target.clear();

	if (it != this->modes.end())
	{
		target = it->second;
		return true;
	}

	return false;
}

void Channel::SetModes(BotInfo *bi, bool enforce_mlock, const char *cmodes, ...)
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
			if (cm->type != MODE_REGULAR && sep.GetToken(sbuf))
			{
				if (cm->type == MODE_STATUS)
				{
					User *targ = User::Find(sbuf);
					if (targ != NULL)
						sbuf = targ->GetUID();
				}
				this->SetMode(bi, cm, sbuf, enforce_mlock);
			}
			else
				this->SetMode(bi, cm, "", enforce_mlock);
		}
		else if (!add)
		{
			if (cm->type != MODE_REGULAR && sep.GetToken(sbuf))
			{
				if (cm->type == MODE_STATUS)
				{
					User *targ = User::Find(sbuf);
					if (targ != NULL)
						sbuf = targ->GetUID();
				}
				this->RemoveMode(bi, cm, sbuf, enforce_mlock);
			}
			else
				this->RemoveMode(bi, cm, "", enforce_mlock);
		}
	}
}

void Channel::SetModesInternal(MessageSource &source, const Anope::string &mode, time_t ts, bool enforce_mlock)
{
	if (source.GetServer() && source.GetServer()->IsSynced())
	{
		if (Anope::CurTime != this->server_modetime)
		{
			this->server_modecount = 0;
			this->server_modetime = Anope::CurTime;
		}

		++this->server_modecount;
	}

	if (!ts)
		;
	else if (ts > this->creation_time)
	{
		Log(LOG_DEBUG) << "Dropping mode " << mode << " on " << this->name << ", " << ts << " > " << this->creation_time;
		return;
	}
	else if (ts < this->creation_time)
	{
		Log(LOG_DEBUG) << "Changing TS of " << this->name << " from " << this->creation_time << " to " << ts;
		this->creation_time = ts;
		this->Reset();
	}

	User *setter = source.GetUser();
	/* Removing channel modes *may* delete this channel */
	Reference<Channel> this_reference(this);

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
				modestring += cm->mchar;
		}

		if (cm->type == MODE_REGULAR)
		{
			if (add)
				this->SetModeInternal(source, cm, "", false);
			else
				this->RemoveModeInternal(source, cm, "", false);
			continue;
		}
		else if (cm->type == MODE_PARAM)
		{
			ChannelModeParam *cmp = anope_dynamic_static_cast<ChannelModeParam *>(cm);

			if (!add && cmp->minus_no_arg)
			{
				this->RemoveModeInternal(source, cm, "", false);
				continue;
			}
		}
		Anope::string token;
		if (sep_modes.GetToken(token))
		{
			User *u = NULL;
			if (cm->type == MODE_STATUS && (u = User::Find(token)))
				paramstring += " " + u->nick;
			else
				paramstring += " " + token;

			/* CheckModes below doesn't check secureops (+ the module event) */
			if (add)
				this->SetModeInternal(source, cm, token, enforce_mlock);
			else
				this->RemoveModeInternal(source, cm, token, enforce_mlock);
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
	
	if (enforce_mlock)
		this->CheckModes();
}

bool Channel::MatchesList(User *u, const Anope::string &mode)
{
	if (!this->HasMode(mode))
		return false;


	std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> m = this->GetModeList(mode);
	for (; m.first != m.second; ++m.first)
	{
		Entry e(mode, m.first->second);
		if (e.Matches(u))
			return true;
	}

	return false;
}

void Channel::KickInternal(MessageSource &source, const Anope::string &nick, const Anope::string &reason)
{
	User *sender = source.GetUser();
	User *target = User::Find(nick);
	if (!target)
	{
		Log() << "Channel::KickInternal got a nonexistent user " << nick << " on " << this->name << ": " << reason;
		return;
	}

	BotInfo *bi = NULL;
	if (target->server == Me)
		bi = BotInfo::Find(nick);

	if (sender)
		Log(sender, this, "kick") << "kicked " << target->nick << " (" << reason << ")";
	else
		Log(target, this, "kick") << "was kicked by " << source.GetSource() << " (" << reason << ")";

	Anope::string chname = this->name;

	if (target->FindChannel(this))
	{
		FOREACH_MOD(I_OnUserKicked, OnUserKicked(this, target, source, reason));
		if (bi)
			this->Extend("INHABIT");
		this->DeleteUser(target);
	}
	else
		Log() << "Channel::KickInternal got kick for user " << target->nick << " from " << source.GetSource() << " who isn't on channel " << this->name;

	/* Bots get rejoined */
	if (bi)
	{
		bi->Join(this, &ModeManager::DefaultBotModes);
		this->Shrink("INHABIT");
	}
}

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
	IRCD->SendKick(bi, this, u, "%s", buf);
	MessageSource ms(bi);
	this->KickInternal(ms, u->nick, buf);
	return true;
}

void Channel::ChangeTopicInternal(const Anope::string &user, const Anope::string &newtopic, time_t ts)
{
	User *u = User::Find(user);

	this->topic = newtopic;
	this->topic_setter = u ? u->nick : user;
	this->topic_ts = ts;
	this->topic_time = Anope::CurTime;

	Log(LOG_DEBUG) << "Topic of " << this->name << " changed by " << (u ? u->nick : user) << " to " << newtopic;

	FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(this, user, this->topic));

	if (this->ci)
		this->ci->CheckTopic();
}

void Channel::ChangeTopic(const Anope::string &user, const Anope::string &newtopic, time_t ts)
{
	User *u = User::Find(user);

	this->topic = newtopic;
	this->topic_setter = u ? u->nick : user;
	this->topic_ts = ts;

	IRCD->SendTopic(this->ci->WhoSends(), this);

	/* Now that the topic is set update the time set. This is *after* we set it so the protocol modules are able to tell the old last set time */
	this->topic_time = Anope::CurTime;

	FOREACH_MOD(I_OnTopicUpdated, OnTopicUpdated(this, user, this->topic));

	if (this->ci)
		this->ci->CheckTopic();
}

void Channel::Hold()
{
	/** A timer used to keep the BotServ bot/ChanServ in the channel
	 * after kicking the last user in a channel
	 */
	class ChanServTimer : public Timer
	{
	 private:
		Reference<Channel> c;

	 public:
	 	/** Constructor
		 * @param chan The channel
		 */
		ChanServTimer(Channel *chan) : Timer(Config->CSInhabit), c(chan)
		{
			if (!ChanServ || !c)
				return;
			c->Extend("INHABIT");
			if (!c->ci || !c->ci->bi)
				ChanServ->Join(c);
			else if (!c->FindUser(c->ci->bi))
				c->ci->bi->Join(c);
		}

		/** Called when the delay is up
		 * @param The current time
		 */
		void Tick(time_t) anope_override
		{
			if (!c)
				return;

			c->Shrink("INHABIT");

			if (!c->ci || !c->ci->bi)
			{
				if (ChanServ)
					ChanServ->Part(c);
			}
			else if (c->users.size() == 1 || c->users.size() < Config->BSMinUsers)
				c->ci->bi->Part(c);
		}
	};

	new ChanServTimer(this);
}

void Channel::SetCorrectModes(User *user, bool give_modes, bool check_noop)
{
	if (user == NULL)
		return;
	
	if (!this->ci)
		return;

	Log(LOG_DEBUG) << "Setting correct user modes for " << user->nick << " on " << this->name << " (" << (give_modes ? "" : "not ") << "giving modes)";

	AccessGroup u_access = ci->AccessFor(user);
	ChannelMode *registered = ModeManager::FindChannelModeByName("REGISTERED");

	/* Only give modes if autoop isn't set */
	give_modes &= (!user->Account() || user->Account()->HasExt("AUTOOP")) && (!check_noop || !ci->HasExt("NOAUTOOP"));
	/* If this channel has secureops, or the registered channel mode exists and the channel does not have +r set (aka the channel
	 * was created just now or while we were off), or the registered channel mode does not exist and channel is syncing (aka just
	 * created *to us*) and the user's server is synced (aka this isn't us doing our initial uplink - without this we would be deopping all
	 * users with no access on a non-secureops channel on startup), and the user's server isn't ulined, then set negative modes.
	 */
	bool take_modes = (ci->HasExt("SECUREOPS") || (registered && !this->HasMode("REGISTERED")) || (!registered && this->HasExt("SYNCING") && user->server->IsSynced())) && !user->server->IsULined();

	bool given = false;
	for (unsigned i = 0; i < ModeManager::GetStatusChannelModesByRank().size(); ++i)
	{
		ChannelModeStatus *cm = ModeManager::GetStatusChannelModesByRank()[i];
		bool has_priv = u_access.HasPriv("AUTO" + cm->name);

		/* If we have already given one mode, don't give more until it has a symbol */
		if (give_modes && has_priv && (!given || cm->symbol))
		{
			this->SetMode(NULL, cm, user->GetUID());
			/* Now if this contains a symbol don't give any more modes, to prevent setting +qaohv etc on users */
			give_modes = !cm->symbol;
			given = true;
		}
		else if (take_modes && !has_priv)
			this->RemoveMode(NULL, cm, user->GetUID());
	}

	// Check mlock
	for (ChannelInfo::ModeList::const_iterator it = ci->GetMLock().begin(), it_end = ci->GetMLock().end(); it != it_end; ++it)
	{
		const ModeLock *ml = it->second;
		ChannelMode *cm = ModeManager::FindChannelModeByName(ml->name);
		if (!cm || cm->type != MODE_STATUS)
			continue;

		if (Anope::Match(user->nick, ml->param) || Anope::Match(user->GetDisplayedMask(), ml->param))
		{
			if (ml->set != this->HasUserStatus(user, ml->name))
			{
				if (ml->set)
					this->SetMode(NULL, cm, user->GetUID(), false);
				else if (!ml->set)
					this->RemoveMode(NULL, cm, user->GetUID(), false);
			}
		}
	}
}

bool Channel::Unban(User *u, bool full)
{
	if (!this->HasMode("BAN"))
		return false;

	bool ret = false;

	std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> bans = this->GetModeList("BAN");
	for (; bans.first != bans.second;)
	{
		Entry ban("BAN", bans.first->second);
		++bans.first;
		if (ban.Matches(u, full))
		{
			this->RemoveMode(NULL, "BAN", ban.GetMask());
			ret = true;
		}
	}

	return ret;
}

Channel* Channel::Find(const Anope::string &name)
{
	channel_map::const_iterator it = ChannelList.find(name);

	if (it != ChannelList.end())
		return it->second;
	return NULL;
}

Channel *Channel::FindOrCreate(const Anope::string &name, bool &created, time_t ts)
{
	Channel* &chan = ChannelList[name];
	created = chan == NULL;
	if (!chan)
		chan = new Channel(name, ts);
	return chan;
}

