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

class CommandBSBotList : public Command
{
 public:
	CommandBSBotList() : Command("BOTLIST", 0, 0)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		int i, count = 0;
		BotInfo *bi;

		if (!nbots) {
			notice_lang(s_BotServ, u, BOT_BOTLIST_EMPTY);
			return MOD_CONT;
		}

		for (i = 0; i < 256; i++) {
			for (bi = botlists[i]; bi; bi = bi->next) {
				if (!(bi->flags & BI_PRIVATE)) {
					if (!count)
						notice_lang(s_BotServ, u, BOT_BOTLIST_HEADER);
					count++;
					u->SendMessage(s_BotServ, "   %-15s  (%s@%s)", bi->nick, bi->user, bi->host);
				}
			}
		}

		if (u->nc->HasCommand("botserv/botlist") && count < nbots) {
			notice_lang(s_BotServ, u, BOT_BOTLIST_PRIVATE_HEADER);

			for (i = 0; i < 256; i++) {
				for (bi = botlists[i]; bi; bi = bi->next) {
					if (bi->flags & BI_PRIVATE) {
						u->SendMessage(s_BotServ, "   %-15s  (%s@%s)", bi->nick, bi->user, bi->host);
						count++;
					}
				}
			}
		}

		if (!count)
			notice_lang(s_BotServ, u, BOT_BOTLIST_EMPTY);
		else
			notice_lang(s_BotServ, u, BOT_BOTLIST_FOOTER, count);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_BotServ, u, BOT_HELP_BOTLIST);
		return true;
	}
};

class BSBotList : public Module
{
 public:
	BSBotList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(BOTSERV, new CommandBSBotList(), MOD_UNIQUE);
	}
	void BotServHelp(User *u)
	{
		notice_lang(s_BotServ, u, BOT_HELP_CMD_BOTLIST);
	}
};

MODULE_INIT("bs_botlist", BSBotList)
