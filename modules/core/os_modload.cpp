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

class CommandOSModLoad : public Command
{
 public:
	CommandOSModLoad() : Command("MODLOAD", 1, 1, "operserv/modload")
	{
		this->SetDesc(_("Load a module"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &mname = params[0];

		Module *m = FindModule(mname);
		if (m)
		{
			source.Reply(_("Module \002%s\002 is already loaded."), mname.c_str());
			return MOD_CONT;
		}

		ModuleReturn status = ModuleManager::LoadModule(mname, u);
		if (status == MOD_ERR_OK)
		{
			ircdproto->SendGlobops(OperServ, "%s loaded module %s", u->nick.c_str(), mname.c_str());
			source.Reply(_("Module \002%s\002 loaded"), mname.c_str());

			/* If a user is loading this module, then the core databases have already been loaded
			 * so trigger the event manually
			 */
			m = FindModule(mname);
			if (m)
				m->OnPostLoadDatabases();
		}
		else
			source.Reply(_("Unable to load module \002%s\002"), mname.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002MODLOAD\002 \002FileName\002\n"
				" \n"
				"This command loads the module named FileName from the modules\n"
				"directory."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "MODLOAD", _("MODLOAD \037FileName\037"));
	}
};

class OSModLoad : public Module
{
	CommandOSModLoad commandosmodload;

 public:
	OSModLoad(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);
		this->SetPermanent(true);

		this->AddCommand(OperServ, &commandosmodload);
	}
};

MODULE_INIT(OSModLoad)
