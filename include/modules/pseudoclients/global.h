/*
 *
 * (C) 2011-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

class GlobalService : public Service
{
public:
	GlobalService(Module *m) : Service(m, "GlobalService", "Global")
	{
	}

	/** Retrieves the bot which sends global messages unless otherwise specified. */
	virtual Reference<BotInfo> GetDefaultSender() = 0;

	/** Send out a global message to all users
	 * @param sender Our client which should send the global
	 * @param source The sender of the global
	 * @param message The message
	 */
	virtual void SendGlobal(BotInfo *sender, const Anope::string &source, const Anope::string &message) = 0;
};
