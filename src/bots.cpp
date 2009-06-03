/*
 * Copyright (C) 2008-2009 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2009 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 *
 * $Id$
 *
 */

#include "services.h"
#include "modules.h"

BotInfo::BotInfo(const char *nnick)
{
	this->nick = sstrdup(nnick);
	this->lastmsg = time(NULL);
	this->uid = ts6_uid_retrieve(); // XXX is this safe? has ts6 been setup yet?
	insert_bot(this); // XXX, this is ugly, but it needs to stay until hashing of bots is redone in STL.
	nbots++;
	this->cmdTable = NULL;

	if (s_ChanServ && !stricmp(s_ChanServ, nnick))
		this->flags |= BI_CHANSERV;
	else if (s_BotServ && !stricmp(s_BotServ, nnick))
		this->flags |= BI_BOTSERV;
	else if (s_HostServ && !stricmp(s_BotServ, nnick))
		this->flags |= BI_HOSTSERV;
	else if (s_OperServ && !stricmp(s_OperServ, nnick))
		this->flags |= BI_OPERSERV;
	else if (s_MemoServ && !stricmp(s_MemoServ, nnick))
		this->flags |= BI_MEMOSERV;
	else if (s_NickServ && !stricmp(s_NickServ, nnick))
		this->flags |= BI_NICKSERV;
	else if (s_GlobalNoticer && !stricmp(s_GlobalNoticer, nnick))
		this->flags |= BI_GLOBAL;
}

BotInfo::BotInfo(const char *nnick, const char *nuser, const char *nhost, const char *nreal)
{
	this->nick = sstrdup(nnick);
	this->user = sstrdup(nuser);
	this->host = sstrdup(nhost);
	this->real = sstrdup(nreal);
	this->lastmsg = time(NULL);
	this->uid = ts6_uid_retrieve(); // XXX is this safe? has ts6 been setup yet?
	insert_bot(this); // XXX, this is ugly, but it needs to stay until hashing of bots is redone in STL.
	nbots++;
	this->cmdTable = NULL;

	if (s_ChanServ && !stricmp(s_ChanServ, nnick))
		this->flags |= BI_CHANSERV;
	else if (s_BotServ && !stricmp(s_BotServ, nnick))
		this->flags |= BI_BOTSERV;
	else if (s_HostServ && !stricmp(s_BotServ, nnick))
		this->flags |= BI_HOSTSERV;
	else if (s_OperServ && !stricmp(s_OperServ, nnick))
		this->flags |= BI_OPERSERV;
	else if (s_MemoServ && !stricmp(s_MemoServ, nnick))
		this->flags |= BI_MEMOSERV;
	else if (s_NickServ && !stricmp(s_NickServ, nnick))
		this->flags |= BI_NICKSERV;
	else if (s_GlobalNoticer && !stricmp(s_GlobalNoticer, nnick))
		this->flags |= BI_GLOBAL;
}

BotInfo::~BotInfo()
{
	int i;
	ChannelInfo *ci;

	for (i = 0; i < 256; i++)
		for (ci = chanlists[i]; ci; ci = ci->next)
			if (ci->bi == this)
				ci->bi = NULL;

	if (this->next)
		this->next->prev = this->prev;
	if (this->prev)
		this->prev->next = this->next;
	else
		botlists[tolower(*this->nick)] = this->next;

	nbots--;

	delete [] this->nick;
	delete [] this->user;
	delete [] this->host;
	delete [] this->real;
}


void BotInfo::ChangeNick(const char *newnick)
{
	if (this->next)
		this->next->prev = this->prev;
	if (this->prev)
		this->prev->next = this->next;
	else
		botlists[tolower(*this->nick)] = this->next;

	if (this->nick)
		delete [] this->nick;
	this->nick = sstrdup(newnick);

	insert_bot(this);
}

void BotInfo::RejoinAll()
{
	int i;
	ChannelInfo *ci;

	for (i = 0; i < 256; i++)
		for (ci = chanlists[i]; ci; ci = ci->next)
			if (ci->bi == this && ci->c && (ci->c->usercount >= BSMinUsers))
				bot_join(ci);
}

void BotInfo::Assign(User *u, ChannelInfo *ci)
{
	EventReturn MOD_RESULT = EVENT_CONTINUE;
	FOREACH_RESULT(I_OnBotAssign, OnBotAssign(u, ci, this));
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (ci->bi)
	{
		if (u)
			ci->bi->UnAssign(u, ci);
		else
			ci->bi->UnAssign(NULL, ci);
	}

	ci->bi = this;
	this->chancount++;
	if (ci->c && ci->c->usercount >= BSMinUsers)
		bot_join(ci);
}

void BotInfo::UnAssign(User *u, ChannelInfo *ci)
{
	EventReturn MOD_RESULT = EVENT_CONTINUE;
	FOREACH_RESULT(I_OnBotUnAssign, OnBotUnAssign(u, ci));
	if (MOD_RESULT == EVENT_STOP)
		return;

	if (u && ci->c && ci->c->usercount >= BSMinUsers)
		ircdproto->SendPart(ci->bi, ci->name, "UNASSIGN from %s", u->nick);

	ci->bi->chancount--;
	ci->bi = NULL;
}


