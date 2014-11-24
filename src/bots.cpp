/*
 *
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2014 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 */

#include "services.h"
#include "anope.h"
#include "bots.h"
#include "servers.h"
#include "protocol.h"
#include "xline.h"
#include "channels.h"
#include "config.h"
#include "language.h"
#include "serialize.h"
#include "event.h"
#include "modules/chanserv.h"

ServiceBot::ServiceBot(const Anope::string &nnick, const Anope::string &nuser, const Anope::string &nhost, const Anope::string &nreal, const Anope::string &bmodes) : LocalUser(nnick, nuser, nhost, "", "", Me, nreal, Anope::CurTime, "", IRCD ? IRCD->UID_Retrieve() : "", NULL), botmodes(bmodes)
{
	this->type = UserType::BOT;
	this->lastmsg = Anope::CurTime;
	this->introduced = false;
	this->conf = false;

	Event::OnCreateBot(&Event::CreateBot::OnCreateBot, this);

	// If we're synchronised with the uplink already, send the bot.
	if (Me && Me->IsSynced())
	{
		Anope::string tmodes = !this->botmodes.empty() ? ("+" + this->botmodes) : IRCD->DefaultPseudoclientModes;
		if (!tmodes.empty())
			this->SetModesInternal(this, tmodes.c_str());

		//XXX
		//XLine x(this->nick, "Reserved for services");
		//IRCD->SendSQLine(NULL, &x);
		IRCD->SendClientIntroduction(this);
		this->introduced = true;
	}
}

ServiceBot::~ServiceBot()
{
	Event::OnDelBot(&Event::DelBot::OnDelBot, this);

	// If we're synchronised with the uplink already, send the bot.
	if (Me && Me->IsSynced())
	{
		IRCD->SendQuit(this, "");
		Event::OnUserQuit(&Event::UserQuit::OnUserQuit, this, "");
		this->introduced = false;
		// XXX ?
		//XLine x(this->nick);
		//IRCD->SendSQLineDel(&x);
	}
}


void ServiceBot::GenerateUID()
{
	if (this->introduced)
		throw CoreException("Changing bot UID when it is introduced?");

	if (!this->uid.empty())
		UserListByUID.erase(this->uid);
	this->uid = IRCD->UID_Retrieve();
	UserListByUID[this->uid] = this;
}

void ServiceBot::OnKill()
{
	this->introduced = false;
	this->GenerateUID();
	IRCD->SendClientIntroduction(this);
	this->introduced = true;

	for (User::ChanUserList::const_iterator cit = this->chans.begin(), cit_end = this->chans.end(); cit != cit_end; ++cit)
		IRCD->SendJoin(this, cit->second->chan, &cit->second->status);
}

void ServiceBot::SetNewNick(const Anope::string &newnick)
{
	UserListByNick.erase(this->nick);

	bi->SetNick(newnick);
	this->nick = newnick;

	UserListByNick[this->nick] = this;
}

std::vector<ChanServ::Channel *> ServiceBot::GetChannels() const
{
	return bi->GetRefs<ChanServ::Channel *>(ChanServ::channel);
}

void ServiceBot::Assign(User *u, ChanServ::Channel *ci)
{
	EventReturn MOD_RESULT;
	MOD_RESULT = Event::OnPreBotAssign(&Event::PreBotAssign::OnPreBotAssign, u, ci, this);
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (ci->GetBot())
		ci->GetBot()->UnAssign(u, ci);

	ci->SetBot(this);

	Event::OnBotAssign(&Event::BotAssign::OnBotAssign, u, ci, this);
}

void ServiceBot::UnAssign(User *u, ChanServ::Channel *ci)
{
	EventReturn MOD_RESULT;
	MOD_RESULT = Event::OnBotUnAssign(&Event::BotUnAssign::OnBotUnAssign, u, ci);
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (ci->c && ci->c->FindUser(ci->GetBot()))
	{
		if (u)
			ci->GetBot()->Part(ci->c, "UNASSIGN from " + u->nick);
		else
			ci->GetBot()->Part(ci->c);
	}

	ci->SetBot(nullptr);
}

unsigned ServiceBot::GetChannelCount() const
{
	return GetChannels().size();
}

void ServiceBot::Join(Channel *c, ChannelStatus *status)
{
	if (c->FindUser(this) != NULL)
		return;

	c->JoinUser(this, status);
	if (IRCD)
		IRCD->SendJoin(this, c, status);

	Event::OnJoinChannel(&Event::JoinChannel::OnJoinChannel, this, c);
}

void ServiceBot::Join(const Anope::string &chname, ChannelStatus *status)
{
	bool c;
	return this->Join(Channel::FindOrCreate(chname, c), status);
}

void ServiceBot::Part(Channel *c, const Anope::string &reason)
{
	if (c->FindUser(this) == NULL)
		return;

	Event::OnPrePartChannel(&Event::PrePartChannel::OnPrePartChannel, this, c);

	IRCD->SendPart(this, c, "%s", !reason.empty() ? reason.c_str() : "");

	Anope::string cname = c->name;
	Reference<Channel> cref = c;

	c->DeleteUser(this);

	Event::OnPartChannel(&Event::PartChannel::OnPartChannel, this, cref, cname, reason);
}

void ServiceBot::OnMessage(User *u, const Anope::string &message)
{
	if (this->commands.empty())
		return;

	CommandSource source(u->nick, u, u->Account(), u, this);
	Command::Run(source, message);
}

CommandInfo& ServiceBot::SetCommand(const Anope::string &cname, const Anope::string &sname, const Anope::string &permission)
{
	CommandInfo ci;
	ci.name = sname;
	ci.cname = cname;
	ci.permission = permission;
	this->commands[cname] = ci;
	return this->commands[cname];
}

CommandInfo *ServiceBot::GetCommand(const Anope::string &cname)
{
	CommandInfo::map::iterator it = this->commands.find(cname);
	if (it != this->commands.end())
		return &it->second;
	return NULL;
}

CommandInfo *ServiceBot::FindCommand(const Anope::string &service)
{
	for (auto& it : commands)
	{
		CommandInfo &ci =  it.second;

		if (ci.name == service)
			return &ci;
	}

	return nullptr;
}

ServiceBot* ServiceBot::Find(const Anope::string &nick, bool nick_only)
{
	User *u = User::Find(nick, nick_only);
	if (u && u->type == UserType::BOT)
		return anope_dynamic_static_cast<ServiceBot *>(u);
	return nullptr;
}

void BotInfo::SetCreated(const time_t &t)
{
	Set(&BotInfoType::created, t);
}

time_t BotInfo::GetCreated()
{
	return Get(&BotInfoType::created);
}

void BotInfo::SetOperOnly(const bool &b)
{
	Set(&BotInfoType::operonly, b);
}

bool BotInfo::GetOperOnly()
{
	return Get(&BotInfoType::operonly);
}

void BotInfo::SetNick(const Anope::string &nick)
{
	Set(&BotInfoType::nick, nick);
}

Anope::string BotInfo::GetNick()
{
	return Get(&BotInfoType::nick);
}

void BotInfo::SetUser(const Anope::string &user)
{
	Set(&BotInfoType::user, user);
}

Anope::string BotInfo::GetUser()
{
	return Get(&BotInfoType::user);
}

void BotInfo::SetHost(const Anope::string &host)
{
	Set(&BotInfoType::host, host);
}

Anope::string BotInfo::GetHost()
{
	return Get(&BotInfoType::host);
}

void BotInfo::SetRealName(const Anope::string &realname)
{
	Set(&BotInfoType::realname, realname);
}

Anope::string BotInfo::GetRealName()
{
	return Get(&BotInfoType::realname);
}

