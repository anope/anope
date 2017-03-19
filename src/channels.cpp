/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "services.h"
#include "channels.h"
#include "logger.h"
#include "modules.h"
#include "users.h"
#include "servers.h"
#include "protocol.h"
#include "users.h"
#include "config.h"
#include "sockets.h"
#include "language.h"
#include "uplink.h"
#include "event.h"
#include "modules/chanserv.h"

channel_map ChannelList;
std::vector<Channel *> Channel::deleting;

Channel::Channel(const Anope::string &nname, time_t ts)
	: logger(this)
{
	if (nname.empty())
		throw CoreException("A channel without a name ?");

	this->name = nname;
	this->creation_time = ts;

	if (Me && Me->IsSynced())
		logger.Category("create").Log("Channel {0} was created", this->GetName());

	EventManager::Get()->Dispatch(&Event::ChannelCreate::OnChannelCreate, this);
}

Channel::~Channel()
{
	EventManager::Get()->Dispatch(&Event::ChannelDelete::OnChannelDelete, this);

	ModeManager::StackerDel(this);

	if (Me && Me->IsSynced())
		logger.Category("destroy").Log("Channel {0} was destroyed", this->GetName());

	if (this->ci)
		this->ci->c = NULL;

	ChannelList.erase(this->name);
}

const Anope::string &Channel::GetName() const
{
	return name;
}

void Channel::Reset()
{
	this->modes.clear();

	for (ChanUserList::const_iterator it = this->users.begin(), it_end = this->users.end(); it != it_end; ++it)
	{
		ChanUserContainer *uc = it->second;

		ChannelStatus f = uc->status;
		uc->status.Clear();

		/* reset modes for my clients */
		if (uc->user->server == Me)
		{
			for (size_t i = 0; i < f.Modes().length(); ++i)
				this->SetMode(NULL, ModeManager::FindChannelModeByChar(f.Modes()[i]), uc->user->GetUID(), false);
			/* Modes might not exist yet, so be sure the status is really reset */
			uc->status = f;
		}
	}

	for (ChanUserList::const_iterator it = this->users.begin(), it_end = this->users.end(); it != it_end; ++it)
		this->SetCorrectModes(it->second->user, true);

	// If the channel is syncing now, do not force a sync due to Reset(), as we are probably iterating over users in Message::SJoin
	// A sync will come soon
	if (!syncing)
		this->Sync();
}

void Channel::Sync()
{
	syncing = false;
	EventManager::Get()->Dispatch(&Event::ChannelSync::OnChannelSync, this);
	CheckModes();
}

void Channel::CheckModes()
{
	if (this->bouncy_modes || this->syncing)
		return;

	/* Check for mode bouncing */
	if (this->chanserv_modetime == Anope::CurTime && this->server_modetime == Anope::CurTime && this->server_modecount >= 3 && this->chanserv_modecount >= 3)
	{
		Anope::Logger.Log("Warning: unable to set modes on channel {0}. Are your servers' U:lines configured correctly?", this->GetName());
		this->bouncy_modes = 1;
		return;
	}

	Reference<Channel> ref = this;
	EventManager::Get()->Dispatch(&Event::CheckModes::OnCheckModes, ref);
}

bool Channel::CheckDelete()
{
	/* Channel is syncing from a netburst, don't destroy it as more users are probably wanting to join immediately
	 * We also don't part the bot here either, if necessary we will part it after the sync
	 */
	if (this->syncing)
		return false;

	/* Permanent channels never get deleted */
	if (this->HasMode("PERM"))
		return false;

	EventReturn MOD_RESULT;
	MOD_RESULT = EventManager::Get()->Dispatch(&Event::CheckDelete::OnCheckDelete, this);

	return MOD_RESULT != EVENT_STOP && this->users.empty();
}

ChanUserContainer* Channel::JoinUser(User *user, const ChannelStatus *status)
{
	if (user->server && user->server->IsSynced())
		logger.Category("join").Log(_("{0} joined {1}"), user->GetMask(), this->GetName());

	ChanUserContainer *cuc = new ChanUserContainer(user, this);
	user->chans[this] = cuc;
	this->users[user] = cuc;
	if (status)
		cuc->status = *status;

	return cuc;
}

void Channel::DeleteUser(User *user)
{
	if (user->server && user->server->IsSynced() && !user->Quitting())
		logger.Category("leave").Log(_("{0} left {1}"), user->GetMask(), this->GetName());

	EventManager::Get()->Dispatch(&Event::LeaveChannel::OnLeaveChannel, user, this);

	ChanUserContainer *cu = user->FindChannel(this);
	if (!this->users.erase(user))
		logger.Debug("Channel::DeleteUser() tried to delete non-existent user {0} from channel {1}", user->GetMask(), this->GetName());

	if (!user->chans.erase(this))
		logger.Debug("Channel::DeleteUser() tried to delete non-existent channel {0} from channel list of user {1}", this->GetName(), user->GetMask());
	delete cu;

	QueueForDeletion();
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
	std::vector<Anope::string> v = this->GetModeList(mname);
	for (unsigned int i = 0; i < v.size(); ++i)
		if (v[i].equals_ci(param))
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

template<typename F, typename S>
struct second
{
	S operator()(const std::pair<F, S> &p)
	{
		return p.second;
	}
};

std::vector<Anope::string> Channel::GetModeList(const Anope::string &mname)
{
	std::vector<Anope::string> r;
	std::transform(modes.lower_bound(mname), modes.upper_bound(mname), std::back_inserter(r), second<Anope::string, Anope::string>());
	return r;
}

void Channel::SetModeInternal(const MessageSource &setter, ChannelMode *ocm, const Anope::string &oparam, bool enforce_mlock)
{
	if (!ocm)
		return;

	Anope::string param = oparam;
	ChannelMode *cm = ocm->Unwrap(param);

	EventReturn MOD_RESULT;

	/* Setting v/h/o/a/q etc */
	if (cm->type == MODE_STATUS)
	{
		if (param.empty())
		{
			Anope::Logger.Log("Channel::SetModeInternal() mode {} with no parameter for channel {}", cm->mchar, this->GetName());
			return;
		}

		User *u = User::Find(param);

		if (!u)
		{
			Anope::Logger.Debug("Mode +{0} for non-existent user {1} on channel {2}", cm->mchar, param, this->GetName());
			return;
		}

		Anope::Logger.Debug("Setting +{0} on {1} for {2}", cm->mchar, this->GetName(), u->GetMask());

		/* Set the status on the user */
		ChanUserContainer *cc = u->FindChannel(this);
		if (cc)
			cc->status.AddMode(cm->mchar);

		MOD_RESULT = EventManager::Get()->Dispatch(&Event::ChannelModeSet::OnChannelModeSet, this, setter, cm, param);

		/* Enforce secureops, etc */
		if (enforce_mlock && MOD_RESULT != EVENT_STOP)
			this->SetCorrectModes(u, false);
		return;
	}

	if (cm->type != MODE_LIST)
		this->modes.erase(cm->name);
	else if (this->HasMode(cm->name, param))
		return;

	this->modes.insert(std::make_pair(cm->name, param));

	if (param.empty() && cm->type != MODE_REGULAR)
	{
		Anope::Logger.Log("Channel::SetModeInternal() mode {0} for {1} with no paramater, but is a param mode", cm->mchar, this->name);
		return;
	}

	MOD_RESULT = EventManager::Get()->Dispatch(&Event::ChannelModeSet::OnChannelModeSet, this, setter, cm, param);

	/* Check if we should enforce mlock */
	if (!enforce_mlock || MOD_RESULT == EVENT_STOP)
		return;

	this->CheckModes();
}

void Channel::RemoveModeInternal(const MessageSource &setter, ChannelMode *ocm, const Anope::string &oparam, bool enforce_mlock)
{
	if (!ocm)
		return;

	Anope::string param = oparam;
	ChannelMode *cm = ocm->Unwrap(param);

	EventReturn MOD_RESULT;

	/* Setting v/h/o/a/q etc */
	if (cm->type == MODE_STATUS)
	{
		if (param.empty())
		{
			Anope::Logger.Log("Channel::RemoveModeInternal() mode {0} with no parameter for channel {1}", cm->mchar, this->GetName());
			return;
		}

		User *u = User::Find(param);

		if (!u)
		{
			Anope::Logger.Debug("Mode -{0} for non-existent user {1} on channel {2}", cm->mchar, param, this->GetName());
			return;
		}

		Anope::Logger.Debug("Setting -{0} on {1} for {2}", cm->mchar, this->GetName(), u->GetMask());

		/* Remove the status on the user */
		ChanUserContainer *cc = u->FindChannel(this);
		if (cc)
			cc->status.DelMode(cm->mchar);

		MOD_RESULT = EventManager::Get()->Dispatch(&Event::ChannelModeUnset::OnChannelModeUnset, this, setter, cm, param);

		if (enforce_mlock && MOD_RESULT != EVENT_STOP)
			this->SetCorrectModes(u, false);

		return;
	}

	if (cm->type == MODE_LIST)
	{
		for (Channel::ModeList::iterator it = modes.lower_bound(cm->name), it_end = modes.upper_bound(cm->name); it != it_end; ++it)
			if (param.equals_ci(it->second))
			{
				this->modes.erase(it);
				break;
			}
	}
	else
		this->modes.erase(cm->name);

	MOD_RESULT = EventManager::Get()->Dispatch(&Event::ChannelModeUnset::OnChannelModeUnset, this, setter, cm, param);

	if (cm->name == "PERM")
	{
		if (this->CheckDelete())
		{
			delete this;
			return;
		}
	}

	/* Check for mlock */
	if (!enforce_mlock || MOD_RESULT == EVENT_STOP)
		return;

	this->CheckModes();
}

void Channel::SetMode(User *bi, ChannelMode *cm, const Anope::string &param, bool enforce_mlock)
{
	Anope::string wparam = param;
	if (!cm)
		return;
	/* Don't set modes already set */
	if (cm->type == MODE_REGULAR && HasMode(cm->name))
		return;
	else if (cm->type == MODE_PARAM)
	{
		ChannelModeParam *cmp = anope_dynamic_static_cast<ChannelModeParam *>(cm);
		if (!cmp->IsValid(wparam))
			return;

		Anope::string cparam;
		if (GetParam(cm->name, cparam) && cparam.equals_cs(wparam))
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

		if (!cml->IsValid(wparam))
			return;

		if (this->HasMode(cm->name, wparam))
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

	ChannelMode *wcm = cm->Wrap(wparam);

	ModeManager::StackerAdd(bi, this, wcm, true, wparam);
	SetModeInternal(bi, wcm, wparam, enforce_mlock);
}

void Channel::SetMode(User *bi, const Anope::string &mname, const Anope::string &param, bool enforce_mlock)
{
	SetMode(bi, ModeManager::FindChannelModeByName(mname), param, enforce_mlock);
}

void Channel::RemoveMode(User *bi, ChannelMode *cm, const Anope::string &param, bool enforce_mlock)
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

	Anope::string wparam = realparam;
	ChannelMode *wcm = cm->Wrap(wparam);

	ModeManager::StackerAdd(bi, this, wcm, false, wparam);
	RemoveModeInternal(bi, wcm, wparam, enforce_mlock);
}

void Channel::RemoveMode(User *bi, const Anope::string &mname, const Anope::string &param, bool enforce_mlock)
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

void Channel::SetModes(User *bi, bool enforce_mlock, const char *cmodes, ...)
{
	char buf[BUFSIZE] = "";
	va_list args;
	Anope::string modebuf, sbuf;
	int add = -1;
	va_start(args, cmodes);
	vsnprintf(buf, BUFSIZE - 1, cmodes, args);
	va_end(args);

	Reference<Channel> this_reference(this);

	spacesepstream sep(buf);
	sep.GetToken(modebuf);
	for (unsigned i = 0, end = modebuf.length(); this_reference && i < end; ++i)
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
	if (!ts)
		;
	else if (ts > this->creation_time)
	{
		Anope::Logger.Debug("Dropping mode {0} on {1}, TS {2] > {3}", mode, this->GetName(), ts, this->creation_time);
		return;
	}
	else if (ts < this->creation_time)
	{
		Anope::Logger.Debug("Changing TS of {0} from {1} to {2}", this->GetName(), this->creation_time, ts);
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
	bool changed = false;
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
					Anope::Logger.Debug("Channel::SetModeInternal: Unknown mode char {0}", m[i]);
					continue;
				}
				modestring += cm->mchar;
		}

		if (cm->type == MODE_REGULAR)
		{
			/* something changed if we are adding a mode we don't have, or removing one we have */
			changed |= !!add != this->HasMode(cm->name);
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

			changed |= !!add != this->HasMode(cm->name, token);
			/* CheckModes below doesn't check secureops (+ the module event) */
			if (add)
				this->SetModeInternal(source, cm, token, enforce_mlock);
			else
				this->RemoveModeInternal(source, cm, token, enforce_mlock);
		}
		else
		{
			Anope::Logger.Log("warning: Channel::SetModesInternal() received more modes requiring params than params, modes: {0}", mode);
		}
	}

	if (!this_reference)
		return;

	if (changed && source.GetServer() && source.GetServer()->IsSynced())
	{
		if (Anope::CurTime != this->server_modetime)
		{
			this->server_modecount = 0;
			this->server_modetime = Anope::CurTime;
		}

		++this->server_modecount;
	}

	if (setter)
		logger.Category("mode").Log("{0} {1}", modestring, paramstring);
	else
		logger.Debug("{0} is setting {1] to {2}{3}", source.GetName(), this->GetName(), modestring, paramstring);

	if (enforce_mlock)
		this->CheckModes();
}

bool Channel::MatchesList(User *u, const Anope::string &mode)
{
	if (!this->HasMode(mode))
		return false;

	std::vector<Anope::string> v = this->GetModeList(mode);
	for (unsigned i = 0; i < v.size(); ++i)
	{
		Entry e(mode, v[i]);
		if (e.Matches(u))
			return true;
	}

	return false;
}

bool Channel::KickInternal(const MessageSource &source, const Anope::string &nick, const Anope::string &reason)
{
	User *sender = source.GetUser();
	User *target = User::Find(nick);

	if (!target)
	{
		Anope::Logger.Debug("Channel::KickInternal got a nonexistent user {0} on {1}: {2}", nick, this->GetName(), reason);
		return false;
	}

	if (sender)
		logger.User(sender).Category("kick").Log(_("{0} kicked {1} from {2} ({3})"), sender->GetMask(), target->GetMask(), this->GetName(), reason);
	else
		logger.Category("kick").Log(_("{0} kicked {1} from {2} ({3})"), source.GetName(), target->GetMask(), this->GetName(), reason);

	Anope::string chname = this->name;

	ChanUserContainer *cu = target->FindChannel(this);
	if (cu == NULL)
	{
		Anope::Logger.Debug("Kick for user {0} who is not in channel {1}", target->GetMask(), this->GetName());
		return false;
	}

	ChannelStatus status = cu->status;

	EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&Event::PreUserKicked::OnPreUserKicked, source, cu, reason);
	if ((sender && sender->server == Me) || source.GetServer() == Me)
		if (MOD_RESULT == EVENT_STOP)
			return false;

	this->DeleteUser(target);
	EventManager::Get()->Dispatch(&Event::UserKicked::OnUserKicked, source, target, this->name, status, reason);
	return true;
}

bool Channel::Kick(User *source, User *u, const Anope::string &reason)
{
	/* Do not kick protected clients or Ulines */
	if (u->IsProtected())
		return false;

	if (!this->KickInternal(source, u->nick, reason))
		return false;
	IRCD->SendKick(source, this, u, reason);
	return true;
}

void Channel::ChangeTopicInternal(User *u, const Anope::string &user, const Anope::string &newtopic, time_t ts)
{
	this->topic = newtopic;
	this->topic_setter = u ? u->nick : user;
	this->topic_ts = ts;
	this->topic_time = Anope::CurTime;

	Anope::Logger.Debug("Topic of {0} changed by {1} to {2}", this->GetName(), this->topic_setter, newtopic);

	EventManager::Get()->Dispatch(&Event::TopicUpdated::OnTopicUpdated, u, this, user, this->topic);
}

void Channel::ChangeTopic(const Anope::string &user, const Anope::string &newtopic, time_t ts)
{
	this->topic = newtopic;
	this->topic_setter = user;
	this->topic_ts = ts;

	IRCD->Send<messages::Topic>(this->ci ? this->ci->WhoSends() : Config->GetClient("ChanServ"), this, newtopic, ts, user);

	/* Now that the topic is set update the time set. This is *after* we set it so the protocol modules are able to tell the old last set time */
	this->topic_time = Anope::CurTime;

	EventManager::Get()->Dispatch(&Event::TopicUpdated::OnTopicUpdated, nullptr, this, user, this->topic);
}

void Channel::SetCorrectModes(User *user, bool give_modes)
{
	if (user == NULL)
		return;

	if (!this->ci)
		return;

	Anope::Logger.Debug("Setting correct user modes for {0} on {1} ({2}giving modes)", user->nick, this->name, give_modes ? "" : "not ");

	ChanServ::AccessGroup u_access = ci->AccessFor(user);

	/* Initially only take modes if the channel is being created by a non netmerge */
	bool take_modes = this->syncing && user->server->IsSynced();

	EventManager::Get()->Dispatch(&Event::SetCorrectModes::OnSetCorrectModes, user, this, u_access, give_modes, take_modes);

	/* Never take modes from ulines */
	if (user->server->IsULined())
		take_modes = false;

	/* whether or not we are giving modes */
	bool giving = give_modes;
	/* whether or not we have given a mode */
	bool given = false;
	for (unsigned i = 0; i < ModeManager::GetStatusChannelModesByRank().size(); ++i)
	{
		ChannelModeStatus *cm = ModeManager::GetStatusChannelModesByRank()[i];
		bool has_priv = u_access.HasPriv("AUTO" + cm->name);

		if (give_modes && has_priv)
		{
			/* Always give op. If we have already given one mode, don't give more until it has a symbol */
			if (cm->name == "OP" || !given || (giving && cm->symbol))
			{
				this->SetMode(NULL, cm, user->GetUID(), false);
				/* Now if this contains a symbol don't give any more modes, to prevent setting +qaohv etc on users */
				giving = !cm->symbol;
				given = true;
			}
		}
		/* modes that have no privileges assigned shouldn't be removed (like operprefix, ojoin) */
		else if (take_modes && !has_priv && ci->GetLevel(cm->name + "ME") != ChanServ::ACCESS_INVALID && !u_access.HasPriv(cm->name + "ME"))
		{
			/* Only remove modes if they are > voice */
			if (cm->name == "VOICE")
				take_modes = false;
			else
				this->RemoveMode(NULL, cm, user->GetUID(), false);
		}
	}
}

bool Channel::Unban(User *u, const Anope::string &mode, bool full)
{
	if (!this->HasMode(mode))
		return false;

	bool ret = false;

	std::vector<Anope::string> v = this->GetModeList(mode);
	for (unsigned int i = 0; i < v.size(); ++i)
	{
		Entry ban(mode, v[i]);
		if (ban.Matches(u, full))
		{
			this->RemoveMode(NULL, mode, ban.GetMask());
			ret = true;
		}
	}

	return ret;
}

bool Channel::CheckKick(User *user)
{
	if (user->super_admin)
		return false;

	/* We don't enforce services restrictions on clients on ulined services
	 * as this will likely lead to kick/rejoin floods. ~ Viper */
	if (user->IsProtected())
		return false;

	Anope::string mask, reason;

	EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&Event::CheckKick::OnCheckKick, user, this, mask, reason);
	if (MOD_RESULT != EVENT_STOP)
		return false;

	if (mask.empty())
		mask = this->ci->GetIdealBan(user);
	if (reason.empty())
		reason = Language::Translate(user->Account(), _("You are not permitted to be on this channel."));

	Anope::Logger.Debug("Autokicking {0} ({1}) from {2}", user->nick, mask, this->name);

	this->SetMode(NULL, "BAN", mask);
	this->Kick(NULL, user, reason);

	return true;
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

void Channel::QueueForDeletion()
{
	if (std::find(deleting.begin(), deleting.end(), this) == deleting.end())
		deleting.push_back(this);
}

void Channel::DeleteChannels()
{
	for (unsigned int i = 0; i < deleting.size(); ++i)
	{
		Channel *c = deleting[i];

		if (c->CheckDelete())
			delete c;
	}
	deleting.clear();
}

