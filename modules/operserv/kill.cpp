/* OperServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandOSKill : public Command
{
 public:
	CommandOSKill(Module *creator) : Command(creator, "operserv/kill", 1, 2)
	{
		this->SetDesc(_("Kill a user"));
		this->SetSyntax(_("\037user\037 [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[0];
		Anope::string reason = params.size() > 1 ? params[1] : "";

		User *u2 = User::Find(nick, true);
		if (u2 == NULL)
			source.Reply(_("\002{0}\002 isn't currently online."), nick);
		else if (u2->IsProtected() || u2->server == Me)
			source.Reply(_("\002{0}\002 is protected and cannot be killed."), u2->nick);
		else
		{
			if (reason.empty())
				reason = "No reason specified";
			if (Config->GetModule("operserv")->Get<bool>("addakiller"))
				reason = "(" + source.GetNick() + ") " + reason;
			Log(LOG_ADMIN, source, this) << "on " << u2->nick << " for " << reason;
			u2->Kill(*source.service, reason);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows you to kill a user from the network. Parameters are the same as for the standard /KILL command."));
		return true;
	}
};

class OSKill : public Module
{
	CommandOSKill commandoskill;

 public:
	OSKill(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandoskill(this)
	{

	}
};

MODULE_INIT(OSKill)
