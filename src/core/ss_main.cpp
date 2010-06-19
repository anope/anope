/* StatServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#include "module.h"

BotInfo *statserv = NULL;

class CommandSSHelp : public Command
{
 public:
	CommandSSHelp() : Command("HELP", 0, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ircdproto->SendMessage(statserv, u->nick.c_str(), "This is a test of the emergency StatServ system.");
		return MOD_CONT;
	}
};

class SSMain : public Module
{
 public:
	SSMain(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
		this->SetPermanent(true);

		statserv = findbot("StatServ");
		if (!statserv)
		{
			Alog() << "Creating SS";
			statserv = new BotInfo("StatServ", Config.ServiceUser, Config.ServiceHost, "Stats Service");
		}
		Alog() << "Done creating SS";

		this->AddCommand(statserv, new CommandSSHelp());
	}

	~SSMain()
	{
		if (statserv)
		{
			for (std::map<ci::string, Command *>::iterator it = statserv->Commands.begin(); it != statserv->Commands.end(); ++it)
			{
				this->DelCommand(statserv, it->second);
			}

			ircdproto->SendQuit(statserv, "Quit due to module unload.");
			delete statserv;
		}
	}
};

MODULE_INIT(SSMain)
