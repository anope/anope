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

class CommandMSRSend final
	: public Command
{
public:
	CommandMSRSend(Module *creator) : Command(creator, "memoserv/rsend", 2, 2)
	{
		this->SetDesc(_("Sends a memo and requests a read receipt"));
		this->SetSyntax(_("{\037nick\037 | \037channel\037} \037memo-text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!MemoServ::service)
		{
			source.Reply(TRY_AGAIN_LATER, source.command.nobreak().c_str());
			return;
		}

		if (Anope::ReadOnly && !source.IsOper())
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const Anope::string &nick = params[0];
		const Anope::string &text = params[1];
		const NickAlias *na = NULL;

		/* prevent user from rsend to themselves */
		if ((na = NickAlias::Find(nick)) && na->nc == source.GetAccount())
		{
			source.Reply(_("You can not request a receipt when sending a memo to yourself."));
			return;
		}

		if (Config->GetModule(this->owner).Get<bool>("operonly") && !source.IsServicesOper())
			source.Reply(ACCESS_DENIED);
		else
		{
			const auto result = MemoServ::service->Send(source.GetNick(), nick, text);
			if (result == MemoServ::MEMO_INVALID_TARGET)
				source.Reply(_("\002%s\002 is not a registered unforbidden nick or channel."), nick.c_str());
			else if (result == MemoServ::MEMO_TOO_FAST)
			{
				auto lastmemosend = source.GetUser() ? source.GetUser()->lastmemosend : 0;
				auto waitperiod = (lastmemosend + Config->GetModule("memoserv").Get<unsigned long>("senddelay")) -  Anope::CurTime;
				source.Reply(_("Please wait %s before using the %s command again."), Anope::Duration(waitperiod, source.GetAccount()).c_str(), source.command.nobreak().c_str());
			}
			else if (result == MemoServ::MEMO_TARGET_FULL)
				source.Reply(_("%s currently has too many memos and cannot receive more."), nick.c_str());
			else
			{
				source.Reply(_("Memo sent to \002%s\002."), nick.c_str());

				bool ischan;
				MemoInfo *mi = MemoInfo::GetMemoInfo(nick, ischan);
				if (mi == NULL)
					throw CoreException("NULL mi in ms_rsend");
				Memo *m = (mi->memos->size() ? mi->GetMemo(mi->memos->size() - 1) : NULL);
				if (m != NULL)
					m->receipt = true;
			}
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Sends the named \037nick\037 or \037channel\037 a memo containing "
			"\037memo-text\037. When sending to a nickname, the recipient will "
			"receive a notice that they have a new memo. The target "
			"nickname/channel must be registered. "
			"Once the memo is read by its recipient, an automatic notification "
			"memo will be sent to the sender informing them that the memo "
			"has been read."
		));
		return true;
	}
};

class MSRSend final
	: public Module
{
	CommandMSRSend commandmsrsend;

public:
	MSRSend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandmsrsend(this)
	{
		if (!MemoServ::service)
			throw ModuleException("No MemoServ!");
	}
};

MODULE_INIT(MSRSend)
