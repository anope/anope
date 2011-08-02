/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandOSMode : public Command
{
 public:
	CommandOSMode(Module *creator) : Command(creator, "operserv/mode", 2, 2, "operserv/mode")
	{
		this->SetDesc(_("Change channel modes"));
		this->SetSyntax(_("\037channel\037 \037modes\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &target = params[0];
		const Anope::string &modes = params[1];

		Channel *c = findchan(target);
		if (!c)
			source.Reply(CHAN_X_NOT_IN_USE, target.c_str());
		else if (c->bouncy_modes)
			source.Reply(_("Services is unable to change modes. Are your servers' U:lines configured correctly?"));
		else
		{
			c->SetModes(source.owner, false, modes.c_str());

			Log(LOG_ADMIN, u, this) << modes << " on " << target;
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows Services operators to change modes for any channel.\n"
				"Parameters are the same as for the standard /MODE command."));
		return true;
	}
};

class CommandOSUMode : public Command
{
 public:
	CommandOSUMode(Module *creator) : Command(creator, "operserv/umode", 2, 2, "operserv/umode")
	{
		this->SetDesc(_("Change channel or user modes"));
		this->SetSyntax(_("\037user\037 \037modes\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &target = params[0];
		const Anope::string &modes = params[1];

		User *u2 = finduser(target);
		if (!u2)
			source.Reply(NICK_X_NOT_IN_USE, target.c_str());
		else
		{
			u2->SetModes(source.owner, "%s", modes.c_str());
			source.Reply(_("Changed usermodes of \002%s\002 to %s."), u2->nick.c_str(), modes.c_str());

			u2->SendMessage(source.owner, _("\002%s\002 changed your usermodes to %s."), u->nick.c_str(), modes.c_str());

			Log(LOG_ADMIN, u, this) << modes << " on " << target;
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows Services operators to change modes for any user.\n"
				"Parameters are the same as for the standard /MODE command."));
		return true;
	}
};

class OSMode : public Module
{
	CommandOSMode commandosmode;
	CommandOSUMode commandosumode;

 public:
	OSMode(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandosmode(this), commandosumode(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandosmode);
		ModuleManager::RegisterService(&commandosumode);
	}
};

MODULE_INIT(OSMode)
