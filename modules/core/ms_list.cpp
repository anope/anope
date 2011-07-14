/* MemoServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class MemoListCallback : public NumberList
{
	CommandSource &source;
	ChannelInfo *ci;
	const MemoInfo *mi;
	bool SentHeader;
 public:
	MemoListCallback(CommandSource &_source, ChannelInfo *_ci, const MemoInfo *_mi, const Anope::string &list) : NumberList(list, false), source(_source), ci(_ci), mi(_mi), SentHeader(false)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > mi->memos.size())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Memos for %s:"), ci ? ci->name.c_str() : source.u->nick.c_str());
			source.Reply(_(" Num  Sender            Date/Time"));
		}

		DoList(source, mi, Number - 1);
	}

	static void DoList(CommandSource &source, const MemoInfo *mi, unsigned index)
	{
		Memo *m = mi->memos[index];
		source.Reply(_("%c%3d  %-16s  %s"), (m->HasFlag(MF_UNREAD)) ? '*' : ' ', index + 1, m->sender.c_str(), do_strftime(m->time).c_str());
	}
};

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
		int i, end;

		if (!param.empty() && param[0] == '#')
		{
			chan = param;
			param = params.size() > 1 ? params[1] : "";

			if (!(ci = cs_findchan(chan)))
			{
				source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
				return;
			}
			else if (!check_access(u, ci, CA_MEMO))
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
			if (!param.empty() && isdigit(param[0]))
			{
				MemoListCallback list(source, ci, mi, param);
				list.Process();
			}
			else
			{
				if (!param.empty())
				{
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

				bool SentHeader = false;

				for (i = 0, end = mi->memos.size(); i < end; ++i)
				{
					if (!param.empty() && !mi->memos[i]->HasFlag(MF_UNREAD))
						continue;

					if (!SentHeader)
					{
						SentHeader = true;
						source.Reply(_("New memo for %s."), ci ? ci->name.c_str() : u->nick.c_str());
						source.Reply(_(" Num  Sender            Date/Time"));
					}

					MemoListCallback::DoList(source, mi, i);
				}
			}
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

		ModuleManager::RegisterService(&commandmslist);
	}
};

MODULE_INIT(MSList)
