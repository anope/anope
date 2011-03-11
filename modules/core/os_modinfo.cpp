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


class CommandOSModInfo : public Command
{
	int showModuleCmdLoaded(BotInfo *bi, const Anope::string &mod_name, CommandSource &source)
	{
		if (!bi)
			return 0;

		int display = 0;

		for (CommandMap::iterator it = bi->Commands.begin(), it_end = bi->Commands.end(); it != it_end; ++it)
		{
			Command *c = it->second;

			if (c->module && c->module->name.equals_ci(mod_name) && c->service)
			{
				source.Reply(_("Providing command: %\002%s %s\002"), c->service->nick.c_str(), c->name.c_str());
				++display;
			}
		}

		return display;
	}

 public:
	CommandOSModInfo() : Command("MODINFO", 1, 1)
	{
		this->SetDesc(_("Info about a loaded module"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &file = params[0];

		Module *m = FindModule(file);
		if (m)
		{
			source.Reply(_("Module: \002%s\002 Version: \002%s\002 Author: \002%s\002 loaded: \002%s\002"), m->name.c_str(), !m->version.empty() ? m->version.c_str() : "?", !m->author.empty() ? m->author.c_str() : "?", do_strftime(m->created).c_str());

			showModuleCmdLoaded(HostServ, m->name, source);
			showModuleCmdLoaded(OperServ, m->name, source);
			showModuleCmdLoaded(NickServ, m->name, source);
			showModuleCmdLoaded(ChanServ, m->name, source);
			showModuleCmdLoaded(BotServ, m->name, source);
			showModuleCmdLoaded(MemoServ, m->name, source);
		}
		else
			source.Reply(_("No information about module \002%s\002 is available"), file.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002MODINFO\002 \002FileName\002\n"
				" \n"
				"This command lists information about the specified loaded module"));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "MODINFO", _("MODINFO \037FileName\037"));
	}
};

class OSModInfo : public Module
{
	CommandOSModInfo commandosmodinfo;

 public:
	OSModInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosmodinfo);
	}
};

MODULE_INIT(OSModInfo)
