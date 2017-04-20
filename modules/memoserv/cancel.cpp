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

class CommandMSCancel : public Command
{
 public:
	CommandMSCancel(Module *creator) : Command(creator, "memoserv/cancel", 1, 1)
	{
		this->SetDesc(_("Cancel the last memo you sent"));
		this->SetSyntax(_("{\037user\037 | \037channel\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		const Anope::string &nname = params[0];

		bool ischan, isregistered;
		MemoServ::MemoInfo *mi = MemoServ::service->GetMemoInfo(nname, ischan, isregistered, false);

		if (!isregistered)
		{
			if (ischan)
				source.Reply(_("Channel \002{0}\002 isn't registered."), nname);
			else
				source.Reply(_("\002{0}\002 isn't registered."), nname);

			return;
		}

		if (mi == nullptr)
			return;

		ChanServ::Channel *ci = NULL;
		NickServ::Nick *na = NULL;
		if (ischan)
		{
			ci = ChanServ::Find(nname);
			if (ci == nullptr)
				return;
		}
		else
		{
			na = NickServ::FindNick(nname);
			if (na == nullptr)
				return;
		}

		auto memos = mi->GetMemos();
		for (int i = memos.size() - 1; i >= 0; --i)
		{
			MemoServ::Memo *m = memos[i];

			if (!m->GetUnread())
				continue;

			NickServ::Nick *sender = NickServ::FindNick(m->GetSender());

			if (sender && sender->GetAccount() == source.GetAccount())
			{
				EventManager::Get()->Dispatch(&MemoServ::Event::MemoDel::OnMemoDel, ischan ? ci->GetName() : na->GetAccount()->GetDisplay(), mi, m);
				mi->Del(i);
				source.Reply(_("Your last memo to \002{0}\002 has been cancelled."), ischan ? ci->GetName() : na->GetAccount()->GetDisplay());
				return;
			}
		}

		source.Reply(_("No memo was cancelable."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Cancels the last memo you sent to \037user\037, provided it has not yet been read."));
		return true;
	}
};

class MSCancel : public Module
{
	CommandMSCancel commandmscancel;

 public:
	MSCancel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmscancel(this)
	{
	}
};

MODULE_INIT(MSCancel)
