/* BotServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandBSBotList : public Command
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

		for (botinfo_map::const_iterator it = BotListByNick->begin(), it_end = BotListByNick->end(); it != it_end; ++it)
		{
			BotInfo *bi = it->second;

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
			source.Reply(_("There are no bots available at this time."));
		else
		{
			source.Reply(_("Bot list:"));

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);

			source.Reply(_("{0} bots available."), count);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		BotInfo *bi;
		Anope::string name;
		Command::FindCommandFromService("botserv/assign", bi, name);
		if (!bi)
			return false;

		source.Reply(_("Lists all available bots. You may use the \002{msg}{service} {assign}\002 command to assign a bot to your channel."
		               "The bot names are vanity; they all proviate the same commands and features."),
				"msg"_kw = Config->StrictPrivmsg, "service"_kw = bi->nick, "assign"_kw = name);
		if (source.HasPriv("botserv/administration"))
			source.Reply(_("Bots prefixed by a * are reserved for Services Operators with the privilege \002{0}\002."),
		                       "botserv/administration");
		source.Reply(_("\n"
		               "Example:\n"
		               "         {command} BOTLIST"),
		               "command"_kw = source.command);
		return true;
	}
};

class BSBotList : public Module
{
	CommandBSBotList commandbsbotlist;

 public:
	BSBotList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandbsbotlist(this)
	{

	}
};

MODULE_INIT(BSBotList)
