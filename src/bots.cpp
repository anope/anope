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
	insert_bot(this); // XXX, this is ugly, but it needs to stay until hashing of bots is redone in STL.
	nbots++;
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
