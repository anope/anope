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

class CommandMSList : public Command
{
 public:
	CommandMSList(Module *creator) : Command(creator, "memoserv/list", 0, 2)
	{
		this->SetDesc(_("List your memos"));
		this->SetSyntax(_("[\037channel\037] [\037list\037 | NEW]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{

		Anope::string param = !params.empty() ? params[0] : "", chan;
		ChanServ::Channel *ci = NULL;
		MemoServ::MemoInfo *mi;

		if (!param.empty() && param[0] == '#')
		{
			chan = param;
			param = params.size() > 1 ? params[1] : "";

			ci = ChanServ::Find(chan);
			if (!ci)
			{
				source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
				return;
			}

			if (!source.AccessFor(ci).HasPriv("MEMO"))
			{
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MEMO", ci->GetName());
				return;
			}

			mi = ci->GetMemos();
		}
		else
		{
			mi = source.nc->GetMemos();
		}

		if (!param.empty() && !isdigit(param[0]) && !param.equals_ci("NEW"))
		{
			this->OnSyntaxError(source, param);
			return;
		}

		if (!mi)
			return;

		auto memos = mi->GetMemos();

		if (!memos.size())
		{
			if (!chan.empty())
				source.Reply(_("\002{0}\002 has no memos."), chan);
			else
				source.Reply(_("You have no memos."));
			return;
		}

		ListFormatter list(source.GetAccount());

		list.AddColumn(_("Number")).AddColumn(_("Sender")).AddColumn(_("Date/Time"));

		if (!param.empty() && isdigit(param[0]))
		{
			NumberList(param, false,
				[&](unsigned int number)
				{
					if (!number || number > memos.size())
						return;

					MemoServ::Memo *m = mi->GetMemo(number - 1);

					ListFormatter::ListEntry entry;
					entry["Number"] = (m->GetUnread() ? "* " : "  ") + stringify(number);
					entry["Sender"] = m->GetSender();
					entry["Date/Time"] = Anope::strftime(m->GetTime(), source.GetAccount());
					list.AddEntry(entry);
				},
				[]{});
		}
		else
		{
			if (!param.empty())
			{
				unsigned i, end;
				for (i = 0, end = memos.size(); i < end; ++i)
					if (mi->GetMemo(i)->GetUnread())
						break;
				if (i == end)
				{
					if (!chan.empty())
						source.Reply(_("\002{0}\002 has no new memos."), chan);
					else
						source.Reply(_("You have no new memos."));
					return;
				}
			}

			for (unsigned i = 0, end = memos.size(); i < end; ++i)
			{
				if (!param.empty() && !mi->GetMemo(i)->GetUnread())
					continue;

				MemoServ::Memo *m = mi->GetMemo(i);

				ListFormatter::ListEntry entry;
				entry["Number"] = (m->GetUnread() ? "* " : "  ") + stringify(i + 1);
				entry["Sender"] = m->GetSender();
				entry["Date/Time"] = Anope::strftime(m->GetTime(), source.GetAccount());
				list.AddEntry(entry);
			}
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		source.Reply(_("Memos for \002{0}\002:"), ci ? ci->GetName().c_str() : source.GetNick().c_str());
		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Lists the memos you currently have."
		               " With \002NEW\002, lists only new (unread) memos."
		               " Unread memos are marked with a \"*\" to the left of the memo number."
		               " You can also specify a list of numbers, as in the example below:\n"
		               "\n"
		               "Example:\n"
		               "\n"
		               "         {0} 2-5,7-9\n"
		               "         Lists memos numbered 2 through 5 and 7 through 9."));
		return true;
	}
};

class MSList : public Module
{
	CommandMSList commandmslist;

 public:
	MSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmslist(this)
	{

	}
};

MODULE_INIT(MSList)
