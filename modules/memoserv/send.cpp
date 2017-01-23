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
#include "modules/memoserv.h"

class CommandMSSend : public Command
{
 public:
	CommandMSSend(Module *creator) : Command(creator, "memoserv/send", 2, 2)
	{
		this->SetDesc(_("Send a memo to a nick or channel"));
		this->SetSyntax(_("{\037user\037 | \037channel\037} \037memo-text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!MemoServ::service)
			return;

		const Anope::string &nick = params[0];
		const Anope::string &text = params[1];

		if (Anope::ReadOnly && !source.IsOper())
		{
			source.Reply(_("Sorry, memo sending is temporarily disabled."));
			return;
		}

		if (source.GetAccount()->IsUnconfirmed())
		{
			source.Reply(_("You must confirm your account before you may send a memo."));
			return;
		}

		MemoServ::MemoServService::MemoResult result = MemoServ::service->Send(source.GetNick(), nick, text);
		if (result == MemoServ::MemoServService::MEMO_SUCCESS)
		{
			source.Reply(_("Memo sent to \002%s\002."), nick.c_str());
			logger.Command(LogType::COMMAND, source, _("{source} used {command} to send a memo to {0}"), nick);
		}
		else if (result == MemoServ::MemoServService::MEMO_INVALID_TARGET)
		{
			source.Reply(_("\002{0}\002 is not a registered unforbidden nick or channel."), nick);
		}
		else if (result == MemoServ::MemoServService::MEMO_TOO_FAST)
		{
			source.Reply(_("Please wait \002{0}\002 seconds before using the \002{1}\002 command again."), Config->GetModule("memoserv/main")->Get<time_t>("senddelay"), source.command);
		}
		else if (result == MemoServ::MemoServService::MEMO_TARGET_FULL)
		{
			source.Reply(_("Sorry, \002{0}\002 currently has too many memos and cannot receive more."), nick);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Sends the named \037user\037 or \037channel\037 a memo containing \037memo-text\037."
		               " The recipient will receive a notice that they have a new memo."
		               " The target user or channel must be registered."));
		return true;
	}
};

class MSSend : public Module
{
	CommandMSSend commandmssend;

 public:
	MSSend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmssend(this)
	{

		if (!MemoServ::service)
			throw ModuleException("No MemoServ!");
	}
};

MODULE_INIT(MSSend)
