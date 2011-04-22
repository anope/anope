/* Global core functions
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
#include "global.h"

class CommandGLHelp : public Command
{
 public:
	CommandGLHelp() : Command("HELP", 1, 1)
	{
		this->SetDesc(_("Displays this list and give information about commands"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		mod_help_cmd(global->Bot(), source.u, NULL, params[0]);
		return MOD_CONT;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		source.Reply(_("%s commands:"), Config->s_Global.c_str());
		for (CommandMap::const_iterator it = global->Bot()->Commands.begin(), it_end = global->Bot()->Commands.end(); it != it_end; ++it)
			if (!Config->HidePrivilegedCommands || it->second->permission.empty() || u->HasCommand(it->second->permission))
				it->second->OnServHelp(source);
	}
};

class GLHelp : public Module
{
	CommandGLHelp commandoshelp;

 public:
	GLHelp(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		if (!global)
			throw ModuleException("Global is not loaded!");

		this->AddCommand(global->Bot(), &commandoshelp);
	}
};

MODULE_INIT(GLHelp)
