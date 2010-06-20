/*
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"

class HelpChannel : public Module
{
	ci::string HelpChan;

 public:
	HelpChannel(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(SUPPORTED);

		Implementation i[] = { I_OnChannelModeSet, I_OnReload };
		ModuleManager::Attach(i, this, 2);

		OnReload(true);
	}

	EventReturn OnChannelModeSet(Channel *c, ChannelModeName Name, const std::string &param)
	{
		if (Name == CMODE_OP && c && c->ci && c->name == this->HelpChan)
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

		this->HelpChan = config.ReadValue("m_helpchan", "helpchannel", "", 0).c_str();
	}
};

MODULE_INIT(HelpChannel)
