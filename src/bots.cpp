/*
 * Copyright (C) 2008-2010 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"
#include "modules.h"
#include "commands.h"

botinfo_map BotListByNick;
botinfo_uid_map BotListByUID;

BotInfo *BotServ = NULL;
BotInfo *ChanServ = NULL;
BotInfo *Global = NULL;
BotInfo *HostServ = NULL;
BotInfo *MemoServ = NULL;
BotInfo *NickServ = NULL;
BotInfo *OperServ = NULL;

BotInfo::BotInfo(const std::string &nnick, const std::string &nuser, const std::string &nhost, const std::string &nreal)
{
	this->nick = nnick;
	this->user = nuser;
	this->host = nhost;
	this->real = nreal;
	this->lastmsg = this->created = time(NULL);
	this->uid = ts6_uid_retrieve();
	this->chancount = 0;

	ci::string ci_nick(nnick.c_str());
	if (Config.s_ChanServ && ci_nick == Config.s_ChanServ)
		ChanServ = this;
	else if (Config.s_BotServ && ci_nick == Config.s_BotServ)
		BotServ = this;
	else if (Config.s_HostServ && ci_nick == Config.s_HostServ)
		HostServ = this;
	else if (Config.s_OperServ && ci_nick == Config.s_OperServ)
		OperServ = this;
	else if (Config.s_MemoServ && ci_nick == Config.s_MemoServ)
		MemoServ = this;
	else if (Config.s_NickServ && ci_nick == Config.s_NickServ)
		NickServ = this;
	else if (Config.s_GlobalNoticer && ci_nick == Config.s_GlobalNoticer)
		Global = this;

	BotListByNick[this->nick.c_str()] = this;
	if (!this->uid.empty())
		BotListByUID[this->uid] = this;

	// If we're synchronised with the uplink already, call introduce_user() for this bot.
	if (Me && Me->GetUplink()->IsSynced())
	{
		ircdproto->SendClientIntroduction(this->nick, this->user, this->host, this->real, ircd->pseudoclient_mode, this->uid);
		XLine x(this->nick.c_str(), "Reserved for services");
		ircdproto->SendSQLine(&x);
	}
}

BotInfo::~BotInfo()
{
	for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;

		if (ci->bi == this)
			ci->bi = NULL;
	}

	BotListByNick.erase(this->nick.c_str());
	if (!this->uid.empty())
		BotListByUID.erase(this->uid);
}


void BotInfo::ChangeNick(const char *newnick)
{
	BotListByNick.erase(this->nick.c_str());

	this->nick = newnick;

	BotListByNick[this->nick.c_str()] = this;
}

void BotInfo::RejoinAll()
{
	for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;

		if (ci->bi == this && ci->c && ci->c->users.size() >= Config.BSMinUsers)
			bot_join(ci);
	}
}

void BotInfo::Assign(User *u, ChannelInfo *ci)
{
	EventReturn MOD_RESULT = EVENT_CONTINUE;
	FOREACH_RESULT(I_OnBotAssign, OnBotAssign(u, ci, this));
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (ci->bi)
		ci->bi->UnAssign(u, ci);

	ci->bi = this;
	++this->chancount;
	if (ci->c && ci->c->users.size() >= Config.BSMinUsers)
		bot_join(ci);
}

void BotInfo::UnAssign(User *u, ChannelInfo *ci)
{
	EventReturn MOD_RESULT = EVENT_CONTINUE;
	FOREACH_RESULT(I_OnBotUnAssign, OnBotUnAssign(u, ci));
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (ci->c && ci->c->users.size() >= Config.BSMinUsers)
	{
		if (u)
			ircdproto->SendPart(ci->bi, ci->c, "UNASSIGN from %s", u->nick.c_str());
		else
			ircdproto->SendPart(ci->bi, ci->c, "");
	}

	--ci->bi->chancount;
	ci->bi = NULL;
}
