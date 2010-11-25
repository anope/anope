/* MemoServ core functions
 *
 * (C) 2003-2010 Anope Team
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
	MemoInfo *mi;
 public:
	MemoListCallback(CommandSource &_source, MemoInfo *_mi, const Anope::string &numlist) : NumberList(numlist, false), source(_source), mi(_mi)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > mi->memos.size())
			return;

		MemoListCallback::DoRead(source, mi, NULL, Number - 1);
	}

	static void DoRead(CommandSource &source, MemoInfo *mi, ChannelInfo *ci, unsigned index)
	{
		User *u = source.u;
		Memo *m = mi->memos[index];
		if (ci)
			u->SendMessage(MemoServ, MEMO_CHAN_HEADER, index + 1, m->sender.c_str(), do_strftime(m->time).c_str(), Config->s_MemoServ.c_str(), ci->name.c_str(), index + 1);
		else
			u->SendMessage(MemoServ, MEMO_HEADER, index + 1, m->sender.c_str(), do_strftime(m->time).c_str(), Config->s_MemoServ.c_str(), index + 1);
		u->SendMessage(MemoServ, MEMO_TEXT, m->text.c_str());
		m->UnsetFlag(MF_UNREAD);

		/* Check if a receipt notification was requested */
		if (m->HasFlag(MF_RECEIPT))
			rsend_notify(u, m, ci ? ci->name : "");
	}
};

class CommandMSRead : public Command
{
 public:
	CommandMSRead() : Command("READ", 0, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		MemoInfo *mi;
		ChannelInfo *ci = NULL;
		Anope::string numstr = !params.empty() ? params[0] : "", chan;
		int num;

		if (!numstr.empty() && numstr[0] == '#')
		{
			chan = numstr;
			numstr = params.size() > 1 ? params[1] : "";

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
		num = !numstr.empty() && numstr.is_number_only() ? convertTo<int>(numstr) : -1;
		if (numstr.empty() || (!numstr.equals_ci("LAST") && !numstr.equals_ci("NEW") && num <= 0))
			this->OnSyntaxError(u, numstr);
		else if (mi->memos.empty())
		{
			if (!chan.empty())
				source.Reply(MEMO_X_HAS_NO_MEMOS, chan.c_str());
			else
				source.Reply(MEMO_HAVE_NO_MEMOS);
		}
		else {
			int i, end;

			if (numstr.equals_ci("NEW"))
			{
				int readcount = 0;
				for (i = 0, end = mi->memos.size(); i < end; ++i)
					if (mi->memos[i]->HasFlag(MF_UNREAD))
					{
						MemoListCallback::DoRead(source, mi, ci, i);
						++readcount;
					}
				if (!readcount)
				{
					if (!chan.empty())
						source.Reply(MEMO_X_HAS_NO_NEW_MEMOS, chan.c_str());
					else
						source.Reply(MEMO_HAVE_NO_NEW_MEMOS);
				}
			}
			else if (numstr.equals_ci("LAST"))
			{
				for (i = 0, end = mi->memos.size() - 1; i < end; ++i);
				MemoListCallback::DoRead(source, mi, ci, i);
			}
			else /* number[s] */
			{
				MemoListCallback list(source, mi, numstr);
				list.Process();
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(MemoServ, MEMO_HELP_READ);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(MemoServ, u, "READ", MEMO_READ_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(MemoServ, MEMO_HELP_CMD_READ);
	}
};

class MSRead : public Module
{
	CommandMSRead commandmsread;

 public:
	MSRead(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, &commandmsread);
	}
};

MODULE_INIT(MSRead)
