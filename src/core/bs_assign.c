/* BotServ core functions
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

class CommandBSAssign : public Command
{
 public:
	CommandBSAssign() : Command("ASSIGN", 2, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *nick = params[1].c_str();
		BotInfo *bi;
		ChannelInfo *ci;

		if (readonly)
		{
			notice_lang(Config.s_BotServ, u, BOT_ASSIGN_READONLY);
			return MOD_CONT;
		}

		if (!(bi = findbot(nick)))
		{
			notice_lang(Config.s_BotServ, u, BOT_DOES_NOT_EXIST, nick);
			return MOD_CONT;
		}

		ci = cs_findchan(chan);

		if (ci->botflags.HasFlag(BS_NOBOT) || (!check_access(u, ci, CA_ASSIGN) && !u->nc->HasPriv("botserv/administration")))
		{
			notice_lang(Config.s_BotServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (bi->HasFlag(BI_PRIVATE) && !u->nc->HasCommand("botserv/assign/private"))
		{
			notice_lang(Config.s_BotServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if ((ci->bi) && (stricmp(ci->bi->nick, nick) == 0))
		{
			notice_lang(Config.s_BotServ, u, BOT_ASSIGN_ALREADY, ci->bi->nick, chan);
			return MOD_CONT;
		}

		bi->Assign(u, ci);
		notice_lang(Config.s_BotServ, u, BOT_ASSIGN_ASSIGNED, bi->nick, ci->name);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_BotServ, u, BOT_HELP_ASSIGN);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_BotServ, u, "ASSIGN", BOT_ASSIGN_SYNTAX);
	}
};

class BSAssign : public Module
{
 public:
	BSAssign(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(BOTSERV, new CommandBSAssign);

		ModuleManager::Attach(I_OnBotServHelp, this);
	}
	void OnBotServHelp(User *u)
	{
		notice_lang(Config.s_BotServ, u, BOT_HELP_CMD_ASSIGN);
	}
};

MODULE_INIT(BSAssign)
