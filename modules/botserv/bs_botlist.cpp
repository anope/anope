/* BotServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandBSBotList final
	: public Command
{
public:
	CommandBSBotList(Module *creator) : Command(creator, "botserv/botlist", 0, 1)
	{
		this->SetDesc(_("Lists available bots"));
		this->SetSyntax("[OPERONLY] [UNUSED] [VANITY]");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const bool is_admin = source.HasCommand("botserv/administration");
		auto operonly = false;
		auto unused = false;
		auto vanity = false;
		if (is_admin && !params.empty())
		{
			spacesepstream keywords(params[0]);
			for (Anope::string keyword; keywords.GetToken(keyword); )
			{
				if (keyword.equals_ci("OPERONLY"))
					operonly = true;
				if (keyword.equals_ci("UNUSED"))
					unused = true;
				if (keyword.equals_ci("VANITY"))
					vanity = true;
			}
		}

		unsigned count = 0;
		ListFormatter list(source.GetAccount());

		list.AddColumn(_("Nick")).AddColumn(_("Mask")).AddColumn(_("Real name"));

		for (const auto &[_, bi] : *BotListByNick)
		{
			if (is_admin || !bi->oper_only)
			{
				if (operonly && !bi->oper_only)
					continue;
				if (unused && bi->GetChannelCount())
					continue;
				if (vanity && bi->conf)
					continue;

				++count;
				ListFormatter::ListEntry entry;
				entry["Nick"] = bi->nick;
				entry["Mask"] = bi->GetIdent() + "@" + bi->host;
				entry["Real name"] = bi->realname;
				list.AddEntry(entry);
			}
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		if (!count)
		{
			source.Reply(_(
				"There are no bots available at this time. "
				"Ask a Services Operator to create one!"
			));
		}
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
		source.Reply(_(
			"Lists all available bots on this network."
			"\n\n"
			"If the OPERONLY, UNUSED or VANITY options are given only "
			"bots which, respectively, are oper-only, unused or were "
			"added at runtime will be displayed. If multiple options are "
			"given, all nicks matching at least one option will be "
			"displayed."
			"\n\n"
			"Note that these options are limited to \037Services Operators\037."
		));
		return true;
	}
};

class BSBotList final
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
