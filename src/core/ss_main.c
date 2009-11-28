/* StatServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */

#include "module.h"

BotInfo *statserv = NULL;
CommandHash *cmdTable[MAX_CMD_HASH];

class CommandSSHelp : public Command
{
 public:
	CommandSSHelp() : Command("HELP", 0, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ircdproto->SendMessage(statserv, u->nick, "This is a test of the emergency StatServ system.");
		return MOD_CONT;
	}
};

class SSMain : public Module
{
 public:
	SSMain(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->SetPermanent(true);

		this->AddCommand(cmdTable, new CommandSSHelp());
		ModuleManager::Attach(I_OnBotPreLoad, this);

		statserv = findbot("StatServ");
		if (!statserv)
		{
			alog("Creating SS");
			statserv = new BotInfo("StatServ", Config.ServiceUser, Config.ServiceHost, "Stats Service");
		}
		alog("Done creating SS");
		statserv->cmdTable = cmdTable;
	}

	~SSMain()
	{
		CommandHash *current;
		Command *c;
		for (int i = 0; i < MAX_CMD_HASH; ++i)
		{
			for (current = cmdTable[i]; current; current = current->next)
			{
				for (c = current->c; c; c = c->next)
					this->DelCommand(cmdTable, c->name.c_str());
			}
		}
		if (statserv)
		{
			ircdproto->SendQuit(statserv, "Quit due to module unload.");
			delete statserv;
		}
	}

	void OnBotPreLoad(BotInfo *bi)
	{
		if (!strcmp(bi->nick, "StatServ"))
		{
			delete statserv;
			statserv = bi;
			statserv->cmdTable = cmdTable;
		}
	}
};

MODULE_INIT(SSMain)
