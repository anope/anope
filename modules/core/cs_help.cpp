/* ChanServ core functions
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

class CommandCSHelp : public Command
{
 public:
	CommandCSHelp() : Command("HELP", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetFlag(CFLAG_STRIP_CHANNEL);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		mod_help_cmd(ChanServ, source.u, NULL, params[0]);

		return MOD_CONT;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		source.Reply(CHAN_HELP);
		for (CommandMap::const_iterator it = ChanServ->Commands.begin(); it != ChanServ->Commands.end(); ++it)
			if (!Config->HidePrivilegedCommands || it->second->permission.empty() || (u->Account() && u->Account()->HasCommand(it->second->permission)))
				it->second->OnServHelp(source);
		if (Config->CSExpire >= 86400)
			source.Reply(CHAN_HELP_EXPIRES, Config->CSExpire / 86400);
		if (u->Account() && u->Account()->IsServicesOper())
			source.Reply(CHAN_SERVADMIN_HELP);
	}
};

class CSHelp : public Module
{
	CommandCSHelp commandcshelp;

 public:
	CSHelp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcshelp);
	}
};

MODULE_INIT(CSHelp)
