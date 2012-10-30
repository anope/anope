/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

class HelpChannel : public Module
{
	Anope::string HelpChan;

 public:
	HelpChannel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnChannelModeSet, I_OnReload };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		OnReload();
	}

	EventReturn OnChannelModeSet(Channel *c, MessageSource &setter, ChannelModeName Name, const Anope::string &param) anope_override
	{
		if (Name == CMODE_OP && c && c->ci && c->name.equals_ci(this->HelpChan))
		{
			User *u = finduser(param);

			if (u && c->ci->AccessFor(u).HasPriv("OPDEOPME"))
				u->SetMode(findbot(Config->OperServ), UMODE_HELPOP);
		}

		return EVENT_CONTINUE;
	}

	void OnReload() anope_override
	{
		ConfigReader config;

		this->HelpChan = config.ReadValue("m_helpchan", "helpchannel", "", 0);
	}
};

MODULE_INIT(HelpChannel)
