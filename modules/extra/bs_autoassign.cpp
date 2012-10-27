/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class BSAutoAssign : public Module
{
	Anope::string bot;

 public:
	BSAutoAssign(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnChanRegistered, I_OnReload };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}

	void OnChanRegistered(ChannelInfo *ci) anope_override
	{
		if (this->bot.empty())
			return;

		BotInfo *bi = findbot(this->bot);
		if (bi == NULL)
		{
			Log(this) << "bs_autoassign is configured to assign bot " << this->bot << ", but it does not exist?";
			return;
		}

		bi->Assign(NULL, ci);
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		this->bot = config.ReadValue("bs_autoassign", "bot", "", 0);
	}
};

MODULE_INIT(BSAutoAssign)
