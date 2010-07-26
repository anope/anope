/* BotServ core functions
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

class CommandBSHelp : public Command
{
 public:
	CommandBSHelp() : Command("HELP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetFlag(CFLAG_STRIP_CHANNEL);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		mod_help_cmd(findbot(Config.s_BotServ), u, params[0]);
		return MOD_CONT;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		// Abuse syntax error to display general list help.
		notice_help(Config.s_BotServ, u, BOT_HELP);
		for (CommandMap::const_iterator it = BotServ->Commands.begin(), it_end = BotServ->Commands.end(); it != it_end; ++it)
			if (!Config.HidePrivilegedCommands || it->second->permission.empty() || (u->Account() && u->Account()->HasCommand(it->second->permission)))
				it->second->OnServHelp(u);
		notice_help(Config.s_BotServ, u, BOT_HELP_FOOTER, Config.BSMinUsers);
	}
};

class BSHelp : public Module
{
 public:
	BSHelp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, new CommandBSHelp());
	}
};

MODULE_INIT(BSHelp)
