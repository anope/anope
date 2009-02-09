/* HelpServ core functions
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

class CommandHEHelp : public Command
{
 public:
	CommandHEHelp() : Command("HELP", 1, 1)
	{
	}

	CommandResult Execute(User *u, std::vector<std::string> &params)
	{
		mod_help_cmd(s_HelpServ, u, HELPSERV, params[0].c_str());
		return MOD_CONT;
	}

	void OnSyntaxError(User *u)
	{
		notice_help(s_HelpServ, u, HELP_HELP, s_NickServ, s_ChanServ, s_MemoServ);
		if (s_BotServ)
			notice_help(s_HelpServ, u, HELP_HELP_BOT, s_BotServ);
		if (s_HostServ)
			notice_help(s_HelpServ, u, HELP_HELP_HOST, s_HostServ);
		moduleDisplayHelp(7, u);
	}
};

class HEHelp : public Module
{
 public:
	HEHelp(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(HELPSERV, new CommandHEHelp(), MOD_UNIQUE);
	}
};

MODULE_INIT("he_help", HEHelp)
