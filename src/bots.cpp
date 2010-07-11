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

BotInfo::BotInfo(const std::string &nnick, const std::string &nuser, const std::string &nhost, const std::string &nreal) : User(nnick, ts6_uid_retrieve())
{
	this->ident = nuser;
	this->host = sstrdup(nhost.c_str());
	this->realname = sstrdup(nreal.c_str());
	this->server = Me;

	this->lastmsg = this->created = time(NULL);

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
	if (Me && Me->GetUplink() && Me->GetUplink()->IsSynced())
	{
		ircdproto->SendClientIntroduction(this->nick, this->GetIdent(), this->host, this->realname, ircd->pseudoclient_mode, this->uid);
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


void BotInfo::SetNewNick(const std::string &newnick)
{
	UserListByNick.erase(this->nick.c_str());
	BotListByNick.erase(this->nick.c_str());

	this->nick = newnick;

	UserListByNick[this->nick.c_str()] = this;
	BotListByNick[this->nick.c_str()] = this;
}

void BotInfo::RejoinAll()
{
	for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
	{
		ChannelInfo *ci = it->second;

		if (ci->bi == this && ci->c && ci->c->users.size() >= Config.BSMinUsers)
			this->Join(ci->c);
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
	if (ci->c && ci->c->users.size() >= Config.BSMinUsers)
		this->Join(ci->c);
}

void BotInfo::UnAssign(User *u, ChannelInfo *ci)
{
	EventReturn MOD_RESULT = EVENT_CONTINUE;
	FOREACH_RESULT(I_OnBotUnAssign, OnBotUnAssign(u, ci));
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (ci->c && ci->c->FindUser(ci->bi))
	{
		if (u)
			ci->bi->Part(ci->c, "UNASSIGN from " + u->nick);
		else
			ci->bi->Part(ci->c);
	}

	ci->bi = NULL;
}

void BotInfo::Join(Channel *c)
{
	if (Config.BSSmartJoin)
	{
		/* We check for bans */
		if (c->bans && c->bans->count)
		{
			Entry *ban, *next;

			for (ban = c->bans->entries; ban; ban = next)
			{
				next = ban->next;

				if (entry_match(ban, this->nick.c_str(), this->GetIdent().c_str(), this->host, 0))
					c->RemoveMode(NULL, CMODE_BAN, ban->mask);
			}

			std::string Limit;
			int limit = 0;
			if (c->GetParam(CMODE_LIMIT, Limit))
				limit = atoi(Limit.c_str());

			/* Should we be invited? */
			if (c->HasMode(CMODE_INVITE) || (limit && c->users.size() >= limit))
				ircdproto->SendNoticeChanops(this, c, "%s invited %s into the channel.", this->nick.c_str(), this->nick.c_str());
		}
	}

	ircdproto->SendJoin(this, c->name.c_str(), c->creation_time);
	for (std::list<ChannelModeStatus *>::iterator it = BotModes.begin(), it_end = BotModes.end(); it != it_end; ++it)
		c->SetMode(this, *it, this->nick, false);
	c->JoinUser(this);
	FOREACH_MOD(I_OnBotJoin, OnBotJoin(c->ci, this));
}

void BotInfo::Join(const std::string &chname)
{
	Channel *c = findchan(chname);
	return this->Join(c ? c : new Channel(chname));
}

void BotInfo::Part(Channel *c, const std::string &reason)
{
	ircdproto->SendPart(this, c, !reason.empty() ? reason.c_str() : "");
	c->DeleteUser(this);
}
