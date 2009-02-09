/* BotServ core functions
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
/*************************************************************************/

#include "module.h"

class CommandBSHelp : public Command
{
 public:
	CommandBSHelp() : Command("HELP", 1, 1)
	{
	}
	
	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		mod_help_cmd(s_BotServ, u, BOTSERV, params[0].c_str());
		return MOD_CONT;
	}

	void OnSyntaxError(User *u)
	{
		notice_help(s_BotServ, u, BOT_HELP);
		moduleDisplayHelp(4, u);
		notice_help(s_BotServ, u, BOT_HELP_FOOTER, BSMinUsers);
	}
};

class BSHelp : public Module
{
 public:
	BSHelp(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(BOTSERV, new CommandBSHelp(), MOD_UNIQUE);
	}
};



MODULE_INIT("bs_help", BSHelp)
