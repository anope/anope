/* BotServ core functions
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

class CommandBSBotList : public Command
{
 public:
	CommandBSBotList() : Command("BOTLIST", 0, 0)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		unsigned count = 0;

		if (BotListByNick.empty())
		{
			source.Reply(BOT_BOTLIST_EMPTY);
			return MOD_CONT;
		}

		for (patricia_tree<BotInfo *, ci::ci_char_traits>::iterator it(BotListByNick); it.next();)
		{
			BotInfo *bi = *it;

			if (!bi->HasFlag(BI_PRIVATE))
			{
				if (!count)
					source.Reply(BOT_BOTLIST_HEADER);
				++count;
				source.Reply("   %-15s  (%s@%s)", bi->nick.c_str(), bi->GetIdent().c_str(), bi->host.c_str());
			}
		}

		if (u->Account()->HasCommand("botserv/botlist") && count < BotListByNick.size())
		{
			source.Reply(BOT_BOTLIST_PRIVATE_HEADER);

			for (patricia_tree<BotInfo *, ci::ci_char_traits>::iterator it(BotListByNick); it.next();)
			{
				BotInfo *bi = *it;

				if (bi->HasFlag(BI_PRIVATE))
				{
					source.Reply("   %-15s  (%s@%s)", bi->nick.c_str(), bi->GetIdent().c_str(), bi->host.c_str());
					++count;
				}
			}
		}

		if (!count)
			source.Reply(BOT_BOTLIST_EMPTY);
		else
			source.Reply(BOT_BOTLIST_FOOTER, count);

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(BOT_HELP_BOTLIST);
		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(BOT_HELP_CMD_BOTLIST);
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
