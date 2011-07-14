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
 public:
	CommandOSModInfo(Module *creator) : Command(creator, "operserv/modinfo", 1, 1)
	{
		this->SetDesc(_("Info about a loaded module"));
		this->SetSyntax(_("\037modname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &file = params[0];

		Module *m = ModuleManager::FindModule(file);
		if (m)
		{
			source.Reply(_("Module: \002%s\002 Version: \002%s\002 Author: \002%s\002 loaded: \002%s\002"), m->name.c_str(), !m->version.empty() ? m->version.c_str() : "?", !m->author.empty() ? m->author.c_str() : "?", do_strftime(m->created).c_str());

			std::vector<Anope::string> services = ModuleManager::GetServiceKeys();
			for (unsigned i = 0; i < services.size(); ++i)
			{
				Service *s = ModuleManager::GetService(services[i]);
				if (!s || s->owner != m)
					continue;
				source.Reply(_("   Providing service: \002%s\002"), s->name.c_str());

				for (botinfo_map::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
				{
					BotInfo *bi = it->second;

					for (command_map::const_iterator cit = bi->commands.begin(), cit_end = bi->commands.end(); cit != cit_end; ++cit)
					{
						if (cit->second != s->name)
							continue;
						source.Reply(_("   Command \002%s\002 on \002%s\002 is linked to \002%s\002"), cit->first.c_str(), bi->nick.c_str(), s->name.c_str());
					}
				}
			}
		}
		else
			source.Reply(_("No information about module \002%s\002 is available"), file.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("This command lists information about the specified loaded module"));
		return true;
	}
};

class CommandOSModList : public Command
{
 public:
	CommandOSModList(Module *creator) : Command(creator, "operserv/modlist", 0, 1, "operserv/modlist")
	{
		this->SetDesc(_("List loaded modules"));
		this->SetSyntax(_("[Core|3rd|protocol|encryption|supported]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &param = !params.empty() ? params[0] : "";

		int count = 0;
		int showCore = 0;
		int showThird = 1;
		int showProto = 1;
		int showEnc = 1;
		int showSupported = 1;
		int showDB = 1;

		char core[] = "Core";
		char third[] = "3rd";
		char proto[] = "Protocol";
		char enc[] = "Encryption";
		char supported[] = "Supported";
		char db[] = "Database";

		if (!param.empty())
		{
			if (param.equals_ci(core))
			{
				showCore = 1;
				showThird = 0;
				showProto = 0;
				showEnc = 0;
				showSupported = 0;
				showDB = 0;
			}
			else if (param.equals_ci(third))
			{
				showCore = 0;
				showThird = 1;
				showSupported = 0;
				showProto = 0;
				showEnc = 0;
				showDB = 0;
			}
			else if (param.equals_ci(proto))
			{
				showCore = 0;
				showThird = 0;
				showProto = 1;
				showEnc = 0;
				showSupported = 0;
				showDB = 0;
			}
			else if (param.equals_ci(supported))
			{
				showCore = 0;
				showThird = 0;
				showProto = 0;
				showSupported = 1;
				showEnc = 0;
				showDB = 0;
			}
			else if (param.equals_ci(enc))
			{
				showCore = 0;
				showThird = 0;
				showProto = 0;
				showSupported = 0;
				showEnc = 1;
				showDB = 0;
			}
			else if (param.equals_ci(db))
			{
				showCore = 0;
				showThird = 0;
				showProto = 0;
				showSupported = 0;
				showEnc = 0;
				showDB = 1;
			}
		}

		source.Reply(_("Current Module list:"));

		for (std::list<Module *>::iterator it = Modules.begin(), it_end = Modules.end(); it != it_end; ++it)
		{
			Module *m = *it;

			switch (m->type)
			{
				case CORE:
					if (showCore)
					{
						source.Reply(_("Module: \002%s\002 [%s] [%s]"), m->name.c_str(), m->version.c_str(), core);
						++count;
					}
					break;
				case THIRD:
					if (showThird)
					{
						source.Reply(_("Module: \002%s\002 [%s] [%s]"), m->name.c_str(), m->version.c_str(), third);
						++count;
					}
					break;
				case PROTOCOL:
					if (showProto)
					{
						source.Reply(_("Module: \002%s\002 [%s] [%s]"), m->name.c_str(), m->version.c_str(), proto);
						++count;
					}
					break;
				case SUPPORTED:
					if (showSupported)
					{
						source.Reply(_("Module: \002%s\002 [%s] [%s]"), m->name.c_str(), m->version.c_str(), supported);
						++count;
					}
					break;
				case ENCRYPTION:
					if (showEnc)
					{
						source.Reply(_("Module: \002%s\002 [%s] [%s]"), m->name.c_str(), m->version.c_str(), enc);
						++count;
					}
					break;
				case DATABASE:
					if (showDB)
					{
						source.Reply(_("Module: \002%s\002 [%s] [%s]"), m->name.c_str(), m->version.c_str(), db);
						++count;
					}
					break;
				default:
					break;
			}
		}
		if (!count)
			source.Reply(_("No modules currently loaded"));
		else
			source.Reply(_("%d Modules loaded."), count);

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists all currently loaded modules."));
		return true;
	}
};

class OSModInfo : public Module
{
	CommandOSModInfo commandosmodinfo;
	CommandOSModList commandosmodlist;

 public:
	OSModInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandosmodinfo(this), commandosmodlist(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandosmodinfo);
		ModuleManager::RegisterService(&commandosmodlist);
	}
};

MODULE_INIT(OSModInfo)
