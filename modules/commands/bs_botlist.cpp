/* BotServ core functions
 *
 * (C) 2003-2012 Anope Team
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
	CommandBSBotList(Module *creator) : Command(creator, "botserv/botlist", 0, 0)
	{
		this->SetDesc(_("Lists available bots"));
		this->SetSyntax("");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		unsigned count = 0;
		ListFormatter list;

		list.addColumn("Nick").addColumn("Mask");

		for (Anope::insensitive_map<BotInfo *>::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		{
			BotInfo *bi = it->second;

			if (u->HasCommand("botserv/botlist") || !bi->HasFlag(BI_PRIVATE))
			{
				++count;
				ListFormatter::ListEntry entry;
				entry["Nick"] = (bi->HasFlag(BI_PRIVATE) ? "* " : "") + bi->nick;
				entry["Mask"] = bi->GetIdent() + "@" + bi->host;
				list.addEntry(entry);
			}
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		if (!count)
			source.Reply(_("There are no bots available at this time.\n"
					"Ask a Services Operator to create one!"));
		else
		{
			source.Reply(_("Bot list:"));

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);

			source.Reply(_("%d bots available."), count);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists all available bots on this network. Bots prefixed"
				"by a * are reserved for IRC operators."));
		return true;
	}
};

class BSBotList : public Module
{
	CommandBSBotList commandbsbotlist;

 public:
	BSBotList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbsbotlist(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(BSBotList)
