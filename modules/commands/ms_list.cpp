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
		const MemoServ::MemoInfo *mi;

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
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MEMO", ci->name);
				return;
			}

			mi = ci->memos;
		}
		else
			mi = source.nc->memos;

		if (!param.empty() && !isdigit(param[0]) && !param.equals_ci("NEW"))
		{
			this->OnSyntaxError(source, param);
			return;
		}

		if (!mi->memos->size())
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
			class MemoListCallback : public NumberList
			{
				ListFormatter &list;
				CommandSource &source;
				const MemoServ::MemoInfo *mi;
			 public:
				MemoListCallback(ListFormatter &_list, CommandSource &_source, const MemoServ::MemoInfo *_mi, const Anope::string &numlist) : NumberList(numlist, false), list(_list), source(_source), mi(_mi)
				{
				}

				void HandleNumber(unsigned number) override
				{
					if (!number || number > mi->memos->size())
						return;

					const MemoServ::Memo *m = mi->GetMemo(number - 1);

					ListFormatter::ListEntry entry;
					entry["Number"] = (m->unread ? "* " : "  ") + stringify(number);
					entry["Sender"] = m->sender;
					entry["Date/Time"] = Anope::strftime(m->time, source.GetAccount());
					this->list.AddEntry(entry);
				}
			}
			mlc(list, source, mi, param);
			mlc.Process();
		}
		else
		{
			if (!param.empty())
			{
				unsigned i, end;
				for (i = 0, end = mi->memos->size(); i < end; ++i)
					if (mi->GetMemo(i)->unread)
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

			for (unsigned i = 0, end = mi->memos->size(); i < end; ++i)
			{
				if (!param.empty() && !mi->GetMemo(i)->unread)
					continue;

				const MemoServ::Memo *m = mi->GetMemo(i);

				ListFormatter::ListEntry entry;
				entry["Number"] = (m->unread ? "* " : "  ") + stringify(i + 1);
				entry["Sender"] = m->sender;
				entry["Date/Time"] = Anope::strftime(m->time, source.GetAccount());
				list.AddEntry(entry);
			}
		}

		std::vector<Anope::string> replies;
		list.Process(replies);

		source.Reply(_("Memos for \002{0}\002:"), ci ? ci->name.c_str() : source.GetNick().c_str());
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
