/* NickServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSSetChanstats : public Command
{
 public:
	CommandCSSetChanstats(Module *creator) : Command(creator, "chanserv/set/chanstats", 2, 2)
	{
		this->SetDesc(_("Turn chanstat statistics on or off"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		User *u = source.u;
		ChannelInfo *ci = cs_findchan(params[0]);
		if (!ci)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (source.permission.empty() && !ci->AccessFor(u).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_STATS);
			source.Reply(_("Chanstats statistics are now enabled for this channel"));
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_STATS);
			source.Reply(_("Chanstats statistics are now disabled for this channel"));
		}
		else
			this->OnSyntaxError(source, "");
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply("Turn Chanstats channel statistics ON or OFF");
		return true;
	}
};

class CSSetChanstats : public Module
{
	CommandCSSetChanstats commandcssetchanstats;
	bool CSDefChanstats;
 public:
	CSSetChanstats(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetchanstats(this)
	{
		this->SetAuthor("Anope");
		Implementation i[] = { I_OnReload, I_OnChanRegistered };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));
		this->OnReload();
	}
	void OnReload() anope_override
	{
		ConfigReader config;
		CSDefChanstats = config.ReadFlag("chanstats", "CSDefChanstats", "0", 0);
	}
	void OnChanRegistered(ChannelInfo *ci) anope_override
	{
		if (CSDefChanstats)
			ci->SetFlag(CI_STATS);
	}
};

MODULE_INIT(CSSetChanstats)
