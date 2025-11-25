// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"
#include "modules/memoserv/service.h"

class CommandMSSend final
	: public Command
{
public:
	CommandMSSend(Module *creator) : Command(creator, "memoserv/send", 2, 2)
	{
		this->SetDesc(_("Send a memo to a nick or channel"));
		this->SetSyntax(_("{\037nick\037 | \037channel\037} \037memo-text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!MemoServ::service)
		{
			source.Reply(TRY_AGAIN_LATER, source.command.nobreak().c_str());
			return;
		}

		const Anope::string &nick = params[0];
		const Anope::string &text = params[1];

		if (Anope::ReadOnly && !source.IsOper())
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		if (source.GetAccount()->HasExt("UNCONFIRMED"))
		{
			source.Reply(_("You must confirm your account before you may send a memo."));
			return;
		}

		const auto result = MemoServ::service->Send(source.GetNick(), nick, text);
		if (result == MemoServ::MEMO_SUCCESS)
		{
			source.Reply(_("Memo sent to \002%s\002."), nick.c_str());
			Log(LOG_COMMAND, source, this) << "to send a memo to " << nick;
		}
		else if (result == MemoServ::MEMO_INVALID_TARGET)
			source.Reply(_("\002%s\002 is not a registered unforbidden nick or channel."), nick.c_str());
		else if (result == MemoServ::MEMO_TOO_FAST)
		{
			auto lastmemosend = source.GetUser() ? source.GetUser()->lastmemosend : 0;
			auto waitperiod = (lastmemosend + Config->GetModule("memoserv").Get<unsigned long>("senddelay")) -  Anope::CurTime;
			source.Reply(_("Please wait %s before using the %s command again."), Anope::Duration(waitperiod, source.GetAccount()).c_str(), source.command.nobreak().c_str());
		}
		else if (result == MemoServ::MEMO_TARGET_FULL)
			source.Reply(_("%s currently has too many memos and cannot receive more."), nick.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Sends the named \037nick\037 or \037channel\037 a memo containing "
			"\037memo-text\037. When sending to a nickname, the recipient will "
			"receive a notice that they have a new memo. The target "
			"nickname/channel must be registered."
		));
		return true;
	}
};

class MSSend final
	: public Module
{
	CommandMSSend commandmssend;

public:
	MSSend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandmssend(this)
	{

		if (!MemoServ::service)
			throw ModuleException("No MemoServ!");
	}
};

MODULE_INIT(MSSend)
