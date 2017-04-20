/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
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

class CommandMSRSend : public Command
{
 public:
	CommandMSRSend(Module *creator) : Command(creator, "memoserv/rsend", 2, 2)
	{
		this->SetDesc(_("Sends a memo and requests a read receipt"));
		this->SetSyntax(_("{\037nick\037 | \037channel\037} \037memo-text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
#warning "this is completely disabled"
#if 0
		if (!MemoServ::service)
			return;

		if (Anope::ReadOnly && !source.IsOper())
		{
			source.Reply(_("Sorry, memo sending is temporarily disabled."));
			return;
		}

		const Anope::string &nick = params[0];
		const Anope::string &text = params[1];
		const NickServ::Nick *na = NULL;

		/* prevent user from rsend to themselves */
		if ((na = NickServ::FindNick(nick)) && na->GetAccount() == source.GetAccount())
		{
			source.Reply(_("You can not request a receipt when sending a memo to yourself."));
			return;
		}

		if (Config->GetModule(this->GetOwner())->Get<bool>("operonly") && !source.IsServicesOper())
			source.Reply(_("Access denied. This command is for operators only."));
		else
		{
			MemoServ::MemoServService::MemoResult result = MemoServ::service->Send(source.GetNick(), nick, text);
			if (result == MemoServ::MemoServService::MEMO_INVALID_TARGET)
				source.Reply(_("\002{0}\002 isn't registered."), nick);
			else if (result == MemoServ::MemoServService::MEMO_TOO_FAST)
				source.Reply(_("Please wait \002{0}\002 seconds before using the \002{1}\002 command again."), Config->GetModule("memoserv/main")->Get<time_t>("senddelay"), source.command);
			else if (result == MemoServ::MemoServService::MEMO_TARGET_FULL)
				source.Reply(_("Sorry, \002{0}\002 currently has too many memos and cannot receive more."), nick);
			else
			{
				source.Reply(_("Memo sent to \002{0}\002."), nick);

				bool ischan, isregistered;
				MemoServ::MemoInfo *mi = MemoServ::service->GetMemoInfo(nick, ischan, isregistered, false);
				if (mi == NULL)
					throw CoreException("NULL mi in ms_rsend");
				MemoServ::Memo *m = (mi->memos->size() ? mi->GetMemo(mi->memos->size() - 1) : NULL);
				if (m != NULL)
					m->receipt = true;
			}
		}
#endif
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
#if 0
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sends the named \037nick\037 or \037channel\037 a memo containing\n"
				"\037memo-text\037. When sending to a nickname, the recipient will\n"
				"receive a notice that he/she has a new memo. The target\n"
				"nickname/channel must be registered.\n"
				"Once the memo is read by its recipient, an automatic notification\n"
				"memo will be sent to the sender informing him/her that the memo\n"
				"has been read."));
		return true;
#endif
	}
};

class MSRSend : public Module
{
	CommandMSRSend commandmsrsend;

 public:
	MSRSend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmsrsend(this)
	{
		if (!MemoServ::service)
			throw ModuleException("No MemoServ!");
		throw ModuleException("XXX");
	}
};

MODULE_INIT(MSRSend)
