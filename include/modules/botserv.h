/*
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#pragma once

namespace BotServ
{
	class BotServService : public Service
	{
	 public:
		BotServService(Module *m) : Service(m, "BotServService", "BotServ")
		{
		}

	};
}
