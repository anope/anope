/* HostServ core functions
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

class CommandHSHelp : public Command
{
 public:
	CommandHSHelp() : Command("HELP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		mod_help_cmd(s_HostServ, u, HOSTSERV, params.size() > 0 ? params[0].c_str() : NULL);
		return MOD_CONT;
	}

	void OnSyntaxError(User *u)
	{
		notice_help(s_HostServ, u, HOST_HELP, s_HostServ);
		moduleDisplayHelp(s_HostServ, u);
	}
};

class HSHelp : public Module
{
 public:
	HSHelp(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(HOSTSERV, new CommandHSHelp(), MOD_UNIQUE);
	}
};

MODULE_INIT("hs_help", HSHelp)
