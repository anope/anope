/*
 * Copyright (C) 2008-2010 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 *
 *
 */

#include "services.h"
#include "modules.h"

BotInfo::BotInfo(const std::string &nnick, const std::string &nuser, const std::string &nhost, const std::string &nreal)
{
	this->nick = nnick;
	this->user = nuser;
	this->host = nhost;
	this->real = nreal;
	this->lastmsg = this->created = time(NULL);
	this->uid = ts6_uid_retrieve();
	this->cmdTable = NULL;
	++nbots;
	this->chancount = 0;

	ci::string ci_nick(nnick.c_str());
	if (Config.s_ChanServ && ci_nick == Config.s_ChanServ)
	{
		this->cmdTable = CHANSERV;
		this->SetFlag(BI_CHANSERV);
	}
	else if (Config.s_BotServ && ci_nick == Config.s_BotServ)
	{
		this->cmdTable = BOTSERV;
		this->SetFlag(BI_BOTSERV);
	}
	else if (Config.s_HostServ && ci_nick == Config.s_HostServ)
	{
		this->cmdTable = HOSTSERV;
		this->SetFlag(BI_HOSTSERV);
	}
	else if (Config.s_OperServ && ci_nick == Config.s_OperServ)
	{
		this->cmdTable = OPERSERV;
		this->SetFlag(BI_OPERSERV);
	}
	else if (Config.s_MemoServ && ci_nick == Config.s_MemoServ)
	{
		this->cmdTable = MEMOSERV;
		this->SetFlag(BI_MEMOSERV);
	}
	else if (Config.s_NickServ && ci_nick == Config.s_NickServ)
	{
		this->cmdTable = NICKSERV;
		this->SetFlag(BI_NICKSERV);
	}
	else if (Config.s_GlobalNoticer && ci_nick == Config.s_GlobalNoticer)
	{
		this->SetFlag(BI_GLOBAL);
	}

	insert_bot(this); // XXX, this is ugly, but it needs to stay until hashing of bots is redone in STL.

	// If we're synchronised with the uplink already, call introduce_user() for this bot.
	if (serv_uplink && serv_uplink->sync == SSYNC_DONE)
	{
		ircdproto->SendClientIntroduction(this->nick, this->user, this->host, this->real, ircd->pseudoclient_mode, this->uid);
		ircdproto->SendSQLine(this->nick, "Reserved for services");
	}
}

BotInfo::~BotInfo()
{
	int i;
	ChannelInfo *ci;

	for (i = 0; i < 256; ++i)
		for (ci = chanlists[i]; ci; ci = ci->next)
			if (ci->bi == this)
				ci->bi = NULL;

	if (this->next)
		this->next->prev = this->prev;
	if (this->prev)
		this->prev->next = this->next;
	else
		botlists[tolower(this->nick[0])] = this->next;

	--nbots;
}


void BotInfo::ChangeNick(const char *newnick)
{
	if (this->next)
		this->next->prev = this->prev;
	if (this->prev)
		this->prev->next = this->next;
	else
		botlists[tolower(this->nick[0])] = this->next;

	this->nick = newnick;

	insert_bot(this);
}

void BotInfo::RejoinAll()
{
	int i;
	ChannelInfo *ci;

	for (i = 0; i < 256; ++i)
		for (ci = chanlists[i]; ci; ci = ci->next)
			if (ci->bi == this && ci->c && (ci->c->users.size() >= Config.BSMinUsers))
				bot_join(ci);
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
