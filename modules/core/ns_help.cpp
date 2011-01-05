/* NickServ core functions
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

class CommandNSHelp : public Command
{
 public:
	CommandNSHelp() : Command("HELP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		mod_help_cmd(NickServ, source.u, NULL, params[0]);
		return MOD_CONT;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		source.Reply(NICK_HELP);
		for (CommandMap::const_iterator it = NickServ->Commands.begin(), it_end = NickServ->Commands.end(); it != it_end; ++it)
			if (!Config->HidePrivilegedCommands || it->second->permission.empty() || (u->Account() && u->Account()->HasCommand(it->second->permission)))
				it->second->OnServHelp(source);
		if (u->Account() && u->Account()->IsServicesOper())
			source.Reply(NICK_SERVADMIN_HELP);
		if (Config->NSExpire >= 86400)
			source.Reply(NICK_HELP_EXPIRES, Config->NSExpire / 86400);
		source.Reply(NICK_HELP_FOOTER);
	}
};

class NSHelp : public Module
{
	CommandNSHelp commandnshelp;

 public:
	NSHelp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnshelp);
	}
};

MODULE_INIT(NSHelp)
