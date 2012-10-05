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

class CommandNSSetAutoOp : public Command
{
 public:
	CommandNSSetAutoOp(Module *creator, const Anope::string &sname = "nickserv/set/autoop", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Should services op you automatically."));
		this->SetSyntax(_("{ON | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = findnick(user);
		if (na == NULL)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetNickOption, OnSetNickOption(source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;
		
		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_AUTOOP);
			source.Reply(_("Services will now autoop %s in channels."), nc->display.c_str());
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_AUTOOP);
			source.Reply(_("Services will no longer autoop %s in channels."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "AUTOOP");

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
		source.Reply(_("Sets whether you will be opped automatically. Set to ON to \n"
				"allow ChanServ to op you automatically when entering channels."));
		return true;
	}
};

class CommandNSSASetAutoOp : public CommandNSSetAutoOp
{
 public:
	CommandNSSASetAutoOp(Module *creator) : CommandNSSetAutoOp(creator, "nickserv/saset/autoop", 2)
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
		source.Reply(_("Sets whether the given nickname will be opped automatically.\n"
				"Set to \002ON\002 to allow ChanServ to op the given nickname \n"
				"omatically when joining channels."));
		return true;
	}
};

class NSSetAutoOp : public Module
{
	CommandNSSetAutoOp commandnssetautoop;
	CommandNSSASetAutoOp commandnssasetautoop;

 public:
	NSSetAutoOp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssetautoop(this), commandnssasetautoop(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSSetAutoOp)
