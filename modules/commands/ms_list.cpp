/* MemoServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandMSList : public Command
{
 public:
	CommandMSList(Module *creator) : Command(creator, "memoserv/list", 0, 2)
	{
		this->SetDesc(_("List your memos"));
		this->SetSyntax(_("[\037channel\037] [\037list\037 | NEW]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		Anope::string param = !params.empty() ? params[0] : "", chan;
		ChannelInfo *ci = NULL;
		const MemoInfo *mi;

		if (!param.empty() && param[0] == '#')
		{
			chan = param;
			param = params.size() > 1 ? params[1] : "";

			if (!(ci = cs_findchan(chan)))
			{
				source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
				return;
			}
			else if (!ci->AccessFor(u).HasPriv("MEMO"))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
			mi = &ci->memos;
		}
		else
			mi = &u->Account()->memos;

		if (!param.empty() && !isdigit(param[0]) && !param.equals_ci("NEW"))
			this->OnSyntaxError(source, param);
		else if (!mi->memos.size())
		{
			if (!chan.empty())
				source.Reply(MEMO_X_HAS_NO_MEMOS, chan.c_str());
			else
				source.Reply(MEMO_HAVE_NO_MEMOS);
		}
		else
		{
			ListFormatter list;

			list.addColumn("Number").addColumn("Sender").addColumn("Date/Time");

			if (!param.empty() && isdigit(param[0]))
			{
				class MemoListCallback : public NumberList
				{
					ListFormatter &list;
					CommandSource &source;
					const MemoInfo *mi;
				 public:
					MemoListCallback(ListFormatter &_list, CommandSource &_source, const MemoInfo *_mi, const Anope::string &numlist) : NumberList(numlist, false), list(_list), source(_source), mi(_mi)
					{
					}

					void HandleNumber(unsigned Number)
					{
						if (!Number || Number > mi->memos.size())
							return;

						Memo *m = mi->memos[Number];

						ListFormatter::ListEntry entry;
						entry["Number"] = (m->HasFlag(MF_UNREAD) ? "* " : "  ") + stringify(Number + 1);
						entry["Sender"] = m->sender;
						entry["Date/Time"] = do_strftime(m->time);
						this->list.addEntry(entry);
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
					for (i = 0, end = mi->memos.size(); i < end; ++i)
						if (mi->memos[i]->HasFlag(MF_UNREAD))
							break;
					if (i == end)
					{
						if (!chan.empty())
							source.Reply(MEMO_X_HAS_NO_NEW_MEMOS, chan.c_str());
						else
							source.Reply(MEMO_HAVE_NO_NEW_MEMOS);
						return;
					}
				}

				for (unsigned i = 0, end = mi->memos.size(); i < end; ++i)
				{
					if (!param.empty() && !mi->memos[i]->HasFlag(MF_UNREAD))
						continue;

					Memo *m = mi->memos[i];

					ListFormatter::ListEntry entry;
					entry["Number"] = (m->HasFlag(MF_UNREAD) ? "* " : "  ") + stringify(i + 1);
					entry["Sender"] = m->sender;
					entry["Date/Time"] = do_strftime(m->time);
					list.addEntry(entry);
				}
			}

			std::vector<Anope::string> replies;
			list.Process(replies);

			source.Reply(_("Memos for %s."), ci ? ci->name.c_str() : u->nick.c_str());
			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists any memos you currently have.  With \002NEW\002, lists only\n"
				"new (unread) memos. Unread memos are marked with a \"*\"\n"
				"to the left of the memo number. You can also specify a list\n"
				"of numbers, as in the example below:\n"
				"   \002LIST 2-5,7-9\002\n"
				"      Lists memos numbered 2 through 5 and 7 through 9."));
		return true;
	}
};

class MSList : public Module
{
	CommandMSList commandmslist;

 public:
	MSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandmslist(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(MSList)
