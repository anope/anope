/* MemoServ core functions
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

class CommandMSHelp : public Command
{
 public:
	CommandMSHelp() : Command("HELP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		mod_help_cmd(MemoServ, source.u, NULL, params[0]);
		return MOD_CONT;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		source.Reply(_("\002%S\002 is a utility allowing IRC users to send short\n"
				"messages to other IRC users, whether they are online at\n"
				"the time or not, or to channels(*). Both the sender's\n"
				"nickname and the target nickname or channel must be\n"
				"registered in order to send a memo.\n"
				"%S's commands include:"));
		for (CommandMap::const_iterator it = MemoServ->Commands.begin(), it_end = MemoServ->Commands.end(); it != it_end; ++it)
			if (!Config->HidePrivilegedCommands || it->second->permission.empty() || (u->Account() && u->Account()->HasCommand(it->second->permission)))
				it->second->OnServHelp(source);
		source.Reply(_("Type \002%R%S HELP \037command\037\002 for help on any of the\n"
				"above commands.\n"
				"(*) By default, any user with at least level 10 access on a\n"
				"    channel can read that channel's memos. This can be\n"
				"    changed with the %s \002LEVELS\002 command."), Config->s_ChanServ.c_str());
	}
};

class MSHelp : public Module
{
	CommandMSHelp commandmshelp;

 public:
	MSHelp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, &commandmshelp);
	}
};

MODULE_INIT(MSHelp)
