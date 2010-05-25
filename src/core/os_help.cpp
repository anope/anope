/* OperServ core functions
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
/*************************************************************************/

#include "module.h"

class CommandOSHelp : public Command
{
 public:
	CommandOSHelp() : Command("HELP", 1, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		mod_help_cmd(OperServ, u, params[0].c_str());
		return MOD_CONT;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP);
		FOREACH_MOD(I_OnOperServHelp, OnOperServHelp(u));
		notice_help(Config.s_OperServ, u, OPER_HELP_LOGGED);
	}
};

class OSHelp : public Module
{
 public:
	OSHelp(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
		this->AddCommand(OperServ, new CommandOSHelp());
	}
};

MODULE_INIT(OSHelp)
