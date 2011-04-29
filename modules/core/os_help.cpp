/* OperServ core functions
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
#include "operserv.h"

class CommandOSHelp : public Command
{
 public:
	CommandOSHelp() : Command("HELP", 1, 1)
	{
		this->SetDesc(_("Displays this list and give information about commands"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		mod_help_cmd(operserv->Bot(), source.u, NULL, params[0]);
		return MOD_CONT;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		source.Reply(_("%s commands:"), Config->s_OperServ.c_str());
		for (CommandMap::const_iterator it = operserv->Bot()->Commands.begin(), it_end = operserv->Bot()->Commands.end(); it != it_end; ++it)
			if (!Config->HidePrivilegedCommands || it->second->permission.empty() || u->HasCommand(it->second->permission))
				it->second->OnServHelp(source);
		source.Reply(_("\002Notice:\002 All commands sent to %s are logged!"), Config->s_OperServ.c_str());
	}
};

class OSHelp : public Module
{
	CommandOSHelp commandoshelp;

 public:
	OSHelp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->AddCommand(operserv->Bot(), &commandoshelp);
	}
};

MODULE_INIT(OSHelp)
