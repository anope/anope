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

class CommandOSModUnLoad : public Command
{
 public:
	CommandOSModUnLoad() : Command("MODUNLOAD", 1, 1, "operserv/modload")
	{
		this->SetDesc(_("Un-Load a module"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &mname = params[0];

		Module *m = ModuleManager::FindModule(mname);
		if (!m)
		{
			source.Reply(_("Module \002%s\002 isn't loaded."), mname.c_str());
			return MOD_CONT;
		}
		
		if (!m->handle || m->GetPermanent() || m->type == PROTOCOL)
		{
			source.Reply(_("Unable to remove module \002%s\002"), m->name.c_str());
			return MOD_CONT;
		}

		Log() << "Trying to unload module [" << mname << "]";

		ModuleReturn status = ModuleManager::UnloadModule(m, u);

		if (status == MOD_ERR_OK)
		{
			source.Reply(_("Module \002%s\002 unloaded"), mname.c_str());
			ircdproto->SendGlobops(operserv->Bot(), "%s unloaded module %s", u->nick.c_str(), mname.c_str());
		}
		else
			source.Reply(_("Unable to remove module \002%s\002"), mname.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002MODUNLOAD\002 \002FileName\002\n"
				" \n"
				"This command unloads the module named FileName from the modules\n"
				"directory."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "MODUNLOAD", _("MODUNLOAD \037FileName\037"));
	}
};

class OSModUnLoad : public Module
{
	CommandOSModUnLoad commandosmodunload;

 public:
	OSModUnLoad(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");
		this->SetPermanent(true);

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->AddCommand(operserv->Bot(), &commandosmodunload);
	}
};

MODULE_INIT(OSModUnLoad)
