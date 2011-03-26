/* BotServ core functions
 *
 * (C) 2003-2011 Anope Team
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
		this->SetDesc(_("Displays this list and give information about commands"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		mod_help_cmd(BotServ, source.u, NULL, params[0]);
		return MOD_CONT;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		// Abuse syntax error to display general list help.
		User *u = source.u;
		source.Reply(_("\002%s\002 allows you to have a bot on your own channel.\n"
				"It has been created for users that can't host or\n"
				"configure a bot, or for use on networks that don't\n"
				"allow user bots. Available commands are listed \n"
				"below; to use them, type \002%s%s \037command\037\002. For\n"
				"more information on a specific command, type\n"
				"\002%s%s HELP \037command\037\002."),
				BotServ->nick.c_str(), Config->UseStrictPrivMsgString.c_str(), BotServ->nick.c_str(),
				Config->UseStrictPrivMsgString.c_str(), BotServ->nick.c_str());
		for (CommandMap::const_iterator it = BotServ->Commands.begin(), it_end = BotServ->Commands.end(); it != it_end; ++it)
			if (!Config->HidePrivilegedCommands || it->second->permission.empty() || u->HasCommand(it->second->permission))
				it->second->OnServHelp(source);
		source.Reply(_("Bot will join a channel whenever there is at least\n"
				"\002%d\002 user(s) on it. Additionally, all %s commands\n"
				"can be used if fantasy is enabled by prefixing the command\n"
				"name with a %c."), Config->BSMinUsers, Config->s_ChanServ.c_str(), Config->BSFantasyCharacter[0]);
	}
};

class BSHelp : public Module
{
	CommandBSHelp commandbshelp;

 public:
	BSHelp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, &commandbshelp);
	}
};

MODULE_INIT(BSHelp)
