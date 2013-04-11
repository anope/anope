/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

class HelpChannel : public Module
{
	Anope::string HelpChan;

 public:
	HelpChannel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
	{

		Implementation i[] = { I_OnChannelModeSet, I_OnReload };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		OnReload();
	}

	EventReturn OnChannelModeSet(Channel *c, MessageSource &setter, const Anope::string &mname, const Anope::string &param) anope_override
	{
		if (mname == "OP" && c && c->ci && c->name.equals_ci(this->HelpChan))
		{
			User *u = User::Find(param);

			if (u && c->ci->AccessFor(u).HasPriv("OPME"))
				u->SetMode(OperServ, "HELPOP");
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
