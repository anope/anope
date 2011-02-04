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

class CommandOSModReLoad : public Command
{
 public:
	CommandOSModReLoad() : Command("MODRELOAD", 1, 1, "operserv/modload")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &mname = params[0];

		Module *m = FindModule(mname);
		if (!m)
		{
			source.Reply(_("Module \002%s\002 isn't loaded."), mname.c_str());
			return MOD_CONT;
		}

		if (!m->handle)
		{
			source.Reply(_("Unable to remove module \002%s\002"), m->name.c_str());
			return MOD_CONT;
		}

		if (m->GetPermanent())
		{
			source.Reply(_("Unable to remove module \002%s\002"));
			return MOD_CONT;
		}

		/* Unrecoverable */
		bool fatal = m->type == PROTOCOL;
		ModuleReturn status = ModuleManager::UnloadModule(m, u);

		if (status != MOD_ERR_OK)
		{
			source.Reply(_("Unable to remove module \002%s\002"), mname.c_str());
			return MOD_CONT;
		}

		status = ModuleManager::LoadModule(mname, u);
		if (status == MOD_ERR_OK)
		{
			ircdproto->SendGlobops(OperServ, "%s reloaded module %s", u->nick.c_str(), mname.c_str());
			source.Reply(_("Module \002%s\002 reloaded"), mname.c_str());

			/* If a user is loading this module, then the core databases have already been loaded
			 * so trigger the event manually
			 */
			m = FindModule(mname);
			if (m)
				m->OnPostLoadDatabases();
		}
		else
		{
			if (fatal)
				throw FatalException("Unable to reload module " + mname);
			else
				source.Reply(_("Unable to load module \002%s\002"), mname.c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002MODRELOAD\002 \002FileName\002\n"
				" \n"
				"This command reloads the module named FileName."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "MODLOAD", _("MODRELOAD \037FileName\037"));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    MODRELOAD   Reload a module"));
	}
};

class OSModReLoad : public Module
{
	CommandOSModReLoad commandosmodreload;

 public:
	OSModReLoad(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);
		this->SetPermanent(true);

		this->AddCommand(OperServ, &commandosmodreload);
	}
};

MODULE_INIT(OSModReLoad)
