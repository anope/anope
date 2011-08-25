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

class CommandOSQuit : public Command
{
 public:
	CommandOSQuit(Module *creator) : Command(creator, "operserv/quit", 0, 0)
	{
		this->SetDesc(_("Terminate Services WITHOUT saving"));
		this->SetSyntax("");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		quitmsg = "QUIT command received from " + u->nick;
		quitting = true;
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Causes Services to do an immediate shutdown; databases are\n"
				"\002not\002 saved.  This command should not be used unless\n"
				"damage to the in-memory copies of the databases is feared\n"
				"and they should not be saved."));
		return true;
	}
};

class CommandOSRestart : public Command
{
 public:
	CommandOSRestart(Module *creator) : Command(creator, "operserv/restart", 0, 0)
	{
		this->SetDesc(_("Save databases and restart Services"));
		this->SetSyntax("");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		quitmsg = "RESTART command received from " + u->nick;
		save_databases();
		quitting = restarting = true;
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(_("Causes Services to save all databases and then restart\n"
				"(i.e. exit and immediately re-run the executable)."));
		return true;
	}
};

class CommandOSShutdown : public Command
{
 public:
	CommandOSShutdown(Module *creator) : Command(creator, "operserv/shutdown", 0, 0)
	{
		this->SetDesc(_("Terminate services with save"));
		this->SetSyntax("");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		quitmsg = source.command + " command received from " + u->nick;
		save_databases();
		quitting = true;
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Causes Services to save all databases and then shut down."));
		return true;
	}
};

class OSShutdown : public Module
{
	CommandOSQuit commandosquit;
	CommandOSRestart commandosrestart;
	CommandOSShutdown commandosshutdown;

 public:
	OSShutdown(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandosquit(this), commandosrestart(this), commandosshutdown(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(OSShutdown)
