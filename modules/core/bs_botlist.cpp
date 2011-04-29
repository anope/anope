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
#include "botserv.h"

class CommandBSBotList : public Command
{
 public:
	CommandBSBotList() : Command("BOTLIST", 0, 0)
	{
		this->SetDesc(_("Lists available bots"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		unsigned count = 0;

		for (Anope::insensitive_map<BotInfo *>::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		{
			BotInfo *bi = it->second;

			if (!bi->HasFlag(BI_PRIVATE))
			{
				if (!count)
					source.Reply(_("Bot list:"));
				++count;
				source.Reply("   %-15s  (%s@%s)", bi->nick.c_str(), bi->GetIdent().c_str(), bi->host.c_str());
			}
		}

		if (u->HasCommand("botserv/botlist") && count < BotListByNick.size())
		{
			source.Reply(_("Bots reserved to IRC operators:"));

			for (Anope::insensitive_map<BotInfo *>::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
			{
				BotInfo *bi = it->second;

				if (bi->HasFlag(BI_PRIVATE))
				{
					source.Reply("   %-15s  (%s@%s)", bi->nick.c_str(), bi->GetIdent().c_str(), bi->host.c_str());
					++count;
				}
			}
		}

		if (!count)
			source.Reply(_("There are no bots available at this time.\n"
					"Ask a Services Operator to create one!"));
		else
			source.Reply(_("%d bots available."), count);

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002BOTLIST\002\n"
				" \n"
				"Lists all available bots on this network."));
		return true;
	}
};

class BSBotList : public Module
{
	CommandBSBotList commandbsbotlist;

 public:
	BSBotList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!botserv)
			throw ModuleException("BotServ is not loaded!");

		this->AddCommand(botserv->Bot(), &commandbsbotlist);
	}
};

MODULE_INIT(BSBotList)
