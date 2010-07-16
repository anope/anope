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

static int showModuleCmdLoaded(BotInfo *bi, const ci::string &mod_name, User *u);

class CommandOSModInfo : public Command
{
 public:
	CommandOSModInfo() : Command("MODINFO", 1, 1)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const std::string file = params[0].c_str();
		struct tm tm;
		char timebuf[64];

		Module *m = FindModule(file.c_str());
		if (m)
		{
			tm = *localtime(&m->created);
			strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_DATE_TIME_FORMAT, &tm);
			notice_lang(Config.s_OperServ, u, OPER_MODULE_INFO_LIST, m->name.c_str(), !m->version.empty() ? m->version.c_str() : "?", !m->author.empty() ? m->author.c_str() : "?", timebuf);

			showModuleCmdLoaded(HostServ, m->name.c_str(), u);
			showModuleCmdLoaded(OperServ, m->name.c_str(), u);
			showModuleCmdLoaded(NickServ, m->name.c_str(), u);
			showModuleCmdLoaded(ChanServ, m->name.c_str(), u);
			showModuleCmdLoaded(BotServ, m->name.c_str(), u);
			showModuleCmdLoaded(MemoServ, m->name.c_str(), u);
		}
		else
			notice_lang(Config.s_OperServ, u, OPER_MODULE_NO_INFO, file.c_str());

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_MODINFO);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "MODINFO", OPER_MODULE_INFO_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_MODINFO);
	}
};

class OSModInfo : public Module
{
 public:
	OSModInfo(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSModInfo());
	}
};

static int showModuleCmdLoaded(BotInfo *bi, const ci::string &mod_name, User *u)
{
	if (!bi)
		return 0;

	int display = 0;

	for (std::map<ci::string, Command *>::iterator it = bi->Commands.begin(), it_end = bi->Commands.end(); it != it_end; ++it)
	{
		Command *c = it->second;

		if (c->module && c->module->name == mod_name)
		{
			notice_lang(Config.s_OperServ, u, OPER_MODULE_CMD_LIST, c->service, c->name.c_str());
			++display;
		}
	}
	return display;
}

MODULE_INIT(OSModInfo)
