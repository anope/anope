/*
 * Copyright (C) 2008 Naram Qashat <cyberbotx@cyberbotx.com>
 * Copyright (C) 2008 Anope Team <info@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 *
 * $Id$
 *
 */

#include "services.h"

Service::Service(const char *nnick, const char *nuser, const char *nhost, const char *nreal) : BotInfo(nnick, nuser, nhost, nreal)
{
	ircdproto->SendClientIntroduction(this->nick, this->user, this->host, this->real, ircd->pseudoclient_mode, this->uid.c_str());
}

Service::~Service()
{
	ircdproto->SendQuit(this, "Terminating");
}
