/* StatServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

BotInfo *statserv = NULL;

class CommandSSHelp : public Command
{
 public:
	CommandSSHelp() : Command("HELP", 0, 0)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ircdproto->SendMessage(statserv, u->nick, "This is a test of the emergency StatServ system.");
		return MOD_CONT;
	}
};

class SSMain : public Module
{
	CommandSSHelp commandsshelp;

 public:
	SSMain(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);
		this->SetPermanent(true);

		statserv = findbot("StatServ");
		if (!statserv)
		{
			Log() << "Creating SS";
			statserv = new BotInfo("StatServ", Config->ServiceUser, Config->ServiceHost, "Stats Service");
		}
		Log() << "Done creating SS";

		this->AddCommand(statserv, &commandsshelp);
	}

	~SSMain()
	{
		if (statserv)
		{
			for (CommandMap::iterator it = statserv->Commands.begin(), it_end = statserv->Commands.end(); it != it_end; ++it)
				this->DelCommand(statserv, it->second);

			delete statserv;
		}
	}
};

MODULE_INIT(SSMain)
