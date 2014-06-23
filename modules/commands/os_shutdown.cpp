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

class CommandOSQuit : public Command
{
 public:
	CommandOSQuit(Module *creator) : Command(creator, "operserv/quit", 0, 0)
	{
		this->SetDesc(_("Terminate Services without saving"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Log(LOG_ADMIN, source, this);
		Anope::QuitReason = source.command + " command received from " + source.GetNick();
		Anope::Quitting = true;
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Causes Services to do an immediate shutdown; databases may \002not\002 saved."
		               " If you are using a real time database such as SQL or Redis, this command is not useful."
		               " This command should not be used unless damage to the in-memory copies of the databases is feared and they should not be saved."));
		return true;
	}
};

class CommandOSRestart : public Command
{
 public:
	CommandOSRestart(Module *creator) : Command(creator, "operserv/restart", 0, 0)
	{
		this->SetDesc(_("Save databases and restart Services"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Log(LOG_ADMIN, source, this);
		Anope::QuitReason = source.command + " command received from " + source.GetNick();
		Anope::Quitting = Anope::Restarting = true;
		Anope::SaveDatabases();
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Causes Services to restart."));
		return true;
	}
};

class CommandOSShutdown : public Command
{
 public:
	CommandOSShutdown(Module *creator) : Command(creator, "operserv/shutdown", 0, 0)
	{
		this->SetDesc(_("Terminate services with save"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Log(LOG_ADMIN, source, this);
		Anope::QuitReason = source.command + " command received from " + source.GetNick();
		Anope::Quitting = true;
		Anope::SaveDatabases();
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Causes Services to shut down"));
		return true;
	}
};

class OSShutdown : public Module
{
	CommandOSQuit commandosquit;
	CommandOSRestart commandosrestart;
	CommandOSShutdown commandosshutdown;

 public:
	OSShutdown(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandosquit(this), commandosrestart(this), commandosshutdown(this)
	{

	}
};

MODULE_INIT(OSShutdown)
