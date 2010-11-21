/* BotServ core functions
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

class CommandBSBotList : public Command
{
 public:
	CommandBSBotList() : Command("BOTLIST", 0, 0)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		unsigned count = 0;

		if (BotListByNick.empty())
		{
			u->SendMessage(BotServ, BOT_BOTLIST_EMPTY);
			return MOD_CONT;
		}

		for (patricia_tree<BotInfo *>::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		{
			BotInfo *bi = *it;

			if (!bi->HasFlag(BI_PRIVATE))
			{
				if (!count)
					u->SendMessage(BotServ, BOT_BOTLIST_HEADER);
				++count;
				u->SendMessage(Config->s_BotServ, "   %-15s  (%s@%s)", bi->nick.c_str(), bi->GetIdent().c_str(), bi->host.c_str());
			}
		}

		if (u->Account()->HasCommand("botserv/botlist") && count < BotListByNick.size())
		{
			u->SendMessage(BotServ, BOT_BOTLIST_PRIVATE_HEADER);

			for (patricia_tree<BotInfo *>::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
			{
				BotInfo *bi = *it;

				if (bi->HasFlag(BI_PRIVATE))
				{
					u->SendMessage(Config->s_BotServ, "   %-15s  (%s@%s)", bi->nick.c_str(), bi->GetIdent().c_str(), bi->host.c_str());
					++count;
				}
			}
		}

		if (!count)
			u->SendMessage(BotServ, BOT_BOTLIST_EMPTY);
		else
			u->SendMessage(BotServ, BOT_BOTLIST_FOOTER, count);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(BotServ, BOT_HELP_BOTLIST);
		return true;
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(BotServ, BOT_HELP_CMD_BOTLIST);
	}
};

class BSBotList : public Module
{
	CommandBSBotList commandbsbotlist;

 public:
	BSBotList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, &commandbsbotlist);
	}
};

MODULE_INIT(BSBotList)
