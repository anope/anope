/* OperServ core functions
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

static int showModuleCmdLoaded(BotInfo *bi, const Anope::string &mod_name, User *u);

class CommandOSModInfo : public Command
{
 public:
	CommandOSModInfo() : Command("MODINFO", 1, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string file = params[0];
		struct tm tm;
		char timebuf[64];

		Module *m = FindModule(file);
		if (m)
		{
			tm = *localtime(&m->created);
			strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, &tm);
			notice_lang(Config->s_OperServ, u, OPER_MODULE_INFO_LIST, m->name.c_str(), !m->version.empty() ? m->version.c_str() : "?", !m->author.empty() ? m->author.c_str() : "?", timebuf);

			showModuleCmdLoaded(HostServ, m->name, u);
			showModuleCmdLoaded(OperServ, m->name, u);
			showModuleCmdLoaded(NickServ, m->name, u);
			showModuleCmdLoaded(ChanServ, m->name, u);
			showModuleCmdLoaded(BotServ, m->name, u);
			showModuleCmdLoaded(MemoServ, m->name, u);
		}
		else
			notice_lang(Config->s_OperServ, u, OPER_MODULE_NO_INFO, file.c_str());

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_OperServ, u, OPER_HELP_MODINFO);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_OperServ, u, "MODINFO", OPER_MODULE_INFO_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_OperServ, u, OPER_HELP_CMD_MODINFO);
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

static int showModuleCmdLoaded(BotInfo *bi, const Anope::string &mod_name, User *u)
{
	if (!bi)
		return 0;

	int display = 0;

	for (CommandMap::iterator it = bi->Commands.begin(), it_end = bi->Commands.end(); it != it_end; ++it)
	{
		Command *c = it->second;

		if (c->module && c->module->name.equals_ci(mod_name) && c->service)
		{
			notice_lang(Config->s_OperServ, u, OPER_MODULE_CMD_LIST, c->service->nick.c_str(), c->name.c_str());
			++display;
		}
	}
	return display;
}

MODULE_INIT(OSModInfo)
