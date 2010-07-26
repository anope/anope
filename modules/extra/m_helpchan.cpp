/*
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

class HelpChannel : public Module
{
	Anope::string HelpChan;

 public:
	HelpChannel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(SUPPORTED);

		Implementation i[] = { I_OnChannelModeSet, I_OnReload };
		ModuleManager::Attach(i, this, 2);

		OnReload(true);
	}

	EventReturn OnChannelModeSet(Channel *c, ChannelModeName Name, const Anope::string &param)
	{
		if (Name == CMODE_OP && c && c->ci && c->name.equals_ci(this->HelpChan))
		{
			User *u = finduser(param);

			if (u)
				u->SetMode(OperServ, UMODE_HELPOP);
		}

		return EVENT_CONTINUE;
	}

	void OnReload(bool)
	{
		ConfigReader config;

		this->HelpChan = config.ReadValue("m_helpchan", "helpchannel", "", 0);
	}
};

MODULE_INIT(HelpChannel)
