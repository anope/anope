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
			if (ci)
				source.Reply(MEMO_LIST_CHAN_MEMOS, ci->name.c_str(), Config->s_MemoServ.c_str(), ci->name.c_str());
			else
				source.Reply(MEMO_LIST_MEMOS, source.u->nick.c_str(), Config->s_MemoServ.c_str());

			source.Reply(MEMO_LIST_HEADER);
		}

		DoList(source, mi, Number - 1);
	}

	static void DoList(CommandSource &source, const MemoInfo *mi, unsigned index)
	{
		Memo *m = mi->memos[index];
		source.Reply(MEMO_LIST_FORMAT, (m->HasFlag(MF_UNREAD)) ? '*' : ' ', index + 1, m->sender.c_str(), do_strftime(m->time).c_str());
	}
};

class CommandMSList : public Command
{
 public:
	CommandMSList() : Command("LIST", 0, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
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
				return MOD_CONT;
			}
			else if (!check_access(u, ci, CA_MEMO))
			{
				source.Reply(ACCESS_DENIED);
				return MOD_CONT;
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
						return MOD_CONT;
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
						if (ci)
							source.Reply(!param.empty() ? MEMO_LIST_CHAN_NEW_MEMOS : MEMO_LIST_CHAN_MEMOS, ci->name.c_str(), Config->s_MemoServ.c_str(), ci->name.c_str());
						else
							source.Reply(!param.empty() ? MEMO_LIST_NEW_MEMOS : MEMO_LIST_MEMOS, u->nick.c_str(), Config->s_MemoServ.c_str());
						source.Reply(MEMO_LIST_HEADER);
					}

					MemoListCallback::DoList(source, mi, i);
				}
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(MEMO_HELP_LIST);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "LIST", MEMO_LIST_SYNTAX);
	}

	void OnServCommand(CommandSource &source)
	{
		source.Reply(MEMO_HELP_CMD_LIST);
	}
};

class MSList : public Module
{
	CommandMSList commandmslist;

 public:
	MSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, &commandmslist);
	}
};

MODULE_INIT(MSList)
