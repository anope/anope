/*
 *
 * (C) 2011-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

class ChanServService
	: public Service
{
public:
	ChanServService(Module *m) : Service(m, "ChanServService", "ChanServ")
	{
	}

	/* Have ChanServ hold the channel, that is, join and set +nsti and wait
	 * for a few minutes so no one can join or rejoin.
	 */
	virtual void Hold(Channel *c) = 0;
};
