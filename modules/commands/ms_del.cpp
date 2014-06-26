/* MemoServ core functions
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
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MEMO", ci->name);
				return;
			}

			mi = ci->memos;
		}
		else
			mi = source.nc->memos;

		if (numstr.empty() || (!isdigit(numstr[0]) && !numstr.equals_ci("ALL") && !numstr.equals_ci("LAST")))
		{
			this->OnSyntaxError(source, numstr);
			return;
		}

		if (!mi || mi->memos->empty())
		{
			if (!chan.empty())
				source.Reply(_("\002{0}\002 has no memos."), chan);
			else
				source.Reply(_("You have no memos."));
			return;
		}

		if (isdigit(numstr[0]))
		{
			NumberList(numstr, true,
				[&](unsigned int number)
				{
					if (!number || number > mi->memos->size())
						return;

					if (MemoServ::Event::OnMemoDel)
						MemoServ::Event::OnMemoDel(&MemoServ::Event::MemoDel::OnMemoDel, ci ? ci->name : source.nc->display, mi, mi->GetMemo(number - 1));

					mi->Del(number - 1);
					source.Reply(_("Memo \002{0}\002 has been deleted."), number);
				},
				[](){});
		}
		else if (numstr.equals_ci("LAST"))
		{
			/* Delete last memo. */
			if (MemoServ::Event::OnMemoDel)
				MemoServ::Event::OnMemoDel(&MemoServ::Event::MemoDel::OnMemoDel, ci ? ci->name : source.nc->display, mi, mi->GetMemo(mi->memos->size() - 1));
			mi->Del(mi->memos->size() - 1);
			source.Reply(_("Memo \002{0}\002 has been deleted."), mi->memos->size() + 1);
		}
		else
		{
			/* Delete all memos. */
			for (unsigned i = mi->memos->size(); i > 0; --i)
			{
				if (MemoServ::Event::OnMemoDel)
					MemoServ::Event::OnMemoDel(&MemoServ::Event::MemoDel::OnMemoDel, ci ? ci->name : source.nc->display, mi, mi->GetMemo(i));
				mi->Del(i - 1);
			}
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
