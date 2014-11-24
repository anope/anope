/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#include "module.h"

class BSAutoAssign : public Module
	, public EventHook<Event::ChanRegistered>
{
 public:
	BSAutoAssign(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::ChanRegistered>("OnChanRegistered")
	{
	}

	void OnChanRegistered(ChanServ::Channel *ci) override
	{
		const Anope::string &bot = Config->GetModule(this)->Get<const Anope::string>("bot");
		if (bot.empty())
			return;

		ServiceBot *bi = ServiceBot::Find(bot, true);
		if (bi == NULL)
		{
			Log(this) << "bs_autoassign is configured to assign bot " << bot << ", but it does not exist?";
			return;
		}

		bi->Assign(NULL, ci);
	}
};

MODULE_INIT(BSAutoAssign)
