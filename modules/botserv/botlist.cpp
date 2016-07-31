/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
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

		for (BotInfo *bi : Serialize::GetObjects<BotInfo *>())
		{
			if (source.HasPriv("botserv/administration") || !bi->GetOperOnly())
			{
				++count;
				ListFormatter::ListEntry entry;
				entry["Nick"] = (bi->GetOperOnly() ? "* " : "") + bi->GetNick();
				entry["Mask"] = bi->GetUser() + "@" + bi->GetHost();
				list.AddEntry(entry);
			}
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		if (!count)
		{
			source.Reply(_("There are no bots available"));
			return;
		}

		source.Reply(_("Bot list:"));

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		source.Reply(_("{0} bots available."), count);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		ServiceBot *bi;
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
