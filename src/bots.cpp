/*
 * Copyright (C) 2008 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008 Anope Team <info@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 *
 * $Id$
 *
 */

#include "services.h"

BotInfo::BotInfo(const char *nnick)
{
	this->nick = sstrdup(nnick);
	this->lastmsg = time(NULL);
	this->uid = ts6_uid_retrieve(); // XXX is this safe? has ts6 been setup yet?
	insert_bot(this); // XXX, this is ugly, but it needs to stay until hashing of bots is redone in STL.
	nbots++;
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

    free(this->nick);
    free(this->user);
    free(this->host);
    free(this->real);
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
		free(this->nick);
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

	send_event(EVENT_BOT_ASSIGN, 2, ci->name, this->nick);
}

void BotInfo::UnAssign(User *u, ChannelInfo *ci)
{
	send_event(EVENT_BOT_UNASSIGN, 2, ci->name, ci->bi->nick);

	if (u && ci->c && ci->c->usercount >= BSMinUsers)
		ircdproto->SendPart(ci->bi->nick, ci->name, "UNASSIGN from %s", u->nick);

	ci->bi->chancount--;
	ci->bi = NULL;
}


