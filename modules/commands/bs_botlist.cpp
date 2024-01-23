/* BotServ core functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandBSBotList
	: public Command
{
public:
	CommandBSBotList(Module *creator) : Command(creator, "botserv/botlist", 0, 0)
	{
		this->SetDesc(_("Lists available bots"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		unsigned count = 0;
		ListFormatter list(source.GetAccount());

		list.AddColumn(_("Nick")).AddColumn(_("Mask"));

		for (const auto &[_, bi] : *BotListByNick)
		{
			if (source.HasPriv("botserv/administration") || !bi->oper_only)
			{
				++count;
				ListFormatter::ListEntry entry;
				entry["Nick"] = (bi->oper_only ? "* " : "") + bi->nick;
				entry["Mask"] = bi->GetIdent() + "@" + bi->host;
				list.AddEntry(entry);
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

			for (const auto &reply : replies)
				source.Reply(reply);

			source.Reply(_("%d bots available."), count);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists all available bots on this network.\n"
				"Bots prefixed by a * are reserved for IRC Operators."));
		return true;
	}
};

class BSBotList
	: public Module
{
	CommandBSBotList commandbsbotlist;

public:
	BSBotList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandbsbotlist(this)
	{

	}
};

MODULE_INIT(BSBotList)
