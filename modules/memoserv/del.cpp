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

class CommandMSDel : public Command
{
 public:
	CommandMSDel(Module *creator) : Command(creator, "memoserv/del", 0, 2)
	{
		this->SetDesc(_("Delete a memo or memos"));
		this->SetSyntax(_("[\037channel\037] {\037num\037 | \037list\037 | LAST | ALL}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		MemoServ::MemoInfo *mi;
		ChanServ::Channel *ci = NULL;
		Anope::string numstr = !params.empty() ? params[0] : "", chan;

		if (!numstr.empty() && numstr[0] == '#')
		{
			chan = numstr;
			numstr = params.size() > 1 ? params[1] : "";

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

		if (numstr.empty() || (!isdigit(numstr[0]) && !numstr.equals_ci("ALL") && !numstr.equals_ci("LAST")))
		{
			this->OnSyntaxError(source, numstr);
			return;
		}

		if (!mi || mi->GetMemos().empty())
		{
			if (!chan.empty())
				source.Reply(_("\002{0}\002 has no memos."), chan);
			else
				source.Reply(_("You have no memos."));
			return;
		}

		auto memos = mi->GetMemos();

		if (isdigit(numstr[0]))
		{
			NumberList(numstr, true,
				[&](unsigned int number)
				{
					if (!number || number > memos.size())
						return;

					EventManager::Get()->Dispatch(&MemoServ::Event::MemoDel::OnMemoDel, ci ? ci->GetName() : source.nc->GetDisplay(), mi, mi->GetMemo(number - 1));

					mi->Del(number - 1);
					source.Reply(_("Memo \002{0}\002 has been deleted."), number);
				},
				[](){});
		}
		else if (numstr.equals_ci("LAST"))
		{
			/* Delete last memo. */
			EventManager::Get()->Dispatch(&MemoServ::Event::MemoDel::OnMemoDel, ci ? ci->GetName() : source.nc->GetDisplay(), mi, mi->GetMemo(memos.size() - 1));
			mi->Del(memos.size() - 1);
			source.Reply(_("Memo \002{0}\002 has been deleted."), memos.size() + 1);
		}
		else
		{
			/* Delete all memos. */
			std::for_each(memos.begin(), memos.end(),
			[&](MemoServ::Memo *m)
			{
				EventManager::Get()->Dispatch(&MemoServ::Event::MemoDel::OnMemoDel, ci ? ci->GetName() : source.nc->GetDisplay(), mi, m);
				delete m;
			});
			if (!chan.empty())
				source.Reply(_("All memos for channel \002{0}\002 have been deleted."), chan);
			else
				source.Reply(_("All of your memos have been deleted."));
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Deletes the specified memo or memos."
		               " You can supply multiple memo numbers or ranges of numbers instead of a single number, as in the second example below."
		               " If \002LAST\002 is given, the last memo will be deleted."
		               " If \002ALL\002 is given, deletes all of your memos.\n"
		               "\n"
		               "Examples:\n"
		               "\n"
		               "         {0} 1\n"
		               "         Deletes your first memo.\n"
		               "\n"
		               "         {0} 2-5,7-9\n"
		               "         Deletes memos numbered 2 through 5 and 7 through 9."),
		               source.command);
		return true;
	}
};

class MSDel : public Module
{
	CommandMSDel commandmsdel;

 public:
	MSDel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmsdel(this)
	{

	}
};

MODULE_INIT(MSDel)
