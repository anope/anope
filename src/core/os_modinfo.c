/* OperServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

int showModuleCmdLoaded(CommandHash *cmdList, const char *mod_name, User *u);

class CommandOSModInfo : public Command
{
 public:
	CommandOSModInfo() : Command("MODINFO", 1, 1)
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *file = params[0].c_str();
		struct tm tm;
		char timebuf[64];
		Module *m;
		int idx = 0;

		m = findModule(file);
		if (m)
		{
			tm = *localtime(&m->created);
			strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, &tm);
			notice_lang(s_OperServ, u, OPER_MODULE_INFO_LIST, m->name.c_str(), !m->version.empty() ? m->version.c_str() : "?", !m->author.empty() ? m->author.c_str() : "?", timebuf);
			for (idx = 0; idx < MAX_CMD_HASH; ++idx)
			{
				showModuleCmdLoaded(HOSTSERV[idx], m->name.c_str(), u);
				showModuleCmdLoaded(OPERSERV[idx], m->name.c_str(), u);
				showModuleCmdLoaded(NICKSERV[idx], m->name.c_str(), u);
				showModuleCmdLoaded(CHANSERV[idx], m->name.c_str(), u);
				showModuleCmdLoaded(BOTSERV[idx], m->name.c_str(), u);
				showModuleCmdLoaded(MEMOSERV[idx], m->name.c_str(), u);
			}
		}
		else
			notice_lang(s_OperServ, u, OPER_MODULE_NO_INFO, file);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_MODINFO);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "MODINFO", OPER_MODULE_INFO_SYNTAX);
	}
};

class OSModInfo : public Module
{
 public:
	OSModInfo(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(OPERSERV, new CommandOSModInfo());
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_MODINFO);
	}
};

int showModuleCmdLoaded(CommandHash *cmdList, const char *mod_name, User *u)
{
	Command *c;
	CommandHash *current;
	int display = 0;

	for (current = cmdList; current; current = current->next)
	{
		for (c = current->c; c; c = c->next)
		{
			if (c->mod_name && !stricmp(c->mod_name, mod_name))
			{
				notice_lang(s_OperServ, u, OPER_MODULE_CMD_LIST, c->service, c->name.c_str());
				++display;
			}
		}
	}
	return display;
}

MODULE_INIT("os_modinfo", OSModInfo)
