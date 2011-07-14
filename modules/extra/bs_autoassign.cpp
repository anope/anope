/*
 *
 * (C) 2003-2011 Anope Team
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
		ModuleManager::Attach(i, this, 2);

		this->OnReload();
	}

	void OnChanRegistered(ChannelInfo *ci)
	{
		if (this->bot.empty())
			return;

		BotInfo *bi = findbot(this->bot);
		if (bi == NULL)
		{
			Log() << "bs_autoassign is configured to assign bot " << this->bot << ", but it does not exist?";
			return;
		}

		bi->Assign(NULL, ci);
	}

	void OnReload()
	{
		ConfigReader config;
		this->bot = config.ReadValue("bs_autoassign", "bot", "", 0);
	}
};

MODULE_INIT(BSAutoAssign)
