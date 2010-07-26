/* OperServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandOSGlobal : public Command
{
 public:
	CommandOSGlobal() : Command("GLOBAL", 1, 1, "operserv/global")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string msg = params[0];

		if (Config.WallOSGlobal)
			ircdproto->SendGlobops(OperServ, "\2%s\2 just used GLOBAL command.", u->nick.c_str());
		oper_global(u->nick, "%s", msg.c_str());
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_GLOBAL, Config.s_GlobalNoticer.c_str());
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "GLOBAL", OPER_GLOBAL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_GLOBAL);
	}
};

class OSGlobal : public Module
{
 public:
	OSGlobal(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSGlobal());
	}
};

MODULE_INIT(OSGlobal)
