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

class CommandNSSetChanstats : public Command
{
 public:
	CommandNSSetChanstats(Module *creator, const Anope::string &sname = "nickserv/set/chanstats", size_t min = 1 ) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Turn chanstat statistic on or off"));
		this->SetSyntax(_("{ON | OFF}"));
	}
	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		NickAlias *na = findnick(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, na->nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (param.equals_ci("ON"))
		{
			na->nc->SetFlag(NI_STATS);
			source.Reply(_("Chanstat statistics are now enabled for your nick"));
		}
		else if (param.equals_ci("OFF"))
		{
			na->nc->UnsetFlag(NI_STATS);
			source.Reply(_("Chanstat statistics are now disabled for your nick"));
		}
		else
			this->OnSyntaxError(source, "CHANSTATS");

		return;
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, source.nc->display, params[0]);
	}
	
	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns Chanstats statistics ON or OFF"));
		return true;
	}
};

class CommandNSSASetChanstats : public CommandNSSetChanstats
{
 public:
	CommandNSSASetChanstats(Module *creator) : CommandNSSetChanstats(creator, "nickserv/saset/chanstats", 2)
	{
		this->ClearSyntax();
		this->SetSyntax(_("\037nickname\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns chanstats channel statistics ON or OFF for this user"));
		return true;
	}
};

class NSSetChanstats : public Module
{
	CommandNSSetChanstats commandnssetchanstats;
	CommandNSSASetChanstats commandnssasetchanstats;
	bool NSDefChanstats;

 public:
	NSSetChanstats(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssetchanstats(this), commandnssasetchanstats(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnNickRegister };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		this->OnReload();
	}
	void OnReload() anope_override
	{
		ConfigReader config;
		NSDefChanstats = config.ReadFlag("chanstats", "NSDefChanstats", "0", 0);
	}
	void OnNickRegister(NickAlias *na) anope_override
	{
		if (NSDefChanstats)
			na->nc->SetFlag(NI_STATS);
	}
};

MODULE_INIT(NSSetChanstats)
