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

class CommandNSSetKill : public Command
{
 public:
	CommandNSSetKill(Module *creator, const Anope::string &sname = "nickserv/set/kill", size_t min = 1) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Turn protection on or off"));
		this->SetSyntax(_("{ON | QUICK | IMMED | OFF}"));
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param)
	{
		const NickAlias *na = findnick(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		if (param.equals_ci("ON"))
		{
			nc->SetFlag(NI_KILLPROTECT);
			nc->UnsetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			source.Reply(_("Protection is now \002on\002 for \002%s\002."), nc->display.c_str());
		}
		else if (param.equals_ci("QUICK"))
		{
			nc->SetFlag(NI_KILLPROTECT);
			nc->SetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			source.Reply(_("Protection is now \002on\002 for \002%s\002, with a reduced delay."), nc->display.c_str());
		}
		else if (param.equals_ci("IMMED"))
		{
			if (Config->NSAllowKillImmed)
			{
				nc->SetFlag(NI_KILLPROTECT);
				nc->SetFlag(NI_KILL_IMMED);
				nc->UnsetFlag(NI_KILL_QUICK);
				source.Reply(_("Protection is now \002on\002 for \002%s\002, with no delay."), nc->display.c_str());
			}
			else
				source.Reply(_("The \002IMMED\002 option is not available on this network."));
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetFlag(NI_KILLPROTECT);
			nc->UnsetFlag(NI_KILL_QUICK);
			nc->UnsetFlag(NI_KILL_IMMED);
			source.Reply(_("Protection is now \002off\002 for \002%s\002."), nc->display.c_str());
		}
		else
			this->OnSyntaxError(source, "KILL");

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
		source.Reply(_("Turns the automatic protection option for your nick\n"
				"on or off.  With protection on, if another user\n"
				"tries to take your nick, they will be given one minute to\n"
				"change to another nick, after which %s will forcibly change\n"
				"their nick.\n"
				" \n"
				"If you select \002QUICK\002, the user will be given only 20 seconds\n"
				"to change nicks instead of the usual 60.  If you select\n"
				"\002IMMED\002, user's nick will be changed immediately \037without\037 being\n"
				"warned first or given a chance to change their nick; please\n"
				"do not use this option unless necessary. Also, your\n"
				"network's administrators may have disabled this option."), Config->NickServ.c_str());
		return true;
	}
};

class CommandNSSASetKill : public CommandNSSetKill
{
 public:
	CommandNSSASetKill(Module *creator) : CommandNSSetKill(creator, "nickserv/saset/kill", 2)
	{
		this->SetSyntax(_("\037nickname\037 {ON | QUICK | IMMED | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		this->ClearSyntax();
		this->Run(source, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Turns the automatic protection option for the nick\n"
				"on or off. With protection on, if another user\n"
				"tries to take the nick, they will be given one minute to\n"
				"change to another nick, after which %s will forcibly change\n"
				"their nick.\n"
				" \n"
				"If you select \002QUICK\002, the user will be given only 20 seconds\n"
				"to change nicks instead of the usual 60. If you select\n"
				"\002IMMED\002, the user's nick will be changed immediately \037without\037 being\n"
				"warned first or given a chance to change their nick; please\n"
				"do not use this option unless necessary. Also, your\n"
				"network's administrators may have disabled this option."), Config->NickServ.c_str());
		return true;
	}
};

class NSSetKill : public Module
{
	CommandNSSetKill commandnssetkill;
	CommandNSSASetKill commandnssasetkill;

 public:
	NSSetKill(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssetkill(this), commandnssasetkill(this)
	{
		this->SetAuthor("Anope");

		if (Config->NoNicknameOwnership)
			throw ModuleException(modname + " can not be used with options:nonicknameownership enabled");
	}
};

MODULE_INIT(NSSetKill)
