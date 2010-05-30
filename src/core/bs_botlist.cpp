/* BotServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class CommandBSBotList : public Command
{
 public:
	CommandBSBotList() : Command("BOTLIST", 0, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		unsigned count = 0;

		if (BotListByNick.empty())
		{
			notice_lang(Config.s_BotServ, u, BOT_BOTLIST_EMPTY);
			return MOD_CONT;
		}

		for (botinfo_map::const_iterator it = BotListByNick.begin(); it != BotListByNick.end(); ++it)
		{
			BotInfo *bi = it->second;
			
			if (!bi->HasFlag(BI_PRIVATE))
			{
				if (!count)
					notice_lang(Config.s_BotServ, u, BOT_BOTLIST_HEADER);
				count++;
				u->SendMessage(Config.s_BotServ, "   %-15s  (%s@%s)", bi->nick.c_str(), bi->user.c_str(), bi->host.c_str());
			}
		}

		if (u->Account()->HasCommand("botserv/botlist") && count < BotListByNick.size())
		{
			notice_lang(Config.s_BotServ, u, BOT_BOTLIST_PRIVATE_HEADER);

			for (botinfo_map::const_iterator it = BotListByNick.begin(); it != BotListByNick.end(); ++it)
			{
				BotInfo *bi = it->second;
				
				if (bi->HasFlag(BI_PRIVATE))
				{
					u->SendMessage(Config.s_BotServ, "   %-15s  (%s@%s)", bi->nick.c_str(), bi->user.c_str(), bi->host.c_str());
					count++;
				}
			}
		}

		if (!count)
			notice_lang(Config.s_BotServ, u, BOT_BOTLIST_EMPTY);
		else
			notice_lang(Config.s_BotServ, u, BOT_BOTLIST_FOOTER, count);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_BotServ, u, BOT_HELP_BOTLIST);
		return true;
	}
};

class BSBotList : public Module
{
 public:
	BSBotList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
		this->AddCommand(BotServ, new CommandBSBotList());

		ModuleManager::Attach(I_OnBotServHelp, this);
	}
	void OnBotServHelp(User *u)
	{
		notice_lang(Config.s_BotServ, u, BOT_HELP_CMD_BOTLIST);
	}
};

MODULE_INIT(BSBotList)
